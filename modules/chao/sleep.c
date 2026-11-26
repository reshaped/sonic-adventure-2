/* a garden Chao's drowsiness, sleep and waking */
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/behavior.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/icon.h"
#include "chao/intention.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/perception.h"
#include "chao/race_chao.h"
#include "chao/sleep.h"
#include "chao/sound.h"
#include "chao/tree.h"
#include "chao/tuning.h"
#include "chao/walk.h"
#include "chao/work.h"
#include "chao/world.h"
#include "object/mover.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

#define DROWSY_COOLDOWN 15
#define SLEEP_INTERVAL 1000
#define WAKE_SLEEPINESS 1000
/* a sleepy Chao walks to a grown tree within TREE_REACH to sleep under it */
#define TREE_REACH 30.0F
/* a sitting Chao more tired than SEATED_TIREDNESS may sleep where it sits */
#define SEATED_TIREDNESS 7000
/* a sleeping Chao may make a sound once each VOICE_PERIOD frames */
#define VOICE_PERIOD 120
/* a sleeping Chao's voices, by pose, each with a rarer second. a turn over
 * takes TURN_VOICE or OTHER_TURN_VOICE */
#define SPRAWLED_VOICE 0x6008
#define SPRAWLED_RARE_VOICE 0x6009
#define PLAIN_VOICE 0x600b
#define PLAIN_RARE_VOICE 0x600a
#define TURN_VOICE 0x600f
#define OTHER_TURN_VOICE 0x6011
#define YAWN_VOICE 0x6005
#define STRETCH_VOICE 0x600e

/* behavior->phase in recover_in_sleep */
enum sleep_phase { SLEEP_DEEPENING, SLEEP_RECOVERING, SLEEP_LIGHTENING };
/* behavior->value in chao_sleep_behavior: the pose it sleeps in */
enum sleep_pose { SLEEP_POSE_NONE, SLEEP_POSE_SPRAWLED, SLEEP_POSE_PLAIN };
/* behavior->mode in chao_sleep_behavior. a turn or a scratch returns to
 * SLEEP_SETTLING, which takes up the pose again */
enum sleep_mode {
    SLEEP_START,
    SLEEP_FALLING,
    SLEEP_SETTLING,
    SLEEP_ASLEEP,
    SLEEP_ON_FRONT,
    SLEEP_OFF_FRONT,
    SLEEP_TURNED_LEFT,
    SLEEP_BACK_FROM_LEFT,
    SLEEP_TURNED_RIGHT,
    SLEEP_BACK_FROM_RIGHT,
    SLEEP_SCRATCHING_LEFT,
    SLEEP_LEFT_SCRATCH_ENDING,
    SLEEP_SCRATCHING_RIGHT,
    SLEEP_RIGHT_SCRATCH_ENDING,
    SLEEP_UNUSED_14,
    SLEEP_WAKING,
    SLEEP_STRETCHING
};
/* behavior->mode in chao_seated_sleep_behavior */
enum seated_mode { SEATED_START, SEATED_ASLEEP, SEATED_DOZING };
/* behavior->mode in drowsy_behavior. each motion ends in DROWSY_DONE */
enum drowsy_mode {
    DROWSY_START,
    DROWSY_YAWNING,
    DROWSY_YAWN_ENDING,
    DROWSY_STRETCHING,
    DROWSY_SEATED_YAWNING,
    DROWSY_SEATED_YAWN_ENDING,
    DROWSY_SEATED_STRETCHING,
    DROWSY_RUBBING_EYES,
    DROWSY_DONE
};

static int settle_to_sleep(struct task *chao);
static int drowsy_behavior(struct task *chao);

void score_drowsiness(struct task *chao, Float *best)
{
    Float score;
    Uint32 low;
    Uint32 peak;
    Uint32 high;
    Uint32 sleepiness;
    Uint32 cooldown;

    score = 0.0F;
    low = chao_tuning.drowsy_low;
    peak = chao_tuning.drowsy_peak;
    high = chao_tuning.drowsy_high;
    sleepiness = chao_emotion_get(chao, CHAO_SLEEPINESS);
    cooldown = chao_cooldown(chao, DROWSY_COOLDOWN);
    if (!cooldown && sleepiness > low) {
        if (sleepiness < peak) {
            score = (Float)(sleepiness - low) / (Float)(peak - low);
        } else if (sleepiness < high) {
            score = 1.0F - ((Float)(sleepiness - peak) / (Float)(high - peak));
        } else {
            score = 0.0F;
        }
        score *= chao_tuning.drowsy_weight;
        chao_vary_score(&score);
    }
    chao_set_cooldown(
        chao, DROWSY_COOLDOWN,
        chao_tuning.drowsy_cooldown +
            (chao_tuning.drowsy_cooldown_range *
             (RANDOM_UNIT() +
              ((CHAO_NEED_MAX - chao_emotion_get(chao, CHAO_SLEEPINESS)) *
               0.0001F)) *
             0.5F));
    if (score > *best) {
        chao_set_behavior(chao, drowsy_behavior);
        chao_clear_intention(chao);
        *best = score;
    }
}

