/* the entrance menu's rows, which slide in from the left, stay shown and
 * slide back out. its widgets share these modes and macros */
#ifndef CHAOSTGENTRANCE_SLIDE_H
#define CHAOSTGENTRANCE_SLIDE_H
#include <ninja.h>
#include "chaostgentrance/entrance.h"
#include "chaostgentrance/graphics.h"
#include "chaostgentrance/menu.h"
#include "chaostgentrance/rows.h"
#include "draw/lines.h"
#include "system/task.h"

enum { ROW_WAIT, ROW_ENTER, ROW_SHOWN, ROW_WAIT_TO_LEAVE, ROW_LEAVE };
#define ROW(task) ((struct entrance_row *)(task)->work)
#define ROW_MODE(row, next)                                                    \
    {                                                                          \
        (row)->mode = (next);                                                  \
        (row)->phase = 0;                                                      \
    }

#define ROW_WAIT_FOR(row, next)                                                \
    if ((row)->age++ > (row)->delay) {                                         \
        (row)->age = 0;                                                        \
        ROW_MODE(row, next);                                                   \
    }
#define ROW_SLIDE_IN(row, on_arrival)                                          \
    (row)->slide *= 0.9F;                                                      \
    (row)->slide += 1.0F;                                                      \
    if ((row)->slide > 0.0F) {                                                 \
        (row)->slide = 0.0F;                                                   \
        ROW_MODE(row, ROW_SHOWN);                                              \
        on_arrival;                                                            \
    }
#define ROW_SLIDE_OUT(task, row)                                               \
    (row)->slide -= 1.0F;                                                      \
    (row)->slide *= 1.1F;                                                      \
    if ((row)->slide < -380.0F) {                                              \
        (row)->slide = -380.0F;                                                \
        task_kill(task);                                                       \
    }
/* a row that isn't shown holds the menu, by setting entrance_menu->wait. a
 * select or back request sends each row out after a delay. the delay grows
 * by column and by row, so the rows leave in turn */
#define ROW_CHECK_REQUEST(widget)                                              \
    switch ((widget)->mode) {                                                  \
    case ROW_SHOWN:                                                            \
        break;                                                                 \
    default:                                                                   \
        entrance_menu->wait = TRUE;                                            \
        break;                                                                 \
    }                                                                          \
    if ((widget)->mode < ROW_WAIT_TO_LEAVE) {                                  \
        if (entrance_menu->request == ENTRANCE_MENU_SELECT ||                  \
            entrance_menu->request == ENTRANCE_MENU_BACK) {                    \
            (widget)->delay =                                                  \
                ((widget)->column * 5) + ((widget)->row * 2) + 5;              \
            ROW_MODE(widget, ROW_WAIT_TO_LEAVE);                               \
        }                                                                      \
    }

#define START_ROW_SPRITE(sprite, row)                                          \
    (sprite).p.x = (row)->x + (row)->slide;                                    \
    (sprite).p.y = (row)->y;                                                   \
    (sprite).p.z = 0.0F;                                                       \
    (sprite).ang = 0;                                                          \
    (sprite).sx = 1.0F;                                                        \
    (sprite).sy = 1.0F;                                                        \
    (sprite).tlist = &entrance_common_texlist;                                 \
    (sprite).tanim = (NJS_TEXANIM *)entrance_common_cells;                     \
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);        \
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA)
#define DRAW(cell, depth) entrance_draw_cell(&sprite, (cell), (depth))
#define NAME_CASE(index)                                                       \
    case (index):                                                              \
        row->label = entrance_label_ids[(index) + 11];                         \
        break

#endif
