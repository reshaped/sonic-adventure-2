/* the memory cards: their drives, the commands that save and load files, and
 * their indicator */
#include <ninja.h>
#include <sg_bup.h>
#include <sg_syrtc.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "draw/text_sprite.h"
#include "save/card.h"
#include "save/save.h"
#include "system/frame.h"
#include "system/game_state.h"
#include "system/task.h"

const Uint8 card_game_name[16] = {0};

/* a card's block, in bytes */
#define BLOCK_SIZE 512

/* the characters of SONIC2___, which opens the name of every file of the
 * game's */
#define PREFIX_LENGTH 9
/* the files card_check asks a drive's directory for, at most */
#define DIRECTORY_FILES 512

/* the indicator's texts end at INDICATOR_RIGHT, with their tops at
 * INDICATOR_TOP, in pixels. each is drawn at TEXT_SCALE of its size, at
 * TEXT_Z, over a shadow at SHADOW_Z. the shadow lies SHADOW_OFFSET below
 * and to the right of its text */
#define INDICATOR_RIGHT 608.0F
#define INDICATOR_TOP 48.0F
#define TEXT_SCALE 0.7F
#define TEXT_Z 1.002F
#define SHADOW_Z 1.001F
#define SHADOW_OFFSET 2.0F
#define INDICATOR_PITCH_X 3 /* the pixels between two characters */
#define TEXT_LAYERS 2 /* the text, then its shadow */
#define POLYGON_VERTICES 4

/* each text's color, and its shadow's */
#define SAVING_COLOR 0xffa0ffd0
#define SAVING_SHADOW 0xff6060a0
#define LOADING_COLOR 0xffe0e080
#define LOADING_SHADOW 0xff806020
#define FAILED_COLOR 0xffffa040
#define FAILED_SHADOW 0xff906030

/* the indicator's texts, as its record numbers them */
enum indicator_text {
    INDICATOR_SAVING, /* NOW SAVING */
    INDICATOR_LOADING, /* NOW LOADING */
    INDICATOR_FAILED, /* -FAILED!- */
    INDICATOR_TEXTS
};

/* a progress is the share of a command's steps done, from 0 to 1. the bar is
 * NOW SAVING or NOW LOADING, drawn from the left over that share of its width.
 * card_indicator_draw takes the progress from the drives that write, by a save
 * or a delete. where none does, it takes it from those that load, if any */
enum indicator_bar { BAR_WRITING, BAR_LOADING, BAR_NONE };
/* the progress card_indicator_draw holds for a kind no drive reports. it lies
 * above 1, so no bar shows it */
#define NO_PROGRESS 2.0F
/* card_complete sizes each card's workspace for its one drive */
#define MOUNT_DRIVES 1

/* a drive, as card_complete and card_progress keep it. mounted, count and
 * max are as struct card_status gives them */
struct card_drive {
    Bool mounted;
    /* the BUD_OP of the drive's save, load or delete, from the command's
     * first report of progress to its end, and otherwise BUD_OP_NOP */
    Sint32 op;
    Sint32 count;
    Sint32 max;
    /* the workspace card_complete gave buMountDisk for the card, or null */
    void *work;
    /* the file image card_save_file or card_load_file made for the drive's
     * command, or null. the command's end frees it */
    void *image;
    /* where a load's end copies the image's save data, and the bytes it
     * copies at most. only card_load_file sets them */
    void *dest;
    Sint32 size;
#ifdef NONMATCHING
    /* TRUE while work waits for a buUnmount that succeeds, which
     * card_complete tries again at the drive's next completion */
    Bool release_work;
    /* the bytes card_start_load gave image, which the load's end checks the
     * file against */
    Uint32 image_bytes;
    /* what card_load_result gives. it is 0 until the drive's first load,
     * BUD_ERR_BUSY from a load's start to its end, then the file's save size
     * or the error that ended the load. card_exit leaves BUD_ERR_NO_DISK */
    volatile Sint32 load_result;
#endif
};

/* a text's width and height in pixels, before its scale */
struct text_size {
    Uint16 width;
    Uint16 height;
};

/* the record of the indicator's task */
struct card_indicator {
    struct text_size saving_size;
    struct text_size loading_size;
    struct text_size failed_size;
    Float progress; /* the progress the bar shows */
    /* each text's sprite, or null where none was made */
    struct text_sprite *texts[INDICATOR_TEXTS];
};

static struct card_drive card_drives[CARD_DRIVES];
/* the clock's date, as card_now last read it */
static SYS_RTC_DATE card_date;

/* TRUE from card_init to card_exit. card_complete mounts a card that comes in
 * only while it is TRUE. the initializers put card_started and indicator_task
 * in the data section, where the image holds them, not in bss */
static Bool card_started = FALSE;
#ifdef NONMATCHING
/* Restart the shutdown grace period whenever a command advances. */
#define CARD_STALL_FRAMES 600
static volatile Uint32 card_activity;
#endif
/* the indicator's task, or null while there is none */
static struct task *indicator_task = NULL;
/* the strip of two triangles each text is drawn as. the draws set its
 * corners */
static NJS_TEXTURE_VTX text_polygon[POLYGON_VERTICES] = {
#include "save/card/text_polygon.inc"
};

