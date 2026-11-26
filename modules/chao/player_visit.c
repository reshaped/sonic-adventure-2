#include <ninja.h>
#include <sg_xpt.h>
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
#include "chao/player_visit.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "object/mover.h"
#include "sound/sound.h"
#include "system/task.h"

#define FOLLOW_COOLDOWN 0
#define GREETING_COOLDOWN 2
/* a Chao greets a character it has met fewer than GREET_MEETINGS times, or
 * after GREET_APART apart. it complains after as long. it follows or
 * confronts after FOLLOW_APART */
#define GREET_MEETINGS 2
#define GREET_APART 300
#define FOLLOW_APART 100
#define COMPLAIN_LIKING (-20)
#define FOLLOW_LIKING 60
#define CONFRONT_LIKING (-60)
#define CONFRONT_AGGRESSION 30
#define CONFRONT_ANGER 200
/* the square of the distance within which a following Chao reaches the
 * player */
#define NEAR_PLAYER 25.0F
#define FOLLOW_FRAMES 1000
#define FOLLOW_TURN 288
/* the pleasure past which a visit ends, and the loneliness it relieves */
#define ENJOYED_PLEASURE 140
#define FOLLOWED_PLEASURE 180
#define VISIT_RELIEF 6000

static int chao_greet_player(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);

    switch (behavior->mode) {
    case 0:
        if (((struct chao_work *)chao->work)->profile.parameters.alignment >
            0.5F) {
            chao_link_motion_frames(chao, CHAO_ANIM_OJIGI, 20);
            sound_play_at(0x6091, NULL, 0, 0, &chao->work->pos);
        } else {
            chao_link_motion_frames(chao, CHAO_ANIM_HELLO, 20);
        }
        chao_set_eye(chao, CHAO_EYE_GLAD);
        chao_set_mouth(chao, CHAO_MOUTH_GLAD);
        behavior->timer = 200;
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- <= 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_record_visit(struct task *chao) /* NOLINT(misc-use-internal-linkage) */
{
    chao_meet_player(chao);
    return BEHAVIOR_FINISH;
}

static int enjoy_player_visit(struct task *chao);
static int follow_liked_player(struct task *chao);

/* @hack spelling: record_visit_callback, which config/chao/symbols.txt
 * places at chao_record_visit, where chao_record_visit would do */
#ifdef NONMATCHING
/* chao_record_visit by its own name, the plain form the hack tag names. this
 * side's objects don't define the second name, which only symbols.txt places */
#define RECORD_VISIT chao_record_visit
#else
int record_visit_callback(struct task *chao);
#define RECORD_VISIT record_visit_callback
#endif

/* @hack redundant: limit, a copy of threshold, and tested, which names one
 * of the two, where threshold would do */
#define SCORE_LONELINESS(score, tuned_threshold, tuned_weight, tested)         \
    (score) = 0.0F;                                                            \
    loneliness = chao_emotion_get(chao, CHAO_LONELINESS);                      \
    threshold = chao_tuning.tuned_threshold;                                   \
    limit = threshold;                                                         \
    if ((Uint32)loneliness > (Uint32)(tested)) {                               \
        (score) = chao_need_above(loneliness, limit);                          \
        (score) *= chao_tuning.tuned_weight;                                   \
        chao_vary_score(&(score));                                             \
    }

void score_player_visit(struct task *chao, Float *best)
{
    struct task *player;
    Uint32 follow_wait;
    Uint32 greeting_wait;
    int liking;
    int loneliness;
    int threshold;
    int limit;
    Float greet;
    Float complain;
    Float follow;
    Float confront;

    /* @hack redundant: chao_emotion_get named before its first call */
    (void)chao_emotion_get;
    player = chao_visible_player_task(chao);
    follow_wait = chao_cooldown(chao, FOLLOW_COOLDOWN);
    greeting_wait = chao_cooldown(chao, GREETING_COOLDOWN);
    if (!player) {
        return;
    }
    liking = chao_player_liking(chao);
    chao_player_fear(chao);
    if (!greeting_wait) {
        if (liking >= 0 && (chao_meetings(chao) < GREET_MEETINGS ||
                            chao_time_apart(chao) > GREET_APART)) {
            SCORE_LONELINESS(greet, greet_loneliness, greet_weight, limit);
            if (greet > *best) {
                chao_set_behavior(chao, chao_greet_player);
                chao_queue_behavior(chao, RECORD_VISIT);
                chao_clear_intention(chao);
                *best = greet;
            }
        }
        if (liking < COMPLAIN_LIKING && chao_time_apart(chao) > GREET_APART) {
            SCORE_LONELINESS(complain, greet_loneliness, greet_weight,
                             threshold);
            if (complain > *best) {
                chao_set_behavior(chao, chao_complain_at_player);
                chao_queue_behavior(chao, RECORD_VISIT);
                chao_clear_intention(chao);
                *best = complain;
            }
        }
    }
    if (!follow_wait) {
        if (liking > FOLLOW_LIKING && chao_time_apart(chao) > FOLLOW_APART) {
            SCORE_LONELINESS(follow, follow_loneliness, follow_weight, limit);
            if (follow > *best) {
                chao_set_behavior(chao, follow_liked_player);
                chao_clear_intention(chao);
                *best = follow;
            }
        }
        if (liking < CONFRONT_LIKING && chao_is_dark(chao) &&
            chao_time_apart(chao) > FOLLOW_APART &&
            chao_emotion_get(chao, CHAO_AGGRESSION) > CONFRONT_AGGRESSION) {
            SCORE_LONELINESS(confront, follow_loneliness, follow_weight, limit);
            if (confront > *best) {
                chao_emotion_add(chao, CHAO_ANGER, CONFRONT_ANGER);
                chao_queue_behavior(chao, chao_confront_player);
                chao_clear_intention(chao);
                *best = confront;
            }
        }
    }
}

static int enjoy_player_visit(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_SURISURI, 20);
        chao_set_eye(chao, 3);
        chao_set_mouth(chao, CHAO_MOUTH_GLAD);
        behavior->mode++;
        /* fall through */
    case 1:
        if (chao_motion_ended(chao)) {
            if (mover_player_distance2(chao, 0) < NEAR_PLAYER) {
                chao_emotion_add(chao, CHAO_PLEASURE, 10);
                chao_set_behavior(chao, enjoy_player_visit);
                if (chao_emotion_get(chao, CHAO_PLEASURE) > ENJOYED_PLEASURE) {
                    chao_emotion_add(chao, CHAO_LONELINESS, -VISIT_RELIEF);
                    return BEHAVIOR_FINISH;
                }
            } else {
                chao_set_behavior(chao, follow_liked_player);
            }
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

static int follow_liked_player(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    struct mover *mover = chao->motion.p;

    chao_skill_points(chao, CHAO_SWIM);
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_SKIP, 10);
        chao_set_eye(chao, CHAO_EYE_GLAD);
        chao_set_mouth(chao, CHAO_MOUTH_GLAD);
        behavior->mode++;
        behavior->timer = 0;
        behavior->count = 0;
        if (chao_emotion_get(chao, CHAO_PLEASURE) > FOLLOWED_PLEASURE) {
            chao_emotion_add(chao, CHAO_LONELINESS, -VISIT_RELIEF);
            return BEHAVIOR_FINISH;
        }
        /* fall through */
    case 1:
        break;
    }
    if (behavior->timer++ > FOLLOW_FRAMES) {
        return BEHAVIOR_FINISH;
    }
    if (mover_player_distance2(chao, 0) < NEAR_PLAYER) {
        chao_emotion_add(chao, CHAO_PLEASURE, 10);
        chao_set_behavior(chao, enjoy_player_visit);
        chao_queue_behavior(chao, follow_liked_player);
    }
    mover_turn_to_player_by(chao, FOLLOW_TURN, 0);
    if (mover->flags & MOVER_ON_FLOOR) {
        mover->acc.x = njSin(work->base.ang.y) * chao_tuning.walk_speed;
        mover->acc.z = njCos(work->base.ang.y) * chao_tuning.walk_speed;
    }
    return BEHAVIOR_CONTINUE;
}
