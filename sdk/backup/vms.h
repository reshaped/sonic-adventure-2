/* the backup library's own declarations, in layers. vmsio.c's card calls
 * reach a memory card through Kaede, the Maple bus driver. vms_rw.c tries
 * each again after a failure, and vmsphys.c's file system works over those
 * retried calls. a call of vmsapi.c's that reads or writes the card queues a
 * command, which its drive's fiber runs */
#ifndef BACKUP_VMS_H
#define BACKUP_VMS_H
#include <sg_bup.h>
#include <sg_xpt.h>

/* the drives, two for each of the four ports: one for each expansion
 * socket of the port's controller */
#define VMS_DRIVES 8

/* the bytes of a card's block */
#define VMS_BLOCK_SIZE 512
/* the bytes of the guard that opens a drive's workspace */
#define VMS_GUARD_SIZE 32

/* the most bytes of a file's name, without its null */
#define VMS_NAME_SIZE 12
/* the bytes a record keeps for a name and its null, which take at most
 * VMS_NAME_SIZE + 1 */
#define VMS_NAME_BUFFER 16
/* the bytes of a volume label, which has no null */
#define VMS_LABEL_SIZE 32
/* the bytes of a time as the card keeps it. the first seven hold the
 * century, year, month, day, hour, minute and second, in BCD. the last
 * holds the card's day of the week */
#define VMS_TIME_SIZE 8

/* the directory blocks a drive's record keeps a flag for */
#define VMS_DIR_BLOCKS_MAX 256

/* the byte offsets of a system block's fields. the volume label, the time,
 * SYS_ICON and SYS_SORT are bytes. each other field is a 16-bit word in the
 * card's byte order */
#define SYS_VOLUME 16
#define SYS_TIME 48
#define SYS_LAST_BLOCK 64
#define SYS_PARTITION 66
#define SYS_SYSTEM_BLOCK 68
#define SYS_FAT_BLOCK 70
#define SYS_FAT_BLOCKS 72
#define SYS_DIRECTORY_BLOCK 74
#define SYS_DIRECTORY_BLOCKS 76
#define SYS_ICON 78
#define SYS_SORT 79
#define SYS_EXTRA_BLOCK 80
#define SYS_EXTRA_BLOCKS 82
#define SYS_EXEC_BLOCK 84
#define SYS_EXEC_BLOCKS 86

/* a card's layout, as vmsio_get_media_info reads it from the card. each
 * area is a first block and a count of blocks */
typedef struct {
    Sint32 last_block; /* the number of the card's last block, not a count */
    Sint32 partition;
    Sint32 system_block;
    Sint32 fat_block;
    Sint32 fat_blocks;
    Sint32 directory_block;
    Sint32 directory_blocks;
    Sint32 extra_block; /* the first block past the user area */
    Sint32 extra_blocks;
    Sint32 icon; /* the system block's SYS_ICON byte */
    Sint32 sort; /* the system block's SYS_SORT byte */
    Sint32 last_byte; /* the offset of a block's last byte */
    /* the user area's blocks, which run from block 0. a mount sets the count
     * to extra_block */
    Sint32 user_blocks;
    Sint32 exec_block;
    Sint32 exec_blocks;
    /* a block's read takes read_count accesses of read_bytes each, and its
     * write takes write_count accesses of write_bytes. the card gives each
     * count, 0 to 15, and vmsio_get_media_info divides a block's bytes by it.
     * a count of 0 gives 0 bytes, as SHC's division by 0 does */
    Sint32 read_count;
    Sint32 write_count;
    Sint32 read_bytes;
    Sint32 write_bytes;
} VMS_MEDIA;

/* the file system's settings, which every drive shares */
typedef struct {
    Sint32 byte_order; /* vmsfs_setdevicetype's bit: 1 for big-endian words */
    /* the capacity class that vmsfs_init's capacity names, 0 to 3 */
    Sint32 cache_class;
    /* 1 where vmsfs_init took no workspace, so that each drive takes its own
     * from vmsfs_setwork */
    Sint32 separate_workspaces;
    /* read or write the 16-bit word at offset in buffer, in the card's byte
     * order */
    Uint16 (*read_word)(void *buffer, Sint32 offset);
    void (*write_word)(void *buffer, Sint32 offset, Uint16 value);
} VMS_CONTEXT;
/* the settings, and _vm, which vmsfs_init points at them */
extern VMS_CONTEXT buVmsContext;
extern VMS_CONTEXT *_vm;

