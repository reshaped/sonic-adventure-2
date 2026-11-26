/* Crazy Gadget's POISON, a tank of poison */
#ifndef STG22_POISON_H
#define STG22_POISON_H
#include <ninja.h>

struct task;

/* the object list's init for POISON. it does nothing where task leaves at its
 * placement's range. where the heap can't give the tank's record, it does
 * nothing, and the next step tries again. otherwise it gives task the tank's
 * step, draw and destructor, and a collision record of kind COLLISION_OBJECT.
 * it makes the tank a missile target.
 *
 * task needs a work. pos places the tank, and ang.y turns it. the low 5 bits
 * of ang.z are its missile target id. a missile of that id breaks the tank.
 * it then fades over 120 steps and ends, as placement_kill ends a task. a
 * player's touch of the whole tank makes player 0 miss. gas then rises from
 * player 0 for 60 steps, and the tank fades over 120 steps and stays */
void gadget_poison_init(struct task *task);
extern NJS_TEXLIST gadget_poison_texlist;

extern NJS_TEXLIST *gadget_poison_texlists[];

#endif
