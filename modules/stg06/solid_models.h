/* Sky Rail's solid models: RAILOBJA and MOASIBA in the stage's object list */
#ifndef STG06_SOLID_MODELS_H
#define STG06_SOLID_MODELS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* a model's record, at task->motion, from the heap. object is the copy of its
 * collision object that the collision world holds. it comes first: world_add
 * and world_remove take task->motion as the copy. a model with no collision
 * object leaves object.model null. center is where the collision model's
 * center lies in the world */
struct solid_model {
    NJS_OBJECT object;
    NJS_POINT3 center;
    NJS_TEXLIST *texlist;
    /* the model RAILOBJA or MOASIBA draws, or null for none */
    NJS_CNK_MODEL *model;
    /* the node MOTOOL, RAILOBJBR or RAILOBJBL draws, or null for none */
    NJS_CNK_OBJECT *node;
};

/* the models' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *solid_models_texlists[];

/* the object list's steps for RAILOBJA and MOASIBA. each does nothing where
 * the work's submode is 0 and task leaves at its placement's range. otherwise
 * it gives task its own step, draw and destructor, and a record in
 * task->motion from syCalloc. the destructor frees the record. where syCalloc
 * fails, the call does nothing, and the next step tries again.
 *
 * task needs a work. its pos places the model, and its angles turn it by
 * njRotateZ, njRotateY and njRotateX, in that order. each later step tests the
 * range as the first did, while the submode is 0. the model draws under
 * DRAW_CONTROL_SHADOW.
 *
 * the model's copy of its collision object stands in the collision world,
 * under WORLD_SOLID. a player is near within the collision model's radius plus
 * 15 of that model's center. the copy collides only while a player is near.
 * each step sets WORLD_ADDED in the work's flags while one is, and clears it
 * otherwise */
void railobja_init(struct task *task);
void moasiba_init(struct task *task);

#endif
