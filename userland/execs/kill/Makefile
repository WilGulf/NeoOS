FILES_KILL=./output/kill.o
FILES_KILLALL=./output/killall.o
INCLUDES= -I../../libs/stdlib/src -I./src
LIBS=../../libs/stdlib/stdlib.elf
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: mkdir ${FILES_KILL} ${FILES_KILLALL}
	i686-elf-gcc -g -T ./link.ld -o ./output/kill.elf -ffreestanding -O0 -nostdlib -fpic -g $(FILES_KILL) $(LIBS)
	i686-elf-gcc -g -T ./link.ld -o ./output/killall.elf -ffreestanding -O0 -nostdlib -fpic -g $(FILES_KILLALL) $(LIBS)

./output/kill.o: ./src/kill.c
	i686-elf-gcc ${INCLUDES} -I./$(FLAGS) -c ./src/kill.c -o ./output/kill.o
./output/killall.o: ./src/killall.c
	i686-elf-gcc ${INCLUDES} -I./$(FLAGS) -c ./src/killall.c -o ./output/killall.o
	
mkdir:
	mkdir -p output

clean:
	rm -rf ./output/*