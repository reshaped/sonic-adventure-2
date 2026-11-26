/* the card calls, over Kaede: a card's layout, its blocks and its banks */
#include <kaede.h>
#include <sg_bup.h>
#include <sg_xpt.h>

#include "vms.h"

/* the bytes of a long, the unit Kaede counts a command's data in */
#define LONG_SIZE 4

/* the offset of a reply's data from the start of recvdata */
#define REPLY_DATA 8
/* the offset of a block read's bytes, which follow the first long of the
 * reply's data */
#define REPLY_BLOCK_DATA (REPLY_DATA + LONG_SIZE)
/* the offset in the media reply's data of a system block's field. the reply
 * lays out the fields from SYS_LAST_BLOCK on as the system block does */
#define MEDIA(field) ((field) - SYS_LAST_BLOCK)

/* the size of every card's executable area, in blocks */
#define EXEC_BLOCKS 128

/* the code of the bank command, which selects, locks or reports a card's
 * bank */
#define BANK_COMMAND 31
/* a set flag's byte in the bank command and its reply. a clear flag's byte
 * is 0 */
#define BANK_FLAG_SET 0xff
/* where each field lies in a bank reply's data */
#define BANK_LOCK_AT 1 /* the lock flag */
#define BANK_LAST_AT 2 /* the number of the card's last bank */
#define BANK_CURRENT_AT 3 /* the bank in use */

/* each drive's Kaede device */
static const Sint32 drive_devices[VMS_DRIVES] = {
    KDD_DEV_A1, KDD_DEV_A2, KDD_DEV_B1, KDD_DEV_B2,
    KDD_DEV_C1, KDD_DEV_C2, KDD_DEV_D1, KDD_DEV_D2};

/* the member exports this function and this hook, though no header of the
 * SDK declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* queue the bank command for Kaede device. where set_lock is nonzero, the
 * command locks the card's bank, or unlocks it where locked is 0. where
 * set_bank is nonzero, it selects bank. the call returns the command's
 * sequence number, or a negative value where Kaede can't take it yet */
Sint32 vmsio_kd_bank_command(Sint32 device, Sint32 set_lock, Sint32 locked,
                             Sint32 set_bank, Sint32 bank);
/* null, or a hook that vmsio_block_write calls each time the card answers
 * its query for the write's error. where the hook returns nonzero, the write
 * takes *response in place of the card's response code. nothing reads what
 * the hook stores in *error */
Sint32 (*vmsio_sim_error)(Sint32 drive, Sint32 block, Uint8 *response,
                          Sint32 *error) = NULL;
/* NOLINTEND(misc-use-internal-linkage) */

void vmsio_init(void)
{
    vmsio_sim_error = NULL;
}

void vmsio_exit(void)
{
}

