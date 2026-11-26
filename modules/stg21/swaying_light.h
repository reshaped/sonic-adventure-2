/* LIGHT in Sand Ocean's object list: a beam of light that sways */
#ifndef STG21_SWAYING_LIGHT_H
#define STG21_SWAYING_LIGHT_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST swaying_light_texlist;
/* the light's texture list for texture_load_shared, then null */
extern NJS_TEXLIST *swaying_light_texlists[];

/* the object list's step for LIGHT. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the light's step, draw and
 * destructor.
 *
 * task needs a work, whose pos places the light and whose ang.y turns it.
 * each later step tests the range as the first did. scl.x + 1 scales the
 * light. its beam sways with game_state.steps, from a phase the init draws at
 * random */
void swaying_light_init(struct task *task);
extern NJS_CNK_MODEL swaying_light_model;

#endif
