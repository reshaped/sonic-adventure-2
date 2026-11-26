/* a garden Chao the player holds. a held Chao wakes, enjoys the hold, waits
 * or protests. a Chao let go is set down, thrown, or hops off */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/behavior.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/garden.h"
#include "chao/held.h"
#include "chao/icon.h"
#include "chao/idle.h"
#include "chao/knowledge.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/sound.h"
#include "chao/tuning.h"
#include "chao/ui.h"
#include "chao/work.h"
#include "collision/collision.h"
#include "object/mover.h"
#include "player/player.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"

/* a held Chao's heading is HELD_TURN less player 0's heading. HELD_TURN turns
 * toward HELD_TURN_TARGET by TURN_STEP a step. HELD_TURN is the mover's aim_y,
 * and HELD_TURN_TARGET its aim_x */
/* @hack type: the mover's aim_x and aim_y read as x and y of a struct angles
 * laid at aim_x, where the mover's own fields would do */
#ifdef NONMATCHING
/* the plain form the hack tag names: the mover's own fields */
#define HELD_TURN_TARGET(turns) (turns)->aim_x
#define HELD_TURN(turns) (turns)->aim_y
#else
#define HELD_TURN_TARGET(turns) (turns)->x
#define HELD_TURN(turns) (turns)->y
#endif
#define TURN_STEP 0x800
/* a Chao's heading drops by PICK_UP_TWIST as it is picked up */
#define PICK_UP_TWIST 4000

/* each time more than RESTLESS_STEPS steps pass, a held Chao's hunger moves
 * it on by one behavior. past WAIT_HUNGER, a Chao that enjoys the hold waits.
 * past PROTEST_HUNGER, a waiting Chao protests. at CHAO_NEED_MAX, a protesting
 * Chao is let go in the three gardens */
#define RESTLESS_STEPS 600
#define WAIT_HUNGER 5000
#define PROTEST_HUNGER 8000
/* a Chao picked up with a sleep depth above SLEEPING_DEPTH wakes by WAKE_STEP
 * a step. it shows anger for ANGER_STEPS, and again each time its timer
 * passes ANGER_REPEAT */
#define SLEEPING_DEPTH 500
#define WAKE_STEP 2
#define ANGER_STEPS 360
#define ANGER_REPEAT 400
/* any other Chao picked up enjoys the hold where its chao_player_liking is
 * above LIKING_MARGIN. it protests below -LIKING_MARGIN, and waits between. it
 * gains 1 affection where its relaxation is below UNRELAXED */
#define LIKING_MARGIN 50
#define UNRELAXED 10
/* a Chao that enjoys a hold picks a motion each ENJOY_STEPS and a random part
 * of ENJOY_SPAN. CHAO_ANIM_DAKKO lasts CALM_STEPS and the same part. a Chao
 * that protests picks one each PROTEST_STEPS and a random part of
 * PROTEST_SPAN */
#define ENJOY_STEPS 140
#define CALM_STEPS 200
#define ENJOY_SPAN 181.0F
#define PROTEST_STEPS 100
#define PROTEST_SPAN 141.0F
/* the first of three protest voices in a row */
#define PROTEST_VOICE 0x6047
/* a Chao let go while the pads are off rises HOP_LIFT. it hops up at
 * HOP_SPEED and drifts at HOP_DRIFT the way it moved */
#define HOP_LIFT 2.0F
#define HOP_SPEED 0.87F
#define HOP_DRIFT 0.2F

static int enjoy_being_held(struct task *chao);
static int wait_while_held(struct task *chao);
static int protest_being_held(struct task *chao);
static int wake_while_held(struct task *chao);
static int choose_held_behavior(struct task *chao);

static void cancel_held_gravity(struct mover *mover)
{
    mover->acc.y += -mover->gravity;
}

/* the step each held behavior ends with. a held Chao turns with player 0. a
 * Chao let go while the pads are off hops off and waits. one let go while
 * player 0 is offered the set-down is set down, and any other is thrown */