Sint32 vmsio_get_media_info(Sint32 drive, VMS_MEDIA *media)
{
    Sint32 device;
    Sint32 sequence;
    const KDS_DEVICE *state;
    const KDS_DEVICEINFO *info;
    KDS_STORAGEINFO storage;
    Uint8 *data;

    device = drive_devices[drive];
    state = kdGetDevice(device);
    info = kdGetDeviceInfo(device);
    if (!state->on) {
        goto absent;
    }
    if (!(info->function & KDD_FUNC_STORAGE)) {
        goto absent;
    }
    kdRecognizeChanged(device);
retry:
    for (;;) {
        /* @hack redundant: a store to state that the store after the wait
         * replaces */
        state = kdGetDevice(device);
        sequence = kdSetGetMediaInfo(device, KDD_FUNC_STORAGE, 0);
        if (sequence >= 0) {
            break;
        }
        vmsfb_sleep();
    }
    for (;;) {
        if (kdGetStatus(device, sequence)) {
            break;
        }
        vmsfb_sleep();
    }
    state = kdGetDevice(device);
    if (kdIsChanged(device)) {
        vmsfb_sleep();
        return VMSIO_CHANGED;
    }
    data = (Uint8 *)state->recvdata + REPLY_DATA;
    switch (state->response) {
    case KDD_RES_DATATRANSFER:
        break;
    case KDD_RES_TRANSMITAGAIN:
        goto retry;
    default:
        return BUD_ERR_NO_DISK;
    }
    media->last_block = _vm->read_word(data, MEDIA(SYS_LAST_BLOCK));
    media->partition = _vm->read_word(data, MEDIA(SYS_PARTITION));
    media->system_block = _vm->read_word(data, MEDIA(SYS_SYSTEM_BLOCK));
    media->fat_block = _vm->read_word(data, MEDIA(SYS_FAT_BLOCK));
    media->fat_blocks = _vm->read_word(data, MEDIA(SYS_FAT_BLOCKS));
    media->directory_block = _vm->read_word(data, MEDIA(SYS_DIRECTORY_BLOCK));
    media->directory_blocks = _vm->read_word(data, MEDIA(SYS_DIRECTORY_BLOCKS));
    media->extra_block = _vm->read_word(data, MEDIA(SYS_EXTRA_BLOCK));
    media->extra_blocks = _vm->read_word(data, MEDIA(SYS_EXTRA_BLOCKS));
    media->exec_block = _vm->read_word(data, MEDIA(SYS_EXEC_BLOCK));
    media->exec_blocks = EXEC_BLOCKS;
    media->icon = data[MEDIA(SYS_ICON)];
    media->sort = data[MEDIA(SYS_SORT)];
    media->last_byte = VMS_BLOCK_SIZE - 1;
    info = kdGetDeviceInfo(device);
    if (kdDevGetStorageInfo(&storage, info) < 0) {
        goto absent;
    }
    media->read_count = storage.readcount;
    media->write_count = storage.writecount;
    media->read_bytes = VMS_BLOCK_SIZE / storage.readcount;
    media->write_bytes = VMS_BLOCK_SIZE / storage.writecount;
    return 0;
absent:
    vmsfb_sleep();
    return BUD_ERR_NO_DISK;
}

Sint32 vmsio_block_read(Sint32 drive, Sint32 block, void *buffer)
{
    Sint32 count;
    Sint32 device;
    Sint32 sequence;
    Sint32 phase;
    Uint8 *cursor;
    const KDS_DEVICE *state;
    Uint8 *reply;
    VMS_DRIVE *record;

    record = &buVmsDrive[drive];
    if (!vmsio_ismount(drive)) {
        vmsfb_sleep();
        return BUD_ERR_NO_DISK;
    }
    device = drive_devices[drive];
    /* @hack redundant: a store to state that the store after the wait
     * replaces */
    state = kdGetDevice(device);
    cursor = buffer;
    /* @hack spelling: count, a counter stepped with phase, where phase alone
     * would do */
    for (count = 0, phase = 0; count < record->media->read_count;
         count++, phase++) {
    retry:
        for (;;) {
            if (kdIsChanged(device)) {
                return VMSIO_CHANGED;
            }
            sequence =
                kdSetBlockRead(device, KDD_FUNC_STORAGE, 0, phase, block);
            if (sequence >= 0) {
                break;
            }
            vmsfb_sleep();
        }
        for (;;) {
            if (kdGetStatus(device, sequence)) {
                break;
            }
            vmsfb_sleep();
        }
        state = kdGetDevice(device);
        if (kdIsChanged(device)) {
            return VMSIO_CHANGED;
        }
        reply = state->recvdata;
        switch (state->response) {
        case KDD_RES_DATATRANSFER:
            break;
        case KDD_RES_TRANSMITAGAIN:
            goto retry;
        case KDD_RES_FILEERROR:
            return BUD_ERR_GENERIC;
        default:
            return BUD_ERR_NO_DISK;
        }
        vmsfs_memcpyl(cursor, reply + REPLY_BLOCK_DATA,
                      record->media->read_bytes);
        cursor += record->media->read_bytes;
    }
    return 0;
}

