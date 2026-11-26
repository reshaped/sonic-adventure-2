/* the file select's step, and the tests of what the drives hold. the step
 * keeps its state in menu_works.work_12, file_select.h's file_select_work */
#include <sg_bup.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "advertise/file_select.h"
#include "advertise/menus.h"
#include "advertise/options.h"
#include "advertise/title_screen.h"
#include "hud/item_stock.h"
#include "save/card.h"
#include "save/save.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "sound/voice_set.h"
#include "system/game_state.h"
#include "system/pad.h"

/* menu_control.step */
enum file_select_step {
    STEP_OPEN, /* open the work, and swing the drives in */
    STEP_DRIVES_IN, /* wait for the drives to stop */
    STEP_DRIVE, /* choose a drive */
    STEP_FILE, /* choose a file, or the new row */
    /* ask whether to make a new file where the Chao world's file has no
     * room */
    STEP_NO_CHAO_ROOM,
    STEP_FILE_DIALOG, /* load the file, go back, or delete it */
    STEP_DELETE, /* ask whether to delete the file */
    STEP_UNK_07, /* nothing sets it */
    STEP_LEFT, /* the file select has left */
    STEP_DELETING, /* wait for the delete, then list the drive again */
    STEP_WAIT_DRIVE /* wait for the main file's drive, then leave */
};

/* the work's messages, numbers of message_tables, with their English
 * texts */
#define MESSAGE_SELECT_CARD 15 /* Select a memory card. */
#define MESSAGE_SELECT_FILE 17 /* Select a file. */
#define MESSAGE_LOAD 19 /* Load a file. */
#define MESSAGE_DELETE 20 /* Do you want to delete this file? */
#define MESSAGE_NO_CHAO_ROOM 21 /* No memory blocks to save Chao data. */

/* the choices of FILE_SELECT_DIALOG_FILE. FILE_SELECT_DIALOG_CONFIRM's are
 * CHOICE_YES and CHOICE_BACK */
#define CHOICE_LOAD 0
#define CHOICE_BACK 1
#define CHOICE_DELETE 2
#define CHOICE_YES 0

/* voice_set_play's sets */
#define FILE_LIST_VOICES 12 /* as the cursor goes to the files */
#define FILE_DIALOG_VOICES 16 /* as a file's dialog opens */
#define NO_CHAO_ROOM_VOICES 27 /* as STEP_NO_CHAO_ROOM's question opens */

/* a port's drives, its two sockets. a drive's low bit is its socket */
#define PORT_DRIVES 2
/* the steps the cursor rests on a drive before the step lists it. a listing
 * that fails waits as long again */
#define READ_STEPS 30
/* the free blocks below which a new file asks first, on a drive without the
 * Chao world's file */
#define CHAO_ROOM_BLOCKS 70
/* save_read's keep for the reads that look at a drive's files. it holds
 * every SAVE_KEEP bit, so a good read changes only save_data and
 * game_state.emblems */
#define READ_KEEP (SAVE_KEEP_STATE | SAVE_KEEP_FLAGS | SAVE_KEEP_BITS)
/* save_data.play_time counts steps */
#define SECONDS_PER_MINUTE 60
#define MINUTES_PER_HOUR 60
#define STEPS_PER_MINUTE (SECONDS_PER_MINUTE * STEPS_PER_SECOND)
#define STEPS_PER_HOUR (MINUTES_PER_HOUR * STEPS_PER_MINUTE)

/* a drive's blocks and files, as file_select_list_files reads them */
struct file_select_list {
    Sint32 total_blocks;
    Sint32 free_blocks;
    Sint32 count;
    struct file_select_file files[FILE_SELECT_SLOTS];
};

struct file_select_work file_select_work;
Sint32 file_select_return_mode;
/* Frames the cursor has rested on the selected drive. */
static Sint32 file_select_timer;
/* Device reported by the last scan of each drive. */
static Sint32 file_select_devices_seen[CARD_DRIVES];

/* by drive, its port as pdGetPeripheralInfo names it */
static const Uint32 drive_ports[CARD_DRIVES] = {
    PDD_PORT_A1, PDD_PORT_A2, PDD_PORT_B1, PDD_PORT_B2,
    PDD_PORT_C1, PDD_PORT_C2, PDD_PORT_D1, PDD_PORT_D2,
};
/* the Chao world's file, as CHAO.PRS names it */
static char chao_file_name[] = "SONIC2___ALF";

