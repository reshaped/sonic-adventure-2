/* Mad Space's fireballs, FIREBALL and FB2 in its object list */
#ifndef STG44_FIREBALL_H
#define STG44_FIREBALL_H

#include <ninja.h>
struct task;

/* the object list's step for FIREBALL. task needs a work, whose pos places
 * the fireball and whose ang turns it. the fireball sheds sparks, and hurts a
 * player it touches. it plays sound 7 of the stage's sound list while a player
 * lies within 500. the touch breaks it. so does the 121st step after a player
 * comes within scl.x + 30 of pos. the break throws twenty pieces, plays sound
 * 9 for each, and ends the task. where the heap can't give the fireball's
 * collision record, the break reads the shape's radius through null */
void mad_space_fireball_init(struct task *task);
/* the object list's step for FB2. it does nothing where task leaves at its
 * placement's range. otherwise it gives task a child, the rock, unless task
 * has one. task needs a work. its pos places the path, its ang turns the path,
 * and its scl.y + 1 scales it.
 *
 * the rock runs round the path scl.x + 0.5 a step, before the path's scale. it
 * spins and sheds sparks. it plays sound 7 while a player lies within 500. it
 * hurts a player it touches unless scl.z is 1 or more. where the heap can't
 * give the rock its cursor, the rock tries again at its next step */
void mad_space_orbit_init(struct task *task);
/* the fireballs' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *mad_space_fireball_textures[];

#endif
