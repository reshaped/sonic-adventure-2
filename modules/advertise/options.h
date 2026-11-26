/* the options, MENU_MODE_OPTIONS. each item on its wheel opens another menu,
 * or sets one of game_state's settings */
#ifndef ADVERTISE_OPTIONS_H
#define ADVERTISE_OPTIONS_H
#include <sg_xpt.h>

/* the item wheel's items, as options_work.item holds them */
enum options_item {
    OPTIONS_ITEM_MUSIC, /* enters MENU_MODE_SOUND_TEST */
    OPTIONS_ITEM_FILE, /* enters MENU_MODE_FILE_SELECT */
    OPTIONS_ITEM_VIBRATION,
    OPTIONS_ITEM_SOUND, /* stereo or mono */
    OPTIONS_ITEM_LANGUAGE, /* the text's language and the voices' */
    OPTIONS_ITEM_THEME,
    /* enters MENU_MODE_TV_SETTING. the wheel holds this last item only once
     * options_work.tv_setting_shown is 1 */
    OPTIONS_ITEM_TV_SETTING,
    OPTIONS_ITEMS
};

/* start the options at their first step, select their textures, and play
 * advsng_3.adx where it isn't playing already. it leaves menu_control.mode
 * to the caller.
 *
 * it marks MENU_DEFAULT_THEME found in options_work.theme_found. it also
 * marks each theme whose file on a memory card holds its key. it waits for
 * each read to end. where a file lacks its key, it sets SAVE_THEMES_REFUSED in
 * save_data.status and starts save_write. a file whose read fails is judged
 * by what the buffer held before the read. while the bit is set, it reads
 * no file, and finds MENU_DEFAULT_THEME alone */
void options_start(void);
/* options_continue's which for the item wheel */
#define OPTIONS_CONTINUE_ITEMS 0
/* go back to the item wheel where which is OPTIONS_CONTINUE_ITEMS. any other
 * which does nothing. it leaves menu_control.mode to the caller */
void options_continue(int which);
/* the options' step, under MENU_MODE_OPTIONS. it returns FALSE.
 *
 * the lists set game_state's mono, vibration, text_language and
 * voice_language, and the theme wheel its theme. the theme wheel opens at
 * the row it was last left on, not at game_state.theme's. the music, file
 * and TV setting items start their menus, and set menu_control.mode. the
 * file item shows a warning instead where no drive holds a memory card. the
 * vibration list opens only where a drive holds a vibration unit.
 *
 * a cancel of the item wheel goes back to the new menu's top group. once a
 * list or the theme wheel has had a choice, the cancel first writes the
 * save. options_start forgets earlier choices. the cancel then also sets the
 * console's sound mode by
 * game_state.mono, through syCfgInit with DEMO_AREA as its work. where
 * save_check refuses the write, the save's warning shows. the step writes
 * the save once a drive's device changes and save_check then passes. where
 * the warning ends first, nothing is written, and save_place.drive becomes
 * SAVE_DRIVE_REFUSED */
Bool options_step(void);
/* the theme at options_work.theme_row, counting only the themes found, or
 * MENU_DEFAULT_THEME where theme_row is past the last */
Sint8 options_row_theme(void);

#endif
