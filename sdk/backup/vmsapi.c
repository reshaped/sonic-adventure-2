/* the backup library's calls. each queues a command for a drive's fiber,
 * or asks the file system at once */
#include <kaede.h>
#include <machine.h>
#include <sg_bup.h>
#include <sg_xpt.h>

#include "vms.h"

/* the count of banks bank_count assumes for a card whose product name opens
 * with BANK. vmsio_bank_info reads the card's own count instead */
#define BANKS 4

/* the member exports these functions and data, the statics aside, though
 * no header of the SDK declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void buInterruptHandler(Uint32 argument);
void buDisableServer(void);
/* each call below does nothing while its drive has a command queued, and
 * returns BUD_ERR_BUSY */
/* queue a command that reads the card's layout, as vmsfs_connect does. it
 * returns 0 */
Sint32 buConnect(Sint32 drive);
/* open the file to read, or create a data file or the executable file to
 * write. each answers at once, as vmsfs_fopen, vmsfs_fcreate and
 * vmsfs_xcreate do */
Sint32 buOpenFile(Sint32 drive, const char *name, BU_FILE **file);
Sint32 buCreateFile(Sint32 drive, const char *name, const BUS_TIME *time,
                    BU_FILE **file);
Sint32 buCreateExecFile(Sint32 drive, const char *name, const BUS_TIME *time,
                        BU_FILE **file);
/* the calls on an open file. file must be an open file's record: each call
 * reads file->drive before its busy test. buSetFileParam answers at once, as
 * vmsfs_fsetparam does. the others queue a command and return 0 */
Sint32 buSetFileParam(BU_FILE *file, Uint16 header, Uint8 copy);
Sint32 buCloseFile(BU_FILE *file);
Sint32 buReadFile(BU_FILE *file, void *buffer, Sint32 blocks);
Sint32 buWriteFile(BU_FILE *file, const void *buffer, Sint32 blocks);
/* queue a command that reads every block of the card into buffer, or
 * writes buffer to every block. each returns 0 */
Sint32 buReadDiskImage(Sint32 drive, void *buffer);
Sint32 buWriteDiskImage(Sint32 drive, const void *buffer);

/* the library's build string */
const char buBuild[] = "\nbu Ver 2.13 Build:Oct 12 2000 19:20:36\n"
                       "VMS File System for Application(dynamic work alloc) ";
/* the slot of Kaede's handlers that runs buExecServer */
const Uint32 buHandlerNo = 20;
/* each drive's Kaede device */
static const Uint8 drive_devices[VMS_DRIVES] = {
    KDD_DEV_A1, KDD_DEV_A2, KDD_DEV_B1, KDD_DEV_B2,
    KDD_DEV_C1, KDD_DEV_C2, KDD_DEV_D1, KDD_DEV_D2};
static Uint32 library_version = 0x00020013;
/* 1 from buInit till buExitForce */
Uint32 buInitializeFlag = 0;
/* the drives that held a card, as bits. drives_ready is from buExecServer's
 * latest run, and drives_last_ready from the run before */
static Uint32 drives_last_ready;
static Uint32 drives_ready;
/* the drives, as bits, whose card came in or left while they had a command
 * queued. buExecServer attaches each card that came in, and reports each that
 * left, once its drive's command ends */
static Uint32 drives_to_attach;
static Uint32 drives_to_remove;
/* the workspace buInit took, or null for drives that take their own */
void *buWorkTop;
/* the bits of the drives buInit started */
Sint32 buUseDrive;
/* NOLINTEND(misc-use-internal-linkage) */

void buInterruptHandler(Uint32 argument)
{
    buExecServer();
}

