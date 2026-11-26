/* Aquatic Mine's solid models: MIFRAME, MILIFT, MIBAR, MIWOOD and MIWOODY in
 * the stage's object list, and miwood and miwoody */
#ifndef STG07_SOLID_MODELS_H
#define STG07_SOLID_MODELS_H
#include <ninja.h>

struct task;

/* in the work's submode: the model never leaves at its placement's range.
 * any submode but 0 does the same */
#define SOLID_MODEL_STAYS 1

/* the models' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *solid_models_texlists[];

/* the object list's steps for MIFRAME, MILIFT, MIBAR, MIWOOD and MIWOODY, and
 * for miwood and miwoody. each does nothing where the work's submode is 0 and
 * task leaves at its placement's range. where the heap can't give its record,
 * it does nothing, and the next step tries again. otherwise it gives task its
 * own step, draw and destructor.
 *
 * task needs a work. its pos places the model, and its angles turn it by
 * njRotateZ, njRotateY and njRotateX, in that order. each later step tests the
 * range as the first did, while the submode is 0. MIFRAME, MILIFT and MIBAR
 * draw under DRAW_CONTROL_SHADOW. the manager of stg07/water.h makes its lift
 * by milift_init, under SOLID_MODEL_STAYS.
 *
 * the model's copy of its collision object stands in the collision world,
 * under WORLD_SOLID. a player is near within the collision model's radius plus
 * 15 of that model's center. the copy collides only while a player is near.
 * each step sets WORLD_ADDED in the work's flags while one is, and clears it
 * otherwise */
void miframe_init(struct task *task);
void milift_init(struct task *task);
void mibar_init(struct task *task);
void miwood_init(struct task *task);
void miwoody_init(struct task *task);

#endif
