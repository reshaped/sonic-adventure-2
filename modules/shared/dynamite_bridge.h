/* BRIDGE and BRIDGE_DYNA, Eternal Engine's and Lost Colony's bridges.
 * BRIDGE_DYNA falls once its dynamite has blown */
#ifndef SHARED_DYNAMITE_BRIDGE_H
#define SHARED_DYNAMITE_BRIDGE_H

#include <ninja.h>

struct task;

extern NJS_TEXLIST dynamite_bridge_texlist;
extern NJS_CNK_MODEL bridge_model[1];
/* only Lost Colony defines it. its bridges draw it after bridge_model, with fog
 * off */
extern NJS_CNK_MODEL bridge_light_model;
extern NJS_OBJECT dynamite_bridge_collision;

/* the object list's steps for BRIDGE and BRIDGE_DYNA. each does nothing where
 * placement_try_leave(task) returns TRUE. task needs a work, and the bridge
 * keeps a record of its own in task->motion. a copy of
 * dynamite_bridge_collision's root goes into the world as a solid. each of
 * the root's two children becomes a solid child of task. a Lost Colony bridge
 * carries a light volume of stg27/light.h, and the solids are the volume's
 * children.
 *
 * BRIDGE_DYNA bursts and falls once dynamite_count finds no charge whose id is
 * the low byte of ang.x. the root's copy leaves the world's collision as the
 * bridge falls */
void fixed_bridge_init(struct task *task);
void dynamite_bridge_init(struct task *task);

#endif
