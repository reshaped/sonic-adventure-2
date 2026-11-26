/* the foothold a flute stand calls up, KDASIBA in a stage's object list. the
 * foothold appears once its stand is played */
#ifndef SHARED_KNUDAI_FOOTHOLD_H
#define SHARED_KNUDAI_FOOTHOLD_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the foothold and its
 * sparkles, ended by null */
extern NJS_TEXLIST *knudai_foothold_texlists[];

/* the object list's step for KDASIBA. task needs a work and TASK_FORCE's
 * records. its pos places the foothold, its ang.y turns it, and its ang.x holds
 * its stand's id. the step does nothing where placement_try_leave(task) returns
 * TRUE. otherwise it takes a record in task->motion, and gives task the
 * foothold's step and destructor. where the heap can't give the record, task
 * keeps this step to try again.
 *
 * a foothold shows at once where its stand was played before it starts.
 * otherwise it waits unseen for the play, and until then players pass through
 * it. after the play, it waits 0.8 steps for each unit of its pos's distance
 * from the origin. that wait lasts at most 160 steps, and the foothold then
 * fades in. the wait measures from the origin, not from the stand:
 * knudai_get_pos gives the origin for a played stand.
 *
 * a foothold that has begun to fade in bobs within 2 of its pos. each step sets
 * the speed.y of TASK_FORCE's two records to the foothold's rise since the step
 * before. a player riding the foothold rises with it */
void knudai_foothold_init(struct task *task);

#endif
