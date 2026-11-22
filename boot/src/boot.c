#include <efi.h>
#include <efilib.h>
#include "string.h"

#include "disk.h"
#include "fs.h"
#include "path_parser.h"
#include "memory.h"

#define BOOTINFO_MAGIC 0x4E45585553

typedef uint8_t uuid_t[16];

struct boot_graphics_info {
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *framebuffer;
    uint32_t horizontal_resolution;
    uint32_t vertical_resolution;
    uint32_t pixels_pre_scanline;
} __attribute__((packed));

struct bootinfo {
    uint64_t magic;
    uuid_t system_uuid;
    uint64_t command_line_ptr;
    uint64_t command_line_len;
    struct boot_graphics_info boot_graphics_info;
} __attribute__((packed));

EFI_STATUS get_framebuffer_info(EFI_GRAPHICS_OUTPUT_PROTOCOL **graphics_out) {
    EFI_STATUS res;

    res = uefi_call_wrapper(gBS->LocateProtocol, 3, &gEfiGraphicsOutputProtocolGuid, NULL, (VOID **)graphics_out);
    if (EFI_ERROR(res)) {
        return res;
    }

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN info_size;
    res = uefi_call_wrapper((*graphics_out)->QueryMode, 4, *graphics_out, (*graphics_out)->Mode->Mode, &info_size, &info);
    if (EFI_ERROR(res)) {
        return res;
    }

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

    EFI_GRAPHICS_OUTPUT_PROTOCOL *graphics_out = NULL;
    res = get_framebuffer_info(&graphics_out);
    if (EFI_ERROR(res)) {
        Print(L"Failed to get graphics info: %r\n", res);
        return res;    
    }

    struct bootinfo *bootinfo = AllocatePool(sizeof(struct bootinfo));
    neofs_volume_uuid(bootinfo->system_uuid);

    bootinfo->magic = BOOTINFO_MAGIC;
    bootinfo->command_line_len = 0;
    bootinfo->command_line_ptr = 0;

    bootinfo->boot_graphics_info.framebuffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)graphics_out->Mode->FrameBufferBase;
    bootinfo->boot_graphics_info.pixels_pre_scanline = graphics_out->Mode->Info->PixelsPerScanLine;
    bootinfo->boot_graphics_info.horizontal_resolution = graphics_out->Mode->Info->HorizontalResolution;
    bootinfo->boot_graphics_info.vertical_resolution = graphics_out->Mode->Info->VerticalResolution;

    Print(L"Loading kernel.bin ...\n");

    uefi_call_wrapper(gBS->ExitBootServices, 2, ImageHandle, 0);

    typedef void (__attribute__((sysv_abi)) *kernel_entry_t)(struct bootinfo *);
    ((kernel_entry_t)(UINTN)kernel_base)(bootinfo);

    return EFI_SUCCESS;
}