/* smoke puffs: sprites of the texture jet1 that drift from a point and fade.
 * Big Foot makes them */
#ifndef SHARED_SMOKE_PUFF_H
#define SHARED_SMOKE_PUFF_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

enum smoke_puff_kind {
    SMOKE_PUFF_LARGE, /* fades 0.12 a step, of size 0.33 to 0.41 */
    SMOKE_PUFF_FAST, /* fades 0.25 a step, of size 0.3 to 0.38 */
    SMOKE_PUFF_SMALL /* fades 0.12 a step, of size 0.24 to 0.32 */
};

/* make a puff of kind as parent's child at *pos, and return its task. it
 * returns null where the heap can't give the task or its records. for a null
 * parent it returns no value. a kind past SMOKE_PUFF_SMALL makes a
 * SMOKE_PUFF_LARGE puff.
 *
 * each step the puff moves by spd times a random factor from 0.9 to 1.1. a
 * SMOKE_PUFF_FAST puff's factor is 1 more. the puff starts opaque, and its
 * task ends itself once the puff's alpha falls to 0.05. the puff holds still
 * while game_state.time_stopped isn't 0 */
struct task *smoke_puff_create(struct task *parent, int kind, NJS_POINT3 *pos,
                               NJS_VECTOR *spd);

#endif
