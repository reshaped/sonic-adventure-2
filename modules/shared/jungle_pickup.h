/* the jungle pickup, PIC in a stage's object list: an object a player picks
 * up and holds */
#ifndef SHARED_JUNGLE_PICKUP_H
#define SHARED_JUNGLE_PICKUP_H
#include <ninja.h>

struct task;

/* the pickup's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *jungle_pickup_texlists[];

/* the object list's step for PIC. it makes task a holdable object of
 * shared/common_object.h, as holdable_init does, under HOLDABLE_PUSHES and
 * OBJECT_MAY_LEAVE */
void jungle_pickup_init(struct task *task);

#endif
