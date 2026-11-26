/* the menus' draws: a list's rows, a row of sprites, and a backdrop that
 * covers the screen */
#ifndef ADVERTISE_MENU_DRAW_H
#define ADVERTISE_MENU_DRAW_H
#include <ninja.h>
#include <sg_xpt.h>

/* the call a list and a row of sprites draw each cell with. it draws cell
 * at *pos, scaled by scale, under njDrawSprite2D's attributes attr. pos is
 * the caller's copy, which the call may change */
typedef void (*menu_sprite_drawer)(Int cell, Float scale, NJS_POINT3 *pos,
                                   Int attr);
/* the attributes a list and a row of sprites draw with */
#define MENU_SPRITE_ATTR (NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR)

/* a list's row. a cell below 0 draws nothing */
struct menu_row {
    Sint32 lit; /* the cell while the cursor is on the row */
    Sint32 dim; /* the cell while it isn't */
};
/* a list of rows for menu_list_draw. each row draws its cell, and a back
 * cell behind it */
struct menu_list {
    const struct menu_row *rows;
    Sint32 count;
    /* the cell behind the row with the cursor, and the cell behind each
     * other row. a cell below 0 draws nothing */
    Sint32 lit_back;
    Sint32 dim_back;
    /* while game_state.text_language is above 0, each row's cell is this
     * many cells further on */
    Sint32 language_offset;
    NJS_POINT3 pos; /* where the window's first row draws */
    NJS_POINT3 step; /* from one row's place to the next's */
    menu_sprite_drawer draw;
};
/* a window of shown rows on a list */
struct menu_scroll {
    Sint32 *top; /* the list's row at the window's top */
    Sint32 *cursor; /* the cursor's row in the window, from 0 */
    Sint32 target; /* the list's row the cursor moves toward */
    Sint32 shown; /* the rows the window holds */
};

/* move the cursor one row toward target. a cursor that would leave the
 * window stays, and the window moves one row instead. a target of 0 sets
 * *top and *cursor to 0 at once */
void menu_scroll_step(const struct menu_scroll *scroll);
/* draw the window's rows of list, but none past the list's last. the row
 * with the cursor draws at scale, and the others at 1. a row's back draws
 * at its pos.z less 1 */
void menu_list_draw(const struct menu_list *list,
                    const struct menu_scroll *scroll, Float scale);

/* a row of count cells of tanim from first, side by side. draw draws each
 * at scale 1. cell first draws at *pos. each next cell's x is the last's
 * plus half the widths of the two, plus 1. each half is rounded down */
struct menu_sprite_row {
    const NJS_TEXANIM *tanim;
    Int first;
    Int count;
    const NJS_POINT3 *pos;
    menu_sprite_drawer draw;
};
/* draw row. the matching side also reads the width of the cell after its
 * last, which tanim must hold, unchecked */
void menu_sprite_row_draw(const struct menu_sprite_row *row);

/* menu_backdrop_draw's flag that colors the backdrop with menu_color, and
 * keeps the constant material's alpha */
#define MENU_BACKDROP_TINT 0x1
/* draw the backdrop at priority from eight textures of texlist, from first.
 * each draws as a sprite under MENU_SPRITE_ATTR, in the constant material's
 * color. the first four, 256 pixels a side, fill the screen's left in two
 * columns. the last four, 128 pixels a side, fill one column at its right.
 * flags takes MENU_BACKDROP_TINT */
void menu_backdrop_draw(Int first, NJS_TEXLIST *texlist, Float priority,
                        Int flags);

#endif
