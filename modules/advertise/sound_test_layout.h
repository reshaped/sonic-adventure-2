/* the sound test panel's layout, its view and its art */
#ifndef ADVERTISE_SOUND_TEST_LAYOUT_H
#define ADVERTISE_SOUND_TEST_LAYOUT_H
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_draw.h"
#include "advertise/menus.h"
#include "camera/viewport.h"
#include "system/angle.h"

struct task;

/* a list as the panel shows it. scroll points at its top and cursor, and moves
 * them toward the work's row */
struct menu_sound_test_list_view {
    Float fade;
    Sint32 top;
    Sint32 cursor;
    struct menu_scroll scroll;
};
/* the task's view of menu_sound_test_work */
struct menu_sound_test_view {
    Float backdrop_fade;
    NJS_POINT3 disc_pos;
    struct angles disc_ang;
    struct menu_sound_test_list_view lists[MENU_SOUND_TEST_LISTS];
};

/* where the prompt's message draws */
extern NJS_POINT3 menu_sound_test_message_pos;
/* the view menu_sound_test_panel_reset restores: the disc slid out of view, and
 * every fade at 0 */
extern struct menu_sound_test_view menu_sound_test_view_start;
/* the task menu_sound_test_panel_create made, or null */
extern struct task *menu_sound_test_task;
/* the area the disc draws in */
extern struct viewport_area menu_sound_test_disc_area;
/* where a list of songs draws its arrows. the up arrow shows while rows lie
 * above the window. the down arrow shows while rows lie below it */
extern Float menu_sound_test_arrow_x;
extern Float menu_sound_test_arrow_up_y;
extern Float menu_sound_test_arrow_down_y;
/* list 0, which names the lists of songs, as menu_list_draw draws it. each list
 * of songs draws through menu_sound_test_songs_list, with its own rows and
 * count */
extern struct menu_list menu_sound_test_lists_list;
extern struct menu_list menu_sound_test_songs_list;
/* menu_sound_test_songs_list.step.y under a second name, which symbols.txt
 * places at the field. it is declared only for the hacks tagged at its uses */
#ifdef NONMATCHING
/* the field by its own spelling: this side's objects don't define the second
 * name, which only symbols.txt places */
#define menu_sound_test_songs_step_y (menu_sound_test_songs_list.step.y)
#else
extern Float menu_sound_test_songs_step_y;
#endif
extern NJS_TEXLIST menu_sound_test_texlist;
extern NJS_CNK_OBJECT menu_sound_test_disc_object;

void menu_sound_test_sprite_draw(Int cell, Float scale, NJS_POINT3 *pos,
                                 Int attr);
#endif
