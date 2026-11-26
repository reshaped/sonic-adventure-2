#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/activities.h"
#include "chao/behavior.h"
#include "chao/body_collision.h"
#include "chao/dance.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/garden_field.h"
#include "chao/intention.h"
#include "chao/knowledge.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/tuning.h"
#include "chao/walk.h"
#include "chao/work.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "object/mover.h"
#include "system/task.h"
#include "system/util.h"

enum dance_lesson {
    HAND_DANCE,
    ROUND_DANCE,
    BOX_DANCE,
    GOGO_DANCE,
    RADIO_EXERCISES,
    DANCE_LESSONS
};
/* the kind of the field a dance makes, in which other Chao watch or join */
#define DANCE_FIELD 166
/* the field lies FIELD_AHEAD ahead of the dancer, FIELD_RADIUS wide */
#define FIELD_AHEAD 8.0F
#define FIELD_RADIUS 20.0F
#define DANCE_ENDED() (!garden_hit_kind(chao, CHAO_PUSH_SHAPE, DANCE_FIELD))
/* a bored Chao's boredom, below BOREDOM_FLOOR, counts BOREDOM_BOOST more
 * toward its bid */
#define BOREDOM_FLOOR 7000
#define BOREDOM_BOOST 3000
/* the boredom and loneliness a Chao sheds as it joins a dance, and the
 * loneliness the dancer sheds */
#define JOIN_RELIEF 4000
#define DANCER_RELIEF 3000
/* what *best becomes once a Chao joins or watches */
#define DANCE_BEST 0.99F

static int hand_dance(struct task *chao);
static int round_dance(struct task *chao);
static int box_dance(struct task *chao);
static int gogo_dance(struct task *chao);
static int radio_exercises(struct task *chao);
static int watch_dance_sitting(struct task *chao);
static int watch_dance_standing(struct task *chao);
static chao_behavior_fn dances[DANCE_LESSONS] = {
    hand_dance, round_dance, box_dance, gogo_dance, radio_exercises};