Sint32 vmsio_block_write(Sint32 drive, Sint32 block, const void *buffer)
{
    Sint32 count;
    Sint32 device;
    Sint32 sequence;
    Sint32 phase;
    Sint32 bytes;
    const Uint8 *cursor;
    const KDS_DEVICE *state;
    /* @hack redundant: reply, a local the write stores and never reads */
    Uint8 *reply; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    VMS_DRIVE *record;
    Uint8 response;
    Sint32 error;

    record = &buVmsDrive[drive];
    if (!vmsio_ismount(drive)) {
        vmsfb_sleep();
        return BUD_ERR_NO_DISK;
    }
    device = drive_devices[drive];
    /* @hack redundant: a store to state that the store after the wait
     * replaces */
    state = kdGetDevice(device);
    cursor = buffer;
    /* @hack spelling: bytes, a local for the write's size, where
     * record->media->write_bytes in place would do */
    bytes = record->media->write_bytes;
    /* @hack spelling: count, a counter stepped with phase, where phase alone
     * would do */
    for (count = 0, phase = 0; count < record->media->write_count;
         count++, phase++) {
    retry_write:
        for (;;) {
            if (kdIsChanged(device)) {
                return VMSIO_CHANGED;
            }
            sequence = kdSetBlockWrite(device, KDD_FUNC_STORAGE, 0, phase,
                                       block, cursor, bytes / LONG_SIZE);
            if (sequence >= 0) {
                break;
            }
            vmsfb_sleep();
        }
        for (;;) {
            if (kdGetStatus(device, sequence)) {
                break;
            }
            vmsfb_sleep();
        }
        state = kdGetDevice(device);
        if (kdIsChanged(device)) {
            return VMSIO_CHANGED;
        }
        reply = state->recvdata;
        switch (state->response) {
        case KDD_RES_DEVICEREPLY:
            break;
        case KDD_RES_TRANSMITAGAIN:
            goto retry_write;
        default:
            return BUD_ERR_NO_DISK;
        }
        cursor += bytes;
    }
retry_error:
    for (;;) {
        if (kdIsChanged(device)) {
            return VMSIO_CHANGED;
        }
        sequence = kdSetGetLastError(device, KDD_FUNC_STORAGE, 0, phase, block);
        if (sequence >= 0) {
            break;
        }
        vmsfb_sleep();
    }
    for (;;) {
        if (kdGetStatus(device, sequence)) {
            break;
        }
        vmsfb_sleep();
    }
    state = kdGetDevice(device);
    if (kdIsChanged(device)) {
        return VMSIO_CHANGED;
    }
    /* @hack spelling: the hook's test nested, with the card's answer copied
     * in both arms, where one test with || would do */
    if (vmsio_sim_error != NULL) {
        if (!vmsio_sim_error(drive, block, &response, &error)) {
            response = state->response;
            /* @hack redundant: this store of the card's errcode and the
             * else's, which nothing reads */
            error = state->errcode;
        }
    } else {
        response = state->response;
        error = state->errcode;
    }
    switch (response) {
    case KDD_RES_DEVICEREPLY:
        return 0;
    case KDD_RES_TRANSMITAGAIN:
        goto retry_error;
    case KDD_RES_FILEERROR:
        return BUD_ERR_WRITE_ERROR;
    default:
        return BUD_ERR_NO_DISK;
    }
}

Sint32 vmsfs_memcmp(const Uint8 *first, const Uint8 *second)
{
    Sint32 i;
    Sint32 result;

    result = 0;
    for (i = 0; i < VMS_BLOCK_SIZE; i++, first++, second++) {
        if (*first != *second) {
            result = -1;
        }
    }
    return result;
}

Sint32 vmsio_isready(Sint32 drive)
{
    Sint32 device;
    const KDS_DEVICE *state;
    const KDS_DEVICEINFO *info;

    device = drive_devices[drive];
    state = kdGetDevice(device);
    info = kdGetDeviceInfo(device);
    if (!state->on) {
        return 0;
    }
    if (!(info->function & KDD_FUNC_STORAGE)) {
        return 0;
    }
    return 1;
}

