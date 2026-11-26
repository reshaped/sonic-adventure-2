/* Final Rush's curved grind rails, GLIND and GLIND_B in its object list. the
 * players grind along a rail's path, and a texture along it scrolls */
#ifndef STG30_CURVED_RAIL_H
#define STG30_CURVED_RAIL_H
#include <ninja.h>

struct task;

/* the two rails' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *curved_rail_texlists[];

/* the object list's steps for GLIND and GLIND_B. each sets its rail up unless
 * placement_try_leave(task) returns TRUE. where the heap can't give the rail's
 * record, the step runs again at task's next step.
 *
 * task needs a work. pos places the rail, and ang turns it. the texture's
 * scroll along u is game_state.steps times the whole part of scl.x. the players
 * grind on a copy of the rail's path, which a child task runs. where the heap
 * can't give the copy, the task or the task's record, the rail has no path.
 *
 * GLIND_B also has a barrier, a wall that crashes a player who runs into it
 * fast enough. the barrier has a child task of its own. where
 * task_create_child can't give that task, GLIND_B has no barrier */
void curved_rail_init(struct task *task);
void curved_rail_b_init(struct task *task);

#endif