void score_nearby_dance(struct task *chao, Float *best)
{
    Float score = 0.0F;
    Uint32 threshold = chao_tuning.boredom_threshold;
    Uint32 boredom = chao_emotion_get(chao, CHAO_BOREDOM);
    struct task *field = garden_hit_kind(chao, CHAO_PUSH_SHAPE, DANCE_FIELD);
    /* @hack redundant: an array no code uses, which sizes the frame */
#ifdef NONMATCHING
#else
    int lessons[8];
#endif
    chao_behavior_fn current;
    int lesson;
    if (*best < 1.0F && field) {
        if (boredom > threshold) {
            if (boredom < BOREDOM_FLOOR) {
                boredom += BOREDOM_BOOST;
            }
            score = chao_need_above(boredom, threshold);
            score *= chao_tuning.boredom_weight;
            chao_vary_score(&score);
        }
        if (score > *best) {
            lesson = -1;
            if (field->parent) {
                current = chao_current_behavior(field->parent);
                if (current == hand_dance) {
                    if (chao_knows_dance(chao, HAND_DANCE)) {
                        lesson = HAND_DANCE;
                    }
                } else if (current == round_dance) {
                    if (chao_knows_dance(chao, ROUND_DANCE)) {
                        lesson = ROUND_DANCE;
                    }
                } else if (current == box_dance) {
                    if (chao_knows_dance(chao, BOX_DANCE)) {
                        lesson = BOX_DANCE;
                    }
                } else if (current == gogo_dance) {
                    if (chao_knows_dance(chao, GOGO_DANCE)) {
                        lesson = GOGO_DANCE;
                    }
                } else if (current == radio_exercises) {
                    if (chao_knows_dance(chao, RADIO_EXERCISES)) {
                        lesson = RADIO_EXERCISES;
                    }
                }
            }
            /* @bug logic: a Chao never joins the hand dance, though it knows
             * it, since the test takes only a lesson above 0 (0x0c52c798) */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: a lesson of 0 joins
             * too */
            if (lesson >= 0) {
#else
            if (lesson > 0) {
#endif
                mover_set_target(chao, &field->work->pos);
                chao_set_behavior(chao, chao_face_point_behavior);
                chao_queue_behavior(chao, dances[lesson]);
                chao_emotion_add(chao, CHAO_BOREDOM, -JOIN_RELIEF);
                chao_emotion_add(chao, CHAO_LONELINESS, -JOIN_RELIEF);
                chao_clear_intention(chao);
                if (field->parent) {
                    chao_emotion_add(field->parent, CHAO_LONELINESS,
                                     -DANCER_RELIEF);
                }
            } else {
                if (field->parent) {
                    mover_set_target(chao, &field->parent->work->pos);
                }
                chao_set_behavior(chao, chao_face_point_behavior);
                chao_queue_behavior(chao, chao_sit_down_behavior);
                if (RANDOM_UNIT() < 0.5F) {
                    chao_queue_behavior(chao, watch_dance_sitting);
                } else {
                    chao_queue_behavior(chao, watch_dance_standing);
                }
            }
            *best = DANCE_BEST;
        }
    }
}

Bool chao_start_dance(struct task *chao)
{
    struct task_work *work = chao->work;
    /* @hack type: lessons holds eight, where DANCE_LESSONS would do */
    int lessons[8];
    NJS_POINT3 center;
    int lesson;
    int i;
    int count;
    Angle yaw;
    count = 0;
    collision_restart_scans();
    if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, DANCE_FIELD)) {
        return FALSE;
    }
    for (i = 0; i < DANCE_LESSONS; i++) {
        if (chao_knows_dance(chao, i)) {
            lessons[count] = i;
            count++;
        }
    }
    if (count > 0) {
        lesson = lessons[(int)(rand() * (count - 0.00001F) * RAND_SCALE)];
        yaw = work->ang.y;
        center.x = work->pos.x + (njSin(yaw) * FIELD_AHEAD);
        center.y = work->pos.y;
        center.z = work->pos.z + (njCos(yaw) * FIELD_AHEAD);
        chao_set_behavior(chao, dances[lesson]);
        garden_create_timed_field(chao, DANCE_FIELD, &center, FIELD_RADIUS,
                                  RANDOM_COUNT(1800, 2401));
        return TRUE;
    }
    return FALSE;
}
static int hand_dance(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    Float choice;
    switch (behavior->mode) {
    case 0:
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        choice = RANDOM_UNIT();
        if (choice < 0.25F) {
            chao_link_motion(chao, CHAO_ANIM_HAND_DANCE_UP);
        } else if (choice < 0.5F) {
            chao_link_motion(chao, CHAO_ANIM_HAND_DANCE_DOWN);
        } else if (choice < 0.75F) {
            chao_link_motion(chao, CHAO_ANIM_HAND_DANCE_RIGHT);
        } else {
            chao_link_motion(chao, CHAO_ANIM_HAND_DANCE_LEFT);
        }
        behavior->mode++;
        /* fall through */
    case 1:
        if (chao_motion_held(chao)) {
            behavior->mode = 0;
        }
        break;
    }
    return DANCE_ENDED();
}
static int round_dance(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    Float choice;
    switch (behavior->mode) {
    case 0:
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        choice = RANDOM_UNIT();
        if (choice < 0.35F) {
            chao_link_motion(chao, CHAO_ANIM_ROUND_DANCE_LEFT);
        } else if (choice < 0.7F) {
            chao_link_motion(chao, CHAO_ANIM_ROUND_DANCE_RIHGT);
        } else if (choice < 0.85F) {
            chao_link_motion(chao, CHAO_ANIM_ROUND_DANCE_LEFT_TURN);
        } else {
            chao_link_motion(chao, CHAO_ANIM_ROUND_DANCE_RIHGT_TURN);
        }
        behavior->mode++;
        /* fall through */
    case 1:
        if (chao_motion_held(chao)) {
            behavior->mode = 0;
        }
        break;
    }
    return DANCE_ENDED();
}
static int box_dance(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_link_motion(chao, CHAO_ANIM_BOX_DANCE);
        behavior->mode++;
        /* fall through */
    case 1:
        /* @hack redundant: an empty arm for mode 1 */
        break;
    }
    return DANCE_ENDED();
}
static int gogo_dance(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_link_motion(chao, CHAO_ANIM_GOGO_DANCE);
        behavior->mode++;
        /* fall through */
    case 1:
        if (chao_motion_ended(chao) && RANDOM_UNIT() < 0.1F) {
            chao_link_motion(chao, CHAO_ANIM_GOGO_DANCE2);
        }
        break;
    }
    return DANCE_ENDED();
}
#define CLEAR_COUNTS()                                                         \
    behavior->timer = 0;                                                       \
    behavior->count = 0

