/* Aquatic Mine's scenery: seven kinds of object that stand still and draw a
 * model each */
#ifndef STG07_SCENERY_H
#define STG07_SCENERY_H
#include <ninja.h>

#include "shared/skull.h" /* IWYU pragma: export */

struct task;

/* the texture lists for texture_load_shared of the kinds but NOCOLSKULL,
 * ended by null */
extern NJS_TEXLIST *scenery_texlists[];

/* the object list's steps for MITRUCKP, MIKUSA, MILATTI, MIRAIL, MIRING and
 * MIFENCE, and for mitruckp, mikusa, mirail and mifence. each does nothing
 * where task leaves at its placement's range. otherwise it gives task the
 * object's step, draw and destructor.
 *
 * task needs a work. its pos places the object, and its ang.y turns it. each
 * later step tests the range as the first did. mikusa_init's and
 * mirail_init's objects turn by all of ang, by njRotateZ, njRotateY and
 * njRotateX, in that order. mitruckp_init's, milatti_init's and
 * mifence_init's objects collide, through a collision record of kind
 * COLLISION_OBJECT.
 *
 * NOCOLSKULL's step, nocolskull_init of shared/skull.h, is compiled here */
void mitruckp_init(struct task *task);
void mikusa_init(struct task *task);
void milatti_init(struct task *task);
void mirail_init(struct task *task);
void miring_init(struct task *task);
void mifence_init(struct task *task);

#endif
