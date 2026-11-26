/* the pickup object, PIC in a stage's object list: an object a player picks
 * up and holds */
#ifndef SHARED_PICKUP_OBJECT_H
#define SHARED_PICKUP_OBJECT_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST pickup_object_texlist;
extern NJS_CNK_MODEL pickup_object_model;

/* the object's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *pickup_object_texlists[];

/* the object list's step for PIC. it makes task a holdable object of
 * shared/common_object.h, as holdable_init does, with OBJECT_MAY_LEAVE its
 * only mode bit. the object thus doesn't roll, and its body's floor_bounce is
 * 0.3. each stage gives the object a model and texture list of its own */
void pickup_object_init(struct task *task);

#endif
