/* Sky Rail's moving models: RAILOBJCL, RAILOBJCR, MOTOOL, RAILOBJBR and
 * RAILOBJBL in the stage's object list */
#ifndef STG06_RAIL_OBJECTS_H
#define STG06_RAIL_OBJECTS_H
#include <ninja.h>

struct task;

/* the models' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *rail_objects_texlists[];

/* the object list's steps for RAILOBJCL and RAILOBJCR. each does nothing where
 * task leaves at its placement's range. otherwise it gives task its model's
 * step, draw and destructor, and a collision record of one shape.
 *
 * task needs a work. its pos places the model, and its angles turn it by
 * njRotateZ, njRotateX and njRotateY, in that order. each later step tests the
 * range as the first did. the model's part swings about x, from a random
 * phase. a time stop holds it still */
void railobjcl_init(struct task *task);
void railobjcr_init(struct task *task);

/* the object list's steps for MOTOOL, RAILOBJBR and RAILOBJBL. each does
 * nothing where the work's submode is 0 and task leaves at its placement's
 * range. otherwise it gives task its own step, draw and destructor, and a
 * record in task->motion from syCalloc. the destructor frees the record. where
 * syCalloc fails, the call does nothing, and the next step tries again.
 *
 * task needs a work. its pos places the model, and its angles turn it by
 * njRotateZ, njRotateY and njRotateX, in that order. each later step tests the
 * range as the first did, while the submode is 0. the model's children turn
 * about x by scl.x degrees a step. a time stop holds them still. the model
 * draws under DRAW_CONTROL_SHADOW.
 *
 * MOTOOL's copy of its collision object stands in the collision world, under
 * WORLD_SOLID. MOTOOL also has a collision record of one shape. a player is
 * near within the collision model's radius plus 15 of that model's center.
 * MOTOOL's step sets WORLD_ADDED in the work's flags while a player is near,
 * and clears it otherwise. RAILOBJBR and RAILOBJBL stand in neither the world
 * nor the collision checks */
void motool_init(struct task *task);
void railobjbr_init(struct task *task);
void railobjbl_init(struct task *task);

#endif
