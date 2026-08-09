all:
	nasm -f elf32 ./print.s -o ./output/print.o
	i686-elf-gcc -g -T ./link.ld -o ./output/print.bin -ffreestanding -O0 -nostdlib -fpic -g ./output/print.o

clean:
	rm -rf ./output/*