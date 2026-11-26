/* Crazy Gadget's gravity switches: GSWITCH2's handle, and the switch of the
 * second kind named SWDRNGC */
#ifndef STG22_GRAVITY_SWITCH_H
#define STG22_GRAVITY_SWITCH_H

#include <ninja.h>

struct task;

extern NJS_TEXLIST gadget_gravity_switch_texlist;
extern NJS_TEXLIST *gadget_gravity_switch_texlists[];

/* the object list's init for the second kind named SWDRNGC. it does nothing
 * where task leaves at its placement's range. otherwise it gives task the
 * switch's step, draw and destructor, and a collision record of kind
 * COLLISION_OBJECT. the switch draws nothing.
 *
 * task needs a work. pos places the switch, a sphere of radius 23 whose
 * center lies 15 above pos. a player's touch has world_tilt_gravity tilt
 * gravity by ang.x and ang.z. the collision checks' up axis follows. the
 * switch then passes over touches for 30 steps */
void gadget_range_switch_init(struct task *task);
/* the object list's init for GSWITCH2, a handle that turns gravity. it does
 * nothing where task leaves at its placement's range. where the heap can't
 * give the solid's copy, it does nothing, and the next step tries again.
 * otherwise it gives task the switch's step, draw and destructor, and a
 * collision record of kind COLLISION_OBJECT. it puts the switch's solid in
 * the world.
 *
 * task needs a work. pos places the switch, and ang turns it. the low 2 bits
 * of (int)scl.x pick the switch's way: its own +y, -y, +z or -z. a player 0
 * that grabs the handle holds on for 40 steps. gravity then turns to point
 * against the way where it pointed along it, and along it otherwise. the
 * collision checks' up axis follows */
void gadget_gravity_switch_init(struct task *task);

#endif
