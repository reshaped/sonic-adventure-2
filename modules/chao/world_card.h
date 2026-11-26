/* the Chao world's file on the drive card_save_drive gives. each call below
 * sets request->drive to that drive. it starts a task that works through
 * *request, which must outlive the task. a call returns 0 with request->state
 * GARDEN_FILE_BUSY, or -1 with it GARDEN_FILE_ERROR where the task can't
 * start */
#ifndef CHAO_WORLD_CARD_H
#define CHAO_WORLD_CARD_H
#include <sg_xpt.h>

/* state holds a garden_save.h garden_file_state. data is the save data's
 * buffer, and size its bytes. a read copies size bytes into data, however
 * many the file holds */
struct garden_card_request {
    Sint32 drive, state;
    Uint32 unk_08;
    void *data;
    Sint32 size;
};
extern struct garden_card_request garden_card_request;
/* load the file's save data into data. the task ends in GARDEN_FILE_OK, in
 * GARDEN_FILE_INVALID for a file the backup library refuses, or in
 * GARDEN_FILE_ERROR. the call returns -1 where the card isn't ready or holds
 * no world file. it returns -1 too where an allocation or the load can't
 * start.
 *
 * the NONMATCHING build's -1 leaves GARDEN_FILE_ABSENT for a card with no
 * world file. it leaves GARDEN_FILE_INVALID for a file or a size it
 * refuses */
int garden_card_read(struct garden_card_request *request);
/* save size bytes of data as the file's save data. data must open with the
 * garden's struct garden_origin. where garden_file_loaded is set, the task
 * first refuses a card whose file holds another origin, or none. the task
 * ends in GARDEN_FILE_OK or GARDEN_FILE_ERROR.
 *
 * a save the card refuses leaves the task GARDEN_FILE_BUSY. it tries again
 * each frame while the card stays ready and idle. the NONMATCHING build ends
 * it in GARDEN_FILE_ERROR instead.
 *
 * the task sets game_state.card_warning to SAVE_NOT_READY for a card that
 * isn't ready at the start. it sets SAVE_NOT_OURS for a card it refuses */
int garden_card_write(struct garden_card_request *request);
#endif
