/* PLANT4 in Dry Lagoon's object list: the plant of stg18/plant.h over models
 * of its own */
#ifndef STG18_PLANT4_H
#define STG18_PLANT4_H
#include <ninja.h>

struct task;

/* The model drawn at each of the 49 sway frames. */
extern NJS_CNK_MODEL *plant4_models[49];

/* this plant's texture lists for texture_load_shared, a frame's at its
 * index, ended by null */
extern NJS_TEXLIST *plant4_texlists[];

/* do what plant_init does, for PLANT4's plant */
void plant4_init(struct task *task);

#endif
