/* White Jungle's props, JP and JP_FAR in the stage's object list. each is
 * one of sixteen models, and JP's model 0 stands in a collision sphere */
#ifndef STG04_JUNGLE_PROP_H
#define STG04_JUNGLE_PROP_H
#include <ninja.h>

struct task;

/* the props' texture lists for texture_load_shared, each model's at its
 * index, ended by null */
extern NJS_TEXLIST *white_jungle_prop_texlists[];
/* Solid scenery also draws its shape 0 with prop 9's textures and model. */
extern NJS_TEXLIST white_jungle_prop_texlist_9;
extern NJS_CNK_MODEL white_jungle_prop_model_9;

/* the object list's steps for JP and JP_FAR. JP's prop leaves at its
 * placement's range, and JP_FAR's at 1450. each does nothing where task
 * leaves. otherwise each gives task the prop's step, draw and destructor.
 *
 * task needs a work. its pos places the prop, and its ang turns it. scl.x, as
 * an unsigned integer modulo 16, picks the model. each later step tests the
 * range as the first did. it registers the work's collision record where the
 * work has one.
 *
 * JP gives model 0's prop a collision record of kind COLLISION_OBJECT, of one
 * sphere of radius 15. it picks the shape by scl.x without the modulo, so
 * scl.x must lie below 16. JP_FAR gives every prop a record too. it reads the
 * record's shape from past the end of the props' table of shapes */
void white_jungle_prop_init(struct task *task);
void white_jungle_prop_far_init(struct task *task);

#endif
