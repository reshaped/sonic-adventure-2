#include <ninja.h>
#include <sg_xpt.h>

#include "chao/behavior.h"
#include "chao/body_collision.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/garden_field.h"
#include "chao/garden_toys.h"
#include "chao/idle.h"
#include "chao/intention.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/parts.h"
#include "chao/perception.h"
#include "chao/spawn_grid.h"
#include "chao/tuning.h"
#include "chao/walk.h"
#include "chao/work.h"
#include "chao/world.h"
#include "object/animal.h"
#include "object/mover.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the index of the cooldown that keeps a Chao from toys after it plays */
#define TOY_COOLDOWN 8
/* a sit by the TV or the radio lasts SIT_STEPS and a random part of SIT_SPAN.
 * once the signal stops reaching the Chao, it claps for CLAP_STEPS and a
 * random part of CLAP_SPAN. after a broadcast or a ride, it keeps from toys
 * for REST_STEPS and a random part of REST_SPAN */
#define SIT_STEPS 300
#define SIT_SPAN 301.0F
#define CLAP_STEPS 120
#define CLAP_SPAN 91.0F
#define REST_STEPS 1800
#define REST_SPAN 1801.0F
/* a ride lasts RIDE_STEPS. each step the Chao closes SADDLE_PULL of its gap
 * to the saddle. it turns up to HORSE_TURN toward the horse's heading */
#define RIDE_STEPS 1800
#define SADDLE_PULL 0.1F
#define HORSE_TURN 0x400
/* a Chao jumps on at MOUNT_SPEED ahead and MOUNT_LIFT up. it links the ride's
 * start over MOUNT_FRAMES, and rides MOUNT_FRAMES - 1 steps later. a field of
 * GARDEN_TOY_ACTIVATE, ACTIVATE_RADIUS wide, then sets off the horse for
 * ACTIVATE_STEPS. it jumps off at DISMOUNT_SPEED to its right and DISMOUNT_LIFT
 * up */
#define MOUNT_SPEED 0.08F
#define MOUNT_LIFT 0.65F
#define MOUNT_FRAMES 30
#define ACTIVATE_RADIUS 3.5F
#define ACTIVATE_STEPS 10
#define DISMOUNT_SPEED 0.3F
#define DISMOUNT_LIFT 0.8F
/* scoring a broadcast in reach, a Chao counts BROADCAST_BOREDOM more boredom
 * while it has less than BROADCAST_BOREDOM_CAP. a broadcast that wins bids
 * BROADCAST_BID */
#define BROADCAST_BOREDOM 3000
#define BROADCAST_BOREDOM_CAP 7000
#define BROADCAST_BID 0.99F
/* the boredom a Chao sheds when it chooses a toy */
#define BOREDOM_RELIEF 5000
/* a Chao doesn't go to a horse that stands on NO_RIDE_TERRAIN */
#define NO_RIDE_TERRAIN 3

/* a playing Chao shows the glad eye or its default, each half the time. it
 * picks its mouth the same way */
#define CHOOSE_PLAYFUL_FACE(chao)                                              \
    {                                                                          \
        if (RANDOM_UNIT() < 0.5F) {                                            \
            chao_set_timed_eye((chao), CHAO_EYE_GLAD, CHAO_FACE_HOLD);         \
        } else {                                                               \
            chao_restore_eyes((chao));                                         \
        }                                                                      \
        if (RANDOM_UNIT() < 0.5F) {                                            \
            chao_set_timed_mouth((chao), CHAO_MOUTH_GLAD, CHAO_FACE_HOLD);     \
        } else {                                                               \
            chao_restore_mouth((chao));                                        \
        }                                                                      \
    }

