/* the jungle's props, JP and JP_FAR in a stage's object list. each is one of
 * eighteen models, which nothing collides with */
#ifndef SHARED_JUNGLE_PROP_H
#define SHARED_JUNGLE_PROP_H
#include <ninja.h>

struct task;

/* the props' texture lists for texture_load_shared, each model's at its
 * index, ended by null */
extern NJS_TEXLIST *jungle_prop_texlists[];
/* prop 10's texture list and model, which shape 2 of shared/solid_scenery.h
 * draws too */
extern NJS_TEXLIST jungle_prop_texlist_10;
extern NJS_CNK_MODEL jungle_prop_model_10;

/* the object list's steps for JP and JP_FAR. JP's prop leaves at its
 * placement's range, and JP_FAR's at 1450. each does nothing where task
 * leaves. otherwise each gives task the prop's step, draw and destructor.
 * each later step tests the range as the first did.
 *
 * task needs a work, and no TASK_MOTION or TASK_FORCE records. the prop keeps
 * its model in task->motion, and its texture list in task->force. the work's
 * pos places the prop, and its ang turns it. scl.x, as an unsigned integer
 * modulo 18, picks the model */
void jungle_prop_init(struct task *task);
void jungle_prop_far_init(struct task *task);

#endif
