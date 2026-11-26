/* the download menu, MENU_MODE_DOWNLOAD. it lists the download files on the
 * memory cards, and plays the kart course a file holds */
#ifndef ADVERTISE_DOWNLOAD_H
#define ADVERTISE_DOWNLOAD_H
#include <sg_xpt.h>

/* start the menu at its first step, select its textures, and play
 * advsng_3.adx and the menu's voice. it leaves menu_control.mode to the
 * caller */
void download_start(void);
/* the menu's step, under MENU_MODE_DOWNLOAD. it lists the files of
 * DOWNLOAD_KIND_1 to DOWNLOAD_KIND_4, but plays only those of
 * DOWNLOAD_KIND_3 and DOWNLOAD_KIND_4. a file plays only where save_data
 * ranks the first mission of each stage the file names.
 *
 * it returns TRUE to end the menus for the file's race, and FALSE
 * otherwise. the race is STAGE_CART_2's, for one player as Sonic, and
 * game_state.kart_course is KART_RACE_DOWNLOADED. download_chosen holds the
 * file's entry, and menu_resume_target is MENU_RESUME_DOWNLOAD.
 * menu_snapshot_save has kept the menus' works. the file's read into DEMO_AREA
 * of system/areas.h has started, where save_download_read could start it.
 *
 * as it goes back to the new menu's extras group, it sets menu_control.mode
 * to MENU_MODE_NEW_MENU */
Bool download_step(void);
/* come back to the menu after its race, at its first step, and set
 * menu_control.mode to MENU_MODE_DOWNLOAD. it restores the menus' works
 * from menu_snapshot_save's snapshot. it starts the textures and the music
 * with kart_race_start */
void download_resume(void);

#endif
