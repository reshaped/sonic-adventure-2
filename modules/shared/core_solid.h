/* a solid model that each of Cannon's Core's five routes links */
#ifndef SHARED_CORE_SOLID_H
#define SHARED_CORE_SOLID_H
#include <ninja.h>

struct task;

extern NJS_CNK_MODEL core_solid_model;
extern NJS_OBJECT core_solid_collision;
extern NJS_TEXLIST core_solid_texlist;
/* the model's texture lists, ended by null. no stage's texture groups name
 * them */
extern NJS_TEXLIST *core_solid_texlists[];

/* make task a solid object of core_solid_model, by solid_object_init with
 * OBJECT_AS_LAND and OBJECT_MAY_LEAVE. its node in the world is a copy of
 * core_solid_collision. no stage's object list names core_solid_init */
void core_solid_init(struct task *task);

#endif
