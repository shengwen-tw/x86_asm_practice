movb $'H' , %al  #Char to print
movb $0x0e, %ah  #TTY mode for 10h interrupt
int  $0x10       #Print interrupt
