#include "chao/activities.h"
#include "chao/behavior.h"
#include "chao/idle.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/work.h"
#include "system/task.h"

/* behavior->mode in each behavior below */
enum posture_mode { POSTURE_START, POSTURE_WAIT };

#define POSE_CASE(from, motion, frames)                                        \
    case (from):                                                               \
        chao_link_motion_frames(chao, (motion), (frames));                     \
        break
#ifdef NONMATCHING
/* the plain form the hack tag names */
#define RETURN_FINISH() return BEHAVIOR_FINISH
#define RETURN_CONTINUE() return BEHAVIOR_CONTINUE
#define RETURN_MODE() return behavior->mode
#else
/* @hack implicit: no return where each step ends, where NONMATCHING's
 * returns of BEHAVIOR_FINISH, BEHAVIOR_CONTINUE and behavior->mode would do.
 * SHC's code returns the value the step computed last: the mode it set,
 * chao_motion_ended's 0, or the mode it switched on */
#define RETURN_FINISH() break
#define RETURN_CONTINUE() break
#define RETURN_MODE() ((void)0)
#endif
#define WAIT_FOR_MOTION()                                                      \
    case POSTURE_WAIT:                                                         \
        if (chao_motion_ended(chao)) {                                         \
            return BEHAVIOR_FINISH;                                            \
        }                                                                      \
        RETURN_CONTINUE()

int chao_stand_up_behavior(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_behavior *behavior = &work->behavior;
    int pose;
    switch (behavior->mode) {
    case POSTURE_START:
        pose = chao_motion_pose(chao);
        switch (pose) {
            POSE_CASE(CHAO_SITTING, CHAO_ANIM_GENKI_TATI, 40);
            POSE_CASE(CHAO_ON_BACK, CHAO_ANIM_JUMP_UP, 40);
            POSE_CASE(CHAO_ON_FRONT, CHAO_ANIM_JUMP_UP, 35);
        case CHAO_STANDING:
        default:
            return BEHAVIOR_FINISH;
        }
        behavior->mode = POSTURE_WAIT;
        /* @bug uninitialized: POSTURE_WAIT never runs. the step returns no
         * value after setting the mode, and the caller takes the mode's 1 left
         * in R0 for BEHAVIOR_FINISH (MOV #1,R0 at 0x0c5348d6, RTS at
         * 0x0c5348ee) */
        RETURN_FINISH();
        WAIT_FOR_MOTION();
    }
    RETURN_MODE();
} /* NOLINT(clang-diagnostic-return-type) */

int chao_sit_down_behavior(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_behavior *behavior = &work->behavior;
    int pose;
    switch (behavior->mode) {
    case POSTURE_START:
        pose = chao_motion_pose(chao);
        switch (pose) {
            POSE_CASE(CHAO_ON_BACK, CHAO_ANIM_OKIRU, 40);
            POSE_CASE(CHAO_ON_FRONT, CHAO_ANIM_OKIRU, 35);
            POSE_CASE(CHAO_STANDING, CHAO_ANIM_GENKI_SUWARI, 40);
        default:
            return BEHAVIOR_FINISH;
        }
        behavior->mode = POSTURE_WAIT;
        /* @bug uninitialized: POSTURE_WAIT never runs, as in
         * chao_stand_up_behavior (MOV #1,R0 at 0x0c53496e, RTS at
         * 0x0c534986) */
        RETURN_FINISH();
        WAIT_FOR_MOTION();
    }
    RETURN_MODE();
} /* NOLINT(clang-diagnostic-return-type) */
int chao_recline_behavior(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_behavior *behavior = &work->behavior;
    int pose;
    switch (behavior->mode) {
    case POSTURE_START:
        pose = chao_motion_pose(chao);
        switch (pose) {
            POSE_CASE(CHAO_SITTING, CHAO_ANIM_SLEEP, 40);
            POSE_CASE(CHAO_STANDING, CHAO_ANIM_BATAN, 40);
            POSE_CASE(CHAO_ON_FRONT, CHAO_ANIM_SLEEP, 40);
        default:
            return BEHAVIOR_FINISH;
        }
        behavior->mode = POSTURE_WAIT;
        /* @bug uninitialized: POSTURE_WAIT never runs, as in
         * chao_stand_up_behavior (MOV #1,R0 at 0x0c5349da, RTS at
         * 0x0c5349f2) */
        RETURN_FINISH();
        WAIT_FOR_MOTION();
    }
    RETURN_MODE();
} /* NOLINT(clang-diagnostic-return-type) */