Sint32 vmsio_ismount(Sint32 drive)
{
    Sint32 device;
    const KDS_DEVICE *state;
    const KDS_DEVICEINFO *info;

    device = drive_devices[drive];
    state = kdGetDevice(device);
    info = kdGetDeviceInfo(device);
    if (!state->on) {
        return 0;
    }
    if (kdIsChanged(device)) {
        return 0;
    }
    if (!(info->function & KDD_FUNC_STORAGE)) {
        return 0;
    }
    return 1;
}

Sint32 vmsio_kd_bank_command(Sint32 device, Sint32 set_lock, Sint32 locked,
                             Sint32 set_bank, Sint32 bank)
{
    Uint8 command[LONG_SIZE];
    Uint8 *cursor;

    /* @hack spelling: the command's bytes stored through a cursor, where
     * command[0] to command[3] would do */
    cursor = command;
    *cursor++ = set_lock ? BANK_FLAG_SET : 0;
    *cursor++ = locked ? BANK_FLAG_SET : 0;
    *cursor++ = set_bank ? BANK_FLAG_SET : 0;
    *cursor++ = bank;
    return kdSetUniCommand(device, KDD_FUNC_STORAGE, BANK_COMMAND, command,
                           sizeof command / LONG_SIZE);
}

Sint32 vmsio_bank_change(Sint32 drive, Sint32 bank)
{
    Sint32 device;
    Sint32 sequence;
    const KDS_DEVICE *state;
    const KDS_DEVICEINFO *info;
    const char *data;
    Sint32 current;

    device = drive_devices[drive];
    state = kdGetDevice(device);
    info = kdGetDeviceInfo(device);
    if (!state->on) {
        goto absent;
    }
    if (!(info->function & KDD_FUNC_STORAGE)) {
        goto absent;
    }
    kdRecognizeChanged(device);
    /* @hack spelling: data, set to the product name for its test, where
     * info->product_name in place would do */
    data = info->product_name;
    if (data[0] != 'B' || data[1] != 'A' || data[2] != 'N' || data[3] != 'K') {
        return 0;
    }
retry:
    for (;;) {
        /* @hack redundant: a store to state that the store after the wait
         * replaces */
        state = kdGetDevice(device);
        sequence = vmsio_kd_bank_command(device, 0, 0, 1, bank);
        if (sequence >= 0) {
            break;
        }
        vmsfb_sleep();
    }
    for (;;) {
        if (kdGetStatus(device, sequence)) {
            break;
        }
        vmsfb_sleep();
    }
    state = kdGetDevice(device);
    if (kdIsChanged(device)) {
        vmsfb_sleep();
        return VMSIO_CHANGED;
    }
    data = (const char *)state->recvdata + REPLY_DATA;
    switch (state->response) {
    case KDD_RES_DATATRANSFER:
        current = data[BANK_CURRENT_AT];
        break;
    case KDD_RES_TRANSMITAGAIN:
        goto retry;
    case KDD_RES_FILEERROR:
        return BUD_ERR_LOCKED;
    default:
        return BUD_ERR_NO_DISK;
    }
    if (current != bank) {
        return BUD_ERR_GENERIC;
    }
    return 0;
absent:
    vmsfb_sleep();
    return BUD_ERR_NO_DISK;
}

