#include <sg_xpt.h>
#include "chao/activities.h"
#include "chao/behavior.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/work.h"
#include "system/task.h"
#include "system/util.h"
#include "chao/reactions.h"

/* behavior->mode in express_pain */
enum pain_mode { PAIN_START, PAIN_STUNNED, PAIN_TEND, PAIN_TENDING };
/* behavior->count counts the tends after the first. the Chao tends its hurt
 * again until the count passes AGAIN_LIMIT: six tends at most */
#define AGAIN_LIMIT 4

static int express_pain(struct task *chao);
void score_pain(struct task *chao, Float *best)
{
    REACTION_SCORE(CHAO_PAIN, CHAO_AGGRESSION, pain_threshold, pain_weight,
                   express_pain);
}
static int express_pain(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    Float choice;
    switch (behavior->mode) {
    case PAIN_START:
        if (chao_emotion_get(chao, CHAO_PAIN) > 100) {
            chao_link_motion_frames(chao, CHAO_ANIM_ITAI_PIYO, 35);
            if (RANDOM_UNIT() < 0.5F) {
                chao_set_eye(chao, 6);
            }
            choice = RANDOM_UNIT();
            if (choice < 0.5F) {
                chao_set_mouth(chao, CHAO_MOUTH_DISTRESSED);
            } else {
                chao_set_mouth(chao, 4);
            }
            behavior->timer = RANDOM_COUNT(240, 181);
            behavior->mode++;
        } else {
            behavior->mode = PAIN_TEND;
        }
        /* fall through */
    case PAIN_STUNNED:
        if (behavior->timer-- <= 0) {
            chao_emotion_add(chao, CHAO_PAIN, -10);
            behavior->mode = PAIN_TEND;
        }
        break;
    case PAIN_TEND:
        choice = RANDOM_UNIT();
        if (choice < 0.33F) {
            chao_link_motion_frames(chao, CHAO_ANIM_ITAI_BURUBURU, 35);
        } else if (choice < 0.66F) {
            chao_link_motion_frames(chao, CHAO_ANIM_ITAI_ATAMAPONPON, 35);
        } else {
            chao_link_motion_frames(chao, CHAO_ANIM_ITAI_KOSHIPONPON, 35);
        }
        if (RANDOM_UNIT() < 0.5F) {
            chao_set_eye(chao, 3);
        } else {
            chao_set_eye(chao, CHAO_EYE_DISTRESSED);
        }
        chao_set_mouth(chao, 4);
        behavior->timer = RANDOM_COUNT(120, 201);
        behavior->mode++;
        break;
    case PAIN_TENDING:
        if (behavior->timer-- <= 0) {
            if (behavior->count++ > AGAIN_LIMIT) {
                return BEHAVIOR_FINISH;
            }
            chao_emotion_add(chao, CHAO_PAIN, -20);
            if (chao_emotion_get(chao, CHAO_PAIN) < 10) {
                return BEHAVIOR_FINISH;
            }
            behavior->mode = PAIN_TEND;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
