/* a hungry garden Chao. it seeks fruit on the ground, begs for fruit the
 * player holds, shows its hunger, or cries for food */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/behavior.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/fruit.h"
#include "chao/intention.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/needs.h"
#include "chao/perception.h"
#include "chao/tears.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chao/world.h"
#include "object/mover.h"
#include "player/player.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

/* the index of the cooldown that holds back shows of hunger. while it runs, a
 * hungry Chao bids only to seek fruit */
#define HUNGER_COOLDOWN 14
/* a Chao bids to cry for food above CRYING_HUNGER */
#define CRYING_HUNGER 9900
/* a Chao walks toward player 0 while the player holds fruit, and turns by
 * TURN_STEP a step. it begs once its squared distance to the player is below
 * BEGGING_DISTANCE2, or after APPROACH_STEPS steps */
#define APPROACH_STEPS 1000
#define TURN_STEP 0x120
#define BEGGING_DISTANCE2 100.0F
/* a Chao begs for BEG_STEPS and a random part of BEG_SPAN. it cries for food
 * for CRY_STEPS and a random part of CRY_SPAN */
#define BEG_STEPS 180
#define BEG_SPAN 121.0F
#define CRY_STEPS 240
#define CRY_SPAN 201.0F
/* the frames the approach's, the begging's and the crying's motions link
 * over */
#define APPROACH_LINK 10
#define BEG_LINK 25
#define CRY_LINK 20
/* a crying Chao sheds tears through TEARS_SHOWN steps of each TEAR_CYCLE */
#define TEAR_CYCLE 8
#define TEARS_SHOWN 4
/* a crying Chao's voice may sound once each VOICE_INTERVAL frames */
#define VOICE_INTERVAL 120
#define HUNGRY_VOICE 0x6003
#define HUNGRY_VOLUME 60
/* a sitting Chao slumps for at least SLUMP_STEPS, and on until its motion
 * ends. any other Chao growls until its motion ends. either then waits more
 * than WAIT_STEPS steps */
#define SLUMP_STEPS 180
#define WAIT_STEPS 60

static int show_hunger(struct task *chao);
static int cry_for_food(struct task *chao);

#define BEGGING_FACE(chao)                                                     \
    {                                                                          \
        chao_set_eye(chao, CHAO_EYE_GLAD);                                     \
        /* @hack type: the default mouth cast to Sint16 for the test, where    \
         * the int would do */                                                 \
        if ((Sint16)chao_default_mouth(chao) != 1) {                           \
            if (RANDOM_UNIT() < 0.5F) {                                        \
                chao_set_mouth(chao, CHAO_MOUTH_GLAD);                         \
            } else {                                                           \
                chao_set_mouth(chao, 2);                                       \
            }                                                                  \
        }                                                                      \
    }

static int approach_food_in_hand(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct task_work *work = chao->work;
    struct mover *mover = chao->motion.p;

    /* @hack redundant: a skill read whose value nothing uses */
    chao_skill_points(chao, CHAO_SWIM);
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_OIKAKE, APPROACH_LINK);
        BEGGING_FACE(chao);
        behavior->mode++;
        behavior->timer = 0;
        behavior->count = 0;
    case 1:
        break;
    }
    if (behavior->timer++ > APPROACH_STEPS) {
        return BEHAVIOR_FINISH;
    }
    if (mover_player_distance2(chao, 0) < BEGGING_DISTANCE2) {
        return BEHAVIOR_FINISH;
    }
    mover_turn_to_player_by(chao, TURN_STEP, 0);
    if (mover->flags & MOVER_ON_FLOOR) {
        mover->acc.x = njSin(work->ang.y) * chao_tuning.walk_speed;
        mover->acc.z = njCos(work->ang.y) * chao_tuning.walk_speed;
    }
    return BEHAVIOR_CONTINUE;
}

