build:
	nasm -f elf32 kernel/loader.s
	ld.lld -m elf_i386 -T kernel/link.ld kernel/loader.o -o kernel/kernel.elf

	cp kernel/kernel.elf iso/boot/kernel.elf
	i686-elf-grub-mkrescue -o neoos.iso iso

run:
	qemu-system-i386 -cdrom neoos.iso

clean:
	rm -f *.iso iso/boot/kernel.elf
	rm -f kernel/*.o kernel/kernel.elf