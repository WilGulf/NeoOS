FILES=./output/stdio.o ./output/stdio_asm.o
INCLUDES= -I./src
FLAGS= -g -nostdlib

all: $(FILES)
	i686-elf-ld -m elf_i386 -relocatable $(FILES) -o ./stdio.elf

./output/stdio.o: ./src/stdio.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -c ./src/stdio.c -o ./output/stdio.o
./output/stdio_asm.o: ./src/stdio_asm.s
	nasm -f elf32 ./src/stdio_asm.s -o ./output/stdio_asm.o

clean:
	rm -rf ./output/*
	rm -f stdio.elf