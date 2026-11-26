/* the painting, PAINTING in a stage's object list: one of three pictures */
#ifndef SHARED_PAINTING_H
#define SHARED_PAINTING_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the pictures' texture lists by picture, for texture_load_shared, ended by
 * null */
extern NJS_TEXLIST *painting_texlists[];

/* build at buf each picture that isn't built yet, if its texture list is
 * loaded. it returns buf past the builds. once one call has built all three,
 * the painting draws from the builds. it draws from the models until then.
 * where objects_fog_off isn't 0, the call builds with the fog off, then turns
 * the fog on */
Uint8 *painting_build(Uint8 *buf);

/* the object list's step for PAINTING. it does nothing where
 * placement_try_leave(task) returns TRUE. otherwise task takes the painting's
 * own step, draw and destructor.
 *
 * task needs a work, whose pos places the picture. its ang.y and ang.x turn the
 * picture by njRotateY, then njRotateX. scl.x + 1 scales the picture along x
 * and y. the whole part of |scl.z|, mod 3, picks the picture. the picture draws
 * under DRAW_CONTROL_SHADOW, so the modifier volumes shadow it */
void painting_init(struct task *task);

#endif
