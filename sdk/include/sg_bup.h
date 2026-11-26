/* Shinobi backup library, as far as the sources use it */
#ifndef SG_BUP_H
#define SG_BUP_H
#include <sg_xpt.h>

/* a backup call's results */
#define BUD_ERR_BUSY ((Sint32)0xffffffff)
#define BUD_ERR_INVALID_PARAM ((Sint32)0xffffff00)
#define BUD_ERR_NO_DISK ((Sint32)0xffffff01)
#define BUD_ERR_UNFORMAT ((Sint32)0xffffff03)
#define BUD_ERR_DISK_FULL ((Sint32)0xffffff04)
#define BUD_ERR_FILE_NOT_FOUND ((Sint32)0xffffff05)
#define BUD_ERR_FILE_EXIST ((Sint32)0xffffff06)
#define BUD_ERR_CANNOT_OPEN ((Sint32)0xffffff07)
#define BUD_ERR_CANNOT_CREATE ((Sint32)0xffffff08)
#define BUD_ERR_EXECFILE_EXIST ((Sint32)0xffffff09)
#define BUD_ERR_CANNOT_DELETE ((Sint32)0xffffff0a)
#define BUD_ERR_ACCESS_DENIED ((Sint32)0xffffff0b)
#define BUD_ERR_VERIFY ((Sint32)0xffffff10)
#define BUD_ERR_BUPFILE_CRC ((Sint32)0xffffff20)
#define BUD_ERR_BUPFILE_ILLEGAL ((Sint32)0xffffff21)
#define BUD_ERR_LOCKED ((Sint32)0xffffff22)
#define BUD_ERR_WRITE_ERROR ((Sint32)0xffffff40)
#define BUD_ERR_FILE_BROKEN ((Sint32)0xffffff41)
#define BUD_ERR_ILLEGAL_DISK ((Sint32)0xffffff81)
#define BUD_ERR_UNKNOWN_DISK ((Sint32)0xffffff83)
#define BUD_ERR_GENERIC ((Sint32)0xffff0000)

/* a date and time a card keeps. dayofweek is 0 to 6 */
typedef struct {
    Uint16 year;
    Uint8 month;
    Uint8 day;
    Uint8 hour;
    Uint8 minute;
    Uint8 second;
    Uint8 dayofweek;
} BUS_TIME;

/* a backup file's parts, which buMakeBackupFileImage packs into an image.
 * the image keeps up to 16 bytes of vms_comment, 32 of btr_comment and 16
 * of game_name. a null ends each sooner. then come the icons' 16-color
 * palette and icon_num icons of 512 bytes each. visual_type gives the size
 * of the visual at visual_data. save_size bytes of save_data end the
 * image */
typedef struct {
    char vms_comment[18];
    char btr_comment[34];
    Uint8 game_name[16];
    void *icon_palette;
    void *icon_data;
    Uint16 icon_num;
    Uint16 icon_speed;
    void *visual_data;
    Uint16 visual_type;
    Uint16 reserved;
    void *save_data;
    Uint32 save_size;
} BUS_BACKUPFILEHEADER;
/* a file's visual_type: no visual, or a visual of 8064, 4544 or 2048 bytes */
#define BUD_VISUALTYPE_NONE 0
#define BUD_VISUALTYPE_A 1
#define BUD_VISUALTYPE_B 2
#define BUD_VISUALTYPE_C 3

/* pack header's parts into an image at buffer, and return its blocks.
 * buffer must hold buCalcBackupFileSize's blocks. the image pads the
 * comments with spaces, and game_name with zeros. zeros pad its end to a
 * whole block. the image keeps the CRC of its bytes before that padding,
 * taken with its CRC field at 0.
 *
 * it returns BUD_ERR_INVALID_PARAM for an icon_num outside 1 to 3, or a
 * visual_type above 3. a null icon_palette or icon_data gives it too. so
 * does a null visual_data, unless visual_type is BUD_VISUALTYPE_NONE. a
 * failed call writes nothing.
 *
 * a null save_data is no failure, and the image's save_size is then 0. the
 * call still copies save_size bytes from address 0 over the image's start,
 * unless save_size is 0 */
