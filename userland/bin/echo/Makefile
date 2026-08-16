FILES=./output/echo.o
INCLUDES= -I../../lib/stdlib/src -I../../lib/stdio/src -I./src
LIBS=../../lib/stdlib/stdlib.elf ../../lib/stdio/stdio.elf
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-error=div-by-zero -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: mkdir ${FILES}
	i686-elf-gcc -g -T ./link.ld -o ./output/echo.elf -ffreestanding -O0 -nostdlib -fpic -g $(FILES) $(LIBS)

./output/echo.o: ./src/echo.c
	i686-elf-gcc ${INCLUDES} -I./$(FLAGS) -c ./src/echo.c -o ./output/echo.o

mkdir:
	mkdir -p output

clean:
	rm -rf ./output/*