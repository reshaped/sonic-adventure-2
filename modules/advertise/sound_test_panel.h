/* the sound test's panel, the task soundTestExec. it draws a backdrop under
 * its title, a spinning disc and the sound test's lists */
#ifndef ADVERTISE_SOUND_TEST_PANEL_H
#define ADVERTISE_SOUND_TEST_PANEL_H
#include <sg_xpt.h>

#include "advertise/menus.h"

/* the work, menu_works.work_10 */
extern struct menu_sound_test_work menu_sound_test_work;

/* create the task, and clear the whole work, its loaded flag too. the panel
 * then starts as menu_sound_test_panel_reset leaves it. the task's
 * destructor releases the textures */
void menu_sound_test_panel_create(void);
/* clear the work but its loaded flag, and give list 0 a row for each list
 * of songs. the panel starts again hidden, with the disc out of sight and
 * each list at its first row */
void menu_sound_test_panel_reset(void);
/* load the textures, the PVM soundTest, where the work says they aren't
 * loaded */
void menu_sound_test_panel_load(void);
/* release the textures, where the work says they are loaded */
void menu_sound_test_panel_release(void);

/* fill each list of songs with the songs of its group that music_heard says
 * are heard. they keep the group's order. the work's lists[n].rows then
 * counts list n's songs */
void menu_sound_test_lists_build(void);
/* the file name of the song on row of list. list is a list of songs, from 1
 * to MENU_SOUND_TEST_LISTS - 1. row must lie below the list's rows,
 * unchecked. a row past them names the song of a row above it, or reads
 * songs[-1] */
const char *menu_sound_test_song_name(Int list, Int row);

#endif