/* the task's destructor. task_destroy frees the record after it */
static void card_indicator_destroy(struct task *task)
{
    struct card_indicator *indicator;

    if (indicator_task == task) {
        indicator_task = NULL;
    }
    indicator = task->extra.p;
    if (indicator != NULL) {
        if (indicator->texts[INDICATOR_SAVING] != NULL) {
            text_sprite_destroy(indicator->texts[INDICATOR_SAVING]);
            indicator->texts[INDICATOR_SAVING] = NULL;
        }
        if (indicator->texts[INDICATOR_LOADING] != NULL) {
            text_sprite_destroy(indicator->texts[INDICATOR_LOADING]);
            indicator->texts[INDICATOR_LOADING] = NULL;
        }
        if (indicator->texts[INDICATOR_FAILED] != NULL) {
            text_sprite_destroy(indicator->texts[INDICATOR_FAILED]);
            indicator->texts[INDICATOR_FAILED] = NULL;
        }
    }
}

/* draw the bar over its text's whole shadow. the text is NOW SAVING under
 * CARD_SAVING and NOW LOADING under CARD_LOADING. under another
 * card_state, nothing is drawn */
static void card_indicator_draw_bar(struct task *task)
{
    struct card_indicator *indicator = task->extra.p;
    struct text_sprite *sprite;
    Float x;
    Float y;
    Float lit;
    Float width;
    Float height;
    /* @hack redundant: text_height, which the arms set and height copies,
     * where height set in the arms would do */
    Float text_height;
    int i;

    if (game_state.card_state == CARD_SAVING) {
        sprite = indicator->texts[INDICATOR_SAVING];
        width = sprite->scale_x * indicator->saving_size.width;
        text_height = sprite->scale_y * indicator->saving_size.height;
    } else if (game_state.card_state == CARD_LOADING) {
        sprite = indicator->texts[INDICATOR_LOADING];
        width = sprite->scale_x * indicator->loading_size.width;
        text_height = sprite->scale_y * indicator->loading_size.height;
    } else {
        return;
    }
    lit = width * indicator->progress;
    height = text_height;
    x = INDICATOR_RIGHT - width;
    y = INDICATOR_TOP;
    njSetTexture(&sprite->texlist);
    text_polygon[0].z = text_polygon[1].z = text_polygon[2].z =
        text_polygon[3].z = TEXT_Z;
    text_polygon[0].col = text_polygon[1].col = text_polygon[2].col =
        text_polygon[3].col = sprite->color.color;
    for (i = 0; i < TEXT_LAYERS; i++) {
        text_polygon[0].x = text_polygon[1].x = x;
        text_polygon[2].x = text_polygon[3].x = x + lit;
        text_polygon[0].y = text_polygon[2].y = y;
        text_polygon[1].y = text_polygon[3].y = y + height;
        text_polygon[0].u = text_polygon[1].u = 0.0F;
        text_polygon[2].u = text_polygon[3].u =
            lit / (sprite->texture_width * sprite->scale_x);
        text_polygon[0].v = text_polygon[2].v = 0.0F;
        text_polygon[1].v = text_polygon[3].v =
            height / (sprite->texture_height * sprite->scale_y);
        njDrawTexture(text_polygon, POLYGON_VERTICES, sprite->texture, TRUE);
        x += SHADOW_OFFSET;
        y += SHADOW_OFFSET;
        text_polygon[0].z = text_polygon[1].z = text_polygon[2].z =
            text_polygon[3].z = SHADOW_Z;
        text_polygon[0].col = text_polygon[1].col = text_polygon[2].col =
            text_polygon[3].col = sprite->shadow_color.color;
        lit = width;
    }
}

/* draw -FAILED!- whole over its shadow */
/* @hack spelling: the text read from the record at each use, where a local
 * for it, as card_indicator_draw_bar's sprite, would do */
static void card_indicator_draw_failed(struct task *task)
{
    struct card_indicator *indicator = task->extra.p;
    Float x;
    Float y;
    Float width;
    Float height;
    int i;

    width = indicator->texts[INDICATOR_FAILED]->scale_x *
            indicator->failed_size.width;
    height = indicator->texts[INDICATOR_FAILED]->scale_y *
             indicator->failed_size.height;
    x = INDICATOR_RIGHT - width;
    y = INDICATOR_TOP;
    njSetTexture(&indicator->texts[INDICATOR_FAILED]->texlist);
    text_polygon[0].z = text_polygon[1].z = text_polygon[2].z =
        text_polygon[3].z = TEXT_Z;
    text_polygon[0].col = text_polygon[1].col = text_polygon[2].col =
        text_polygon[3].col = indicator->texts[INDICATOR_FAILED]->color.color;
    text_polygon[0].u = text_polygon[1].u = 0.0F;
    text_polygon[2].u = text_polygon[3].u =
        width / (indicator->texts[INDICATOR_FAILED]->texture_width *
                 indicator->texts[INDICATOR_FAILED]->scale_x);
    text_polygon[0].v = text_polygon[2].v = 0.0F;
    text_polygon[1].v = text_polygon[3].v =
        height / (indicator->texts[INDICATOR_FAILED]->texture_height *
                  indicator->texts[INDICATOR_FAILED]->scale_y);
    for (i = 0; i < TEXT_LAYERS; i++) {
        text_polygon[0].x = text_polygon[1].x = x;
        text_polygon[2].x = text_polygon[3].x = x + width;
        text_polygon[0].y = text_polygon[2].y = y;
        text_polygon[1].y = text_polygon[3].y = y + height;
        njDrawTexture(text_polygon, POLYGON_VERTICES,
                      indicator->texts[INDICATOR_FAILED]->texture, TRUE);
        x += SHADOW_OFFSET;
        y += SHADOW_OFFSET;
        text_polygon[0].z = text_polygon[1].z = text_polygon[2].z =
            text_polygon[3].z = SHADOW_Z;
        text_polygon[0].col = text_polygon[1].col = text_polygon[2].col =
            text_polygon[3].col =
                indicator->texts[INDICATOR_FAILED]->shadow_color.color;
    }
}

