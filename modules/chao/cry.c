#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/behavior.h"
#include "chao/cry.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/intention.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/sound.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chao/tears.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

#define SORROW_RELIEF 10
/* the sorrow past which a Chao may sob rather than sigh */
#define SOB_SORROW 120
/* a sobbing Chao sheds tears TEAR_FRAMES frames in each TEAR_PERIOD. it may
 * wail once each WAIL_PERIOD frames, with a chance of WAIL_CHANCE */
#define TEAR_PERIOD 8
#define TEAR_FRAMES 4
#define WAIL_PERIOD 180
#define WAIL_CHANCE 0.3F
#define VOICE(id)                                                              \
    sound_play_at((id), NULL, 0, GARDEN_VOICE_VOLUME, &chao->work->pos)

static int choose_cry(struct task *chao);

void score_sorrow(struct task *chao, Float *best)
{
    struct chao_tuning *tuning = &chao_tuning;
    Float score;
    Uint32 sorrow = chao_emotion_get(chao, CHAO_SORROW);

    if (sorrow > tuning->sorrow_threshold) {
        score = 1.0F - ((Float)(chao_emotion_get(chao, CHAO_AGGRESSION) + 100) *
                        (1.0F - tuning->sorrow_weight) * 0.005F);
        if (score > *best) {
            chao_set_behavior(chao, choose_cry);
            chao_clear_intention(chao);
            *best = score;
        }
    }
}

static int choose_cry(struct task *chao)
{
    int sorrow = chao_emotion_get(chao, CHAO_SORROW);

    /* @hack spelling: a random draw in each arm of the test of sorrow, where
     * one draw before the test would do */
    if (sorrow > SOB_SORROW) {
        if (RANDOM_UNIT() < 0.5F) {
            chao_set_behavior(chao, chao_cry_behavior);
        } else {
            chao_set_behavior(chao, chao_sob_behavior);
        }
    } else {
        if (RANDOM_UNIT() < 0.5F) {
            chao_set_behavior(chao, chao_cry_behavior);
        } else {
            chao_set_behavior(chao, chao_sigh_behavior);
        }
    }
    return BEHAVIOR_CONTINUE;
}