Sint32 buInit(Sint32 capacity, Sint32 drives, void *workspace,
              void (*initialized)(void))
{
    Sint32 result;
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    buUseDrive = drives;
    buWorkTop = workspace;
    drives_last_ready = 0;
    drives_ready = 0;
    drives_to_attach = 0;
    drives_to_remove = 0;
#ifdef NONMATCHING
    /* this side fails buInit where vmsfs_c_init or vmsfs_init fails, and takes
     * down what they set up first. the matching side ignores vmsfs_c_init's
     * result */
    result = vmsfs_c_init(drives);
    if (result == 0) {
        result = vmsfs_init(capacity, drives, workspace);
    }
    if (result != 0) {
        buExitForce();
        set_imask(mask);
        return result;
    }
#else
    vmsfs_c_init(drives);
    result = vmsfs_init(capacity, drives, workspace);
#endif
    kdSetHandler(buHandlerNo, buExecServer);
    if (result == 0) {
        if (initialized != NULL) {
            initialized();
        }
    }
    buInitializeFlag = 1;
    set_imask(mask);
    return result;
}

void buDisableServer(void)
{
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    kdSetHandler(buHandlerNo, NULL);
    set_imask(mask);
}

Sint32 buExitForce(void)
{
    buInitializeFlag = 0;
    kdSetHandler(buHandlerNo, NULL);
    vmsfs_c_exit();
    return vmsfs_exit();
}

Sint32 buExit(void)
{
    Sint32 drive;

    for (drive = 0; drive < VMS_DRIVES; drive++) {
        if (vmsfs_c_getstate(drive) != 0) {
            return BUD_ERR_BUSY;
        }
    }
    return buExitForce();
}

void buExecServer(void)
{
    Sint32 drive;
    Sint32 bit;
    Uint32 inserted;
    Uint32 idle;
    Uint32 removed;
    Sint32 (*attach)(Sint32 drive);

    if (!buInitializeFlag) {
        return;
    }
    if (buWorkTop != NULL) {
        attach = buMount;
    } else {
        attach = buConnect;
    }
    vmsfs_c_synch();
    drives_last_ready = drives_ready;
    drives_ready = vmsfs_isready();
    drives_ready &= buUseDrive;
    idle = 0;
    for (drive = 0, bit = 1; drive < VMS_DRIVES; drive++, bit <<= 1) {
        idle |= buStat(drive) == 0 ? bit : 0;
    }
    inserted = (drives_last_ready ^ drives_ready) & drives_ready;
    for (drive = 0, bit = 1; drive < VMS_DRIVES; drive++, bit <<= 1) {
        if (bit & inserted) {
            if (bit & idle) {
                attach(drive);
                drives_to_attach &= ~bit;
            } else {
                drives_to_attach |= bit;
            }
        }
        if (bit & idle) {
            if ((drives_to_attach & bit) != 0) {
                attach(drive);
                drives_to_attach &= ~bit;
            }
        }
    }
    removed = (drives_last_ready ^ drives_ready) & drives_last_ready;
    for (drive = 0, bit = 1; drive < VMS_DRIVES; drive++, bit <<= 1) {
        if (bit & removed) {
            if (bit & idle) {
                vmsfs_c_execcallback(drive, BUD_OP_UNMOUNT, 0);
                drives_to_remove &= ~bit;
            } else {
                drives_to_remove |= bit;
            }
        }
        if (bit & idle) {
            if ((drives_to_remove & bit) != 0) {
                vmsfs_c_execcallback(drive, BUD_OP_UNMOUNT, 0);
                drives_to_remove &= ~bit;
            }
        }
    }
}

void buSetCompleteCallback(BU_COMPLETECALLBACK callback)
{
    vmsfs_c_setcompletecallback(callback);
}

void buSetProgressCallback(BU_PROGRESSCALLBACK callback)
{
    vmsfs_c_setprogresscallback(callback);
}

void buSetCallbackParam(Uint32 argument)
{
    vmsfs_c_setcallbackparam(argument);
}

Uint32 buGetVersion(void)
{
    return library_version;
}

Sint32 buIsReady(Sint32 drive)
{
    return vmsfs_ismount(drive);
}

Sint32 buMount(Sint32 drive)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, 0, 0, 0, 0, 0);
    vmsfs_c_setfunc(drive, BUD_OP_MOUNT);
    return 0;
}

