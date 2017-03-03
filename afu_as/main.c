#include <stdio.h>
#include <string.h>

#include "isa.h"

int generate_binary(char *source_code, char *binary_code);
int parse_instruction(char *line_start, char *line_end, char *binary_code);

/* Instruction handlers */
int add_handler(instruction_arg_t *args, int arg_cnt);
int dec_handler(instruction_arg_t *args, int arg_cnt);
int mov_handler(instruction_arg_t *args, int arg_cnt);
int push_handler(instruction_arg_t *args, int arg_cnt);
int pop_handler(instruction_arg_t *args, int arg_cnt);
int int_handler(instruction_arg_t *args, int arg_cnt);

/* Define supported instruction for this assembler */
instruction_list_t instruction_list[INSTRUCTION_CNT] = {
	DEF_INSTRUCTION(add),
	DEF_INSTRUCTION(dec),
	DEF_INSTRUCTION(mov),
	DEF_INSTRUCTION(push),
	DEF_INSTRUCTION(int)	
};

/* Define supported register */
char *register_list[] = {
	DEF_REGISTER(ah),
	DEF_REGISTER(al),
	DEF_REGISTER(ax),

	DEF_REGISTER(bh),
	DEF_REGISTER(bl),
	DEF_REGISTER(bx),

	DEF_REGISTER(ch),
	DEF_REGISTER(cl),
	DEF_REGISTER(cx),

	DEF_REGISTER(dh),
	DEF_REGISTER(dl),
	DEF_REGISTER(dx),

	DEF_REGISTER(bp),
	DEF_REGISTER(si),
	DEF_REGISTER(di),
	DEF_REGISTER(sp)
};

int main(int argc, char **argv)
{
	char src_memory_pool[1024] = {'\0'};
	char bin_memory_pool[1024] = {'\0'};

	if(argc != 3) {
		printf("afu as: simple 16-bit x86 assembler\n"
			"===================================\n"
			"Usage: ./afu_as source binary\n"
			"===================================\n"
			"Support instructions:\n"
			"add, dec, mov, push, pop, int\n"
			"Support registers:\n"
			"ax, bx, cx, dx, bp, si, di, sp\n"
		);
		return 0;
	}

	FILE *source = fopen(argv[1], "r");
	if(source == NULL) {
		printf("afu_as: error: no such file\n");
		return 0;
	}

	fseek(source, 0, SEEK_END); //Seek to end and get char size
	long size = ftell(source);
	fseek(source, 0L, SEEK_SET); //Seek back to begin

	//Read assembly language source code
	fread(src_memory_pool, sizeof(char), size, source);

	if(generate_binary(src_memory_pool, bin_memory_pool) == 0) {
		//Succeed to generate the binary file
		FILE *binary = fopen(argv[2], "w");
		//TODO: fwrite
		fclose(binary);
	}

	fclose(source);

	return 0;
}

int generate_binary(char *source_code, char *binary_code)
{
	char *binary_ptr = binary_code;
	char *line_start = source_code;
	char *line_end = source_code;

	while(1) {
		/* Search the end of code line */
		char *line_end = strchr(line_start, '\n');

		/* According to POSIX standard, the last byte before EOF
		   should be \n indicate it is not a binary file */
		if(line_end == NULL) {
			//Reach the end of the source code, leave
			break;
		}

		*line_end = '\0';

		/* Parse instruction and generate machine code */
		int write_offset = parse_instruction(line_start, line_end, binary_ptr);
		if(write_offset == -1) {
			//Error occured, leave
			return 1;
		}

		binary_ptr += write_offset;
		line_start = line_end + 1;
	}

	return 0;
}

/* Split a token of the instruction and return the address of next token */
static char *split_token(char *token, char *instruction, int *size)
{
	int i = 0;

	/* Ignore spaces before first token */
	while(instruction[i] == ' ') {
		i++;
	}

	instruction += i;

	/* Start to split one token */
	for(i = 0; i < *size; i++) {
		if(instruction[i] == ' ') {
			strncpy(token, instruction, i);
			token[i + 1] = '\0';

			//Read all the space until next token
			while(instruction[i + 1] == ' ' && i < *size) {
				i++;
			}

			*size -= i + 1;

			return instruction + i + 1;
		}
	}

	//There is only one token in this instruction
	strncpy(token, instruction, i);
	token[i + 1] = '\0';
}

static void remove_comment_and_space(char *old_str, char *new_str)
{
	int new_str_len = 0;

	int i;
	for(i = 0; i < strlen(old_str); i++) {
		//Ignore space
		if(old_str[i] != ' ') {
			//Ignore comment and return the new string
			if(old_str[i] == '#') {
				new_str[new_str_len] = '\0';
				return;
			} else {
				new_str[new_str_len] = old_str[i];
				new_str_len++;
			}
		}
	}

	new_str[new_str_len] = '\0';
}

