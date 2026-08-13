all:
	nasm -f elf32 ./keyboard.s -o ./output/keyboard.o
	i686-elf-gcc -g -T ./link.ld -o ./output/keyboard.bin -ffreestanding -O0 -nostdlib -fpic -g ./output/keyboard.o

clean:
	rm -rf ./output/*