/* the laser barrier, LAZER in a stage's object list: rows of beams that hurt a
 * player on contact. Cannon's Core, Crazy Gadget, Eternal Engine and Lost
 * Colony place it */
#ifndef SHARED_CORE_LASER_H
#define SHARED_CORE_LASER_H

#include <ninja.h>

struct task;

extern NJS_TEXLIST core_laser_texlist;
/* the barrier's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *core_laser_texlists[];

/* the object list's step for LAZER. it does nothing where
 * placement_try_leave(task) returns TRUE. task needs a work, and the barrier
 * keeps a record of its own in task->motion.
 *
 * the work's pos is the barrier's middle. njRotateY by ang.y, then njRotateZ by
 * ang.z, turns it. scl.z cut to an integer is its count of rows, or 1 where
 * that is less. the rows stand scl.y + 10 apart. each row's posts stand scl.x
 * either side of the middle. where bit 0 of ang.x is set, one solid box that
 * hurts stands for the beams. otherwise each player has a contact that hurts,
 * at the point of the barrier's face nearest them. the box and the contacts
 * collide only while game_state.time_stopped is 0 */
void core_laser_init(struct task *task);

#endif
