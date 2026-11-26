/* the world ranking's menu, MENU_MODE_WORLD_RANKING. the new menu starts it
 * from NEW_MENU_EXTRAS_RANKING, which new_menu_screen_start always
 * disables */
#ifndef ADVERTISE_WORLD_RANKING_MENU_H
#define ADVERTISE_WORLD_RANKING_MENU_H
#include <sg_xpt.h>

/* start the menu at its first step, select its textures, gather
 * emblem_results.h's lines and play advsng_3.adx. it leaves
 * menu_control.mode to the caller */
void world_ranking_menu_start(void);
/* the menu's step, under MENU_MODE_WORLD_RANKING. it does nothing and
 * returns FALSE. so the menus stay in it until a reset ends them, as
 * pad_reset_pressed finds one */
Bool world_ranking_menu_step(void);

#endif
