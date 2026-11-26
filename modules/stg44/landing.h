/* the landing, STAGE and STAGE_F in Mad Space's object list: a solid floor.
 * part of it raises dust under a player */
#ifndef STG44_LANDING_H
#define STG44_LANDING_H
#include <ninja.h>

struct task;
extern NJS_TEXLIST landing_texlist;
/* the landing's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *landing_textures[];

/* the object list's step for STAGE and for STAGE_F, which place the same
 * landing. solid_object_init_flags makes task a solid object of
 * landing_collision, under OBJECT_AS_LAND and OBJECT_MAY_LEAVE, that draws
 * landing_model's model. unless task leaves or the heap can't give its record,
 * task takes a solid child too. the child's copy of landing_dust_collision
 * goes into the world as WORLD_DUSTY. task needs a work */
void landing_init(struct task *task);

#endif
