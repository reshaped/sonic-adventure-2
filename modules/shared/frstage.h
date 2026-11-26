/* the space stages' pulsing platforms, by their names in the object lists:
 *
 *   FRSTAGE       Final Rush and Final Chase
 *   FRSTAGE_FAR   Final Rush
 *   STAGE         Cosmic Wall
 *   AIRCONT       Meteor Herd and Mad Space
 */
#ifndef SHARED_FRSTAGE_H
#define SHARED_FRSTAGE_H
#include <ninja.h>

struct task;

/* @hack type: each node as an array of one, where the node alone would do */
extern NJS_CNK_OBJECT frstage_light[];
extern NJS_CNK_OBJECT frstage_floor[];
extern NJS_CNK_OBJECT frstage_fence_0[];
extern NJS_OBJECT frstage_fence_collision_0[];

extern NJS_TEXLIST frstage_texlist;
/* the platform's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *frstage_texlists[];

/* the object list's step for a platform. it does nothing where task leaves
 * at its placement's range. where the heap can't give a copy of
 * frstage_collision, it does nothing, and the next step tries again.
 * otherwise it gives task the platform's step, draw and destructor. task's
 * motion holds the copy. the copy goes into the world as a solid, at the
 * work's pos.
 *
 * task needs a work, whose ang.y turns the platform. FRSTAGE and FRSTAGE_FAR
 * add fence n where nibble n of ang.x isn't 0. STAGE adds its one fence where
 * bit 0 of ang.x is set. each fence is a solid child of task. AIRCONT has no
 * fence, and all three of its angles turn it.
 *
 * the platform collides only while a player lies within 80 of pos, or within
 * 120 for AIRCONT. the destructor takes the copy out of the world and frees
 * it */
void frstage_init(struct task *task);

#endif
