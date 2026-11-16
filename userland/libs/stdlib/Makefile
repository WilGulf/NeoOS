FILES=./output/start_asm.o ./output/start.o ./output/stdlib.o ./output/stdlib_asm.o ./output/string.o ./output/memory.o ./output/stdio.o ./output/dirent.o
INCLUDES= -I./src
FLAGS= -g -nostdlib

all: mkdir $(FILES)
	x86_64-elf-ld -m elf_x86_64 -relocatable $(FILES) -o ./stdlib.elf

./output/start_asm.o: ./src/start_asm.s
	nasm -f elf64 ./src/start_asm.s -o ./output/start_asm.o
./output/stdlib.o: ./src/stdlib.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -c ./src/stdlib.c -o ./output/stdlib.o
./output/stdlib_asm.o: ./src/stdlib_asm.s
	nasm -f elf64 ./src/stdlib_asm.s -o ./output/stdlib_asm.o
./output/string.o: ./src/string.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -c ./src/string.c -o ./output/string.o
./output/memory.o: ./src/memory.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -c ./src/memory.c -o ./output/memory.o
./output/start.o: ./src/start.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -c ./src/start.c -o ./output/start.o
./output/stdio.o: ./src/stdio.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -c ./src/stdio.c -o ./output/stdio.o
./output/dirent.o: ./src/dirent.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -c ./src/dirent.c -o ./output/dirent.o
	
mkdir:
	mkdir -p output
	
clean:
	rm -rf ./output/*
	rm -f stdlib.elf