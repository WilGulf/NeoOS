#ifndef DISK_H
#define DISK_H

EFI_STATUS disk_init(EFI_HANDLE image_handle);
EFI_STATUS disk_seek(UINTN new_pos);
EFI_STATUS disk_read(VOID *out, UINTN total);
EFI_STATUS disk_write(VOID *in, UINTN total);

#endif