static void follow_holder(struct task *chao)
{
    struct task_work *work = chao->work;
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct mover *mover = chao->motion.p;
#ifdef NONMATCHING
    /* the plain form of the hack tagged at HELD_TURN: the mover itself */
    struct mover *turns = mover;
#else
    struct angles *turns = (struct angles *)&mover->aim_x;
#endif
    struct player *player;
    NJS_VECTOR velocity;
    Uint32 hunger;
    Bool set_down;
    int i;
    cancel_held_gravity(mover);
    chao_emotion_add(chao, CHAO_RELAXATION, CHAO_FEELING_MAX);
    if (!(work->flags & WORK_HELD_BY_PLAYER)) {
        if (!pad_is_enabled()) {
            /* @hack redundant: mover read from chao->motion again, as its
             * declaration did */
            mover = chao->motion.p;
            velocity.x = mover->spd.x;
            velocity.y = 0.0F;
            velocity.z = mover->spd.z;
            if (njScalor(&velocity) > 0.0F) {
                njUnitVector(&velocity);
            }
            work->pos.y += HOP_LIFT;
            velocity.x *= HOP_DRIFT;
            velocity.y = HOP_SPEED;
            velocity.z *= HOP_DRIFT;
            mover_set_spd(chao, &velocity);
            chao_set_behavior(chao, chao_wait_held_behavior);
        } else {
            /* PLAYER_MODE_UNK_14 among player 0's offered actions is the
             * set-down */
            player = player_records[0];
            set_down = FALSE;
            for (i = 0; i < player->action_count; i++) {
                if (player->actions[i] == PLAYER_MODE_UNK_14) {
                    mover_clear_spd(chao);
                    mover_clear_acc(chao);
                    HELD_TURN_TARGET(turns) = 0;
                    chao_set_behavior(chao, chao_idle_behavior);
                    CHAO_WORK(chao)->flags |= CHAO_COLLIDES;
                    set_down = TRUE;
                    break;
                }
            }
            if (!set_down) {
                HELD_TURN_TARGET(turns) = 0;
                chao_set_behavior(chao, chao_thrown_behavior);
                CHAO_WORK(chao)->flags |= CHAO_COLLIDES;
            }
        }
    } else {
        /* @hack spelling: chao->work read again, where work holds it, and
         * the negated heading added, where HELD_TURN less the heading would
         * do */
        chao->work->ang.y = -player_work[0]->ang.y + HELD_TURN(turns);
        HELD_TURN(turns) = angle_approach(HELD_TURN(turns),
                                          HELD_TURN_TARGET(turns), TURN_STEP);
        mover_clear_spd(chao);
        mover_clear_acc(chao);
        if (behavior->count++ > RESTLESS_STEPS) {
            hunger = chao_emotion_get(chao, CHAO_HUNGER);
            if (hunger >= CHAO_NEED_MAX) {
                if (chao_current_behavior(chao) == protest_being_held) {
                    switch (chao_current_area()) {
                    case CHAO_AREA_NEUTRAL:
                    case CHAO_AREA_HERO:
                    case CHAO_AREA_DARK:
                        player_let_go(0);
                        chao_set_behavior(chao, chao_idle_behavior);
                    }
                }
            }
            if (hunger > PROTEST_HUNGER) {
                if (chao_current_behavior(chao) == wait_while_held) {
                    chao_set_behavior(chao, protest_being_held);
                }
            }
            if (hunger > WAIT_HUNGER) {
                if (chao_current_behavior(chao) == enjoy_being_held) {
                    chao_set_behavior(chao, wait_while_held);
                }
            }
            behavior->count = 0;
        }
    }
}

static int enjoy_being_held(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    Float choice;
    switch (behavior->mode) {
    case 0:
        chao_set_eye(chao, CHAO_EYE_GLAD);
        if (RANDOM_UNIT() < 0.5F) {
            chao_set_mouth(chao, 5);
        } else {
            chao_set_mouth(chao, CHAO_MOUTH_GLAD);
        }
        choice = RANDOM_UNIT();
        behavior->timer = RANDOM_COUNT(ENJOY_STEPS, ENJOY_SPAN);
        if (choice < 0.25F) {
            chao_link_motion(chao, CHAO_ANIM_DAKKO);
            chao_restore_eyes(chao);
            chao_restore_mouth(chao);
            behavior->timer = RANDOM_COUNT(CALM_STEPS, ENJOY_SPAN);
        } else if (choice < 0.4F) {
            chao_link_motion(chao, CHAO_ANIM_DAKKO_KIMO_LR);
        } else if (choice < 0.55F) {
            chao_link_motion(chao, CHAO_ANIM_DAKKO_KIMO_FB);
        } else if (choice < 0.7F) {
            chao_link_motion(chao, CHAO_ANIM_DAKKO_KIMO_JITABATA);
        } else if (choice < 0.85F) {
            chao_link_motion(chao, CHAO_ANIM_DAKKO_GLAD);
        } else {
            chao_link_motion(chao, CHAO_ANIM_DAKKO_GLAD_CLAP);
        }
        /* @hack spelling: the Chao's work cast again, where work holds it */
        CHAO_WORK(chao)->flags &= ~CHAO_COLLIDES;
        behavior->mode++;
        break;
    case 1:
        if (behavior->timer-- <= 0) {
            behavior->mode = 0;
        }
        break;
    }
    follow_holder(chao);
    return BEHAVIOR_CONTINUE;
}

static int wait_while_held(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_DAKKO);
        /* @hack spelling: the Chao's work cast again, where work holds it */
        CHAO_WORK(chao)->flags &= ~CHAO_COLLIDES;
        behavior->mode++;
        break;
    }
    follow_holder(chao);
    return BEHAVIOR_CONTINUE;
}

