/* the card file system. a card's last block is its system block. files take
 * the user blocks, those below extra_block. each drive's workspace caches the
 * card's system block, FAT and directory */
#include <sg_bup.h>
#include <sg_xpt.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "vms.h"

/* the most files open at once, over every drive */
#define FILES 8

/* the last block of a card of each capacity class, 0 to 3 */
#define CLASS_0_LAST_BLOCK 255
#define CLASS_1_LAST_BLOCK 511
#define CLASS_2_LAST_BLOCK 1023
#define CLASS_3_LAST_BLOCK 2047
/* the blocks a drive's caches take for a card of each class. each counts the
 * system block, and the class's FAT and directory blocks. vmsfs_init's
 * capacity names a class by one of them */
#define CLASS_0_CACHE_BLOCKS 15
#define CLASS_1_CACHE_BLOCKS 33
#define CLASS_2_CACHE_BLOCKS 65
#define CLASS_3_CACHE_BLOCKS 122

/* the bytes of a FAT entry */
#define FAT_ENTRY_SIZE 2
/* a directory block holds 1 << DIR_ENTRY_SHIFT entries */
#define DIR_ENTRY_SHIFT 4

/* vmsfs_init's count for vmsfs_setretry: the tries each retried call makes
 * after a failure */
#define RETRIES 10

/* a drive's workspace opens with VMS_GUARD_SIZE bytes of 'G'. its system
 * block, FAT and directory caches follow in turn. each cache's first
 * MARK_SIZE bytes start as its letter: 'S', 'F' or 'D' */
#define MARK_SIZE 32

/* a directory entry's kinds: an empty entry, a data file, and an executable
 * file */
#define KIND_EMPTY 0
#define KIND_DATA 0x33
#define KIND_EXEC 0xcc

/* a directory entry as the card holds it. its 16-bit words are in the
 * card's byte order, so only _vm->read_word and _vm->write_word reach them */
struct dir_entry {
    Uint8 kind;
    Uint8 copy;
    Uint16 first_block;
    char name[VMS_NAME_SIZE];
    Uint8 time[VMS_TIME_SIZE];
    Uint16 blocks;
    Uint16 header; /* the offset of the file's header in the file, in blocks */
    Uint8 unk_1c[4];
};

/* the offsets of struct dir_entry's words, which _vm->read_word and
 * _vm->write_word take */
#define DIR_FIRST_BLOCK 2
#define DIR_BLOCKS 24
#define DIR_HEADER 26

/* the offsets of struct dir_entry's byte fields, for the tagged copies that
 * reach a field by offset */
#define DIR_COPY 1
#define DIR_NAME 4
#define DIR_TIME 16

/* a block's FAT entry holds the next block of its chain, or one of these.
 * FAT_LAST also marks the system block and each block of the FAT */
#define FAT_FREE 0xfffc /* a free block */
#define FAT_LAST 0xfffa /* a chain's last block */
#define FAT_BAD 0xffff /* a block whose write failed */

/* the first_block of a file record that has no block yet */
#define NO_BLOCK 0xffff

/* the header offsets, in blocks, that a new data file and a new executable
 * file take */
#define DATA_HEADER 0
#define EXEC_HEADER 1

/* the states vmsfs_lockbank sets a card's bank to */
#define BANK_UNLOCKED 0
#define BANK_LOCKED 1

/* a formatted card's system block opens with SYSTEM_MAGIC_SIZE bytes of
 * SYSTEM_MAGIC */
#define SYSTEM_MAGIC 0x55
#define SYSTEM_MAGIC_SIZE 16

/* start drive's progress at step 1 of total */
#define PROGRESS_START(drive, total)                                           \
    {                                                                          \
        VMS_DRIVE *progress_record = &buVmsDrive[drive];                       \
        /* @hack spelling: the comma that joins the two stores, where two      \
         * statements would do */                                              \
        progress_record->progress_done = 1,                                    \
        progress_record->progress_total = (total);                             \
    }
/* the steps that operation, a BUD_OP_ value, takes on a card of the drive's
 * capacity class. it reads the record PROGRESS_START declares, so it serves
 * only in PROGRESS_START's total */
#define PROGRESS_STEPS(operation)                                              \
    buProgMaxTbl[progress_record->capacity_class][operation]
/* report drive's next step to its progress callback, and count it done */
#define PROGRESS_STEP(drive)                                                   \
    {                                                                          \
        VMS_DRIVE *progress_record = &buVmsDrive[drive];                       \
        VMS_COMMAND *command = &buVmsCmd[drive];                               \
        vmsfs_c_progress(drive, command->operation,                            \
                         progress_record->progress_done++,                     \
                         progress_record->progress_total);                     \
    }
/* a format that erases no block takes 1 / NO_ERASE_DIVISOR of a format's
 * progress steps */
#define NO_ERASE_DIVISOR 10
/* the steps a verified save takes for each block: its write and its read
 * back */
#define VERIFY_STEPS 2

static Sint32 fat_find_system(Sint32 drive, Uint16 value);
static Sint32 fat_find_in(Sint32 drive, Uint16 value, Sint32 first,
                          Sint32 last);
static Sint32 drop_file(Sint32 drive, const char *name);
static Sint32 first_used_block(Sint32 drive);
static Sint32 last_free_block(Sint32 drive, Sint32 first);
static struct dir_entry *entry_starting_at(Sint32 drive, Sint32 block);
static Sint32 check_links(Sint32 drive);
static Sint32 cache_dir(Sint32 drive);
static Sint32 rechain_dir(Sint32 drive);
static Sint32 flush_dir(Sint32 drive);
static Sint32 name_length(const char *name);
static Sint32 name_equal(const char *first, const char *second);
static void unpack_entry(struct dir_entry *entry, VMS_DIRINFO *output);
static Sint32 find_file(Sint32 drive, const char *name, VMS_DIRINFO *output);
static Sint32 find_exec_file(Sint32 drive, VMS_DIRINFO *output);
static Sint32 find_free_slot(Sint32 drive);
static Sint32 read_sys(Sint32 drive);
static Sint32 write_sys(Sint32 drive);
static Sint32 read_fat(Sint32 drive);
static Sint32 write_fat_once(Sint32 drive);
static Sint32 find_fat_run(Sint32 drive);
static Sint32 write_fat(Sint32 drive);
static Sint32 fat_find_user(Sint32 drive, Uint16 value);
static Sint32 find_free_block(Sint32 drive);
static Sint32 unclaimed_block(Sint32 drive, Uint16 block);
static void fat_store(Sint32 drive, Sint32 index, Uint16 value);
static Uint16 fat_fetch(Sint32 drive, Sint32 index);
static Sint32 load_caches(Sint32 drive);
static struct dir_entry *dir_slot(Sint32 drive, Sint32 index);
static void dir_mark_unsaved(Sint32 drive, Sint32 index);
static BU_FILE *alloc_file(Sint32 drive);
static void free_file(BU_FILE *file);
static Sint32 check_name(Sint32 drive, const char *name);
static Sint32 check_chain(Sint32 drive, const char *name);
static Uint8 pack_bcd(Uint32 value);
static Uint16 unpack_bcd(Uint32 value);
static void pack_time(Uint8 *output, const BUS_TIME *time);
static void unpack_time(BUS_TIME *time, const Uint8 *input);
static Sint32 check_media(VMS_DRIVE *record);

VMS_CONTEXT buVmsContext;
VMS_CONTEXT *_vm;
VMS_DRIVE buVmsDrive[VMS_DRIVES];
void (*_vmsfs_dbg_out)(const char *message) = NULL;

/* the member exports these functions and data, though no header of the SDK
 * declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* format a message as sprintf does, and print it with _vmsfs_dbg_out where
 * that isn't null. the message and its null must fit MESSAGE_SIZE bytes,
 * since vsprintf doesn't check */
void vmsfs_dpf(char *format, ...);
/* make _vm read and write the card's words big-endian where byte_order's low
 * bit is 1. where the bit is 0, they are little-endian */
void vmsfs_setdevicetype(Sint32 byte_order);
/* vmsfs_init's setups of the drives, without a workspace and with one. each
 * returns 0, or BUD_ERR_INVALID_PARAM for a capacity of no class.
 *
 * vmsfs_init_work_normal enables each drive whose bit enabled_drives sets. it
 * lays out each such drive's guard and caches in workspace, in drive order.
 * vmsfs_init_work_new enables no drive */
Sint32 vmsfs_init_work_new(Sint32 capacity, Sint32 enabled_drives,
                           void *workspace);
Sint32 vmsfs_init_work_normal(Sint32 capacity, Sint32 enabled_drives,
                              void *workspace);
/* clear drive's record, and point its media back at media_table's entry for
 * the drive. it returns 0 */
Sint32 vmsfs_resetwork(Sint32 drive);
/* vmsfs_mount where each drive takes its own workspace: the read of the
 * caches alone */
Sint32 vmsfs_mount2(Sint32 drive);
/* do nothing, and return 0 */
Sint32 vmsfs_stat(void);

/* each call below checks its drive first, as vms.h's file system calls do,
 * unless its comment says otherwise. vmsfs_fread1, vmsfs_foverwrite1,
 * vmsfs_fwrite1 and vmsfs_xwrite1 skip only the test for an enabled drive */

/* the count of FAT entries that hold FAT_FREE, outside the user area too */
Sint32 vmsfs_sysdiskfree(Sint32 drive);
/* copy the card's volume label, VMS_LABEL_SIZE bytes without a null, to
 * output. it returns 0 */
Sint32 vmsfs_getvolume(Sint32 drive, char *output);
/* make the VMS_LABEL_SIZE bytes at label the card's volume label, and write
 * the system block. it returns 0 or the write's failure */
Sint32 vmsfs_setvolume(Sint32 drive, const char *label);
/* vmsfs_fsettime sets the file's time, and writes the directory.
 * vmsfs_fgettime stores the file's time in *time. each returns 0, or
 * BUD_ERR_FILE_NOT_FOUND where the card has no file of the name.
 * vmsfs_fsettime can also return the directory write's failure */
Sint32 vmsfs_fsettime(Sint32 drive, const char *name, const BUS_TIME *time);
Sint32 vmsfs_fgettime(Sint32 drive, const char *name, BUS_TIME *time);
/* vmsfs_fread1 reads the file's next block into buffer, and
 * vmsfs_foverwrite1 writes buffer over that block. each returns the blocks
 * left after it, or 0 once none is left. each failure is negative.
 *
 * each returns the read's or the write's failure. each returns
 * BUD_ERR_GENERIC where the chain ends before the record's count of blocks
 * left runs out. each returns BUD_ERR_FILE_BROKEN where the chain meets a
 * free or bad block */
Sint32 vmsfs_fread1(BU_FILE *file, void *buffer);
Sint32 vmsfs_foverwrite1(BU_FILE *file, const void *buffer);
/* write buffer as a data file's next block, in the highest free user block
 * no file starts at. it returns 0, or the first write's BUD_ERR_NO_DISK.
 *
 * each block whose write fails otherwise is marked bad. the highest free user
 * block left then takes its place. it returns BUD_ERR_DISK_FULL where no free
 * user block is left. a card that leaves after such a failure gives
 * BUD_ERR_DISK_FULL too */
Sint32 vmsfs_fwrite1(BU_FILE *file, const void *buffer);
/* write buffer as the next block of the executable file. the block's number
 * is the count of the file's blocks written so far. it returns 0 or the
 * write's failure. it returns BUD_ERR_DISK_FULL where that block isn't free
 * or lies past the executable area */
Sint32 vmsfs_xwrite1(BU_FILE *file, const void *buffer);

/* store the file's chain in chain. chain takes the file's first block, then
 * each of its blocks' FAT entries in turn. chain must have room for one entry
 * more than the file has blocks. the call checks no drive.
 *
 * it returns the index of the file's directory entry, or
 * BUD_ERR_FILE_NOT_FOUND. it returns BUD_ERR_FILE_BROKEN for a broken chain,
 * after it has filled chain */
Sint32 vmsfs_scanfile(Sint32 drive, const char *name, Uint16 *chain);
/* skip the first count blocks of a file. the next read or overwrite then
 * takes the block after them. no read, overwrite or seek may have moved the
 * file since its open. the seek starts from the file's first block, but
 * counts down the blocks left from where they stand. origin is unused, and
 * the call checks no drive.
 *
 * it returns the blocks left, which is 0 where count reaches or passes the
 * file's end. it returns BUD_ERR_GENERIC where the chain and the record's
 * count of blocks left disagree */
Sint32 vmsfs_fseek(BU_FILE *file, Uint32 count, Sint32 origin);
/* vmsfs_save_file and vmsfs_save_exec_file with BUD_FLAG_VERIFY, without
 * their test for free blocks */
Sint32 vmsfs_save_file_verify(Sint32 drive, const char *name,
                              const void *buffer, Uint32 block_count,
                              BUS_TIME *timestamp, Sint32 flags);
Sint32 vmsfs_save_exec_file_verify(Sint32 drive, const char *name,
                                   const void *buffer, Uint32 block_count,
                                   BUS_TIME *timestamp, Sint32 flags);
/* where it isn't 0, vmsfs_diskfree skips a free user block that a file's entry
 * names as its first */
const Sint32 buRealFree = 0;
/* where it isn't 0, vmsfs_fsetattr writes a file's header offset */
const Uint32 buEnableHeaderOffset = 0;
/* buCapsFats holds the FAT's blocks on a card of each capacity class, 0 to 3.
 * buCapsDirs holds the directory's */
Sint32 buCapsFats[] = {1, 2, 4, 8};
Sint32 buCapsDirs[] = {13, 30, 60, 113};
/* buProgMaxTbl's columns, for the BUD_OP_ values from 0 to 23 */
#define PROGRESS_COLUMNS 24
/* the progress steps a command takes on a card of each capacity class, by the
 * command's BUD_OP_ value. each column past BUD_OP_DELETEFILEEX holds 0, so
 * vmsfs_connect's total is 0 */
Sint32 buProgMaxTbl[][PROGRESS_COLUMNS] = {
    {0, 17, 0, 285, 2, 0, 0, 2, 2, 0, 2, 0, 2, 2, 0, 256, 256, 2},
    {0, 30, 0, 570, 3, 0, 0, 3, 2, 0, 2, 0, 3, 3, 0, 512, 512, 2},
    {0, 57, 0, 1140, 2, 0, 0, 5, 2, 0, 2, 0, 5, 5, 0, 1024, 1024, 2},
    {0, 124, 0, 2295, 9, 0, 0, 9, 2, 0, 9, 0, 9, 9, 0, 2048, 2048, 9},
};
/* 1 for a drive whose FAT or directory read gave BUD_ERR_GENERIC when its
 * caches last loaded. it is 0 for any other drive */
Sint32 buIllegalFormat[VMS_DRIVES];
/* the bytes of each drive's verify buffer, room for four blocks */
#define VERIFY_SIZE 2048
/* each drive's buffer for a save's verify. the verify reads the saved blocks
 * back into its first block, one at a time */
