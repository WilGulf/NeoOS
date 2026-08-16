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
	kernel/task/task.o kernel/task/task_asm.o kernel/task/process.o kernel/task/formats/elf.o kernel/task/formats/elf_loader.o \
	kernel/syscalls/isr80h.o \
	kernel/syscalls/misc.o \
	kernel/syscalls/io.o \
	kernel/syscalls/heap.o \
	kernel/syscalls/process.o

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

all: mkdir kernel output/disk.img userland_bins

kernel: kernel.elf

mkdir:
	mkdir -p output

kernel.elf: $(OBJECTS)
	ld.lld $(LDFLAGS) $(OBJECTS) -o output/kernel.elf

run: userland_bins kernel.elf
	mcopy -o -i output/disk.img -o userland/bin/sh/output/sh.elf ::sh.elf
	mcopy -o -i output/disk.img -o userland/bin/fetch/output/fetch.elf ::fetch.elf
	mcopy -o -i output/disk.img -o userland/bin/echo/output/echo.elf ::echo.elf
	qemu-system-i386 -kernel output/kernel.elf -hda output/disk.img

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

output/disk.img:
	dd if=/dev/zero of=output/disk.img bs=1M count=64
	mkfs.fat -F 16 output/disk.img

userland_bins:
	cd ./userland/lib/stdlib && $(MAKE) all
	cd ./userland/lib/stdio && $(MAKE) all 
	cd ./userland/bin/sh && $(MAKE) all
	cd ./userland/bin/fetch && $(MAKE) all
	cd ./userland/bin/echo && $(MAKE) all

userland_clean:
	cd ./userland/lib/stdlib && $(MAKE) clean 
	cd ./userland/lib/stdio && $(MAKE) clean 
	cd ./userland/bin/sh && $(MAKE) clean
	cd ./userland/bin/fetch && $(MAKE) clean
	cd ./userland/bin/echo && $(MAKE) clean

kernel_clean:
	rm -f output/*.elf
	rm -f output/*.iso
	rm -f kernel/*/*/*.o
	rm -f kernel/*/*/*/*.o
	rm -f kernel/*/*.o
	rm -f kernel/*.o output/kernel.elf iso/boot/kernel.elf

clean: kernel_clean userland_clean
