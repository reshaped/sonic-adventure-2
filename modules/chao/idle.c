#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/behavior.h"
#include "chao/face.h"
#include "chao/icon.h"
#include "chao/idle.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/sound.h"
#include "chao/work.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

/* an idle Chao stays IDLE_STEPS and a random part of IDLE_SPAN. each step on
 * the floor it keeps IDLE_SLOWING of its speed */
#define IDLE_STEPS 60
#define IDLE_SPAN 121.0F
#define IDLE_SLOWING 0.8F
/* the frames an idle Chao's seated motion links over */
#define SIT_LINK_FRAMES 35
#define QUESTION_STEPS 180
/* the look and the stand play their motion LEAST_PLAYS times and a random
 * part of PLAYS_SPAN more. the wait plays its own WAIT_PLAYS times and the
 * same part */
#define LEAST_PLAYS 5
#define WAIT_PLAYS 10
#define PLAYS_SPAN 10.0F

int chao_idle_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int pose;
    Float choice;

    switch (behavior->mode) {
    case 0:
        pose = chao_motion_pose(chao);
        switch (pose) {
        case CHAO_ON_BACK:
        case CHAO_SITTING:
        case CHAO_ON_FRONT:
            chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT, SIT_LINK_FRAMES);
            break;
        case CHAO_STANDING:
        default:
            choice = RANDOM_UNIT();
            if (choice < 0.15F) {
                chao_link_motion(chao, CHAO_ANIM_HATENA_HATE2);
                chao_show_mark(chao, CHAO_MARK_QUESTION, QUESTION_STEPS);
                sound_play_at(GARDEN_QUESTION_VOICE, NULL, 0,
                              GARDEN_VOICE_VOLUME, &chao->work->pos);
            } else if (choice < 0.5F) {
                chao_link_motion(chao, CHAO_ANIM_STAND);
            } else if (choice < 0.6F) {
                chao_link_motion(chao, CHAO_ANIM_KYOROKYORO_UP);
            } else if (choice < 0.7F) {
                chao_link_motion(chao, CHAO_ANIM_KYOROKYORO_DOWN);
            } else {
                chao_link_motion(chao, CHAO_ANIM_OSYABURI);
            }
            break;
        }
        chao_set_eye(chao, CHAO_EYE_OPEN);
        chao_set_mouth(chao, 0);
        behavior->timer = RANDOM_COUNT(IDLE_STEPS, IDLE_SPAN);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- <= 0) {
            return BEHAVIOR_CHOOSE;
        }
        break;
    }
    chao_slow_on_floor(chao, IDLE_SLOWING);
    return BEHAVIOR_CONTINUE;
}

int chao_idle_look_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);

    switch (behavior->mode) {
    case 0:
        if (RANDOM_UNIT() < 0.5F) {
            chao_link_motion(chao, CHAO_ANIM_KYOROKYORO_UP);
        } else {
            chao_link_motion(chao, CHAO_ANIM_KYOROKYORO_DOWN);
        }
        behavior->mode++;
        behavior->timer =
            (Uint16)(rand() * (PLAYS_SPAN * RAND_SCALE)) + LEAST_PLAYS;
        /* fall through */
    case 1:
        if (chao_motion_ended(chao) && --behavior->timer == 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_idle_stand_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);

    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_STAND);
        behavior->mode++;
        behavior->timer =
            (Uint16)(rand() * (PLAYS_SPAN * RAND_SCALE)) + LEAST_PLAYS;
        /* fall through */
    case 1:
        if (chao_motion_ended(chao) && --behavior->timer == 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_idle_wait_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);

    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_OSYABURI);
        behavior->mode++;
        behavior->timer =
            (Uint16)(rand() * (PLAYS_SPAN * RAND_SCALE)) + WAIT_PLAYS;
        /* fall through */
    case 1:
        if (chao_motion_ended(chao) && --behavior->timer == 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
