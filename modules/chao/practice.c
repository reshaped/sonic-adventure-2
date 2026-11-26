#include <stdlib.h>

#include "chao/behavior.h"
#include "chao/body.h"
#include "chao/face.h"
#include "chao/instrument_models.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/parts.h"
#include "chao/practice.h"
#include "chao/toy_art.h"
#include "chao/work.h"
#include "system/task.h"
#include "system/util.h"

#define EQUIP(node, model)                                                     \
    chao_equip_node(chao, (node), &(model), &garden_toy_texlist)

int chao_practice_singing(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_UTAU_LEFT);
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        if (RANDOM_UNIT() < 0.5F) {
            chao_set_timed_mouth(chao, CHAO_MOUTH_GLAD, CHAO_FACE_HOLD);
        } else {
            chao_set_timed_mouth(chao, 0, CHAO_FACE_HOLD);
        }
        behavior->mode++;
        break;
    case 1:
        if (chao_motion_held(chao)) {
            behavior->mode--;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_practice_maracas(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int motion;
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_MARACAS_SHAKA_UP);
        chao_set_timed_eye(chao, CHAO_EYE_DISTRESSED, CHAO_FACE_HOLD);
        chao_set_timed_mouth(chao, 1, CHAO_FACE_HOLD);
        EQUIP(CHAO_NODE_HAND_4, chao_maraca_4);
        EQUIP(CHAO_NODE_HAND_11, chao_maraca_11);
        behavior->timer = RANDOM_COUNT(300, 301);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- <= 0 || chao_motion_held(chao)) {
            if (RANDOM_UNIT() < 0.3F) {
                chao_link_motion_frames(chao, CHAO_ANIM_MARACAS_JUMP_B, 10);
            } else {
                motion = chao_motion_index(chao);
                switch (motion) {
                case CHAO_ANIM_MARACAS_SHAKA_UP:
                    chao_link_motion_synced(chao, CHAO_ANIM_MARACAS_SHAKA_DOWN);
                    behavior->timer = RANDOM_COUNT(60, 61);
                    break;
                case CHAO_ANIM_MARACAS_SHAKA_DOWN:
                    chao_link_motion_synced(chao, CHAO_ANIM_MARACAS_SHAKA_UP);
                    behavior->timer = RANDOM_COUNT(60, 61);
                    break;
                default:
                    chao_link_motion_frames(chao, CHAO_ANIM_MARACAS_SHAKA_UP,
                                            10);
                    behavior->timer = RANDOM_COUNT(60, 61);
                    break;
                }
            }
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
int chao_practice_drums(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int motion;
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_DRUM_LR);
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_set_timed_mouth(chao, CHAO_MOUTH_GLAD, CHAO_FACE_HOLD);
        EQUIP(CHAO_NODE_HAND_4, chao_drumstick_4);
        EQUIP(CHAO_NODE_HAND_11, chao_drumstick_11);
        EQUIP(CHAO_NODE_MEDAL, chao_drum);
        behavior->timer = RANDOM_COUNT(300, 301);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- > 0) {
            break;
        }
        motion = chao_motion_index(chao);
        switch (motion) {
        case CHAO_ANIM_DRUM_LR:
            if (RANDOM_UNIT() < 0.6F) {
                chao_link_motion_synced(chao, CHAO_ANIM_DRUM_LR);
            } else {
                chao_link_motion_frames(chao, CHAO_ANIM_DRUM_DOUBLE, 10);
            }
            behavior->timer = RANDOM_COUNT(60, 181);
            break;
        case CHAO_ANIM_DRUM_DOUBLE:
            chao_link_motion_frames(chao, CHAO_ANIM_DRUM_LR, 10);
            behavior->timer = RANDOM_COUNT(60, 181);
            break;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
int chao_practice_cymbals(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int motion;
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_CYMBAL_WAIT);
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_set_timed_mouth(chao, CHAO_MOUTH_GLAD, CHAO_FACE_HOLD);
        EQUIP(CHAO_NODE_HAND_4, chao_cymbal_4);
        EQUIP(CHAO_NODE_HAND_11, chao_cymbal_11);
        behavior->timer = RANDOM_COUNT(300, 301);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- > 0) {
            break;
        }
        motion = chao_motion_index(chao);
        switch (motion) {
        case CHAO_ANIM_CYMBAL_WAIT:
            if (RANDOM_UNIT() < 0.6F) {
                if (RANDOM_UNIT() < 0.5F) {
                    chao_link_motion_frames(chao, CHAO_ANIM_CYMBAL_PAN, 10);
                } else {
                    chao_link_motion_frames(chao, CHAO_ANIM_CYMBAL_PANPAN, 10);
                }
            } else {
                chao_link_motion_frames(chao, CHAO_ANIM_CYMBAL_PAPAPAN, 10);
            }
            behavior->timer = RANDOM_COUNT(60, 181);
            break;
        default:
            chao_link_motion_frames(chao, CHAO_ANIM_CYMBAL_WAIT, 10);
            behavior->timer = RANDOM_COUNT(60, 181);
            break;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
int chao_practice_tambourine(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_TAMBOURINE_SHAKA);
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_set_timed_mouth(chao, CHAO_MOUTH_GLAD, CHAO_FACE_HOLD);
        EQUIP(CHAO_NODE_HAND_4, chao_tambourine);
        behavior->timer = RANDOM_COUNT(60, 61);
        behavior->mode++;
        break;
    case 1:
#ifdef NONMATCHING
#else
        /* @hack redundant: a second tambourine stroke behind if (0), which
         * never runs */
        if (0) {
            switch (rand() % 2) {
            case 1:
                chao_link_motion_frames(chao, CHAO_ANIM_TAMBOURINE_PAN, 10);
                behavior->timer = RANDOM_COUNT(60, 61);
            }
        }
#endif
        break;
    }
    return BEHAVIOR_CONTINUE;
}

const char *chao_practice_names[CHAO_PRACTICES] = {
    "Ojigi", "Laugh", "Sing", "Maracas", "Drum", "Cymbal", "Tambourine"};

chao_behavior_fn chao_practice_actions[CHAO_PRACTICES + 1] = {
    chao_bow,
    chao_laugh_seated,
    chao_practice_singing,
    chao_practice_maracas,
    chao_practice_drums,
    chao_practice_cymbals,
    chao_practice_tambourine,
    NULL};