static int split_arguments(char *str, char (*args)[MAX_CHAR_LINE], int *arg_cnt)
{
	char new_str[MAX_CHAR_LINE];
	remove_comment_and_space(str, new_str);

	size_t str_len = strlen(new_str);

	if(str_len == 0) {
		*arg_cnt = 0;
		return 0;
	}

	*arg_cnt = 0;
	int cur_i = 0; //Current argument's index

	int i;
	for(i = 0; i < str_len; i++) {
		if(new_str[i] == ',') {
			args[*arg_cnt][cur_i] = '\0';
			(*arg_cnt)++;
			cur_i = 0;
		} else {
			args[*arg_cnt][cur_i] = new_str[i];
			cur_i++;
		}
	}

	args[*arg_cnt][cur_i] = '\0'; //Padding end null symbol  for last arument

	(*arg_cnt)++;
}

static int parse_arguments_str(char (*args_in_str)[MAX_CHAR_LINE],
	instruction_arg_t *args, int arg_cnt)
{
	int i;
	for(i = 0; i < arg_cnt; i++) {
		if(args_in_str[i][0] == '$') {
			//Direct value
			args[i].type = DIRECT_VALUE;

			int decimal_number;

			/* Parse value */
			if(args_in_str[i][1] == '\'' & args_in_str[i][3] == '\'') {
				//Is char
				args[i].value = (int)args_in_str[i][2];
			} else if(sscanf(args_in_str[i] + 1, "%x", &decimal_number) != EOF) {
				//Hexadecimal number
				args[i].value = decimal_number;
			} else if(sscanf(args_in_str[i] + 1, "%d", &decimal_number)) {
				//Decimal number
				args[i].value = decimal_number;
			} else if(sscanf(args_in_str[i] + 1, "%o", &decimal_number)) {
				//Octal number
				args[i].value = decimal_number;
			} else {
				//Unknown type value
				printf("afu_as: error: invalid number\n");
				return 1;
			}
		} else if(args_in_str[i][0] == '%') {
			//Register
			args[i].type = REGISTER;

			int match = 0;

			/* Parse register name */
			int j;
			for(j = 0; j < REG_CNT; j++) {
				if(strcmp(register_list[j], args_in_str[i] + 1) == 0) {
					args[i].value = j;
					match = 1;
				}
			}

			/* Unknown register name */
			if(match == 0) {
				printf("afu_as: error: unknown register name\n");
				return 1;
			}
		} else {
			//Unknown
			printf("afu_as: error: unknown instruction argument\n");

			return 1;
		}
	}

	return 0;
}

#if USE_DEBUG_PRINT
static void instruction_debug_print(char *name, char (*args)[MAX_CHAR_LINE],
	instruction_arg_t *instruction_args, int arg_cnt)
{
	printf("%s", name);
	int i;
	for(i = 0; i < arg_cnt; i++) {
		printf("(%s)", args[i]);
	}

	printf(" -> ");

	if(arg_cnt == 0) {
		printf(" no argument");
	}

	for(i = 0; i < arg_cnt; i++) {
		printf("(type:%d, value:%d)",
			instruction_args[i].type, instruction_args[i].value);
	}

	printf("\n");
}
#endif

/* Parse a line of assembly code and append its machine code to the output file,
   the function returns the size of machine code  */
int parse_instruction(char *line_start, char *line_end, char *binary_code)
{
	char *line_ptr = line_start;
	char first_token[MAX_CHAR_LINE] = {'\0'};
	int line_size = line_end - line_start;

	line_ptr = split_token(first_token, line_start, &line_size);

	int arg_cnt = 0;
	char splited_args[MAX_CHAR_LINE][MAX_CHAR_LINE];
	instruction_arg_t instruction_args[MAX_ARGS];

	/* Identify the instruction type and call its handler */
	int i = 0;
	for(i = 0; i < INSTRUCTION_CNT; i++) {
		if(strcmp(instruction_list[i]._name, first_token) == 0) {
			//Split and parse arguments
			split_arguments(line_ptr, splited_args, &arg_cnt);
			if(parse_arguments_str(splited_args, instruction_args, arg_cnt)) {
				return -1; //Failed to parse to argument
			}

			//Debug print
			instruction_debug_print(first_token, splited_args, instruction_args, arg_cnt);

			//Call instruction handler and pass the argument
			return instruction_list[i].func(instruction_args, arg_cnt);
		}
	}

	if(first_token[0] == '#') {
		return 0; //Ignore this line, it is a comment
	}

	printf("afu_as: error: invaild instruction \"%s\"\n", first_token);

	return -1;
}

int add_handler(instruction_arg_t *args, int arg_cnt)
{
}

int dec_handler(instruction_arg_t *args, int arg_cnt)
{
}

int mov_handler(instruction_arg_t *args, int arg_cnt)
{
	if(arg_cnt > 2) {
		printf("afu_as: error: too many argument for \"mov\" instruction\n");
		return -1;
	} else if (arg_cnt < 2) {
		printf("afu_as: error: too few argument for \"mov\" instruction\n");
		return -1;
	}
}

int push_handler(instruction_arg_t *args, int arg_cnt)
{
}

int pop_handler(instruction_arg_t *args, int arg_cnt)
{
}

int int_handler(instruction_arg_t *args, int arg_cnt)
{
	if(arg_cnt > 1) {
		printf("afu_as: error: too many argument for \"int\" instruction\n");
		return -1;
	} else if (arg_cnt < 1) {
		printf("afu_as: error: too few argument for \"int\" instruction\n");
		return -1;
	}
}
