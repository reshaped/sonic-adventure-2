/* the bomb hat, a task named TBombHatExec: a ring of the bombfire texture.
 * the ring grows about a point, and fades as it grows */
#ifndef SHARED_BOMB_HAT_H
#define SHARED_BOMB_HAT_H
#include <ninja.h>
#include <sg_xpt.h>

/* the ring's texture list, of bombfire alone */
extern NJS_TEXLIST bomb_hat_texlist;

/* draw the ring about the y axis of the current matrix. its outer edge lies
 * 0.9999 of radius from the axis. its inner edge lies 0.915 of radius from the
 * axis, and 0.029 of radius higher. fade blends the ring's color byte by byte,
 * as color_blend does. a fade of 0 gives opaque white, and 1 the faint yellow
 * 0x10ffff80.
 *
 * the call leaves bomb_hat_texlist as the current texture list. it writes the
 * colored ring over the shared workspace at WORK_AREA, of system/areas.h */
void bomb_hat_draw_ring(Float radius, Float fade);

#endif