/* an open file's modes */
#define VMS_FILE_READ 0
#define VMS_FILE_WRITE 1
#define VMS_FILE_WRITE_EXEC 2

/* an open file. a file being written keeps its name, time, copy flag and
 * header offset till vmsfs_fclose writes them to its directory entry */
typedef struct bu_file {
    char open; /* 1 while the record holds an open file */
    Uint8 copy;
    Uint16 header; /* the header's offset, in blocks */
    Sint32 mode; /* a VMS_FILE_ mode */
    Sint32 type; /* BUD_FILETYPE_NORMAL or BUD_FILETYPE_EXECUTABLE */
    Sint32 index; /* the record's index among the file system's file records */
    char name[VMS_NAME_BUFFER];
    Uint8 time[VMS_TIME_SIZE];
    Sint32 drive;
    Sint32 directory_slot; /* the entry a written file takes as it closes */
    Sint32 written_blocks;
    Sint32 remaining_blocks; /* the blocks a read has still to read */
    Sint32 first_block;
    Sint32 previous_block; /* the block written last */
    Sint32 next_block; /* the block a read reads next */
    Sint32 unk_44;
} BU_FILE;

/* a file's directory entry, unpacked. kind is the entry's kind byte */
typedef struct {
    Sint32 unk_00;
    Sint32 kind;
    Uint16 header; /* the header's offset, in blocks */
    Uint8 copy;
    char unk_0b;
    Sint32 first_block;
    char name[VMS_NAME_BUFFER];
    Uint8 time[VMS_TIME_SIZE];
    Sint32 blocks;
} VMS_DIRINFO;

/* a drive's record. its caches hold the card's system block, FAT and
 * directory, in a workspace that opens with the guard */
typedef struct {
    Sint16 drive;
    /* 1 for a drive with a workspace, from vmsfs_init or vmsfs_setwork */
    char enabled;
    Sint32 capacity_class; /* the card's capacity class, 0 to 3 */
    char mounted;
    /* 1 once vmsfs_mount has read the card's layout, or vmsfs_connect its
     * layout and block 0, and after vmsfs_setwork. a failed read of the
     * layout and vmsfs_unmount set it to 0 */
    char connected;
    char system_cached;
    char fat_cached;
    char directory_cached;
    Sint16 system_capacity; /* the blocks system_cache holds */
    Sint16 fat_capacity; /* the FAT blocks fat_cache holds */
    Sint16 directory_capacity; /* the blocks directory_cache holds */
    Sint16 guard_size; /* VMS_GUARD_SIZE, the bytes at guard */
    /* the entry after the last that vmsfs_findfirst or vmsfs_findnext
     * found */
    Sint32 directory_cursor;
    VMS_MEDIA *media;
    Uint8 *system_cache;
    Uint8 *fat_cache;
    Uint8 *directory_cache;
    Uint8 *guard; /* the workspace's first VMS_GUARD_SIZE bytes, each 'G' */
    Sint32 progress_done; /* the steps of the drive's command done */
    /* the steps the drive's command takes. each failed try of a card call
     * adds one */
    Sint32 progress_total;
    char unk_38[16];
    /* 1 for each directory block changed since it was last written */
    char dir_unsaved[VMS_DIR_BLOCKS_MAX];
    Sint32 unk_148[3];
} VMS_DRIVE;
extern VMS_DRIVE buVmsDrive[VMS_DRIVES];

/* 1 where the drive holds a card, and 0 where not */
Sint32 vmsio_isready(Sint32 drive);
/* 1 where the drive holds a card that hasn't changed, and 0 where not. a
 * change counts till vmsio_get_media_info or a bank call next asks the card */
Sint32 vmsio_ismount(Sint32 drive);
/* vmsio_init removes vmsio.c's error hook, vmsio_sim_error. vmsio_exit does
 * nothing */
