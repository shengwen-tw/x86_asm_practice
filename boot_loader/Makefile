IMAGE=floppy.img
BIN=boot_loader

all:
	as ${BIN}.s -o ${BIN}.o
	ld -Ttext 0x7c00 --oformat=binary ${BIN}.o -o ${BIN}.bin
	dd if=/dev/zero of=${IMAGE} bs=512 count=2880
	dd if=${BIN}.bin of=${IMAGE}

clean:
	rm ${IMAGE} ${BIN}.o ${BIN}.bin

qemu:
	qemu-system-i386 -fda ${IMAGE} -boot a