/* the task's draw. under a failed card_state it draws -FAILED!-, and
 * otherwise the bar. the bar shows the least progress of the drives that
 * write. without one, it shows the least of the drives that load.
 * card_state picks its text, whichever drive gave the progress */
static void card_indicator_draw(struct task *task)
{
    struct card_drive *card;
    struct card_indicator *indicator;
    /* the least of each kind so far */
    Float write_progress = NO_PROGRESS;
    Float load_progress = NO_PROGRESS;
    Float progress;
    enum indicator_bar bar;
    int i;

    for (i = 0; i < CARD_DRIVES; i++) {
        card = &card_drives[i];
        progress = (Float)card->count / (Float)card->max;
        switch (card->op) {
        case BUD_OP_SAVEFILE:
        case BUD_OP_SAVEEXECFILE:
        case BUD_OP_REWRITEEXECFILE:
        case BUD_OP_DELETEFILE:
            if (progress < write_progress) {
                write_progress = progress;
            }
            break;
        case BUD_OP_LOADFILE:
        case BUD_OP_LOADFILEEX:
            if (progress < load_progress) {
                load_progress = progress;
            }
            break;
        }
    }
    if (game_state.card_state < CARD_IDLE) {
        card_indicator_draw_failed(task);
        return;
    }
    bar = BAR_NONE;
    progress = 0.0F;
    indicator = task->extra.p;
    if (write_progress >= 0.0F && write_progress <= 1.0F) {
        bar = BAR_WRITING;
        progress = write_progress;
    } else if (load_progress >= 0.0F && load_progress <= 1.0F) {
        bar = BAR_LOADING;
        progress = load_progress;
    }
    indicator->progress = progress;
    if (bar == BAR_WRITING || bar == BAR_LOADING) {
        card_indicator_draw_bar(task);
    }
}

/* the task's step. its first call sets the draw, which task_create leaves
 * null, and makes the record and its texts. every later call returns at
 * once. the branch for a record already there never runs: task_create gives
 * the task none. that branch frees two of the three texts */
static void card_indicator_step(struct task *task)
{
    struct card_indicator *indicator;

    if (task->draw != NULL) {
        return;
    }
#ifdef NONMATCHING
    /* this side repairs the two tags in this step. it makes the record before
     * it sets any draw, and publishes the record and the draw at the step's
     * end, once all three texts exist */
    indicator = syCalloc(1, sizeof(*indicator));
    if (!indicator) {
        return;
    }
#else
    indicator = task->extra.p;
    /* @bug memory: the draws write and read through a null record where
     * syCalloc fails. the step sets the draw first, and a null from syCalloc
     * leaves the task with it (TST R14,R14 at 0x8c061854).
     * card_indicator_draw then stores progress through the null (FMOV.S
     * FR4,@(R0,R6) at 0x8c0617f6), and card_indicator_draw_failed reads its
     * text through it (MOV.L @(24,R13),R14 at 0x8c0615fe) */
    task->draw = card_indicator_draw;
    if (indicator != NULL) {
        if (indicator->texts[INDICATOR_SAVING] != NULL) {
            text_sprite_destroy(indicator->texts[INDICATOR_SAVING]);
            indicator->texts[INDICATOR_SAVING] = NULL;
        }
        if (indicator->texts[INDICATOR_FAILED] != NULL) {
            text_sprite_destroy(indicator->texts[INDICATOR_FAILED]);
            indicator->texts[INDICATOR_FAILED] = NULL;
        }
    } else {
        indicator = syCalloc(1, sizeof(struct card_indicator));
    }
    if (indicator == NULL) {
        return;
    }
    task->extra.p = indicator;
#endif
    /* @bug memory: the draws read through a text that text_sprite_create_styled
     * couldn't make. the step tests each text for null (TST R3,R3 at
     * 0x8c061878, 0x8c0618be and 0x8c061906), and leaves a null one in the
     * record. card_indicator_draw_bar reads its text's scale untested (FMOV.S
     * @(R0,R14),FR2 at 0x8c06145a and 0x8c061482), and so does
     * card_indicator_draw_failed (FMOV.S @(R0,R14),FR12 at 0x8c061606) */
    indicator->texts[INDICATOR_SAVING] = text_sprite_create_styled(
        "NOW SAVING", game_state.text_language, TEXT_WHITE, INDICATOR_PITCH_X,
        TEXT_PITCH_DEFAULT);
    if (indicator->texts[INDICATOR_SAVING] != NULL) {
        text_sprite_measure(indicator->texts[INDICATOR_SAVING], "NOW SAVING",
                            game_state.text_language,
                            &indicator->saving_size.width,
                            &indicator->saving_size.height);
        indicator->texts[INDICATOR_SAVING]->scale_x = TEXT_SCALE;
        indicator->texts[INDICATOR_SAVING]->scale_y = TEXT_SCALE;
        indicator->texts[INDICATOR_SAVING]->color.color = SAVING_COLOR;
        indicator->texts[INDICATOR_SAVING]->shadow_color.color = SAVING_SHADOW;
    }
    indicator->texts[INDICATOR_LOADING] = text_sprite_create_styled(
        "NOW LOADING", game_state.text_language, TEXT_WHITE, INDICATOR_PITCH_X,
        TEXT_PITCH_DEFAULT);
    if (indicator->texts[INDICATOR_LOADING] != NULL) {
        text_sprite_measure(indicator->texts[INDICATOR_LOADING], "NOW LOADING",
                            game_state.text_language,
                            &indicator->loading_size.width,
                            &indicator->loading_size.height);
        indicator->texts[INDICATOR_LOADING]->scale_x = TEXT_SCALE;
        indicator->texts[INDICATOR_LOADING]->scale_y = TEXT_SCALE;
        indicator->texts[INDICATOR_LOADING]->color.color = LOADING_COLOR;
        indicator->texts[INDICATOR_LOADING]->shadow_color.color =
            LOADING_SHADOW;
    }
    indicator->texts[INDICATOR_FAILED] = text_sprite_create_styled(
        "-FAILED!-", game_state.text_language, TEXT_WHITE, INDICATOR_PITCH_X,
        TEXT_PITCH_DEFAULT);
    if (indicator->texts[INDICATOR_FAILED] != NULL) {
        text_sprite_measure(indicator->texts[INDICATOR_FAILED], "-FAILED!-",
                            game_state.text_language,
                            &indicator->failed_size.width,
                            &indicator->failed_size.height);
        indicator->texts[INDICATOR_FAILED]->scale_x = TEXT_SCALE;
        indicator->texts[INDICATOR_FAILED]->scale_y = TEXT_SCALE;
        indicator->texts[INDICATOR_FAILED]->color.color = FAILED_COLOR;
        indicator->texts[INDICATOR_FAILED]->shadow_color.color = FAILED_SHADOW;
    }
#ifdef NONMATCHING
    /* this side publishes the record and the draw only with all three texts,
     * and otherwise frees what it made for the next step to try again */
    if (!indicator->texts[INDICATOR_SAVING] ||
        !indicator->texts[INDICATOR_LOADING] ||
        !indicator->texts[INDICATOR_FAILED]) {
        int i;
        for (i = 0; i < INDICATOR_TEXTS; i++) {
            if (indicator->texts[i]) {
                text_sprite_destroy(indicator->texts[i]);
            }
        }
        syFree(indicator);
        return;
    }
    task->extra.p = indicator;
    task->draw = card_indicator_draw;
#endif
}

