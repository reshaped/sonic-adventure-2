/* a garden Chao a player strikes or throws, or a jack-in-the-box startles. a
 * strike or a throw hurts the Chao's feelings, and it lies dazed where it
 * lands */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/behavior.h"
#include "chao/body_collision.h"
#include "chao/cocoon.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/garden.h"
#include "chao/garden_toys.h"
#include "chao/held.h"
#include "chao/icon.h"
#include "chao/knowledge.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/sound.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "collision/collision.h"
#include "object/mover.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* a Chao lies dazed for DAZED_STEPS and a random part of DAZED_SPAN */
#define DAZED_STEPS 180
#define DAZED_SPAN 121.0F
#define ANGER_STEPS 150
/* the frames each of the motions here links over */
#define LINK_FRAMES 15
/* a struck Chao flies off at KNOCK_SPEED away from its attacker, and up at
 * KNOCK_LIFT */
#define KNOCK_SPEED 0.8F
#define KNOCK_LIFT 1.5F
/* a strike costs STRUCK_AFFECTION and makes the Chao STRUCK_FEAR more afraid
 * of the player. a throw costs THROWN_AFFECTION and adds THROWN_FEAR */
#define STRUCK_AFFECTION 30
#define STRUCK_FEAR 50
#define THROWN_AFFECTION 5
#define THROWN_FEAR 10
/* a struck Chao that links CHAO_ANIM_HIMA_SIT spins by SPIN_STEP a step until
 * it lands */
#define SPIN_STEP 0x400
/* a startled Chao lies crying for CRY_STEPS steps, and its anger mark holds
 * for CRY_ANGER_STEPS */
#define CRY_STEPS 500
#define CRY_ANGER_STEPS 300
#define CRY_VOICE 0x6029
#define OTHER_CRY_VOICE 0x606d
#define FACE_VELOCITY()                                                        \
    work->ang.y = (Angle)(atan2f(mover->spd.x, mover->spd.z) * ANGLE_PER_RADIAN)
/* parameters.hat clears whether or not garden_drop_headgear makes the hat */
#ifdef NONMATCHING
/* this side clears the hat only where garden_drop_headgear makes it */
static void drop_hat(struct task *chao, struct mover *mover)
{
    if (garden_drop_headgear(CHAO_WORK(chao)->profile.parameters.hat,
                             &chao->work->pos, RANDOM_TURN(), &mover->spd)) {
        CHAO_WORK(chao)->profile.parameters.hat = 0;
    }
}
#define DROP_HAT() drop_hat(chao, mover)
#else
#define DROP_HAT()                                                             \
    garden_drop_headgear(CHAO_WORK(chao)->profile.parameters.hat,              \
                         &chao->work->pos, RANDOM_TURN(), &mover->spd);        \
    CHAO_WORK(chao)->profile.parameters.hat = 0
#endif
#define LAND_DAZED()                                                           \
    chao_link_motion_frames(chao, CHAO_ANIM_KOKE, LINK_FRAMES);                \
    behavior->timer = RANDOM_COUNT(DAZED_STEPS, DAZED_SPAN)
/* a strike or a throw adds DISTRESS and a random part of DISTRESS_SPAN to
 * each of five feelings. ADD_DISTRESS sets the caller's delta to the amount */
#define DISTRESS 50.0F
#define DISTRESS_SPAN 100.0F
#define ADD_DISTRESS(delta, feeling)                                           \
    (delta) = DISTRESS + (rand() * (DISTRESS_SPAN * RAND_SCALE));              \
    chao_emotion_add(chao, (feeling), (delta))
#define HURT_FEELINGS(delta)                                                   \
    ADD_DISTRESS(delta, CHAO_ANGER);                                           \
    ADD_DISTRESS(delta, CHAO_SORROW);                                          \
    ADD_DISTRESS(delta, CHAO_FEAR);                                            \
    ADD_DISTRESS(delta, CHAO_SURPRISE);                                        \
    ADD_DISTRESS(delta, CHAO_PAIN)

static int struck(struct task *chao);
static int fall_crying(struct task *chao);

