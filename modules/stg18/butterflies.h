/* the butterflies, BUTTERFLY in Dry Lagoon's object list: a flock that
 * flutters about a point */
#ifndef STG18_BUTTERFLIES_H
#define STG18_BUTTERFLIES_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the butterflies' texture lists for texture_load_shared, a kind's first
 * frame's at the kind's index, ended by null */
extern NJS_TEXLIST *butterflies_texlists[];

/* build at buf each frame of each kind that isn't built yet, if its texture
 * list is loaded. it returns buf past the builds. a butterfly draws from its
 * build once a call has built all nine, and from its model before. where
 * objects_fog_off isn't 0, the call builds with the fog off, then turns the
 * fog on */
Uint8 *butterflies_build(Uint8 *buf);

/* the object list's step for BUTTERFLY. unless placement_try_leave(task)
 * returns TRUE, it gives task the flock's own step, draw and destructor, and
 * ang.z butterflies. a butterfly syMalloc can't give is left out.
 *
 * task needs a work. a butterfly's distance from pos swings between about a
 * third of scl.x and scl.x. its place tilts from straight above pos about x,
 * then turns about y. the tilt and the turn each swing by up to ang.x either
 * way. the flock keeps its list in task->extra, and the destructor frees it */
void butterflies_init(struct task *task);

#endif
