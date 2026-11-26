/* the big jump, BIGJUMP in a stage's object list: a platform that launches
 * a player standing on it */
#ifndef SHARED_BIG_JUMP_H
#define SHARED_BIG_JUMP_H

#include <ninja.h>

struct task;

/* the texture lists of big_jump_model and big_jump_anim_model */
extern NJS_TEXLIST big_jump_texlist;
extern NJS_TEXLIST big_jump_anim_texlist;

/* the object list's step for BIGJUMP. task needs a work, whose pos places the
 * big jump and whose ang turns it. the first step adds a copy of the big jump's
 * shape to the collision world. the copy takes WORLD_SOLID and WORLD_UPRIGHT.
 * world_pool_alloc lends the copy, which task->extra keeps. the pool must have
 * one to lend, unchecked. the big jump keeps its counts of launches in
 * task->motion, which must hold no record. the destructor clears it, and takes
 * the copy out of the world and back to the pool.
 *
 * a player is near within the copy's model radius plus 32 of pos. the copy
 * collides only while player 0 or 1 is near. while one is, a player who stands
 * on the big jump is launched. the launch repeats on each of the next 20 steps
 * while one still is. the first launch and the one after it each vibrate the
 * player's pad. a launch's speed is scl.x forward and scl.z + 3.2 up, in the
 * player's own axes. it turns the player to face ang.y plus a half turn. it
 * gives a stick_lock of |ceilf(scl.y)|. a negative ceilf(scl.y) launches by
 * player_launch_speed, and any other by fun_8c0492d0 */
void big_jump_step(struct task *task);

#endif
