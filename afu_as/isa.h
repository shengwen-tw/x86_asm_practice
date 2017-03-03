#ifndef __ISA_H__
#define __ISA_H__

#define INSTRUCTION_LEN 10
#define MAX_CHAR_LINE 64
#define MAX_ARGS 5

#define DEF_INSTRUCTION(name) [_ ## name] = \
	{._name = #name, .func = name ## _ ## handler}

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

typedef struct {
	char _name[INSTRUCTION_LEN];
	int (*func)(char *arguments);
} instruction_list_t;

typedef struct {
	int mode; //Direct value, register, etc...
	int value;
} instruction_arg_t;

#endif
