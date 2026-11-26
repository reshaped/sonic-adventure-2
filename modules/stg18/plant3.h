/* PLANT3 in Dry Lagoon's object list: the plant of stg18/plant2.h over a
 * model of its own */
#ifndef STG18_PLANT3_H
#define STG18_PLANT3_H
#include <ninja.h>

struct task;

/* this plant's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *plant3_texlists[];

/* do what plant2_init does, for PLANT3's plant */
void plant3_init(struct task *task);

#endif
