OBJECTS := kernel/loader.o kernel/kmain.o kernel/kernel.o kernel/panic.o \
	kernel/include/util.o \
	kernel/idt/idt.o kernel/idt/idt_asm.o kernel/idt/irq.o kernel/timer/timer.o \
	kernel/gdt/gdt.o kernel/gdt/gdt_asm.o \
	kernel/drivers/io/fb/fb.o kernel/drivers/io/fb/print.o kernel/drivers/io/io_ports.o \
	kernel/drivers/io/fb/console.o \
	output/spleen-12x24.o output/spleen-16x32.o output/spleen-32x64.o \
	kernel/drivers/keyboard/keyboard.o kernel/drivers/keyboard/ps2.o \
	kernel/memory/heap.o kernel/memory/kheap.o kernel/memory/multiheap.o kernel/memory/paging.o kernel/memory/paging_asm.o kernel/memory/memory.o \
	kernel/task/task.o kernel/task/task_asm.o kernel/task/process.o \
	kernel/task/formats/elf.o kernel/task/formats/elf_loader.o \
	kernel/drivers/disk/disk.o kernel/drivers/disk/disk_streamer.o kernel/drivers/disk/gpt.o \
	kernel/drivers/fs/file.o kernel/drivers/fs/path_parser.o \
	kernel/drivers/fs/neofs/neofs.o kernel/drivers/fs/ramfs/ramfs.o \
	kernel/syscalls/isr80h.o kernel/syscalls/io.o kernel/syscalls/heap.o kernel/syscalls/process.o kernel/syscalls/promise.o kernel/syscalls/privilege.o kernel/syscalls/kernel.o kernel/syscalls/fs.o kernel/syscalls/timer.o \
	kernel/drivers/io/graphics/graphics.o

#C compiler
CC := x86_64-elf-gcc
CFLAGS := -m64 -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
	-Wall -Wextra -g -c

#Linker
LD := ld.lld
LDFLAGS := -m elf_x86_64 -T kernel/link.ld

#Assembly compiler
AS := nasm
ASFLAGS := -f elf64

#Objcopy
OBJCOPY := x86_64-elf-objcopy

#Emulator
QEMU ?= qemu-system-x86_64
QEMUFLAGS ?=

MAKEFILE_DIR := $(dir $(MAKEFILE_PATH))

.PHONY: all run clean kernel_clean mkdir bootloader fonts

all: output/disk.img disk_content

debug: output/kernel.elf

run: all
	$(QEMU) -drive if=pflash,format=raw,readonly=on,file=/opt/homebrew/share/qemu/edk2-x86_64-code.fd \
  		-drive file=output/disk.img,format=raw -m 512M $(QEMUFLAGS)

output:
	mkdir -p output


FONT_DIR := kernel/drivers/io/fb/psf
output/spleen-%.o: $(FONT_DIR)/spleen-%.psfu | output
	cd $(FONT_DIR) && $(OBJCOPY) -I binary -O elf64-x86-64 -B i386 spleen-$*.psfu $(abspath $@)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

output/kernel.bin: $(OBJECTS) | output
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

output/link_elf.ld: kernel/link.ld | output
	sed '/OUTPUT_FORMAT/d' $< > $@

output/kernel.elf: $(OBJECTS) output/link_elf.ld | output
	$(LD) -m elf_x86_64 -T output/link_elf.ld $(OBJECTS) -o $@

