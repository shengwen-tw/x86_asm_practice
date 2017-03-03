ELF=afu_as
IMAGE=floppy.img
TEST=test

all:
	gcc -g -o afu_as main.c

clean:
	rm ${ELF} ${TEST}.bin

qemu:
	qemu-system-i386 -fda ${IMAGE} -boot a

objdump:
	objdump -D -Mintel,i8086 -b binary -m i386 ${BIN}.bin

gdbauto:
	cgdb --args ${ELF} test.s test.bin

test:
	./afu_as ${TEST}.s ${TEST}.bin