Uint32 _vms_verify_buf[VMS_DRIVES][VERIFY_SIZE / sizeof(Uint32)];
/* NOLINTEND(misc-use-internal-linkage) */

/* each drive's card layout, which its record's media points to */
static VMS_MEDIA media_table[VMS_DRIVES];
static BU_FILE files[FILES];
/* the count of records in files that alloc_file searches. vmsfs_init sets it
 * to FILES */
static Sint32 file_limit;
/* vmsfs_fopen stores find_file's result here, the index of the file's
 * directory entry or -1. alloc_file stores BUD_ERR_CANNOT_OPEN where every
 * record is open */
static Sint32 dat_8c276a10;
/* the count of drives whose bits vmsfs_init's enabled_drives sets */
static Sint32 drive_count;
/* each drive's block of zeros, which a format writes to each block it
 * erases */
static Sint32 blank_blocks[VMS_DRIVES][VMS_BLOCK_SIZE / sizeof(Sint32)];
/* the bytes of vmsfs_dpf's buffer */
#define MESSAGE_SIZE 512
static char dpf_buffer[MESSAGE_SIZE];
/* the buffer vmsfs_connect reads a card's block 0 into, to test the card */
static Sint32 probe_block[VMS_BLOCK_SIZE / sizeof(Sint32)];

void vmsfs_dpf(char *format, ...)
{
    va_list arguments;

    va_start(arguments, format);
    vsprintf(dpf_buffer, format, arguments);
    if (_vmsfs_dbg_out) {
        _vmsfs_dbg_out(dpf_buffer);
    }
}

void vmsfs_setdevicetype(Sint32 byte_order)
{
    byte_order &= 1;
    _vm->byte_order = byte_order;
    if (byte_order == 1) {
        _vm->read_word = vmsfs_getword_big;
        _vm->write_word = vmsfs_setword_big;
    } else {
        _vm->read_word = vmsfs_getword_little;
        _vm->write_word = vmsfs_setword_little;
    }
}

Sint32 vmsfs_init(Sint32 capacity, Sint32 enabled_drives, void *workspace)
{
    Sint32 drive;

    /* @hack redundant: unused_locals, locals that only size the frame */
    Sint32 unused_locals[4];
    Sint32 bit;

    _vm = &buVmsContext;
    for (drive = 0; drive < VMS_DRIVES; drive++) {
        buIllegalFormat[drive] = 0;
    }
    memset(&buVmsContext, 0, sizeof(buVmsContext));
    memset(buVmsDrive, 0, sizeof(buVmsDrive));
    vmsfs_setdevicetype(0);
    drive_count = 0;
    for (drive = 0, bit = 1; drive < VMS_DRIVES; drive++, bit <<= 1) {
        if (enabled_drives & bit) {
            drive_count++;
        }
    }
    if (workspace) {
        vmsfs_init_work_normal(capacity, enabled_drives, workspace);
        _vm->separate_workspaces = 0;
    } else {
        vmsfs_init_work_new(capacity, enabled_drives, workspace);
        _vm->separate_workspaces = 1;
    }
    file_limit = FILES;
    dat_8c276a10 = 0;
    vmsfs_setretry(RETRIES);
    memset(files, 0, sizeof(BU_FILE) * file_limit);
    vmsio_init();
    return 0;
}

Sint32 vmsfs_init_work_new(Sint32 capacity, Sint32 enabled_drives,
                           void *workspace)
{
    switch (capacity) {
    case CLASS_0_CACHE_BLOCKS:
        _vm->cache_class = 0;
        break;
    case CLASS_1_CACHE_BLOCKS:
        _vm->cache_class = 1;
        break;
    case CLASS_2_CACHE_BLOCKS:
        _vm->cache_class = 2;
        break;
    case CLASS_3_CACHE_BLOCKS:
        _vm->cache_class = 3;
        break;
    default:
        return BUD_ERR_INVALID_PARAM;
    }
    return 0;
}

Sint32 vmsfs_init_work_normal(Sint32 capacity, Sint32 enabled_drives,
                              void *workspace)
{
    Sint32 drive;
    Uint8 *cursor;
    Sint32 workspace_bytes;
    Sint32 fat_blocks;
    Sint32 directory_blocks;
    Sint32 bit;
    VMS_DRIVE *record = buVmsDrive;

    switch (capacity) {
    case CLASS_0_CACHE_BLOCKS:
        _vm->cache_class = 0;
        fat_blocks = buCapsFats[0];
        directory_blocks = buCapsDirs[0];
        break;
    case CLASS_1_CACHE_BLOCKS:
        _vm->cache_class = 1;
        fat_blocks = buCapsFats[1];
        directory_blocks = buCapsDirs[1];
        break;
    case CLASS_2_CACHE_BLOCKS:
        _vm->cache_class = 2;
        fat_blocks = buCapsFats[2];
        directory_blocks = buCapsDirs[2];
        break;
    case CLASS_3_CACHE_BLOCKS:
        _vm->cache_class = 3;
        fat_blocks = buCapsFats[3];
        directory_blocks = buCapsDirs[3];
        break;
    default:
        return BUD_ERR_INVALID_PARAM;
    }
    cursor = workspace;
    workspace_bytes = (capacity * VMS_BLOCK_SIZE) + VMS_GUARD_SIZE;
    for (drive = 0, bit = 1; drive < VMS_DRIVES; drive++, bit <<= 1, record++) {
        if (!(enabled_drives & bit)) {
            continue;
        }
        record->enabled = 1;
        record->drive = drive;
        record->media = &media_table[drive];
        record->system_cached = 0;
        record->fat_cached = 0;
        record->directory_cached = 0;
        record->guard = cursor;
        record->system_cache = cursor + VMS_GUARD_SIZE;
        record->fat_cache = cursor + VMS_BLOCK_SIZE + VMS_GUARD_SIZE;
        record->directory_cache =
            cursor + ((fat_blocks + 1) * VMS_BLOCK_SIZE) + VMS_GUARD_SIZE;
        record->mounted = 0;
        record->connected = 0;
        record->system_capacity = 1;
        record->fat_capacity = fat_blocks;
        record->directory_capacity = directory_blocks;
        record->guard_size = VMS_GUARD_SIZE;
        memset(record->unk_38, 0, sizeof(record->unk_38));
        memset(record->dir_unsaved, 0, sizeof(record->dir_unsaved));
        memset(record->guard, 'G', VMS_GUARD_SIZE);
        memset(record->system_cache, 'S', MARK_SIZE);
        memset(record->fat_cache, 'F', MARK_SIZE);
        memset(record->directory_cache, 'D', MARK_SIZE);
        cursor += workspace_bytes;
    }
    return 0;
}

Sint32 vmsfs_resetwork(Sint32 drive)
{
    VMS_DRIVE *record;

    record = &buVmsDrive[drive];
    memset(record, 0, sizeof(VMS_DRIVE));
    record->media = &media_table[drive];
    return 0;
}

Sint32 vmsfs_setwork(Sint32 drive, void *workspace, Sint32 size)
{

    /* @hack redundant: unused_local, a local that only sizes the frame */
    Sint32 unused_local;
    Uint8 *cursor;
    Sint32 fat_blocks;
    Sint32 directory_blocks;
    Sint32 capacity_class;
    Sint32 needed;
    VMS_DRIVE *record;

    if (_vm->separate_workspaces == 0) {
        return BUD_ERR_INVALID_PARAM;
    }
    if (!workspace) {
        return BUD_ERR_INVALID_PARAM;
    }
    record = &buVmsDrive[drive];
    capacity_class = record->capacity_class;
    switch (capacity_class) {
    case 0:
        fat_blocks = buCapsFats[0];
        directory_blocks = buCapsDirs[0];
        break;
    case 1:
        fat_blocks = buCapsFats[1];
        directory_blocks = buCapsDirs[1];
        break;
    case 2:
        fat_blocks = buCapsFats[2];
        directory_blocks = buCapsDirs[2];
        break;
    case 3:
        fat_blocks = buCapsFats[3];
        directory_blocks = buCapsDirs[3];
        break;
    default:
        return BUD_ERR_INVALID_PARAM;
    }
    needed =
        ((fat_blocks + directory_blocks + 1) * VMS_BLOCK_SIZE) + VMS_GUARD_SIZE;
    if (needed > size) {
        return BUD_ERR_INVALID_PARAM;
    }
    cursor = workspace;
    record->enabled = 1;
    record->drive = drive;
    record->media = &media_table[drive];
    record->system_cached = 0;
    record->fat_cached = 0;
    record->directory_cached = 0;
    record->guard = cursor;
    record->system_cache = cursor + VMS_GUARD_SIZE;
    record->fat_cache = cursor + VMS_BLOCK_SIZE + VMS_GUARD_SIZE;
    record->directory_cache =
        cursor + ((fat_blocks + 1) * VMS_BLOCK_SIZE) + VMS_GUARD_SIZE;
    record->system_capacity = 1;
    record->fat_capacity = fat_blocks;
    record->directory_capacity = directory_blocks;
    record->guard_size = VMS_GUARD_SIZE;
    memset(record->unk_38, 0, sizeof(record->unk_38));
    memset(record->dir_unsaved, 0, sizeof(record->dir_unsaved));
    memset(record->guard, 'G', VMS_GUARD_SIZE);
    memset(record->system_cache, 'S', MARK_SIZE);
    memset(record->fat_cache, 'F', MARK_SIZE);
    memset(record->directory_cache, 'D', MARK_SIZE);
    record->media->user_blocks = record->media->extra_block;
    record->mounted = 1;
    record->connected = 1;
    return 0;
}

Sint32 vmsfs_exit(void)
{
    memset(buVmsDrive, 0, sizeof(buVmsDrive));
    vmsio_exit();
    return 0;
}

Sint32 vmsfs_isready(void)
{
    Sint32 drive;
    Sint32 ready_bits = 0;
    Sint32 bit;

    for (drive = 0, bit = 1; drive < VMS_DRIVES; drive++, bit <<= 1) {
        if (vmsio_isready(drive)) {
            ready_bits |= bit;
        }
    }
    return ready_bits;
}

Sint32 vmsfs_ismount(Sint32 drive)
{
    VMS_DRIVE *record = &buVmsDrive[drive];

    if (vmsio_ismount(drive) && record->mounted) {
        return 1;
    }
    return 0;
}

Sint32 vmsfs_mount(Sint32 drive)
{
    /* @hack redundant: result, a local vmsfs_mount stores and never reads */
    Sint32 result;
    VMS_DRIVE *record = &buVmsDrive[drive];

    if (_vm->separate_workspaces == 1) {
        return vmsfs_mount2(drive);
    }
    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    PROGRESS_START(drive, PROGRESS_STEPS(BUD_OP_MOUNT));
    PROGRESS_STEP(drive);
    if ((result = vmsfs_mediainfo_retry(drive, record->media)) < 0) {
        record->connected = 0;
        record->mounted = 0;
        return result = BUD_ERR_NO_DISK;
    }
    if (check_media(record) < 0) {
        return BUD_ERR_UNKNOWN_DISK;
    }
    record->media->user_blocks = record->media->extra_block;
    record->connected = 1;
    record->mounted = 1;
    result = 0;
    record->system_cached = 0;
    record->fat_cached = 0;
    record->directory_cached = 0;
    return load_caches(drive);
}

Sint32 vmsfs_connect(Sint32 drive)
{
    /* @hack redundant: result, a local vmsfs_connect stores and never reads */
    Sint32 result;
    VMS_DRIVE *record = &buVmsDrive[drive];

    record->media = &media_table[drive];
    PROGRESS_START(drive, PROGRESS_STEPS(BUD_OP_CONNECT));
    PROGRESS_STEP(drive);
    if ((result = vmsfs_mediainfo_retry(drive, record->media)) < 0) {
        record->connected = 0;
        record->mounted = 0;
        return BUD_ERR_NO_DISK;
    }
    if (check_media(record) < 0) {
        return BUD_ERR_UNKNOWN_DISK;
    }
    record->media->user_blocks = record->media->extra_block;
    if (vmsio_block_read(drive, 0, probe_block) < 0) {
        return BUD_ERR_NO_DISK;
    }
    record->connected = 1;
    record->system_cached = 0;
    record->fat_cached = 0;
    record->directory_cached = 0;
    switch (record->capacity_class) {
    case 0:
        return CLASS_0_CACHE_BLOCKS;
    case 1:
        return CLASS_1_CACHE_BLOCKS;
    case 2:
        return CLASS_2_CACHE_BLOCKS;
    case 3:
        return CLASS_3_CACHE_BLOCKS;
    }
    return BUD_ERR_UNKNOWN_DISK;
}

Sint32 vmsfs_mount2(Sint32 drive)
{
    /* @hack redundant: unused_result, a local that only sizes the frame */
    Sint32 unused_result;
    /* @hack redundant: record, a local vmsfs_mount2 stores and never
     * reads */
    VMS_DRIVE *record = &buVmsDrive[drive];

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    PROGRESS_START(drive, PROGRESS_STEPS(BUD_OP_MOUNT));
    PROGRESS_STEP(drive);
    return load_caches(drive);
}

Sint32 vmsfs_unmount(Sint32 drive)
{
    VMS_DRIVE *record = &buVmsDrive[drive];

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    record->connected = 0;
    record->mounted = 0;
    record->system_cached = 0;
    record->fat_cached = 0;
    record->directory_cached = 0;
    record->capacity_class = 0;
    return 0;
}

Sint32 vmsfs_fgetinfo(Sint32 drive, const char *name, BUS_FILEINFO *output)
{
    /* @hack redundant: result, a local vmsfs_fgetinfo stores and never reads */
    Sint32 result;
    VMS_DIRINFO info;
    VMS_DRIVE *record = &buVmsDrive[drive];

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    if ((result = find_file(drive, name, &info)) < 0) {
        return BUD_ERR_FILE_NOT_FOUND;
    }
    output->filesize = info.blocks * (record->media->last_byte + 1);
    output->blocks = info.blocks;
    /* @hack redundant: a store of type that the test below replaces */
    output->type = info.kind;
    output->copyflag = info.copy;
    output->headerofs = info.header;
    if (info.kind == KIND_EXEC) {
        output->type = BUD_FILETYPE_EXECUTABLE;
    } else {
        output->type = BUD_FILETYPE_NORMAL;
    }
    unpack_time(&output->time, info.time);
    return 0;
}

