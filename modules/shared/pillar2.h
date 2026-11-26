/* PILLAR2 in a stage's object list: a pillar in the players' way, tilted by
 * its angles */
#ifndef SHARED_PILLAR2_H
#define SHARED_PILLAR2_H
#include <ninja.h>

struct task;

/* the pillar's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *pillar2_texlists[];

/* the object list's step for PILLAR2. it does nothing where
 * placement_try_leave(task) returns TRUE. otherwise it gives task the pillar's
 * own step, draw and destructor. it also gives task a COLLISION_OBJECT cylinder
 * that the work's angles turn. where the heap has no room for the collision
 * record, the pillar goes on without one.
 *
 * task needs a work, whose pos places the pillar's foot. its ang.x and ang.z
 * turn the pillar, about x first. the pillar's scale is 1 more than scl.x. at
 * a scale of 1 the cylinder is 160 long, its middle 80 from pos */
void pillar2_init(struct task *task);
extern NJS_CNK_MODEL pillar2_model;
extern NJS_TEXLIST pillar2_texlist;

#endif
