/* HATADAI in Sand Ocean's object list: a flag's stand */
#ifndef STG21_FLAG_STAND_H
#define STG21_FLAG_STAND_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST flag_stand_texlist;
extern NJS_CNK_MODEL flag_stand_model;
extern NJS_CNK_OBJECT flag_stand_root;
/* the stand's texture list for texture_load_shared, then null */
extern NJS_TEXLIST *flag_stand_texlists[];

/* the object list's step for HATADAI. it does nothing where task leaves at
 * its placement's range. otherwise it gives task the stand's step, draw and
 * destructor, and a collision record of kind COLLISION_OBJECT. the record
 * holds a cylinder. where the heap has no room for the record, the stand
 * goes on without one.
 *
 * task needs a work, whose pos places the stand's foot and whose ang.y turns
 * it. each later step tests the range as the first did. scl.x + 1 scales the
 * stand and its cylinder */
void flag_stand_init(struct task *task);

#endif