static int radio_exercises(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_link_motion(chao, CHAO_ANIM_TAISOU);
        behavior->mode++;
        CLEAR_COUNTS();
        /* fall through */
    case 1:
        if (chao_motion_ended(chao) && RANDOM_UNIT() < 0.3F) {
            chao_link_motion(chao, CHAO_ANIM_TAISOU_YOKO2KOKI);
            behavior->mode++;
            CLEAR_COUNTS();
        }
        break;
    case 2:
        if (chao_motion_ended(chao)) {
            behavior->count++;
            if (behavior->count >= 2) {
                chao_link_motion(chao, CHAO_ANIM_TAISOU_KOKI_L);
                behavior->mode++;
                CLEAR_COUNTS();
            } else {
                chao_link_motion(chao, CHAO_ANIM_TAISOU_KOKI_R);
            }
        }
        break;
    case 3:
        if (chao_motion_ended(chao)) {
            behavior->count++;
            if (behavior->count >= 1) {
                chao_link_motion(chao, CHAO_ANIM_TAISOU_MAE);
                behavior->mode++;
                CLEAR_COUNTS();
            } else {
                chao_link_motion(chao, CHAO_ANIM_TAISOU_KOKI_L);
            }
        }
        break;
    case 4:
        if (chao_motion_ended(chao)) {
            behavior->count++;
            if (behavior->count >= 1) {
                chao_link_motion(chao, CHAO_ANIM_TAISOU);
                behavior->mode = 1;
                CLEAR_COUNTS();
            } else {
                chao_link_motion(chao, CHAO_ANIM_TAISOU_MAE);
            }
        }
        break;
    }
    return DANCE_ENDED();
}
static int watch_dance_sitting(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        if (RANDOM_UNIT() < 0.5F) {
            chao_link_motion(chao, CHAO_ANIM_MUSIC_SIT);
        } else {
            chao_link_motion(chao, CHAO_ANIM_HIMA_SIT_GUDE);
        }
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_set_timed_mouth(chao, CHAO_MOUTH_GLAD, CHAO_FACE_HOLD);
        behavior->timer = RANDOM_COUNT(300, 301);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- <= 0) {
            behavior->mode = 0;
        }
        if (DANCE_ENDED()) {
            chao_link_motion(chao, CHAO_ANIM_HAKUSYU_SIT);
            behavior->timer = RANDOM_COUNT(180, 121);
            behavior->mode = 2;
        }
        break;
    case 2:
        if (behavior->timer-- <= 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
static int watch_dance_standing(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_MUSIC_STAND);
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_set_timed_mouth(chao, CHAO_MOUTH_GLAD, CHAO_FACE_HOLD);
        behavior->timer = RANDOM_COUNT(300, 301);
        behavior->mode++;
        /* fall through */
    case 1:
        if (DANCE_ENDED()) {
            chao_link_motion(chao, CHAO_ANIM_HAKUSYU_STAND);
            behavior->timer = RANDOM_COUNT(180, 121);
            behavior->mode = 2;
        }
        break;
    case 2:
        if (behavior->timer-- <= 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