Sint32 buMakeBackupFileImage(void *buffer, const BUS_BACKUPFILEHEADER *header);
/* the blocks an image takes with icon_num icons, a visual of visual_type
 * and save_size bytes of save data. it returns BUD_ERR_INVALID_PARAM for an
 * icon_num outside 1 to 3, or a visual_type above 3 */
Sint32 buCalcBackupFileSize(Uint32 icon_num, Uint32 visual_type,
                            Uint32 save_size);
/* fill header from the image at buffer, and check the image's CRC. it
 * returns 0, BUD_ERR_BUPFILE_ILLEGAL or BUD_ERR_BUPFILE_CRC.
 *
 * the image is illegal for an icon_num outside 1 to 3, or a visual_type
 * above 3. it is illegal too where its save_size, or its size before the
 * padding, is over 0xe1000 bytes. an illegal image leaves header as it was.
 *
 * header takes copies of the image's comments and game_name. a null follows
 * each comment, but none follows game_name. header takes the image's
 * icon_num, icon_speed, visual_type and save_size too. header's four
 * pointers point into the image. visual_data and save_data are null where
 * the image has no visual or no save data.
 *
 * a failed CRC check still leaves header filled. an image whose CRC is 0
 * passes unchecked. the check clears the image's CRC while it runs, and
 * puts it back */
Sint32 buAnalyzeBackupFileImage(BUS_BACKUPFILEHEADER *header, void *buffer);
#ifdef NONMATCHING
/* Check the physical extent and CRC before publishing any image pointers.
 * A stored zero CRC retains the SDK's unchecked-CRC convention. */
Sint32 buAnalyzeBackupFileImageSized(BUS_BACKUPFILEHEADER *header, void *buffer,
                                     Uint32 bytes);
#endif
/* the CRC-16 of size bytes at buffer. its polynomial is 0x1021, taken most
 * significant bit first. it starts at 0, and the result isn't inverted */
Uint16 buCalcCRC(const void *buffer, Uint32 size);

/* a card's details, as buGetDiskInfo stores them */
typedef struct {
    Uint8 volume[32];
    Uint16 total_blocks;
    Uint16 total_user_blocks;
    Uint16 free_blocks;
    Uint16 free_user_blocks;
    Uint16 total_exe_blocks;
    Uint16 free_exe_blocks;
    Uint16 block_size;
    Uint16 icon_no;
    BUS_TIME time;
} BUS_DISKINFO;
/* a file's details, as buGetFileInfo stores them */
typedef struct {
    Uint32 filesize;
    Uint16 blocks;
    Uint8 type;
    Uint8 copyflag;
    Uint16 headerofs;
    BUS_TIME time;
} BUS_FILEINFO;

/* what a queued command calls once it is done. it passes the drive, the
 * operation, the result and the argument buSetCallbackParam gave. what the
 * callback returns is ignored */
typedef Sint32 (*BU_COMPLETECALLBACK)(Sint32 drive, Sint32 operation,
                                      Sint32 result, Uint32 argument);
/* what a command calls as it goes, with done of total steps */
typedef Sint32 (*BU_PROGRESSCALLBACK)(Sint32 drive, Sint32 operation,
                                      Sint32 done, Sint32 total);

/* the operation buExecServer passes to the completed callback for a card
 * that has left its drive. the result it passes is 0 */
#define BUD_OP_UNMOUNT 2
/* the operation of each command a call queues, which the command passes to
 * the completed and progress callbacks. BUD_OP_NOP is a drive's operation
 * while it has no command queued */
