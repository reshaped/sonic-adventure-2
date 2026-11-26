/* the spinner, SPINNER in Death Chamber and Egg Quarters. it is the shaft of
 * shared/shaft.h over a model, a motion and textures of its own */
#ifndef SHARED_SPINNER_H
#define SHARED_SPINNER_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST spinner_texlist;

/* the spinner's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *spinner_texlists[];

/* the object list's step for SPINNER. it does what shaft_init does,
 * and the work's fields mean what they mean there */
void spinner_init(struct task *task);

extern NJS_CNK_OBJECT spinner_object;
extern NJS_MOTION spinner_motion[1];

#endif
