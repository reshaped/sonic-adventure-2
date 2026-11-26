/* Death Chamber's machinery, MECHA in the stage's object list: a curved or
 * straight model */
#ifndef STG25_MECHA_H
#define STG25_MECHA_H
#include <ninja.h>

struct task;

/* the machinery's texture lists for texture_load_shared, a kind's at its
 * index, ended by null */
extern NJS_TEXLIST *chamber_mecha_texlists[];

/* the object list's step for MECHA. task needs a work. its pos places the
 * model, and its ang.y turns it. the whole part of |scl.z| modulo 2 picks its
 * kind: 0 is curved, and 1 straight */
void chamber_mecha_init(struct task *task);

#endif
