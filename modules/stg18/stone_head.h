/* the stone head, STONEHEAD in Dry Lagoon's object list: a head that a
 * player pushes along a rail */
#ifndef STG18_STONE_HEAD_H
#define STG18_STONE_HEAD_H
#include <ninja.h>

struct task;

/* the head's and the rail's texture lists for texture_load_shared, ended by
 * null */
extern NJS_TEXLIST *lagoon_stone_head_texlists[];

/* the object list's step for STONEHEAD, which makes task the rail. unless
 * placement_try_leave(task) returns TRUE, it gives task->extra a record of
 * the rail's ends from the Shinobi heap. it gives task the head as its child.
 * where the heap can't give either, it destroys task. otherwise it gives task
 * the rail's own step, draw and destructor.
 *
 * task needs a work and a placement. the work's pos and ang.y place the rail,
 * which runs along the z axis ang.y turns. scl.x + 1 scales the head, and the
 * rail but for its height. scl.y + 1 also scales the rail's length. a place on
 * the rail runs from 0 at one end to 255 at the other. the head starts at the
 * place the placement's state keeps. where the state is 0, it starts at the low
 * byte of ang.z. the rail's destructor keeps the head's place as the state,
 * held to 1 to 255. task's child must then still be the head, unchecked.
 *
 * the head takes a collision record of a cylinder and a box, of kind
 * COLLISION_OBJECT. the heap must have room for it, unchecked. the head's
 * work takes FALL_KIND_11 as its id and FALL_OFFERABLE among its flags.
 * fall_step_medium moves it, held on the rail. it moves player 0 while
 * player 0 holds the head and a player touches the head. player 0 then stands 1
 * ahead of the head's pos, turned half round from the head. ahead is z, turned
 * by the head's ang.y. player 0's record must not be null then, unchecked */
void lagoon_stone_head_init(struct task *task);

#endif
