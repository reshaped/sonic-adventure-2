/* the program's exit, and the boot ROM's disc check */
#include <app_root.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* the boot ROM's system call vector, in low memory, and its function
 * numbers */
#define BOOT_SYSTEM (*(Sint32(**)(Sint32 func))0x8c0000e0)
#define BOOT_EXIT 1
#define BOOT_CHECK_DISC 2
#define BOOT_UNK_3 3

/* @hack type: mode and unk_e as Uint8 bytes at addresses of their own,
 * where APP_ROOT_INFO's Sint8 fields would do */
#define APP_ROOT_MODE (*(Uint8 *)0x8c0080fc)
#define APP_ROOT_UNK_E (*(Uint8 *)0x8c0080fe)

#define DAT_8C008000 ((char *)0x8c008000)
#define DAT_8C000064 (*(char **)0x8c000064)
#define FUN_8C008300 ((void (*)(void))0x8c008300)

/* the video chip's punch-through alpha threshold register, and the highest
 * threshold, which syBtExit leaves in it */
/* @hack type: the register as a plain Uint32, where a volatile one would
 * do */
#define PT_ALPHA_REF (*(Uint32 *)0xa05f811c)
#define PT_ALPHA_REF_MAX 0xff

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the library's version and build time */
const char *const sygBtBuild = "\nsyBt Ver 1.07 Build:Jun 22 2000 16:16:08\n";
/* NOLINTEND(misc-use-internal-linkage) */

void syBtExit(void)
{
    PT_ALPHA_REF = PT_ALPHA_REF_MAX;
    if (APP_ROOT_MODE == APP_ROOT_SET) {
        if (APP_ROOT_UNK_E == 1) {
            FUN_8C008300();
        } else {
            DAT_8C000064 = DAT_8C008000;
            BOOT_SYSTEM(BOOT_UNK_3);
        }
    } else {
        BOOT_SYSTEM(BOOT_EXIT);
    }
}

Sint32 syBtCheckDisc(void)
{
    syCachePurgeAll();
    return BOOT_SYSTEM(BOOT_CHECK_DISC);
}
