/* the arrow, ARROW in Final Chase's and Cosmic Wall's object lists. its two
 * parts blink in turn between a bright color and a dim one */
#ifndef SHARED_ARROW_H
#define SHARED_ARROW_H
#include <ninja.h>

struct task;

/* the two models' objects. an object's child is the first part, and the
 * child's sibling the second */
extern NJS_CNK_OBJECT arrow_object;
extern NJS_CNK_OBJECT arrow_2_object;

/* the arrows' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *arrow_texlists[];
extern NJS_TEXLIST arrow_texlist;
extern NJS_TEXLIST arrow_2_texlist;

/* the object list's step for ARROW. task needs a work, whose pos places the
 * arrow and whose ang turns it. a scl.x of 1 or more picks the second of two
 * models */
void arrow_init(struct task *task);

#endif