/* read the clock into card_date, and return it as the time a file takes.
 * SYS_RTC_DATE opens with BUS_TIME's fields */
static BUS_TIME *card_now(void)
{
    syRtcGetDate(&card_date);
    return (BUS_TIME *)&card_date;
}

/* the backup library's progress callback */
static Sint32 card_progress(Sint32 drive, Sint32 op, Sint32 count, Sint32 max)
{
#ifdef NONMATCHING
    /* this side ignores a report for a drive outside card_drives, and counts
     * each other as the activity card_exit's wait watches */
    if (drive < 0 || drive >= CARD_DRIVES) {
        return 0;
    }
    card_activity++;
#endif
    switch (op) {
    case BUD_OP_LOADFILE:
    case BUD_OP_LOADFILEEX:
        if (drive >= 0 && drive < CARD_DRIVES) {
            card_drives[drive].count = count;
            card_drives[drive].max = max;
            card_drives[drive].op = op;
        }
        game_state.card_state = CARD_LOADING;
        break;
    case BUD_OP_DELETEFILE:
        if (drive >= 0 && drive < CARD_DRIVES) {
            card_drives[drive].count = count;
            card_drives[drive].max = max;
            card_drives[drive].op = op;
        }
        game_state.card_state = CARD_DELETING;
        break;
    case BUD_OP_SAVEFILE:
    case BUD_OP_SAVEEXECFILE:
    case BUD_OP_REWRITEEXECFILE:
        if (drive >= 0 && drive < CARD_DRIVES) {
            card_drives[drive].count = count;
            card_drives[drive].max = max;
            card_drives[drive].op = op;
        }
        game_state.card_state = CARD_SAVING;
        break;
    }
    return 0;
}

/* @hack spelling: card_state_read, a function SHC writes into card_complete
 * under #pragma inline, with a jump to the label that follows it, where
 * game_state.card_state read in card_complete would do */
#pragma inline(card_state_read)
static int card_state_read(void)
{
    int state;

    state = game_state.card_state;
    goto done;
done:
    return state;
}

#ifdef NONMATCHING
/* free card's workspace once buUnmount succeeds, and clear release_work.
 * buUnmount refuses while the drive has a command queued, and release_work then
 * stays TRUE for the drive's next completion */
static void card_release_work(Sint32 drive, struct card_drive *card)
{
    if (!card->work) {
        card->release_work = FALSE;
    } else if (buUnmount(drive) == 0) {
        syFree(card->work);
        card->work = NULL;
        card->release_work = FALSE;
    }
}
#endif

/* the backup library's completed callback. drive must be below CARD_DRIVES,
 * unchecked. a connect's status is the blocks the card's caches take. while
 * card_started is TRUE, the card is mounted with a workspace for those blocks
 * from the Shinobi heap. nothing tests status first, so a failed connect's
 * error makes a size the heap can't give. the result is 1 for a card connected
 * but not mounted, and 0 otherwise. the library ignores it */
