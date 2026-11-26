/* the menus' textures: which works a menu keeps loaded, and their reload
 * across a change of display */
#ifndef ADVERTISE_MENU_TEXTURES_H
#define ADVERTISE_MENU_TEXTURES_H
#include <sg_xpt.h>

/* the groups of works menu_textures_select takes, each a menu's. a group
 * bears the number of its menu's own work in menu_works.
 * MENU_TEXTURES_DOWNLOAD is the exception: the download menu shows the kart
 * race's work */
enum {
    MENU_TEXTURES_TITLE = 0,
    MENU_TEXTURES_MAIN_MENU = 1,
    MENU_TEXTURES_STAGE_MAP = 2,
    MENU_TEXTURES_STORY_SELECT = 3,
    MENU_TEXTURES_VERSUS_SELECT = 4,
    MENU_TEXTURES_NEW_MENU = 5,
    MENU_TEXTURES_SYNOPSIS = 6,
    MENU_TEXTURES_OPTIONS = 8,
    MENU_TEXTURES_TV_SETTING = 9,
    MENU_TEXTURES_SOUND_TEST = 10,
    MENU_TEXTURES_FILE_SELECT = 12,
    MENU_TEXTURES_KART_RACE = 13,
    MENU_TEXTURES_DOWNLOAD = 14,
    MENU_TEXTURES_BOSS_ATTACK = 15,
    MENU_TEXTURES_TUTORIAL = 16,
    MENU_TEXTURES_WORLD_RANKING = 18,
    MENU_TEXTURES_EMBLEM_RESULTS = 19
};

/* release the textures of every work outside group, and gather Ninja's
 * free texture memory. then load the textures of group's works, but those
 * loaded already. each group the enum above names takes the screen
 * effect's work besides its menu's. every group takes the share data's
 * work, and a group the enum doesn't name takes that alone */
void menu_textures_select(Int group);
/* change the display to display, a DISPLAY_ value of system/game_state.h,
 * unchecked. the selected works' textures are released across the change,
 * and loaded again after it. Kamui's vertex buffer and the debug print
 * start again as system_boot_init leaves them. game_state.display stays as
 * it was */
void menu_textures_change_display(Int display);

#endif
