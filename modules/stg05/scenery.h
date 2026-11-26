/* Pumpkin Hill's scenery: eight kinds of object that stand still and draw a
 * model each */
#ifndef STG05_SCENERY_H
#define STG05_SCENERY_H
#include <sg_xpt.h>

#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the scenery's eight kinds,
 * ended by null */
extern NJS_TEXLIST *scenery_texlists[];

/* compile each kind's model at buf, and return where the builds end. where a
 * kind's build fails, every object draws its model uncompiled */
Uint8 *scenery_build(Uint8 *buf);

/* the object list's steps for TIM, KUSA, KAKASI, FENCES, FENCE, WOODEAD,
 * PPK_SM and PPK_BG. each does nothing where task leaves at its placement's
 * range. otherwise it gives task the object's step, draw and destructor.
 *
 * task needs a work. its pos places the object, and its ang.y turns it. each
 * later step tests the range as the first did.
 * grass_init's object turns by all of ang. every object but grass_init's
 * collides, through a collision record of kind COLLISION_OBJECT. the pumpkins
 * and the dead tree pulse, from a random phase. a hunter's punch sets a
 * pumpkin wobbling */
void tim_init(struct task *task);
void grass_init(struct task *task);
void scarecrow_init(struct task *task);
void fence_small_init(struct task *task);
void fence_init(struct task *task);
void dead_tree_init(struct task *task);
void pumpkin_small_init(struct task *task);
void pumpkin_big_init(struct task *task);

#endif
