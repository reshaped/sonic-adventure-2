#include <sg_xpt.h>
#include "chao/activities.h"
#include "chao/behavior.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/sound.h"
#include "chao/work.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"
#include "chao/reactions.h"

/* behavior->mode in express_pleasure */
enum pleasure_mode {
    PLEASURE_START,
    PLEASURE_FROLIC,
    PLEASURE_LAND,
    PLEASURE_GLAD,
    PLEASURE_BANZAI,
    PLEASURE_AGAIN
};
/* behavior->count counts the shows after the first. the Chao shows its
 * pleasure again until the count passes AGAIN_LIMIT: ten shows at most */
#define AGAIN_LIMIT 8
/* the sound id of the first of five glad voices, the rest following it */
#define GLAD_VOICES 0x602f
#define VOICE(id)                                                              \
    sound_play_at((id), NULL, 0, GARDEN_VOICE_VOLUME, &chao->work->pos)

static int express_pleasure(struct task *chao);
void score_pleasure(struct task *chao, Float *best)
{
    REACTION_SCORE(CHAO_PLEASURE, CHAO_CALM, pleasure_threshold,
                   pleasure_weight, express_pleasure);
}
static int express_pleasure(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    Float choice;
    switch (behavior->mode) {
    case PLEASURE_START:
        choice = RANDOM_UNIT();
        if (choice < 0.2F) {
            chao_link_motion(chao, CHAO_ANIM_HASYAGU_JUMP);
            behavior->mode = PLEASURE_FROLIC;
        } else if (choice < 0.6F) {
            chao_link_motion(chao, CHAO_ANIM_GLAD_STAND);
            behavior->mode = PLEASURE_GLAD;
        } else {
            chao_link_motion(chao, CHAO_ANIM_BANZAI_JUMP);
            behavior->mode = PLEASURE_BANZAI;
        }
        choice = RANDOM_UNIT();
        if (choice < 0.5F) {
            chao_set_eye(chao, CHAO_EYE_DISTRESSED);
        } else {
            chao_set_eye(chao, CHAO_EYE_GLAD);
        }
        choice = RANDOM_UNIT();
        if (choice < 0.4F) {
            chao_set_mouth(chao, CHAO_MOUTH_GLAD);
        } else if (choice < 0.8F) {
            chao_set_mouth(chao, 5);
        } else {
            chao_set_mouth(chao, 2);
        }
        if (RANDOM_UNIT() < 0.7F) {
            choice = RANDOM_UNIT();
            if (choice < 0.2F) {
                VOICE(GLAD_VOICES);
            } else if (choice < 0.4F) {
                VOICE(GLAD_VOICES + 1);
            } else if (choice < 0.6F) {
                VOICE(GLAD_VOICES + 2);
            } else if (choice < 0.8F) {
                VOICE(GLAD_VOICES + 3);
            } else {
                VOICE(GLAD_VOICES + 4);
            }
        }
        behavior->timer = RANDOM_COUNT(120, 61);
        /* fall through */
    case PLEASURE_FROLIC:
        if (--behavior->timer <= 0) {
            chao_link_motion(chao, CHAO_ANIM_HASYAGU_LAND);
            behavior->mode = PLEASURE_LAND;
        }
        break;
    case PLEASURE_LAND:
        if (chao_motion_ended(chao)) {
            behavior->mode = PLEASURE_AGAIN;
        }
        break;
    case PLEASURE_GLAD:
        if (--behavior->timer <= 0) {
            behavior->mode = PLEASURE_AGAIN;
        }
        break;
    case PLEASURE_BANZAI:
        if (--behavior->timer <= 0 && chao_motion_ended(chao)) {
            behavior->mode = PLEASURE_AGAIN;
        }
        break;
    case PLEASURE_AGAIN:
        if (behavior->count++ > AGAIN_LIMIT) {
            return BEHAVIOR_FINISH;
        }
        chao_emotion_add(chao, CHAO_PLEASURE, -20);
        if (chao_emotion_get(chao, CHAO_PLEASURE) < 10) {
            return BEHAVIOR_FINISH;
        }
        behavior->mode = PLEASURE_START;
        break;
    }
    return BEHAVIOR_CONTINUE;
}
