FILES=./output/test.o
INCLUDES= -I../../libs/stdlib/src -I../../libs/curses/src -I./src
LIBS=../../libs/stdlib/stdlib.elf ../../libs/curses/curses.elf
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: mkdir ${FILES}
	i686-elf-gcc -g -T ./link.ld -o ./output/test.elf -ffreestanding -O0 -nostdlib -fpic -g $(FILES) $(LIBS)

./output/test.o: ./src/test.c
	i686-elf-gcc ${INCLUDES} -I./$(FLAGS) -c ./src/test.c -o ./output/test.o

mkdir:
	mkdir -p output

clean:
	rm -rf ./output/*