void vmsio_init(void);
void vmsio_exit(void);
/* the card calls, which wait for the card's answer. each must run on a
 * drive's fiber, which it sleeps while it waits. each returns 0 where it
 * succeeds, and VMSIO_CHANGED where the card changes during the call. each
 * returns BUD_ERR_NO_DISK where the drive holds no card, or where the card's
 * response is unexpected. the bank calls below are card calls too.
 *
 * vmsio_block_read and vmsio_block_write return BUD_ERR_NO_DISK too where the
 * card changed before the call. a file error from the card gives
 * BUD_ERR_GENERIC from vmsio_block_read, and BUD_ERR_WRITE_ERROR from
 * vmsio_block_write */
#define VMSIO_CHANGED ((Sint32)0xffffff30)
Sint32 vmsio_get_media_info(Sint32 drive, VMS_MEDIA *media);
Sint32 vmsio_block_read(Sint32 drive, Sint32 block, void *buffer);
Sint32 vmsio_block_write(Sint32 drive, Sint32 block, const void *buffer);
/* the bank calls. a card has banks where its product name opens with BANK.
 * on any other card, each returns 0 without asking it.
 *
 * vmsio_bank_change selects bank, and vmsio_bank_lock locks the card's bank,
 * or unlocks it where locked is 0. each returns BUD_ERR_GENERIC where the
 * card then reports another bank or lock than it asked for. a file error
 * from the card gives BUD_ERR_LOCKED from vmsio_bank_change, and
 * BUD_ERR_GENERIC from vmsio_bank_lock and vmsio_bank_info.
 *
 * vmsio_bank_info stores the bank in use in *current, and the count of banks
 * in *count. it stores 1 in *locked for a locked bank, and 0 for another. a
 * card without banks reads as bank 0 of 1, unlocked. the call skips each null
 * pointer, and stores nothing where it fails */
Sint32 vmsio_bank_change(Sint32 drive, Sint32 bank);
Sint32 vmsio_bank_lock(Sint32 drive, Sint32 locked);
Sint32 vmsio_bank_info(Sint32 drive, Sint32 *current, Sint32 *locked,
                       Sint32 *count);

/* set how many more tries each retried call makes after a failure, and
 * return count. a count of -1 makes each try till it succeeds */
Sint32 vmsfs_setretry(Sint32 count);
/* the card calls, each tried again after a failure as vmsfs_setretry sets.
 * each returns 0 once a try succeeds, and BUD_ERR_NO_DISK at once where the
 * card changes during a try. once the tries run out, each returns the last
 * try's failure. BUD_ERR_NO_DISK stays as it is, and any other becomes
 * BUD_ERR_GENERIC. vmsfs_write_retry keeps BUD_ERR_WRITE_ERROR too, and
 * vmsfs_bank_change_retry BUD_ERR_LOCKED.
 *
 * each failed try of the media, read and write calls adds a step to the
 * drive's progress_total. as its tries end, such a call reports the drive's
 * progress with vmsfs_c_progress, and counts a step done. a change of card
 * ends the call without a report */
Sint32 vmsfs_mediainfo_retry(Sint32 drive, VMS_MEDIA *media);
Sint32 vmsfs_read_retry(Sint32 drive, Sint32 block, void *buffer);
Sint32 vmsfs_write_retry(Sint32 drive, Sint32 block, const void *buffer);
Sint32 vmsfs_bank_change_retry(Sint32 drive, Sint32 bank);
Sint32 vmsfs_bank_lock_retry(Sint32 drive, Sint32 locked);
Sint32 vmsfs_bank_info_retry(Sint32 drive, Sint32 *current, Sint32 *locked,
                             Sint32 *count);

/* the file system, over the retried calls. a call that reads or writes the
 * card must run on a drive's fiber. the format, the deletes, the defrag and
 * the saves lock the card's bank while they write. the rewrites unlock it.
 * each restores the bank's lock as it was */

/* the function the file system prints its messages with, or null */
extern void (*_vmsfs_dbg_out)(const char *message);