#define SIT_NEAR_TOY(name, field)                                              \
    static int name(struct task *chao)                                         \
    {                                                                          \
        struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;           \
        Float choice;                                                          \
                                                                               \
        switch (behavior->mode) {                                              \
        case 0:                                                                \
            choice = RANDOM_UNIT();                                            \
            if (choice < 0.3F) {                                               \
                chao_link_motion(chao, CHAO_ANIM_HIMA_SIT);                    \
            } else if (choice < 0.7F) {                                        \
                chao_link_motion(chao, CHAO_ANIM_HIMA_SIT_GUDE);               \
            } else {                                                           \
                chao_link_motion(chao, CHAO_ANIM_LAUGH_SIT);                   \
            }                                                                  \
            if (chao_has_eaten_minimal(chao, ANIMAL_KUJA)) {                   \
                if (RANDOM_UNIT() < 0.3F &&                                    \
                    chao_motion_index(chao) != CHAO_ANIM_OTAKAI_L &&           \
                    chao_motion_index(chao) != CHAO_ANIM_OTAKAI_R) {           \
                    if (RANDOM_UNIT() < 0.5F) {                                \
                        chao_link_motion(chao, CHAO_ANIM_OTAKAI_L);            \
                    } else {                                                   \
                        chao_link_motion(chao, CHAO_ANIM_OTAKAI_R);            \
                    }                                                          \
                }                                                              \
            }                                                                  \
            CHOOSE_PLAYFUL_FACE(chao);                                         \
            behavior->timer = RANDOM_COUNT(SIT_STEPS, SIT_SPAN);               \
            behavior->mode++;                                                  \
            /* fall through */                                                 \
        case 1:                                                                \
            if (behavior->timer-- <= 0) {                                      \
                behavior->mode = 0;                                            \
            }                                                                  \
            if (!garden_hit_kind(chao, CHAO_PUSH_SHAPE, (field))) {            \
                chao_link_motion(chao, CHAO_ANIM_HAKUSYU_SIT);                 \
                behavior->timer = RANDOM_COUNT(CLAP_STEPS, CLAP_SPAN);         \
                behavior->mode = 2;                                            \
            }                                                                  \
            break;                                                             \
        case 2:                                                                \
            if (behavior->timer-- <= 0) {                                      \
                chao_set_cooldown(chao, TOY_COOLDOWN,                          \
                                  RANDOM_COUNT(REST_STEPS, REST_SPAN));        \
                return BEHAVIOR_FINISH;                                        \
            }                                                                  \
            break;                                                             \
        }                                                                      \
        return BEHAVIOR_CONTINUE;                                              \
    }

SIT_NEAR_TOY(watch_television, GARDEN_TV_SIGNAL)
SIT_NEAR_TOY(listen_to_radio, GARDEN_RADIO_SIGNAL)

static int dismount_horse(struct task *chao)
{
    struct task_work *work = chao->work;
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct mover *mover = chao->motion.p;
    NJS_VECTOR jump;

    switch (behavior->mode) {
    case 0:
        chao_set_motion(chao, CHAO_ANIM_LOOKUP_JUMP);
        jump.x = njSin(work->ang.y + QUARTER_TURN) * DISMOUNT_SPEED;
        jump.y = DISMOUNT_LIFT;
        jump.z = njCos(work->ang.y + QUARTER_TURN) * DISMOUNT_SPEED;
        mover_set_spd(chao, &jump);
        chao_resist_displacement(chao);
        break;
    }
    if (mover->flags & MOVER_ON_FLOOR) {
        return BEHAVIOR_FINISH;
    }
    return BEHAVIOR_CONTINUE;
}

static int ride_horse(struct task *chao)
{
    struct task_work *work = chao->work;
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct mover *mover = chao->motion.p;
    NJS_VECTOR jump;
    struct garden_entry *horse;
    NJS_POINT3 *position;
    NJS_POINT3 *saddle;
    Float approach;

    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_LOOKUP_JUMP);
        CHOOSE_PLAYFUL_FACE(chao);
        chao_resist_displacement(chao);
        jump.x = njSin(work->ang.y) * MOUNT_SPEED;
        jump.y = MOUNT_LIFT;
        jump.z = njCos(work->ang.y) * MOUNT_SPEED;
        mover_set_spd(chao, &jump);
        behavior->mode++;
        /* fall through */
    case 1:
        if (mover->spd.y < 0.0F) {
            chao_link_motion_frames(chao, CHAO_ANIM_HORSE_START, MOUNT_FRAMES);
            CHAO_WORK(chao)->flags &= ~CHAO_MOVES;
            behavior->timer = MOUNT_FRAMES - 1;
            behavior->mode++;
        }
        break;
    case 2:
        if (--behavior->timer <= 0) {
            chao_set_motion(chao, CHAO_ANIM_HORSE_YURAYURA);
            garden_create_timed_field(chao, GARDEN_TOY_ACTIVATE, &work->pos,
                                      ACTIVATE_RADIUS, ACTIVATE_STEPS);
            behavior->timer = RIDE_STEPS;
            behavior->mode++;
        }
        break;
    case 3:
        if (--behavior->timer <= 0) {
            chao_set_cooldown(chao, TOY_COOLDOWN,
                              RANDOM_COUNT(REST_STEPS, REST_SPAN));
            CHAO_WORK(chao)->flags |= CHAO_MOVES;
            chao_set_behavior(chao, dismount_horse);
            return BEHAVIOR_FINISH;
        }
        break;
    }
    horse = garden_mutual_partner(chao);
    if (horse) {
        position = &work->pos;
        saddle = &horse->task->work->pos;
        approach = SADDLE_PULL;
        switch (behavior->mode) {
        case 3:
        case 2:
            position->x += (saddle->x - position->x) * approach;
            position->y += (saddle->y - position->y) * approach;
            position->z += (saddle->z - position->z) * approach;
            /* fall through */
        case 1:
            work->ang.y = angle_approach(work->ang.y, horse->task->work->ang.y,
                                         HORSE_TURN);
            break;
        }
    } else {
        chao_set_behavior(chao, chao_idle_behavior);
        return BEHAVIOR_FINISH;
    }
    return BEHAVIOR_CONTINUE;
}

