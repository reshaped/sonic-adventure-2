/* the menus' shared digits and mini emblem. the other menus draw numbers and
 * times with them. every group of menu_textures_select loads their
 * textures */
#ifndef ADVERTISE_SHARE_DATA_H
#define ADVERTISE_SHARE_DATA_H
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menus.h"
#include "save/save.h"
#include "system/menu_snapshot.h" /* IWYU pragma: export */

/* the cells. cell n is the digit n, for n from 0 to 9. the time draws put a
 * separator between each two numbers */
#define SHARE_DATA_SEPARATOR_CELL 10
#define SHARE_DATA_ALT_SEPARATOR_CELL 11
#define SHARE_DATA_EMBLEM_CELL 13 /* the mini emblem */

/* share_data_reset restores this word along with the panel. */
extern Sint32 share_data_reset_word;
extern struct share_data_work share_data_work;

/* create the task, advShareDataExec, and clear the whole work, its loaded
 * flag too. it centers the cells. the task's destructor releases the
 * textures */
void share_data_create(void);
/* clear the work but its loaded flag */
void share_data_reset(void);
/* load the textures, the PVM advShareData, where the work says they aren't
 * loaded */
void share_data_load(void);
/* release the textures, where the work says they are loaded */
void share_data_release(void);

/* draw cell centered at *pos's x and y, at priority pos->z, under
 * njDrawSprite2D's attributes attr. share_data_sprite_draw scales the cell
 * by scale both ways, and share_data_sprite_draw_tall scales its height
 * alone. the textures must be loaded, and share_data_create must have run */
void share_data_sprite_draw(Int cell, Float scale, const NJS_POINT3 *pos,
                            Int attr);
void share_data_sprite_draw_tall(Int cell, Float scale, const NJS_POINT3 *pos,
                                 Int attr);

/* draw value in decimal under MENU_SPRITE_ATTR, its places scale * 23
 * pixels apart. digits counts the places. 0 gives a place to each digit of
 * value. a count above 0 gives that many places, and skips the zeros that
 * lead. its last place always draws. a count below 0 gives as many places
 * as its size, and draws the zeros that lead. share_data_number_draw
 * puts the first digit it draws at *pos, so a zero it skips takes no room.
 * share_data_number_draw_right puts the last place at *pos.
 *
 * value must not be below 0, unchecked: its digits would draw cells below
 * 0. it must not have more digits than the places, unchecked: its first
 * cell would then be past 9. digits must lie from -10 to 10, unchecked: a
 * power of ten overflows otherwise */
void share_data_number_draw(const NJS_POINT3 *pos, Int value, Int digits,
                            Float scale);
void share_data_number_draw_right(const NJS_POINT3 *pos, Int value, Int digits,
                                  Float scale);

/* draw time's minute, second and hundredths from *pos, in two places each,
 * with SHARE_DATA_SEPARATOR_CELL between them. scale scales the cells and
 * the gaps. share_data_time_draw_alt draws SHARE_DATA_ALT_SEPARATOR_CELL
 * between them instead, and share_data_time_draw_short leaves out the
 * hundredths */
void share_data_time_draw(const NJS_POINT3 *pos, const struct save_time *time,
                          Float scale);
void share_data_time_draw_alt(const NJS_POINT3 *pos,
                              const struct save_time *time, Float scale);
void share_data_time_draw_short(const NJS_POINT3 *pos,
                                const struct save_time *time, Float scale);
/* draw clock's hour in three places, then its minute and second in two, with
 * SHARE_DATA_SEPARATOR_CELL between them. the hour starts 12 pixels left of
 * *pos, whatever the scale */
void share_data_clock_draw(const NJS_POINT3 *pos,
                           const struct share_data_clock *clock, Float scale);

#endif
