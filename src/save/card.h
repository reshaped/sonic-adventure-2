/* the memory cards: their drives, the commands that save and load files, and
 * their indicator */
#ifndef SAVE_CARD_H
#define SAVE_CARD_H
#include <sg_bup.h>
#include <sg_xpt.h>

/* the backup library's drives: two sockets on each of the four connectors */
#define CARD_DRIVES 8

/* The game-name field copied to Chao backup files, including the book VMU
 * file. All 16 bytes are zero. */
extern const Uint8 card_game_name[16];

/* of the library's commands, card.c follows saves, loads and deletes. a
 * save is a buSaveFile, buSaveExecFile or buRewriteExecFile. a load is a
 * buLoadFile or buLoadFileEx, and a delete a buDeleteFile */

/* a drive as card_get_status reports it */
struct card_status {
    Sint32 busy; /* buStat's result: BUD_STAT_BUSY, or 0 */
    Sint32 error; /* buGetLastError's result */
    /* count is the steps done, and max the steps in all. both are as the last
     * report of progress of the drive's save, load or delete gave them. a call
     * that starts a command sets both to 0 */
    Sint32 count;
    Sint32 max;
    /* The port sets this only after the card has mounted successfully. */
    Bool mounted;
};

/* start the backup library on every drive, and the clock. the library gets no
 * workspace. each card that comes in is mounted with one of its own, from the
 * Shinobi heap. card_init clears the drives' records. it creates the
 * indicator's task unless one lives. where task_create returns null, no
 * indicator shows until a later card_init.
 *
 * until card_exit, the library's callbacks keep game_state.card_state. each
 * report of a save's, a load's or a delete's progress sets its busy state,
 * whatever its drive. a load's end sets CARD_IDLE, or CARD_LOAD_FAILED for a
 * result other than 0. a buSaveFile's or a buSaveExecFile's end sets CARD_IDLE
 * or CARD_SAVE_FAILED alike. a buRewriteExecFile's or a delete's end sets
 * CARD_IDLE whatever its result.
 *
 * the callbacks also follow the card in the drive save_set_drive gave. where
 * it leaves, a busy or failed card_state becomes its failed state. under
 * CARD_IDLE, game_state.card_removed becomes 1 instead. the end of a mount
 * there sets card_removed to 0 on success (on every result in retail). A failed
 * state stays until a command or other code sets another.
 *
 * the indicator's task draws at the top right of the screen. under CARD_SAVING
 * or CARD_LOADING, it shows NOW SAVING or NOW LOADING once a command reports
 * progress. the text's color fills it from the left as the command goes, over
 * its shadow. under a failed card_state, it shows -FAILED!- */
void card_init(void);
/* end what card_init started. card_exit kills the indicator's task, and
 * unmounts each mounted drive and frees its workspace. it stops the clock,
 * then waits until no drive has a command queued and ends the library.
 * The port drains the commands before releasing their buffers. If no
 * command advances for 600 frames it retires the server and reports failure. */
void card_exit(void);

/* each call below starts a command on drive and returns 0, or returns -1 where
 * it can't. it can't for a null name, or a drive outside 0 to CARD_DRIVES - 1.
 * it can't for a drive without a mounted card that hasn't changed, or with a
 * command queued. nor can it where the library refuses the command. a command
 * that starts sets game_state.card_state to CARD_SAVING or CARD_LOADING by its
 * kind. its end sets card_state as card_init says */

/* start saving header's parts to drive as the backup file name. the call packs
 * them into an image from the Shinobi heap, so they may change once it
 * returns. the command's end frees the image. copy is the file's copy flag.
 * the save verifies each block, and dates the file by the clock. the call also
 * returns -1 for a null header, or for parts that buCalcBackupFileSize or
 * buMakeBackupFileImage refuses. it returns -1 too where the heap can't give
 * the image */
Sint32 card_save_file(Sint32 drive, const char *name,
                      BUS_BACKUPFILEHEADER *header, char copy);
/* start saving the blocks at buffer to drive as the executable file name.
 * copy, the verify and the date are as card_save_file has them. buffer must
 * stay as it is until the command ends. the call also returns -1 for a null
 * buffer */
Sint32 card_save_exec_file(Sint32 drive, const char *name, const void *buffer,
                           Uint32 blocks, char copy);
/* start writing buffer over the executable file name on drive: blocks of its
 * blocks, from its block first. buffer must stay as it is until the command
 * ends */
Sint32 card_rewrite_exec_file(Sint32 drive, const char *name,
                              const void *buffer, Uint32 first, Uint32 blocks);
/* start loading the backup file name on drive, whole, into an image from the
 * Shinobi heap. the command's end copies up to size bytes of the file's save
 * data to dest. it then frees the image. it copies only where the load's
 * result is 0 and the image passes validation. The port also checks the
 * supplied physical extent and any nonzero CRC.
 * dest is otherwise left as it was, and must stay until the command ends. the
 * call also returns -1 where buGetFileSize finds no blocks of the file. it
 * returns -1 too where the heap can't give the image */
Sint32 card_load_file(Sint32 drive, const char *name, void *dest, Sint32 size);
#ifdef NONMATCHING
/* Start a load with a specific SDK error on refusal. Once the drive is idle,
 * card_load_result gives the entire validated payload size or the read error.
 * A successful read copies up to size bytes and clears the unused tail. */
Sint32 card_start_load(Sint32 drive, const char *name, void *dest, Sint32 size);
Sint32 card_load_result(Sint32 drive);
#endif
/* start loading the file name on drive into buffer: blocks of its blocks,
 * from its block first. buffer must stay until the command ends */
Sint32 card_load_file_ex(Sint32 drive, const char *name, void *buffer,
                         Uint32 first, Uint32 blocks);

/* store drive's state in status and return 0. for a drive outside 0 to
 * CARD_DRIVES - 1, it returns -1 and leaves status as it was */
Sint32 card_get_status(Sint32 drive, struct card_status *status);
/* the drive save_set_drive gave */
Sint32 card_save_drive(void);

/* start the backup library on every drive, without a workspace. card_boot
 * then waits on each drive in turn until it has no command queued, and
 * calls buExit unchecked */
void card_boot(void);
/* whether a card can take the game's save. it returns CARD_CHECK_OK for the
 * first card that holds a file whose name opens SONIC2___. a card with at
 * least SAVE_BLOCKS free once its directory reads to its end counts too.
 * without one, it returns CARD_CHECK_NO_ROOM where a card's directory read to
 * its end. it returns CARD_CHECK_NO_CARD where none did.
 *
 * card_check starts the backup library on every drive. it gives the library a
 * workspace from the Shinobi heap, for cards of up to 1 MB. it waits a frame
 * before it reads the directories and a frame after. it then calls buExit
 * unchecked, and frees the workspace */
Sint32 card_check(void);
/* card_check's results */
#define CARD_CHECK_NO_CARD (-1)
#define CARD_CHECK_NO_ROOM 0
#define CARD_CHECK_OK 1

#endif
