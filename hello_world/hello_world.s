.section .rodata
string:
	.asciz "Hello World\n"

.section .text
.global _start
_start:
	#Use syscall "write"(1) to print Hello World
	mov $1, %rax      #rax for syscall number
	mov $1, %rdi      #move 1 (stdout) into rdi (first argument by convention)
	mov $string, %rsi #move string address into rsi (second argument)
	mov $13, %rdx     #Move the string size into rdx (third argument)
	syscall

	#Use syscall "exit"(60) to leave
	mov $60, %rax
	xor %rdi, %rdi     #Clear zero
	syscall
	
