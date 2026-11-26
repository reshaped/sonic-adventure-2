/* GRAVE in Sand Ocean's object list: a block with three posts along each long
 * edge of its top */
#ifndef STG21_GRAVE_H
#define STG21_GRAVE_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST grave_texlist;
/* the grave's texture list for texture_load_shared, then null */
extern NJS_TEXLIST *grave_texlists[];

/* the object list's step for GRAVE. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the grave's step, draw and
 * destructor, and a collision record of kind COLLISION_OBJECT. the record
 * holds a box for the block and a cylinder for each post. where the heap has
 * no room for the record, the grave goes on without one.
 *
 * task needs a work, whose pos is the center of the grave's bottom and whose
 * ang.y turns it. each later step tests the range as the first did. scl.x + 1
 * scales the grave and its shapes */
void grave_init(struct task *task);
extern NJS_CNK_MODEL grave_model;

#endif