static Sint32 card_complete(Sint32 drive, Sint32 op, Sint32 status,
                            Uint32 argument)
{
    Sint32 result = 0;
#ifdef NONMATCHING
    /* card is set below, once drive is checked */
    struct card_drive *card;
#else
    struct card_drive *card = &card_drives[drive];
#endif
    BUS_BACKUPFILEHEADER header;
    void *work;
    Sint32 size;
    int state;
    /* @hack type: bytes as a Uint32 that the test compares as a Sint32,
     * where bytes as a Sint32 would do */
    Uint32 bytes;

#ifdef NONMATCHING
    /* this side ignores a drive outside card_drives. while the drive's
     * release_work is set, it first tries the release again, and drops a
     * mount's completion. it counts each other completion as the activity
     * card_exit's wait watches */
    if (drive < 0 || drive >= CARD_DRIVES) {
        return 0;
    }
    card = &card_drives[drive];
    if (card->release_work) {
        card_release_work(drive, card);
        if (op == BUD_OP_MOUNT) {
            return 0;
        }
    }
    card_activity++;
#endif
    card->op = BUD_OP_NOP;
    switch (op) {
    case BUD_OP_CONNECT:
        result = 1;
#ifdef NONMATCHING
        /* this side mounts no card that holds a workspace already, nor one
         * whose status, which the matching side sizes a workspace by, lies
         * outside 1 to BUD_CAPACITY_1MB */
        if (card->work || status <= 0 || status > BUD_CAPACITY_1MB) {
            break;
        }
#endif
        if (card_started) {
            size = BUM_WORK_SIZE(status, MOUNT_DRIVES);
            work = syMalloc(size);
            if (work != NULL) {
                if (buMountDisk(drive, work, size) == 0) {
                    card->work = work;
                    result = 0;
                } else {
                    syFree(work);
                }
            }
        }
        break;
    case BUD_OP_MOUNT:
        /* @bug logic: marks the drive mounted, and clears card_removed,
         * for a mount that failed too: nothing tests status ahead of
         * MOV.L R8,@R14 at 0x8c061b0c */
#ifdef NONMATCHING
        /* this side repairs the tag above: a failed mount leaves the drive
         * unmounted, and frees its workspace */
        if (status != 0) {
            card->mounted = FALSE;
            card->release_work = TRUE;
            card_release_work(drive, card);
            break;
        }
#endif
        card->mounted = TRUE;
        if (drive == save_place.drive) {
            game_state.card_removed = 0;
        }
        break;
    case BUD_OP_UNMOUNT:
        card->mounted = FALSE;
#ifdef NONMATCHING
        /* this side frees the workspace by card_release_work, only once
         * buUnmount succeeds, as a queued command can still work in its
         * caches */
        card->release_work = TRUE;
        card_release_work(drive, card);
#else
        if (card->work != NULL) {
            syFree(card->work);
            card->work = NULL;
        }
#endif
        if (drive == save_place.drive) {
            state = card_state_read();
            if (state == CARD_LOADING || state == CARD_LOAD_FAILED) {
                game_state.card_state = CARD_LOAD_FAILED;
            } else if (state == CARD_SAVING || state == CARD_SAVE_FAILED) {
                game_state.card_state = CARD_SAVE_FAILED;
            } else if (state == CARD_DELETING || state == CARD_DELETE_FAILED) {
                game_state.card_state = CARD_DELETE_FAILED;
            } else {
                game_state.card_removed = 1;
            }
        }
        break;
    case BUD_OP_LOADFILE:
#ifdef NONMATCHING
        /* this side checks the image against the bytes it read, and records
         * the result for card_load_result. it copies at most size bytes, and
         * clears the rest of dest */
        if (status == 0) {
            status = buAnalyzeBackupFileImageSized(&header, card->image,
                                                   card->image_bytes);
        }
        if (status == 0) {
            bytes = header.save_size;
            if (bytes > (Uint32)card->size) {
                bytes = (Uint32)card->size;
            }
            if (bytes != 0) {
                memcpy(card->dest, header.save_data, bytes);
            }
            memset((Uint8 *)card->dest + bytes, 0, (Uint32)card->size - bytes);
            card->load_result = (Sint32)header.save_size;
        } else {
            card->load_result = status;
        }
        card->image_bytes = 0;
        card->dest = NULL;
        card->size = 0;
#else
        if (status == 0) {
            if (buAnalyzeBackupFileImage(&header, card->image) !=
                BUD_ERR_BUPFILE_ILLEGAL) {
                bytes = header.save_size;
                if ((Sint32)bytes > card->size) {
                    bytes = card->size;
                }
                memcpy(card->dest, header.save_data, bytes);
            }
        }
#endif
        /* fall through */
    case BUD_OP_LOADFILEEX:
        if (status) {
            game_state.card_state = CARD_LOAD_FAILED;
        } else {
            game_state.card_state = CARD_IDLE;
        }
        break;
    case BUD_OP_SAVEFILE:
    case BUD_OP_SAVEEXECFILE:
        if (status) {
            game_state.card_state = CARD_SAVE_FAILED;
        } else {
            game_state.card_state = CARD_IDLE;
        }
        break;
    case BUD_OP_DELETEFILE:
    case BUD_OP_REWRITEEXECFILE:
        game_state.card_state = CARD_IDLE;
        break;
    }
#ifdef NONMATCHING
    /* this side frees the image only at the end of the save or the load it
     * serves, where the matching side frees it at any command's end */
    if ((op == BUD_OP_SAVEFILE || op == BUD_OP_LOADFILE) && card->image) {
#else
    if (card->image != NULL) {
#endif
        syFree(card->image);
        card->image = NULL;
    }
    return result;
}

static void card_init_done(void)
{
    buSetProgressCallback(card_progress);
    buSetCompleteCallback(card_complete);
}

void card_init(void)
{
#ifdef NONMATCHING
    /* a second card_init without card_exit starts nothing again */
    if (card_started) {
        return;
    }
#endif
    memset(card_drives, 0, sizeof(card_drives));
    card_started = TRUE;
#ifdef NONMATCHING
    /* where buInit fails, this side leaves card_started FALSE and starts
     * nothing else */
    if (buInit(0, BUD_USE_DRIVE_ALL, NULL, card_init_done) != 0) {
        card_started = FALSE;
        return;
    }
#else
    buInit(0, BUD_USE_DRIVE_ALL, NULL, card_init_done);
#endif
    syRtcInit();
    if (indicator_task == NULL) {
        indicator_task =
            task_create(0, 0, card_indicator_step, "VmIndicatorExec");
        if (indicator_task != NULL) {
            indicator_task->destructor = card_indicator_destroy;
        }
    }
}

void card_exit(void)
{
    struct card_drive *card;
    int i;

#ifdef NONMATCHING
    /* card_exit's wait below gives up after CARD_STALL_FRAMES frames without
     * activity. a card_exit without card_init does nothing */
    Uint32 last_activity = card_activity;
    int stalled = 0;

    if (!card_started) {
        return;
    }
#endif
    card_started = FALSE;
    if (indicator_task != NULL) {
        task_kill(indicator_task);
#ifdef NONMATCHING
        /* this side forgets the killed task at once, so a card_init before its
         * destructor runs makes a new indicator */
        indicator_task = NULL;
#endif
    }
#ifdef NONMATCHING
    /* this side repairs the tag below: it frees the workspaces only once
     * buExit has run. buExit returns BUD_ERR_BUSY while any drive has a
     * command under way, and the loop asks again each frame. once buExit
     * runs, it removes the server's handler and clears every drive's cache
     * pointers */
    while (buExit() == BUD_ERR_BUSY) {
        frame_wait();
        if (card_activity != last_activity) {
            last_activity = card_activity;
            stalled = 0;
        } else if (++stalled == CARD_STALL_FRAMES) {
            /* Maple owns its command and reply buffers; the backup fibers
             * copy between those buffers and these caches. Stop the server
             * and retire its commands before releasing the game's buffers. */
            buExitForce();
            if (game_state.card_state == CARD_SAVING) {
                game_state.card_state = CARD_SAVE_FAILED;
            } else if (game_state.card_state == CARD_DELETING) {
                game_state.card_state = CARD_DELETE_FAILED;
            } else if (game_state.card_state == CARD_LOADING) {
                game_state.card_state = CARD_LOAD_FAILED;
            }
            break;
        }
    }
    for (i = 0; i < CARD_DRIVES; i++) {
        card = &card_drives[i];
        if (card->image) {
            syFree(card->image);
        }
        if (card->work) {
            syFree(card->work);
        }
        memset(card, 0, sizeof(*card));
        card->load_result = BUD_ERR_NO_DISK;
    }
    syRtcFinish();
#else
    for (i = 0; i < CARD_DRIVES; i++) {
        card = &card_drives[i];
        if (card->mounted) {
            /* @bug memory: frees the workspace of a drive that buUnmount
             * left mounted. buUnmount unmounts nothing while the drive has a
             * command queued, and nothing reads its BUD_ERR_BUSY (JSR @R10
             * at 0x8c061caa). syFree then frees the workspace, which holds
             * the caches the command works in (JSR @R11 at 0x8c061cb4). the
             * wait on buExit below lets the command run on */
            buUnmount(i);
            if (card->work != NULL) {
                syFree(card->work);
                card->work = NULL;
            }
        }
    }
    syRtcFinish();
    while (buExit() != 0) {
    }
#endif
}

Sint32 card_save_file(Sint32 drive, const char *name,
                      BUS_BACKUPFILEHEADER *header, char copy)
{
    struct card_drive *card;
    void *image = NULL;
    Sint32 size;
    Sint32 blocks;
    Sint32 result = -1;
    /* @hack redundant: image_at, size_at and blocks_at, pointers to three
     * locals, which nothing reads */
    void **image_at = &image;
    Sint32 *size_at = &size;
    Sint32 *blocks_at = &blocks;
#ifdef NONMATCHING
    /* the interrupt mask the save starts under, and the date it takes before
     * the mask goes on */
    int mask;
    BUS_TIME *date;
#endif

#ifdef NONMATCHING
    /* this side starts nothing while the backup library isn't running: before
     * card_init, or after card_exit */
    if (!card_started) {
        return -1;
    }
#endif
    if (name == NULL || header == NULL || drive < 0 || drive >= CARD_DRIVES) {
        return -1;
    }
    if (buIsReady(drive) && buStat(drive) == 0) {
        size = buCalcBackupFileSize(header->icon_num, header->visual_type,
                                    header->save_size);
        if (size > 0) {
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((image = syMalloc(size * BLOCK_SIZE)) != NULL) {
                blocks = buMakeBackupFileImage(image, header);
#ifdef NONMATCHING
                /* this side tests the library and the drive again with
                 * interrupts masked, and starts the save under the same mask,
                 * so no interrupt comes between the test and the start */
                date = card_now();
                mask = get_imask();
                set_imask(IMASK_ALL);
                if (card_started && blocks > 0 && buIsReady(drive) &&
                    buSaveFile(drive, name, image, blocks, date,
#else
                if (blocks > 0 &&
                    buSaveFile(drive, name, image, blocks, card_now(),
#endif
                               BUD_FLAG_VERIFY | BUD_FLAG_COPY(copy)) == 0) {
                    card = &card_drives[drive];
                    card->count = card->max = 0;
                    result = 0;
                    card->image = image;
                    image = NULL;
                    game_state.card_state = CARD_SAVING;
                }
#ifdef NONMATCHING
                /* this side restores the interrupt level saved before the
                 * test */
                set_imask(mask);
#endif
            }
        }
    }
    if (image != NULL) {
        syFree(image);
    }
    return result;
}

Sint32 card_save_exec_file(Sint32 drive, const char *name, const void *buffer,
                           Uint32 blocks, char copy)
{
    struct card_drive *card;
    Sint32 result = -1;

#ifdef NONMATCHING
    /* this side starts nothing while the backup library isn't running: before
     * card_init, or after card_exit */
    if (!card_started) {
        return -1;
    }
#endif
    if (name != NULL && buffer != NULL && drive >= 0 && drive < CARD_DRIVES &&
        buIsReady(drive) && buStat(drive) == 0 &&
        buSaveExecFile(drive, name, buffer, blocks, card_now(),
                       BUD_FLAG_VERIFY | BUD_FLAG_COPY(copy)) == 0) {
        card = &card_drives[drive];
        card->count = card->max = 0;
        result = 0;
        card->image = NULL;
        game_state.card_state = CARD_SAVING;
    }
    return result;
}

Sint32 card_rewrite_exec_file(Sint32 drive, const char *name,
                              const void *buffer, Uint32 first, Uint32 blocks)
{
    struct card_drive *card;
    Sint32 result = -1;

#ifdef NONMATCHING
    /* this side starts nothing while the backup library isn't running: before
     * card_init, or after card_exit */
    if (!card_started) {
        return -1;
    }
#endif
    if (name != NULL && drive >= 0 && drive < CARD_DRIVES && buIsReady(drive) &&
        buStat(drive) == 0 &&
        buRewriteExecFile(drive, name, buffer, first, blocks) == 0) {
        card = &card_drives[drive];
        card->count = card->max = 0;
        result = 0;
        card->image = NULL;
        game_state.card_state = CARD_SAVING;
    }
    return result;
}

#ifdef NONMATCHING
Sint32 card_start_load(Sint32 drive, const char *name, void *dest, Sint32 size)
{
    struct card_drive *card;
    Sint32 blocks;
    Sint32 status;
    Uint32 bytes;
    void *image;
    int mask;

    if (!card_started) {
        return BUD_ERR_NO_DISK;
    }
    if (drive < 0 || drive >= CARD_DRIVES || !name || !dest || size < 0) {
        return BUD_ERR_INVALID_PARAM;
    }
    if (!buIsReady(drive)) {
        return BUD_ERR_NO_DISK;
    }
    if (buStat(drive) != 0) {
        return BUD_ERR_BUSY;
    }
    blocks = buGetFileSize(drive, name);
    if (blocks < 0) {
        return blocks;
    }
    /* A directory entry stores its physical block count in 16 bits. */
    if (blocks == 0 || blocks > 0xffff) {
        return BUD_ERR_BUPFILE_ILLEGAL;
    }
    bytes = (Uint32)blocks * BLOCK_SIZE;
    image = syMalloc(bytes);
    if (!image) {
        return BUD_ERR_GENERIC;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    if (!card_started || buStat(drive) || !buIsReady(drive)) {
        set_imask(mask);
        syFree(image);
        return BUD_ERR_BUSY;
    }
    card = &card_drives[drive];
    card->image = image;
    card->dest = dest;
    card->size = size;
    card->image_bytes = bytes;
    card->load_result = BUD_ERR_BUSY;
    card->count = card->max = 0;
    status = buLoadFile(drive, name, image, blocks);
    if (status != 0) {
        card->image = NULL;
        card->dest = NULL;
        card->size = 0;
        card->image_bytes = 0;
        card->load_result = status;
        set_imask(mask);
        syFree(image);
        return status;
    }
    game_state.card_state = CARD_LOADING;
    set_imask(mask);
    return 0;
}

Sint32 card_load_result(Sint32 drive)
{
    if (drive < 0 || drive >= CARD_DRIVES) {
        return BUD_ERR_INVALID_PARAM;
    }
    return card_drives[drive].load_result;
}
#endif

Sint32 card_load_file(Sint32 drive, const char *name, void *dest, Sint32 size)
{
#ifdef NONMATCHING
    /* this side starts the load by card_start_load, which checks the file's
     * size and records the result for card_load_result */
    return card_start_load(drive, name, dest, size) == 0 ? 0 : -1;
#else
    struct card_drive *card;
    Sint32 result = -1;
    void *image = NULL;
    Sint32 blocks;
    /* @hack redundant: image_at and blocks_at, pointers to two locals, as in
     * card_save_file, which nothing reads */
    void **image_at = &image;
    Sint32 *blocks_at = &blocks;

    if (name != NULL && drive >= 0 && drive < CARD_DRIVES && buIsReady(drive) &&
        buStat(drive) == 0) {
        blocks = buGetFileSize(drive, name);
        if (blocks > 0) {
            /* @hack spelling: the assignment inside the test, as in
             * card_save_file, where a statement of its own would do */
            if ((image = syMalloc(blocks * BLOCK_SIZE)) != NULL &&
                buLoadFile(drive, name, image, blocks) == 0) {
                card = &card_drives[drive];
                card->count = card->max = 0;
                result = 0;
                card->image = image;
                card->dest = dest;
                card->size = size;
                image = NULL;
                game_state.card_state = CARD_LOADING;
            }
        }
    }
    if (image != NULL) {
        syFree(image);
    }
    return result;
#endif
}

Sint32 card_load_file_ex(Sint32 drive, const char *name, void *buffer,
                         Uint32 first, Uint32 blocks)
{
    struct card_drive *card;
    Sint32 result = -1;

#ifdef NONMATCHING
    /* this side starts nothing while the backup library isn't running: before
     * card_init, or after card_exit */
    if (!card_started) {
        return -1;
    }
#endif
    if (name != NULL && drive >= 0 && drive < CARD_DRIVES && buIsReady(drive) &&
        buStat(drive) == 0 &&
        buLoadFileEx(drive, name, buffer, first, blocks) == 0) {
        card = &card_drives[drive];
        card->count = card->max = 0;
        result = 0;
        card->image = NULL;
        game_state.card_state = CARD_LOADING;
    }
    return result;
}

Sint32 card_get_status(Sint32 drive, struct card_status *status)
{
    struct card_drive *card;
    Sint32 result = -1;

    if (drive >= 0 && drive < CARD_DRIVES) {
        card = &card_drives[drive];
        status->busy = buStat(drive);
        status->error = buGetLastError(drive);
        result = 0;
        status->count = card->count;
        status->max = card->max;
        status->mounted = card->mounted;
    }
    return result;
}

Sint32 card_save_drive(void)
{
    return save_place.drive;
}

/* the port of each drive's socket */
static const Sint32 drive_ports[CARD_DRIVES] = {
    PDD_PORT_A1, PDD_PORT_A2, PDD_PORT_B1, PDD_PORT_B2,
    PDD_PORT_C1, PDD_PORT_C2, PDD_PORT_D1, PDD_PORT_D2,
};

/* the port of drive's socket, or -1 for a drive outside 0 to CARD_DRIVES - 1.
 * nothing calls it, and no header declares it */
/* @hack redundant: external linkage, which no call needs, where static would
 * do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 card_drive_port(Sint32 drive);
/* NOLINTEND(misc-use-internal-linkage) */

Sint32 card_drive_port(Sint32 drive)
{
    Sint32 port = -1;

    if (drive >= 0 && drive < CARD_DRIVES) {
        port = drive_ports[drive];
    }
    return port;
}

void card_boot(void)
{
    static const Sint32 drives[CARD_DRIVES] = {
        BUD_DRIVE_A1, BUD_DRIVE_A2, BUD_DRIVE_B1, BUD_DRIVE_B2,
        BUD_DRIVE_C1, BUD_DRIVE_C2, BUD_DRIVE_D1, BUD_DRIVE_D2,
    };
    int i;

    buInit(0, BUD_USE_DRIVE_ALL, NULL, NULL);
    for (i = 0; i < CARD_DRIVES; i++) {
        while (buStat(drives[i]) == BUD_STAT_BUSY) {
        }
    }
    buExit();
}

Sint32 card_check(void)
{
    static const Sint32 drives[CARD_DRIVES] = {
        BUD_DRIVE_A1, BUD_DRIVE_A2, BUD_DRIVE_B1, BUD_DRIVE_B2,
        BUD_DRIVE_C1, BUD_DRIVE_C2, BUD_DRIVE_D1, BUD_DRIVE_D2,
    };
    /* the main file's name, less its slot's two digits */
    static char *main_name = "SONIC2___S";
    char name[16];
    /* @hack spelling: prefix, a pointer to main_name that the compare reads
     * through, where main_name written in its place would do */
    char **prefix;
    const Sint32 *next;
    void *work;
    Sint32 drive;
    Sint32 found;
    Sint32 result = CARD_CHECK_NO_CARD;
    int i;

    work = syMalloc(BUM_WORK_SIZE(BUD_CAPACITY_1MB, CARD_DRIVES));
    buInit(BUD_CAPACITY_1MB, BUD_USE_DRIVE_ALL, work, NULL);
    frame_wait();
    next = drives;
    prefix = &main_name;
    while (next < &drives[CARD_DRIVES]) {
        drive = *next++;
        for (i = 0; i < DIRECTORY_FILES; i++) {
            if (i == 0) {
                while ((found = buFindFirstFile(drive, name)) == BUD_ERR_BUSY) {
                }
            } else {
                while ((found = buFindNextFile(drive, name)) == BUD_ERR_BUSY) {
                }
            }
            if (found == 0) {
                if (strncmp(name, *prefix, PREFIX_LENGTH) == 0) {
                    result = CARD_CHECK_OK;
                    goto done;
                }
                /* @hack redundant: the continue */
                continue; /* NOLINT(readability-else-after-return) */
            } else if (found == BUD_ERR_FILE_NOT_FOUND) {
                /* the directory's end. without the room, the loop goes on
                 * asking for a next file, up to DIRECTORY_FILES asks in
                 * all */
                if (buGetDiskFree(drive, BUD_FILETYPE_NORMAL) >= SAVE_BLOCKS) {
                    result = CARD_CHECK_OK;
                    goto done;
                }
                result = CARD_CHECK_NO_ROOM;
            } else {
                break;
            }
        }
    }
done:
    frame_wait();
    buExit();
    syFree(work);
    return result;
}
