/* the rail, RBR in Radical Highway's object list: a solid model with a rail
 * along its top. the players grind on the rail */
#ifndef STG14_RAIL_H
#define STG14_RAIL_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the rail's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *highway_rail_texlists[];

/* the object list's step for RBR. unless placement_try_leave(task) returns
 * TRUE, it gives task the rail's own step, draw and destructor. it also gives
 * task a collision record of the rail's box.
 *
 * task needs a work. pos places the rail, and ang.y turns it. a child task
 * puts the players on a copy of the rail's path, placed as the rail is. the
 * child's step is path_rail_init. where the heap can't give the copy, the child
 * or its record, the rail has no path */
void highway_rail_init(struct task *task);

#endif
