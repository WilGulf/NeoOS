#include <efi.h>
#include <efilib.h>

static HARDDRIVE_DEVICE_PATH *find_hd_node(EFI_DEVICE_PATH *dev_path) {
    for (; !IsDevicePathEnd(dev_path); dev_path = NextDevicePathNode(dev_path)) {
        if (
            DevicePathType(dev_path) == MEDIA_DEVICE_PATH &&
            DevicePathSubType(dev_path) == MEDIA_HARDDRIVE_DP
        ) {
            return (HARDDRIVE_DEVICE_PATH *)dev_path;
        }
    }

    return NULL;
}

static EFI_STATUS find_partition(EFI_HANDLE image_handle, UINT32 partition, EFI_HANDLE *out) {
    EFI_STATUS res = 0;
    EFI_LOADED_IMAGE *image;
    EFI_DEVICE_PATH *boot_dev_path;
    HARDDRIVE_DEVICE_PATH *boot_hd;
    EFI_HANDLE *handles = NULL;
    UINTN num_handles = 0;

    res = uefi_call_wrapper(BS->HandleProtocol, 3, image_handle, &LoadedImageProtocol, (VOID **)&image);
    if (EFI_ERROR(res)) {
        return res;
    }

    res = uefi_call_wrapper(BS->HandleProtocol, 3, image->DeviceHandle, &DevicePathProtocol, (VOID **)&boot_dev_path);
    if (EFI_ERROR(res)) {
        return res;
    }

    boot_hd = find_hd_node(boot_dev_path);
    if (!boot_hd) {
        return EFI_NOT_FOUND;
    }

    UINTN prefix_len = (UINT8 *)boot_hd - (UINT8 *)boot_dev_path;
    res = LibLocateHandle(ByProtocol, &BlockIoProtocol, NULL, &num_handles, &handles);
    if (EFI_ERROR(res)) {
        return res;
    }

    res = EFI_NOT_FOUND;
    for (UINTN i = 0; i < num_handles; i++) {
        EFI_BLOCK_IO *block_io;
        EFI_DEVICE_PATH *dev_path;

        if (EFI_ERROR(uefi_call_wrapper(BS->HandleProtocol, 3, handles[i], &BlockIoProtocol, (VOID **)&block_io))) {
            continue;
        }

        if (!block_io->Media->LogicalPartition || !block_io->Media->MediaPresent) {
            continue;
        }

        if (EFI_ERROR(uefi_call_wrapper(BS->HandleProtocol, 3, handles[i], &DevicePathProtocol, (VOID **)&dev_path))) {
            continue;
        }

        HARDDRIVE_DEVICE_PATH *harddrive = find_hd_node(dev_path);
        if (!harddrive || (UINT8 *)harddrive - (UINT8 *)dev_path != (INTN)prefix_len) {
            continue;
        }
        if (CompareMem(dev_path, boot_dev_path, prefix_len) != 0) {
            continue;
        }
        if (harddrive->PartitionNumber != partition) {
            continue;
        }

        *out = handles[i];
        res = EFI_SUCCESS;
        break;
    }

    FreePool(handles);
    return res;
}

EFI_BLOCK_IO *block_io;
UINT32 block_size = 0;
UINT64 last_lba = 0;
UINT64 pos = 0;
VOID *bounce_raw;
VOID *bounce;

EFI_STATUS disk_init(EFI_HANDLE image_handle) {
    EFI_STATUS res;
    EFI_HANDLE partition_handle;

    res = find_partition(image_handle, 2, &partition_handle);
    if (EFI_ERROR(res)) {
        return res;
    }

    res = uefi_call_wrapper(BS->HandleProtocol, 3, partition_handle, &BlockIoProtocol, (VOID **)&block_io);
    if (EFI_ERROR(res)) {
        return res;
    }

    block_size = block_io->Media->BlockSize;
    last_lba = block_io->Media->LastBlock;

    UINTN align = block_io->Media->IoAlign > 1 ? block_io->Media->IoAlign : 1;
    
    bounce_raw = AllocatePool(block_size + align);
    if (!bounce_raw) {
        return EFI_OUT_OF_RESOURCES;
    }

    bounce = (VOID *)(((UINTN)bounce_raw + align - 1) & ~((UINTN)align - 1));
    pos = 0;

    return EFI_SUCCESS;
}

EFI_STATUS disk_seek(UINTN new_pos) {
    pos = new_pos;
    return EFI_SUCCESS;
}

EFI_STATUS disk_read(VOID *out, UINTN total) {
    UINT8 *dst = out;

    while (total > 0) {
        UINT64 lba = pos / block_size;
        UINTN offset = pos % block_size;

        EFI_STATUS res = uefi_call_wrapper(block_io->ReadBlocks, 5,
        block_io, block_io->Media->MediaId, lba, (UINTN)block_size, bounce);
        UINT8 *b = bounce;;
        if (EFI_ERROR(res)) return res;

        UINTN n = block_size - offset;
        if (n > total) n = total;

        for (UINTN k = 0; k < n; k++) dst[k] = b[offset + k];

        dst += n; pos += n; total -= n;
    }

    return EFI_SUCCESS;
}

EFI_STATUS disk_write(VOID *in, UINTN total) {
    (VOID)in; (VOID)total;
    return EFI_UNSUPPORTED;
}