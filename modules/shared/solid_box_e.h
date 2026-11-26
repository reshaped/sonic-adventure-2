/* the solid box of shared/solid_box.h, compiled a second time over textures
 * whose names end in e. it is SOLIDBOX in the object lists of stg25, stg26
 * and stg28. stg23's list names SOLIDBOX twice, once for each box */
#ifndef SHARED_SOLID_BOX_E_H
#define SHARED_SOLID_BOX_E_H
#include <ninja.h>

struct task;

/* this box's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *solid_box_e_texlists[];

/* do what solid_box_init does, for a box that draws in these textures */
void solid_box_e_init(struct task *task);

#endif
