#Files to be output of compilation
OBJECTS = kernel/loader.o kernel/kmain.o kernel/kernel_asm.o \
	kernel/drivers/io/fb_io.o kernel/drivers/io/io_asm.o kernel/drivers/io/kprint.o \
	kernel/drivers/keyboard/keyboard.o kernel/drivers/keyboard/ps2.o \
	kernel/gdt/gdt_asm.o kernel/gdt/gdt.o \
	kernel/idt/idt.o kernel/idt/idt_asm.o \
	kernel/include/util.o \
	kernel/memory/paging.o kernel/memory/paging_asm.o kernel/memory/heap.o kernel/memory/kheap.o\
	kernel/drivers/fs/disk.o kernel/drivers/fs/path_parser.o kernel/drivers/fs/disk_streamer.o kernel/drivers/fs/file.o \
	kernel/drivers/fs/fat/fat16.o \
	kernel/panic.o \
	kernel/syscalls/isr80h.o kernel/syscalls/misc.o \
	kernel/task/task.o kernel/task/task_asm.o kernel/task/process.o \
	kernel/syscalls/io.o

#C compiler
CC = i686-elf-gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
	-nostartfiles -nodefaultlibs \
	-Wall -Wextra -Wno-int-conversion -c

#Linker
LDFLAGS = -m elf_i386 -T kernel/link.ld

#Assembly compiler
AS = nasm
ASFLAGS = -f elf32

all: kernel userland_bins

kernel: kernel.elf

kernel.elf: $(OBJECTS)
	ld.lld $(LDFLAGS) $(OBJECTS) -o output/kernel.elf

os.iso: kernel.elf
	cp output/kernel.elf iso/boot/kernel.elf
	i686-elf-grub-mkrescue -o neoos.iso iso
	mv neoos.iso output/neoos.iso

run: os.iso userland_bins
	mcopy -o -i output/disk.img -o userland/bin/print/output/print.bin ::print.bin
	qemu-system-i386 -kernel output/kernel.elf -hda output/disk.img

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

userland_bins:
	cd ./userland/bin/print && $(MAKE) all  	

userland_clean:
	cd ./userland/bin/print && $(MAKE) clean

kernel_clean:
	rm -f output/*.elf
	rm -f output/*.iso
	rm -f kernel/*/*/*.o
	rm -f kernel/*/*/*/*.o
	rm -f kernel/*/*.o
	rm -f kernel/*.o output/kernel.elf iso/boot/kernel.elf

clean: kernel_clean userland_clean
