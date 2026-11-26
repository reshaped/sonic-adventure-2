/* trail styles. shared/common_object.h declares the calls that keep and draw
 * a trail, and the main program reaches them through stage_hooks */
#ifndef DRAW_TRAIL_H
#define DRAW_TRAIL_H
#include <nj_render_types.h>
#include <sg_xpt.h>

/* the bits of trail_style's flags */
#define TRAIL_TEXTURED 0x1 /* draw with the style's texture */
/* the bits that pick how width and color run along the trail. a point's place
 * runs from 0 at the oldest point to 1 at the newest. a point's width and color
 * lie a share of the way from the old to the new. the share is the place.
 * TRAIL_SQUARE squares it, and TRAIL_SINE makes it the sine of the place times
 * a quarter turn. both bits together count as neither */
#define TRAIL_CURVE 0x6
#define TRAIL_SQUARE 0x2
#define TRAIL_SINE 0x4
/* keep a point only for odd values of game_state.steps */
#define TRAIL_ODD_STEPS 0x8
struct trail_style {
    Sint32 flags;
    Float new_width; /* the width at the newest point */
    Float old_width; /* the width the trail runs toward at its oldest point */
    Uint32 new_color; /* the packed color at the newest point */
    Uint32 old_color; /* the color the trail runs toward at its oldest point */
    /* the texture list that holds texture, or NULL. trail_draw never reads
     * it: the caller sets the list as current before it draws a textured
     * trail */
    NJS_TEXLIST *texlist;
    /* with TRAIL_TEXTURED, the number of the style's texture in the current
     * texture list */
    Uint32 texture;
};

#endif
