/* a Chao the player pets. it relaxes while the petting lasts, or wakes if
 * its sleep is deeper than DEEP_SLEEP */
#include <sg_xpt.h>
#include <stdlib.h>
#include "chao/activities.h"
#include "chao/behavior.h"
#include "chao/body_collision.h"
#include "chao/cry.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/sleep.h"
#include "chao/sound.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "object/mover.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

/* the id of the player's petting shape */
#define PETTING_HIT 2
#define DEEP_SLEEP 500

/* behavior->mode in enjoy_petting */
enum pet_mode { PET_START, PET_REACT, PET_ENJOY, PET_END, PET_UNWIND };

static int enjoy_petting(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int pose;
    switch (behavior->mode) {
    case PET_START:
        chao_resist_displacement(chao);
        pose = chao_motion_pose(chao);
        switch (pose) {
        case CHAO_SITTING:
            chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT, 20);
            break;
        case CHAO_ON_BACK:
            chao_link_motion_frames(chao, CHAO_ANIM_OKIRU, 20);
            break;
        case CHAO_ON_FRONT:
            chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT, 20);
            break;
        case CHAO_STANDING:
        default:
            chao_link_motion_frames(chao, CHAO_ANIM_STAND, 20);
            break;
        }
        if (chao_emotion_get(chao, CHAO_RELAXATION) < 10) {
            chao_player_affection(chao, 1, chao_tuning.pet_alignment);
        }
        behavior->mode++;
        break;
    case PET_REACT:
        if (!chao_motion_ended(chao)) {
            break;
        } else if (chao_emotion_get(chao, CHAO_SLEEP_DEPTH) > DEEP_SLEEP) {
            chao_set_behavior(chao, chao_wake_behavior);
            break;
        }
        pose = chao_motion_pose(chao);
        switch (pose) {
        case CHAO_SITTING:
        case CHAO_ON_BACK:
            if (rand() * RAND_SCALE < 0.5F) {
                chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT_PATAPATA, 20);
            } else {
                chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT_SWING, 20);
            }
            break;
        case CHAO_STANDING:
        default:
            if (rand() * RAND_SCALE < 0.5F) {
                chao_link_motion_frames(chao, CHAO_ANIM_NADE_GLAD, 20);
            } else {
                chao_link_motion_frames(chao, CHAO_ANIM_NADE_KIMOCHII, 20);
            }
            break;
        }
        behavior->mode++;
        behavior->timer = 10;
        if (rand() * RAND_SCALE < 0.3F) {
            if (rand() * RAND_SCALE < 0.5F) {
                sound_play_at(0x6074, NULL, 0, GARDEN_VOICE_VOLUME,
                              &chao->work->pos);
            } else {
                sound_play_at(0x6075, NULL, 0, GARDEN_VOICE_VOLUME,
                              &chao->work->pos);
            }
        }
        break;
    case PET_ENJOY:
        if (chao_motion_ended(chao)) {
            if (--behavior->timer <= 0) {
                chao_disable_body_shapes(chao);
                behavior->mode++;
            }
        }
        if (chao_emotion_get(chao, CHAO_RELAXATION) > 100) {
            chao_set_eye(chao, 3);
            chao_set_mouth(chao, CHAO_MOUTH_GLAD);
        }
        if (behavior->count++ >= 3) {
            chao_emotion_add(chao, CHAO_RELAXATION, 1);
        }
        if (!collision_first_with_id(chao, PETTING_HIT)) {
            switch (chao_motion_pose(chao)) {
            case CHAO_SITTING:
                chao_link_motion_frames(chao, CHAO_ANIM_OSYABURI_L, 20);
                break;
            case CHAO_STANDING:
                chao_link_motion_frames(chao, CHAO_ANIM_OSYABURI, 20);
                break;
            }
            behavior->mode = PET_UNWIND;
            behavior->timer = 0;
            behavior->count = 0;
        }
        break;
    case PET_END:
        if (behavior->timer++ > 10) {
            return BEHAVIOR_FINISH;
        }
        break;
    case PET_UNWIND:
        if (behavior->count++ >= 15) {
            chao_emotion_add(chao, CHAO_RELAXATION, -1);
        }
        if (chao_emotion_get(chao, CHAO_RELAXATION) < 10) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_try_petting(struct task *chao)
{
    chao_behavior_fn action = chao_current_behavior(chao);
    if (action != enjoy_petting && action != chao_wake_behavior &&
        action != chao_sob_behavior && action != chao_sigh_behavior &&
        action != chao_cry_behavior) {
        if ((chao->work->flags & MOVER_WORK_ON_FLOOR) &&
            garden_hit_kind(chao, CHAO_PUSH_SHAPE, PETTING_HIT)) {
            chao_set_behavior(chao, enjoy_petting);
            return TRUE;
        }
    }
    return FALSE;
}