int chao_knockback_behavior(struct task *chao)
{
    struct task_work *work = chao->work;
    struct chao_behavior *behavior = &((struct chao_work *)work)->behavior;
    struct mover *mover;
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_THROWN, LINK_FRAMES);
        mover = chao->motion.p;
        FACE_VELOCITY();
        chao_set_eye(chao, CHAO_EYE_DISTRESSED);
        chao_show_mark(chao, CHAO_MARK_ANGER, ANGER_STEPS);
        behavior->mode++;
        break;
    case 1:
        if (work->flags & MOVER_WORK_ON_FLOOR) {
            LAND_DAZED();
            behavior->mode++;
        }
        break;
    case 2:
        if (--behavior->timer <= 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
int chao_try_knockback(struct task *chao)
{
    struct task_work *work = chao->work;
    chao_behavior_fn current = chao_current_behavior(chao);
    struct task *attacker;
    struct mover *mover;
    NJS_VECTOR away;
    Float bounce;
    if (current != struck && current != chao_evolve_behavior &&
        current != chao_die_behavior && (work->flags & WORK_HURT)) {
        attacker = collision_next_player_by_shape(chao, CHAO_PUSH_SHAPE);
        work->flags &= ~WORK_HURT;
        if (attacker) {
            bounce = KNOCK_SPEED;
            mover = chao->motion.p;
            away.x = attacker->work->pos.x - work->pos.x;
            /* @bug uninitialized: away.y is never set. njUnitVector counts
             * the stack's leftover there in the length, so the knock across
             * shrinks by an amount the stack decides (0x0c52bea0) */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: away.y is 0 */
            away.y = 0.0F;
#endif
            away.z = attacker->work->pos.z - work->pos.z;
            njUnitVector(&away);
            mover->spd.x = -(away.x * bounce);
            mover->spd.y = KNOCK_LIFT;
            mover->spd.z = -(away.z * bounce);
            chao_set_behavior(chao, struck);
            if (CHAO_WORK(chao)->profile.parameters.hat) {
                DROP_HAT();
            }
            return TRUE;
        }
    }
    return FALSE;
}
static int struck(struct task *chao)
{
    struct task_work *work = chao->work;
    struct chao_behavior *behavior = &((struct chao_work *)work)->behavior;
    struct mover *mover;
    /* @hack type: delta a Uint32, where the int chao_emotion_add takes would
     * do */
    Uint32 delta;
    switch (behavior->mode) {
    case 0:
        mover = chao->motion.p;
        FACE_VELOCITY();
        chao_set_eye(chao, CHAO_EYE_DISTRESSED);
        chao_show_mark(chao, CHAO_MARK_ANGER, ANGER_STEPS);
        if (RANDOM_UNIT() < 0.5F) {
            chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT, LINK_FRAMES);
            behavior->mode = 1;
        } else {
            chao_link_motion_frames(chao, CHAO_ANIM_THROWN, LINK_FRAMES);
            behavior->mode = 2;
        }
        chao_player_affection(chao, -STRUCK_AFFECTION,
                              -chao_tuning.struck_alignment);
        chao_fear_player(chao, STRUCK_FEAR);
        HURT_FEELINGS(delta);
        break;
    case 1:
        if (work->flags & MOVER_WORK_ON_FLOOR) {
            LAND_DAZED();
            ((struct chao_work *)work)->body_angle = 0;
            chao_show_mark(chao, CHAO_MARK_ANGER, ANGER_STEPS);
            behavior->mode++;
        } else {
            ((struct chao_work *)work)->body_angle += SPIN_STEP;
        }
        break;
    case 2:
        if (work->flags & MOVER_WORK_ON_FLOOR) {
            LAND_DAZED();
            chao_show_mark(chao, CHAO_MARK_ANGER, ANGER_STEPS);
            behavior->mode++;
        }
        break;
    case 3:
        if (--behavior->timer <= 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_thrown_behavior(struct task *chao)
{
    struct task_work *work = chao->work;
    struct chao_behavior *behavior = &((struct chao_work *)work)->behavior;
    struct mover *mover = chao->motion.p;
    /* @hack type: delta a Uint32, where the int chao_emotion_add takes would
     * do */
    Uint32 delta;
    switch (behavior->mode) {
    case 0:
        /* @hack redundant: mover read from chao->motion again, as its
         * declaration did */
        mover = chao->motion.p;
        FACE_VELOCITY();
        chao_set_eye(chao, CHAO_EYE_DISTRESSED);
        chao_show_mark(chao, CHAO_MARK_ANGER, ANGER_STEPS);
        chao_link_motion_frames(chao, CHAO_ANIM_THROWN, LINK_FRAMES);
        behavior->mode = 2;
        chao_player_affection(chao, -THROWN_AFFECTION,
                              -chao_tuning.thrown_alignment);
        chao_fear_player(chao, THROWN_FEAR);
        HURT_FEELINGS(delta);
        break;
    case 2:
        if ((mover->flags & MOVER_AT_WALL) &&
            CHAO_WORK(chao)->profile.parameters.hat) {
            DROP_HAT();
        }
        if (work->flags & MOVER_WORK_ON_FLOOR) {
            LAND_DAZED();
            chao_show_mark(chao, CHAO_MARK_ANGER, ANGER_STEPS);
            behavior->mode++;
        }
        break;
    case 3:
        if (--behavior->timer <= 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
int chao_try_cry(struct task *chao)
{
    chao_behavior_fn current = chao_current_behavior(chao);
    if (current == fall_crying) {
        return FALSE;
    }
    if (collision_first_with_id(chao, GARDEN_JACK_SPRING)) {
        chao_set_timed_behavior(chao, fall_crying, CRY_STEPS);
        return TRUE;
    }
    return FALSE;
}
static int fall_crying(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct task_work *work = chao->work;
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_KUSAI_BATAN);
        chao_set_timed_eye(chao, CHAO_EYE_DISTRESSED, CHAO_FACE_HOLD);
        chao_set_timed_mouth(chao, CHAO_MOUTH_DISTRESSED, CHAO_FACE_HOLD);
        chao_show_mark(chao, CHAO_MARK_ANGER, CRY_ANGER_STEPS);
        behavior->mode++;
        behavior->timer = 0;
        if (RANDOM_UNIT() < 0.5F) {
            sound_play_at(CRY_VOICE, NULL, 0, GARDEN_VOICE_VOLUME, &work->pos);
        } else {
            /* @hack spelling: the pos of chao->work, where work's would do */
            sound_play_at(OTHER_CRY_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                          &chao->work->pos);
        }
        break;
    case 1:
        break;
    }
    chao_slow_on_floor(chao, 0.9F);
    return BEHAVIOR_CONTINUE;
}
