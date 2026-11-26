/* STAFF in Hidden Base's object list: a spinning staff that breaks */
#ifndef STG23_STAFF_H
#define STG23_STAFF_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST *staff_texlists[];

/* the object list's step for STAFF. task needs a work. the staff turns about y
 * by ang.z a step. a sway adds or takes up to a fifth of that, and runs a
 * cycle in 256 steps. the integer part of |scl.z|, mod 2, picks its bright
 * look or its dark. a hit on its weak point breaks its head into three
 * pieces. the placement's state keeps the break: the staff starts again
 * broken, with no weak point */
void staff_init(struct task *task);

#endif
