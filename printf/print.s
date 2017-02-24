add:
	mov %edi, %eax
	add %esi, %eax
	ret

.global main
main:
	mov $1, %edi
	mov $3, %esi

	call add

	mov %esi, %edx  #Third argument
	mov %edi, %esi  #Second argument

	mov $format, %rdi #First argument, format string address

	mov  %eax, %ecx #Forth argument
	xor %eax, %eax   #Clear zero for no variable arguments (Check x86_64 ABI)
	call printf

	ret

.rodata:
format:
	.asciz "add(%d, %d) = %d\n"