#define APPROACH_TOY(name, find_seat, activity)                                \
    static int name(struct task *chao)                                         \
    {                                                                          \
        struct task *toy;                                                      \
        NJS_POINT3 seat;                                                       \
        toy = chao_nearest_toy_task(chao);                                     \
        if (toy && (find_seat)(&seat)) {                                       \
            garden_set_target(chao, toy);                                      \
            mover_set_target(chao, &seat);                                     \
            chao_set_behavior(chao, chao_stand_up_behavior);                   \
            chao_queue_behavior(chao, chao_pause_with_mark_behavior);          \
            chao_queue_behavior(chao, chao_walk_to_target_behavior);           \
            chao_queue_behavior(chao, chao_switch_on_toy_behavior);            \
            chao_queue_behavior(chao, chao_walk_to_point_behavior);            \
            chao_queue_behavior(chao, chao_face_target_behavior);              \
            chao_queue_behavior(chao, activity);                               \
            return BEHAVIOR_CONTINUE;                                          \
        }                                                                      \
        return BEHAVIOR_FINISH;                                                \
    }

APPROACH_TOY(approach_television, garden_tv_seat, watch_television)
APPROACH_TOY(approach_radio, garden_radio_seat, listen_to_radio)

/* the behavior of a Chao that chooses a toy it doesn't play with: it finishes
 * at once */
#define IGNORE_TOY(name)                                                       \
    static int name(struct task *chao)                                         \
    {                                                                          \
        /* @hack redundant: the nearest toy found, which nothing uses */       \
        chao_nearest_toy_task(chao);                                           \
        return BEHAVIOR_FINISH;                                                \
    }

IGNORE_TOY(ignore_ball)
IGNORE_TOY(ignore_toy_5)
IGNORE_TOY(ignore_toy_6)

static int approach_horse(struct task *chao)
{
    /* @hack redundant: a seat nothing uses, which only sizes the frame */
    NJS_POINT3 seat; /* NOLINT(clang-diagnostic-unused-variable) */
    struct task *horse = chao_nearest_toy_task(chao);

    if (horse && !garden_paired_elsewhere(chao, horse)) {
        mover_set_target(chao, &horse->work->pos);
        garden_pair_objects(chao, horse);
        chao_set_behavior(chao, chao_stand_up_behavior);
        chao_queue_behavior(chao, chao_pause_with_mark_behavior);
        chao_queue_behavior(chao, chao_walk_to_point_behavior);
        chao_queue_behavior(chao, chao_face_point_behavior);
        chao_queue_behavior(chao, ride_horse);
        return BEHAVIOR_CONTINUE;
    }
    return BEHAVIOR_FINISH;
}

static int approach_jack(struct task *chao)
{
    /* @hack redundant: a seat nothing uses, which only sizes the frame */
    NJS_POINT3 seat; /* NOLINT(clang-diagnostic-unused-variable) */
    struct task *box = chao_nearest_toy_task(chao);

    if (box) {
        garden_set_target(chao, box);
        chao_set_behavior(chao, chao_stand_up_behavior);
        chao_queue_behavior(chao, chao_pause_with_mark_behavior);
        chao_queue_behavior(chao, chao_walk_to_target_behavior);
        chao_queue_behavior(chao, chao_switch_on_toy_behavior);
        chao_queue_behavior(chao, chao_idle_behavior);
        return BEHAVIOR_CONTINUE;
    }
    return BEHAVIOR_FINISH;
}