static int protest_being_held(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    Float choice;
    switch (behavior->mode) {
    case 0:
        chao_set_eye(chao, CHAO_EYE_DISTRESSED);
        chao_set_mouth(chao, 4);
        choice = RANDOM_UNIT();
        if (choice < 0.3F) {
            chao_link_motion(chao, CHAO_ANIM_DAKKO);
            chao_restore_eyes(chao);
            chao_restore_mouth(chao);
        } else if (choice < 0.5F) {
            chao_link_motion(chao, CHAO_ANIM_DAKKO_IYA);
        } else if (choice < 0.6F) {
            chao_link_motion(chao, CHAO_ANIM_DAKKO_IYA_LR);
        } else if (choice < 0.7F) {
            chao_link_motion(chao, CHAO_ANIM_DAKKO_IYA_UD);
        } else if (choice < 0.8F) {
            chao_link_motion(chao, CHAO_ANIM_DAKKO_IYA_HAND);
        } else if (choice < 0.9F) {
            chao_link_motion(chao, CHAO_ANIM_DAKKO_IYA_L);
        } else {
            chao_link_motion(chao, CHAO_ANIM_DAKKO_IYA_R);
        }
        behavior->timer = RANDOM_COUNT(PROTEST_STEPS, PROTEST_SPAN);
        if (RANDOM_UNIT() < 0.4F) {
            choice = RANDOM_UNIT();
            if (choice < 0.33F) {
                /* @hack spelling: chao->work read again for pos, here and in
                 * the two calls below, where work's base would do */
                sound_play_at(PROTEST_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                              &chao->work->pos);
            } else if (choice < 0.66F) {
                sound_play_at(PROTEST_VOICE + 1, NULL, 0, GARDEN_VOICE_VOLUME,
                              &chao->work->pos);
            } else {
                sound_play_at(PROTEST_VOICE + 2, NULL, 0, GARDEN_VOICE_VOLUME,
                              &chao->work->pos);
            }
        }
        behavior->mode++;
        /* @hack spelling: the Chao's work cast again, where work holds it */
        CHAO_WORK(chao)->flags &= ~CHAO_COLLIDES;
        break;
    case 1:
        if (behavior->timer-- <= 0) {
            behavior->mode = 0;
        }
        break;
    }
    follow_holder(chao);
    return BEHAVIOR_CONTINUE;
}

static int wake_while_held(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_DAKKO);
        chao_set_timed_eye(chao, CHAO_EYE_HALF_SHUT, CHAO_FACE_HOLD);
        chao_show_mark(chao, CHAO_MARK_ANGER, ANGER_STEPS);
        CHAO_WORK(chao)->flags &= ~CHAO_COLLIDES;
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer++ > ANGER_REPEAT) {
            chao_show_mark(chao, CHAO_MARK_ANGER, ANGER_STEPS);
            behavior->timer = 0;
        }
        chao_emotion_add(chao, CHAO_SLEEP_DEPTH, -WAKE_STEP);
        if (chao_emotion_get(chao, CHAO_SLEEP_DEPTH) == 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    follow_holder(chao);
    return BEHAVIOR_CONTINUE;
}
static int choose_held_behavior(struct task *chao)
{
    int liking;
    if (chao_emotion_get(chao, CHAO_SLEEP_DEPTH) > SLEEPING_DEPTH) {
        chao_set_behavior(chao, wake_while_held);
    } else {
        liking = chao_player_liking(chao);
        if (liking > LIKING_MARGIN) {
            chao_set_behavior(chao, enjoy_being_held);
        } else if (liking < -LIKING_MARGIN) {
            chao_set_behavior(chao, protest_being_held);
        } else {
            chao_set_behavior(chao, wait_while_held);
        }
        if (chao_emotion_get(chao, CHAO_RELAXATION) < UNRELAXED) {
            chao_player_affection(chao, 1, chao_tuning.held_alignment);
        }
    }
    CHAO_WORK(chao)->flags &= ~CHAO_COLLIDES;
    follow_holder(chao);
    return BEHAVIOR_CONTINUE;
}
int chao_try_player_hold(struct task *chao)
{
    chao_behavior_fn current = chao_current_behavior(chao);
#ifdef NONMATCHING
    /* the plain form of the hack tagged at HELD_TURN: the mover itself */
    struct mover *turns;
#else
    struct angles *turns;
#endif
    if (current != choose_held_behavior && current != enjoy_being_held &&
        current != wait_while_held && current != protest_being_held &&
        current != wake_while_held) {
        if (chao->work->flags & WORK_HELD_BY_PLAYER) {
#ifdef NONMATCHING
            /* the plain form of the hack tagged at HELD_TURN: the mover
             * itself */
            turns = chao->motion.p;
#else
            turns = (struct angles *)&((struct mover *)chao->motion.p)->aim_x;
#endif
            HELD_TURN(turns) =
                chao->work->ang.y + player_work[0]->ang.y - PICK_UP_TWIST;
            if (angle_distance(mover_player_heading(chao, 0),
                               chao->work->ang.y) < QUARTER_TURN) {
                HELD_TURN_TARGET(turns) = HALF_TURN + QUARTER_TURN;
            } else {
                HELD_TURN_TARGET(turns) = QUARTER_TURN;
            }
            mover_clear_spd(chao);
            mover_clear_acc(chao);
            chao_set_behavior(chao, choose_held_behavior);
            garden_create_name_label(chao);
            return TRUE;
        }
    }
    return FALSE;
}
