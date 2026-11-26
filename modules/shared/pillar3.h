/* PILLAR3 in a stage's object list: three posts under a beam, which stand in
 * the players' way */
#ifndef SHARED_PILLAR3_H
#define SHARED_PILLAR3_H
#include <ninja.h>

struct task;

/* the object's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *pillar3_texlists[];

/* the object list's step for PILLAR3. it does nothing where
 * placement_try_leave(task) returns TRUE. otherwise it gives task the object's
 * own step, draw and destructor. it also gives task a COLLISION_OBJECT record:
 * a box for the beam and a cylinder for each post. where the heap has no room
 * for the record, the object goes on without one.
 *
 * task needs a work, whose pos places the middle post's foot and whose ang.y
 * turns the whole. the object's scale is 1 more than scl.x. at a scale of 1
 * the posts stand 85 apart along z, 120 high */
void pillar3_init(struct task *task);
extern NJS_CNK_MODEL pillar3_model;
extern NJS_TEXLIST pillar3_texlist;

#endif
