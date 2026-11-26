/* PLANT in Dry Lagoon's object list: a long water plant that sways a frame a
 * step */
#ifndef STG18_PLANT_H
#define STG18_PLANT_H
#include <ninja.h>

struct task;

/* The model drawn at each of the 49 sway frames. */
extern NJS_CNK_MODEL *plant_models[49];

/* the plant's texture lists for texture_load_shared, a frame's at its index,
 * ended by null */
extern NJS_TEXLIST *plant_texlists[];

/* the object list's step for PLANT. unless placement_try_leave(task) returns
 * TRUE, it gives task the plant's own step, draw and destructor.
 *
 * task needs a work. its pos places the plant, and scl.x + 1 scales it. its
 * ang turns it about y, then x. the plant runs through its 49 frames, a frame
 * for each step game_state.steps counts. the call picks the frame it starts
 * from at random */
void plant_init(struct task *task);

#endif
