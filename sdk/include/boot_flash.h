/* the boot ROM's flash memory services. sybtfl.src passes each call to the
 * boot ROM. fm.c is their one caller, and each comment below says how fm.c
 * reads the call's result */
#ifndef BOOT_FLASH_H
#define BOOT_FLASH_H
#include <sg_xpt.h>

/* fm.c opens the blocks from 1 to one below this */
#define BOOT_FLASH_BLOCK_END 5

/* the two results of a service that fm.c tests by name */
#define BOOT_FLASH_SUCCESS 0
#define BOOT_FLASH_FAILURE (-1)

/* a block's flash address and its size in bytes */
typedef struct SYS_BT_FL_BLK_INFO {
    Sint32 fmaddr;
    Sint32 size;
} SY_BT_FL_BLKINFO;

/* store the flash address and size of block part in *info. fm.c tests no
 * result */
Sint32 syBtFlGetInfo(Sint32 part, SY_BT_FL_BLKINFO *info);
/* read size bytes of the flash at address into buffer. fm.c takes
 * BOOT_FLASH_FAILURE for a failure */
Sint32 syBtFlRead(Sint32 address, void *buffer, Sint32 size);
/* write size bytes from buffer to the flash at address. fm.c takes any
 * result but size for a failure */
Sint32 syBtFlWrite(Sint32 address, void *buffer, Sint32 size);
/* erase the flash's sector at address. fm.c takes BOOT_FLASH_FAILURE for a
 * failure, and before a format any result but BOOT_FLASH_SUCCESS */
Sint32 syBtFlDeleteSct(Sint32 address);

#endif
