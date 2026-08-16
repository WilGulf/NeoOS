FILES=./output/start_asm.o ./output/start.o ./output/stdlib.o ./output/stdlib_asm.o ./output/string.o ./output/memory.o
INCLUDES= -I./src
FLAGS= -g -nostdlib

all: mkdir $(FILES)
	i686-elf-ld -m elf_i386 -relocatable $(FILES) -o ./stdlib.elf

./output/start_asm.o: ./src/start_asm.s
	nasm -f elf32 ./src/start_asm.s -o ./output/start_asm.o
./output/stdlib.o: ./src/stdlib.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -c ./src/stdlib.c -o ./output/stdlib.o
./output/stdlib_asm.o: ./src/stdlib_asm.s
	nasm -f elf32 ./src/stdlib_asm.s -o ./output/stdlib_asm.o
./output/string.o: ./src/string.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -c ./src/string.c -o ./output/string.o
./output/memory.o: ./src/memory.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -c ./src/memory.c -o ./output/memory.o
./output/start.o: ./src/start.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -c ./src/start.c -o ./output/start.o

mkdir:
	mkdir -p output
	
clean:
	rm -rf ./output/*
	rm -f stdlib.elf