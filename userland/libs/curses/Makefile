FILES=./output/curses.o ./output/format.o
INCLUDES= -I./src -I../stdlib/src
FLAGS= -g -nostdlib

all: mkdir $(FILES)
	i686-elf-ld -m elf_i386 -relocatable $(FILES) -o ./curses.elf

./output/curses.o: ./src/curses.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -c ./src/curses.c -o ./output/curses.o
./output/format.o: ./src/format.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -c ./src/format.c -o ./output/format.o

mkdir:
	mkdir -p output
	
clean:
	rm -rf ./output/*
	rm -f curses.elf