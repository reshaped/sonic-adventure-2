#include <sg_xpt.h>
#include "chao/activities.h"
#include "chao/behavior.h"
#include "chao/emotion.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/race_chao.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "system/task.h"
#include "system/util.h"

int chao_rest_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    Float choice;
    switch (behavior->mode) {
    case 0:
        behavior->timer = RANDOM_COUNT(120, 501);
        choice = RANDOM_UNIT();
        if (choice < 0.2F) {
            chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT, 40);
        } else if (choice < 0.3F) {
            chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT_PATAPATA, 40);
        } else if (choice < 0.4F) {
            chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT_GUDE, 40);
        } else if (choice < 0.5F) {
            chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT_PATA, 40);
        } else if (choice < 0.6F) {
            chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT_PATAPATA, 40);
        } else if (choice < 0.7F) {
            chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT_SWING, 40);
        } else if (choice < 0.8F) {
            behavior->timer = RANDOM_COUNT(120, 121);
            if (RANDOM_UNIT() < 0.5F) {
                chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT_LOOK_R, 40);
            } else {
                chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT_LOOK_L, 40);
            }
        } else {
            behavior->timer = RANDOM_COUNT(60, 121);
            chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT_PORI, 40);
        }
        behavior->mode++;
        break;
    case 1:
        if (behavior->timer-- <= 0) {
            if ((Uint32)chao_emotion_get(chao, CHAO_HUNGER) >
                chao_tuning.rest_interrupt_threshold) {
                return BEHAVIOR_FINISH;
            }
            if ((Uint32)chao_emotion_get(chao, CHAO_SLEEPINESS) >
                chao_tuning.rest_interrupt_threshold) {
                return BEHAVIOR_FINISH;
            }
            if ((Uint32)chao_emotion_get(chao, CHAO_BOREDOM) >
                chao_tuning.rest_interrupt_threshold) {
                return BEHAVIOR_FINISH;
            }
            if ((Uint32)chao_emotion_get(chao, CHAO_LONELINESS) >
                chao_tuning.rest_interrupt_threshold) {
                return BEHAVIOR_FINISH;
            }
            if (chao_emotion_get(chao, CHAO_TIREDNESS) < 100) {
                return BEHAVIOR_FINISH;
            }
            if (RANDOM_UNIT() < 0.3F) {
                return BEHAVIOR_FINISH;
            }
            behavior->mode = 0;
        }
        /* at each upkeep tick, the rest takes tiredness off and adds
         * condition, each scaled by the Chao's recovery */
        if (chao_take_upkeep_tick(chao)) {
            chao_emotion_add(
                chao, CHAO_TIREDNESS,
                -(chao_tuning.rest_tiredness_loss *
                  (chao_emotion_get(chao, CHAO_RECOVERY) + CHAO_TRAIT_SCALE)) /
                    CHAO_TRAIT_SCALE);
            chao_emotion_add(
                chao, CHAO_CONDITION,
                (chao_tuning.rest_condition_gain *
                 (chao_emotion_get(chao, CHAO_RECOVERY) + CHAO_TRAIT_SCALE)) /
                    CHAO_TRAIT_SCALE);
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