/* start the file system on the drives whose bits enabled_drives sets.
 * capacity is the blocks each drive's caches take, for its system block, FAT
 * and directory. it must be a capacity class's count: 15, 33, 65 or 122 for
 * class 0 to 3. workspace holds VMS_GUARD_SIZE bytes of guard and capacity
 * blocks for each enabled drive in turn. a null workspace enables no drive:
 * each takes its own from vmsfs_setwork.
 *
 * it returns 0 whatever capacity is, and a capacity of no class enables no
 * drive. it sets the retried calls' count to 10 */
Sint32 vmsfs_init(Sint32 capacity, Sint32 enabled_drives, void *workspace);
/* give drive size bytes of workspace for the card vmsfs_connect found, and
 * mark the drive mounted. it returns 0, or BUD_ERR_INVALID_PARAM where
 * vmsfs_init took a workspace, workspace is null, or size is too small */
Sint32 vmsfs_setwork(Sint32 drive, void *workspace, Sint32 size);
/* clear every drive's record. returns 0 */
Sint32 vmsfs_exit(void);
/* the bits of the drives that hold cards */
Sint32 vmsfs_isready(void);
/* 1 where drive holds a mounted card that hasn't changed, and 0 where not */
Sint32 vmsfs_ismount(Sint32 drive);

/* read the card's layout and then its caches, and mark the drive mounted.
 * where each drive takes its own workspace, the call reads only the caches.
 * vmsfs_connect must have read the layout, and vmsfs_setwork must have
 * marked the drive mounted.
 *
 * it returns 0, BUD_ERR_NO_DISK, BUD_ERR_UNKNOWN_DISK for a layout no
 * capacity class has, or a failed read's result. once the system block is
 * read, the call returns 0 where vmsfs_isformat fails, and reads no more. it
 * returns 0 too where a FAT or directory read gives BUD_ERR_GENERIC, which
 * marks the card unformatted */
Sint32 vmsfs_mount(Sint32 drive);
/* read the card's layout and its first block, for a drive that takes its
 * own workspace. it returns the blocks the card's caches take, counted as
 * vmsfs_init's capacity is. vmsfs_setwork's workspace must hold that many
 * blocks and VMS_GUARD_SIZE bytes of guard. it returns BUD_ERR_NO_DISK or
 * BUD_ERR_UNKNOWN_DISK where it fails */
Sint32 vmsfs_connect(Sint32 drive);
/* forget drive's card and its caches. returns 0, or BUD_ERR_NO_DISK for a
 * drive that isn't enabled */
Sint32 vmsfs_unmount(Sint32 drive);

/* each call below checks its drive first, unless its comment says
 * otherwise. the checks return BUD_ERR_NO_DISK where the drive isn't
 * enabled, or its card has left, changed or isn't mounted. a drive whose
 * card has left is unmounted. the checks return BUD_ERR_UNFORMAT for a card
 * that isn't formatted */

/* 0 for a formatted card, and BUD_ERR_UNFORMAT for any other. a formatted
 * card's system block opens with 16 bytes of 0x55. a card whose FAT or
 * directory read gave BUD_ERR_GENERIC when its caches last loaded counts as
 * unformatted. a card without the 16 bytes has its FAT and directory marked
 * uncached */
Sint32 vmsfs_isformat(Sint32 drive);
/* format the card, and leave its FAT and directory empty. the new system
 * block takes the VMS_LABEL_SIZE bytes at label as its volume label. it
 * takes icon and timestamp too. where erase_blocks isn't 0, the format
 * writes zeros to each block below the system block. an erased block whose
 * write gives BUD_ERR_WRITE_ERROR is marked bad.
 *
 * it returns BUD_ERR_ILLEGAL_DISK where the system block's first write gives
 * BUD_ERR_WRITE_ERROR. it returns BUD_ERR_UNKNOWN_DISK for a layout no
 * capacity class has, and can also return BUD_ERR_NO_DISK. otherwise it
 * returns what reading the caches back gives. the writes of the new
 * directory, FAT and system block don't change that result.
 *
 * the format skips two of the checks above. it takes an unformatted card,
 * and it doesn't unmount a drive whose card has left */
Sint32 vmsfs_format(Sint32 drive, const char *label, Sint32 icon,
                    const BUS_TIME *timestamp, Sint32 erase_blocks);

