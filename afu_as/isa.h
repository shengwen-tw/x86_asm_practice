#ifndef __ISA_H__
#define __ISA_H__

#define INSTRUCTION_LEN 10

#define DEF_INSTRUCTION(name) [name] = \
	{._name = #name, .func = name ## _ ## handler}

enum {
	ADD,
	DEC,
	MOV,
	PUSH,
	POP,
	INT,
	INSTRUCTION_CNT	
} SUPPORT_INSTRUCTIONS;

typedef struct {
	char _name[INSTRUCTION_LEN];
	int (*func)(char *arguments);
} instruction_list_t;

#endif
