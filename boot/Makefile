CC := x86_64-elf-gcc
CFLAGS := -Wall -Wextra -g -fshort-wchar -ffreestanding -mno-red-zone -fpic -fno-stack-protector -fno-stack-check -maccumulate-outgoing-args \
	-DEFI_FUNCTION_WRAPPER \
	-I./gnu-efi/inc \
	-I./gnu-efi/inc/x86_64

OBJCOPY := x86_64-elf-objcopy


LD := x86_64-elf-ld
LDFLAGS := -nostdlib -shared -Bsymbolic -znocombreloc \
	-T ./gnu-efi/gnuefi/elf_x86_64_efi.lds \
	-L./gnu-efi/x86_64/gnuefi \
	-L./gnu-efi/x86_64/lib

LIBS := -lefi -lgnuefi
CRT0 := ./gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o

SRC := src
OUT := output
GNUEFI := ./gnu-efi

SRCC := $(wildcard $(SRC)/*.c)
OBJS := $(patsubst $(SRC)/%.c,$(OUT)/%.o,$(SRCC))
FEOS := $(OBJS:.o=.d)

all: $(OUT)/BOOTX64.EFI

$(OUT):
	mkdir -p $@

$(CRT0):
	$(MAKE) -C $(GNUEFI) all ARCH=x86_64 CROSS_COMPILE=x86_64-elf- prefix=

$(OUT)/%.o: $(SRC)/%.c | $(OUT)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT)/boot.so: $(OBJS) $(CRT0)
	$(LD) $(LDFLAGS) $(CRT0) $(OBJS) $(LIBS) -o $@

$(OUT)/BOOTX64.EFI: $(OUT)/boot.so
	$(OBJCOPY) \
		-j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc \
		-I elf64-x86-64 -O efi-app-x86_64 \
		$< $@

clean:
	rm -rf output/*

-include $(DEPS)

.PHONY: all clean gnu-efi