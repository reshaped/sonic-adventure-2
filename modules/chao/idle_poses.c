/* the first two of chao_practice_actions: a bow, and a seated laugh. each
 * goes round until another behavior replaces it */
#include "chao/behavior.h"
#include "chao/face.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/practice.h"
#include "chao/work.h"
#include "system/task.h"
#include "system/util.h"

/* a bow holds BOW_STEPS and a random part of BOW_SPAN. a laugh holds
 * LAUGH_STEPS and a random part of LAUGH_SPAN */
#define BOW_STEPS 120
#define BOW_SPAN 121.0F
#define LAUGH_STEPS 60
#define LAUGH_SPAN 61.0F

int chao_bow(struct task *chao)
{
    struct chao_behavior *behavior =
        &((struct chao_work *)chao->work)->behavior;
    switch (behavior->mode) {
    case 0:
        if (RANDOM_UNIT() < 0.4F) {
            chao_link_motion(chao, CHAO_ANIM_OJIGI);
        }
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_set_timed_mouth(chao, CHAO_MOUTH_GLAD, CHAO_FACE_HOLD);
        behavior->timer = RANDOM_COUNT(BOW_STEPS, BOW_SPAN);
        behavior->mode++;
        break;
    case 1:
        if (behavior->timer-- <= 0) {
            behavior->mode--;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
int chao_laugh_seated(struct task *chao)
{
    struct chao_behavior *behavior =
        &((struct chao_work *)chao->work)->behavior;
    switch (behavior->mode) {
    case 0:
        if (RANDOM_UNIT() < 0.4F) {
            chao_link_motion(chao, CHAO_ANIM_LAUGH_SIT);
        } else {
            chao_link_motion(chao, CHAO_ANIM_HIMA_SIT);
        }
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_set_timed_mouth(chao, CHAO_MOUTH_GLAD, CHAO_FACE_HOLD);
        behavior->timer = RANDOM_COUNT(LAUGH_STEPS, LAUGH_SPAN);
        behavior->mode++;
        break;
    case 1:
        if (behavior->timer-- <= 0) {
            behavior->mode--;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
