/* the kart race menu, MENU_MODE_KART_RACE. each player picks a driver, and
 * then a course or a download file's course */
#ifndef ADVERTISE_KART_RACE_H
#define ADVERTISE_KART_RACE_H
#include <sg_xpt.h>

/* start the menu at its first step, select its textures, and play
 * advsng_4.adx. it leaves menu_control.mode to the caller */
void kart_race_start(void);
/* the menu's step, under MENU_MODE_KART_RACE. the second player takes part
 * while kart_race_work.two_players is 1. losing the second pad then leaves
 * the menu.
 *
 * it returns TRUE to end the menus for the race, and FALSE otherwise. the
 * race is STAGE_CART_2's, for the drivers picked. game_state.kart_course is
 * the course row chosen, and menu_resume_target is MENU_RESUME_KART_RACE.
 * menu_snapshot_save has kept the menus' works. for KART_RACE_DOWNLOADED,
 * download_chosen holds the file's entry. the file's read into DEMO_AREA of
 * system/areas.h has then started, where save_download_read could start it.
 *
 * it leaves for the emblem results where kart_race_work.from_emblems is 1,
 * and sets menu_control.mode to MENU_MODE_EMBLEM_RESULTS. otherwise it goes
 * back to the new menu's group for one player or two. it then sets
 * menu_control.mode to MENU_MODE_NEW_MENU */
Bool kart_race_step(void);
/* come back to the menu after its race, and set menu_control.mode to
 * MENU_MODE_KART_RACE. it restores the menus' works from
 * menu_snapshot_save's snapshot, and starts the menu with kart_race_start.
 * the step goes on at the course list after a wait */
void kart_race_resume(void);

#endif
