all:
	nasm -f elf32 ./blank.s -o ./output/blank.o
	i686-elf-gcc -g -T ./link.ld -o ./output/blank.bin -ffreestanding -O0 -nostdlib -fpic -g ./output/blank.o

clean:
	rm -rf ./output/*