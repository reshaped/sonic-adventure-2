/* the pickup, PIC in City Escape's object list: an object a player picks up
 * and holds */
#ifndef STG13_PICKUP_H
#define STG13_PICKUP_H
#include <ninja.h>

struct task;

/* the pickup's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *pickup_texlists[];

/* the object list's step for PIC. it makes task a holdable object of
 * shared/common_object.h, as holdable_init does, under HOLDABLE_BOUNCY,
 * HOLDABLE_ROLLS and OBJECT_MAY_LEAVE. it draws pickup_model with
 * pickup_texlist */
void pickup_init(struct task *task);

extern NJS_TEXLIST pickup_texlist;
extern NJS_CNK_MODEL pickup_model;

#endif
