/* Sky Rail's scenery: six kinds of object that stand still and draw a model
 * each */
#ifndef STG06_SCENERY_H
#define STG06_SCENERY_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the scenery's six kinds, ended
 * by null */
extern NJS_TEXLIST *scenery_texlists[];

/* the object list's steps for RCROSS, TIM, KUSA, KAKASI, FENCE and WOODEAD.
 * each does nothing where task leaves at its placement's range. otherwise it
 * gives task the object's step, draw and destructor.
 *
 * task needs a work. its pos places the object, and its ang.y turns it. each
 * later step tests the range as the first did. rcross_init's and grass_init's
 * objects turn by all of ang, by njRotateZ, njRotateX and njRotateY, in that
 * order. every object but grass_init's collides, through a collision record of
 * kind COLLISION_OBJECT */
void rcross_init(struct task *task);
void tim_init(struct task *task);
void grass_init(struct task *task);
void scarecrow_init(struct task *task);
void fence_init(struct task *task);
void dead_tree_init(struct task *task);

#endif
