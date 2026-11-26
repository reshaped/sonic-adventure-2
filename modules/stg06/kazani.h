/* KAZANI in Sky Rail's object list: a flock of flyers about a box. each steers
 * toward targets of its own */
#ifndef STG06_KAZANI_H
#define STG06_KAZANI_H
#include <ninja.h>

struct task;

/* the flyer's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *kazani_texlists[];

/* the object list's step for KAZANI. it does nothing where task leaves at its
 * placement's range. otherwise it gives task a record in task->motion, which
 * points at room for the flyers, both from syCalloc. it then gives task its own
 * step, draw and destructor, which frees both. where syCalloc fails, the call
 * frees what it took, and the next step tries again.
 *
 * task needs a work. pos is the middle of the flock's box, ang.y turns the box
 * and scl scales it. each later step tests the range as the first did. the low
 * byte of ang.x, plus 1, is the count of flyers. each step caps that byte at
 * 20, but the count stays as the init fixed it. while game_state.time_stopped
 * isn't 0, the flyers hold still */
void kazani_init(struct task *task);

#endif
