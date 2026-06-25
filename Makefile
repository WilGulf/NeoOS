build:
	nasm -f elf32 loader.s
	ld.lld -m elf_i386 -T link.ld loader.o -o kernel.elf

	cp kernel.elf iso/boot/kernel.elf
	i686-elf-grub-mkrescue -o neoos.iso iso

run:
	qemu-system-i386 -cdrom neoos.iso

clean:
	rm -f *.o kernel.elf *.iso iso/boot/kernel.elf