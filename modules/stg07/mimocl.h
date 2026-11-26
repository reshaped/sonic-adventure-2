/* MIMOCL in Aquatic Mine's object list: a solid model whose child part turns
 * about its z axis */
#ifndef STG07_MIMOCL_H
#define STG07_MIMOCL_H
#include <ninja.h>

struct task;

/* the texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *mimocl_texlists[];

/* the object list's step for MIMOCL. it does nothing where task leaves at its
 * placement's range. where the heap can't give its collision object's copy, it
 * does nothing, and the next step tries again. otherwise it gives task the
 * object's step, draw and destructor. it puts the copy in the collision world.
 * each later step tests the range as the first did.
 *
 * task needs a work, whose pos places the object. its ang turns the object in
 * the order z, y, x, but the copy takes no ang.x. the part turns at a steady
 * speed, which each object draws at random */
void mimocl_init(struct task *task);

#endif