Sint32 buConnect(Sint32 drive)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, 0, 0, 0, 0, 0);
    vmsfs_c_setfunc(drive, BUD_OP_CONNECT);
    return 0;
}

Sint32 buRemount(Sint32 drive)
{
    return buMount(drive);
}

Sint32 buUnmount(Sint32 drive)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    return vmsfs_unmount(drive);
}

Sint32 buIsFormat(Sint32 drive)
{
    return vmsfs_isformat(drive);
}

Sint32 buFormatDisk(Sint32 drive, const Uint8 *label, Sint32 icon,
                    const BUS_TIME *time, Uint32 flags)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, (Sint32)label, icon, (Sint32)time, flags, 0);
    vmsfs_c_setvolume(drive, label);
    vmsfs_c_settime(drive, time);
    vmsfs_c_setfunc(drive, BUD_OP_FORMATDISK);
    return 0;
}

Sint32 buGetDiskInfo(Sint32 drive, BUS_DISKINFO *info)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    return vmsfs_getinfo(drive, info);
}

Sint32 buGetDiskFree(Sint32 drive, Sint32 kind)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    return vmsfs_diskfree(drive, kind);
}

Sint32 buFindFirstFile(Sint32 drive, char *name)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    return vmsfs_findfirst(drive, name);
}

Sint32 buFindNextFile(Sint32 drive, char *name)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    return vmsfs_findnext(drive, name);
}

Sint32 buFindFirstFileEx(Sint32 drive, char *name, BUS_FILEINFO *info)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    return vmsfs_findfirst_ex(drive, name, info);
}

Sint32 buFindNextFileEx(Sint32 drive, char *name, BUS_FILEINFO *info)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    return vmsfs_findnext_ex(drive, name, info);
}

Sint32 buFindExecFile(Sint32 drive, char *name)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    return vmsfs_xfind(drive, name);
}

Sint32 buOpenFile(Sint32 drive, const char *name, BU_FILE **file)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    return vmsfs_fopen(drive, name, file);
}

Sint32 buCreateFile(Sint32 drive, const char *name, const BUS_TIME *time,
                    BU_FILE **file)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    return vmsfs_fcreate(drive, name, time, file);
}

Sint32 buCreateExecFile(Sint32 drive, const char *name, const BUS_TIME *time,
                        BU_FILE **file)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    return vmsfs_xcreate(drive, name, time, file);
}

Sint32 buSetFileParam(BU_FILE *file, Uint16 header, Uint8 copy)
{
    Sint32 drive;

    drive = file->drive;
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    return vmsfs_fsetparam(file, header, copy);
}

Sint32 buCloseFile(BU_FILE *file)
{
    Sint32 drive;

    drive = file->drive;
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, (Sint32)file, 1, 0, 0, 0, 0);
    vmsfs_c_setfunc(drive, BUD_OP_CLOSEFILE);
    return 0;
}

Sint32 buReadFile(BU_FILE *file, void *buffer, Sint32 blocks)
{
    Sint32 drive;

    drive = file->drive;
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, (Sint32)file, (Sint32)buffer, blocks, 0, 0, 0);
    vmsfs_c_setfunc(drive, BUD_OP_READFILE);
    return 0;
}

Sint32 buWriteFile(BU_FILE *file, const void *buffer, Sint32 blocks)
{
    Sint32 drive;

    drive = file->drive;
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, (Sint32)file, (Sint32)buffer, blocks, 0, 0, 0);
    vmsfs_c_setfunc(drive, BUD_OP_WRITEFILE);
    return 0;
}

Sint32 buDeleteFile(Sint32 drive, const char *name)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, (Sint32)name, 0, 0, 0, 0);
    vmsfs_c_setfname(drive, name, name);
    vmsfs_c_setfunc(drive, BUD_OP_DELETEFILE);
    return 0;
}

