/* Big the Cat, BIG THE CAT in an object list: Big stands and plays one of
 * his actions */
#ifndef SHARED_BIG_THE_CAT_H
#define SHARED_BIG_THE_CAT_H

#include <ninja.h>

#include "enemy/enemy.h"

struct task;

/* the object list's step for BIG THE CAT. task needs a work, whose pos places
 * Big. ang.x turns him by njRotateZ, ang.z by njRotateX, and ang.y plus a
 * quarter turn by njRotateY. scl.x, rounded toward 0 into a Sint8, picks which
 * of his 24 actions he plays. a value outside 0 to 23 counts as 0. action 23
 * draws a compiled model without motion or shadow. that model turns about y by
 * whole 256ths of a turn. his action and his work's timer stand still while
 * game_state.time_stopped isn't 0.
 *
 * a step passes task to placement_kill in a two-player game. it does the same
 * where game_state.unk_53 is 0 or game_state.unk_5b isn't 0. once Big is set
 * up, a step returns at once where placement_try_leave(task) returns TRUE.
 *
 * the first step gives task's motion an enemy_work, and the heap must have room
 * for it, unchecked. where Big's textures aren't loaded, the step loads
 * E_BIGTEX. it makes a task at level 0 that keeps them loaded, and holds action
 * 23's compiled model. the heap must have room for that task too, unchecked.
 * where it has no room for the model, action 23's draw reads through null. the
 * load leaves Big's texture list current, light 2 applied, the fog on and
 * njSetDirectClip's clip set */
void big_step(struct task *task);

#define BIG_ACTION_COUNT 24

extern NJS_CNK_OBJECT big_object;
/* @hack type: the cheap model declared as an array of objects, where a
 * single object would do */
extern NJS_CNK_OBJECT big_cheap_object[1];
extern NJS_TEXLIST big_texlist;
extern struct enemy_action big_actions[BIG_ACTION_COUNT];

#endif