#define SCORE_BROADCAST(name, field, toy, seat_for, activity)                  \
    void name(struct task *chao, Float *best)                                  \
    {                                                                          \
        Float score = 0.0F;                                                    \
        NJS_POINT3 seat;                                                       \
        Uint32 threshold = chao_tuning.boredom_threshold;                      \
        Uint32 boredom = chao_emotion_get(chao, CHAO_BOREDOM);                 \
        struct task *signal = garden_hit_kind(chao, CHAO_PUSH_SHAPE, (field)); \
        if (*best < 1.0F && signal) {                                          \
            if (boredom > threshold) {                                         \
                if (boredom < BROADCAST_BOREDOM_CAP) {                         \
                    boredom += BROADCAST_BOREDOM;                              \
                }                                                              \
                score = chao_need_above(boredom, threshold);                   \
                score *= chao_tuning.boredom_weight;                           \
                chao_vary_score(&score);                                       \
            }                                                                  \
            if (score > *best) {                                               \
                if ((toy) && (seat_for)(&seat)) {                              \
                    mover_set_target(chao, &seat);                             \
                    garden_set_target(chao, (toy));                            \
                    chao_set_behavior(chao, chao_stand_up_behavior);           \
                    chao_queue_behavior(chao, chao_pause_with_mark_behavior);  \
                    chao_queue_behavior(chao, chao_walk_to_point_behavior);    \
                    chao_queue_behavior(chao, chao_face_target_behavior);      \
                    chao_queue_behavior(chao, activity);                       \
                }                                                              \
                *best = BROADCAST_BID;                                         \
            }                                                                  \
        }                                                                      \
    }

SCORE_BROADCAST(chao_score_radio, GARDEN_RADIO_SIGNAL, garden_radio,
                garden_radio_seat, listen_to_radio)
SCORE_BROADCAST(chao_score_television, GARDEN_TV_SIGNAL, garden_television,
                garden_tv_seat, watch_television)

void chao_score_garden_toy(struct task *chao, Float *best)
{
    Float score = 0.0F;
    Uint32 boredom = chao_emotion_get(chao, CHAO_BOREDOM);
    Uint32 threshold;
    Uint32 cooldown;
    struct task *toy;
    int kind;
    int terrain;

    cooldown = chao_cooldown(chao, TOY_COOLDOWN);
    /* @hack spelling: <= 0 on the unsigned cooldown, where == 0 would do */
    if (cooldown <= 0) {
        toy = chao_nearest_toy_task(chao);
        if (!toy) {
            return;
        }
        threshold = chao_tuning.boredom_threshold;
        score = chao_need_above(boredom, threshold);
        score *= chao_tuning.boredom_weight;
        chao_vary_score(&score);
        if (score > *best) {
            kind = ((struct garden_entry *)toy->force.p)->kind;
            switch (kind) {
            case GARDEN_TOY_TV:
                chao_set_behavior(chao, approach_television);
                break;
            case GARDEN_TOY_RADIO:
                chao_set_behavior(chao, approach_radio);
                break;
            case GARDEN_TOY_BALL:
                chao_set_behavior(chao, ignore_ball);
                break;
            case GARDEN_TOY_5:
                chao_set_behavior(chao, ignore_toy_5);
                break;
            case GARDEN_TOY_6:
                chao_set_behavior(chao, ignore_toy_6);
                break;
            case GARDEN_TOY_HORSE:
                terrain = garden_spawn_kind(&toy->work->pos);
                /* @hack redundant: the outer test of terrains 1 and 2, which
                 * NO_RIDE_TERRAIN always passes */
                if (terrain != 1 && terrain != 2) {
                    if (terrain == NO_RIDE_TERRAIN) {
                        return;
                    }
                }
                chao_set_behavior(chao, approach_horse);
                break;
            case GARDEN_TOY_JACK:
                chao_set_behavior(chao, approach_jack);
                break;
            }
            chao_emotion_add(chao, CHAO_BOREDOM, -BOREDOM_RELIEF);
            chao_clear_intention(chao);
            *best = score;
        }
    }
}
