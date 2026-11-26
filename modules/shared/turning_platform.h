/* the turning platform, KAITEN in the object lists of both Sand Ocean stages.
 * it turns about y, and carries the players round with it. a fire burns at its
 * middle */
#ifndef SHARED_TURNING_PLATFORM_H
#define SHARED_TURNING_PLATFORM_H
#include <ninja.h>

struct task;

/* the platform's texture list, and the array of it ended by null that a
 * stage's texture groups hold */
extern NJS_TEXLIST turning_platform_texlist;
extern NJS_TEXLIST *turning_platform_texlists[];
/* a texture list of the fire's first frame. the draw takes the fire's
 * frames from ocean_fire_texlist */
extern NJS_TEXLIST turning_platform_flame_texlist;

/* the object list's step for KAITEN. unless placement_try_leave(task) returns
 * TRUE, it creates a child task. where task_create_child fails, the call
 * stays task's step, and tries again at the next. the platform and the child
 * each put a copy of a collision model in the collision world. each task's
 * extra keeps its copy, which world_pool_alloc lends. the pool must have a
 * copy to lend for each, unchecked. the child adds its copy with
 * GROUND_CLIMB_REVERSED among its flags. in Sand Ocean, task also gets a
 * collision record for the fire, which hurts players.
 *
 * task needs a work and TASK_FORCE's records. pos places the platform, and
 * scl.x + 1 scales it. the platform turns by ang.x a step, or by -ang.x where
 * bit 0 of ang.z is set. its angle is ang.y plus that turn times
 * game_state.steps, so it doesn't depend on when the task started. the force
 * records turn a riding player with the platform, and move the player round
 * pos by the platform's turn a step. each record's move is the one that turn
 * gives player 0's place, but in the two-player stage the second record's is
 * player 1's.
 * player_work[0] must not be null, nor player_work[1] in the two-player
 * stage, unchecked.
 *
 * the platform's copies collide only while a player lies within
 * (scl.x + 1) * 150 + 15 of pos. the platform plays the stage's sound 1 from
 * (scl.x + 1) * 10 above pos. it plays sound 2 there while player 0 lies within
 * 100 of that point. in the two-player stage, either player will do. the fire
 * shows the frames of ocean_fire_texlist in turn, one a step */
void turning_platform_init(struct task *task);
/* the platform's collision model, its child's, and the models the draw draws.
 * the stages' art sources define them */
extern NJS_OBJECT turning_platform_child_object;
/* @hack type: the fire's model declared as an array of one, where the model
 * alone would do */
extern NJS_CNK_MODEL turning_platform_fire_model[1];
extern NJS_CNK_MODEL turning_platform_model;
extern NJS_OBJECT turning_platform_object;

#endif