/* fill output with the card's volume label, time, block counts and icon */
Sint32 vmsfs_getinfo(Sint32 drive, BUS_DISKINFO *output);
/* the free blocks a file of kind can take. for BUD_FILETYPE_EXECUTABLE, it
 * counts the free blocks from block 0 up, to at most exec_blocks. it stops at
 * the first block that isn't free. for any other kind, it counts the user
 * area's free blocks */
Sint32 vmsfs_diskfree(Sint32 drive, Sint32 kind);

/* each call below that names a file returns BUD_ERR_FILE_NOT_FOUND where the
 * card has no file of the name. a call's comment says where it doesn't. a
 * name is up to VMS_NAME_SIZE bytes. a name a call stores takes one more, for
 * its null */
/* fill output with the file's details */
Sint32 vmsfs_fgetinfo(Sint32 drive, const char *name, BUS_FILEINFO *output);
/* the file's blocks */
Sint32 vmsfs_fgetsize(Sint32 drive, const char *name);
/* 0 where the file is on the card */
Sint32 vmsfs_fisexist(Sint32 drive, const char *name);
/* vmsfs_findfirst stores the name of the card's first file. each call of
 * vmsfs_findnext stores the name of the file after the last one found.
 * the _ex forms also fill output as vmsfs_fgetinfo does. each returns 0,
 * or BUD_ERR_FILE_NOT_FOUND past the last file */
Sint32 vmsfs_findfirst(Sint32 drive, char *name);
Sint32 vmsfs_findnext(Sint32 drive, char *name);
Sint32 vmsfs_findfirst_ex(Sint32 drive, char *name, BUS_FILEINFO *output);
Sint32 vmsfs_findnext_ex(Sint32 drive, char *name, BUS_FILEINFO *output);
/* store the name of the card's executable file. it returns
 * BUD_ERR_FILE_NOT_FOUND where the card has none */
Sint32 vmsfs_xfind(Sint32 drive, char *name);
/* set the file's copy flag, and write the directory. it ignores
 * header_offset, and returns 0 or the write's failure */
Sint32 vmsfs_fsetattr(Sint32 drive, const char *name, Uint16 header_offset,
                      Uint8 copy_flag);
/* rename the file, and write the directory. it returns 0 or the write's
 * failure, or BUD_ERR_FILE_EXIST where new_name is taken */
Sint32 vmsfs_frename(Sint32 drive, const char *old_name, const char *new_name);
/* delete the file, and write the directory and FAT. vmsfs_fdelete_ex deletes
 * each of the count files that names lists. each returns 0, or a failure to
 * write the directory or FAT. a file whose chain is broken loses its entry,
 * but its blocks stay in use.
 *
 * vmsfs_fdelete_ex goes on past a name the card has no file of. once the
 * directory and FAT are written, it then returns BUD_ERR_CANNOT_DELETE */
Sint32 vmsfs_fdelete(Sint32 drive, const char *name);
Sint32 vmsfs_fdelete_ex(Sint32 drive, const char **names, Sint32 count);
/* pack the user area's used blocks at its top. an executable file can then
 * take the blocks from 0 up. each move takes the lowest used block to the
 * highest free block above it. buffer must hold a block.
 *
 * it returns 0, or the first failed read's or write's result. it returns
 * BUD_ERR_ACCESS_DENIED where the card has no executable area or holds an
 * executable file. a FAT entry that links outside its area gives it too */
Sint32 vmsfs_xdefrag(Sint32 drive, void *buffer);

/* open the file to read, store its record in *output, and return 0. where it
 * fails, it stores null.
 *
 * it returns BUD_ERR_CANNOT_OPEN where every file record is open. it returns
 * BUD_ERR_FILE_BROKEN where the file's chain is broken, or where the card has
 * no such file */
Sint32 vmsfs_fopen(Sint32 drive, const char *name, BU_FILE **output);
/* open a new data file to write, store its record in *opened, and return 0.
 * where it fails, it stores null. a file of the name is deleted first, in the
 * caches alone, even where the create then fails. the card loses that file at
 * the next write of the directory and FAT, such as the close's. the new file
 * takes its directory entry only as it closes. a second create before then
 * can pick the same entry.
 *
 * it returns BUD_ERR_CANNOT_CREATE where the directory has no empty entry, and
 * BUD_ERR_CANNOT_OPEN where every file record is open */
