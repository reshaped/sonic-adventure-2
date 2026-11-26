/* the stage map's menu, MENU_MODE_STAGE_MAP. the player picks a stage on
 * stage_map.h's grid, then a mission, and the menus end on that game */
#ifndef ADVERTISE_STAGE_MAP_MENU_H
#define ADVERTISE_STAGE_MAP_MENU_H
#include <sg_xpt.h>

/* start the menu at its first step, select its textures and play
 * advsng_4.adx. it leaves menu_control.mode to the caller. it marks Tails
 * and his mech in save_data.played where either is marked, and Eggman and
 * his mech alike */
void stage_map_menu_start(void);
/* the menu's step, under MENU_MODE_STAGE_MAP. on the grid, Choose takes a
 * lit cell, and Green Hill's starts at once. on the missions, Choose takes
 * one that stage_map_mission_open finds open.
 *
 * it returns TRUE to end the menus for the game, once the map has gone,
 * and FALSE otherwise. stage_map_start has readied the game,
 * menu_resume_target is MENU_RESUME_STAGE_MAP, and menu_snapshot_save has
 * kept the menus' works.
 *
 * Cancel on the grid goes back to the new menu's single-player group once
 * the map has gone. it then sets menu_control.mode to MENU_MODE_NEW_MENU.
 * Cancel on the missions goes back to the grid */
Bool stage_map_menu_step(void);
/* come back to the menu after its game, and set menu_control.mode to
 * MENU_MODE_STAGE_MAP. it restores the menus' works from
 * menu_snapshot_save's snapshot. it starts the menu as stage_map_menu_start
 * does, with the missions hidden */
void stage_map_menu_resume(void);

#endif
