FILES=./output/start.o ./output/stdlib.o
INCLUDES= -I./src
FLAGS= -g

all: $(FILES)
	i686-elf-ld -m elf_i386 -relocatable $(FILES) -o ./stdlib.elf

./output/start.o: ./src/start.s
	nasm -f elf32 ./src/start.s -o ./output/start.o
./output/stdlib.o: ./src/stdlib.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -c ./src/stdlib.c -o ./output/stdlib.o

clean:
	rm -rf ./output/*
	rm -f stdlib.elf