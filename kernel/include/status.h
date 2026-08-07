#ifndef STATUS_H
#define STATUS_H

#define ALL_OK 0
#define ERROR_IO 1
#define ERROR_INVALID_ARG 2
#define ERROR_NO_MEM 3
#define ERROR_BAD_PATH 4
#define ERROR_FS_NOT_US 5
#define ERROR_RO 6

#define ERROR(value) (void *)(value)
#define ERROR_I(value) (int)(value)
#define ISERR(value) ((int)value < 0)

#endif