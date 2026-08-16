FILES=./output/sh.o
INCLUDES= -I../../lib/stdlib/src -I../../lib/stdio/src -I./src
LIBS=../../lib/stdlib/stdlib.elf ../../lib/stdio/stdio.elf
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: mkdir ${FILES}
	i686-elf-gcc -g -T ./link.ld -o ./output/sh.elf -ffreestanding -O0 -nostdlib -fpic -g $(FILES) $(LIBS)

./output/sh.o: ./src/sh.c
	i686-elf-gcc ${INCLUDES} -I./$(FLAGS) -c ./src/sh.c -o ./output/sh.o

mkdir:
	mkdir -p output

clean:
	rm -rf ./output/*