void score_waking(struct task *chao, Float *best)
{
    Uint32 threshold;
    Uint32 depth;
    Float score;

    score = 0.0F;
    /* @hack spelling: threshold, a local that holds 500, where the constant
     * would do */
    threshold = 500;
    depth = chao_emotion_get(chao, CHAO_SLEEP_DEPTH);
    if (depth > threshold) {
        score = chao_need_above(depth, threshold);
        score = (score * 0.5F) + 0.5F;
        chao_vary_score(&score);
    }
    if (score > *best) {
        chao_set_behavior(chao, chao_wake_behavior);
        chao_clear_intention(chao);
        *best = score;
    }
}

void score_sleep(struct task *chao, Float *best)
{
    struct task_work *work;
    struct mover *mover;
    Uint32 threshold;
    Uint32 sleepiness;
    Float score;

    score = 0.0F;
    threshold = chao_tuning.sleep_threshold;
    sleepiness = chao_emotion_get(chao, CHAO_SLEEPINESS);
    work = chao->work;
    mover = chao->motion.p;
    if (mover->flat_y > work->pos.y) {
        return;
    }
    if (sleepiness > threshold) {
        score = chao_need_above(sleepiness, threshold);
        score *= chao_tuning.sleep_weight;
        chao_vary_score(&score);
    }
    if (score > *best) {
        chao_set_behavior(chao, settle_to_sleep);
        chao_clear_intention(chao);
        *best = score;
    }
}

static Bool ready_to_wake(struct task *chao)
{
    if (chao_emotion_get(chao, CHAO_SLEEPINESS) < WAKE_SLEEPINESS &&
        chao_emotion_get(chao, CHAO_SLEEP_DEPTH) < chao_tuning.wake_depth) {
        return TRUE;
    }
    return FALSE;
}

static void recover_in_sleep(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);

    if (!chao_take_upkeep_tick(chao)) {
        return;
    }
    switch (behavior->phase) {
    case SLEEP_DEEPENING:
        chao_emotion_add(chao, CHAO_SLEEP_DEPTH, chao_tuning.sleep_depth_gain);
        if (chao_emotion_get(chao, CHAO_SLEEP_DEPTH) >= CHAO_NEED_MAX) {
            behavior->phase++;
        }
        break;
    case SLEEP_RECOVERING:
        chao_emotion_add(chao, CHAO_SLEEPINESS, -chao_tuning.sleepiness_loss);
        chao_emotion_add(
            chao, CHAO_TIREDNESS,
            -(chao_tuning.tiredness_recovery *
              (chao_emotion_get(chao, CHAO_RECOVERY) + CHAO_TRAIT_SCALE)) /
                CHAO_TRAIT_SCALE);
        if (chao_emotion_get(chao, CHAO_SLEEPINESS) <= 0) {
            behavior->phase++;
        }
        break;
    case SLEEP_LIGHTENING:
        chao_emotion_add(chao, CHAO_SLEEPINESS, -chao_tuning.sleepiness_loss);
        chao_emotion_add(chao, CHAO_SLEEP_DEPTH, -chao_tuning.sleep_depth_loss);
        break;
    }
}

