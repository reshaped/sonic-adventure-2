/* a Dark Chao's bid and behaviors of anger at a player it dislikes */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/aggression.h"
#include "chao/behavior.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/intention.h"
#include "chao/knowledge.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/perception.h"
#include "chao/race_chao.h"
#include "chao/sound.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "object/mover.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

#define VOICE(id)                                                              \
    sound_play_at((id), NULL, 0, GARDEN_VOICE_VOLUME, &chao->work->pos)
/* a Chao with more kindness bids nothing */
#define GENTLE_KINDNESS 30
/* the least aggression to confront a player, and to complain at one */
#define CONFRONT_AGGRESSION 30
#define COMPLAIN_AGGRESSION (-50)
#define SULK_LEAST 180
#define SULK_CHOICES 301
#define SULK_VOICE_FRAMES 150 /* how often a sulking Chao may cry out */
#define CONFRONT_FRAMES 1000
#define STRIKE_RANGE 5.0F
#define SULK_RANGE 10.0F
#define STRIKE_RELIEF 10 /* the anger each strike takes away */
#define LEAST_ANGER 10 /* the anger a confrontation needs to begin */
#define CONFRONT_TURN 0x120
#define ANGRY_FACE()                                                           \
    {                                                                          \
        chao_set_eye(chao, CHAO_EYE_NARROW);                                   \
        /* @hack type: the default mouth narrowed to a Sint16, where the int   \
         * would do */                                                         \
        if ((Sint16)chao_default_mouth(chao) == 1) {                           \
            chao_set_mouth(chao, 9);                                           \
        } else {                                                               \
            chao_set_mouth(chao, 4);                                           \
        }                                                                      \
    }
#define PLAYER_ANGER_BID()                                                     \
    {                                                                          \
        score = 0.0F;                                                          \
        if (anger > threshold) {                                               \
            score = chao_feeling_above(anger, threshold);                      \
            score *= chao_tuning.anger_player_weight;                          \
            chao_vary_score(&score);                                           \
        }                                                                      \
        bid = TRUE;                                                            \
    }

static int sulk(struct task *chao);

void score_player_dislike(struct task *chao, Float *best)
{
    Float score;
    Uint32 threshold;
    Uint32 anger;
    Bool bid;
    struct task *player;
    int liking;
    score = 0.0F;
    threshold = chao_tuning.anger_threshold;
    anger = chao_emotion_get(chao, CHAO_ANGER);
    if (chao_emotion_get(chao, CHAO_KINDNESS) > GENTLE_KINDNESS) {
        return;
    }
    if (!chao_is_dark(chao)) {
        return;
    }
    if (anger <= chao_tuning.anger_threshold) {
        return;
    }
    bid = FALSE;
    player = chao_visible_player_task(chao);
    if (player) {
        liking = chao_player_liking(chao);
        if (liking < 0) {
            if (chao_emotion_get(chao, CHAO_AGGRESSION) > CONFRONT_AGGRESSION &&
                rand() * RAND_SCALE < 0.7F) {
                PLAYER_ANGER_BID();
                if (score > *best) {
                    chao_set_behavior(chao, sulk);
                    chao_queue_behavior(chao, chao_confront_player);
                    chao_clear_intention(chao);
                    *best = score;
                }
            } else if (chao_emotion_get(chao, CHAO_AGGRESSION) >
                       COMPLAIN_AGGRESSION) {
                PLAYER_ANGER_BID();
                if (score > *best) {
                    chao_set_behavior(chao, chao_complain_at_player);
                    chao_clear_intention(chao);
                    *best = score;
                }
            }
        }
    }
    if (!bid) {
        score = 1.0F - ((100 - chao_emotion_get(chao, CHAO_AGGRESSION)) *
                        (1.0F - chao_tuning.anger_idle_weight) * 0.005F);
        if (score > *best) {
            chao_set_behavior(chao, sulk);
            chao_clear_intention(chao);
            *best = score;
        }
    }
}
static int sulk(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    Float choice;
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_ANGRY_STAND, 20);
        behavior->mode++;
        ANGRY_FACE();
        /* @hack type: the timer narrowed to a Uint16, where the int would do */
        behavior->timer =
            (Uint16)(SULK_LEAST + (int)(rand() * (SULK_CHOICES * RAND_SCALE)));
        /* fall through */
    case 1:
        if (behavior->timer % SULK_VOICE_FRAMES == 0) {
            if (rand() * RAND_SCALE < 0.5F) {
                choice = rand() * RAND_SCALE;
                if (choice < 0.33F) {
                    VOICE(0x6065);
                } else if (choice < 0.66F) {
                    VOICE(0x6066);
                } else {
                    VOICE(0x6068);
                }
            }
        }
        if (--behavior->timer == 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

/* a behavior that strikes once with motion, and finishes as it ends */
#define ANGRY_GESTURE(name, motion)                                            \
    static int name(struct task *chao)                                         \
    {                                                                          \
        struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);                  \
        switch (behavior->mode) {                                              \
        case 0:                                                                \
            chao_link_motion_frames(chao, (motion), 20);                       \
            behavior->mode++;                                                  \
            ANGRY_FACE();                                                      \
            /* fall through */                                                 \
        case 1:                                                                \
            if (chao_motion_ended(chao)) {                                     \
                return BEHAVIOR_FINISH;                                        \
            }                                                                  \
            break;                                                             \
        }                                                                      \
        return BEHAVIOR_CONTINUE;                                              \
    }
