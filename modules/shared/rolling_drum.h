/* the rolling drum, EECONT in Crazy Gadget's and Eternal Engine's object
 * lists. it rolls along a track, and fades in at one end and out at the
 * other */
#ifndef SHARED_ROLLING_DRUM_H
#define SHARED_ROLLING_DRUM_H
#include <ninja.h>

extern NJS_TEXLIST rolling_drum_texlist;
extern NJS_TEXLIST *rolling_drum_texlists[];
extern NJS_CNK_OBJECT rolling_drum_object;

struct task;

/* the object list's step for EECONT. unless placement_try_leave(task)
 * returns TRUE, it gives task the drum's step, draw and destructor. it also
 * gives task a child that carries the drum's cylinder. where the heap can't
 * give the drum's record, it does nothing, and the next step tries again.
 *
 * task needs a work. the track runs along x turned by ang.y, and pos is its
 * middle. it is scl.y + 2 * scl.x long. the drum rolls scl.z along it a step,
 * and starts again from the other end once past one. it fades in over the
 * track's first scl.x, and out over its last */
void rolling_drum_init(struct task *task);

#endif