static Bool file_select_scan_devices(void);
static Bool file_select_read_drive(void);
static Sint32 file_select_list_files(int drive, struct file_select_list *list);
static void file_select_new_file(void);
static void file_select_leave(void);
static Bool file_select_chao_file_exists(void);

void file_select_start(void)
{
    menu_control.step = STEP_OPEN;
    menu_works.work_12->message = FILE_SELECT_NO_MESSAGE;
}

static void file_select_close(void)
{
    menu_works.work_12->open = FALSE;
    menu_works.work_12->unk_04 = 0;
    game_state.card_state = CARD_IDLE;
}

Bool file_select_step(void)
{
    if (file_select_scan_devices()) {
        /* @hack spelling: a switch with an empty case, where a test of
         * STEP_OPEN would do */
        switch (menu_control.step) {
        case STEP_OPEN:
            break;
        default:
            /* @hack spelling: the work by its own name, where
             * menu_works.work_12 would do */
            file_select_work.drives_shown = FALSE;
            file_select_work.drive_lit = FALSE;
            file_select_work.choosing_file = FALSE;
            file_select_work.dialog_shown = FALSE;
            file_select_work.unk_fc = 0;
            menu_control.step = STEP_OPEN;
            menu_works.work_12->message = FILE_SELECT_NO_MESSAGE;
            break;
        }
    }
    switch (menu_control.step) {
    case STEP_OPEN:
        menu_works.work_12->open = TRUE;
        menu_works.work_12->drives_shown = TRUE;
        menu_works.work_12->drive_lit = FALSE;
        menu_control.step = STEP_DRIVES_IN;
        break;
    case STEP_DRIVES_IN:
        /* @hack spelling: the step in the else of an empty test, where a
         * test of moving == 0 would do */
        if (menu_works.work_12->moving != 0) {
        } else {
            menu_control.step = STEP_DRIVE;
            file_select_timer = 0;
            menu_works.work_12->drive_lit = TRUE;
        }
        break;
    case STEP_DRIVE:
        file_select_timer++;
        if (file_select_timer == READ_STEPS) {
            if (!file_select_read_drive()) {
                file_select_timer = 0;
            }
        }
        menu_works.work_12->message = MESSAGE_SELECT_CARD;
        /* @hack type: the pad's word and its mask cast to int in STEP_DRIVE's
         * cancel and choose tests, and in the choose tests of
         * STEP_FILE_DIALOG, STEP_NO_CHAO_ROOM and STEP_DELETE, where their
         * own Uint32 would do */
        if ((int)pad_press_repeat[0] & (int)pad_cancel_buttons) {
            menu_works.work_12->message = FILE_SELECT_NO_MESSAGE;
            if (file_select_return_mode == MENU_MODE_TITLE) {
                save_set_drive(SAVE_DRIVE_REFUSED);
                save_clear();
            }
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            file_select_leave();
        } else if (pad_press_repeat[0] & PDD_DGT_KU) {
            if (menu_works.work_12->drive % PORT_DRIVES != 0) {
                menu_works.work_12->drive--;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                file_select_timer = 0;
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KD) {
            if (menu_works.work_12->drive % PORT_DRIVES == 0) {
                menu_works.work_12->drive++;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                file_select_timer = 0;
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KL) {
            if (menu_works.work_12->drive >= PORT_DRIVES) {
                menu_works.work_12->drive -= PORT_DRIVES;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                file_select_timer = 0;
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KR) {
            if (menu_works.work_12->drive <= CARD_DRIVES - PORT_DRIVES - 1) {
                menu_works.work_12->drive += PORT_DRIVES;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                file_select_timer = 0;
            }
        } else if ((int)pad_press_repeat[0] & (int)pad_choose_buttons) {
            menu_works.work_12->message = FILE_SELECT_NO_MESSAGE;
            file_select_read_drive();
            if (menu_works.work_12->files_shown &&
                (menu_works.work_12->room || menu_works.work_12->file_count)) {
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                voice_set_play(FILE_LIST_VOICES);
                menu_control.step = STEP_FILE;
                menu_works.work_12->choosing_file = TRUE;
                if (menu_works.work_12->file_count == 0) {
                    menu_works.work_12->on_new = TRUE;
                } else {
                    menu_works.work_12->file = 0;
                    menu_works.work_12->on_new = FALSE;
                }
            } else {
                sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
            }
        }
        break;
    case STEP_FILE:
        /* @hack redundant: a test of room whose two arms set the same
         * message */
        if (menu_works.work_12->room) {
            menu_works.work_12->message = MESSAGE_SELECT_FILE;
        } else {
            menu_works.work_12->message = MESSAGE_SELECT_FILE;
        }
        /* @bug logic: a cancel pressed with a direction or choose goes back
         * to the drives, and then acts on the files as the other button
         * asks. every other step tests cancel in its chain of else ifs, and
         * this one tests it alone (the test of up at 0x8c512854 follows the
         * cancel's arm) */
        if (pad_press_repeat[0] & pad_cancel_buttons) {
            menu_works.work_12->choosing_file = FALSE;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            menu_control.step = STEP_DRIVE;
            menu_works.work_12->message = FILE_SELECT_NO_MESSAGE;
        }
        if (pad_press_repeat[0] & PDD_DGT_KU) {
            if (menu_works.work_12->on_new) {
                if (menu_works.work_12->file_count > 0) {
                    menu_works.work_12->on_new = FALSE;
                    menu_works.work_12->file =
                        menu_works.work_12->file_count - 1;
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            } else if (menu_works.work_12->file != 0) {
                menu_works.work_12->file--;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KD) {
            /* @hack spelling: the move in the else of an empty test, where a
             * test of on_new == 0 would do */
            if (menu_works.work_12->on_new) {
            } else {
                /* @hack spelling: the sound in each arm, where one after the
                 * test would do */
                if (menu_works.work_12->file !=
                    menu_works.work_12->file_count - 1) {
                    menu_works.work_12->file++;
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                } else {
                    menu_works.work_12->on_new = TRUE;
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            }
        } else if (pad_press_repeat[0] & pad_choose_buttons) {
            if (menu_works.work_12->on_new == 0) {
                menu_works.work_12->choosing_file = FALSE;
                menu_works.work_12->dialog_shown = TRUE;
                menu_control.step = STEP_FILE_DIALOG;
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_works.work_12->dialog = FILE_SELECT_DIALOG_FILE;
                menu_works.work_12->choice = CHOICE_BACK;
                menu_works.work_12->message = FILE_SELECT_NO_MESSAGE;
                voice_set_play(FILE_DIALOG_VOICES);
            } else {
                if (menu_works.work_12->room) {
                    if (file_select_chao_file_exists() ||
                        menu_works.work_12->free_blocks >= CHAO_ROOM_BLOCKS) {
                        save_set_drive(menu_works.work_12->drive);
                        sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                        file_select_new_file();
                        menu_control.step = STEP_WAIT_DRIVE;
                        menus_reset();
                    } else {
                        menu_works.work_12->choosing_file = FALSE;
                        menu_works.work_12->dialog_shown = TRUE;
                        menu_works.work_12->dialog = FILE_SELECT_DIALOG_CONFIRM;
                        menu_works.work_12->choice = CHOICE_BACK;
                        menu_control.step = STEP_NO_CHAO_ROOM;
                        sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                        voice_set_play(NO_CHAO_ROOM_VOICES);
                    }
                } else {
                    sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
                }
                menu_works.work_12->message = FILE_SELECT_NO_MESSAGE;
            }
        }
        break;
    case STEP_FILE_DIALOG:
        menu_works.work_12->message = MESSAGE_LOAD;
        if (pad_press_repeat[0] & PDD_DGT_KL) {
            if (menu_works.work_12->choice) {
                menu_works.work_12->choice--;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KR) {
            if (menu_works.work_12->choice != CHOICE_DELETE) {
                menu_works.work_12->choice++;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if ((int)pad_press_repeat[0] & (int)pad_choose_buttons) {
            menu_works.work_12->message = FILE_SELECT_NO_MESSAGE;
            switch (menu_works.work_12->choice) {
            case CHOICE_LOAD:
                save_set_drive(menu_works.work_12->drive);
                save_set_slot(
                    menu_works.work_12->files[menu_works.work_12->file].slot);
#ifdef NONMATCHING
                /* this side goes back to the file list where save_read fails.
                 * the matching side ignores the result and loads on from
                 * whatever save_data then holds */
                if (save_read(0) != SAVE_READ_OK) {
                    menu_works.work_12->choosing_file = TRUE;
                    menu_works.work_12->dialog_shown = FALSE;
                    menu_control.step = STEP_FILE;
                    sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
                    break;
                }
#else
                save_read(0);
#endif
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_control.step = STEP_WAIT_DRIVE;
                music_stop();
                sound_task_kill();
                sound_task_create(NULL);
                music_play_name("advsng_3.adx");
                item_stock_clear_all();
                menus_reset();
                break;
            case CHOICE_BACK:
                menu_works.work_12->choosing_file = TRUE;
                menu_works.work_12->dialog_shown = FALSE;
                menu_control.step = STEP_FILE;
                sound_play(MENU_SOUND_BACK, NULL, 0, 0);
                break;
            case CHOICE_DELETE:
                menu_control.step = STEP_DELETE;
                menu_works.work_12->dialog = FILE_SELECT_DIALOG_CONFIRM;
                menu_works.work_12->choice = CHOICE_BACK;
                menus_reset();
                break;
            }
        } else if (pad_press_repeat[0] & pad_cancel_buttons) {
            menu_works.work_12->message = FILE_SELECT_NO_MESSAGE;
            menu_works.work_12->choosing_file = TRUE;
            menu_works.work_12->dialog_shown = FALSE;
            menu_control.step = STEP_FILE;
        }
        break;
    case STEP_NO_CHAO_ROOM:
        menu_works.work_12->message = MESSAGE_NO_CHAO_ROOM;
        if (pad_press_repeat[0] & PDD_DGT_KL) {
            if (menu_works.work_12->choice) {
                menu_works.work_12->choice = CHOICE_YES;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KR) {
            if (menu_works.work_12->choice == CHOICE_YES) {
                menu_works.work_12->choice = CHOICE_BACK;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if ((int)pad_press_repeat[0] & (int)pad_choose_buttons) {
            menu_works.work_12->message = FILE_SELECT_NO_MESSAGE;
            switch (menu_works.work_12->choice) {
            case CHOICE_YES:
                save_set_drive(menu_works.work_12->drive);
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                file_select_new_file();
                menu_control.step = STEP_WAIT_DRIVE;
                menus_reset();
                break;
            case CHOICE_BACK:
                menu_works.work_12->choosing_file = TRUE;
                menu_works.work_12->dialog_shown = FALSE;
                menu_control.step = STEP_FILE;
                sound_play(MENU_SOUND_BACK, NULL, 0, 0);
                break;
            }
        } else if (pad_press_repeat[0] & pad_cancel_buttons) {
            menu_works.work_12->message = FILE_SELECT_NO_MESSAGE;
            menu_works.work_12->choosing_file = TRUE;
            menu_works.work_12->dialog_shown = FALSE;
            menu_control.step = STEP_FILE;
        }
        break;
    case STEP_DELETE:
        menu_works.work_12->message = MESSAGE_DELETE;
        if (pad_press_repeat[0] & PDD_DGT_KL) {
            if (menu_works.work_12->choice) {
                menu_works.work_12->choice--;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KR) {
            if (menu_works.work_12->choice != CHOICE_BACK) {
                menu_works.work_12->choice++;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if ((int)pad_press_repeat[0] & (int)pad_choose_buttons) {
            menu_works.work_12->message = FILE_SELECT_NO_MESSAGE;
            switch (menu_works.work_12->choice) {
            case CHOICE_YES:
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                save_set_drive(menu_works.work_12->drive);
                save_set_slot(
                    menu_works.work_12->files[menu_works.work_12->file].slot);
                save_delete();
                menu_control.step = STEP_DELETING;
                save_set_drive(SAVE_DRIVE_REFUSED);
                save_set_slot(0);
                item_stock_clear_all();
                menus_reset();
                break;
            case CHOICE_BACK:
                menu_works.work_12->choosing_file = TRUE;
                menu_works.work_12->dialog_shown = FALSE;
                menu_control.step = STEP_FILE;
                sound_play(MENU_SOUND_BACK, NULL, 0, 0);
                break;
            }
        } else if (pad_press_repeat[0] & pad_cancel_buttons) {
            menu_works.work_12->message = FILE_SELECT_NO_MESSAGE;
            menu_works.work_12->choosing_file = TRUE;
            menu_works.work_12->dialog_shown = FALSE;
            menu_control.step = STEP_FILE;
        }
        break;
    case STEP_WAIT_DRIVE:
        if (save_busy() == 0) {
            menu_control.step = STEP_LEFT;
            file_select_leave();
        }
        break;
    case STEP_DELETING:
        /* @bug logic: the wait for the delete passes at once, so the drive
         * is listed again while its delete may still be queued. the delete's
         * case set save_place's drive to SAVE_DRIVE_REFUSED (MOV #-2,R4 at
         * 0x8c512dde), and save_busy returns 0 without a drive (the call at
         * 0x8c512e4e) */
        if (save_busy() == 0) {
            menu_works.work_12->files_shown = FALSE;
            menu_works.work_12->choosing_file = FALSE;
            menu_works.work_12->dialog_shown = FALSE;
            menu_control.step = STEP_OPEN;
            file_select_read_drive();
        }
        break;
    case STEP_UNK_07:
    case STEP_LEFT:
        break;
    }
    return FALSE;
}

/* note the device in each drive's socket in the work. it returns TRUE where
 * one has changed since the last scan, and FALSE otherwise */
static Bool file_select_scan_devices(void)
{
    const PDS_PERIPHERALINFO *info;
    Bool changed;
    int i;

    changed = FALSE;
    for (i = 0; i < CARD_DRIVES; i++) {
        info = pdGetPeripheralInfo(drive_ports[i]);
        if (info->type & PDD_DEVTYPE_STORAGE) {
            if (info->type & PDD_DEVTYPE_LCD) {
                menu_works.work_12->devices[i] = FILE_SELECT_CARD_LCD;
            } else {
                menu_works.work_12->devices[i] = FILE_SELECT_CARD;
            }
        } else if (info->type & PDD_DEVTYPE_VIBRATION) {
            menu_works.work_12->devices[i] = FILE_SELECT_VIBRATION;
        } else {
            menu_works.work_12->devices[i] = FILE_SELECT_NO_DEVICE;
        }
        if (file_select_devices_seen[i] != menu_works.work_12->devices[i]) {
            changed = TRUE;
            file_select_devices_seen[i] = menu_works.work_12->devices[i];
        }
    }
    return changed;
}

/* list the drive under the cursor in the work, and put the cursor on its
 * first file. it returns TRUE, or FALSE where buGetDiskInfo fails.
 * a failure sets files_shown to 0, and leaves the work's files as they were.
 * save_data is as it was after either */
static Bool file_select_read_drive(void)
{
    struct file_select_list list;
    int i;

    save_keep();
    if (file_select_list_files(menu_works.work_12->drive, &list) == 0) {
        menu_works.work_12->file = 0;
        menu_works.work_12->file_count = list.count;
        menu_works.work_12->total_blocks = list.total_blocks;
        menu_works.work_12->free_blocks = list.free_blocks;
        if (menu_works.work_12->free_blocks < SAVE_BLOCKS) {
            menu_works.work_12->room = FALSE;
        } else {
            menu_works.work_12->room = TRUE;
        }
        for (i = 0; i < FILE_SELECT_SLOTS; i++) {
            menu_works.work_12->files[i].stage = list.files[i].stage;
            menu_works.work_12->files[i].character = list.files[i].character;
            menu_works.work_12->files[i].emblems = list.files[i].emblems;
            menu_works.work_12->files[i].slot = list.files[i].slot;
            menu_works.work_12->files[i].time = list.files[i].time;
        }
        menu_works.work_12->files_shown = TRUE;
        /* @bug logic: leaves game_state.emblems at the last listed file's
         * count. save_restore puts back save_data alone (JSR save_restore at
         * 0x8c513214), and each good read of the listing set
         * game_state.emblems from its file. after a cancel back to
         * MENU_MODE_OPTIONS, the next stage's end finds the count apart from
         * save_data's, unless a later read set it (game_mode_divert's CMP/EQ
         * at 0x8c02d14e). the emblem screen then counts up from it */
        save_restore();
        return TRUE;
        /* @hack spelling: the else after a branch that returns, where an if
         * of its own would do */
    } else {
        menu_works.work_12->files_shown = FALSE;
        save_restore();
        return FALSE;
    }
}

/* read drive's blocks and its files into list. a file is a slot below
 * FILE_SELECT_SLOTS that save_read reads with SAVE_READ_OK. each read
 * overwrites save_data, so the caller must keep it. it returns buGetDiskInfo's
 * result, and reads no file where that isn't 0. save_place is as it was
 * after */
static Sint32 file_select_list_files(int drive, struct file_select_list *list)
{
    BUS_DISKINFO info;
    struct save_place kept;
    /* @hack redundant: the name's initial value, which the copy of
     * save_place.name overwrites */
    char name[] = "SONIC2___S01";
    Sint32 result;
    int i;
    int slot;

    if ((result = buGetDiskInfo(drive, &info)) != 0) {
        return result;
    }
    kept = save_place;
    /* @hack type: i an int cast to Uint32 in this copy's test and the copy
     * back's, where a Uint32 i would do */
    for (i = 0; (Uint32)i < sizeof(name); i++) {
        name[i] = save_place.name[i];
    }
    list->total_blocks = info.total_blocks;
    list->free_blocks = info.free_blocks;
    list->count = 0;
    save_set_drive(drive);
    for (slot = 0; slot < FILE_SELECT_SLOTS; slot++) {
        save_set_slot(slot);
        if (save_read(READ_KEEP) == SAVE_READ_OK) {
            /* @hack redundant: a wait on save_busy, which save_read has made
             * already */
            while (save_busy() != 0) {
            }
            list->files[list->count].emblems = save_data.emblems;
            list->files[list->count].stage = save_data.stage;
            list->files[list->count].character = save_data.character;
            list->files[list->count].time.hour =
                save_data.play_time / STEPS_PER_HOUR;
            list->files[list->count].time.minute =
                save_data.play_time % STEPS_PER_HOUR / STEPS_PER_MINUTE;
            list->files[list->count].time.second =
                save_data.play_time % STEPS_PER_MINUTE / STEPS_PER_SECOND;
            list->files[list->count].slot = slot;
            list->count++;
        }
    }
    save_place = kept;
    for (i = 0; (Uint32)i < sizeof(name); i++) {
        save_place.name[i] = name[i];
    }
    return result;
}

/* make save_data a new save. start writing it to the first slot from 1
 * whose read can't start. where every slot reads, it writes nothing, and
 * sets save_place's drive to SAVE_DRIVE_REFUSED. it empties the item stocks
 * only after a write */
static void file_select_new_file(void)
{
    int slot;

    for (slot = 1; slot < FILE_SELECT_SLOTS; slot++) {
        save_set_slot(slot);
        if (save_read(READ_KEEP) == SAVE_READ_NOT_STARTED) {
            break;
        }
        /* @hack redundant: a call of save_busy, in a loop whose test fails
         * after it */
        while (save_busy() != 0 && 0) {
        }
    }
    if (slot == FILE_SELECT_SLOTS) {
        save_set_drive(SAVE_DRIVE_REFUSED);
        save_set_slot(0);
        save_clear();
        return;
    }
    save_clear();
    save_write();
    item_stock_clear_all();
}

static void file_select_leave(void)
{
    music_stop();
    sound_task_kill();
    sound_task_create(NULL);
    if (file_select_return_mode == MENU_MODE_OPTIONS) {
        file_select_close();
        menu_control.mode = MENU_MODE_OPTIONS;
        options_continue(OPTIONS_CONTINUE_ITEMS);
        music_play_name("advsng_3.adx");
    } else if (file_select_return_mode == MENU_MODE_TITLE) {
        file_select_close();
        menu_control.mode = MENU_MODE_TITLE;
        title_screen_continue(TITLE_SCREEN_CONTINUE);
    }
}

Bool file_select_card_present(void)
{
    const PDS_PERIPHERALINFO *info;
    int i;

    for (i = 0; i < CARD_DRIVES; i++) {
        info = pdGetPeripheralInfo(drive_ports[i]);
        if (info->type & PDD_DEVTYPE_STORAGE) {
            return TRUE;
        }
    }
    return FALSE;
}

Bool file_select_vibration_present(void)
{
    const PDS_PERIPHERALINFO *info;
    int i;

    for (i = 0; i < CARD_DRIVES; i++) {
        info = pdGetPeripheralInfo(drive_ports[i]);
        if (info->type & PDD_DEVTYPE_VIBRATION) {
            return TRUE;
        }
    }
    return FALSE;
}

static Bool file_select_chao_file_exists(void)
{
    BUS_FILEINFO info;
    /* @hack spelling: the drive and the name in locals, where the call's
     * arguments would do */
    Sint32 drive;
    char *name;

    drive = menu_works.work_12->drive;
    name = chao_file_name;
    if (buGetFileInfo(drive, name, &info) != 0) {
        return FALSE;
    }
    return TRUE;
}