output/BOOTX64.EFI: $(wildcard boot/src/*.c boot/src/*.h) | output
	cd ./boot && $(MAKE) all
	cp ./boot/output/BOOTX64.EFI ./output/BOOTX64.EFI

disk_content: userland_execs output/kernel.bin output/disk.img
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=output/kernel.bin trgt=boot/kernel.bin --flags rx
	
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/configs/boot.cfg trgt=sysro/boot.cfg --flags r
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/configs/nebulo.cfg trgt=boot/nebulo/nebulo.cfg --flags rw

	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/launch/output/launch.elf trgt=sysro/launch --flags rx

	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/execs/sh/output/sh.elf 			trgt=execs/sh 		--flags rx
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/execs/fetch/output/fetch.elf 		trgt=execs/fetch 	--flags rx
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/execs/echo/output/echo.elf 		trgt=execs/echo 	--flags rx
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/execs/sysinfo/output/sysinfo.elf 	trgt=execs/sysinfo 	--flags rx
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/execs/fileutils/output/read.elf 	trgt=execs/read 	--flags rx
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/execs/running/output/running.elf 	trgt=execs/running 	--flags rx
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/execs/kill/output/kill.elf 		trgt=execs/kill 	--flags rx
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/execs/test/output/test.elf 		trgt=execs/test 	--flags rx
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/execs/fileutils/output/rm.elf 	trgt=execs/rm 		--flags rx
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/execs/fileutils/output/crt.elf 	trgt=execs/crt 		--flags rx
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/execs/fileutils/output/list.elf 	trgt=execs/list 	--flags rx
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/execs/fileutils/output/mkdir.elf 	trgt=execs/mkdir 	--flags rx
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/execs/help/output/help.elf 		trgt=execs/help 	--flags rx
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/execs/uptime/output/uptime.elf 	trgt=execs/uptime 	--flags rx
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/execs/pwd/output/pwd.elf 			trgt=execs/pwd 	 	--flags rx
	./fstools/copy.neofs --disk output/disk.img --offset 17825792 if=userland/execs/fbtest/output/fbtest.elf 	trgt=execs/fbtest 	--flags rx

output/disk.img: output output/BOOTX64.EFI
	dd if=/dev/zero of=output/disk.img bs=1M count=64
	sgdisk --new=1:2048:+16M --typecode=1:EF00 \
		--new=2:0:0 --typecode=2:8300 \
		output/disk.img

	mkfs.fat -F 16 -n ESP --offset 2048 output/disk.img 16384
	
	mmd -i output/disk.img@@1048576 ::/EFI ::/EFI/BOOT
	mcopy -i output/disk.img@@1048576 output/BOOTX64.EFI ::/EFI/BOOT/BOOTX64.EFI

	./fstools/mkfs.neofs --disk output/disk.img --offset 17825792 --block_size 4096

userland_execs:
	cd ./userland/libs/stdlib && $(MAKE) all
	cd ./userland/libs/curses && $(MAKE) all
	cd ./userland/launch && $(MAKE) all
	cd ./userland/execs/sh && $(MAKE) all
	cd ./userland/execs/fetch && $(MAKE) all
	cd ./userland/execs/echo && $(MAKE) all
	cd ./userland/execs/sysinfo && $(MAKE) all
	cd ./userland/execs/running && $(MAKE) all
	cd ./userland/execs/kill && $(MAKE) all
	cd ./userland/execs/uptime && $(MAKE) all
	cd ./userland/execs/help && $(MAKE) all
	cd ./userland/execs/test && $(MAKE) all
	cd ./userland/execs/pwd && $(MAKE) all
	cd ./userland/execs/fileutils && $(MAKE) all
	cd ./userland/execs/fbtest && $(MAKE) all

userland_clean:
	cd ./userland/libs/stdlib && $(MAKE) clean 
	cd ./userland/libs/curses && $(MAKE) clean
	cd ./userland/launch && $(MAKE) clean
	cd ./userland/execs/sh && $(MAKE) clean
	cd ./userland/execs/fetch && $(MAKE) clean
	cd ./userland/execs/echo && $(MAKE) clean
	cd ./userland/execs/sysinfo && $(MAKE) clean
	cd ./userland/execs/running && $(MAKE) clean
	cd ./userland/execs/kill && $(MAKE) clean
	cd ./userland/execs/uptime && $(MAKE) clean
	cd ./userland/execs/help && $(MAKE) clean
	cd ./userland/execs/test && $(MAKE) clean
	cd ./userland/execs/pwd && $(MAKE) clean
	cd ./userland/execs/fileutils && $(MAKE) clean
	cd ./userland/execs/fbtest && $(MAKE) clean

kernel_clean:
	rm -rf output/*
	find kernel -name "*.o" -delete

boot_clean:
	cd ./boot && $(MAKE) clean

clean: kernel_clean userland_clean boot_clean
