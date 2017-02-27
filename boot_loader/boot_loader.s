.code16 #Generate 16-bit code

.text
.global _start;
_start:
	#Use BIOS interrupt to print on screen
	movb $'H' , %al  #Char to print
	movb $0x0e, %ah  #TTY mode for 10h interrupt
	int  $0x10	 #Print interrupt

	movb $'e' , %al
	movb $0x0e, %ah
	int  $0x10

	movb $'l' , %al
	movb $0x0e, %ah
	int  $0x10

	movb $'l' , %al
	movb $0x0e, %ah
	int  $0x10

	movb $'o' , %al
	movb $0x0e, %ah
	int  $0x10

	#Add boot signature
	. = _start + 510      #mov to 510th byte from 0 pos
	.byte 0x55
	.byte 0xaa
