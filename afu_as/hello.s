#Use BIOS interrupt print hello world
mov $'H' , %al  #char to print
mov $0x0e, %ah  #tty mode
int $0x10       #call interrupt

mov $'e' , %al
mov $0x0e, %ah
int $0x10

mov $'l' , %al
mov $0x0e, %ah
int $0x10

mov $'l' , %al
mov $0x0e, %ah
int  $0x10

mov $'o' , %al
mov $0x0e, %ah
int $0x10

sub $0xffff, %ax
sub $0x12, %al
