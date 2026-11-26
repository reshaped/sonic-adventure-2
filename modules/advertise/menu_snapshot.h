/* the menus' snapshot and their resume. the snapshot copies every menu's
 * work into the main executable's data, which lasts while the program is
 * gone */
#ifndef ADVERTISE_MENU_SNAPSHOT_H
#define ADVERTISE_MENU_SNAPSHOT_H
#include <sg_xpt.h>

#include "advertise/alpha_title.h"
#include "advertise/file_select.h"
#include "advertise/menus.h"
#include "system/menu_snapshot.h" /* IWYU pragma: export */

/* the values of main.h's menu_resume_target, each naming the menu
 * menus_resume resumes. the versus select resumes at its list of games or
 * of stages. the new menu sets MENU_RESUME_VERSUS as it chooses the Chao
 * race, and resumes at its versus group */
#define MENU_RESUME_VERSUS_GAMES 1
#define MENU_RESUME_VERSUS_STAGES 2
#define MENU_RESUME_STAGE_MAP 3
#define MENU_RESUME_KART_RACE 4
#define MENU_RESUME_BOSS_ATTACK 5
#define MENU_RESUME_EMBLEM_RESULTS 6
#define MENU_RESUME_VERSUS 7
#define MENU_RESUME_DOWNLOAD 8
/* start the menu that menu_resume_target names, or the title screen for
 * any other value. it then sets menu_resume_target to 0. while
 * game_state.display_pending isn't 0, it opens the TV setting instead. it
 * hides screen_fade.h's cover */
void menus_resume(void);
/* copy each menu's work into the snapshot. each copy's loaded is 0, so a
 * menu restored from it loads its textures again */
void menu_snapshot_save(void);
/* copy the snapshot over each menu's work. each work's loaded comes back
 * 0, as menu_snapshot_save kept it */
void menu_snapshot_restore(void);

#endif