Sint32 vmsfs_fcreate(Sint32 drive, const char *name, const BUS_TIME *timestamp,
                     BU_FILE **opened);
/* vmsfs_fcreate for the executable file, which grows from block 0 up. it
 * returns BUD_ERR_FILE_EXIST where the name is taken, and
 * BUD_ERR_EXECFILE_EXIST where an executable file is on the card. it returns
 * BUD_ERR_CANNOT_CREATE where the card has no executable area, or block 0
 * isn't free */
Sint32 vmsfs_xcreate(Sint32 drive, const char *name, const BUS_TIME *timestamp,
                     BU_FILE **opened);
/* set the header offset and copy flag that the file's close records. it
 * returns 0, or BUD_ERR_INVALID_PARAM for a null file */
Sint32 vmsfs_fsetparam(BU_FILE *file, Uint16 header_offset, Uint8 copy_flag);
/* close the file and free its record. closing a file being written stores
 * its directory entry, and writes the directory and FAT. a report_progress
 * that isn't 0 starts the drive's progress first.
 *
 * it returns 0 or the directory's write failure, whatever the FAT's write
 * gives. it returns BUD_ERR_INVALID_PARAM for a null file. a drive that fails
 * its checks still frees the record, and returns 0 */
Sint32 vmsfs_fclose(BU_FILE *file, Sint32 report_progress);
/* read or write count blocks from or to buffer. each returns 0 or the first
 * failure, or BUD_ERR_INVALID_PARAM for a null file.
 * vmsfs_fwrite returns BUD_ERR_ACCESS_DENIED for a file opened to read */
Sint32 vmsfs_fread(BU_FILE *file, void *buffer, Sint32 count);
Sint32 vmsfs_fwrite(BU_FILE *file, const void *buffer, Sint32 count);

/* read every block of the card into buffer, or write buffer to every
 * block, in order. each returns 0 or the first failure. neither checks the
 * drive first */
Sint32 vmsfs_read_disk_image(Sint32 drive, void *buffer);
Sint32 vmsfs_write_disk_image(Sint32 drive, const void *buffer);
/* read block_count blocks of the file into buffer. a block_count of 0
 * reads the whole file, or the rest of it from first_block. the _ex form
 * starts at block first_block. each returns 0 or the first failure, of
 * vmsfs_fgetsize, vmsfs_fopen or a read */
Sint32 vmsfs_load_file(Sint32 drive, const char *name, void *buffer,
                       Uint32 block_count);
Sint32 vmsfs_load_file_ex(Sint32 drive, const char *name, void *buffer,
                          Uint32 first_block, Uint32 block_count);
/* save block_count blocks from buffer as the file. vmsfs_save_file replaces
 * any file of the name. vmsfs_save_exec_file saves the executable file, and
 * replaces no file. flags' low byte is the copy flag, and BUD_FLAG_VERIFY
 * reads each block back to check it.
 *
 * each returns 0 or the first failure, and BUD_ERR_VERIFY for a block that
 * reads back changed. the close's result is dropped, so a save whose
 * directory or FAT write fails still returns 0. each returns
 * BUD_ERR_DISK_FULL where the card lacks the free blocks. vmsfs_save_file
 * counts as free the blocks of the file it replaces. vmsfs_save_exec_file
 * counts the free blocks from block 0 up, as vmsfs_diskfree does. an executable
 * file already on the card leaves none */
Sint32 vmsfs_save_file(Sint32 drive, const char *name, const void *buffer,
                       Uint32 block_count, BUS_TIME *timestamp, Sint32 flags);
Sint32 vmsfs_save_exec_file(Sint32 drive, const char *name, const void *buffer,
                            Uint32 block_count, BUS_TIME *timestamp,
                            Sint32 flags);
