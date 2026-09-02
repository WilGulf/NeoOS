#Files to be output of compilation
OBJECTS = kernel/loader.o kernel/kmain.o kernel/kernel_asm.o \
	kernel/drivers/io/fb_io.o kernel/drivers/io/io_asm.o kernel/drivers/io/kprint.o \
	kernel/drivers/keyboard/keyboard.o kernel/drivers/keyboard/ps2.o \
	kernel/gdt/gdt_asm.o kernel/gdt/gdt.o \
	kernel/idt/idt.o kernel/idt/idt_asm.o kernel/timer/timer.o \
	kernel/include/util.o \
	kernel/memory/paging.o kernel/memory/paging_asm.o kernel/memory/heap.o kernel/memory/kheap.o\
	kernel/drivers/fs/disk.o kernel/drivers/fs/path_parser.o kernel/drivers/fs/disk_streamer.o kernel/drivers/fs/file.o \
	kernel/drivers/fs/fat/fat16.o kernel/drivers/fs/ramfs/ramfs.o \
	kernel/panic.o \
	kernel/task/task.o kernel/task/task_asm.o kernel/task/process.o kernel/task/formats/elf.o kernel/task/formats/elf_loader.o \
	kernel/syscalls/isr80h.o \
	kernel/syscalls/io.o \
	kernel/syscalls/heap.o \
	kernel/syscalls/process.o \
	kernel/syscalls/promise.o \
	kernel/syscalls/privilege.o \
	kernel/syscalls/kernel.o \
	kernel/syscalls/fs.o \
	kernel/syscalls/timer.o

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

all: mkdir kernel userland_execs disk_contents

kernel: kernel.elf

mkdir:
	mkdir -p output

kernel.elf: $(OBJECTS)
	ld.lld $(LDFLAGS) $(OBJECTS) -o output/kernel.elf

disk_contents: output/disk.img
	mdir -i output/disk.img ::/execs >/dev/null 2>&1 || mmd -i output/disk.img ::/execs
	mdir -i output/disk.img ::/sysro >/dev/null 2>&1 || mmd -i output/disk.img ::/sysro
	mdir -i output/disk.img ::/data >/dev/null 2>&1 || mmd -i output/disk.img ::/data

	mcopy -o -i output/disk.img -o userland/configs/boot.cfg ::/sysro/boot.cfg

	mcopy -o -i output/disk.img -o userland/launch/output/launch.elf ::/sysro/launch.elf
	mcopy -o -i output/disk.img -o userland/execs/sh/output/sh.elf ::/execs/sh.elf
	mcopy -o -i output/disk.img -o userland/execs/fetch/output/fetch.elf ::/execs/fetch.elf
	mcopy -o -i output/disk.img -o userland/execs/echo/output/echo.elf ::/execs/echo.elf
	mcopy -o -i output/disk.img -o userland/execs/sysinfo/output/sysinfo.elf ::/execs/sysinfo.elf
	mcopy -o -i output/disk.img -o userland/execs/read/output/read.elf ::/execs/read.elf
	mcopy -o -i output/disk.img -o userland/execs/running/output/running.elf ::/execs/running.elf
	mcopy -o -i output/disk.img -o userland/execs/kill/output/kill.elf ::/execs/kill.elf
	mcopy -o -i output/disk.img -o userland/execs/kill/output/killall.elf ::/execs/killall.elf
	mcopy -o -i output/disk.img -o userland/execs/test/output/test.elf ::/execs/test.elf

run: all
	qemu-system-i386 -kernel output/kernel.elf -hda output/disk.img

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

output/disk.img:
	dd if=/dev/zero of=output/disk.img bs=1M count=64
	mkfs.fat -F 16 output/disk.img

userland_execs:
	cd ./userland/libs/stdlib && $(MAKE) all
	cd ./userland/libs/curses && $(MAKE) all
	cd ./userland/launch && $(MAKE) all
	cd ./userland/execs/sh && $(MAKE) all
	cd ./userland/execs/fetch && $(MAKE) all
	cd ./userland/execs/echo && $(MAKE) all
	cd ./userland/execs/sysinfo && $(MAKE) all
	cd ./userland/execs/read && $(MAKE) all
	cd ./userland/execs/running && $(MAKE) all
	cd ./userland/execs/kill && $(MAKE) all
	cd ./userland/execs/test && $(MAKE) all

userland_clean:
	cd ./userland/libs/stdlib && $(MAKE) clean 
	cd ./userland/libs/curses && $(MAKE) clean
	cd ./userland/launch && $(MAKE) clean
	cd ./userland/execs/sh && $(MAKE) clean
	cd ./userland/execs/fetch && $(MAKE) clean
	cd ./userland/execs/echo && $(MAKE) clean
	cd ./userland/execs/sysinfo && $(MAKE) clean
	cd ./userland/execs/read && $(MAKE) clean
	cd ./userland/execs/running && $(MAKE) clean
	cd ./userland/execs/kill && $(MAKE) clean
	cd ./userland/execs/test && $(MAKE) clean

kernel_clean:
	rm -f output/*.elf
	rm -f output/*.img
	rm -f kernel/*/*/*.o
	rm -f kernel/*/*/*/*.o
	rm -f kernel/*/*.o
	rm -f kernel/*.o

clean: kernel_clean userland_clean
