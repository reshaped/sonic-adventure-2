/* the short and long fences, FENCES and FENCEL in Prison Lane's object list,
 * and the panel, PANEL */
#ifndef STG09_FENCE_H
#define STG09_FENCE_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the two fences and of the
 * panel, ended by null */
extern NJS_TEXLIST *fence_texlists[];

/* the object list's steps for FENCES and FENCEL. each does nothing where task
 * leaves at its placement's range. otherwise it gives task the fence's step,
 * draw and destructor, and a collision record of kind COLLISION_OBJECT. the
 * short fence's record holds a box, and the long one's a box and two bars.
 * where the heap has no room for the record, the fence goes on without one.
 * task->motion holds the fence's model, and task->force its texture list.
 *
 * task needs a work, whose pos places the fence and whose ang.y turns it. each
 * later step tests the range as the first did. where the work's mode or
 * submode isn't 0 as the init runs, the fence takes another draw. that draw
 * can pulse the fence, shadow it, or turn it about x and z. the set manager
 * leaves both 0 */
void fence_short_init(struct task *task);
void fence_long_init(struct task *task);
/* the object list's step for PANEL. it makes task a solid object of the panel,
 * as solid_object_init does. the object's mode holds OBJECT_AS_LAND and
 * OBJECT_MAY_LEAVE, and it has no collision shapes */
void panel_init(struct task *task);

#endif