/* write block_count blocks from buffer over the file, starting at the
 * file's block first_block, counted from 0. vmsfs_rewrite_exec_file takes
 * the executable file, and vmsfs_rewrite_file a data file. a block_count of
 * 0 makes vmsfs_rewrite_file write to the file's end.
 *
 * a file of the other kind gives BUD_ERR_ACCESS_DENIED. blocks past the
 * file's end give BUD_ERR_INVALID_PARAM, and a broken chain
 * BUD_ERR_FILE_BROKEN. vmsfs_rewrite_exec_file otherwise returns 0 or the
 * first failed write's result. vmsfs_rewrite_file returns 0, even where a
 * write fails */
Sint32 vmsfs_rewrite_exec_file(Sint32 drive, const char *name,
                               const void *buffer, Uint32 first_block,
                               Uint32 block_count);
Sint32 vmsfs_rewrite_file(Sint32 drive, const char *name, const void *buffer,
                          Uint32 first_block, Uint32 block_count);

/* the card's banks. each checks only for an enabled drive with a mounted
 * card that hasn't changed. without one, each returns BUD_ERR_NO_DISK.
 * vmsfs_getbankinfo leaves *count as it was. vmsfs_changebank mounts the
 * card again in its new bank */
Sint32 vmsfs_getbankinfo(Sint32 drive, Sint32 *current, Sint32 *locked,
                         Sint32 *count);
Sint32 vmsfs_changebank(Sint32 drive, Sint32 bank);
Sint32 vmsfs_lockbank(Sint32 drive, Sint32 locked);

/* a drive's command, and what it calls back */
typedef struct {
    Sint32 unk_00;
    /* 1 from vmsfs_c_setparam till the command ends, and 0 while none is
     * queued. the drive's fiber runs inside buExecServer, Kaede's handler,
     * and clears it there. it is volatile since the handler can interrupt a
     * caller that polls buStat */
    volatile Sint32 state;
    Sint32 operation; /* a BUD_OP_ value: the command to run, or none */
    Sint32 unk_0c;
    Sint32 drive; /* the drive the running command works on */
    Sint32 argument[6]; /* vmsfs_c_setparam's arguments */
    /* a file's name and a rename's new name: up to VMS_NAME_SIZE bytes, and
     * then nulls */
    char name[VMS_NAME_BUFFER];
    char new_name[VMS_NAME_BUFFER];
    char label[VMS_LABEL_SIZE]; /* a format's volume label */
    BUS_TIME time; /* a format's or a save's time */
    BU_COMPLETECALLBACK completed;
    BU_PROGRESSCALLBACK progress;
    Uint32 callback_argument;
} VMS_COMMAND;

/* the result of each drive's last command */
extern Sint32 buLastError[VMS_DRIVES];
extern VMS_COMMAND buVmsCmd[VMS_DRIVES];

/* clear each drive's command and last result, and start a fiber for each drive
 * whose bit drives sets. Returns 0 on success or -1 on setup failure. */
Sint32 vmsfs_c_init(Sint32 drives);
/* clear each drive's command. returns 0 */
Sint32 vmsfs_c_exit(void);
/* run each drive's fiber till it sleeps */
void vmsfs_c_synch(void);
/* call drive's completed callback with operation and result */
void vmsfs_c_execcallback(Sint32 drive, Sint32 operation, Sint32 result);
/* call drive's progress callback with done of total steps of operation */
void vmsfs_c_progress(Sint32 drive, Sint32 operation, Sint32 done,
                      Sint32 total);
/* vmsfs_c_setcompletecallback sets every drive's completed callback, and
 * vmsfs_c_setprogresscallback every drive's progress callback.
 * vmsfs_c_setcallbackparam sets the argument every completed callback takes
 * last */
void vmsfs_c_setcompletecallback(BU_COMPLETECALLBACK callback);
void vmsfs_c_setprogresscallback(BU_PROGRESSCALLBACK callback);
void vmsfs_c_setcallbackparam(Uint32 argument);
/* 0 where drive has no command queued, and 1 where it has one */
Sint32 vmsfs_c_getstate(Sint32 drive);
/* the setters of drive's command. vmsfs_c_setparam sets its arguments, and
 * marks it queued. vmsfs_c_setvolume, vmsfs_c_setfname and vmsfs_c_settime
 * set its volume label, file names and time. vmsfs_c_setfunc sets its
 * operation. the drive's fiber runs the command once its operation is set,
 * so a caller sets the operation last.
 *
 * vmsfs_c_setfname keeps at most VMS_NAME_SIZE bytes of each name, and a
 * null after them */
