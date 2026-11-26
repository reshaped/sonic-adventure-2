/* the card calls, each tried again after a failure as many times as
 * vmsfs_setretry sets */
#include <sg_bup.h>
#include <sg_xpt.h>

#include "vms.h"

/* the member exports these data, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* where set, a try that finds the card changed ends its call with
 * BUD_ERR_NO_DISK */
const Sint32 _vmsfs_strictCheck = 1;
/* how many more tries a call makes after a failure, or -1 for tries till it
 * succeeds */
Sint32 _vmsfs_retryCount;
/* NOLINTEND(misc-use-internal-linkage) */

Sint32 vmsfs_setretry(Sint32 count)
{
    return _vmsfs_retryCount = count;
}

Sint32 vmsfs_mediainfo_retry(Sint32 drive, VMS_MEDIA *media)
{
    Sint32 result;
    Sint32 left;
    VMS_DRIVE *record;
    VMS_COMMAND *command;

    left = _vmsfs_retryCount;
    do {
        result = vmsio_get_media_info(drive, media);
        if (result == 0) {
            break;
        }
        if (_vmsfs_strictCheck && result == VMSIO_CHANGED) {
            return BUD_ERR_NO_DISK;
        }
        record = &buVmsDrive[drive];
        record->progress_total++;
    } while (left-- != 0 || _vmsfs_retryCount == -1);
    record = &buVmsDrive[drive];
    command = &buVmsCmd[drive];
    vmsfs_c_progress(drive, command->operation, record->progress_done++,
                     record->progress_total);
    switch (result) {
    case 0:
        return 0;
    case BUD_ERR_NO_DISK:
        return BUD_ERR_NO_DISK;
    default:
        return BUD_ERR_GENERIC;
    }
}

Sint32 vmsfs_read_retry(Sint32 drive, Sint32 block, void *buffer)
{
    Sint32 result;
    Sint32 left;
    VMS_DRIVE *record;
    VMS_COMMAND *command;

    left = _vmsfs_retryCount;
    do {
        result = vmsio_block_read(drive, block, buffer);
        if (result == 0) {
            break;
        }
        if (_vmsfs_strictCheck && result == VMSIO_CHANGED) {
            return BUD_ERR_NO_DISK;
        }
        record = &buVmsDrive[drive];
        record->progress_total++;
    } while (left-- != 0 || _vmsfs_retryCount == -1);
    record = &buVmsDrive[drive];
    command = &buVmsCmd[drive];
    vmsfs_c_progress(drive, command->operation, record->progress_done++,
                     record->progress_total);
    switch (result) {
    case 0:
        return 0;
    case BUD_ERR_NO_DISK:
        return BUD_ERR_NO_DISK;
    default:
        return BUD_ERR_GENERIC;
    }
}

Sint32 vmsfs_write_retry(Sint32 drive, Sint32 block, const void *buffer)
{
    Sint32 result;
    Sint32 left;
    VMS_DRIVE *record;
    VMS_COMMAND *command;

    left = _vmsfs_retryCount;
    do {
        result = vmsio_block_write(drive, block, buffer);
        if (result == 0) {
            break;
        }
        if (_vmsfs_strictCheck && result == VMSIO_CHANGED) {
            return BUD_ERR_NO_DISK;
        }
        record = &buVmsDrive[drive];
        record->progress_total++;
    } while (left-- != 0 || _vmsfs_retryCount == -1);
    record = &buVmsDrive[drive];
    command = &buVmsCmd[drive];
    vmsfs_c_progress(drive, command->operation, record->progress_done++,
                     record->progress_total);
    switch (result) {
    case 0:
        return 0;
    case BUD_ERR_NO_DISK:
        return BUD_ERR_NO_DISK;
    case BUD_ERR_WRITE_ERROR:
        return BUD_ERR_WRITE_ERROR;
    default:
        return BUD_ERR_GENERIC;
    }
}

Sint32 vmsfs_bank_change_retry(Sint32 drive, Sint32 bank)
{
    Sint32 result;
    Sint32 left;

    left = _vmsfs_retryCount;
    do {
        result = vmsio_bank_change(drive, bank);
        if (result == 0) {
            break;
        }
        if (_vmsfs_strictCheck && result == VMSIO_CHANGED) {
            return BUD_ERR_NO_DISK;
        }
    } while (left-- != 0 || _vmsfs_retryCount == -1);
    switch (result) {
    case 0:
        return 0;
    case BUD_ERR_NO_DISK:
        return BUD_ERR_NO_DISK;
    case BUD_ERR_LOCKED:
        return BUD_ERR_LOCKED;
    default:
        return BUD_ERR_GENERIC;
    }
}

Sint32 vmsfs_bank_lock_retry(Sint32 drive, Sint32 locked)
{
    Sint32 result;
    Sint32 left;

    left = _vmsfs_retryCount;
    do {
        result = vmsio_bank_lock(drive, locked);
        if (result == 0) {
            break;
        }
        if (_vmsfs_strictCheck && result == VMSIO_CHANGED) {
            return BUD_ERR_NO_DISK;
        }
    } while (left-- != 0 || _vmsfs_retryCount == -1);
    switch (result) {
    case 0:
        return 0;
    case BUD_ERR_NO_DISK:
        return BUD_ERR_NO_DISK;
    default:
        return BUD_ERR_GENERIC;
    }
}

Sint32 vmsfs_bank_info_retry(Sint32 drive, Sint32 *current, Sint32 *locked,
                             Sint32 *count)
{
    Sint32 result;
    Sint32 left;

    left = _vmsfs_retryCount;
    do {
        result = vmsio_bank_info(drive, current, locked, count);
        if (result == 0) {
            break;
        }
        if (_vmsfs_strictCheck && result == VMSIO_CHANGED) {
            return BUD_ERR_NO_DISK;
        }
    } while (left-- != 0 || _vmsfs_retryCount == -1);
    switch (result) {
    case 0:
        return 0;
    case BUD_ERR_NO_DISK:
        return BUD_ERR_NO_DISK;
    default:
        return BUD_ERR_GENERIC;
    }
}
