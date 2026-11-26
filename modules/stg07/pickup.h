/* the pickup, PIC in Aquatic Mine's object list. it is the pumpkin of
 * shared/pumpkin_pickup.h, with a model and textures of its own */
#ifndef STG07_PICKUP_H
#define STG07_PICKUP_H
#include <ninja.h>

struct task;

/* the pickup's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *pickup_texlists[];

/* the object list's step for PIC. it does what pumpkin_pickup_init does, but
 * leaves BODY_RESTS out of the pickup's body */
void pickup_init(struct task *task);

#endif
