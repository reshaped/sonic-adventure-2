/* LEAF in Dry Lagoon's object list: the plant of stg18/sasa.h over models
 * of its own */
#ifndef STG18_LEAF_H
#define STG18_LEAF_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* this plant's texture lists for texture_load_shared, a kind's at its
 * index, ended by null */
extern NJS_TEXLIST *leaf_texlists[];

/* do what sasa_build does, for this plant's models */
Uint8 *leaf_build(Uint8 *buf);

/* do what sasa_init does, for LEAF's plant. the plant's inside is where
 * player 0 starts its sway. the inside reaches 6 times the scale above its
 * pos. across x and z, it lies within 7 times the scale. the call also sets
 * task's unk_3c to 0, the value task_create gives it */
void leaf_init(struct task *task);

#endif