static int settle_to_sleep(struct task *chao)
{
    struct chao_perceived_object *tree;
    Float stay_chance;
    struct task *object;
    struct garden_tree *work;
    int stage;

    chao_set_behavior(chao, chao_sleep_behavior);
    tree = chao_nearest_visible_tree(chao);
    if (tree == NULL) {
#ifdef NONMATCHING
        /* the plain form the hack tag names. GCC refuses a return with no value
         * in a function that returns one */
        return BEHAVIOR_CONTINUE;
#else
        /* @hack implicit: a return with no value, where return
         * BEHAVIOR_CONTINUE would do. its result is the null tree just
         * tested, which is BEHAVIOR_CONTINUE's 0 */
        return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
    }
    stay_chance = (chao_emotion_get(chao, CHAO_AGGRESSION) * 0.003F) + 0.5F;
    if (stay_chance < RANDOM_UNIT() && tree->distance < TREE_REACH &&
        tree->entry != NULL) {
        object = tree->entry->task;
        if (object != NULL) {
            work = (struct garden_tree *)object->work;
            stage = work->stage;
            switch (stage) {
            case TREE_STAGE_MATURE:
            case TREE_STAGE_PERMANENT:
                garden_set_target(chao, tree->entry->task);
                chao_set_behavior(chao, chao_walk_to_target_behavior);
                chao_queue_behavior(chao, chao_turn_around_behavior);
                chao_queue_behavior(chao, chao_sleep_behavior);
                break;
                /* @bug uninitialized: a young tree near the Chao ends or
                 * replaces the sleep set on entry. no case returns, so where
                 * the tree's stage is neither TREE_STAGE_MATURE nor
                 * TREE_STAGE_PERMANENT, the result is the stage loaded at
                 * 0x0c536f40. chao_behavior_step, which calls the behavior at
                 * 0x0c50134e, takes TREE_STAGE_SAPLING's 2 as BEHAVIOR_CHOOSE
                 * at 0x0c50139e. it takes TREE_STAGE_SEED's 1 as
                 * BEHAVIOR_FINISH at 0x0c5013d4 */
#ifdef NONMATCHING
            /* this side keeps the tag's result: it returns the stage, which the
             * image leaves in R0 */
            default:
                return stage;
#endif
            }
        }
    }
#ifdef NONMATCHING
    /* chao_behavior_step acts only on 1, 2 and 3. on the image's other paths
     * R0 holds none of them, so BEHAVIOR_CONTINUE does the same */
    return BEHAVIOR_CONTINUE;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

int chao_sleep_behavior(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_behavior *behavior = &work->behavior;
    int pose;
    int aggression;
    Float choice;

    switch (behavior->mode) {
    case SLEEP_START:
        pose = chao_motion_pose(chao);
        switch (pose) {
        case CHAO_STANDING:
            chao_link_motion(chao, CHAO_ANIM_BATAN);
            behavior->mode = SLEEP_FALLING;
            break;
        case CHAO_SITTING:
            if (chao_emotion_get(chao, CHAO_TIREDNESS) > SEATED_TIREDNESS &&
                RANDOM_UNIT() < 0.5F) {
                chao_set_behavior(chao, chao_seated_sleep_behavior);
            } else {
                behavior->mode = SLEEP_SETTLING;
            }
            break;
        case CHAO_ON_BACK:
        case CHAO_ON_FRONT:
        default:
            behavior->mode = SLEEP_SETTLING;
        }
        chao_set_behavior_value(chao, SLEEP_POSE_NONE);
        break;
    case SLEEP_FALLING:
        if (chao_motion_ended(chao)) {
            behavior->mode = SLEEP_SETTLING;
        }
        break;
    case SLEEP_SETTLING:
        aggression = chao_emotion_get(chao, CHAO_AGGRESSION);
        /* @hack redundant: the read of aggression, and an empty test of its
         * sign */
        if (aggression >= 0) {
        }
        switch (chao_behavior_value(chao)) {
        case SLEEP_POSE_NONE:
            if (chao_emotion_get(chao, CHAO_AGGRESSION) >
                rand() * (100.0F * RAND_SCALE)) {
                chao_link_motion_frames(chao, CHAO_ANIM_DAINOJI, 40);
                chao_set_behavior_value(chao, SLEEP_POSE_SPRAWLED);
            } else {
                chao_link_motion_frames(chao, CHAO_ANIM_SLEEP, 40);
                chao_set_behavior_value(chao, SLEEP_POSE_PLAIN);
            }
            break;
        case SLEEP_POSE_SPRAWLED:
            chao_link_motion(chao, CHAO_ANIM_DAINOJI_NOBI);
            break;
        case SLEEP_POSE_PLAIN:
            chao_link_motion(chao, CHAO_ANIM_SLEEP_LOOP);
            break;
        }
        behavior->timer = SLEEP_INTERVAL;
        chao_set_eye(chao, CHAO_EYE_ASLEEP);
        behavior->mode = SLEEP_ASLEEP;
        /* fall through */
    case SLEEP_ASLEEP:
        switch (chao_behavior_value(chao)) {
        case SLEEP_POSE_SPRAWLED:
            if (behavior->timer % VOICE_PERIOD == 0 && RANDOM_UNIT() < 0.5F) {
                if (RANDOM_UNIT() < 0.8F) {
                    sound_play_at(SPRAWLED_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                                  &chao->work->pos);
                } else {
                    sound_play_at(SPRAWLED_RARE_VOICE, NULL, 0,
                                  GARDEN_VOICE_VOLUME, &chao->work->pos);
                }
            }
            break;
        case SLEEP_POSE_PLAIN:
            if (behavior->timer % VOICE_PERIOD == 0 && RANDOM_UNIT() < 0.4F) {
                if (RANDOM_UNIT() < 0.8F) {
                    sound_play_at(PLAIN_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                                  &chao->work->pos);
                } else {
                    sound_play_at(PLAIN_RARE_VOICE, NULL, 0,
                                  GARDEN_VOICE_VOLUME, &chao->work->pos);
                }
            }
            break;
        }
        if (behavior->timer-- <= 0) {
            choice = RANDOM_UNIT();
            if (choice < 0.3F) {
                behavior->timer = SLEEP_INTERVAL;
                chao_link_motion(chao, CHAO_ANIM_UTUBUSE_NEGAERI);
                behavior->mode = SLEEP_ON_FRONT;
            } else if (choice < 0.6F) {
                if (RANDOM_UNIT() < 0.5F) {
                    behavior->timer = SLEEP_INTERVAL;
                    chao_link_motion(chao, CHAO_ANIM_NEGAERI_L);
                    behavior->mode = SLEEP_TURNED_LEFT;
                } else {
                    behavior->timer = SLEEP_INTERVAL;
                    chao_link_motion(chao, CHAO_ANIM_NEGAERI_R);
                    behavior->mode = SLEEP_TURNED_RIGHT;
                }
            } else if (choice < 0.9F) {
                if (RANDOM_UNIT() < 0.5F) {
                    behavior->timer =
                        (Uint32)(rand() * (4.0F * RAND_SCALE)) + 2;
                    chao_link_motion(chao, CHAO_ANIM_NERU_PORI_L);
                    behavior->mode = SLEEP_SCRATCHING_LEFT;
                } else {
                    behavior->timer =
                        (Uint32)(rand() * (4.0F * RAND_SCALE)) + 2;
                    chao_link_motion(chao, CHAO_ANIM_NERU_PORI_R);
                    behavior->mode = SLEEP_SCRATCHING_RIGHT;
                }
            } else {
                behavior->timer = SLEEP_INTERVAL;
            }
            if (RANDOM_UNIT() < 0.25F) {
                if (RANDOM_UNIT() < 0.5F) {
                    sound_play_at(TURN_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                                  &chao->work->pos);
                } else {
                    sound_play_at(OTHER_TURN_VOICE, NULL, 0,
                                  GARDEN_VOICE_VOLUME, &chao->work->pos);
                }
            }
        }
        if (ready_to_wake(chao)) {
            behavior->mode = SLEEP_WAKING;
        }
        break;
    case SLEEP_ON_FRONT:
        if (behavior->timer-- <= 0 || ready_to_wake(chao)) {
            chao_link_motion(chao, CHAO_ANIM_UTUBUSE_END);
            behavior->mode = SLEEP_OFF_FRONT;
        }
        break;
    /* @hack spelling: a case of its own for each wait that returns to
     * SLEEP_SETTLING, where one case list would do */
    case SLEEP_OFF_FRONT:
        if (chao_motion_ended(chao)) {
            behavior->mode = SLEEP_SETTLING;
        }
        break;
    case SLEEP_TURNED_LEFT:
        if (behavior->timer-- <= 0 || ready_to_wake(chao)) {
            chao_link_motion(chao, CHAO_ANIM_NEGAERI_L_RETURN);
            behavior->mode = SLEEP_BACK_FROM_LEFT;
        }
        break;
    case SLEEP_BACK_FROM_LEFT:
        if (chao_motion_ended(chao)) {
            behavior->mode = SLEEP_SETTLING;
        }
        break;
    case SLEEP_TURNED_RIGHT:
        if (behavior->timer-- <= 0 || ready_to_wake(chao)) {
            chao_link_motion(chao, CHAO_ANIM_NEGAERI_R_RETURN);
            behavior->mode = SLEEP_BACK_FROM_RIGHT;
        }
        break;
    case SLEEP_BACK_FROM_RIGHT:
        if (chao_motion_ended(chao)) {
            behavior->mode = SLEEP_SETTLING;
        }
        break;
    case SLEEP_SCRATCHING_LEFT:
        if (chao_motion_ended(chao) && behavior->timer-- <= 0) {
            chao_link_motion(chao, CHAO_ANIM_NERU_PORI_L_END);
            behavior->mode = SLEEP_LEFT_SCRATCH_ENDING;
        }
        if (ready_to_wake(chao)) {
            chao_link_motion(chao, CHAO_ANIM_NERU_PORI_L_END);
            behavior->mode = SLEEP_LEFT_SCRATCH_ENDING;
        }
        break;
    case SLEEP_LEFT_SCRATCH_ENDING:
        if (chao_motion_ended(chao)) {
            behavior->mode = SLEEP_SETTLING;
        }
        break;
    case SLEEP_SCRATCHING_RIGHT:
        if (chao_motion_ended(chao) && behavior->timer-- <= 0) {
            chao_link_motion(chao, CHAO_ANIM_NERU_PORI_R_END);
            behavior->mode = SLEEP_RIGHT_SCRATCH_ENDING;
        }
        if (ready_to_wake(chao)) {
            chao_link_motion(chao, CHAO_ANIM_NERU_PORI_R_END);
            behavior->mode = SLEEP_RIGHT_SCRATCH_ENDING;
        }
        break;
    case SLEEP_RIGHT_SCRATCH_ENDING:
        if (chao_motion_ended(chao)) {
            behavior->mode = SLEEP_SETTLING;
        }
        break;
    case SLEEP_WAKING:
        chao_link_motion_frames(chao, CHAO_ANIM_NOBI_SIT, 40);
        behavior->mode = SLEEP_STRETCHING;
        chao_set_eye(chao, CHAO_EYE_DISTRESSED);
        chao_set_mouth(chao, CHAO_MOUTH_GLAD);
        break;
    case SLEEP_STRETCHING:
        if (chao_motion_held(chao)) {
            chao_clear_intention(chao);
            return BEHAVIOR_FINISH;
        }
        break;
    }
    recover_in_sleep(chao);
    return BEHAVIOR_CONTINUE;
}

int chao_seated_sleep_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);

    switch (behavior->mode) {
    case SEATED_START:
        if (chao_emotion_get(chao, CHAO_TIREDNESS) * 0.0001F > RANDOM_UNIT()) {
            chao_link_motion(chao, CHAO_ANIM_SIT_SLEEP);
            behavior->mode = SEATED_ASLEEP;
        } else {
            chao_link_motion(chao, CHAO_ANIM_UTOUTO);
            behavior->mode = SEATED_DOZING;
            behavior->timer = (rand() * (10.0F * RAND_SCALE)) + 3.0F;
        }
        chao_set_eye(chao, CHAO_EYE_ASLEEP);
        break;
    case SEATED_ASLEEP:
        if (ready_to_wake(chao)) {
            chao_link_motion(chao, CHAO_ANIM_HIMA_SIT);
            return BEHAVIOR_FINISH;
        }
        break;
    case SEATED_DOZING:
        if (chao_motion_ended(chao) && behavior->timer-- <= 0) {
            chao_link_motion(chao, CHAO_ANIM_UTOUTO_KOKKURI);
            behavior->timer = (rand() * (10.0F * RAND_SCALE)) + 3.0F;
        }
        if (ready_to_wake(chao)) {
            chao_link_motion(chao, CHAO_ANIM_HIMA_SIT);
            chao_clear_intention(chao);
            return BEHAVIOR_FINISH;
        }
        break;
    }
    recover_in_sleep(chao);
    return BEHAVIOR_CONTINUE;
}

