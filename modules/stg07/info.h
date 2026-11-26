/* MIINFO in Aquatic Mine's object list: a solid model whose face shows one of
 * three textures */
#ifndef STG07_INFO_H
#define STG07_INFO_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the model and of its face,
 * ended by null */
extern NJS_TEXLIST *info_texlists[];

/* the object list's step for MIINFO. it does nothing where task leaves at its
 * placement's range. where the heap can't give its record, it does nothing,
 * and the next step tries again. otherwise it gives task the object's step,
 * draw and destructor. it puts a copy of its collision object in the collision
 * world. each later step tests the range as the first did.
 *
 * task needs a work, whose pos places the object. its ang turns the object in
 * the order z, y, x. the whole part of |scl.x|, modulo 3, picks the face's
 * texture */
void info_init(struct task *task);

#endif
