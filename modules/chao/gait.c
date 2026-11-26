/* a Chao's walk to its mover's target. its run skill sets the gait: crawl,
 * walk, run or run with arms out */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/activities.h"
#include "chao/behavior.h"
#include "chao/body_collision.h"
#include "chao/emotion.h"
#include "chao/gait.h"
#include "chao/garden_field.h"
#include "chao/idle.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/race_chao.h"
#include "chao/sound.h"
#include "chao/spawn_grid.h"
#include "chao/swim.h"
#include "chao/tuning.h"
#include "chao/walk.h"
#include "chao/work.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "object/mover.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

/* a walk ends within ARRIVAL of the target, squared */
#define ARRIVAL 36.0F
#define TURN_STEP 0x120
/* a Chao facing a wall inside a climb field's box climbs out, and rises
 * CLIMB_LIFT. one facing any other wall turns partway. so does one whose body
 * meets a shape of TURN_ASIDE_ID, once its timer passes TURN_ASIDE_STEPS */
#define CLIMB_LIFT 0.1F
#define TURN_ASIDE_ID 204
#define TURN_ASIDE_STEPS 30
/* a Chao hops at HOP_SPEED up. it hops up a floor whose normal's y is below
 * STEEP_FLOOR. it hops off a ledge more than LEDGE_DROP high. one with
 * GLIDE_POINTS of flying or more glides off a ledge four times in ten. no Chao
 * glides toward water */
#define STEEP_FLOOR 0.7F
#define LEDGE_DROP 2.0F
#define HOP_SPEED 0.55F
#define GLIDE_POINTS 400

/* every VOICE_INTERVAL of its frames, a crawling Chao makes one of its two
 * voices three times in ten */
#define VOICE_INTERVAL 120
#define CRAWL_VOICE 0x6014
#define OTHER_CRAWL_VOICE 0x6015
/* a walking Chao may stumble on step 1 of each WALK_STUMBLE_INTERVAL. it
 * stumbles with chance WALK_STUMBLE. a running one may stumble on step 0 of
 * each RUN_STUMBLE_INTERVAL, with chance RUN_STUMBLE */
#define WALK_STUMBLE_INTERVAL 85
#define WALK_STUMBLE 0.03F
#define RUN_STUMBLE_INTERVAL 100
#define RUN_STUMBLE 0.02F
/* a walk's motion plays at WALK_MOTION_RATE times the Chao's speed, and a
 * run's at RUN_MOTION_RATE times it */
#define WALK_MOTION_RATE 16.0F
#define RUN_MOTION_RATE 14.0F

static const NJS_VECTOR up = {0.0F, 1.0F, 0.0F};

static int head_to_target(struct task *chao)
{
#ifdef NONMATCHING
#else
    /* @hack redundant: the read of the behavior's first word, which nothing
     * uses */
    Sint32 word = *(Sint32 *)CHAO_BEHAVIOR(chao);
#endif
    struct task_work *work = chao->work;
    struct mover *mover = chao->motion.p;
    NJS_VECTOR vertical;
    Float slope;
    if (chao_target_distance_squared(chao) < ARRIVAL) {
        return BEHAVIOR_FINISH;
    }
    if (mover->flags & MOVER_FACING_WALL) {
        if (collision_first_with_id(chao, GARDEN_CLIMB_FIELD_ID)) {
            chao_set_behavior(chao, chao_climb_out_behavior);
            work->pos.y += CLIMB_LIFT;
        } else {
            chao_set_behavior(chao, chao_turn_partway_behavior);
        }
    }
    if (work->timer++ > TURN_ASIDE_STEPS &&
        garden_hit_kind(chao, CHAO_PUSH_SHAPE, TURN_ASIDE_ID)) {
        chao_set_behavior(chao, chao_turn_partway_behavior);
    }
    mover_turn_to_target_by(chao, TURN_STEP);
    if (mover->flags & MOVER_ON_FLOOR) {
        vertical = up;
        slope = njInnerProduct(&mover->probe.sides[SURFACE_NEG_Y].normal,
                               &vertical);
        if (slope < STEEP_FLOOR) {
            mover->spd.y += HOP_SPEED;
            chao_set_behavior(chao, chao_hop_forward);
        }
    } else if (work->pos.y - mover->probe.sides[SURFACE_NEG_Y].pos >
               LEDGE_DROP) {
        mover->spd.y += HOP_SPEED;
        if (garden_spawn_kind(&mover->target) == TERRAIN_WATER) {
            chao_set_behavior(chao, chao_hop_forward);
        } else if (chao_skill_points(chao, CHAO_FLY) < GLIDE_POINTS ||
                   RANDOM_UNIT() < 0.6F) {
            chao_set_behavior(chao, chao_hop_forward);
        } else {
            chao_set_behavior(chao, chao_glide_behavior);
        }
    }
    if (chao_take_upkeep_tick(chao)) {
        chao_emotion_add(chao, CHAO_BOREDOM, -chao_tuning.run_boredom_loss);
        chao_emotion_add(chao, CHAO_TIREDNESS, chao_tuning.run_tiredness_gain);
    }
    return BEHAVIOR_CONTINUE;
}

