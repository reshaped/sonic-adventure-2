/* PLANT2 in Dry Lagoon's object list: a plant that sways without end */
#ifndef STG18_PLANT2_H
#define STG18_PLANT2_H
#include <ninja.h>

struct task;

/* the plant's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *plant2_texlists[];

/* the object list's step for PLANT2. unless placement_try_leave(task) returns
 * TRUE, it gives task the plant's own step, draw and destructor.
 *
 * task needs a work. its pos places the plant, and scl.x + 1 scales it. its
 * ang turns it about z, then x, then y. the sway runs |scl.y + 1| frames of the
 * plant's shape motion a step, round and round. scl.z keeps the sway's frame,
 * which the call sets to 0. the plant draws under DRAW_CONTROL_SHADOW, so the
 * modifier volumes shadow it */
void plant2_init(struct task *task);

#endif
