#include <stdio.h>
#include <string.h>

#include "isa.h"

int generate_binary(char *source_code, char *binary_code);
int parse_instruction(char *line_start, char *line_end, char *binary_code);

/* Instruction handlers */
int add_handler(char *args);
int dec_handler(char *args);
int mov_handler(char *args);
int push_handler(char *args);
int pop_handler(char *args);
int int_handler(char *args);

/* Define supported instruction for this assembler */
instruction_list_t instruction_list[INSTRUCTION_CNT] = {
	DEF_INSTRUCTION(add),
	DEF_INSTRUCTION(dec),
	DEF_INSTRUCTION(mov),
	DEF_INSTRUCTION(push),
	DEF_INSTRUCTION(int)	
};

int main(int argc, char **argv)
{
	char src_memory_pool[1024] = {'\0'};
	char bin_memory_pool[1024] = {'\0'};

	if(argc != 3) {
		printf("afu as: x86 assembler\n"
			"=============================\n"
			"Usage: ./afu_as source binary\n"
			"Support instructions:\n"
			"add, dec, mov, push, pop, int\n");
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
char *split_token(char *token, char *instruction, int *size)
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

/* Parse a line of assembly code and append its machine code to the output file,
   the function returns the size of machine code  */
int parse_instruction(char *line_start, char *line_end, char *binary_code)
{
	char *line_ptr = line_start;
	char first_token[64] = {'\0'};
	int line_size = line_end - line_start;

	line_ptr = split_token(first_token, line_start, &line_size);

	/* Identify the instruction type and call its handler */
	int i = 0;
	for(i = 0; i < INSTRUCTION_CNT; i++) {
		if(strcmp(instruction_list[i]._name, first_token) == 0) {
			return instruction_list[i].func(line_ptr);
		}
	}

	if(first_token[0] == '#') {
		return 0; //Ignore this line, it is a comment
	}

	printf("afu_as: error: invaild instruction \"%s\"\n", first_token);

	return -1;
}

int add_handler(char *args)
{
	printf("add:%s\n", args);
}

int dec_handler(char *args)
{
	printf("dec%s\n", args);
}

int mov_handler(char *args)
{
	printf("mov%s\n", args);
}

int push_handler(char *args)
{
	printf("push%s\n", args);
}

int pop_handler(char *args)
{
	printf("pop%s\n", args);
}

int int_handler(char *args)
{
	printf("int%s\n", args);
}
