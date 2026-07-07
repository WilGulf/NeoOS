#Files to be output of compilation
OBJECTS = kernel/loader.o kernel/kmain.o \
	kernel/drivers/io/io.o kernel/drivers/io/io_asm.o kernel/drivers/io/kprint.o \
	kernel/gdt/gdt_asm.o kernel/gdt/gdt.o \
	kernel/idt/idt.o kernel/idt/idt_asm.o \
	kernel/include/util.o \
	kernel/memory/memory.o
#C compiler
CC = i686-elf-gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
	-nostartfiles -nodefaultlibs \
	-Wall -Wextra -c

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
	rm -f kernel/drivers/io/*.o
	rm -f kernel/gdt/*.o
	rm -f kernel/idt/*.o
	rm -f kernel/include/*.o
	rm -f kernel/memory/*.o
	rm -f kernel/*.o output/kernel.elf iso/boot/kernel.elf