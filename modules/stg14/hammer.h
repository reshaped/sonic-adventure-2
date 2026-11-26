/* the hammer, HAMMER in Radical Highway's object list: a solid column the
 * players can ride. it strokes along y to its pos and back, and rests at each
 * end */
#ifndef STG14_HAMMER_H
#define STG14_HAMMER_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the hammer's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *highway_hammer_texlists[];

/* the object list's step for HAMMER. unless placement_try_leave(task) returns
 * TRUE, it gives task the hammer's own step, draw and destructor. it also puts
 * a copy of the hammer's collision object in the collision world.
 * world_pool_alloc lends the copy, which task->motion keeps. where the pool
 * has none to lend, highway_hammer_init writes through null.
 *
 * task needs a work and TASK_FORCE's records. the set file gives the hammer
 * these settings in the work.
 * - the low byte of ang.x: the steps it rests at each end.
 * - the high byte of ang.x: the way it strokes. with 0 it strokes up to pos
 *   from scl.z below it. with any other value it strokes down to pos from
 *   scl.z above it.
 * - the high byte of ang.y: its turn about y.
 * - the low byte of ang.y: its phase, in 256ths of its cycle.
 * - the low half of ang.z: the angle its stroke turns each step. a stroke
 *   lasts 0x8000 divided by that angle, in whole steps. the angle must not
 *   be 0, unchecked.
 * - scl.x and scl.y: its scale across and up, each less 1.
 *
 * in a cycle the hammer strokes to pos, rests there, strokes back and rests
 * at the far end. a stroke eases out and in, along half a cosine wave. each
 * step works out the hammer's place in its cycle from game_state.steps. so
 * hammers of the same settings keep in step. while game_state.time_stopped
 * isn't 0, the hammer stands where it is.
 *
 * each step moves the copy with the hammer. it sets speed.y of both TASK_FORCE
 * records to how far the copy moved, which carries a riding player. the world
 * collects the copy only while player_near finds a player within the hammer's
 * radius plus 10. the radius is the length of (25, 50 * (scl.y + 1)). the
 * hammer draws with a glow that pulses every 64 steps */
void highway_hammer_init(struct task *task);
/* the WORLD_SIZE_CLASS flag of an object of radius, by the limits
 * WORLD_SIZE_CLASS_1_MAX to WORLD_SIZE_CLASS_3_MAX */
Uint32 highway_hammer_size_class(Float radius);

#endif
