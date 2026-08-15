FILES=./output/sh.o
INCLUDES= -I../../lib/stdlib/src -I../../lib/stdio/src
LIBS=../../lib/stdlib/stdlib.elf ../../lib/stdio/stdio.elf

all: $(FILES)
	i686-elf-gcc -g -T ./link.ld -o ./output/sh.elf -ffreestanding -O0 -nostdlib -fpic -g $(FILES) $(LIBS)

./output/sh.o: ./sh.c
	i686-elf-gcc ${INCLUDES} -I./$(FLAGS) -c ./sh.c -o ./output/sh.o

clean:
	rm -rf ./output/*