#define BUD_OP_NOP 0
#define BUD_OP_MOUNT 1
#define BUD_OP_FORMATDISK 3
#define BUD_OP_CLOSEFILE 4
#define BUD_OP_READFILE 5
#define BUD_OP_WRITEFILE 6
#define BUD_OP_DELETEFILE 7
#define BUD_OP_RENAMEFILE 8
#define BUD_OP_DEFRAGDISK 9
#define BUD_OP_SETFILEATTR 10
#define BUD_OP_LOADFILE 11
#define BUD_OP_SAVEFILE 12
#define BUD_OP_SAVEEXECFILE 13
#define BUD_OP_LOADFILEEX 14
#define BUD_OP_READIMAGE 15
#define BUD_OP_WRITEIMAGE 16
#define BUD_OP_DELETEFILEEX 17
/* a connect's result, where it succeeds, is the card's capacity, as buInit
 * takes it */
#define BUD_OP_CONNECT 18
#define BUD_OP_REWRITEEXECFILE 20
#define BUD_OP_CHANGEBANK 21
#define BUD_OP_LOCKBANK 22
#define BUD_OP_GETBANKINFO 23
#define BUD_OP_REWRITEFILE 24

/* the drives' numbers: A1 and A2 are the sockets of port A's pad, and so
 * on */
#define BUD_DRIVE_A1 0
#define BUD_DRIVE_A2 1
#define BUD_DRIVE_B1 2
#define BUD_DRIVE_B2 3
#define BUD_DRIVE_C1 4
#define BUD_DRIVE_C2 5
#define BUD_DRIVE_D1 6
#define BUD_DRIVE_D2 7
/* buInit's drives for every drive */
#define BUD_USE_DRIVE_ALL 0xff
/* buInit's capacity for cards of up to 1 MB. its caches take a system
 * block, 8 FAT blocks and 113 directory blocks */
#define BUD_CAPACITY_1MB (1 + 8 + 113)
/* the bytes of workspace that a count of drives takes, where each drive's
 * caches take capacity blocks. each drive takes a 32-byte guard and capacity
 * blocks of 512 bytes */
#define BUM_WORK_SIZE(capacity, drives)                                        \
    ((((capacity) * (drives)) * 512) + (32 * (drives)))

/* start the library on the drives whose bits drives sets, and call
 * initialized unless it is null. capacity is the blocks each drive's caches
 * take. workspace holds them for every drive. with a null workspace, a card
 * that comes in is only connected. buMountDisk then gives its drive a
 * workspace. it returns 0 */
Sint32 buInit(Sint32 capacity, Sint32 drives, void *workspace,
              void (*initialized)(void));
/* end the library, and return 0. while a drive has a command queued, it
 * ends nothing and returns BUD_ERR_BUSY */
Sint32 buExit(void);
/* Stop the backup server and discard pending commands and drive caches. */
Sint32 buExitForce(void);
/* run the drives' commands. mount or connect each card that comes into an
 * idle drive, and report each that leaves */
void buExecServer(void);
void buSetCompleteCallback(BU_COMPLETECALLBACK callback);
void buSetProgressCallback(BU_PROGRESSCALLBACK callback);
void buSetCallbackParam(Uint32 argument);
/* the library's version, 0x00020013 for 2.13 */
Uint32 buGetVersion(void);
/* 1 where drive holds a mounted card that hasn't changed, and 0 where not */
Sint32 buIsReady(Sint32 drive);
/* The race entrance also queries the mounted card's bank count. */
Sint32 is_multi_bank(Sint32 drive);

/* each call below does nothing while its drive has a command queued, and
 * returns BUD_ERR_BUSY. buIsFormat alone makes no such test. buMountDisk
 * gives the workspace before it makes the test.
 *
 * the calls in this group queue a command and return 0. the command passes
 * its result to the completed callback, and buGetLastError returns it */
Sint32 buMount(Sint32 drive);
Sint32 buRemount(Sint32 drive);
Sint32 buFormatDisk(Sint32 drive, const Uint8 *label, Sint32 icon,
                    const BUS_TIME *time, Uint32 flags);
