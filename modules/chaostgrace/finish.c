/* the racers' behaviors from the finish line on: the run past it, and the
 * victory and defeat poses */
#include <ninja.h>
#include <sg_xpt.h>
#include "chao/behavior.h"
#include "chao/minimal.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/work.h"
#include "chaostgrace/race.h"
#include "chaostgrace/racer.h"
#include "draw/motion.h"
#include "object/mover.h"
#include "system/task.h"

/* on the floor, the run past the finish line pushes a racer FINISH_ACCEL a
 * frame along its yaw */
#define FINISH_ACCEL 0.045F

int race_victory_pose(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_BANZAI_JUMP);
        RACE_ADVANCE(behavior);
        if (work->profile.parameters.body == CHAO_BODY_MINIMAL) {
            motion_link(&work->minimal_motion, MINIMAL_IDLE);
        }
        /* fall through */
    case 1:
        break;
    }
    return BEHAVIOR_CONTINUE;
}

int race_defeat_pose(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_HAKUSYU_STAND);
        RACE_USE_STAGE(chao);
        RACE_ADVANCE(behavior);
        if (work->profile.parameters.body == CHAO_BODY_MINIMAL) {
            motion_link(&work->minimal_motion, MINIMAL_IDLE);
        }
        /* fall through */
    case 1:
        break;
    }
    return BEHAVIOR_CONTINUE;
}

int race_cross_finish(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct chao_work *work = CHAO_WORK(chao);
    struct mover *mover = chao->motion.p;

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_ARUKU);
        RACE_USE_STAGE(chao);
        RACE_ADVANCE(behavior);
        /* fall through */
    case 1:
        break;
    }
    if (mover->flags & MOVER_ON_FLOOR) {
        mover->acc.x = njSin(work->base.ang.y) * FINISH_ACCEL;
        mover->acc.z = njCos(work->base.ang.y) * FINISH_ACCEL;
    }
    return BEHAVIOR_CONTINUE;
}

int race_end_finish(struct task *chao)
{
#ifdef NONMATCHING
#else
    /* @hack redundant: a read of the behavior's first word, which nothing
     * uses */
    (void)*(Uint32 *)&CHAO_WORK(chao)->behavior;
#endif
    RACE_USE_STAGE(chao);
    chao_set_behavior(chao, race_victory_pose);
    return BEHAVIOR_CONTINUE;
}
