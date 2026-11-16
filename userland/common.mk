CC := x86_64-elf-gcc

USERLAND_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
STDLIB_DIR := $(USERLAND_DIR)libs/stdlib
STDLIB := $(STDLIB_DIR)/stdlib.elf

SRC_DIR := src
OUT_DIR := output
LINKER_SCRIPT ?= $(USERLAND_DIR)/link.ld

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)
ELF := $(OUT_DIR)/$(TARGET).elf

INCLUDES := -I$(SRC_DIR) -I$(STDLIB_DIR)/src $(EXTRA_INCLUDES)

CFLAGS := -g $(OPT) -m64 -ffreestanding -fno-builtin -fno-pic -fno-pie \
        -fomit-frame-pointer -nostdlib -nostartfiles -nodefaultlibs \
        -Wall -Werror -Wno-unused-function -Wno-unused-label \
        -Wno-unused-parameter -Wno-cpp -mgeneral-regs-only \
        -MMD -MP $(INCLUDES) $(EXTRA_CFLAGS)

LDFLAGS := -g -ffreestanding -nostdlib -static -no-pie \
        -Wl,-z,max-page-size=0x1000

.PHONY: all clean
all: $(ELF)

$(ELF): $(OBJS) $(STDLIB) $(LINKER_SCRIPT)
	$(CC) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $@ $(OBJS) $(STDLIB) $(EXTRA_LIBS)

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OUT_DIR)

-include $(DEPS)