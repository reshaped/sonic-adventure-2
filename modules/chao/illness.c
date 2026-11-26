/* the symptoms of a garden Chao's six illnesses. illness_actions holds them in
 * the order of emotions.illness. a Chao has each illness whose entry in
 * emotions.illness is below 0 */
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/behavior.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/needs.h"
#include "chao/work.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

#define SYMPTOM_SOUND(id, volume)                                              \
    sound_play_at((id), NULL, 0, (volume), &chao->work->pos)
/* a symptom shows where a roll from 0 to SYMPTOM_ROLL passes the Chao's
 * condition plus ROLL_MARGIN. that is a third of the time at condition 0, and
 * never at full condition. it replaces any earlier bid below 1 and bids
 * SYMPTOM_BID */
#define SYMPTOM_ROLL 30000.0F
#define ROLL_MARGIN 20000
#define SYMPTOM_BID 0.99F
#define COUGH_VOICE 0x606a
#define SNEEZE_VOICE 0x606b
#define SCRATCH_SOUND 0x1015
#define ITCH_VOICE 0x602c
#define UNWELL_SOUND 0x1010
#define HICCUP_VOICE 0x606e
#define SLUMP_VOICE 0x6022
#define OTHER_SLUMP_VOICE 0x6023
#define SYMPTOM_VOLUME 110
#define UNWELL_VOLUME 70
/* the frames each symptom's motion links over */
#define LINK_FRAMES 40
/* a sneeze sounds SNEEZE_STEP steps into each round, and a hiccup HICCUP_STEP
 */
#define SNEEZE_STEP 180
#define HICCUP_STEP 20
/* the steps between a symptom's timed rolls to sound, to end or to go round
 * again */
#define SYMPTOM_INTERVAL 60
/* a scratch lasts SCRATCH_STEPS and a random part of SCRATCH_SPAN, or until
 * its motion holds */
#define SCRATCH_STEPS 60
#define SCRATCH_SPAN 61.0F
/* the counts a symptom's rounds are tested against, and a scratch's and
 * sitting unwell's. only MOST_SCRATCHES ever ends one */
#define MOST_ROUNDS 10
#define MOST_SCRATCHES 13
#define MOST_UNWELL_ROUNDS 2
/* a Chao sitting unwell rests more than UNWELL_REST steps after its sound, and
 * goes round again */
#define UNWELL_REST 60

static int cough(struct task *chao);
static int sneeze(struct task *chao);
static int scratch(struct task *chao);
static int sit_unwell(struct task *chao);
static int hiccup(struct task *chao);
static int slump(struct task *chao);
chao_behavior_fn illness_actions[CHAO_ILLNESSES] = {cough,      sneeze, scratch,
                                                    sit_unwell, hiccup, slump};

void score_condition(struct task *chao, Float *best)
{
    struct chao_emotions *emotions = &CHAO_WORK(chao)->profile.emotions;
    Sint8 *illness;
    chao_behavior_fn *action;

    if (*best >= 1.0F) {
        return;
    }
    /* @hack spelling: action stepped by an assignment, where action++ would
     * do */
    for (action = illness_actions, illness = emotions->illness;
         illness < emotions->illness + CHAO_ILLNESSES;
         illness++, action = action + 1) {
        if (*illness < 0) {
            if (rand() * (SYMPTOM_ROLL * RAND_SCALE) >
                chao_emotion_get(chao, CHAO_CONDITION) + ROLL_MARGIN) {
                chao_set_behavior(chao, *action);
                *best = SYMPTOM_BID;
            }
        }
    }
}

#define SICK_EYES(chao)                                                        \
    {                                                                          \
        if (RANDOM_UNIT() < 0.7F) {                                            \
            chao_set_timed_eye(chao, CHAO_EYE_DISTRESSED, CHAO_FACE_HOLD);     \
        } else {                                                               \
            chao_set_timed_eye(chao, 3, CHAO_FACE_HOLD);                       \
        }                                                                      \
    }

/* link seated where the Chao sits or lies, and standing in any other pose */
#define SYMPTOM_MOTION(chao, seated, standing)                                 \
    {                                                                          \
        int pose = chao_motion_pose(chao);                                     \
        switch (pose) {                                                        \
        case CHAO_SITTING:                                                     \
        case CHAO_ON_BACK:                                                     \
        case CHAO_ON_FRONT:                                                    \
            chao_link_motion_frames(chao, seated, LINK_FRAMES);                \
            break;                                                             \
        case CHAO_STANDING:                                                    \
        default:                                                               \
            chao_link_motion_frames(chao, standing, LINK_FRAMES);              \
            break;                                                             \
        }                                                                      \
    }