Sint32 vmsio_bank_lock(Sint32 drive, Sint32 locked)
{
    Sint32 device;
    Sint32 sequence;
    const KDS_DEVICE *state;
    const KDS_DEVICEINFO *info;
    const char *data;
    Sint32 lock;

    device = drive_devices[drive];
    state = kdGetDevice(device);
    info = kdGetDeviceInfo(device);
    if (!state->on) {
        goto absent;
    }
    if (!(info->function & KDD_FUNC_STORAGE)) {
        goto absent;
    }
    kdRecognizeChanged(device);
    /* @hack spelling: data, set to the product name for its test, where
     * info->product_name in place would do */
    data = info->product_name;
    if (data[0] != 'B' || data[1] != 'A' || data[2] != 'N' || data[3] != 'K') {
        return 0;
    }
retry:
    for (;;) {
        /* @hack redundant: a store to state that the store after the wait
         * replaces */
        state = kdGetDevice(device);
        sequence = vmsio_kd_bank_command(device, 1, locked, 0, 0);
        if (sequence >= 0) {
            break;
        }
        vmsfb_sleep();
    }
    for (;;) {
        if (kdGetStatus(device, sequence)) {
            break;
        }
        vmsfb_sleep();
    }
    state = kdGetDevice(device);
    if (kdIsChanged(device)) {
        vmsfb_sleep();
        return VMSIO_CHANGED;
    }
    data = (const char *)state->recvdata + REPLY_DATA;
    switch (state->response) {
    case KDD_RES_DATATRANSFER:
        lock = (Uint8)data[BANK_LOCK_AT];
        break;
    case KDD_RES_TRANSMITAGAIN:
        goto retry;
    case KDD_RES_FILEERROR:
        return BUD_ERR_GENERIC;
    default:
        return BUD_ERR_NO_DISK;
    }
    /* @hack spelling: locked overwritten with its flag's byte, where lock
     * compared with that byte would do */
    locked = locked ? BANK_FLAG_SET : 0;
    if (lock != locked) {
        return BUD_ERR_GENERIC;
    }
    return 0;
absent:
    vmsfb_sleep();
    return BUD_ERR_NO_DISK;
}

Sint32 vmsio_bank_info(Sint32 drive, Sint32 *current, Sint32 *locked,
                       Sint32 *count)
{
    Sint32 device;
    Sint32 sequence;
    const KDS_DEVICE *state;
    const KDS_DEVICEINFO *info;
    const char *data;
    Sint32 bank;
    Sint32 lock;
    Sint32 banks;

    device = drive_devices[drive];
    state = kdGetDevice(device);
    info = kdGetDeviceInfo(device);
    if (!state->on) {
        goto absent;
    }
    if (!(info->function & KDD_FUNC_STORAGE)) {
        goto absent;
    }
    kdRecognizeChanged(device);
    /* @hack spelling: data, set to the product name for its test, where
     * info->product_name in place would do */
    data = info->product_name;
    if (data[0] != 'B' || data[1] != 'A' || data[2] != 'N' || data[3] != 'K') {
        if (current != NULL) {
            *current = 0;
        }
        if (locked != NULL) {
            *locked = 0;
        }
        if (count != NULL) {
            *count = 1;
        }
        return 0;
    }
retry:
    for (;;) {
        /* @hack redundant: a store to state that the store after the wait
         * replaces */
        state = kdGetDevice(device);
        sequence = vmsio_kd_bank_command(device, 0, 0, 0, 0);
        if (sequence >= 0) {
            break;
        }
        vmsfb_sleep();
    }
    for (;;) {
        if (kdGetStatus(device, sequence)) {
            break;
        }
        vmsfb_sleep();
    }
    state = kdGetDevice(device);
    if (kdIsChanged(device)) {
        vmsfb_sleep();
        return VMSIO_CHANGED;
    }
    data = (const char *)state->recvdata + REPLY_DATA;
    switch (state->response) {
    case KDD_RES_DATATRANSFER:
        lock = data[BANK_LOCK_AT] ? 1 : 0;
        banks = data[BANK_LAST_AT] + 1;
        bank = data[BANK_CURRENT_AT];
        break;
    case KDD_RES_TRANSMITAGAIN:
        goto retry;
    case KDD_RES_FILEERROR:
        return BUD_ERR_GENERIC;
    default:
        return BUD_ERR_NO_DISK;
    }
    if (current != NULL) {
        *current = bank;
    }
    if (locked != NULL) {
        *locked = lock;
    }
    if (count != NULL) {
        *count = banks;
    }
    return 0;
absent:
    vmsfb_sleep();
    return BUD_ERR_NO_DISK;
}