static int beg_for_food(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_GIVEME_P, BEG_LINK);
        BEGGING_FACE(chao);
        behavior->mode++;
        behavior->timer = RANDOM_COUNT(BEG_STEPS, BEG_SPAN);
        /* fall through */
    case 1:
        if (behavior->timer-- <= 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

void score_hunger(struct task *chao, Float *best)
{
    Float score = 0.0F;
    Uint32 hunger = chao_emotion_get(chao, CHAO_HUNGER);
    Uint32 cooldown = chao_cooldown(chao, HUNGER_COOLDOWN);
    Uint32 threshold;
    struct task *fruit;
    struct chao_perceived_object *tree;

    if (cooldown == 0 && RANDOM_UNIT() < 0.7F) {
        threshold = chao_tuning.show_hunger_threshold;
        score = chao_need_above(hunger, threshold);
        score *= chao_tuning.show_hunger_weight;
        chao_vary_score(&score);
        if (score > *best) {
            chao_set_behavior(chao, show_hunger);
            chao_clear_intention(chao);
            *best = score;
        }
        if (hunger > CRYING_HUNGER) {
            score = 1.0F;
            chao_vary_score(&score);
            if (score > *best) {
                chao_set_behavior(chao, cry_for_food);
                chao_clear_intention(chao);
                *best = score;
            }
        }
    } else {
        fruit = chao_nearest_fruit_task(chao);
        if (fruit) {
            if (garden_paired_elsewhere(chao, fruit)) {
                return;
            }
            if (fruit->work->flags & WORK_HELD_BY_PLAYER) {
                threshold = chao_tuning.seek_food_threshold;
                score = chao_need_above(hunger, threshold);
                score *= chao_tuning.seek_food_weight;
                chao_vary_score(&score);
                if (score > *best) {
                    chao_set_behavior(chao, approach_food_in_hand);
                    chao_queue_behavior(chao, beg_for_food);
                    chao_clear_intention(chao);
                    *best = score;
                }
            } else {
                threshold = chao_tuning.seek_food_threshold;
                score = chao_need_above(hunger, threshold);
                score *= chao_tuning.seek_food_weight;
                chao_vary_score(&score);
                if (score > *best) {
                    chao_set_behavior(chao, chao_seek_fruit_behavior);
                    chao_clear_intention(chao);
                    *best = score;
                }
            }
            return;
        }
        tree = chao_nearest_visible_tree(chao);
        if (tree) {
            /* a tree in view is scored as fruit on the ground is, and
             * chao_vary_score draws from rand. a score that beats *best
             * still sets no behavior */
            threshold = chao_tuning.seek_food_threshold;
            score = chao_need_above(hunger, threshold);
            score *= chao_tuning.seek_food_weight;
            chao_vary_score(&score);
            if (score > *best) {
                return;
            }
        }
    }
}

/* set the hunger cooldown to hunger_cooldown steps and a part of
 * hunger_cooldown_range more. the less hungry the Chao, the larger the part */
#define DELAY_HUNGER_DISPLAY(chao)                                             \
    chao_set_cooldown(                                                         \
        chao, HUNGER_COOLDOWN,                                                 \
        chao_tuning.hunger_cooldown +                                          \
            (chao_tuning.hunger_cooldown_range *                               \
             (RANDOM_UNIT() +                                                  \
              ((CHAO_NEED_MAX - chao_emotion_get(chao, CHAO_HUNGER)) *         \
               (1.0F / CHAO_NEED_MAX))) *                                      \
             0.5F))

static int show_hunger(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    int pose;

    switch (behavior->mode) {
    case 0:
        pose = chao_motion_pose(chao);
        switch (pose) {
        case CHAO_SITTING:
            chao_link_motion(chao, CHAO_ANIM_GUTA_A);
            behavior->timer = SLUMP_STEPS;
            behavior->mode = 2;
            break;
        case CHAO_ON_BACK:
        case CHAO_ON_FRONT:
        case CHAO_STANDING:
        default:
            chao_link_motion(chao, CHAO_ANIM_GOO);
            behavior->mode = 1;
            break;
        }
        chao_set_eye(chao, 3);
        break;
    case 1:
        if (chao_motion_ended(chao)) {
            behavior->mode = 3;
            behavior->timer = 0;
            behavior->count = 0;
        }
        break;
    case 2:
        if (behavior->timer <= 0) {
            if (chao_motion_ended(chao)) {
                behavior->mode = 3;
                behavior->timer = 0;
                behavior->count = 0;
            }
        } else {
            behavior->timer--;
        }
        break;
    case 3:
        if (behavior->timer++ > WAIT_STEPS) {
            chao_clear_intention(chao);
            DELAY_HUNGER_DISPLAY(chao);
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

static int cry_for_food(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    struct chao_shape_work *shape;
    NJS_POINT3 position;
    NJS_VECTOR speed;
    switch (behavior->mode) {
    case 0:
        chao_set_timed_eye(chao, CHAO_EYE_DISTRESSED, CHAO_FACE_HOLD);
        chao_set_timed_mouth(chao, 6, CHAO_FACE_HOLD);
        chao_link_motion_frames(chao, CHAO_ANIM_DADA, CRY_LINK);
        behavior->timer = RANDOM_COUNT(CRY_STEPS, CRY_SPAN);
        behavior->mode++;
        break;
    case 1:
        if (behavior->timer-- <= 0) {
            DELAY_HUNGER_DISPLAY(chao);
            return BEHAVIOR_FINISH;
        }
        break;
    }
    if (behavior->timer % TEAR_CYCLE < TEARS_SHOWN) {
        shape = &CHAO_WORK(chao)->shape;
        CHAO_SHED_TEAR(shape, 0, position, speed);
        CHAO_SHED_TEAR(shape, 1, position, speed);
    }
    if (CHAO_WORK(chao)->frames % VOICE_INTERVAL == 0 && RANDOM_UNIT() < 0.3F) {
        sound_play_at(HUNGRY_VOICE, NULL, 0, HUNGRY_VOLUME, &chao->work->pos);
    }
    return BEHAVIOR_CONTINUE;
}
