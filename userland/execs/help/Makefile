FILES=./output/help.o
INCLUDES= -I../../libs/stdlib/src -I./src
LIBS=../../libs/stdlib/stdlib.elf
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: mkdir ${FILES}
	i686-elf-gcc -g -T ./link.ld -o ./output/help.elf -ffreestanding -O0 -nostdlib -fpic -g $(FILES) $(LIBS)

./output/help.o: ./src/help.c
	i686-elf-gcc ${INCLUDES} -I./$(FLAGS) -c ./src/help.c -o ./output/help.o
	
mkdir:
	mkdir -p output

clean:
	rm -rf ./output/*