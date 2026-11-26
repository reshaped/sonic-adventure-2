/* a garden Chao's walks, turns and pauses, and the field that switches on a
 * toy */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/behavior.h"
#include "chao/face.h"
#include "chao/garden_field.h"
#include "chao/garden_toys.h"
#include "chao/icon.h"
#include "chao/idle.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/tuning.h"
#include "chao/walk.h"
#include "chao/work.h"
#include "chao/world.h"
#include "object/mover.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the steps a walk runs before it gives way to chao_idle_behavior */
#define WALK_LIMIT 1800
#define TURN_STEP ((Angle)(2 * ANGLE_PER_DEGREE))
#define AIM_TOLERANCE ((Angle)ANGLE_PER_DEGREE)
/* a Chao with fewer run points than WALK_POINTS crawls */
#define WALK_POINTS 150
/* a walk waits more than WALK_START steps to start, and ends within
 * WALK_REACH of where it heads */
#define WALK_START 30
#define WALK_REACH 4.0F
#define WALK_TURN 0x180
#define FACE_TURN 0x200
#define PAUSE_FRAMES 120
/* after SWITCH_FRAMES frames of its motion, a Chao places the field that
 * switches on a toy. the field lies SWITCH_AHEAD before the Chao, and lasts
 * SWITCH_FIELD_FRAMES frames */
#define SWITCH_AHEAD 3.0F
#define SWITCH_FIELD_FRAMES 10
#define SWITCH_FRAMES 60

int chao_pause_with_mark_behavior(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_behavior *behavior = &work->behavior;

    switch (behavior->mode) {
    case 0:
        behavior->limit = WALK_LIMIT;
        chao_show_mark(chao, CHAO_MARK_EXCLAMATION, 180);
        behavior->mode++;
        behavior->timer = 0;
        behavior->count = 0;
        break;
    case 1:
        if (behavior->timer++ > PAUSE_FRAMES) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    if (--behavior->limit <= 0) {
        chao_set_behavior(chao, chao_idle_behavior);
    }
    return BEHAVIOR_CONTINUE;
}

int chao_face_point_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);

    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_ARUKU, 15);
        behavior->mode++;
        behavior->timer = 60;
        break;
    }
    if (mover_turn_to_target_by(chao, FACE_TURN) < AIM_TOLERANCE) {
        return BEHAVIOR_FINISH;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_walk_to_point_behavior(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_behavior *behavior = &work->behavior;
    Float distance;

    switch (behavior->mode) {
    case 0:
        chao_set_eye(chao, CHAO_EYE_GLAD);
        behavior->limit = WALK_LIMIT;
        if (behavior->timer++ > WALK_START) {
            if (chao_skill_points(chao, CHAO_RUN) < WALK_POINTS) {
                chao_link_motion(chao, CHAO_ANIM_HAIHAI);
            } else {
                chao_link_motion(chao, CHAO_ANIM_ARUKU);
            }
            behavior->mode++;
        }
        break;
    case 1:
        mover_turn_to_target_by(chao, WALK_TURN);
        chao_set_forward_acceleration(chao, chao_tuning.walk_speed * 0.8F);
        distance = mover_target_distance(chao);
        /* mover_target_distance is never negative, so this test never holds
         * here. the same test in chao_walk_to_target_behavior catches a
         * missing target */
        if (distance < 0.0F) {
            chao_set_behavior(chao, chao_idle_behavior);
        } else if (distance < WALK_REACH) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    chao_set_motion_speed(chao, 1.5F);
    if (--behavior->limit <= 0) {
        chao_set_behavior(chao, chao_idle_behavior);
    }
    return BEHAVIOR_CONTINUE;
}

int chao_face_target_behavior(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_behavior *behavior = &work->behavior;

    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_ARUKU, 15);
        behavior->timer = 90;
        behavior->mode = 1;
        break;
    case 1:
        if (behavior->timer-- > 0) {
            garden_turn_to_target(chao, FACE_TURN);
        } else {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    chao_set_motion_speed(chao, 1.6F);
    return BEHAVIOR_CONTINUE;
}

/* the body of a turn: left or right at even odds, TURN_STEP a step, for
 * duration steps. duration is evaluated after the direction is drawn, so a
 * random duration takes the second rand() */
#define RUN_TURN(chao, behavior, work, duration)                               \
    switch ((behavior)->mode) {                                                \
    case 0:                                                                    \
        chao_link_motion_frames((chao), CHAO_ANIM_ARUKU, 15);                  \
        if (RANDOM_UNIT() < 0.5F) {                                            \
            (behavior)->mode = 1;                                              \
        } else {                                                               \
            (behavior)->mode = 2;                                              \
        }                                                                      \
        /* @hack type: duration cast to Uint16, where its int would do */      \
        (behavior)->timer = (Uint16)(duration);                                \
        break;                                                                 \
    case 1:                                                                    \
        if ((behavior)->timer-- > 0) {                                         \
            (work)->ang.y += TURN_STEP;                                        \
        } else {                                                               \
            return BEHAVIOR_FINISH;                                            \
        }                                                                      \
        break;                                                                 \
    case 2:                                                                    \
        if ((behavior)->timer-- > 0) {                                         \
            (work)->ang.y -= TURN_STEP;                                        \
        } else {                                                               \
            return BEHAVIOR_FINISH;                                            \
        }                                                                      \
        break;                                                                 \
    }

int chao_turn_partway_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    struct task_work *work = chao->work;

    RUN_TURN(chao, behavior, work, 45 + (int)(rand() * (46.0F * RAND_SCALE)));
    chao_set_motion_speed(chao, 1.6F);
    return BEHAVIOR_CONTINUE;
}

int chao_turn_in_place_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    struct task_work *work = chao->work;

    RUN_TURN(chao, behavior, work, 90);
    chao_set_motion_speed(chao, 1.6F);
    return BEHAVIOR_CONTINUE;
}

