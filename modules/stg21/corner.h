/* CORNER in Sand Ocean's and Hidden Base's object lists: a fence bent at a
 * right angle */
#ifndef STG21_CORNER_H
#define STG21_CORNER_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST corner_texlist;
/* the corner's texture list for texture_load_shared, then null */
extern NJS_TEXLIST *corner_texlists[];

/* the object list's step for CORNER. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the corner's step, draw and
 * destructor, and a collision record of kind COLLISION_OBJECT. the record
 * holds a box for each arm. where the heap has no room for the record, the
 * corner goes on without one.
 *
 * task needs a work, whose pos places the bend and whose ang.y turns the
 * fence. each later step tests the range as the first did. at an ang.y of 0,
 * the fence's two arms run from pos toward -x and toward +z */
void corner_init(struct task *task);
extern NJS_CNK_MODEL corner_model;

#endif
