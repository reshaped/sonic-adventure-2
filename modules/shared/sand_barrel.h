/* the sand barrel, BUFFER in Mission Street's object list. a strike breaks
 * the barrel into its top and four sides. it leaves a heap of sand that
 * rises, then fades */
#ifndef SHARED_SAND_BARREL_H
#define SHARED_SAND_BARREL_H
#include <ninja.h>

struct task;

/* the barrel's, its pieces', its sand's, the burst's and the dust's texture
 * lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *sand_barrel_texlists[];

/* the object list's step for BUFFER. it does nothing where
 * placement_try_leave(task) returns TRUE. otherwise it gives task the barrel's
 * step, draw and destructor. it also gives task a collision record of one
 * cylinder, 20 across and 29 high.
 *
 * task needs a work. pos places the barrel's base, and ang.y turns it. a
 * barrel within its model's radius plus 15 of the camera draws see-through.
 *
 * an attack among the hits of task's record breaks the barrel, and so does
 * WORK_HURT in its work. the break ends task, adds 20 to the score and plays
 * sound 12. the pieces fly out, and fall to the floor under pos. the break
 * leaves a heap of sand at pos unless task_create fails. the heap is a task
 * named ObjectSand at task level 2.
 *
 * the heap waits 2 steps, then rises through its shape motion over 24 steps.
 * it fades out over the next 30 steps, and then ends. all the while it sheds
 * puffs of dust */
void sand_barrel_init(struct task *task);

#endif
