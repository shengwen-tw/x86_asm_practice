#ifndef __ISA_H__
#define __ISA_H__

#define INSTRUCTION_LEN 10
#define MAX_CHAR_LINE 64
#define MAX_ARGS 5

#define DEF_INSTRUCTION(name) [_ ## name] = \
	{._name = #name, .func = name ## _ ## handler}

#define DEF_REGISTER(reg_name) #reg_name

#define USE_DEBUG_PRINT 1
#if !USE_DEBUG_PRINT
	#define instruction_debug_print(...)
#endif

enum {
	_add,
	_dec,
	_mov,
	_push,
	_pop,
	_int,
	INSTRUCTION_CNT	
} SUPPORT_INSTRUCTIONS;

enum {
	DIRECT_VALUE,
	REGISTER
} argument_type;

enum {
	ah,
	al,
	ax,
	bh,
	bl,
	bx,
	ch,
	cl,
	cx,
	dh,
	dl,
	dx,
	bp,
	si,
	di,
	sp,
	REG_CNT
} SUPPORT_REGISTER;

enum {
	INT_IMM8 = 0xcd
} OPCODE;

typedef struct {
	int type; //Direct value, register, etc...
	int value;
} instruction_arg_t;

typedef struct {
	char _name[INSTRUCTION_LEN];
	int (*func)(instruction_arg_t *args, int arg_cnt, char *machine_code);
} instruction_list_t;

#endif
