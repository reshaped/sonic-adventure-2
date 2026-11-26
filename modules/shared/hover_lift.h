/* the hover lift, SHOUKOU, SHOUKOUK and BOXSHOUKOU in a stage's object
 * list. it is a platform of one of three shapes. it rides up and down along y,
 * and rests after each ride */
#ifndef SHARED_HOVER_LIFT_H
#define SHARED_HOVER_LIFT_H
#include <ninja.h>

struct task;

/* the collision objects of SHOUKOU's and SHOUKOUK's shapes. White Jungle's
 * solid scenery takes them too */
extern NJS_OBJECT hover_lift_collision_0;
extern NJS_OBJECT hover_lift_collision_1;

extern NJS_TEXLIST hover_lift_texlist_0;
extern NJS_TEXLIST hover_lift_texlist_1;

/* the texture lists of SHOUKOU's, SHOUKOUK's and BOXSHOUKOU's shapes for
 * texture_load_shared, ended by null */
extern NJS_TEXLIST *hover_lift_texlists[];

/* the object list's steps for SHOUKOU, SHOUKOUK and BOXSHOUKOU. unless
 * placement_try_leave(task) returns TRUE, each gives task the lift's own
 * step, draw and destructor. task needs a work and TASK_FORCE's records. it
 * needs no record in extra, where the lift keeps its height.
 *
 * the lift puts a copy of its shape's collision object in the collision
 * world. task->motion keeps the copy, which world_pool_alloc lends. where the
 * pool has none to lend, the call writes through null.
 *
 * the work holds the lift's entry in the set file:
 *
 * - ang.x: the ride's phase, as below;
 * - ang.y: the lift's turn about y;
 * - ang.z: the angle the ride turns a step, by its low half. it must not be
 *   0, unchecked: the call divides by it. where scl.x gives no rest, it must
 *   not pass 0x8000 either, unchecked. the cycle is then 0 steps long, and
 *   the call divides by its length;
 * - scl.x: the steps of each rest, by the low byte of its whole part;
 * - scl.y: the lift's scale along y, less 1;
 * - scl.z: the rise of the ride. where it lies within 0.001 of 0, the copy
 *   goes in the world without WORLD_MOVING.
 *
 * in each cycle, the ride's angle holds at 0 for a rest, then turns to a half
 * turn. it holds there for a rest, then turns on to a full turn. at an angle
 * a, the lift lies scl.z * (1 + cos(ang.x + a)) / 2 above pos. the lift takes
 * its place in the cycle from game_state.steps as it starts. lifts of the
 * same settings thus keep in step.
 *
 * each step moves the copy with the lift. it sets the speed.y of both
 * TASK_FORCE records to how far the copy moved. it sets WORLD_ADDED in the
 * work's flags while a player is near, and clears it otherwise. a player within
 * the copy's radius plus 32 of the copy's pos, as player_near measures, is
 * near. that radius is the model's, times scl.y + 1 where that is larger */
void hover_lift_init(struct task *task);
void hover_lift_k_init(struct task *task);
void hover_lift_box_init(struct task *task);

#endif
