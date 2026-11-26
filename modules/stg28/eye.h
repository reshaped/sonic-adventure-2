/* the eye, EYE in Pyramid Cave's object list: a solid eyeball, with a light
 * that faces the camera */
#ifndef STG28_EYE_H
#define STG28_EYE_H
#include <ninja.h>

struct task;

/* the eye's texture lists for texture_load_shared, the eyeball's and its
 * light's, ended by null */
extern NJS_TEXLIST *cave_eye_texlists[];

/* the object list's step for EYE. task needs a work. its pos places the eye,
 * and its ang turns the eyeball by njRotateY, njRotateX and njRotateZ.
 *
 * the players collide with a copy of the eyeball's shape in the collision
 * world, which task->extra holds. world_pool_alloc lends the copy, and the
 * call writes through null where the pool has none. the copy collides only
 * while a player lies within 210 of its pos */
void cave_eye_init(struct task *task);

#endif
