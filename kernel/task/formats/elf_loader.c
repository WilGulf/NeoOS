#include "elf_loader.h"

#include "../../include/status.h"
#include "../../include/config.h"
#include "../../include/util.h"
#include "../../drivers/fs/file.h"
#include "../../memory/kheap.h"
#include "../../memory/paging.h"
#include "../../kernel.h"
#include "../task.h"

const char elf_signature[] = {0x7F, 'E', 'L', 'F'};

static kbool elf_valid_signature(void *buffer) {
    return memcmp(buffer, (void *)elf_signature, sizeof(elf_signature)) == 0;
}

static kbool elf_valid_class(struct elf_header *header) {
    return header->e_ident[EI_CLASS] == ELFCLASSNONE || header->e_ident[EI_CLASS] == ELFCLASS32;
}

static kbool elf_valid_encoding(struct elf_header *header) {
    return header->e_ident[EI_DATA] == ELFDATANONE || header->e_ident[EI_DATA] == ELFDATA2LSB;
}

static kbool elf_is_executable(struct elf_header *header) {
    return header->e_type == ET_EXEC && header->e_entry >= PROGRAM_VIRTUAL_ADDRESS;
}

static kbool elf_has_program_header(struct elf_header *header) {
    return header->e_phoff != 0;
}

void *elf_memory(struct elf_file *file) {
    return file->elf_memory;
}

void *elf_image_memory(struct elf_file *file) {
    return file->elf_image_memory;
}

struct elf_header *elf_header(struct elf_file *file) {
    return file->elf_memory;
}

struct elf32_shdr *elf_sheader(struct elf_header *header) {
    return (struct elf32_shdr *)((int)header + header->e_shoff);
}

struct elf32_phdr *elf_pheader(struct elf_header *header) {
    if (header->e_phoff == 0) {
        return 0;
    }

    return (struct elf32_phdr *)((int)header + header->e_phoff);
}

struct elf32_phdr *elf_program_header(struct elf_header *header, int index) {
    return &elf_pheader(header)[index];
}

struct elf32_shdr *elf_section(struct elf_header *header, int index) {
    return &elf_sheader(header)[index];
}

void *elf_phdr_physical_address(struct elf_file *file, struct elf32_phdr *phdr) {
    return elf_image_memory(file) + (phdr->p_vaddr - (int)file->image_virtual_base_address);
}

char *elf_str_table(struct elf_header *header) {
    return (char *)header + elf_section(header, header->e_shstrndx)->sh_offset;
}

void *elf_virtual_base(struct elf_file *file) {
    return file->virtual_base_address;
}

void *elf_virtual_end(struct elf_file *file) {
    return file->virtual_end_address;
}

void *elf_physical_base(struct elf_file *file) {
    return file->physical_base_address;
}

void *elf_physical_end(struct elf_file *file) {
    return file->physical_end_address;
}

int elf_validate_loaded(struct elf_header *header) {
    return (elf_valid_signature(header) && elf_valid_class(header) && elf_valid_encoding(header) && elf_has_program_header(header) && elf_is_executable(header)) ? ALL_OK : -ERROR_INVALID_FORMAT;
}

int elf_process_phdr_pt_load(struct elf_file *elf_file, struct elf32_phdr *phdr) {
    if (elf_file->virtual_base_address >= (void *)phdr->p_vaddr || elf_file->virtual_base_address == 0x00) {
        elf_file->virtual_base_address = (void *)phdr->p_vaddr;
        elf_file->physical_base_address = elf_memory(elf_file) + phdr->p_offset;
    }

    uint32_t end_virtual_address = phdr->p_vaddr + phdr->p_memsz;
    if (elf_file->virtual_end_address <= (void *)(end_virtual_address) || elf_file->virtual_end_address == 0x00) {
        elf_file->virtual_end_address = (void *)end_virtual_address;
        elf_file->physical_end_address = elf_memory(elf_file) + phdr->p_offset + phdr->p_filesz;
    }

    return 0;
}

