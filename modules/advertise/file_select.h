/* the file select, MENU_MODE_FILE_SELECT. the player picks a memory card,
 * then loads or deletes a file on it, or makes one. the step sets the work,
 * and the task file_select_create makes shows it */
#ifndef ADVERTISE_FILE_SELECT_H
#define ADVERTISE_FILE_SELECT_H
#include <sg_xpt.h>

#include "advertise/share_data.h"
#include "save/card.h"
#include "system/menu_snapshot.h" /* IWYU pragma: export */

/* what a drive holds */
enum file_select_device {
    FILE_SELECT_NO_DEVICE,
    FILE_SELECT_CARD_LCD, /* a memory card with a screen */
    FILE_SELECT_CARD, /* a memory card without a screen */
    FILE_SELECT_VIBRATION
};

/* the work's dialog: a file's three choices, or a question's two */
enum file_select_dialog { FILE_SELECT_DIALOG_FILE, FILE_SELECT_DIALOG_CONFIRM };

/* the work's message while it shows none */
#define FILE_SELECT_NO_MESSAGE (-1)

/* the work */
extern struct file_select_work file_select_work;

/* create the task, fileSelectExec, and clear the whole work, its loaded flag
 * too. the drives start swung out. the task's destructor releases the
 * textures */
void file_select_create(void);
/* load the textures, the PVM fileSelect, where the work says they aren't
 * loaded */
void file_select_load(void);
/* release the textures, where the work says they are loaded */
void file_select_release(void);

/* the menu the file select leaves for. a
 * caller sets it to MENU_MODE_TITLE or MENU_MODE_OPTIONS before
 * file_select_start. under any other value the file select never leaves */
extern Sint32 file_select_return_mode;

/* start the file select at its first step, with no message. it leaves
 * menu_control.mode to the caller */
void file_select_start(void);
/* the file select's step, under MENU_MODE_FILE_SELECT. it returns FALSE:
 * the file select never ends the menus.
 *
 * the step lists the files of a drive chosen or rested on. a listing keeps
 * save_data and save_place, but game_state.emblems keeps the count of the
 * last file read. a change of any drive's device starts the file select
 * over.
 *
 * a load reads the file with save_read(0), at its drive and slot. a new file
 * needs SAVE_BLOCKS free on the drive. where the drive lacks the Chao world's
 * file and room for it too, the step asks first. a new file is a new
 * save_data, written to the first slot from 1 whose read can't start. where
 * every slot reads, nothing is written, and save_place's drive is
 * SAVE_DRIVE_REFUSED. after a load or a new file, the step waits for the
 * drive, then leaves. a delete sets save_place's drive to SAVE_DRIVE_REFUSED,
 * and starts the file select over.
 *
 * a load and a new file run menus_reset. so does the file's delete choice,
 * before its question is answered. a load, a delete and a new file written
 * empty the item stocks.
 *
 * a cancel on the drives leaves at once. where file_select_return_mode is
 * MENU_MODE_TITLE, it first sets save_place's drive to SAVE_DRIVE_REFUSED,
 * and makes save_data a new save. a leave makes the sound task again, sets
 * game_state.card_state to CARD_IDLE and goes back to
 * file_select_return_mode's menu */
Bool file_select_step(void);
/* TRUE where any drive holds a memory card, and FALSE otherwise */
Bool file_select_card_present(void);
/* TRUE where any drive holds a vibration unit, and FALSE otherwise */
Bool file_select_vibration_present(void);

#endif