#define START_GAIT(motion_index)                                               \
    switch (behavior->mode) {                                                  \
    case 0:                                                                    \
        chao_link_motion(chao, (motion_index));                                \
        behavior->mode++;                                                      \
    case 1:                                                                    \
        break;                                                                 \
    }

static int crawl(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    struct mover *mover;
    /* @hack redundant: a skill read whose value nothing uses */
    chao_skill_points(chao, CHAO_RUN);
    mover = chao->motion.p;
    START_GAIT(CHAO_ANIM_HAIHAI);
    if (CHAO_WORK(chao)->frames % VOICE_INTERVAL == 0 && RANDOM_UNIT() < 0.3F) {
        if (RANDOM_UNIT() < 0.5F) {
            sound_play_at(CRAWL_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                          &chao->work->pos);
        } else {
            sound_play_at(OTHER_CRAWL_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                          &chao->work->pos);
        }
    }
    if (mover->flags & MOVER_ON_FLOOR) {
        chao_set_forward_acceleration(chao, chao_tuning.approach_acceleration);
    }
    return head_to_target(chao);
}
static int walk(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    struct mover *mover;
#ifdef NONMATCHING
#else
    Float acceleration;
#endif
    /* @hack redundant: a skill read whose value nothing uses */
    chao_skill_points(chao, CHAO_RUN);
#ifdef NONMATCHING
#else
    /* @hack redundant: a block that never runs, and the local it sets */
    if (0) {
        acceleration = chao_tuning.walk_speed;
        chao_set_forward_acceleration(chao, acceleration);
        if (acceleration) {
        }
    }
#endif
    mover = chao->motion.p;
    START_GAIT(CHAO_ANIM_ARUKU);
    if (behavior->timer++ % WALK_STUMBLE_INTERVAL == 1 &&
        RANDOM_UNIT() < WALK_STUMBLE) {
        chao_set_behavior(chao, chao_stumble_behavior);
    }
    if (mover->flags & MOVER_ON_FLOOR) {
        chao_set_forward_acceleration(chao, chao_tuning.walk_speed);
    }
    chao_set_motion_speed(chao, njScalor(&mover->spd) * WALK_MOTION_RATE);
    return head_to_target(chao);
}

#define ACCELERATE_ON_FLOOR()                                                  \
    if (mover->flags & MOVER_ON_FLOOR) {                                       \
        acceleration = chao_tuning.run_speed +                                 \
                       (points * chao_tuning.run_speed_per_point);             \
        mover->acc.x = njSin(work->ang.y) * acceleration;                      \
        mover->acc.z = njCos(work->ang.y) * acceleration;                      \
    }

static int run(struct task *chao)
{
    struct task_work *work = chao->work;
    struct chao_behavior *behavior = &((struct chao_work *)work)->behavior;
    struct mover *mover;
    Uint16 points;
    Float acceleration;
    points = chao_skill_points(chao, CHAO_RUN);
    mover = chao->motion.p;
    START_GAIT(CHAO_ANIM_HASIRU);
    if (behavior->timer++ % RUN_STUMBLE_INTERVAL == 0 &&
        RANDOM_UNIT() < RUN_STUMBLE) {
        chao_set_behavior(chao, chao_stumble_behavior);
    }
    ACCELERATE_ON_FLOOR();
    chao_set_motion_speed(chao, njScalor(&mover->spd) * RUN_MOTION_RATE);
    return head_to_target(chao);
}
static int run_arms_out(struct task *chao)
{
    struct task_work *work = chao->work;
    struct chao_behavior *behavior = &((struct chao_work *)work)->behavior;
    struct mover *mover;
    Uint16 points;
    Float acceleration;
    points = chao_skill_points(chao, CHAO_RUN);
    mover = chao->motion.p;
    START_GAIT(CHAO_ANIM_HASIRU_HIROGE);
    ACCELERATE_ON_FLOOR();
    chao_set_motion_speed(chao, njScalor(&mover->spd) * RUN_MOTION_RATE);
    return head_to_target(chao);
}

void chao_start_skill_walk(struct task *chao)
{
#ifdef NONMATCHING
#else
    /* @hack redundant: the behavior's first word, and a pointer to the
     * behavior, which nothing uses */
    Sint32 word = *(Sint32 *)CHAO_BEHAVIOR(chao);
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
#endif
    Uint16 points = chao_skill_points(chao, CHAO_RUN);
    if (points < chao_tuning.walk_slow_points) {
        chao_set_behavior(chao, chao_sit_down_behavior);
        chao_queue_behavior(chao, crawl);
    } else if (points < chao_tuning.walk_points) {
        chao_set_behavior(chao, chao_stand_up_behavior);
        chao_queue_behavior(chao, walk);
    } else if (points < chao_tuning.walk_fast_points) {
        chao_set_behavior(chao, chao_stand_up_behavior);
        chao_queue_behavior(chao, run);
    } else {
        chao_set_behavior(chao, chao_stand_up_behavior);
        chao_queue_behavior(chao, run_arms_out);
    }
}
int chao_choose_gait(struct task *chao)
{
    chao_start_skill_walk(chao);
    return BEHAVIOR_CONTINUE;
}