Sint32 vmsfs_format(Sint32 drive, const char *label, Sint32 icon,
                    const BUS_TIME *timestamp, Sint32 erase_blocks)
{
    Sint32 index;
    Sint32 result;
    Sint32 saved_lock;
    /* @hack redundant: user_blocks, a local vmsfs_format stores and never
     * reads */
    Sint32 user_blocks; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    Sint32 directory_blocks;
    Sint32 directory_start;
    Sint32 fat_start;
    Uint8 *erase_buffer;
    /* @hack spelling: label_cursor, a second pointer that steps through label,
     * where label[index] would do */
    const char *label_cursor;
    /* @hack spelling: disk_time, the packed time that the system cache then
     * takes, where pack_time could pack it there */
    Uint8 disk_time[VMS_TIME_SIZE];
    VMS_DRIVE *record = &buVmsDrive[drive];

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (erase_blocks) {
        PROGRESS_START(drive, PROGRESS_STEPS(BUD_OP_FORMATDISK));
    } else {
        PROGRESS_START(drive,
                       PROGRESS_STEPS(BUD_OP_FORMATDISK) / NO_ERASE_DIVISOR);
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    erase_buffer = (Uint8 *)blank_blocks[drive];
    vmsfs_getbankinfo(drive, NULL, &saved_lock, NULL);
    vmsfs_lockbank(drive, BANK_LOCKED);
    memset(record->system_cache, 0, VMS_BLOCK_SIZE);
    if ((result = vmsfs_write_retry(drive, record->media->system_block,
                                    record->system_cache)) < 0) {
        if (result == BUD_ERR_WRITE_ERROR) {
            vmsfs_lockbank(drive, saved_lock);
            return BUD_ERR_ILLEGAL_DISK;
        }
        vmsfs_lockbank(drive, saved_lock);
        return BUD_ERR_NO_DISK;
    }
    /* @hack redundant: result stored in the test, which nothing reads */
    if ((result = vmsfs_mediainfo_retry(drive, record->media)) < 0) {
        vmsfs_lockbank(drive, saved_lock);
        return BUD_ERR_NO_DISK;
    }
    if (check_media(record) < 0) {
        vmsfs_lockbank(drive, saved_lock);
        return BUD_ERR_UNKNOWN_DISK;
    }
    record->media->user_blocks = record->media->extra_block;
    /* @hack redundant: the system cache zeroed a second time */
    memset(record->system_cache, 0, VMS_BLOCK_SIZE);
    /* @hack redundant: the label filled with spaces, which the copy below
     * overwrites */
    memset(record->system_cache + SYS_VOLUME, ' ', VMS_LABEL_SIZE);
    label_cursor = label;
    for (index = 0; index < VMS_LABEL_SIZE; index++, label_cursor++) {
        record->system_cache[index + SYS_VOLUME] = *label_cursor;
    }
    pack_time(disk_time, timestamp);
    for (index = 0; index < VMS_TIME_SIZE; index++) {
        record->system_cache[index + SYS_TIME] = disk_time[index];
    }
    fat_start = record->media->fat_block;
    directory_start = record->media->directory_block;
    _vm->write_word(record->system_cache, SYS_PARTITION,
                    record->media->partition);
    record->system_cache[SYS_ICON] = icon;
    /* @hack redundant: the store of 0 to SYS_SORT, which the zeroing above has
     * made */
    record->system_cache[SYS_SORT] = 0;
    _vm->write_word(record->system_cache, SYS_LAST_BLOCK,
                    record->media->last_block);
    _vm->write_word(record->system_cache, SYS_DIRECTORY_BLOCK, directory_start);
    _vm->write_word(record->system_cache, SYS_FAT_BLOCK, fat_start);
    _vm->write_word(record->system_cache, SYS_DIRECTORY_BLOCKS,
                    record->media->directory_blocks);
    _vm->write_word(record->system_cache, SYS_FAT_BLOCKS,
                    record->media->fat_blocks);
    _vm->write_word(record->system_cache, SYS_EXTRA_BLOCK,
                    record->media->extra_block);
    _vm->write_word(record->system_cache, SYS_EXTRA_BLOCKS,
                    record->media->extra_blocks);
    _vm->write_word(record->system_cache, SYS_EXEC_BLOCK,
                    record->media->exec_block);
    _vm->write_word(record->system_cache, SYS_EXEC_BLOCKS,
                    record->media->exec_blocks);
    /* last_block is the system block, as check_media requires */
    _vm->write_word(record->system_cache, SYS_SYSTEM_BLOCK,
                    record->media->last_block);
    memset(erase_buffer, 0, VMS_BLOCK_SIZE);
    for (index = 0; index < record->media->last_block; index++) {
        if (erase_blocks) {
            result = vmsfs_write_retry(drive, index, erase_buffer);
        } else {
            result = 0;
        }
        switch (result) {
        case BUD_ERR_NO_DISK:
            vmsfs_lockbank(drive, saved_lock);
            return BUD_ERR_NO_DISK;
        case BUD_ERR_WRITE_ERROR:
            fat_store(drive, index, FAT_BAD);
            break;
        default:
            fat_store(drive, index, FAT_FREE);
        }
    }
    user_blocks = record->media->user_blocks;
    directory_blocks = record->media->directory_blocks;
    memset(record->directory_cache, 0, directory_blocks * VMS_BLOCK_SIZE);
    rechain_dir(drive);
    fat_store(drive, record->media->system_block, FAT_LAST);
    for (index = 0; index < record->media->fat_blocks; index++) {
        fat_store(drive, record->media->fat_block + index, FAT_LAST);
    }
    /* @hack redundant: the system and FAT blocks marked a second time */
    fat_store(drive, record->media->system_block, FAT_LAST);
    for (index = 0; index < record->media->fat_blocks; index++) {
        fat_store(drive, record->media->fat_block + index, FAT_LAST);
    }
    write_fat(drive);
    memset(record->system_cache, SYSTEM_MAGIC, SYSTEM_MAGIC_SIZE);
    write_sys(drive);
    memset(record->system_cache, 0, SYSTEM_MAGIC_SIZE);
    record->fat_cached = 0;
    record->directory_cached = 0;
    record->system_cached = 0;
    vmsfs_lockbank(drive, saved_lock);
    return load_caches(drive);
}

/* the highest block whose FAT entry is value, in the directory's area or
 * else in the extra area. it returns -1 where neither holds one. the
 * directory's area is the directory_blocks blocks that end at
 * system_block - fat_blocks - 1. the extra area is the extra_blocks blocks
 * from extra_block */
static Sint32 fat_find_system(Sint32 drive, Uint16 value)
{
    Sint32 first;
    Sint32 last;
    Sint32 found;
    VMS_DRIVE *record = &buVmsDrive[drive];

    first = record->media->system_block - record->media->fat_blocks -
            record->media->directory_blocks;
    last = record->media->system_block - record->media->fat_blocks - 1;
    if ((found = fat_find_in(drive, value, first, last)) > 0) {
        return found;
    }
    first = record->media->extra_block;
    last = record->media->extra_block + record->media->extra_blocks - 1;
    if ((found = fat_find_in(drive, value, first, last)) > 0) {
        return found;
    }
    return -1;
}

/* the highest block from first to last whose FAT entry is value, or -1 */
static Sint32 fat_find_in(Sint32 drive, Uint16 value, Sint32 first, Sint32 last)
{
    Sint32 index;
    VMS_DRIVE *record = &buVmsDrive[drive];

    for (index = last; index >= first; index--) {
        if (_vm->read_word(record->fat_cache, index * FAT_ENTRY_SIZE) ==
            value) {
            return index;
        }
    }
    return -1;
}

Sint32 vmsfs_getinfo(Sint32 drive, BUS_DISKINFO *output)
{
    Sint32 index;

    /* @hack redundant: unused_counter, a local the copy steps and nothing
     * reads */
    Sint32 unused_counter;
    /* @hack spelling: disk_time, a copy of the system block's time, where
     * unpack_time could read the block itself */
    Uint8 disk_time[VMS_TIME_SIZE];
    VMS_DRIVE *record = &buVmsDrive[drive];

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    for (index = 0; index < VMS_LABEL_SIZE; index++,
        unused_counter++) { /* NOLINT(clang-diagnostic-uninitialized) */
        output->volume[index] = record->system_cache[SYS_VOLUME + index];
    }
    for (index = 0; index < VMS_TIME_SIZE; index++) {
        disk_time[index] = record->system_cache[SYS_TIME + index];
    }
    unpack_time(&output->time, disk_time);
    output->total_blocks = record->media->last_block + 1;
    output->total_user_blocks = record->media->user_blocks;
    output->free_user_blocks = vmsfs_diskfree(drive, BUD_FILETYPE_NORMAL);
    output->free_blocks = output->free_user_blocks;
    output->total_exe_blocks = record->media->exec_blocks;
    output->free_exe_blocks = vmsfs_diskfree(drive, BUD_FILETYPE_EXECUTABLE);
    output->block_size = record->media->last_byte + 1;
    output->icon_no = record->system_cache[SYS_ICON];
    return 0;
}

Sint32 vmsfs_diskfree(Sint32 drive, Sint32 kind)
{
    Sint32 block;
    Sint32 available;
    VMS_DRIVE *record = &buVmsDrive[drive];

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    available = 0;
    if (kind == BUD_FILETYPE_EXECUTABLE) {
        for (block = 0; block < record->media->exec_blocks; block++) {
            if (fat_fetch(drive, block) != FAT_FREE) {
                break;
            }
        }
        available = block;
    } else {
        if (buRealFree) {
            for (block = 0; block < record->media->user_blocks; block++) {
                if (fat_fetch(drive, block) == FAT_FREE) {
                    if (unclaimed_block(drive, block) >= 0) {
                        available++;
                    }
                }
            }
        } else {
            for (block = 0; block < record->media->user_blocks; block++) {
                if (fat_fetch(drive, block) == FAT_FREE) {
                    available++;
                }
            }
        }
    }
    return available;
}

Sint32 vmsfs_sysdiskfree(Sint32 drive)
{
    Sint32 block;
    Sint32 available;
    VMS_DRIVE *record = &buVmsDrive[drive];

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    available = 0;
    for (block = 0; block < record->media->fat_blocks *
                                (record->media->last_byte + 1) / FAT_ENTRY_SIZE;
         block++) {
        if (fat_fetch(drive, block) == FAT_FREE) {
            available++;
        }
    }
    return available;
}

Sint32 vmsfs_fopen(Sint32 drive, const char *name, BU_FILE **output)
{
    BU_FILE *file;
    VMS_DIRINFO info;
    /* @hack redundant: result, a local vmsfs_fopen stores and never reads */
    Sint32 result;

    *output = NULL;
    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    if (check_name(drive, name) < 0) {
        return BUD_ERR_CANNOT_OPEN;
    }
    if ((result = check_chain(drive, name)) < 0) {
        return BUD_ERR_FILE_BROKEN;
    }
    if (!(file = alloc_file(drive))) {
        return BUD_ERR_CANNOT_OPEN;
    }
    if ((dat_8c276a10 = find_file(drive, name, &info)) < 0) {
        free_file(file);
        return BUD_ERR_FILE_NOT_FOUND;
    }
    file->mode = VMS_FILE_READ;
    file->remaining_blocks = info.blocks;
    file->first_block = info.first_block;
    file->next_block = file->first_block;
    file->drive = drive;
    file->header = info.header;
    file->copy = info.copy;
    *output = file;
    return 0;
}

Sint32 vmsfs_xcreate(Sint32 drive, const char *name, const BUS_TIME *timestamp,
                     BU_FILE **opened)
{
    Sint32 slot;
    /* @hack redundant: entry, a local vmsfs_xcreate stores and never
     * reads */
    struct dir_entry
        *entry; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    char *destination;
    BU_FILE *file;
    VMS_DIRINFO info;
    VMS_DRIVE *record = &buVmsDrive[drive];

    *opened = NULL;
    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    if (!record->media->exec_blocks) {
        return BUD_ERR_CANNOT_CREATE;
    }
    if (check_name(drive, name) < 0) {
        return BUD_ERR_CANNOT_CREATE;
    }
    if (find_file(drive, name, &info) >= 0) {
        return BUD_ERR_FILE_EXIST;
    }
    if (find_exec_file(drive, &info) >= 0) {
        return BUD_ERR_EXECFILE_EXIST;
    }
    if (fat_fetch(drive, 0) != FAT_FREE) {
        return BUD_ERR_CANNOT_CREATE;
    }
    if ((slot = find_free_slot(drive)) < 0) {
        return BUD_ERR_CANNOT_CREATE;
    }
    entry = dir_slot(drive, slot);
    if ((file = alloc_file(drive)) == NULL) {
        return BUD_ERR_CANNOT_OPEN;
    }
    file->type = BUD_FILETYPE_EXECUTABLE;
    file->mode = VMS_FILE_WRITE_EXEC;
    file->directory_slot = slot;
    file->first_block = NO_BLOCK;
    file->written_blocks = 0;
    file->drive = drive;
    file->header = EXEC_HEADER;
    file->copy = 0;
    destination = file->name;
    while (*name) {
        *destination++ = *name++;
    }
    *destination = 0;
    pack_time(file->time, timestamp);
    *opened = file;
    return 0;
}

Sint32 vmsfs_fcreate(Sint32 drive, const char *name, const BUS_TIME *timestamp,
                     BU_FILE **opened)
{
    Sint32 slot;
    /* @hack redundant: entry, a local vmsfs_fcreate stores and never
     * reads */
    struct dir_entry
        *entry; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    char *destination;
    BU_FILE *file;
    VMS_DIRINFO info;

    *opened = NULL;
    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    if (check_name(drive, name) < 0) {
        return BUD_ERR_CANNOT_CREATE;
    }
    if (find_file(drive, name, &info) >= 0) {
        if ((slot = drop_file(drive, name)) < 0) {
            return slot;
        }
    }
    if ((slot = find_free_slot(drive)) < 0) {
        return BUD_ERR_CANNOT_CREATE;
    }
    entry = dir_slot(drive, slot);
    if ((file = alloc_file(drive)) == NULL) {
        return BUD_ERR_CANNOT_OPEN;
    }
    file->type = BUD_FILETYPE_NORMAL;
    file->mode = VMS_FILE_WRITE;
    file->directory_slot = slot;
    file->first_block = NO_BLOCK;
    file->written_blocks = 0;
    file->drive = drive;
    file->header = DATA_HEADER;
    file->copy = 0;
    destination = file->name;
    while (*name) {
        *destination++ = *name++;
    }
    *destination = 0;
    pack_time(file->time, timestamp);
    *opened = file;
    return 0;
}

Sint32 vmsfs_fsetparam(BU_FILE *file, Uint16 header_offset, Uint8 copy_flag)
{
    /* @hack redundant: unused_locals, locals that only size the frame */
    Sint32 unused_locals[4];

    if (!file) {
        return BUD_ERR_INVALID_PARAM;
    }
    if (!buVmsDrive[file->drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(file->drive)) {
        vmsfs_unmount(file->drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(file->drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[file->drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(file->drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    file->header = header_offset;
    file->copy = copy_flag;
    return 0;
}

Sint32 vmsfs_fclose(BU_FILE *file, Sint32 report_progress)
{
    Sint32 index;
    Sint32 result;
    Sint32 drive;
    struct dir_entry *entry;
    /* @hack spelling: kind, which holds the entry's new kind till its store,
     * where each branch could store entry->kind */
    Uint8 kind;

    if (!file) {
        return BUD_ERR_INVALID_PARAM;
    }
    result = 0;
    if (!buVmsDrive[file->drive].enabled) {
        goto release_file;
    }
    if (!vmsio_ismount(file->drive)) {
        goto release_file;
    }
    if (!buVmsDrive[file->drive].mounted) {
        goto release_file;
    }
    if (!vmsio_isready(file->drive)) {
        vmsfs_unmount(file->drive);
        goto release_file;
    }
    if (buIsFormat(file->drive) < 0) {
        goto release_file;
    }
    drive = file->drive;
    if (file->mode != VMS_FILE_READ) {
        if (report_progress) {
            PROGRESS_START(drive, PROGRESS_STEPS(BUD_OP_CLOSEFILE));
        }
        entry = dir_slot(drive, file->directory_slot);
        if (file->type == BUD_FILETYPE_EXECUTABLE) {
            kind = KIND_EXEC;
        } else {
            kind = KIND_DATA;
        }
        entry->kind = kind;
        entry->copy = file->copy;
        _vm->write_word(entry, DIR_FIRST_BLOCK, file->first_block);
        _vm->write_word(entry, DIR_BLOCKS, file->written_blocks);
        _vm->write_word(entry, DIR_HEADER, file->header);
        /* @hack spelling: both copies store into the entry's bytes at index
         * plus the field's offset, where entry->name[index] and
         * entry->time[index] would do */
        for (index = 0; index < VMS_NAME_SIZE; index++) {
            ((Uint8 *)entry)[index + DIR_NAME] = file->name[index];
        }
        for (index = 0; index < VMS_TIME_SIZE; index++) {
            ((Uint8 *)entry)[index + DIR_TIME] = file->time[index];
        }
        dir_mark_unsaved(drive, file->directory_slot);
        if ((result = flush_dir(drive)) < 0) {
            goto release_file;
        }
        result = write_fat(drive);
    }
    /* @bug logic: overwrites write_fat's result with 0, so a close whose
     * FAT failed to write still returns 0. MOV.L R0,@(16,R15) at 0x8c0c490a
     * stores the result, and MOV.L R3,@(16,R15) at 0x8c0c490e the 0 */
    result = 0;
release_file:
    free_file(file);
    return result;
}

Sint32 vmsfs_fwrite(BU_FILE *file, const void *buffer, Sint32 count)
{
    Sint32 index;
    Sint32 result;
    const char *cursor;
    Sint32 (*write_block)(BU_FILE *, const void *);

    if (!file) {
        return BUD_ERR_INVALID_PARAM;
    }
    if (!buVmsDrive[file->drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    switch (file->mode) {
    case VMS_FILE_WRITE:
        write_block = vmsfs_fwrite1;
        break;
    case VMS_FILE_WRITE_EXEC:
        write_block = vmsfs_xwrite1;
        break;
    default:
        return BUD_ERR_ACCESS_DENIED;
    }
    cursor = buffer;
    PROGRESS_START(file->drive, count);
    for (index = 0; index < count; index++) {
        if ((result = write_block(file, cursor)) < 0) {
            return result;
        }
        cursor += VMS_BLOCK_SIZE;
    }
    return 0;
}

Sint32 vmsfs_fwrite1(BU_FILE *file, const void *buffer)
{
    Sint32 block;
    Sint32 result;
    Sint32 drive;
    /* @hack redundant: type, a local vmsfs_fwrite1 stores and never reads */
    Sint32 type; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    if (!vmsio_isready(file->drive)) {
        vmsfs_unmount(file->drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(file->drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[file->drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(file->drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    type = file->type;
    drive = file->drive;
    if ((block = find_free_block(drive)) < 0) {
        return BUD_ERR_DISK_FULL;
    }
    if ((result = vmsfs_write_retry(drive, block, buffer)) < 0) {
        if (result == BUD_ERR_NO_DISK) {
            return result;
        }
        /* @bug logic: marks every free user block bad in the FAT cache once
         * the card leaves after a failed write. the call then returns
         * BUD_ERR_DISK_FULL, not BUD_ERR_NO_DISK. only the first write's
         * failure is tested for BUD_ERR_NO_DISK (CMP/EQ R2,R0 at 0x8c0c4ae4).
         * each later write is tested only for success (TST R0,R0 at
         * 0x8c0c4b3c) */
        while (1) {
            fat_store(drive, block, FAT_BAD);
            if ((block = fat_find_user(drive, FAT_FREE)) < 0) {
                return BUD_ERR_DISK_FULL;
            }
            if ((result = vmsfs_write_retry(drive, block, buffer)) == 0) {
                break;
            }
        }
    }
    fat_store(drive, block, FAT_LAST);
    /* @hack type: the casts of block to short in the three stores below,
     * where block alone would do. the two are equal while block fits a
     * short */
    if (file->first_block == NO_BLOCK) {
        file->first_block = (short)block;
        file->previous_block = (short)block;
    } else {
        fat_store(drive, file->previous_block, block);
        file->previous_block = (short)block;
    }
    file->written_blocks++;
    return 0;
}

Sint32 vmsfs_xwrite1(BU_FILE *file, const void *buffer)
{
    Sint32 block;
    Sint32 result;
    Sint32 drive;
    /* @hack redundant: type, a local vmsfs_xwrite1 stores and never reads */
    Sint32 type; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    VMS_DRIVE *record = &buVmsDrive[file->drive];

    if (!vmsio_isready(file->drive)) {
        vmsfs_unmount(file->drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(file->drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[file->drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(file->drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    type = file->type;
    drive = file->drive;
    block = file->written_blocks;
    if (fat_fetch(drive, block) != FAT_FREE) {
        return BUD_ERR_DISK_FULL;
    }
    if (file->written_blocks >= record->media->exec_blocks) {
        return BUD_ERR_DISK_FULL;
    }
    if ((result = vmsfs_write_retry(drive, block, buffer)) < 0) {
        return result;
    }
    fat_store(drive, block, FAT_LAST);
    /* @hack type: the casts of block to short in the three stores below,
     * where block alone would do. the two are equal while block fits a
     * short */
    if (file->first_block == NO_BLOCK) {
        file->first_block = (short)block;
        file->previous_block = (short)block;
    } else {
        fat_store(drive, file->previous_block, block);
        file->previous_block = (short)block;
    }
    file->written_blocks++;
    return 0;
}

Sint32 vmsfs_fread(BU_FILE *file, void *buffer, Sint32 count)
{
    Sint32 index;
    Sint32 result;
    char *cursor;

    if (!file) {
        return BUD_ERR_INVALID_PARAM;
    }
    if (!buVmsDrive[file->drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    cursor = buffer;
    PROGRESS_START(file->drive, count);
    for (index = 0; index < count; index++) {
        if ((result = vmsfs_fread1(file, cursor)) < 0) {
            return result;
        }
        cursor += VMS_BLOCK_SIZE;
    }
    return 0;
}

Sint32 vmsfs_fread1(BU_FILE *file, void *buffer)
{
    Sint32 block;
    Sint32 drive;
    Sint32 result;

    if (!vmsio_isready(file->drive)) {
        vmsfs_unmount(file->drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(file->drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[file->drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(file->drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    drive = file->drive;
    /* @hack spelling: the else after a return, where the second return could
     * follow the if */
    if (file->next_block == FAT_LAST) {
        if (file->remaining_blocks == 0) {
            return 0;
        } else {
            return BUD_ERR_GENERIC;
        }
    }
    if (file->remaining_blocks == 0) {
        return 0;
    }
    if (file->next_block == FAT_FREE) {
        return BUD_ERR_FILE_BROKEN;
    }
    if (file->next_block == FAT_BAD) {
        return BUD_ERR_FILE_BROKEN;
    }
    block = file->next_block;
    if ((result = vmsfs_read_retry(drive, block, buffer)) < 0) {
        return result;
    }
    file->next_block = fat_fetch(drive, block);
    file->remaining_blocks--;
    return file->remaining_blocks;
}

Sint32 vmsfs_fisexist(Sint32 drive, const char *name)
{
    /* @hack redundant: unused_local, a local that only sizes the frame */
    Sint32 unused_local;
    VMS_DIRINFO info;

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    if (find_file(drive, name, &info) >= 0) {
        return 0;
    }
    return BUD_ERR_FILE_NOT_FOUND;
}

Sint32 vmsfs_findfirst(Sint32 drive, char *name)
{
    Sint32 index;
    Sint32 character;
    Sint32 found;
    Uint8 kind;
    struct dir_entry *entry;
    VMS_DRIVE *record = &buVmsDrive[drive];

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    record->directory_cursor = 0;
    entry = dir_slot(drive, record->directory_cursor);
    found = 0;
    for (index = 0; index < record->media->user_blocks; index++, entry++) {
        kind = entry->kind;
        if (kind == KIND_DATA || kind == KIND_EXEC) {
            found = 1;
            break;
        }
        record->directory_cursor++;
    }
    if (!found) {
        return BUD_ERR_FILE_NOT_FOUND;
    }
    /* @hack spelling: the name read as the entry's bytes at character plus
     * DIR_NAME, where entry->name[character] would do */
    for (character = 0; character < VMS_NAME_SIZE; character++) {
        name[character] = ((Uint8 *)entry)[character + DIR_NAME];
    }
    name[VMS_NAME_SIZE] = 0;
    record->directory_cursor++;
    return 0;
}

Sint32 vmsfs_findnext(Sint32 drive, char *name)
{
    Sint32 index;
    Sint32 character;
    Sint32 found;
    Uint8 kind;
    struct dir_entry *entry;
    VMS_DRIVE *record = &buVmsDrive[drive];

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    entry = dir_slot(drive, record->directory_cursor);
    found = 0;
    for (index = record->directory_cursor; index < record->media->user_blocks;
         index++, entry++) {
        record->directory_cursor++;
        kind = entry->kind;
        if (kind == KIND_DATA || kind == KIND_EXEC) {
            found = 1;
            break;
        }
    }
    if (!found) {
        return BUD_ERR_FILE_NOT_FOUND;
    }
    /* @hack spelling: the name read as the entry's bytes at character plus
     * DIR_NAME, where entry->name[character] would do */
    for (character = 0; character < VMS_NAME_SIZE; character++) {
        name[character] = ((Uint8 *)entry)[character + DIR_NAME];
    }
    name[VMS_NAME_SIZE] = 0;
    return 0;
}

Sint32 vmsfs_findfirst_ex(Sint32 drive, char *name, BUS_FILEINFO *output)
{
    Sint32 index;
    Sint32 character;
    Sint32 found;
    Uint8 kind;
    struct dir_entry *entry;
    VMS_DRIVE *record = &buVmsDrive[drive];
    VMS_DIRINFO info;

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    record->directory_cursor = 0;
    entry = dir_slot(drive, record->directory_cursor);
    found = 0;
    for (index = 0; index < record->media->user_blocks; index++, entry++) {
        kind = entry->kind;
        if (kind == KIND_DATA || kind == KIND_EXEC) {
            found = 1;
            break;
        }
        record->directory_cursor++;
    }
    if (!found) {
        return BUD_ERR_FILE_NOT_FOUND;
    }
    /* @hack spelling: the name read as the entry's bytes at character plus
     * DIR_NAME, where entry->name[character] would do */
    for (character = 0; character < VMS_NAME_SIZE; character++) {
        name[character] = ((Uint8 *)entry)[character + DIR_NAME];
    }
    name[VMS_NAME_SIZE] = 0;
    record->directory_cursor++;
    unpack_entry(entry, &info);
    output->filesize = info.blocks * (record->media->last_byte + 1);
    output->blocks = info.blocks;
    /* @hack redundant: a store of type that the test below replaces */
    output->type = info.kind;
    output->copyflag = info.copy;
    output->headerofs = info.header;
    if (info.kind == KIND_EXEC) {
        output->type = BUD_FILETYPE_EXECUTABLE;
    } else {
        output->type = BUD_FILETYPE_NORMAL;
    }
    unpack_time(&output->time, info.time);
    return 0;
}

Sint32 vmsfs_findnext_ex(Sint32 drive, char *name, BUS_FILEINFO *output)
{
    Sint32 index;
    Sint32 character;
    Sint32 found;
    Uint8 kind;
    struct dir_entry *entry;
    VMS_DRIVE *record = &buVmsDrive[drive];
    VMS_DIRINFO info;

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    entry = dir_slot(drive, record->directory_cursor);
    found = 0;
    for (index = record->directory_cursor; index < record->media->user_blocks;
         index++, entry++) {
        record->directory_cursor++;
        kind = entry->kind;
        if (kind == KIND_DATA || kind == KIND_EXEC) {
            found = 1;
            break;
        }
    }
    if (!found) {
        return BUD_ERR_FILE_NOT_FOUND;
    }
    /* @hack spelling: the name read as the entry's bytes at character plus
     * DIR_NAME, where entry->name[character] would do */
    for (character = 0; character < VMS_NAME_SIZE; character++) {
        name[character] = ((Uint8 *)entry)[character + DIR_NAME];
    }
    name[VMS_NAME_SIZE] = 0;
    unpack_entry(entry, &info);
    output->filesize = info.blocks * (record->media->last_byte + 1);
    output->blocks = info.blocks;
    /* @hack redundant: a store of type that the test below replaces */
    output->type = info.kind;
    output->copyflag = info.copy;
    output->headerofs = info.header;
    if (info.kind == KIND_EXEC) {
        output->type = BUD_FILETYPE_EXECUTABLE;
    } else {
        output->type = BUD_FILETYPE_NORMAL;
    }
    unpack_time(&output->time, info.time);
    return 0;
}

/* free the file's blocks, unless its chain is broken, and clear its
 * directory entry. the directory and FAT stay to be written. returns 0, or
 * BUD_ERR_FILE_NOT_FOUND */
static Sint32 drop_file(Sint32 drive, const char *name)
{
    Sint32 index;
    /* @hack redundant: result, a local drop_file stores and never reads */
    Sint32 result;
    Sint32 block;
    Sint32 next;
    Sint32 slot;
    struct dir_entry *entry;
    VMS_DIRINFO info;

    if ((slot = find_file(drive, name, &info)) < 0) {
        return BUD_ERR_FILE_NOT_FOUND;
    }
    entry = dir_slot(drive, slot);
    if ((result = check_chain(drive, name)) == 0) {
        block = info.first_block;
        for (index = 0; index < info.blocks; index++) {
            next = fat_fetch(drive, block);
            fat_store(drive, block, FAT_FREE);
            block = next;
        }
    }
    memset(entry, 0, sizeof(*entry));
    dir_mark_unsaved(drive, slot);
    return 0;
}

Sint32 vmsfs_fdelete(Sint32 drive, const char *name)
{
    Sint32 result;
    Sint32 saved_lock;

    PROGRESS_START(drive, PROGRESS_STEPS(BUD_OP_DELETEFILE));
    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    if (check_name(drive, name) < 0) {
        /* @bug uninitialized: the goto restores the bank's lock from
         * saved_lock, which no store has set on this path. the code at
         * restore_lock passes saved_lock to vmsfs_lockbank (MOV.L
         * @(4,R15),R5 at 0x8c0c5972). only the vmsfs_getbankinfo below sets
         * it (JSR @R3 at 0x8c0c5916), and this path branches past that call
         * (BRA at 0x8c0c5906). no call takes the path, since check_name
         * always returns 0 (MOV #0,R0 at 0x8c0c7976) */
        result = BUD_ERR_ACCESS_DENIED;
        goto restore_lock;
    }
    vmsfs_getbankinfo(drive, NULL, &saved_lock, NULL);
    vmsfs_lockbank(drive, BANK_LOCKED);
    if ((result = drop_file(drive, name)) < 0) {
        goto restore_lock;
    }
    if ((result = flush_dir(drive)) < 0) {
        goto restore_lock;
    }
    result = write_fat(drive);
restore_lock:
    vmsfs_lockbank(drive, saved_lock);
    return result;
}

Sint32 vmsfs_fdelete_ex(Sint32 drive, const char **names, Sint32 count)
{
    Sint32 index;
    Sint32 result;
    Sint32 failed;
    Sint32 saved_lock;

    PROGRESS_START(drive, PROGRESS_STEPS(BUD_OP_DELETEFILEEX));
    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    for (index = 0; index < count; index++) {
        if (check_name(drive, names[index]) < 0) {
            return BUD_ERR_CANNOT_DELETE;
        }
    }
    failed = 0;
    for (index = 0; index < count; index++) {
        /* @hack redundant: result stored in the test, which nothing reads */
        if ((result = drop_file(drive, names[index])) < 0) {
            failed = 1;
        }
    }
    vmsfs_getbankinfo(drive, NULL, &saved_lock, NULL);
    vmsfs_lockbank(drive, BANK_LOCKED);
    if ((result = flush_dir(drive)) < 0) {
        goto restore_lock;
    }
    if ((result = write_fat(drive)) < 0) {
        goto restore_lock;
    }
    if (failed) {
        result = BUD_ERR_CANNOT_DELETE;
    }
restore_lock:
    vmsfs_lockbank(drive, saved_lock);
    return result;
}

Sint32 vmsfs_frename(Sint32 drive, const char *old_name, const char *new_name)
{
    Sint32 index;
    Sint32 slot;
    struct dir_entry *entry;
    VMS_DIRINFO info;

    PROGRESS_START(drive, PROGRESS_STEPS(BUD_OP_RENAMEFILE));
    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    if (check_name(drive, old_name) < 0) {
        return BUD_ERR_ACCESS_DENIED;
    }
    if ((slot = find_file(drive, old_name, &info)) < 0) {
        return BUD_ERR_FILE_NOT_FOUND;
    }
    if (find_file(drive, new_name, &info) >= 0) {
        return BUD_ERR_FILE_EXIST;
    }
    entry = dir_slot(drive, slot);
    /* @hack spelling: the name stored as the entry's bytes at index plus
     * DIR_NAME, where entry->name[index] would do */
    for (index = 0; index < VMS_NAME_SIZE; index++) {
        ((Uint8 *)entry)[index + DIR_NAME] = new_name[index];
    }
    dir_mark_unsaved(drive, slot);
    return flush_dir(drive);
}

Sint32 vmsfs_fsettime(Sint32 drive, const char *name, const BUS_TIME *time)
{
    Sint32 index;
    Sint32 slot;
    struct dir_entry *entry;
    /* @hack spelling: disk_time, the packed time that the entry's bytes then
     * take, where pack_time(entry->time, time) would do */
    Uint8 disk_time[VMS_TIME_SIZE];
    VMS_DIRINFO info;

    PROGRESS_START(drive, PROGRESS_STEPS(BUD_OP_SETFILEATTR));
    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    if (check_name(drive, name) < 0) {
        return BUD_ERR_ACCESS_DENIED;
    }
    if ((slot = find_file(drive, name, &info)) < 0) {
        return BUD_ERR_FILE_NOT_FOUND;
    }
    entry = dir_slot(drive, slot);
    pack_time(disk_time, time);
    /* @hack spelling: the time stored as the entry's bytes at index plus
     * DIR_TIME, where entry->time[index] would do */
    for (index = 0; index < VMS_TIME_SIZE; index++) {
        ((Uint8 *)entry)[index + DIR_TIME] = disk_time[index];
    }
    dir_mark_unsaved(drive, slot);
    return flush_dir(drive);
}

Sint32 vmsfs_fgettime(Sint32 drive, const char *name, BUS_TIME *time)
{
    Sint32 index;
    Sint32 slot;
    struct dir_entry *entry;
    VMS_DIRINFO info;
    /* @hack spelling: disk_time, a copy of the entry's time, where
     * unpack_time(time, entry->time) would do */
    Uint8 disk_time[VMS_TIME_SIZE];

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    if (check_name(drive, name) < 0) {
        return BUD_ERR_ACCESS_DENIED;
    }
    if ((slot = find_file(drive, name, &info)) < 0) {
        return BUD_ERR_FILE_NOT_FOUND;
    }
    entry = dir_slot(drive, slot);
    /* @hack spelling: the time read as the entry's bytes at index plus
     * DIR_TIME, where entry->time[index] would do */
    for (index = 0; index < VMS_TIME_SIZE; index++) {
        disk_time[index] = ((Uint8 *)entry)[index + DIR_TIME];
    }
    unpack_time(time, disk_time);
    return 0;
}

Sint32 vmsfs_fsetattr(Sint32 drive, const char *name, Uint16 header_offset,
                      Uint8 copy_flag)
{
    /* @hack redundant: unused_local, a local that only sizes the frame */
    Sint32 unused_local;
    Sint32 slot;
    struct dir_entry *entry;
    VMS_DIRINFO info;

    PROGRESS_START(drive, PROGRESS_STEPS(BUD_OP_SETFILEATTR));
    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    if (check_name(drive, name) < 0) {
        return BUD_ERR_ACCESS_DENIED;
    }
    if ((slot = find_file(drive, name, &info)) < 0) {
        return BUD_ERR_FILE_NOT_FOUND;
    }
    entry = dir_slot(drive, slot);
    if (buEnableHeaderOffset) {
        _vm->write_word(entry, DIR_HEADER, header_offset);
    }
    entry->copy = copy_flag;
    dir_mark_unsaved(drive, slot);
    return flush_dir(drive);
}

Sint32 vmsfs_fgetsize(Sint32 drive, const char *name)
{
    /* @hack redundant: slot, a local vmsfs_fgetsize stores and never reads */
    Sint32 slot;
    VMS_DIRINFO info;

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    if (check_name(drive, name) < 0) {
        return BUD_ERR_ACCESS_DENIED;
    }
    if ((slot = find_file(drive, name, &info)) < 0) {
        return BUD_ERR_FILE_NOT_FOUND;
    }
    return info.blocks;
}

Sint32 vmsfs_xfind(Sint32 drive, char *name)
{
    Sint32 index;
    /* @hack redundant: slot, a local vmsfs_xfind stores and never reads */
    Sint32 slot;
    VMS_DIRINFO info;

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    if ((slot = find_exec_file(drive, &info)) < 0) {
        return BUD_ERR_FILE_NOT_FOUND;
    }
    for (index = 0; index < VMS_NAME_SIZE; index++) {
        name[index] = info.name[index];
    }
    name[VMS_NAME_SIZE] = 0;
    return 0;
}

Sint32 vmsfs_isformat(Sint32 drive)
{
    Sint32 index;
    VMS_DRIVE *record = &buVmsDrive[drive];

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    for (index = 0; index < SYSTEM_MAGIC_SIZE; index++) {
        if (record->system_cache[index] != SYSTEM_MAGIC) {
            record->fat_cached = 0;
            record->directory_cached = 0;
            return BUD_ERR_UNFORMAT;
        }
    }
    if (buIllegalFormat[drive]) {
        return BUD_ERR_UNFORMAT;
    }
    return 0;
}

Sint32 vmsfs_getvolume(Sint32 drive, char *output)
{
    Sint32 index;
    VMS_DRIVE *record = &buVmsDrive[drive];

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    for (index = 0; index < VMS_LABEL_SIZE; index++) {
        *output++ = record->system_cache[index + SYS_VOLUME];
    }
    return 0;
}

Sint32 vmsfs_setvolume(Sint32 drive, const char *label)
{
    Sint32 index;
    VMS_DRIVE *record = &buVmsDrive[drive];

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    /* @hack redundant: the label zeroed, which the copy below overwrites */
    for (index = 0; index < VMS_LABEL_SIZE; index++) {
        record->system_cache[index + SYS_VOLUME] = 0;
    }
    for (index = 0; index < VMS_LABEL_SIZE; index++) {
        record->system_cache[index + SYS_VOLUME] = *label++;
    }
    return write_sys(drive);
}

Sint32 vmsfs_stat(void)
{
    return 0;
}

/* the lowest user block that is neither free nor bad, or -1 */
static Sint32 first_used_block(Sint32 drive)
{
    Sint32 index;
    /* @hack spelling: value, which holds the entry for the switch, where the
     * read itself would do */
    Sint32 value;
    VMS_DRIVE *record = &buVmsDrive[drive];

    for (index = 0; index < record->media->user_blocks; index++) {
        value = _vm->read_word(record->fat_cache, index * FAT_ENTRY_SIZE);
        switch (value) {
        case FAT_FREE:
        case FAT_BAD:
            break;
        default:
            return index;
        }
    }
    return -1;
}

/* the highest free user block from first up, or -1 */
static Sint32 last_free_block(Sint32 drive, Sint32 first)
{
    Sint32 index;
    VMS_DRIVE *record = &buVmsDrive[drive];

    for (index = record->media->user_blocks - 1; index >= first; index--) {
        if (_vm->read_word(record->fat_cache, index * FAT_ENTRY_SIZE) ==
            FAT_FREE) {
            return index;
        }
    }
    return -1;
}

/* the directory entry of the file whose first block is block, or null */
static struct dir_entry *entry_starting_at(Sint32 drive, Sint32 block)
{
    Sint32 index;
    /* @hack spelling: kind, a copy of the entry's kind for the test, where
     * entry->kind would do */
    Uint8 kind;
    struct dir_entry *entry;
    VMS_DRIVE *record = &buVmsDrive[drive];

    entry = (struct dir_entry *)record->directory_cache;
    for (index = 0; index < record->media->user_blocks; index++, entry++) {
        kind = entry->kind;
        if (kind != KIND_EMPTY) {
            if (_vm->read_word(entry, DIR_FIRST_BLOCK) == block) {
                return entry;
            }
        }
    }
    return NULL;
}

/* -1 where a FAT entry links out of its area, and 0 where none does. a user
 * block's entry must link within the user area, and any other block's within
 * the card */
static Sint32 check_links(Sint32 drive)
{
    VMS_DRIVE *record = &buVmsDrive[drive];
    Sint32 index;
    Sint32 value;

    for (index = 0; index < record->media->last_block + 1; index++) {
        value = fat_fetch(drive, index);
        switch (value) {
        case FAT_FREE:
        case FAT_BAD:
        case FAT_LAST:
            break;
        default:
            if (index < record->media->user_blocks) {
                if (value >= record->media->user_blocks) {
                    return -1;
                }
            } else {
                if (value >= record->media->last_block + 1) {
                    return -1;
                }
            }
            break;
        }
    }
    return 0;
}

Sint32 vmsfs_xdefrag(Sint32 drive, void *buffer)
{
    Sint32 index;
    Sint32 result;
    Sint32 saved_lock;
    Sint32 free_block;
    Sint32 used_block;
    Sint32 previous;
    char executable_name[VMS_NAME_BUFFER];
    VMS_DRIVE *record = &buVmsDrive[drive];
    struct dir_entry *entry;

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_isready(drive)) {
        vmsfs_unmount(drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    if (!record->media->exec_blocks) {
        return BUD_ERR_ACCESS_DENIED;
    }
    /* @hack redundant: result stored in the test, which nothing reads */
    if ((result = vmsfs_xfind(drive, executable_name)) == 0) {
        return BUD_ERR_ACCESS_DENIED;
    }
    if (check_links(drive) < 0) {
        return BUD_ERR_ACCESS_DENIED;
    }
    vmsfs_getbankinfo(drive, NULL, &saved_lock, NULL);
    vmsfs_lockbank(drive, BANK_LOCKED);
    while (1) {
        if ((used_block = first_used_block(drive)) < 0) {
            break;
        }
        if ((free_block = last_free_block(drive, used_block + 1)) < 0) {
            break;
        }
        if ((result = vmsfs_read_retry(drive, used_block, buffer)) < 0) {
            goto restore_lock;
        }
        if ((result = vmsfs_write_retry(drive, free_block, buffer)) < 0) {
            goto restore_lock;
        }
        if ((previous = fat_find_user(drive, used_block)) > 0) {
            fat_store(drive, previous, free_block);
        } else {
            entry = entry_starting_at(drive, used_block);
            /* @bug memory: stores through null where no file starts at the
             * moved block and no other user block links to it.
             * entry_starting_at then returns null (MOV #0,R0 at 0x8c0c685c).
             * vmsfs_xdefrag passes the null to _vm->write_word untested (JSR
             * @R1 at 0x8c0c6ada). the store is vmsfs_setword_little's MOV.W
             * R6,@R4 at 0x8c15cef2, or vmsfs_setword_big's at 0x8c15ceec. a
             * file that drop_file deletes with a broken chain can leave such
             * a block */
#ifdef NONMATCHING
            /* this side repairs the tag above: it updates no entry where no
             * file starts at the moved block */
            if (entry != NULL) {
                _vm->write_word(entry, DIR_FIRST_BLOCK, free_block);
            }
#else
            _vm->write_word(entry, DIR_FIRST_BLOCK, free_block);
#endif
        }
        fat_store(drive, free_block, fat_fetch(drive, used_block));
        fat_store(drive, used_block, FAT_FREE);
    }
    for (index = 0; index < VMS_DIR_BLOCKS_MAX; index++) {
        record->dir_unsaved[index] = 1;
    }
    if ((result = flush_dir(drive)) < 0) {
        goto restore_lock;
    }
    result = write_fat(drive);
restore_lock:
    vmsfs_lockbank(drive, saved_lock);
    return result;
}

/* read the directory's chain into its cache. returns 0 or a read's failure */
static Sint32 cache_dir(Sint32 drive)
{
    Sint32 index;
    Sint32 result;
    Sint32 count;
    Uint16 block;
    Uint16 next;
    VMS_DRIVE *record = &buVmsDrive[drive];

    count = record->media->directory_blocks;
    block = record->media->directory_block;
    for (index = 0; index < count; index++) {
        if ((result = vmsfs_read_retry(drive, block,
                                       record->directory_cache +
                                           (index * VMS_BLOCK_SIZE))) < 0) {
            return result;
        }
        if ((next = fat_fetch(drive, block)) == FAT_LAST) {
            break;
        }
        block = next;
    }
    return 0;
}

/* free the directory's chain, and write the directory cache to a new one.
 * the new chain starts at the old one's first block, and takes each later
 * block from fat_find_system. a block whose write fails is marked bad, and
 * fat_find_system's free block takes its place. the system block records a
 * new first block, and is written with the FAT. it returns 0, or
 * BUD_ERR_NO_DISK */
static Sint32 rechain_dir(Sint32 drive)
{
    Uint16 block;
    Uint16 next;
    Uint16 first;
    Sint32 index;
    Sint32 result;
    Sint32 count;
    VMS_DRIVE *record = &buVmsDrive[drive];

    first = record->media->directory_block;
    count = record->media->directory_blocks;
    block = first;
    /* @bug memory: reads past the FAT cache's end where the FAT entry of the
     * directory's first block is FAT_BAD. vmsfs_format leaves FAT_BAD there
     * where the block's erase gives BUD_ERR_WRITE_ERROR. the loop stops only at
     * FAT_LAST or FAT_FREE (CMP/EQ at 0x8c0c6c54 and 0x8c0c6c72). the loop
     * takes FAT_BAD for the next block. fat_fetch then reads block 0xffff's
     * entry (BSR at 0x8c0c6c3a) */
    for (index = 0; index < count; index++) {
        next = fat_fetch(drive, block);
        fat_store(drive, block, FAT_FREE);
        if (next == FAT_LAST) {
            break;
        }
        if (next == FAT_FREE) {
            break;
        }
        block = next;
    }
    block = first;
    /* @bug logic: writes the directory to block 0xffff, past the card, where
     * neither area has a free block. the loop takes fat_find_system's -1 as
     * block and as next without a test (MOV.W R0,@(22,R15) at 0x8c0c6cf4,
     * MOV.W R0,@(20,R15) at 0x8c0c6d34). flush_dir tests that result (CMP/PZ
     * R0 at 0x8c0c6e60). a failed write of block 0xffff finds no block to
     * take its place. the loop then repeats till a write succeeds or gives
     * BUD_ERR_NO_DISK */
    for (index = 0; index < count; index++) {
        while (1) {
            result = vmsfs_write_retry(drive, block,
                                       record->directory_cache +
                                           (index * VMS_BLOCK_SIZE));
            if (result == 0) {
                break;
            }
            if (result == BUD_ERR_NO_DISK) {
                return result;
            }
            fat_store(drive, block, FAT_BAD);
            block = fat_find_system(drive, FAT_FREE);
        }
        if (index == 0) {
            first = block;
        }
        fat_store(drive, block, FAT_LAST);
        if (index != count - 1) {
            next = fat_find_system(drive, FAT_FREE);
            fat_store(drive, block, next);
            block = next;
        }
    }
    /* @bug logic: leaves media's directory_block on the old first block where
     * the directory's first block moves. only the system block records the
     * new first block (JSR @R1 at 0x8c0c6d74). cache_dir and flush_dir then
     * start from the old block till a mount reads the layout again. write_fat
     * sets media's fat_block where the FAT moves (MOV.L R1,@(12,R3) at
     * 0x8c0c7508) */
    if (first != record->media->directory_block) {
        _vm->write_word(record->system_cache, SYS_DIRECTORY_BLOCK, first);
        write_fat(drive);
        write_sys(drive);
    }
    return 0;
}

/* write each unsaved block of the directory cache to its block of the
 * directory's chain. a block whose write fails is marked bad, and
 * fat_find_system's free block takes its place. where a block moved, the FAT
 * and system block are then written.
 *
 * it returns 0, BUD_ERR_NO_DISK, or a failure to write the FAT or system
 * block. it returns BUD_ERR_DISK_FULL where no free block is left to take a
 * bad one's place */
static Sint32 flush_dir(Sint32 drive)
{
    Sint32 block;
    Sint32 next;
    Sint32 previous;
    Sint32 replacement;
    Uint16 first;
    Sint32 index;
    Sint32 result;
    Sint32 count;
    Sint32 moved;
    VMS_DRIVE *record = &buVmsDrive[drive];

    first = record->media->directory_block;
    count = record->media->directory_blocks;
    block = first;
    moved = 0;
#ifdef NONMATCHING
    /* only an index past 0 reads previous, and the index before it sets it.
     * GCC can't prove that, so GCC's build sets previous here */
    previous = first;
#endif
    for (index = 0; index < count; index++) {
        if (record->dir_unsaved[index]) {
            while (1) {
                result = vmsfs_write_retry(drive, block,
                                           record->directory_cache +
                                               (index * VMS_BLOCK_SIZE));
                if (result == 0) {
                    next = fat_fetch(drive, block);
                    break;
                }
                if (result == BUD_ERR_NO_DISK) {
                    return result;
                }
                moved = 1;
                next = fat_fetch(drive, block);
                fat_store(drive, block, FAT_BAD);
                if ((replacement = fat_find_system(drive, FAT_FREE)) < 0) {
                    return BUD_ERR_DISK_FULL;
                }
                fat_store(drive, replacement, next);
                /* @bug logic: records the new first block in the system
                 * block alone, and leaves media's directory_block on the bad
                 * one (JSR @R1 at 0x8c0c6ea2), as rechain_dir does */
                if (index == 0) {
                    _vm->write_word(record->system_cache, SYS_DIRECTORY_BLOCK,
                                    replacement);
                } else {
                    fat_store(drive, previous, replacement);
                }
                /* @bug logic: breaks the directory's chain where a block past
                 * the first fails, and so does its first replacement. this
                 * store makes the failed block previous (MOV.L R2,@(28,R15) at
                 * 0x8c0c6eb6), so the next try links the second replacement
                 * from it (BSR at 0x8c0c6eb0). the block before the failed one
                 * then links to the first replacement, whose entry is FAT_BAD.
                 * the failed block's own entry links to the second, over its
                 * FAT_BAD */
                previous = block;
                block = replacement;
            }
            previous = block;
            block = next;
            record->dir_unsaved[index] = 0;
        } else {
            next = fat_fetch(drive, block);
            previous = block;
            block = next;
        }
    }
    if (moved) {
        if ((result = write_fat(drive)) < 0) {
            return result;
        }
        if ((result = write_sys(drive)) < 0) {
            return result;
        }
    }
    return 0;
}

static Sint32 name_length(const char *name)
{
    Sint32 index;
    Sint32 length;

    length = 0;
    for (index = 0; index < VMS_NAME_SIZE; index++, name++) {
        if (*name) {
            length++;
        } else {
            break;
        }
    }
    return length;
}

/* 1 where the two names are the same, up to VMS_NAME_SIZE bytes, and 0
 * where not */
static Sint32 name_equal(const char *first, const char *second)
{
    Sint32 index;
    Sint32 first_length;
    Sint32 second_length;

    first_length = name_length(first);
    second_length = name_length(second);
    if (first_length != second_length) {
        return 0;
    }
    for (index = 0; index < first_length; index++) {
        if (first[index] != second[index]) {
            return 0;
        }
    }
    return 1;
}

static void unpack_entry(struct dir_entry *entry, VMS_DIRINFO *output)
{
    Sint32 index;

    output->kind = entry->kind;
    /* @hack spelling: both copies read each byte at entry plus index plus
     * the field's offset, where entry->name[index] and entry->time[index]
     * would do */
    for (index = 0; index < VMS_NAME_SIZE; index++) {
        output->name[index] = *((Uint8 *)entry + index + DIR_NAME);
    }
    output->name[VMS_NAME_SIZE] = 0;
    for (index = 0; index < VMS_TIME_SIZE; index++) {
        output->time[index] = *((Uint8 *)entry + index + DIR_TIME);
    }
    output->first_block = _vm->read_word(entry, DIR_FIRST_BLOCK);
    output->blocks = _vm->read_word(entry, DIR_BLOCKS);
    /* @hack spelling: the copy flag read as the entry's byte at DIR_COPY,
     * where entry->copy would do */
    output->copy = ((Uint8 *)entry)[DIR_COPY];
    output->header = _vm->read_word(entry, DIR_HEADER);
}

/* the index of the file's directory entry, unpacked into output, or -1 */
static Sint32 find_file(Sint32 drive, const char *name, VMS_DIRINFO *output)
{
    Sint32 index;
    /* @hack spelling: kind, a copy of the entry's kind for the switch, where
     * entry->kind would do */
    Uint8 kind;
    struct dir_entry *entry;
    VMS_DRIVE *record = &buVmsDrive[drive];

    entry = (struct dir_entry *)record->directory_cache;
    for (index = 0; index < record->media->user_blocks; index++, entry++) {
        kind = entry->kind;
        switch (kind) {
        case KIND_DATA:
        case KIND_EXEC:
            if (name_equal(name, entry->name)) {
                unpack_entry(entry, output);
                return index;
            }
            break;
        /* @hack redundant: an empty entry's case, which default takes as
         * well */
        case KIND_EMPTY:
            break;
        default:
            break;
        }
    }
    return -1;
}

/* the index of the executable file's directory entry, unpacked into
 * output, or -1 */
static Sint32 find_exec_file(Sint32 drive, VMS_DIRINFO *output)
{
    Sint32 index;
    /* @hack spelling: kind, a copy of the entry's kind for the test, where
     * entry->kind would do */
    Uint8 kind;
    struct dir_entry *entry;
    VMS_DRIVE *record = &buVmsDrive[drive];

    entry = (struct dir_entry *)record->directory_cache;
    for (index = 0; index < record->media->user_blocks; index++, entry++) {
        kind = entry->kind;
        if (kind == KIND_EXEC) {
            unpack_entry(entry, output);
            return index;
        }
    }
    return -1;
}

/* the index of the first empty directory entry, or -1 */
static Sint32 find_free_slot(Sint32 drive)
{
    Sint32 index;
    /* @hack spelling: kind, a copy of the entry's kind for the test, where
     * entry->kind would do */
    Uint8 kind;
    struct dir_entry *entry;
    VMS_DRIVE *record = &buVmsDrive[drive];

    entry = (struct dir_entry *)record->directory_cache;
    for (index = 0; index < record->media->user_blocks; index++, entry++) {
        kind = entry->kind;
        if (kind == KIND_EMPTY) {
            return index;
        }
    }
    return -1;
}

static Sint32 read_sys(Sint32 drive)
{
    VMS_DRIVE *record = &buVmsDrive[drive];

    return vmsfs_read_retry(drive, record->media->system_block,
                            record->system_cache);
}

static Sint32 write_sys(Sint32 drive)
{
    VMS_DRIVE *record = &buVmsDrive[drive];

    return vmsfs_write_retry(drive, record->media->system_block,
                             record->system_cache);
}

static Sint32 read_fat(Sint32 drive)
{
    Sint32 index;
    Sint32 result;
    Sint32 count;
    VMS_DRIVE *record = &buVmsDrive[drive];

    count = record->media->fat_blocks;
    for (index = 0; index < count; index++) {
        if ((result = vmsfs_read_retry(drive, record->media->fat_block + index,
                                       record->fat_cache +
                                           (index * VMS_BLOCK_SIZE))) < 0) {
            return result;
        }
    }
    return 0;
}

/* write the FAT cache to the FAT's blocks once, and mark each block written
 * FAT_LAST. it stops at the first block whose write fails, and marks that
 * block bad. it returns 0 or that failure */
static Sint32 write_fat_once(Sint32 drive)
{
    Sint32 index;
    Sint32 result;
    Sint32 count;
    VMS_DRIVE *record = &buVmsDrive[drive];

    count = record->media->fat_blocks;
    for (index = 0; index < count; index++) {
        if ((result = vmsfs_write_retry(drive, record->media->fat_block + index,
                                        record->fat_cache +
                                            (index * VMS_BLOCK_SIZE))) < 0) {
            fat_store(drive, record->media->fat_block + index, FAT_BAD);
            return result;
        }
        fat_store(drive, record->media->fat_block + index, FAT_LAST);
    }
    return 0;
}

/* the first block of the highest run of fat_blocks free blocks whose top
 * lies in the extra area. the run can reach below the extra area. it
 * returns -1 where no such run exists */
static Sint32 find_fat_run(Sint32 drive)
{
    Sint32 index;
    Sint32 first;
    Sint32 last;
    Sint32 found;
    Sint32 count;
    VMS_DRIVE *record = &buVmsDrive[drive];

    first = record->media->extra_block;
    last = record->media->extra_block + record->media->extra_blocks - 1;
    count = record->media->fat_blocks;
    while (1) {
        if ((found = fat_find_in(drive, FAT_FREE, first, last)) < 0) {
            return -1;
        }
        for (index = 0; index < count; index++) {
            if (fat_fetch(drive, found - index) != FAT_FREE) {
                last = found - index - 1;
                break;
            }
        }
        if (index == count) {
            return found - count + 1;
        }
    }
}

/* write the FAT cache. where a block's write gives BUD_ERR_WRITE_ERROR, the
 * FAT moves to find_fat_run's run. the system block records the move. it
 * returns 0, BUD_ERR_NO_DISK, or BUD_ERR_WRITE_ERROR where no run is free.
 * a write that fails another way gives 0 too, with the FAT partly written */
static Sint32 write_fat(Sint32 drive)
{
    Sint32 index;
    Sint32 result;
    Sint32 replacement;
    Sint32 moved;
    VMS_DRIVE *record = &buVmsDrive[drive];

    moved = 0;
    while (1) {
        result = write_fat_once(drive);
        switch (result) {
        case BUD_ERR_NO_DISK:
            return BUD_ERR_NO_DISK;
        case BUD_ERR_WRITE_ERROR:
            if ((replacement = find_fat_run(drive)) < 0) {
                return BUD_ERR_WRITE_ERROR;
            }
            record->media->fat_block = replacement;
            _vm->write_word(record->system_cache, SYS_FAT_BLOCK, replacement);
            for (index = 0; index < record->media->fat_blocks; index++) {
                fat_store(drive, record->media->fat_block + index, FAT_LAST);
            }
            moved = 1;
            break;
        /* @hack redundant: the case of 0, which default takes as well */
        case 0:
        default:
            if (moved) {
                write_sys(drive);
            }
            return 0;
        }
    }
}

/* the highest user block whose FAT entry is value, or -1 */
static Sint32 fat_find_user(Sint32 drive, Uint16 value)
{
    Sint32 index;
    Sint32 count;
    VMS_DRIVE *record = &buVmsDrive[drive];

    count = record->media->user_blocks;
    for (index = count - 1; index >= 0; index--) {
        if (_vm->read_word(record->fat_cache, index * FAT_ENTRY_SIZE) ==
            value) {
            return index;
        }
    }
    return -1;
}

/* the highest free user block that no file starts at, or -1. a free block
 * above it that a file starts at is marked FAT_LAST as the search passes */
static Sint32 find_free_block(Sint32 drive)
{
    Sint32 index;

    while (1) {
        if ((index = fat_find_user(drive, FAT_FREE)) < 0) {
            return -1;
        }
        if (unclaimed_block(drive, index) < 0) {
            fat_store(drive, index, FAT_LAST);
        } else {
            break;
        }
    }
    return index;
}

/* block, where no file starts at it, or -1 where a file does */
static Sint32 unclaimed_block(Sint32 drive, Uint16 block)
{
    struct dir_entry *entry;
    /* @hack spelling: first_block, which holds the entry's word for the test,
     * where the read itself would do */
    Uint16 first_block;
    Sint32 kind;
    Sint32 index;
    VMS_DRIVE *record = &buVmsDrive[drive];

    for (index = 0; index < record->media->user_blocks; index++) {
        entry = dir_slot(drive, index);
        kind = entry->kind;
        if (kind == KIND_DATA || kind == KIND_EXEC) {
            first_block = _vm->read_word(entry, DIR_FIRST_BLOCK);
            if (block == first_block) {
                return -1;
            }
        }
    }
    return block;
}

/* set the FAT entry at index to value, unless index is past the last block */
static void fat_store(Sint32 drive, Sint32 index, Uint16 value)
{
    VMS_DRIVE *record = &buVmsDrive[drive];

    if (index > record->media->last_block) {
        return;
    }
    _vm->write_word(record->fat_cache, index * FAT_ENTRY_SIZE, value);
}

static Uint16 fat_fetch(Sint32 drive, Sint32 index)
{
    VMS_DRIVE *record = &buVmsDrive[drive];

    return _vm->read_word(record->fat_cache, index * FAT_ENTRY_SIZE);
}

/* read the system block, FAT and directory into the caches, each where it
 * isn't cached yet. it returns 0 or the first read's failure.
 *
 * the reads stop after the system block where vmsfs_isformat fails. a FAT or
 * directory read that gives BUD_ERR_GENERIC sets the drive's
 * buIllegalFormat. the call returns 0 in both cases */
static Sint32 load_caches(Sint32 drive)
{
    Sint32 result;
    VMS_DRIVE *record = &buVmsDrive[drive];

    buIllegalFormat[drive] = 0;
    if (!record->system_cached) {
        if ((result = read_sys(drive)) < 0) {
            return result;
        }
    }
    /* @hack redundant: result stored in the test, which nothing reads */
    if ((result = vmsfs_isformat(drive)) < 0) {
        return 0;
    }
    if (!record->fat_cached) {
        if ((result = read_fat(drive)) < 0) {
            /* @hack spelling: the else after a return, where the second return
             * could follow the if */
            if (result == BUD_ERR_GENERIC) {
                buIllegalFormat[drive] = 1;
                return 0;
            } else {
                return result;
            }
            /* @hack redundant: a return the branches above never reach */
            return result;
        }
    }
    if (!record->directory_cached) {
        if ((result = cache_dir(drive)) < 0) {
            /* @hack spelling: the else after a return, where the second return
             * could follow the if */
            if (result == BUD_ERR_GENERIC) {
                buIllegalFormat[drive] = 1;
                return 0;
            } else {
                return result;
            }
        }
    }
    record->system_cached = 1;
    record->fat_cached = 1;
    record->directory_cached = 1;
    return 0;
}

static struct dir_entry *dir_slot(Sint32 drive, Sint32 index)
{
    VMS_DRIVE *record = &buVmsDrive[drive];

    return (struct dir_entry *)record->directory_cache + index;
}

static void dir_mark_unsaved(Sint32 drive, Sint32 index)
{
    VMS_DRIVE *record = &buVmsDrive[drive];

    record->dir_unsaved[index >> DIR_ENTRY_SHIFT] = 1;
}

/* a free file record, marked open, or null where every record is open */
static BU_FILE *alloc_file(Sint32 drive)
{
    Sint32 index;
    BU_FILE *file = files;

    for (index = 0; index < file_limit; index++, file++) {
        if (!file->open) {
            file->open = 1;
            file->index = index;
            return file;
        }
    }
    dat_8c276a10 = BUD_ERR_CANNOT_OPEN;
    return NULL;
}

static void free_file(BU_FILE *file)
{
    memset(file, 0, sizeof(*file));
}

static Sint32 check_name(Sint32 drive, const char *name)
{
    return 0;
}

/* 0 where the file's chain ends after as many user blocks as its directory
 * entry counts. it returns BUD_ERR_FILE_BROKEN where the chain doesn't, and
 * BUD_ERR_FILE_NOT_FOUND where there is no file */
static Sint32 check_chain(Sint32 drive, const char *name)
{
    Sint32 index;
    Sint32 block;
    Sint32 next;
    Sint32 slot;
    /* @hack redundant: entry, a local check_chain stores and never reads */
    struct dir_entry
        *entry; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    VMS_DIRINFO info;
    VMS_DRIVE *record = &buVmsDrive[drive];

    if ((slot = find_file(drive, name, &info)) < 0) {
        return BUD_ERR_FILE_NOT_FOUND;
    }
    entry = dir_slot(drive, slot);
    block = info.first_block;
    if (block >= record->media->user_blocks) {
        return BUD_ERR_FILE_BROKEN;
    }
    for (index = 0; index < info.blocks; index++) {
        next = fat_fetch(drive, block);
        if (index == info.blocks - 1) {
            if (next != FAT_LAST) {
                return BUD_ERR_FILE_BROKEN;
            }
        } else {
            if (next == block) {
                return BUD_ERR_FILE_BROKEN;
            }
            if (next == FAT_LAST) {
                return BUD_ERR_FILE_BROKEN;
            }
            if (next == FAT_FREE) {
                return BUD_ERR_FILE_BROKEN;
            }
            if (next == FAT_BAD) {
                return BUD_ERR_FILE_BROKEN;
            }
            if (next >= record->media->user_blocks) {
                return BUD_ERR_FILE_BROKEN;
            }
        }
        block = next;
    }
    return 0;
}

static Uint8 pack_bcd(Uint32 value)
{
    Uint32 tens;
    Uint32 units;

    tens = value / 10;
    units = value % 10;
    /* @hack type: the casts to char and Uint8, where (tens << 4) + units
     * would do */
    return ((char)tens << 4) + (Uint8)units;
}

static Uint16 unpack_bcd(Uint32 value)
{
    Sint32 tens;
    Sint32 units;

    tens = value >> 4;
    units = value & 15;
    return (tens * 10) + units;
}

/* pack time into output's VMS_TIME_SIZE bytes, as the card keeps a time */
static void pack_time(Uint8 *output, const BUS_TIME *time)
{
    /* the card's number for each of BUS_TIME's days of the week. the card
     * counts from BUS_TIME's day 1, so BUS_TIME's day 0 is the card's 6.
     * disk_weekday[7] names no day, and holds 255 */
    static const Uint8 disk_weekday[8] = {6, 0, 1, 2, 3, 4, 5, 255};

    *output++ = pack_bcd(time->year / 100);
    *output++ = pack_bcd(time->year % 100);
    *output++ = pack_bcd(time->month);
    *output++ = pack_bcd(time->day);
    *output++ = pack_bcd(time->hour);
    *output++ = pack_bcd(time->minute);
    *output++ = pack_bcd(time->second);
    *output++ = disk_weekday[time->dayofweek & 7];
}

static void unpack_time(BUS_TIME *time, const Uint8 *input)
{
    /* BUS_TIME's day for each of the card's, the inverse of disk_weekday.
     * the card's 255, masked to 7, gives 255 back */
    static const Uint8 api_weekday[8] = {1, 2, 3, 4, 5, 6, 0, 255};

    time->year = (unpack_bcd(input[0]) * 100) + unpack_bcd(input[1]);
    time->month = unpack_bcd(input[2]);
    time->day = unpack_bcd(input[3]);
    time->hour = unpack_bcd(input[4]);
    time->minute = unpack_bcd(input[5]);
    time->second = unpack_bcd(input[6]);
    time->dayofweek = api_weekday[input[7] & 7];
}

/* set the drive's capacity class from the card's layout. it returns 0, or -1
 * for a layout of no class or one the drive can't take. the system block
 * must be the card's last. the FAT, directory and executable area must start
 * on the card. with vmsfs_init's workspace, the drive's caches must hold the
 * FAT and the directory too */
static Sint32 check_media(VMS_DRIVE *record)
{
    VMS_MEDIA *media = record->media;

    if (media->last_block < 1 || media->last_block > CLASS_3_LAST_BLOCK) {
        return -1;
    }
    if (media->last_block != media->system_block) {
        return -1;
    }
    if (media->fat_block < 0 || media->fat_block >= media->last_block) {
        return -1;
    }
    if (media->directory_block < 0 ||
        media->directory_block >= media->last_block) {
        return -1;
    }
    if (media->exec_block < 0 || media->exec_block > media->last_block) {
        return -1;
    }
    if (media->exec_blocks < 0 || media->exec_blocks > media->last_block) {
        return -1;
    }
    if (_vm->separate_workspaces == 0) {
        if (record->fat_capacity < media->fat_blocks) {
            return -1;
        }
        if (record->directory_capacity < media->directory_blocks) {
            return -1;
        }
    }
    switch (media->last_block) {
    case CLASS_0_LAST_BLOCK:
        record->capacity_class = 0;
        break;
    case CLASS_1_LAST_BLOCK:
        record->capacity_class = 1;
        break;
    case CLASS_2_LAST_BLOCK:
        record->capacity_class = 2;
        break;
    case CLASS_3_LAST_BLOCK:
        record->capacity_class = 3;
        break;
    default:
        return -1;
    }
    return 0;
}

Sint32 vmsfs_read_disk_image(Sint32 drive, void *buffer)
{
    Sint32 index;
    Sint32 result;
    char *cursor;
    VMS_DRIVE *record = &buVmsDrive[drive];

    cursor = buffer;
    PROGRESS_START(drive, PROGRESS_STEPS(BUD_OP_READIMAGE));
    for (index = 0; index < record->media->last_block + 1; index++) {
        if ((result = vmsfs_read_retry(drive, index, cursor)) < 0) {
            return result;
        }
        cursor += VMS_BLOCK_SIZE;
    }
    return 0;
}

Sint32 vmsfs_write_disk_image(Sint32 drive, const void *buffer)
{
    Sint32 index;
    Sint32 result;
    const char *cursor;
    VMS_DRIVE *record = &buVmsDrive[drive];

    cursor = buffer;
    PROGRESS_START(drive, PROGRESS_STEPS(BUD_OP_WRITEIMAGE));
    for (index = 0; index < record->media->last_block + 1; index++) {
        if ((result = vmsfs_write_retry(drive, index, cursor)) < 0) {
            return result;
        }
        cursor += VMS_BLOCK_SIZE;
    }
    return 0;
}

Sint32 vmsfs_load_file(Sint32 drive, const char *name, void *buffer,
                       Uint32 block_count)
{
    Uint32 index;
    Sint32 result;
    Sint32 file_blocks;
    Uint8 *cursor;
    BU_FILE *file;

    if (!block_count) {
        if ((file_blocks = vmsfs_fgetsize(drive, name)) < 0) {
            return file_blocks;
        }
        block_count = file_blocks;
    }
    if ((result = vmsfs_fopen(drive, name, &file)) < 0) {
        return result;
    }
#ifdef NONMATCHING
    /* The directory may have changed since the caller sized its buffer.
     * fread1 also returns zero when already at EOF, without copying. */
    if (file->remaining_blocks < 0 ||
        block_count > (Uint32)file->remaining_blocks) {
        result = BUD_ERR_FILE_BROKEN;
        goto close_file;
    }
#endif
    cursor = buffer;
    PROGRESS_START(drive, block_count);
    for (index = 0; index < block_count; index++) {
        if ((result = vmsfs_fread1(file, cursor)) < 0) {
            goto close_file;
        }
        cursor += VMS_BLOCK_SIZE;
    }
    result = 0;
close_file:
    vmsfs_fclose(file, 0);
    return result;
}

Sint32 vmsfs_save_file(Sint32 drive, const char *name, const void *buffer,
                       Uint32 block_count, BUS_TIME *timestamp, Sint32 flags)
{
    Uint32 index;
    Sint32 result;
    Sint32 free_blocks;
    Sint32 replaced_blocks;
    /* @hack spelling: verify_requested, which holds the test's flag, where
     * flags & BUD_FLAG_VERIFY would do */
    Sint32 verify_requested;
    Sint32 saved_lock;
    const Uint8 *cursor;
    BU_FILE *file;

    free_blocks = vmsfs_diskfree(drive, BUD_FILETYPE_NORMAL);
    if ((replaced_blocks = vmsfs_fgetsize(drive, name)) > 0) {
        free_blocks += replaced_blocks;
    }
    if (free_blocks < 0) {
        return free_blocks;
    }
    if ((Uint32)free_blocks < block_count) {
        return BUD_ERR_DISK_FULL;
    }
    verify_requested = flags & BUD_FLAG_VERIFY;
    if (verify_requested) {
        return vmsfs_save_file_verify(drive, name, buffer, block_count,
                                      timestamp, flags);
    }
    if ((result = vmsfs_fcreate(drive, name, timestamp, &file)) < 0) {
        return result;
    }
    file->copy = flags;
    cursor = buffer;
    PROGRESS_START(drive, block_count + PROGRESS_STEPS(BUD_OP_SAVEFILE));
    vmsfs_getbankinfo(drive, NULL, &saved_lock, NULL);
    vmsfs_lockbank(drive, BANK_LOCKED);
    for (index = 0; index < block_count; index++) {
        if ((result = vmsfs_fwrite1(file, cursor)) < 0) {
            goto close_file;
        }
        cursor += VMS_BLOCK_SIZE;
    }
    result = 0;
close_file:
    vmsfs_fclose(file, 0);
    vmsfs_lockbank(drive, saved_lock);
    return result;
}

Sint32 vmsfs_save_file_verify(Sint32 drive, const char *name,
                              const void *buffer, Uint32 block_count,
                              BUS_TIME *timestamp, Sint32 flags)
{
    Uint32 index;
    Sint32 result;
    Sint32 saved_lock;
    const Uint8 *cursor;
    BU_FILE *file;

    if ((result = vmsfs_fcreate(drive, name, timestamp, &file)) < 0) {
        return result;
    }
    file->copy = flags;
    cursor = buffer;
    PROGRESS_START(drive, (block_count * VERIFY_STEPS) +
                              PROGRESS_STEPS(BUD_OP_SAVEFILE));
    vmsfs_getbankinfo(drive, NULL, &saved_lock, NULL);
    vmsfs_lockbank(drive, BANK_LOCKED);
    for (index = 0; index < block_count; index++) {
        if ((result = vmsfs_fwrite1(file, cursor)) < 0) {
            goto close_file;
        }
        cursor += VMS_BLOCK_SIZE;
    }
    /* @hack redundant: the store of 0 to result, which the open's result
     * replaces */
    result = 0;
    vmsfs_fclose(file, 0);
    if ((result = vmsfs_fopen(drive, name, &file)) < 0) {
        vmsfs_lockbank(drive, saved_lock);
        return result;
    }
    cursor = buffer;
    for (index = 0; index < block_count; index++) {
        if ((result = vmsfs_fread1(file, _vms_verify_buf[drive])) < 0) {
            goto close_file;
        }
        if (vmsfs_memcmp((const Uint8 *)_vms_verify_buf[drive], cursor)) {
            result = BUD_ERR_VERIFY;
            goto close_file;
        }
        cursor += VMS_BLOCK_SIZE;
    }
    result = 0;
close_file:
    vmsfs_fclose(file, 0);
    vmsfs_lockbank(drive, saved_lock);
    return result;
}

Sint32 vmsfs_save_exec_file(Sint32 drive, const char *name, const void *buffer,
                            Uint32 block_count, BUS_TIME *timestamp,
                            Sint32 flags)
{
    Uint32 index;
    Sint32 result;
    Sint32 free_blocks;
    /* @hack spelling: verify_requested, which holds the test's flag, where
     * flags & BUD_FLAG_VERIFY would do */
    Sint32 verify_requested;
    Sint32 saved_lock;
    const Uint8 *cursor;
    BU_FILE *file;

    if ((free_blocks = vmsfs_diskfree(drive, BUD_FILETYPE_EXECUTABLE)) < 0) {
        return free_blocks;
    }
    if ((Uint32)free_blocks < block_count) {
        return BUD_ERR_DISK_FULL;
    }
    verify_requested = flags & BUD_FLAG_VERIFY;
    if (verify_requested) {
        return vmsfs_save_exec_file_verify(drive, name, buffer, block_count,
                                           timestamp, flags);
    }
    if ((result = vmsfs_xcreate(drive, name, timestamp, &file)) < 0) {
        return result;
    }
    file->copy = flags;
    cursor = buffer;
    PROGRESS_START(drive, block_count + PROGRESS_STEPS(BUD_OP_SAVEFILE));
    vmsfs_getbankinfo(drive, NULL, &saved_lock, NULL);
    vmsfs_lockbank(drive, BANK_LOCKED);
    for (index = 0; index < block_count; index++) {
        if ((result = vmsfs_xwrite1(file, cursor)) < 0) {
            goto close_file;
        }
        cursor += VMS_BLOCK_SIZE;
    }
    result = 0;
close_file:
    vmsfs_fclose(file, 0);
    vmsfs_lockbank(drive, saved_lock);
    return result;
}

Sint32 vmsfs_save_exec_file_verify(Sint32 drive, const char *name,
                                   const void *buffer, Uint32 block_count,
                                   BUS_TIME *timestamp, Sint32 flags)
{
    Uint32 index;
    Sint32 result;
    Sint32 saved_lock;
    const Uint8 *cursor;
    BU_FILE *file;

    if ((result = vmsfs_xcreate(drive, name, timestamp, &file)) < 0) {
        return result;
    }
    file->copy = flags;
    cursor = buffer;
    PROGRESS_START(drive, (block_count * VERIFY_STEPS) +
                              PROGRESS_STEPS(BUD_OP_SAVEFILE));
    vmsfs_getbankinfo(drive, NULL, &saved_lock, NULL);
    vmsfs_lockbank(drive, BANK_LOCKED);
    for (index = 0; index < block_count; index++) {
        if ((result = vmsfs_xwrite1(file, cursor)) < 0) {
            goto close_file;
        }
        cursor += VMS_BLOCK_SIZE;
    }
    /* @hack redundant: the store of 0 to result, which the open's result
     * replaces */
    result = 0;
    vmsfs_fclose(file, 0);
    if ((result = vmsfs_fopen(drive, name, &file)) < 0) {
        vmsfs_lockbank(drive, saved_lock);
        return result;
    }
    cursor = buffer;
    for (index = 0; index < block_count; index++) {
        if ((result = vmsfs_fread1(file, _vms_verify_buf[drive])) < 0) {
            goto close_file;
        }
        if (vmsfs_memcmp((const Uint8 *)_vms_verify_buf[drive], cursor)) {
            result = BUD_ERR_VERIFY;
            goto close_file;
        }
        cursor += VMS_BLOCK_SIZE;
    }
    result = 0;
close_file:
    vmsfs_fclose(file, 0);
    vmsfs_lockbank(drive, saved_lock);
    return result;
}

Sint32 vmsfs_fseek(BU_FILE *file, Uint32 count, Sint32 origin)
{
    /* @hack spelling: block, which holds next_block for the fetch, where
     * fat_fetch(drive, file->next_block) would do */
    Sint32 block;
    Sint32 drive;
    Uint32 index;

    drive = file->drive;
    file->next_block = file->first_block;
    for (index = 0; index < count; index++) {
        /* @hack spelling: the else after a return, where the second return
         * could follow the if */
        if (file->next_block == FAT_LAST) {
            if (file->remaining_blocks == 0) {
                return 0;
            } else {
                return BUD_ERR_GENERIC;
            }
        }
        if (file->remaining_blocks == 0) {
            return BUD_ERR_GENERIC;
        }
        block = file->next_block;
        file->next_block = fat_fetch(drive, block);
        file->remaining_blocks--;
    }
    return file->remaining_blocks;
}

Sint32 vmsfs_load_file_ex(Sint32 drive, const char *name, void *buffer,
                          Uint32 first_block, Uint32 block_count)
{
    Uint32 index;
    Sint32 result;
    Sint32 file_blocks;
    Uint8 *cursor;
    BU_FILE *file;

    if (!block_count) {
        if ((file_blocks = vmsfs_fgetsize(drive, name)) < 0) {
            return file_blocks;
        }
#ifdef NONMATCHING
        /* this side refuses a first block past the file's last, from which
         * block_count would wrap */
        if (first_block > (Uint32)file_blocks) {
            return BUD_ERR_INVALID_PARAM;
        }
#endif
        block_count = file_blocks - first_block;
    }
    if ((result = vmsfs_fopen(drive, name, &file)) < 0) {
        return result;
    }
#ifdef NONMATCHING
    /* A queued read can open a different file than the caller sized.
     * fread1 returns zero at EOF even when no block was copied. */
    if (first_block > (Uint32)file->remaining_blocks ||
        block_count > (Uint32)file->remaining_blocks - first_block) {
        result = BUD_ERR_INVALID_PARAM;
        goto close_file;
    }
#endif
    cursor = buffer;
    PROGRESS_START(drive, block_count);
#ifdef NONMATCHING
    /* this side fails where the seek fails, which the matching side ignores */
    if ((result = vmsfs_fseek(file, first_block, 0)) < 0) {
        goto close_file;
    }
#else
    vmsfs_fseek(file, first_block, 0);
#endif
    for (index = 0; index < block_count; index++) {
        if ((result = vmsfs_fread1(file, cursor)) < 0) {
            goto close_file;
        }
        cursor += VMS_BLOCK_SIZE;
    }
    result = 0;
close_file:
    vmsfs_fclose(file, 0);
    return result;
}

Sint32 vmsfs_scanfile(Sint32 drive, const char *name, Uint16 *chain)
{
    Sint32 index;
    Sint32 result;
    Sint32 block;
    /* @hack spelling: next_block, which holds each link on its way to block,
     * where block = fat_fetch(drive, block) would do */
    Sint32 next_block;
    Sint32 slot;
    /* @hack redundant: entry, a local vmsfs_scanfile stores and never
     * reads */
    struct dir_entry
        *entry; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    VMS_DIRINFO info;

    if ((slot = find_file(drive, name, &info)) < 0) {
        return BUD_ERR_FILE_NOT_FOUND;
    }
    entry = dir_slot(drive, slot);
    block = info.first_block;
    *chain++ = block;
    /* @bug memory: reads past the FAT cache's end where the file's chain is
     * broken. the loop follows the chain before check_chain tests it (BSR at
     * 0x8c0c886c). a FAT_LAST, FAT_FREE or FAT_BAD before the chain's end, or
     * a link past the card, is then taken for a block. fat_fetch reads that
     * block's entry (JSR @R3 at 0x8c0c8836). the game never calls
     * vmsfs_scanfile: no word in any image holds its address */
    for (index = 0; index < info.blocks; index++) {
        next_block = fat_fetch(drive, block);
        block = next_block;
        *chain++ = block;
    }
    if ((result = check_chain(drive, name)) < 0) {
        return result;
    }
    return slot;
}

Sint32 vmsfs_rewrite_exec_file(Sint32 drive, const char *name,
                               const void *buffer, Uint32 first_block,
                               Uint32 block_count)
{
    Sint32 index;
    Sint32 result;
    Sint32 saved_lock;
    const Uint8 *cursor;
    /* @hack redundant: record, a local vmsfs_rewrite_exec_file stores and
     * never reads */
    VMS_DRIVE *record = &buVmsDrive[drive];
    BUS_FILEINFO info;

    cursor = buffer;
    PROGRESS_START(drive, block_count);
    if ((result = vmsfs_fgetinfo(drive, name, &info)) < 0) {
        return result;
    }
    if (info.type != BUD_FILETYPE_EXECUTABLE) {
        return BUD_ERR_ACCESS_DENIED;
    }
    if (first_block >= info.blocks) {
        return BUD_ERR_INVALID_PARAM;
    }
    if (block_count > info.blocks) {
        return BUD_ERR_INVALID_PARAM;
    }
    if (first_block + block_count > info.blocks) {
        return BUD_ERR_INVALID_PARAM;
    }
    if ((result = check_chain(drive, name)) < 0) {
        return result;
    }
    vmsfs_getbankinfo(drive, NULL, &saved_lock, NULL);
    vmsfs_lockbank(drive, BANK_UNLOCKED);
    for (index = 0; (Uint32)index < block_count; index++) {
        if ((result = vmsfs_write_retry(drive, first_block + index, cursor)) <
            0) {
            goto close_file;
        }
        cursor += VMS_BLOCK_SIZE;
    }
close_file:
    vmsfs_lockbank(drive, saved_lock);
    return result;
}

Sint32 vmsfs_foverwrite1(BU_FILE *file, const void *buffer)
{
    Sint32 block;
    Sint32 drive;
    Sint32 result;

    if (!vmsio_isready(file->drive)) {
        vmsfs_unmount(file->drive);
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(file->drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[file->drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if (buIsFormat(file->drive) < 0) {
        return BUD_ERR_UNFORMAT;
    }
    drive = file->drive;
    /* @hack spelling: the else after a return, where the second return could
     * follow the if */
    if (file->next_block == FAT_LAST) {
        if (file->remaining_blocks == 0) {
            return 0;
        } else {
            return BUD_ERR_GENERIC;
        }
    }
    if (file->remaining_blocks == 0) {
        return 0;
    }
    if (file->next_block == FAT_FREE) {
        return BUD_ERR_FILE_BROKEN;
    }
    if (file->next_block == FAT_BAD) {
        return BUD_ERR_FILE_BROKEN;
    }
    block = file->next_block;
    if ((result = vmsfs_write_retry(drive, block, buffer)) < 0) {
        return result;
    }
    file->next_block = fat_fetch(drive, block);
    file->remaining_blocks--;
    return file->remaining_blocks;
}

Sint32 vmsfs_rewrite_file(Sint32 drive, const char *name, const void *buffer,
                          Uint32 first_block, Uint32 block_count)
{
    Uint32 index;
    Sint32 result;
    Sint32 file_blocks;
    Sint32 saved_lock;
    const Uint8 *cursor;
    BUS_FILEINFO info;
    BU_FILE *file;

    cursor = buffer;
    PROGRESS_START(drive, block_count);
    if ((result = vmsfs_fgetinfo(drive, name, &info)) < 0) {
        return result;
    }
    if (info.type != BUD_FILETYPE_NORMAL) {
        return BUD_ERR_ACCESS_DENIED;
    }
    if (!block_count) {
        if ((file_blocks = vmsfs_fgetsize(drive, name)) < 0) {
            return file_blocks;
        }
        block_count = file_blocks - first_block;
    }
    if (first_block >= info.blocks) {
        return BUD_ERR_INVALID_PARAM;
    }
    if (block_count > info.blocks) {
        return BUD_ERR_INVALID_PARAM;
    }
    if (first_block + block_count > info.blocks) {
        return BUD_ERR_INVALID_PARAM;
    }
    if ((result = check_chain(drive, name)) < 0) {
        return result;
    }
    if ((result = vmsfs_fopen(drive, name, &file)) < 0) {
        return result;
    }
    vmsfs_getbankinfo(drive, NULL, &saved_lock, NULL);
    vmsfs_lockbank(drive, BANK_UNLOCKED);
    vmsfs_fseek(file, first_block, 0);
    cursor = buffer;
    PROGRESS_START(drive, block_count);
    for (index = 0; index < block_count; index++) {
        if ((result = vmsfs_foverwrite1(file, cursor)) < 0) {
            goto close_file;
        }
        cursor += VMS_BLOCK_SIZE;
    }
close_file:
    vmsfs_fclose(file, 0);
    vmsfs_lockbank(drive, saved_lock);
    /* @bug logic: returns 0 even where a block failed to write.
     * vmsfs_foverwrite1's failure (BRA at 0x8c0c8cce) reaches the success
     * path's MOV #0,R0 at 0x8c0c8cfc */
    return 0;
}

Sint32 vmsfs_getbankinfo(Sint32 drive, Sint32 *current, Sint32 *locked,
                         Sint32 *count)
{
    /* @hack redundant: record, a local vmsfs_getbankinfo stores and never
     * reads */
    VMS_DRIVE *record = &buVmsDrive[drive];

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    /* this passes null for count, so *count is never set. no caller passes a
     * count: buGetBankInfo's command and every call in this file pass null */
    return vmsfs_bank_info_retry(drive, current, locked, NULL);
}

Sint32 vmsfs_changebank(Sint32 drive, Sint32 bank)
{
    Sint32 result;

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    if ((result = vmsfs_bank_change_retry(drive, bank)) != 0) {
        return result;
    }
    return vmsfs_mount(drive);
}

Sint32 vmsfs_lockbank(Sint32 drive, Sint32 locked)
{
    /* @hack redundant: unused_local, a local that only sizes the frame */
    Sint32 unused_local;

    if (!buVmsDrive[drive].enabled) {
        return BUD_ERR_NO_DISK;
    }
    if (!vmsio_ismount(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (!buVmsDrive[drive].mounted) {
        return BUD_ERR_NO_DISK;
    }
    return vmsfs_bank_lock_retry(drive, locked);
}
