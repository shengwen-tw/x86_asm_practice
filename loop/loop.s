.global main
main:
	mov $10, %ecx

	loop:
		mov %ecx, %esi  #Second argument

		push %rcx #rcx will not be preserve after calling the function!

		mov $format, %rdi #First argument, format string address
		xor %eax, %eax   #Clear zero for no variable arguments (Check x86_64 ABI)
		call printf

		pop %rcx

		dec %ecx
		jnz loop #Jump if not zero

	ret

.rodata:
format:
	.asciz "loop counter: %d\n"
