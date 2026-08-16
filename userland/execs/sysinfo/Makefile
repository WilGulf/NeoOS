FILES=./output/info.o
INCLUDES= -I../../libs/stdlib/src -I../../libs/stdio/src -I./src
LIBS=../../libs/stdlib/stdlib.elf ../../libs/stdio/stdio.elf
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: mkdir ${FILES}
	i686-elf-gcc -g -T ./link.ld -o ./output/sysinfo.elf -ffreestanding -O0 -nostdlib -fpic -g $(FILES) $(LIBS)

./output/info.o: ./src/info.c
	i686-elf-gcc ${INCLUDES} -I./$(FLAGS) -c ./src/info.c -o ./output/info.o

mkdir:
	mkdir -p output

clean:
	rm -rf ./output/*