OBJECTS = kernel/loader.o kernel/kmain.o kernel/drivers/io/io.o

#C compiler
CC = i686-elf-gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
	-nostartfiles -nodefaultlibs \
	-Wall -Wextra -Werror -c

#Linker
LDFLAGS = -m elf_i386 -T kernel/link.ld

#Assembly compiler
AS = nasm
ASFLAGS = -f elf32

all: kernel.elf

kernel.elf: $(OBJECTS)
	ld.lld $(LDFLAGS) $(OBJECTS) -o output/kernel.elf

os.iso: kernel.elf
	cp output/kernel.elf iso/boot/kernel.elf
	i686-elf-grub-mkrescue -o neoos.iso iso
	mv neoos.iso output/neoos.iso

run: os.iso
	qemu-system-i386 -cdrom output/neoos.iso

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f output/*
	rm -f kernel/*.o output/kernel.elf iso/boot/kernel.elf