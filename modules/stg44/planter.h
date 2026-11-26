/* the planter, PLANTER in Mad Space's object list: a decoration that turns */
#ifndef STG44_PLANTER_H
#define STG44_PLANTER_H
#include <ninja.h>

struct task;
extern NJS_TEXLIST planter_texlist;
/* the planter's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *planter_textures[];

/* the object list's step for PLANTER. it does nothing where task leaves at
 * its placement's range. otherwise it gives task the planter's step, draw and
 * destructor. each later step tests the range as the first did.
 *
 * task needs a work, whose pos places the planter and whose three angles turn
 * it. the planter draws planter_object's child, the one node with a model,
 * which spins about y as game_state.steps counts */
void planter_init(struct task *task);
#endif