int chao_turn_around_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    struct task_work *work = chao->work;

    RUN_TURN(chao, behavior, work, (int)(rand() * (91.0F * RAND_SCALE)));
    chao_set_motion_speed(chao, 1.6F);
    return BEHAVIOR_CONTINUE;
}

int chao_walk_to_target_behavior(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_behavior *behavior = &work->behavior;
    Float distance;

    switch (behavior->mode) {
    case 0:
        chao_set_eye(chao, CHAO_EYE_GLAD);
        behavior->limit = WALK_LIMIT;
        if (behavior->timer++ > WALK_START) {
            chao_link_motion(chao, CHAO_ANIM_ARUKU);
            behavior->mode++;
        }
        break;
    case 1:
        garden_turn_to_target(chao, WALK_TURN);
        chao_set_forward_acceleration(chao, chao_tuning.walk_speed * 0.8F);
        distance = garden_target_distance(chao);
        if (distance < 0.0F) {
            chao_set_behavior(chao, chao_idle_behavior);
        } else if (distance < WALK_REACH) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    chao_set_motion_speed(chao, 1.5F);
    if (--behavior->limit <= 0) {
        chao_set_behavior(chao, chao_idle_behavior);
    }
    return BEHAVIOR_CONTINUE;
}

int chao_switch_on_toy_behavior(struct task *chao)
{
    struct task_work *work = chao->work;
    struct chao_behavior *behavior = &((struct chao_work *)work)->behavior;
    NJS_POINT3 position;

    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_MOTU);
        behavior->mode++;
        behavior->timer = 0;
        /* fall through */
    case 1:
        if (behavior->timer++ > SWITCH_FRAMES) {
            position.x = work->pos.x + (njSin(work->ang.y) * SWITCH_AHEAD);
            position.y = work->pos.y + 1.0F;
            position.z = work->pos.z + (njCos(work->ang.y) * SWITCH_AHEAD);
            garden_create_timed_field(chao, GARDEN_TOY_ACTIVATE, &position,
                                      3.5F, SWITCH_FIELD_FRAMES);
            return BEHAVIOR_FINISH;
        }
        break;
    }
    chao_slow_on_floor(chao, 0.9F);
    return BEHAVIOR_CONTINUE;
}
