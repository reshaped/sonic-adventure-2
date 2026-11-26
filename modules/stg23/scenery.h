#ifndef STG23_SCENERY_H
#define STG23_SCENERY_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST *base_pillar_texlists[];
extern NJS_TEXLIST *base_plant_texlists[];
extern NJS_TEXLIST *base_mat_texlists[];

/* PILLAR scales its model and its collision box by scl.x + 1 */
void base_pillar_init(struct task *task);
/* WEED draws the small plant or the large one, as |scl.z|'s whole part is
 * even or odd. it scales by scl.x + 1 */
void base_plant_init(struct task *task);
/* MAT scales in x and z by scl.x + 1 */
void base_mat_init(struct task *task);
extern NJS_CNK_MODEL base_pillar_model;
extern NJS_CNK_OBJECT base_small_plant;
extern NJS_CNK_OBJECT base_large_plant;
extern NJS_CNK_MODEL base_mat_model;

#endif
