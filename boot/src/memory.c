#include <efi.h>
#include <efilib.h>

#include "memory.h"

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