Sint32 buDeleteFileEx(Sint32 drive, const char **names, Sint32 count)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, (Sint32)names, count, 0, 0, 0);
    vmsfs_c_setfunc(drive, BUD_OP_DELETEFILEEX);
    return 0;
}

Sint32 buSetFileAttr(Sint32 drive, const char *name, Uint16 header, Uint8 copy)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, (Sint32)name, header, copy, 0, 0);
    vmsfs_c_setfname(drive, name, name);
    vmsfs_c_setfunc(drive, BUD_OP_SETFILEATTR);
    return 0;
}

Sint32 buIsExistFile(Sint32 drive, const char *name)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    return vmsfs_fisexist(drive, name);
}

Sint32 buGetFileInfo(Sint32 drive, const char *name, BUS_FILEINFO *info)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    return vmsfs_fgetinfo(drive, name, info);
}

Sint32 buStat(Sint32 drive)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_STAT_BUSY;
    }
    return 0;
}

Sint32 buGetLastError(Sint32 drive)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    return buLastError[drive];
}

Sint32 buRenameFile(Sint32 drive, const char *name, const char *new_name)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, (Sint32)name, (Sint32)new_name, 0, 0, 0);
    vmsfs_c_setfname(drive, name, new_name);
    vmsfs_c_setfunc(drive, BUD_OP_RENAMEFILE);
    return 0;
}

Sint32 buGetFileSize(Sint32 drive, const char *name)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    return vmsfs_fgetsize(drive, name);
}

Sint32 buDefragDisk(Sint32 drive, void *workspace)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, (Sint32)workspace, 0, 0, 0, 0);
    vmsfs_c_setfunc(drive, BUD_OP_DEFRAGDISK);
    return 0;
}

Sint32 buLoadFile(Sint32 drive, const char *name, void *buffer, Uint32 blocks)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, (Sint32)name, (Sint32)buffer, blocks, 0, 0);
    vmsfs_c_setfname(drive, name, name);
    vmsfs_c_setfunc(drive, BUD_OP_LOADFILE);
    return 0;
}

Sint32 buSaveFile(Sint32 drive, const char *name, const void *buffer,
                  Uint32 blocks, BUS_TIME *time, Uint32 flags)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, (Sint32)name, (Sint32)buffer, blocks,
                     (Sint32)time, flags);
    vmsfs_c_setfname(drive, name, name);
    vmsfs_c_settime(drive, time);
    vmsfs_c_setfunc(drive, BUD_OP_SAVEFILE);
    return 0;
}

Sint32 buSaveExecFile(Sint32 drive, const char *name, const void *buffer,
                      Uint32 blocks, BUS_TIME *time, Uint32 flags)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, (Sint32)name, (Sint32)buffer, blocks,
                     (Sint32)time, flags);
    vmsfs_c_setfname(drive, name, name);
    vmsfs_c_settime(drive, time);
    vmsfs_c_setfunc(drive, BUD_OP_SAVEEXECFILE);
    return 0;
}

Sint32 buLoadFileEx(Sint32 drive, const char *name, void *buffer, Uint32 first,
                    Uint32 blocks)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, (Sint32)name, (Sint32)buffer, first, blocks,
                     0);
    vmsfs_c_setfname(drive, name, name);
    vmsfs_c_setfunc(drive, BUD_OP_LOADFILEEX);
    return 0;
}

Sint32 buRewriteExecFile(Sint32 drive, const char *name, const void *buffer,
                         Uint32 first, Uint32 blocks)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, (Sint32)name, (Sint32)buffer, first, blocks,
                     0);
    vmsfs_c_setfname(drive, name, name);
    vmsfs_c_setfunc(drive, BUD_OP_REWRITEEXECFILE);
    return 0;
}

Sint32 buRewriteFile(Sint32 drive, const char *name, const void *buffer,
                     Uint32 first, Uint32 blocks)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, (Sint32)name, (Sint32)buffer, first, blocks,
                     0);
    vmsfs_c_setfname(drive, name, name);
    vmsfs_c_setfunc(drive, BUD_OP_REWRITEFILE);
    return 0;
}