ANGRY_GESTURE(punch, CHAO_ANIM_PUNCH)
ANGRY_GESTURE(pummel, CHAO_ANIM_POKAPOKA)
ANGRY_GESTURE(strike_with_knuckles, CHAO_ANIM_GENKOTU)
ANGRY_GESTURE(kick, CHAO_ANIM_KICK)

int chao_complain_at_player(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_AKANBE, 20);
        behavior->mode++;
        chao_set_eye(chao, CHAO_EYE_DISTRESSED);
        chao_set_mouth(chao, 4);
        VOICE(0x6069);
        /* fall through */
    case 1:
        if (chao_motion_ended(chao)) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_confront_player(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    struct task_work *work = chao->work;
    struct mover *mover = chao->motion.p;
    Float distance2;
    Float choice;

    chao_skill_points(chao, CHAO_SWIM);
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_ANGER_WALK, 10);
        ANGRY_FACE();
        behavior->mode++;
        behavior->timer = 0;
        behavior->count = 0;
        if (chao_emotion_get(chao, CHAO_ANGER) < LEAST_ANGER) {
            return BEHAVIOR_FINISH;
        }
        /* fall through */
    case 1:
        break;
    }
    /* @hack redundant: an empty test of the tick's result, where the call
     * alone would do */
    if (chao_take_upkeep_tick(chao)) {
    }
    if (behavior->timer++ > CONFRONT_FRAMES) {
        return BEHAVIOR_FINISH;
    }
    distance2 = mover_player_distance2(chao, 0);
    if (distance2 < STRIKE_RANGE * STRIKE_RANGE) {
        chao_emotion_add(chao, CHAO_ANGER, -STRIKE_RELIEF);
        choice = rand() * RAND_SCALE;
        if (choice < 0.35F) {
            chao_set_behavior(chao, punch);
        } else if (choice < 0.7F) {
            chao_set_behavior(chao, kick);
        } else if (choice < 0.9F) {
            chao_set_behavior(chao, pummel);
        } else {
            chao_set_behavior(chao, strike_with_knuckles);
        }
        chao_queue_behavior(chao, chao_confront_player);
        if (rand() * RAND_SCALE < 0.6F) {
            choice = rand() * RAND_SCALE;
            if (choice < 0.25F) {
                VOICE(0x606f);
            } else if (choice < 0.5F) {
                VOICE(0x6070);
            } else if (choice < 0.75F) {
                VOICE(0x6071);
            } else {
                VOICE(0x6072);
            }
        }
    }
    if (distance2 > SULK_RANGE * SULK_RANGE) {
        chao_set_behavior(chao, sulk);
    }
    mover_turn_to_player_by(chao, CONFRONT_TURN, 0);
    if (mover->flags & MOVER_ON_FLOOR) {
        mover->acc.x = njSin(work->ang.y) * chao_tuning.walk_speed;
        mover->acc.z = njCos(work->ang.y) * chao_tuning.walk_speed;
    }
    return BEHAVIOR_CONTINUE;
}