int elf_process_pheader(struct elf_file *elf_file, struct elf32_phdr *phdr) {
    int res = 0;
    switch(phdr->p_type) {
        case PT_LOAD:
            res = elf_process_phdr_pt_load(elf_file, phdr);
        break;
    }

    return res;
}

int elf_process_pheaders(struct elf_file *elf_file) {
    int res = 0;
    struct elf_header *header = elf_header(elf_file);
    for (int i = 0; i < header->e_phnum; i++) {
        struct elf32_phdr *phdr = elf_program_header(header, i);
        res = elf_process_pheader(elf_file, phdr);
        if (res < 0) {
            break;
        }
    }

    return res;
}

void elf_file_free(struct elf_file *file) {
    if (file->elf_memory) {
        kfree(file->elf_memory);
    }
    if (file->elf_image_memory) {
        kfree(file->elf_image_memory);
    }

    kfree(file);
}

struct elf_file *allocate_elf_file() {
    return (struct elf_file *)kzalloc(sizeof(struct elf_file));
}

int elf_process_loaded(struct elf_file *file) {
    int res = 0;
    struct elf_header *header = elf_header(file);
    res = elf_validate_loaded(header);
    if (res < 0) {
        goto out;
    }

    res = elf_process_pheaders(file);
    if (res < 0) {
        goto out;
    }

out:
    return res;
}

int elf_build_image(struct elf_file *file) {
    uint32_t virtual_base_address = 0;
    uint32_t virtual_end_address = 0;

    struct elf_header *header = elf_header(file);
    for (int i = 0; i < header->e_phnum; i++) {
        struct elf32_phdr *phdr = elf_program_header(header, i);

        if (phdr->p_type != PT_LOAD) {
            continue;
        }

        if (i == 0) {
            virtual_base_address = phdr->p_vaddr;
        }

        if (phdr->p_vaddr < virtual_base_address) {
            virtual_base_address = phdr->p_vaddr;
        }
        if ((phdr->p_vaddr + phdr->p_memsz) > virtual_end_address) {
            virtual_end_address = phdr->p_vaddr + phdr->p_memsz;
        }
    }

    file->elf_image_memory = kzalloc(virtual_end_address - virtual_base_address);

    for (int i = 0; i < header->e_phnum; i++) {
        struct elf32_phdr *phdr = elf_program_header(header, i);

        if (phdr->p_type != PT_LOAD) {
            continue;
        }

        if (phdr->p_filesz > 0) {
            memcpy(file->elf_image_memory + (phdr->p_vaddr - virtual_base_address), file->elf_memory + phdr->p_offset, phdr->p_filesz);
        }
    }

    file->image_virtual_base_address = virtual_base_address;
}

int elf_load(const char *filename, struct elf_file **file_out) {
    struct elf_file *elf_file = allocate_elf_file();
    int fd = 0;
    int res = fopen(filename, "r");
    if (res <= 0) {
        res = -ERROR_IO;
        goto out;
    }

    fd = res;
    struct file_stat stat;
    res = fstat(fd, &stat);
    if (res < 0) {
        goto out;
    }

    elf_file->elf_memory = kzalloc(stat.filesize);
    res = fread(elf_file->elf_memory, stat.filesize, 1, fd);
    if (res < 0) {
        goto out;
    }

    res = elf_process_loaded(elf_file);
    if (res < 0) {
        goto out;
    }

    res = elf_build_image(elf_file);
    if (res < 0) {
        goto out;
    }

    *file_out = elf_file;
out:
    if (res < 0) {
        elf_file_free(elf_file);
    }
    
    fclose(fd);
    return res;
}

void elf_close(struct elf_file *file) {
    if (!file) {
        return;
    }

    kfree(file->elf_memory);
    kfree(file->elf_image_memory);
    kfree(file);
}