Sint32 buDeleteFile(Sint32 drive, const char *name);
Sint32 buDeleteFileEx(Sint32 drive, const char **names, Sint32 count);
Sint32 buSetFileAttr(Sint32 drive, const char *name, Uint16 header, Uint8 copy);
Sint32 buRenameFile(Sint32 drive, const char *name, const char *new_name);
Sint32 buDefragDisk(Sint32 drive, void *workspace);
Sint32 buLoadFile(Sint32 drive, const char *name, void *buffer, Uint32 blocks);
/* the bit of buSaveFile's and buSaveExecFile's flags that reads each block
 * back to check it. the low byte of flags is the file's copy flag */
#define BUD_FLAG_VERIFY 0x80000000
/* a copy flag as the low byte of those flags */
#define BUD_FLAG_COPY(n) ((n) & 0xff)
Sint32 buSaveFile(Sint32 drive, const char *name, const void *buffer,
                  Uint32 blocks, BUS_TIME *time, Uint32 flags);
Sint32 buSaveExecFile(Sint32 drive, const char *name, const void *buffer,
                      Uint32 blocks, BUS_TIME *time, Uint32 flags);
Sint32 buLoadFileEx(Sint32 drive, const char *name, void *buffer, Uint32 first,
                    Uint32 blocks);
Sint32 buRewriteExecFile(Sint32 drive, const char *name, const void *buffer,
                         Uint32 first, Uint32 blocks);
Sint32 buRewriteFile(Sint32 drive, const char *name, const void *buffer,
                     Uint32 first, Uint32 blocks);
/* the command stores the drive's bank and whether it is locked in *current
 * and *locked */
Sint32 buGetBankInfo(Sint32 drive, Sint32 *current, Sint32 *locked);
Sint32 buChangeBank(Sint32 drive, Sint32 bank);
Sint32 buLockBank(Sint32 drive, Sint32 locked);
/* give drive size bytes of workspace for its file system, then mount it as
 * buMount does. where the file system refuses the workspace, the call
 * returns its failure */
Sint32 buMountDisk(Sint32 drive, void *workspace, Sint32 size);

/* the calls in this group answer at once: 0, what they find, or a
 * failure */
Sint32 buUnmount(Sint32 drive);
Sint32 buUnmountDisk(Sint32 drive);
/* 0 for a formatted card, or a failure */
Sint32 buIsFormat(Sint32 drive);
Sint32 buGetDiskInfo(Sint32 drive, BUS_DISKINFO *info);
/* buGetDiskFree's kind for the blocks free to an ordinary file */
#define BUD_FILETYPE_NORMAL 0
/* buGetDiskFree's kind for the blocks free to the executable file. the two
 * kinds are also BUS_FILEINFO's type */
#define BUD_FILETYPE_EXECUTABLE 1
Sint32 buGetDiskFree(Sint32 drive, Sint32 kind);
Sint32 buFindFirstFile(Sint32 drive, char *name);
Sint32 buFindNextFile(Sint32 drive, char *name);
Sint32 buFindFirstFileEx(Sint32 drive, char *name, BUS_FILEINFO *info);
Sint32 buFindNextFileEx(Sint32 drive, char *name, BUS_FILEINFO *info);
Sint32 buFindExecFile(Sint32 drive, char *name);
Sint32 buIsExistFile(Sint32 drive, const char *name);
Sint32 buGetFileInfo(Sint32 drive, const char *name, BUS_FILEINFO *info);
Sint32 buGetFileSize(Sint32 drive, const char *name);
/* buStat's result for a drive that has a command queued. it returns 0 for
 * one that hasn't */
#define BUD_STAT_BUSY ((Sint32)0xffffffff)
Sint32 buStat(Sint32 drive);
/* the result of drive's last command */
Sint32 buGetLastError(Sint32 drive);

/* give the file system a function to print its messages with */
void buSetPrintFunc(void (*print)(const char *message));
/* set how many times the file system tries a card call again */
void buSetRetryCount(Sint32 count);

#endif
