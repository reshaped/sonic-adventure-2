/* the kart race panel, the task kartRaceExec, which menus_init creates. the
 * kart race menu and the download menu both show it through kart_race_work */
#ifndef ADVERTISE_KART_RACE_PANEL_H
#define ADVERTISE_KART_RACE_PANEL_H
#include <sg_xpt.h>

#include "advertise/menus.h"
#include "save/save.h"

/* the drivers a player picks from */
#define KART_RACE_DRIVERS 6
/* the row of a download file's course, after a course row for each of
 * save_data's kart records */
#define KART_RACE_DOWNLOADED SAVE_KART_RECORDS
/* kart_race_work.choice's answers to the panel's question */
#define KART_RACE_YES 0
#define KART_RACE_NO 1

/* menu_works.work_13, and the download files the two menus find */
extern struct kart_race_work kart_race_work;
extern struct download_entry kart_race_downloads[DOWNLOAD_SLOTS];

/* create the task, kartRaceExec, and clear the whole work, its loaded flag
 * too. every part starts hidden. the task's destructor releases the
 * textures */
void kart_race_panel_create(void);
/* clear the work but its loaded flag, and hide every part at once. the
 * list's window goes back to its first entry */
void kart_race_panel_reset(void);
/* load the textures, where the work says they aren't loaded. they are the
 * PVMs kartRace and kartRaceBg, and the file cartData.pvm */
void kart_race_panel_load(void);
/* release the textures, where the work says they are loaded */
void kart_race_panel_release(void);
/* set the list to the first count of entries, with the cursor and the
 * window on the first. the panel keeps entries, and reads them while the
 * list or the question shows */
void kart_race_panel_set_list(struct download_entry *entries, int count);

#endif