static int cough(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int pose;
    switch (behavior->mode) {
    case 0:
        pose = chao_motion_pose(chao);
        switch (pose) {
        case CHAO_SITTING:
        case CHAO_ON_BACK:
        case CHAO_ON_FRONT:
            if (RANDOM_UNIT() < 0.5F) {
                chao_link_motion_frames(chao, CHAO_ANIM_SEKI_SIT_ALL,
                                        LINK_FRAMES);
            } else {
                chao_link_motion_frames(chao, CHAO_ANIM_SEKI_SIT_KEHON,
                                        LINK_FRAMES);
            }
            break;
        case CHAO_STANDING:
        default:
            if (RANDOM_UNIT() < 0.5F) {
                chao_link_motion_frames(chao, CHAO_ANIM_SEKI_STAND_ALL,
                                        LINK_FRAMES);
            } else {
                chao_link_motion_frames(chao, CHAO_ANIM_SEKI_STAND_KEHON,
                                        LINK_FRAMES);
            }
            break;
        }
        SICK_EYES(chao);
        if (RANDOM_UNIT() < 0.7F) {
            chao_set_timed_mouth(chao, 2, CHAO_FACE_HOLD);
        } else {
            chao_set_timed_mouth(chao, 4, CHAO_FACE_HOLD);
        }
        behavior->mode++;
        behavior->timer = 0;
        behavior->count = 0;
        /* fall through */
    case 1:
        if (chao_motion_held(chao)) {
            /* @bug logic: the count never ends the cough. mode 0 sets it
             * to 0 at 0x0c52ef14 each round, so it is 0 at the test at
             * 0x0c52ef36 */
            if (RANDOM_UNIT() < 0.1F || behavior->count++ > MOST_ROUNDS) {
                return BEHAVIOR_FINISH;
            }
            behavior->mode = 0;
        }
        if (behavior->timer++ % SYMPTOM_INTERVAL == 0 && RANDOM_UNIT() < 0.4F) {
            SYMPTOM_SOUND(COUGH_VOICE, SYMPTOM_VOLUME);
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

static int sneeze(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        SYMPTOM_MOTION(chao, CHAO_ANIM_KUSYAMI_SIT, CHAO_ANIM_KUSYAMI_STAND);
        SICK_EYES(chao);
        chao_set_timed_mouth(chao, CHAO_MOUTH_DISTRESSED, CHAO_FACE_HOLD);
        behavior->mode++;
        behavior->timer = 0;
        behavior->count = 0;
        /* fall through */
    case 1:
        if (chao_motion_held(chao) && behavior->timer % SYMPTOM_INTERVAL == 0 &&
            RANDOM_UNIT() < 0.3F) {
            /* @bug logic: the count never ends the sneeze. mode 0 sets it
             * to 0 at 0x0c52f084 each round, so it is 0 at the test at
             * 0x0c52f0cc */
            if (RANDOM_UNIT() < 0.4F || behavior->count++ > MOST_ROUNDS) {
                return BEHAVIOR_FINISH;
            }
            behavior->mode = 0;
        }
        if (behavior->timer++ == SNEEZE_STEP) {
            SYMPTOM_SOUND(SNEEZE_VOICE, SYMPTOM_VOLUME);
            chao_set_timed_mouth(chao, 4, CHAO_FACE_HOLD);
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

static int scratch(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int pose;
    switch (behavior->mode) {
    case 0:
        /* @hack spelling: a switch on the pose whose every case scratches
         * alike, where the scratch alone would do */
        pose = chao_motion_pose(chao);
        switch (pose) {
        case CHAO_SITTING:
        case CHAO_ON_BACK:
        case CHAO_ON_FRONT:
        case CHAO_STANDING:
        default:
            if (RANDOM_UNIT() < 0.2F) {
                chao_link_motion_frames(chao, CHAO_ANIM_STAND, LINK_FRAMES);
            } else {
                chao_link_motion_frames(chao, CHAO_ANIM_PORIPORI_TATI,
                                        LINK_FRAMES);
                SYMPTOM_SOUND(SCRATCH_SOUND, SYMPTOM_VOLUME);
                behavior->timer = RANDOM_COUNT(SCRATCH_STEPS, SCRATCH_SPAN);
            }
            break;
        }
        chao_set_timed_eye(chao, 3, CHAO_FACE_HOLD);
        chao_set_timed_mouth(chao, CHAO_MOUTH_DISTRESSED, CHAO_FACE_HOLD);
        if (RANDOM_UNIT() < 0.1F) {
            SYMPTOM_SOUND(ITCH_VOICE, SYMPTOM_VOLUME);
        }
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- <= 0 || chao_motion_held(chao)) {
            if (RANDOM_UNIT() < 0.2F || behavior->count++ > MOST_SCRATCHES) {
                return BEHAVIOR_FINISH;
            }
            behavior->mode = 0;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

static int sit_unwell(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        SYMPTOM_MOTION(chao, CHAO_ANIM_SITDOWN_SLOW, CHAO_ANIM_STAND);
        chao_set_timed_eye(chao, CHAO_EYE_HALF_SHUT, CHAO_FACE_HOLD);
        if (RANDOM_UNIT() < 0.4F) {
            chao_set_timed_mouth(chao, CHAO_MOUTH_DISTRESSED, CHAO_FACE_HOLD);
        } else {
            chao_set_timed_mouth(chao, 0, CHAO_FACE_HOLD);
        }
        behavior->mode++;
        behavior->timer = 0;
        behavior->count = 0;
        /* fall through */
    case 1:
        if (behavior->timer++ % SYMPTOM_INTERVAL == 0 && RANDOM_UNIT() < 0.3F) {
            /* @bug logic: the count never ends sitting unwell. mode 0 sets
             * it to 0 at 0x0c52f392 each round, so it is 0 at the test at
             * 0x0c52f3d2 */
            if (RANDOM_UNIT() < 0.4F ||
                behavior->count++ > MOST_UNWELL_ROUNDS) {
                return BEHAVIOR_FINISH;
            }
            behavior->mode = 2;
            behavior->timer = 0;
            behavior->count = 0;
            SYMPTOM_SOUND(UNWELL_SOUND, UNWELL_VOLUME);
        }
        break;
    case 2:
        if (behavior->timer++ > UNWELL_REST) {
            behavior->mode = 0;
            behavior->timer = 0;
            behavior->count = 0;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

static int hiccup(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        SYMPTOM_MOTION(chao, CHAO_ANIM_SYAKKURI_SIT, CHAO_ANIM_SYAKKURI_STAND);
        SICK_EYES(chao);
        if (RANDOM_UNIT() < 0.5F) {
            chao_set_timed_mouth(chao, 4, CHAO_FACE_HOLD);
        } else {
            chao_set_timed_mouth(chao, CHAO_MOUTH_DISTRESSED, CHAO_FACE_HOLD);
        }
        behavior->mode++;
        behavior->timer = 0;
        behavior->count = 0;
        /* fall through */
    case 1:
        if (behavior->timer++ % SYMPTOM_INTERVAL == 0 &&
            chao_motion_held(chao) && RANDOM_UNIT() < 0.5F) {
            /* @bug logic: the count never ends the hiccup. mode 0 sets it
             * to 0 at 0x0c52f516 each round, so it is 0 at the test at
             * 0x0c52f562 */
            if (RANDOM_UNIT() < 0.3F || behavior->count++ > MOST_ROUNDS) {
                return BEHAVIOR_FINISH;
            }
            behavior->mode = 0;
        }
        if (behavior->timer == HICCUP_STEP) {
            SYMPTOM_SOUND(HICCUP_VOICE, SYMPTOM_VOLUME);
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

static int slump(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        SYMPTOM_MOTION(chao, CHAO_ANIM_GUTA_A, CHAO_ANIM_GOO);
        SICK_EYES(chao);
        chao_set_timed_mouth(chao, CHAO_MOUTH_DISTRESSED, CHAO_FACE_HOLD);
        if (RANDOM_UNIT() < 0.3F) {
            SYMPTOM_SOUND(SLUMP_VOICE, SYMPTOM_VOLUME);
        } else {
            SYMPTOM_SOUND(OTHER_SLUMP_VOICE, SYMPTOM_VOLUME);
        }
        behavior->mode++;
        behavior->timer = 0;
        behavior->count = 0;
        /* fall through */
    case 1:
        if (behavior->timer++ % SYMPTOM_INTERVAL == 0 && RANDOM_UNIT() < 0.3F) {
            /* @bug logic: the count never ends the slump. mode 0 sets it
             * to 0 at 0x0c52f6c2 each round, so it is 0 at the test at
             * 0x0c52f704 */
            if (RANDOM_UNIT() < 0.4F || behavior->count++ > MOST_ROUNDS) {
                return BEHAVIOR_FINISH;
            }
            behavior->mode = 0;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
