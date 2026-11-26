/* the race banners' cells among the HUD's, and the macros their draws
 * share */
#ifndef CHAOSTGRACE_BANNER_DRAW_H
#define CHAOSTGRACE_BANNER_DRAW_H
#include <ninja.h>
#include "chaostgrace/hud_art.h"
#include "draw/lines.h"
#include "system/task.h"

extern NJS_TEXLIST race_hud_texlist;
extern NJS_POINT2 race_hud_positions[];

enum race_banner_cell {
    BANNER_START_LEFT = 9,
    BANNER_START_RIGHT,
    BANNER_GOAL,
    BANNER_READY_LEFT,
    BANNER_READY_RIGHT,
    BANNER_WIN_LEFT,
    BANNER_WIN_RIGHT,
    BANNER_RETRY
};

/* @hack spelling: the sprite set up by macros in each draw, where a helper
 * taking its address would do */
#define BANNER_POSITION(sprite, center_y)                                      \
    do {                                                                       \
        (sprite).p.x = 320.0F;                                                 \
        (sprite).p.y = (center_y);                                             \
        (sprite).p.z = 0.0F;                                                   \
        (sprite).ang = 0;                                                      \
    } while (0)

#define BANNER_MATERIAL(sprite, opacity)                                       \
    do {                                                                       \
        njFogDisable();                                                        \
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);    \
        njColorBlendingMode(NJD_DESTINATION_COLOR,                             \
                            NJD_COLOR_BLENDING_INVSRCALPHA);                   \
        constant_material_set((opacity), 1.0F, 1.0F, 1.0F);                    \
        (sprite).tlist = &race_hud_texlist;                                    \
        (sprite).tanim = RACE_HUD_CELLS;                                       \
    } while (0)

#define BANNER_DRAW_CELL(sprite, cell, depth)                                  \
    njDrawSprite2D(&(sprite), (cell), (depth), (sprite).tanim[(cell)].attr)

#endif
