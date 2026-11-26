/* the spinning hammer, HAMMER2 in Radical Highway's object list: a drum above
 * a bar. the drum spins about its own x axis. both are solid */
#ifndef STG14_SPINNING_HAMMER_H
#define STG14_SPINNING_HAMMER_H
#include <ninja.h>

struct task;

/* the hammer's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *spinning_hammer_texlists[];

/* the object list's step for HAMMER2. unless placement_try_leave(task) returns
 * TRUE, it gives task the hammer's own step, draw and destructor. it also puts
 * copies of the drum's collision object and the bar's in the collision world.
 * world_pool_alloc lends each copy. task->motion keeps the drum's, and
 * task->extra the bar's. where the pool has none to lend, spinning_hammer_init
 * writes through null.
 *
 * task needs a work and TASK_FORCE's records. the set file gives the hammer
 * these settings in the work.
 * - the low half of ang.x: the angle the drum turns each step. the drum's
 *   turn is that angle times game_state.steps, so drums of the same angle
 *   keep in step. both TASK_FORCE records' turn.x take ang.x whole, which
 *   turns a riding player with the drum.
 * - ang.y: the hammer's turn about y.
 * - scl.x: the scale of the drum and the bar along x, less 1.
 * - scl.y: their scale along y and z, less 1. the bar's along z is twice
 *   that.
 * - scl.z: 0 to draw the drum's shadow on the plane 0.1 above pos.
 *
 * the drum's axis lies 25 * (scl.y + 1) + 5 above pos. the bar's center lies
 * 5 * (scl.y + 1) + 2.5 above pos. each step plays sound 4 of the stage's list
 * at pos, keyed by the work. the world collects the copies only while a
 * player is near. player_near tests for one within the hammer's radius plus 10
 * of the drum's middle. the radius is the length of (50 * (scl.x + 1),
 * 25 * (scl.y + 1)). the drum draws with a glow that pulses every 64 steps */
void spinning_hammer_init(struct task *task);

#endif
