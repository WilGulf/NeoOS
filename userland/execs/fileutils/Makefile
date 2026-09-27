FILES=./output/create.o ./output/remove.o ./output/list.o ./output/read.o ./output/mkdir.o
INCLUDES= -I../../libs/stdlib/src -I./src
LIBS=../../libs/stdlib/stdlib.elf
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions  -fno-builtin -Werror -Wno-error=div-by-zero -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: mkdir ${FILES}
	i686-elf-gcc -g -T ./link.ld -o ./output/crt.elf -ffreestanding -O0 -nostdlib -fpic -g ./output/create.o $(LIBS)
	i686-elf-gcc -g -T ./link.ld -o ./output/rm.elf -ffreestanding -O0 -nostdlib -fpic -g ./output/remove.o $(LIBS)
	i686-elf-gcc -g -T ./link.ld -o ./output/list.elf -ffreestanding -O0 -nostdlib -fpic -g ./output/list.o $(LIBS)
	i686-elf-gcc -g -T ./link.ld -o ./output/read.elf -ffreestanding -O0 -nostdlib -fpic -g ./output/read.o $(LIBS)
	i686-elf-gcc -g -T ./link.ld -o ./output/mkdir.elf -ffreestanding -O0 -nostdlib -fpic -g ./output/mkdir.o $(LIBS)

./output/create.o: ./src/create.c
	i686-elf-gcc ${INCLUDES} -I./$(FLAGS) -c ./src/create.c -o ./output/create.o
./output/remove.o: ./src/remove.c
	i686-elf-gcc ${INCLUDES} -I./$(FLAGS) -c ./src/remove.c -o ./output/remove.o
./output/list.o: ./src/list.c
	i686-elf-gcc ${INCLUDES} -I./$(FLAGS) -c ./src/list.c -o ./output/list.o
./output/read.o: ./src/read.c
	i686-elf-gcc ${INCLUDES} -I./$(FLAGS) -c ./src/read.c -o ./output/read.o
./output/mkdir.o: ./src/mkdir.c
	i686-elf-gcc ${INCLUDES} -I./$(FLAGS) -c ./src/mkdir.c -o ./output/mkdir.o

mkdir:
	mkdir -p output

clean:
	rm -rf ./output/*