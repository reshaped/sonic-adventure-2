/* the pumpkin walls, PUMSASI and PUMWALL in Pumpkin Hill's object list. the
 * players stand on them and run against them */
#ifndef STG05_PUMPKIN_WALL_H
#define STG05_PUMPKIN_WALL_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of PUMWALL's wall and of
 * PUMSASI's, ended by null */
extern NJS_TEXLIST *pumpkin_wall_texlists[];

/* the object list's steps for PUMSASI and PUMWALL. each does nothing where
 * task leaves at its placement's range. otherwise the call gives task the
 * wall's step, draw and destructor. it gives task a record in task->motion from
 * syCalloc, which the destructor frees. where syCalloc fails, the call does
 * nothing more, and the next step tries again.
 *
 * task needs a work. its pos and ang place and turn the wall. each later step
 * tests the range as the first did. the players stand on a copy of the wall's
 * collision object in the collision world. the copy collides only while a
 * player lies within its model's radius plus 15 of the model's center.
 * PUMSASI's wall draws under DRAW_CONTROL_SHADOW, and PUMWALL's without it. a
 * player who runs or lands on PUMSASI's wall raises puffs from it.
 *
 * where task has no child, pumsasi_init also gives it one, at task's pos and
 * angles. the child puts a second collision object in the world, and draws
 * nothing. it leaves only with task. the heap must have room for the child's
 * record, unchecked */
void pumsasi_init(struct task *task);
void pumpkin_wall_init(struct task *task);

#endif
