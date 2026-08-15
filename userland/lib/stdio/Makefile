FILES=./output/stdio.o
INCLUDES=

all: $(FILES)
	i686-elf-ld -m elf_i386 -relocatable $(FILES) -o ./stdio.elf

./output/stdio.o: ./src/stdio.s
	nasm -f elf ./src/stdio.s -o ./output/stdio.o
	
clean:
	rm -rf ./output/*
	rm -f stdio.elf