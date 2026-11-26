/* the waterfall, TAKI in White Jungle's object list: a model of two parts.
 * the first part's texture scrolls along v */
#ifndef STG04_TAKI_H
#define STG04_TAKI_H
#include <ninja.h>

struct task;

/* the waterfall's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *taki_texlists[];

/* the object list's step for TAKI. the waterfall leaves at its placement's
 * range. task needs a work, whose pos places the model and whose ang.y turns
 * it. the first part's texture scrolls along v by scl.x's whole part for each
 * count of game_state.steps */
void taki_init(struct task *task);

#endif
