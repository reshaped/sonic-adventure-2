/* the breakable slab, BOAB in Metal Harbor's object list. it breaks into seven
 * pieces at a touch */
#ifndef STG10_BREAKABLE_SLAB_H
#define STG10_BREAKABLE_SLAB_H
#include <ninja.h>

struct task;

/* the slab's texture lists for texture_load_shared, ended by null. they hold
 * the slab's and its pieces' lists, then the burst's and the dust's */
extern NJS_TEXLIST *breakable_slab_texlists[];

/* the object list's step for BOAB. unless placement_try_leave(task) returns
 * TRUE, it gives task the slab's own step, draw and destructor. it also gives
 * task a collision record of kind COLLISION_OBJECT.
 *
 * task needs a work. pos places the slab, and ang.y turns it. a placed slab
 * takes its placement's state as its work's mode. 0 starts it whole, and any
 * other value broken. the destructor stores the mode in the state. a whole
 * slab's collision is a flat box that pushes nothing. the first step that
 * finds COLLISION_TOUCHED in the record breaks the slab and ends its task. the
 * pieces fall away at player 0's falling speed. a sound plays, and player 0's
 * pad shakes.
 *
 * a task that starts broken draws nothing. its collision is a wider box that
 * pushes, which it registers only while player 0 lies below pos. the caller
 * must keep a player in slot 0 while such a task runs */
void breakable_slab_init(struct task *task);

#endif