void vmsfs_c_setfunc(Sint32 drive, Sint32 operation);
void vmsfs_c_setparam(Sint32 drive, Sint32 param0, Sint32 param1, Sint32 param2,
                      Sint32 param3, Sint32 param4, Sint32 param5);
void vmsfs_c_setvolume(Sint32 drive, const Uint8 *label);
void vmsfs_c_setfname(Sint32 drive, const char *name, const char *new_name);
void vmsfs_c_settime(Sint32 drive, const BUS_TIME *time);

/* the fiber library. a server runs its fibers in turn, each till it sleeps
 * or ends */
typedef struct bu_fbr_server BU_FBR_SERVER;
/* Initialize the fiber library while all servers are idle. */
void buFbrInit(void);
/* Reserve one of the two servers for 1 to 10 fibers. Returns NULL when
 * both servers are in use. The GCC port also rejects an invalid count. */
BU_FBR_SERVER *buFbrCreateServer(Sint32 fibers);
/* Start entry on the last word of a caller-owned, word-aligned stack.
 * size counts bytes and must cover whole words, with enough space below
 * the initial stack pointer for entry and its calls. The stack stays
 * live until the slot is freed. An entry yields or calls buFbrEndFiber.
 * Returns the slot index or -1 on failure. The original SDK relies on
 * the caller to enforce the server's capacity. */
Sint32 buFbrCreateFiber(BU_FBR_SERVER *server, void (*entry)(void),
                        Uint32 *stack, Uint32 size);
/* Free the fibers and their server after its scheduling call returns. */
void buFbrDestroyServer(BU_FBR_SERVER *server);
/* end the running fiber. it doesn't return */
void buFbrEndFiber(void);
/* sleep the running fiber till its server's next run */
void buFbrSleepVFiber(void);
/* run the server once. each due fiber runs till it sleeps or ends, and the
 * run then advances the server's tick. the scheduler runs with FPSCR_DN alone.
 * dispatch and yield restore the destination context's saved FPSCR. the
 * first dispatch inherits the mode in which the fiber was created. The
 * SH-4 jump routines save the active floating-point bank, FPUL and FPSCR.
 * Fibers yield in the game's single-precision mode and register bank.
 * The server restores its caller's FPSCR on return. A fiber may run the
 * other server while its own scheduling call is suspended. */
void buFbrExecFiberServer(BU_FBR_SERVER *server);

/* clear every server, and create the drives' server, with room for
 * VMS_DRIVES fibers and none in it. it returns the server */
BU_FBR_SERVER *vmsfb_init(void);
/* give the drives' server a fiber, as buFbrCreateFiber does */
Sint32 vmsfb_create(void (*entry)(void), void *stack, Sint32 size);
/* run the drives' server once, as buFbrExecFiberServer does */
void vmsfb_exec(void);
#ifdef NONMATCHING
/* Release the drive server after setup fails. */
void vmsfb_exit(void);
#endif
/* sleep the running fiber till the server's next run */
void vmsfb_sleep(void);

/* -1 where the blocks at first and second differ, and 0 where they don't.
 * each is VMS_BLOCK_SIZE bytes */
Sint32 vmsfs_memcmp(const Uint8 *first, const Uint8 *second);
/* copy bytes / 4 long words from source to destination. source and
 * destination must be multiples of 4. bytes must be 4 or more. the copy tests
 * its count after each word, so 0 to 3 bytes copy 2^32 words */
void vmsfs_memcpyl(void *destination, const void *source, Sint32 bytes);
/* read the 16-bit word at offset in buffer, big- or little-endian.
 * buffer + offset must be even */
Uint16 vmsfs_getword_big(void *buffer, Sint32 offset);
Uint16 vmsfs_getword_little(void *buffer, Sint32 offset);
/* write the 16-bit word at offset in buffer, big- or little-endian.
 * buffer + offset must be even */
void vmsfs_setword_big(void *buffer, Sint32 offset, Uint16 value);
void vmsfs_setword_little(void *buffer, Sint32 offset, Uint16 value);

#endif