static int drowsy_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int pose;
    Float choice;

    switch (behavior->mode) {
    case DROWSY_START:
        pose = chao_motion_pose(chao);
        switch (pose) {
        case CHAO_STANDING:
            if (RANDOM_UNIT() < 0.5F) {
                behavior->timer = (rand() * (6.0F * RAND_SCALE)) + 3.0F;
                chao_link_motion(chao, CHAO_ANIM_AKUBI);
                behavior->mode = DROWSY_YAWNING;
                sound_play_at(YAWN_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                              &chao->work->pos);
                chao_set_eye(chao, 3);
                chao_set_mouth(chao, 2);
            } else {
                chao_link_motion(chao, CHAO_ANIM_NOBI_STAND);
                sound_play_at(STRETCH_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                              &chao->work->pos);
                behavior->mode = DROWSY_STRETCHING;
                chao_set_eye(chao, CHAO_EYE_DISTRESSED);
                chao_set_mouth(chao, 2);
            }
            break;
        case CHAO_SITTING:
        default:
            choice = RANDOM_UNIT();
            if (choice < 0.3F) {
                behavior->timer = (rand() * (6.0F * RAND_SCALE)) + 2.0F;
                chao_link_motion(chao, CHAO_ANIM_SIT_AKUBI);
                behavior->mode = DROWSY_SEATED_YAWNING;
                sound_play_at(YAWN_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                              &chao->work->pos);
                chao_set_eye(chao, 3);
                chao_set_mouth(chao, 2);
            } else if (choice < 0.6F) {
                chao_link_motion(chao, CHAO_ANIM_NOBI_SIT);
                behavior->mode = DROWSY_SEATED_STRETCHING;
                sound_play_at(STRETCH_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                              &chao->work->pos);
                chao_set_eye(chao, CHAO_EYE_DISTRESSED);
                chao_set_mouth(chao, 2);
            } else {
                chao_link_motion(chao, CHAO_ANIM_MEKOSURI);
                behavior->mode = DROWSY_RUBBING_EYES;
                chao_set_eye(chao, 3);
                chao_set_mouth(chao, CHAO_MOUTH_DISTRESSED);
            }
            break;
        }
        break;
    case DROWSY_YAWNING:
        if (chao_motion_ended(chao) && behavior->timer-- <= 0) {
            chao_link_motion(chao, CHAO_ANIM_AKUBI_END);
            behavior->mode = DROWSY_YAWN_ENDING;
        }
        break;
    /* @hack spelling: a case of its own for each wait that ends in
     * DROWSY_DONE, where one case list would do */
    case DROWSY_YAWN_ENDING:
        if (chao_motion_ended(chao)) {
            behavior->mode = DROWSY_DONE;
        }
        break;
    case DROWSY_STRETCHING:
        if (chao_motion_ended(chao)) {
            behavior->mode = DROWSY_DONE;
        }
        break;
    case DROWSY_SEATED_YAWNING:
        if (chao_motion_ended(chao) && behavior->timer-- <= 0) {
            chao_link_motion(chao, CHAO_ANIM_SIT_AKUBI_END);
            behavior->mode = DROWSY_SEATED_YAWN_ENDING;
        }
        break;
    case DROWSY_SEATED_YAWN_ENDING:
        if (chao_motion_ended(chao)) {
            behavior->mode = DROWSY_DONE;
        }
        break;
    case DROWSY_SEATED_STRETCHING:
        if (chao_motion_ended(chao)) {
            behavior->mode = DROWSY_DONE;
        }
        break;
    case DROWSY_RUBBING_EYES:
        if (chao_motion_ended(chao)) {
            behavior->mode = DROWSY_DONE;
        }
        break;
    case DROWSY_DONE:
        chao_clear_intention(chao);
        return BEHAVIOR_FINISH;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_wake_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int pose;

    switch (behavior->mode) {
    case 0:
        pose = chao_motion_pose(chao);
        switch (pose) {
        case CHAO_SITTING:
            chao_link_motion(chao, CHAO_ANIM_HIMA_SIT_GUDE);
            break;
        case CHAO_ON_BACK:
            chao_link_motion(chao, CHAO_ANIM_SLEEP);
            break;
        case CHAO_STANDING:
        default:
            chao_link_motion(chao, CHAO_ANIM_STAND);
            break;
        }
        chao_set_timed_eye(chao, CHAO_EYE_HALF_SHUT, CHAO_FACE_HOLD);
        if (RANDOM_UNIT() < 0.3F) {
            chao_set_timed_mouth(chao, CHAO_MOUTH_DISTRESSED, CHAO_FACE_HOLD);
        }
        chao_show_mark(chao, CHAO_MARK_ANGER, 360);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer++ > 400) {
            chao_show_mark(chao, CHAO_MARK_ANGER, 360);
            behavior->timer = 0;
        }
        chao_emotion_add(chao, CHAO_SLEEP_DEPTH, -2);
        if (chao_emotion_get(chao, CHAO_SLEEP_DEPTH) == 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