Sint32 buReadDiskImage(Sint32 drive, void *buffer)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, (Sint32)buffer, 0, 0, 0, 0);
    vmsfs_c_setfunc(drive, BUD_OP_READIMAGE);
    return 0;
}

Sint32 buWriteDiskImage(Sint32 drive, const void *buffer)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, (Sint32)buffer, 0, 0, 0, 0);
    vmsfs_c_setfunc(drive, BUD_OP_WRITEIMAGE);
    return 0;
}

void buSetPrintFunc(void (*print)(const char *message))
{
    _vmsfs_dbg_out = print;
}

void buSetRetryCount(Sint32 count)
{
    vmsfs_setretry(count);
}

Sint32 buMountDisk(Sint32 drive, void *workspace, Sint32 size)
{
    /* @hack redundant: result, a local buMountDisk stores and never reads */
    Sint32 result; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    if ((result = vmsfs_setwork(drive, workspace, size)) == 0) {
        return buMount(drive);
    }
    /* @bug uninitialized: returns no value where vmsfs_setwork fails, though
     * the only caller tests R0 (TST R0,R0 at 0x8c061ad2). R0 still holds
     * vmsfs_setwork's failure there, since BF at 0x8c0c0466 goes straight to
     * the epilogue */
#ifdef NONMATCHING
    /* this side keeps the result of the bug tagged above: it returns
     * vmsfs_setwork's failure, the value R0 holds in the image */
    return result;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

Sint32 buUnmountDisk(Sint32 drive)
{
    return buUnmount(drive);
}

Sint32 buGetBankInfo(Sint32 drive, Sint32 *current, Sint32 *locked)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, (Sint32)current, (Sint32)locked, 0, 0, 0);
    vmsfs_c_setfunc(drive, BUD_OP_GETBANKINFO);
    return 0;
}

Sint32 buChangeBank(Sint32 drive, Sint32 bank)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, bank, 0, 0, 0, 0);
    vmsfs_c_setfunc(drive, BUD_OP_CHANGEBANK);
    return 0;
}

Sint32 buLockBank(Sint32 drive, Sint32 locked)
{
    if (vmsfs_c_getstate(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    vmsfs_c_setparam(drive, drive, locked, 0, 0, 0, 0);
    vmsfs_c_setfunc(drive, BUD_OP_LOCKBANK);
    return 0;
}

/* the banks drive's card is taken to have. it is BANKS where the card's
 * product name opens with BANK, and 1 where it doesn't */
static Sint32 bank_count(Sint32 drive)
{
    const KDS_DEVICEINFO *info;
    const Uint8 *name;

    info = kdGetDeviceInfo(drive_devices[drive]);
    /* @hack type: name, the product name read as unsigned bytes, where its
     * own char would do */
    name = (const Uint8 *)info->product_name;
    if (name[0] != 'B' || name[1] != 'A' || name[2] != 'N' || name[3] != 'K') {
        return 1;
    }
    return BANKS;
}

/* 1 for a card with banks, and 0 for any other */
/* @hack redundant: external linkage for a function nothing calls, where
 * static would do */
Sint32 is_multi_bank(Sint32 drive) /* NOLINT(misc-use-internal-linkage) */
{
    if (bank_count(drive) == BANKS) {
        return 1;
    }
    return 0;
}

/* 1 for a card with storage, an LCD and a timer, and 0 for any other */
/* @hack redundant: external linkage for a function nothing calls, where
 * static would do */
Sint32 is_vms(Sint32 drive) /* NOLINT(misc-use-internal-linkage) */
{
    const KDS_DEVICEINFO *info;
    Uint32 functions;

    info = kdGetDeviceInfo(drive_devices[drive]);
    /* @hack spelling: functions, a local for the mask, where the mask in
     * place would do */
    functions = KDD_FUNC_STORAGE | KDD_FUNC_LCD | KDD_FUNC_TIMER;
    if ((info->function & functions) == functions) {
        return 1;
    }
    return 0;
}
