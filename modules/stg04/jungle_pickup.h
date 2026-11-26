/* White Jungle's pickup, PIC in the stage's object list. it is the jungle
 * pickup of shared/jungle_pickup.h over a model and textures of its own */
#ifndef STG04_JUNGLE_PICKUP_H
#define STG04_JUNGLE_PICKUP_H
#include <ninja.h>

struct task;

/* the pickup's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *white_jungle_pickup_texlists[];

/* the object list's step for PIC. it does what jungle_pickup_init does */
void white_jungle_pickup_init(struct task *task);

#endif
