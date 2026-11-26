#ifndef STG22_GLOW_H
#define STG22_GLOW_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/surface.h"

/* the glow under player 0, at the background task's unk_3c, or null.
 * gadget_background_step allocates it with syCalloc while it is null, and
 * gadget_background_destroy frees it. pos is player 0's center on the last
 * step that laid the glow. sides[0] is the ground there. its found holds the
 * SURFACE_ side that gravity points to, not a Bool. its pos is the ground's
 * coordinate on that side's axis. its ang_x and ang_z are the glow's tilt */
#define GADGET_GLOW(task) ((struct surface_probe *)(task)->unk_3c.p)

#define GADGET_KILL_VOLUMES 38

/* a box in which gravity.c makes player 0 fall out. the object's pos, ang
 * and scl.x and scl.z place a unit square. the box reaches 30 below it. no
 * code reads the model or reserved, which is 0 in every volume */
struct gadget_kill_volume {
    NJS_OBJECT *object;
    Sint32 reserved[6];
};
extern struct gadget_kill_volume gadget_kill_volumes[GADGET_KILL_VOLUMES];
extern Float gadget_glow_reach;

#endif
