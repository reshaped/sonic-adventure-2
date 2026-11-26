/* the fence, FENCE in Sand Ocean's and Hidden Base's object lists: a row of
 * panels */
#ifndef SHARED_OCEAN_FENCE_H
#define SHARED_OCEAN_FENCE_H
#include <ninja.h>

struct task;

/* the kinds' models and texture lists, which ocean_fence_art.c defines */
extern NJS_CNK_MODEL ocean_fence_bright_model;
extern NJS_TEXLIST ocean_fence_bright_texlist;
extern NJS_CNK_MODEL ocean_fence_dark_model;
extern NJS_TEXLIST ocean_fence_dark_texlist;
/* the kinds' texture lists, for texture_load_shared, ended by null */
extern NJS_TEXLIST *ocean_fence_texlists[];

/* the object list's step for FENCE. it does nothing where
 * placement_try_leave(task) returns TRUE. otherwise task takes the fence's own
 * step, draw and destructor, and a collision record of kind COLLISION_OBJECT.
 * the record holds one box, which pushes players, enemies and objects. where
 * the heap has no room for the record, the task goes on without one.
 *
 * task needs a work, whose pos places the row's first panel and whose ang.y
 * turns the row. ang.z % 2 is the kind, BRIGHT or DARK, and ang.z must not be
 * negative. the whole part of scl.y counts the panels. they lie 50 apart along
 * the row's x, toward -x for a negative count. a count of 0 draws one panel.
 * the box is 40 high and 10 deep, its middle 20 above pos. it lies over the
 * row. its length is 50 times the count's size, or 50 for a count from -1 to 1.
 * for a count below -1, it lies a panel's length further toward -x than the
 * row */
void ocean_fence_init(struct task *task);

#endif
