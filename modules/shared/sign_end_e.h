/* SIGNENDE in the object lists of stg30 and Final Chase: a model whose
 * texture scrolls */
#ifndef SHARED_SIGN_END_E_H
#define SHARED_SIGN_END_E_H
#include <ninja.h>

struct task;

/* the object's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *sign_end_e_texlists[];

/* the object list's step for SIGNENDE. task needs a work, whose pos places
 * the model and whose ang turns it. a scl.x of 1 or more draws the model
 * with the second texture list */
void sign_end_e_init(struct task *task);

#endif