int chao_sigh_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int pose;

    switch (behavior->mode) {
    case 0:
        /* @hack spelling: the pose held in pose, where the switch would read
         * chao_motion_pose itself */
        pose = chao_motion_pose(chao);
        switch (pose) {
        case CHAO_SITTING:
        case CHAO_ON_BACK:
        case CHAO_ON_FRONT:
            chao_link_motion_frames(chao, CHAO_ANIM_TAMEIKI_SIT, 35);
            break;
        case CHAO_STANDING:
        default:
            chao_link_motion_frames(chao, CHAO_ANIM_TAMEIKI_STAND, 30);
            break;
        }
        chao_set_eye(chao, 3);
        chao_set_mouth(chao, 4);
        behavior->timer = RANDOM_COUNT(180, 121);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- <= 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_cry_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int pose;
    Float choice;

    switch (behavior->mode) {
    case 0:
        /* @hack spelling: the pose held in pose, where the switch would read
         * chao_motion_pose itself */
        pose = chao_motion_pose(chao);
        switch (pose) {
        case CHAO_SITTING:
        case CHAO_ON_BACK:
        case CHAO_ON_FRONT:
            choice = RANDOM_UNIT();
            if (choice < 0.3F) {
                chao_link_motion_frames(chao, CHAO_ANIM_MESO_SIT_START, 35);
            } else if (choice < 0.6F) {
                chao_link_motion_frames(chao, CHAO_ANIM_MESO_SIT_MEKOSURI, 35);
            } else if (choice < 0.8F) {
                chao_link_motion_frames(chao, CHAO_ANIM_MESO_SIT_MEKOSURI_HIKKU,
                                        35);
            } else {
                chao_link_motion_frames(chao, CHAO_ANIM_MESO_SIT_HIKKU_TATE,
                                        35);
            }
            break;
        case CHAO_STANDING:
        default:
            choice = RANDOM_UNIT();
            if (choice < 0.3F) {
                chao_link_motion_frames(chao, CHAO_ANIM_MESO_STAND_START, 35);
            } else if (choice < 0.6F) {
                chao_link_motion_frames(chao, CHAO_ANIM_MESO_STAND_MEKOSURI,
                                        35);
            } else if (choice < 0.8F) {
                chao_link_motion_frames(
                    chao, CHAO_ANIM_MESO_STAND_MEKOSURI_HIKKU, 35);
            } else {
                chao_link_motion_frames(chao, CHAO_ANIM_MESO_STAND_HIKKU_TATE,
                                        35);
            }
            break;
        }
        if (RANDOM_UNIT() < 0.5F) {
            chao_set_eye(chao, 3);
        } else {
            chao_set_eye(chao, CHAO_EYE_DISTRESSED);
        }
        choice = RANDOM_UNIT();
        if (choice < 0.5F) {
            chao_set_mouth(chao, 4);
        } else {
            chao_set_mouth(chao, 6);
        }
        behavior->timer = RANDOM_COUNT(100, 101);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- <= 0) {
            chao_emotion_add(chao, CHAO_SORROW, -SORROW_RELIEF);
            if (chao_emotion_get(chao, CHAO_SORROW) < SORROW_RELIEF) {
                return BEHAVIOR_FINISH;
            }
            behavior->mode = 0;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_sob_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int pose;
    struct chao_shape_work *shape;
    NJS_POINT3 position;
    NJS_VECTOR speed;
    Float choice;

    switch (behavior->mode) {
    case 0:
        /* @hack spelling: the pose held in pose, where the switch would read
         * chao_motion_pose itself */
        pose = chao_motion_pose(chao);
        switch (pose) {
        case CHAO_SITTING:
        case CHAO_ON_BACK:
        case CHAO_ON_FRONT:
            choice = RANDOM_UNIT();
            if (choice < 0.4F) {
                chao_link_motion_frames(chao, CHAO_ANIM_CRY_SIT_START, 35);
            } else if (choice < 0.8F) {
                chao_link_motion_frames(chao, CHAO_ANIM_CRY_SIT_SHIKUSHIKU, 35);
            } else {
                chao_link_motion_frames(chao, CHAO_ANIM_CRY_SIT_JITABATA, 35);
            }
            break;
        case CHAO_STANDING:
        default:
            choice = RANDOM_UNIT();
            if (choice < 0.5F) {
                chao_link_motion_frames(chao, CHAO_ANIM_CRY_STAND_START, 35);
            } else {
                chao_link_motion_frames(chao, CHAO_ANIM_CRY_STAND_HIKKU2SHIKU,
                                        35);
            }
            break;
        }
        if (RANDOM_UNIT() < 0.5F) {
            chao_set_eye(chao, 3);
        } else {
            chao_set_eye(chao, CHAO_EYE_DISTRESSED);
        }
        choice = RANDOM_UNIT();
        if (choice < 0.4F) {
            chao_set_mouth(chao, 4);
        } else {
            chao_set_mouth(chao, 6);
        }
        behavior->timer = RANDOM_COUNT(100, 101);
        behavior->mode++;
        /* fall through */
    case 1:
        if (chao_motion_held(chao) || behavior->timer-- <= 0) {
            chao_emotion_add(chao, CHAO_SORROW, -SORROW_RELIEF);
            if (chao_emotion_get(chao, CHAO_SORROW) < SORROW_RELIEF) {
                return BEHAVIOR_FINISH;
            }
            behavior->mode = 0;
        }
        break;
    }

    if (behavior->timer % TEAR_PERIOD < TEAR_FRAMES) {
        shape = &CHAO_WORK(chao)->shape;
        CHAO_SHED_TEAR(shape, 0, position, speed);
        CHAO_SHED_TEAR(shape, 1, position, speed);
    }
    if (CHAO_WORK(chao)->frames % WAIL_PERIOD == 0 &&
        RANDOM_UNIT() < WAIL_CHANCE) {
        VOICE(0x6001);
    }
    return BEHAVIOR_CONTINUE;
}
