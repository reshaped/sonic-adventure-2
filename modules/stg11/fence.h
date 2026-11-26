/* the short and long fences, FENCES and FENCEL in Iron Gate's object list, and
 * the panel, PANEL. SGOAL, a shut door, runs as a fence */
#ifndef STG11_FENCE_H
#define STG11_FENCE_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the two fences, SGOAL and the
 * panel, ended by null */
extern NJS_TEXLIST *iron_gate_fence_texlists[];

/* the object list's steps for SGOAL, FENCES and FENCEL. each does nothing where
 * placement_try_leave(task) returns TRUE. otherwise it gives task the fence's
 * step, draw and destructor, and a collision record of kind COLLISION_OBJECT.
 * SGOAL's record holds five boxes about a shut door. the short fence's holds a
 * box, and the long one's a box and two bars. where the heap has no room for
 * the record, the fence goes on without one. task->motion holds the fence's
 * model, and task->force its texture list.
 *
 * task needs a work, whose pos places the fence and whose ang.y turns it. each
 * step does nothing where placement_try_leave(task) returns TRUE. where the
 * work's mode or submode isn't 0 as the init runs, the fence takes another
 * draw. that draw can pulse the fence, shadow it, or turn it about x and z. the
 * set manager leaves both 0 */
void sgoal_init(struct task *task);
void iron_gate_fence_short_init(struct task *task);
void iron_gate_fence_long_init(struct task *task);
/* the object list's step for PANEL. it makes task a solid object of the panel,
 * as solid_object_init does. the object's mode holds OBJECT_AS_LAND and
 * OBJECT_MAY_LEAVE, and it has no collision shapes */
void iron_gate_panel_init(struct task *task);

#endif
