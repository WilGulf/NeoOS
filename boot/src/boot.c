#include <efi.h>
#include <efilib.h>
#include "string.h"

#include "disk.h"
#include "fs.h"
#include "path_parser.h"

#define MEMORY_MAP_TOTAL_ENTRIES_LOCATION 0x210000
#define MEMORY_MAP_LOCATION 0x210008

typedef struct __attribute__((packed)) e820_entry_t {
    UINT64 base_addr;
    UINT64 length;
    UINT32 type;
    UINT32 extended_attr;
} e820_entry_t;

typedef struct __attribute__((packed)) e820_entries_t {
    UINT64 count;
    e820_entry_t entries[];
} e820_entries_t;

EFI_STATUS setup_memory_maps(void) {
    EFI_STATUS res;
    UINTN memory_map_size = 0;
    EFI_MEMORY_DESCRIPTOR *memory_map = NULL;
    UINTN map_key;
    UINTN descriptor_size;
    UINT32 descriptor_version;

    res = uefi_call_wrapper(gBS->GetMemoryMap, 5, &memory_map_size, NULL, &map_key, &descriptor_size, &descriptor_version);

    if (res != EFI_BUFFER_TOO_SMALL && EFI_ERROR(res)) {
        return res;
    }

    memory_map_size += descriptor_size * 10;

    memory_map = AllocatePool(memory_map_size);
    if (!memory_map) {
        return EFI_OUT_OF_RESOURCES;
    }

    res = uefi_call_wrapper(gBS->GetMemoryMap, 5, &memory_map_size, memory_map, &map_key, &descriptor_size, &descriptor_version);
    if (EFI_ERROR(res)) {
        FreePool(memory_map);
        return res;
    }

    UINTN descriptor_count = memory_map_size / descriptor_size;
    EFI_MEMORY_DESCRIPTOR *descriptor = memory_map;
    UINTN total_descriptors = 0;
    for (UINTN i = 0; i < descriptor_count; i++) {
        if (descriptor->Type == EfiConventionalMemory) {
            total_descriptors++;
        }

        descriptor = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)descriptor + descriptor_size);
    }

    EFI_PHYSICAL_ADDRESS memory_map_location_e820 = MEMORY_MAP_TOTAL_ENTRIES_LOCATION;
    UINTN memory_map_size_e820 = (sizeof(e820_entry_t) * total_descriptors) + sizeof(UINT64);
    res = uefi_call_wrapper(gBS->AllocatePages, 4, AllocateAddress, EfiLoaderData, EFI_SIZE_TO_PAGES(memory_map_size_e820), &memory_map_location_e820);
    if (EFI_ERROR(res)) {
        return res;
    }

    e820_entries_t *e820_entries = (e820_entries_t *)memory_map_location_e820;
    UINTN conventional_memory_i = 0;
    descriptor = memory_map;
    for (UINTN i = 0; i < descriptor_count; i++) {
        if (descriptor->Type == EfiConventionalMemory) {
            e820_entry_t *e820_entry = &e820_entries->entries[conventional_memory_i];
            e820_entry->base_addr = descriptor->PhysicalStart;
            e820_entry->length = descriptor->NumberOfPages * 4096;
            e820_entry->type = 1;
            e820_entry->extended_attr = 0;
            conventional_memory_i++;
        }

        descriptor = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)descriptor + descriptor_size);
    } 

    e820_entries->count = total_descriptors;
    FreePool(memory_map);

    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI efi_main(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    Print(L"NeBuLo\n");

    setup_memory_maps();

    EFI_STATUS res = disk_init(ImageHandle);
    if (EFI_ERROR(res)) {
        return res;
    }

    res = neofs_resolve();
    if (EFI_ERROR(res)) {
        return res;
    }

    struct path_root *path = parse_path("boot/kernel.bin");
    if (!path || !path->first) {
        return EFI_INVALID_PARAMETER;
    }

    void *fd = neofs_open(path->first, FILE_MODE_READ);
    path_parser_free(path);
    if (!fd) {
        return EFI_NOT_FOUND;
    }

    struct file_stat st;
    if (neofs_stat(fd, &st) < 0 || st.filesize == 0) {
        neofs_close(fd);
        return EFI_LOAD_ERROR;
    }

    UINTN size = st.filesize;
    VOID *buffer = AllocatePool(size);
    if (!buffer) return EFI_OUT_OF_RESOURCES;

    if (neofs_read(fd, size, 1, buffer) != (int)size) {
        return EFI_LOAD_ERROR;
    }
    neofs_close(fd);

    EFI_PHYSICAL_ADDRESS kernel_base = 0x100000;
    res = uefi_call_wrapper(gBS->AllocatePages, 4, AllocateAddress, EfiLoaderData, EFI_SIZE_TO_PAGES(size), &kernel_base);
    if (EFI_ERROR(res)) {
        return res;
    }

    memcpy((VOID *)kernel_base, buffer, size);

    Print(L"Loading kernel.bin ...\n");

    uefi_call_wrapper(gBS->ExitBootServices, 2, ImageHandle, 0);

    __asm__("jmp *%0" : : "r"(kernel_base));

    return EFI_SUCCESS;
}