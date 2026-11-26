/* a garden Chao in water: its entry, its strokes and its float */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/behavior.h"
#include "chao/body_collision.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/garden.h"
#include "chao/garden_field.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/parts.h"
#include "chao/race_chao.h"
#include "chao/sound.h"
#include "chao/spawn_grid.h"
#include "chao/swim.h"
#include "chao/tuning.h"
#include "chao/walk.h"
#include "chao/work.h"
#include "collision/collision.h"
#include "object/animal.h"
#include "object/mover.h"
#include "player/player.h"
#include "shared/water_ripple.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the bits chao_try_swimming keeps in behavior->flags. it sets both while
 * the Chao is in water, and clears both out of it. a held Chao loses
 * WATER_ENTERED, so it enters the water again when dropped in it */
#define WATER_ENTERED 1
#define IN_WATER 4
/* the height of each garden's water surface. an area without water puts the
 * surface far below any Chao */
#define NEUTRAL_WATER_Y (-2.2F)
#define HERO_WATER_Y (-1.5F)
#define DARK_WATER_Y (-1.0F)
#define NO_WATER_Y (-10000.0F)
/* a Chao whose pos lies more than ENTRY_DEPTH below the water's surface is
 * in water. a stroke lifts it no higher than SWIM_DEPTH below the surface.
 * SWIM_DEPTH is the deeper, so a swimming Chao stays in water */
#define ENTRY_DEPTH 2.0F
#define SWIM_DEPTH 2.1F
/* a swimming Chao with less thirst than WANDER_THIRST aims at a point of
 * TERRAIN_WANDER, not of water. one with less than CLIMB_THIRST climbs out
 * where it swims into a climb field */
#define WANDER_THIRST 100
#define CLIMB_THIRST 1000
/* a swim's target lies from 0.3 to 1 of TARGET_REACH ahead. its heading is
 * within half of TARGET_SPREAD of the Chao's yaw. after TARGET_TRIES points
 * out of water, the target is x and z 0 */
#define TARGET_REACH 60.0F
#define TARGET_SPREAD (200.0F * ANGLE_PER_DEGREE)
#define TARGET_TRIES 100
/* a stroke ends within SWIM_REACH of its target. it also ends once
 * behavior->count passes SWIM_STEPS, which swim_step counts twice a step */
#define SWIM_REACH 6.0F
#define SWIM_STEPS 600
#define SWIM_TURN 288
#define DIVE_SOUND 0x1020
#define SWIM_SOUND 0x101f
#define FLOAT_SOUND 0x1021
#define PADDLE_VOICE 0x6024
#define GASP_VOICE 0x6025

static int choose_swim(struct task *chao);

Bool chao_try_swimming(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    struct mover *mover = chao->motion.p;

    fun_0c501218(chao);
    if (chao->work->flags & WORK_HELD_BY_PLAYER) {
        behavior->flags &= ~WATER_ENTERED;
        return FALSE;
    }
    switch (chao_current_area()) {
    case CHAO_AREA_NEUTRAL:
        mover->flat_y = NEUTRAL_WATER_Y;
        break;
    case CHAO_AREA_HERO:
        mover->flat_y = HERO_WATER_Y;
        break;
    case CHAO_AREA_DARK:
        mover->flat_y = DARK_WATER_Y;
        break;
    default:
        mover->flat_y = NO_WATER_Y;
        break;
    }
    if (mover->flat_y > chao->work->pos.y + ENTRY_DEPTH) {
        if (!(behavior->flags & WATER_ENTERED)) {
            behavior->flags |= WATER_ENTERED;
            sound_play_at(DIVE_SOUND, NULL, 0, 0, &chao->work->pos);
            chao_set_behavior(chao, choose_swim);
        }
        if (mover->spd.y < 0.0F) {
            mover->spd.y *= 0.1F;
        }
        behavior->flags |= IN_WATER;
        return TRUE;
    }
    behavior->flags &= ~WATER_ENTERED;
    behavior->flags &= ~IN_WATER;
    return FALSE;
}

/* aim the Chao at a point of water ahead of it. a Chao with less thirst
 * than WANDER_THIRST aims at a point of TERRAIN_WANDER instead, and keeps
 * its aim_y */
static void choose_swim_target(struct task *chao)
{
    struct task_work *work = chao->work;
    struct mover *mover = chao->motion.p;
    NJS_POINT3 *position = &work->pos;
    NJS_POINT3 target;
    Float distance;
    Float dx;
    Float dz;
    Angle yaw;
    int attempts = 0;

    if (chao_emotion_get(chao, CHAO_THIRST) < WANDER_THIRST) {
        garden_spawn_pick(TERRAIN_WANDER, &mover->target);
        return;
    }
    target.y = position->y;
    for (;;) {
        distance = (((rand() * RAND_SCALE) * 0.7F) + 0.3F) * TARGET_REACH;
        yaw = (Angle)((0.5F - (rand() * RAND_SCALE)) * TARGET_SPREAD) +
              work->ang.y;
        target.x = position->x + (njSin(yaw) * distance);
        target.z = position->z + (njCos(yaw) * distance);
        if (garden_spawn_kind(&target) == TERRAIN_WATER) {
            break;
        }
        /* @hack spelling: the increment as a statement of its own, where
         * ++attempts in the test would do */
        attempts++;
        if (attempts == TARGET_TRIES) {
            target.x = 0.0F;
            target.z = 0.0F;
            break;
        }
    }
    mover->target = target;
    dx = mover->target.x - work->pos.x;
    dz = mover->target.z - work->pos.z;
    mover->aim_y = (Angle)(atan2f(dx, dz) * ANGLE_PER_RADIAN);
}

static void float_step(struct task *chao)
{
    struct task_work *work = chao->work;
    struct chao_behavior *behavior = &((struct chao_work *)work)->behavior;
    struct mover *mover = chao->motion.p;
    NJS_POINT3 position;
    NJS_VECTOR growth;

    /* @hack redundant: a call whose result nothing reads */
    chao_skill_points(chao, CHAO_SWIM);
    /* @hack spelling: the two tests in nested ifs, where && would do */
    if (!(behavior->flags & WATER_ENTERED)) {
        if (mover->flags & MOVER_ON_FLOOR) {
            chao_set_behavior(chao, chao_choose_destination);
        }
    }
    if (behavior->count++ % 10 == 0 && rand() * RAND_SCALE < 0.4F) {
        position.x = work->pos.x;
        position.y = mover->flat_y;
        position.z = work->pos.z;
        growth.x = 0.03F;
        growth.y = 0.0F;
        growth.z = 0.7F;
        ripple_create(&position, &growth, 0.2F);
    }
    if (work->pos.y <= mover->flat_y - SWIM_DEPTH) {
        work->pos.y += 0.02F;
        if (work->pos.y > mover->flat_y - SWIM_DEPTH) {
            work->pos.y = mover->flat_y - SWIM_DEPTH;
        }
        mover->acc.y = -mover->gravity - (mover->spd.y * 0.05F);
        /* @hack redundant: the sine and cosine of ang.y, times a speed of 0 */
        mover->acc.x = (njSin(work->ang.y) * 0.0F) - (mover->spd.x * 0.05F);
        mover->acc.z = (njCos(work->ang.y) * 0.0F) - (mover->spd.z * 0.05F);
    }
    if (chao_take_upkeep_tick(chao)) {
        chao_emotion_add(chao, CHAO_THIRST, -chao_tuning.swim_thirst_loss);
        chao_emotion_add(chao, CHAO_BOREDOM, -chao_tuning.swim_boredom_loss);
        chao_emotion_add(chao, CHAO_TIREDNESS,
                         -chao_tuning.float_tiredness_loss);
    }
    sound_play_at_for(FLOAT_SOUND, chao, 1, 0, 120, &work->pos);
}

static void swim_step(struct task *chao)
{
    struct task_work *work = chao->work;
    struct chao_behavior *behavior = &((struct chao_work *)work)->behavior;
    struct mover *mover = chao->motion.p;
    Uint16 points;
    NJS_POINT3 position;
    NJS_VECTOR growth;
    Float speed;

    points = chao_skill_points(chao, CHAO_SWIM);
    if (!(behavior->flags & IN_WATER) && (mover->flags & MOVER_ON_FLOOR)) {
        chao_set_behavior(chao, chao_choose_destination);
    }
    if (behavior->count++ % 10 == 0 && rand() * RAND_SCALE < 0.4F) {
        position.x = work->pos.x;
        position.y = mover->flat_y;
        position.z = work->pos.z;
        growth.x = 0.03F;
        growth.y = 0.0F;
        growth.z = 0.7F;
        ripple_create(&position, &growth, 0.2F);
    }
    if (mover->flags & MOVER_FACING_WALL) {
        if (chao_emotion_get(chao, CHAO_THIRST) < CLIMB_THIRST) {
            if (collision_first_with_id(chao, GARDEN_CLIMB_FIELD_ID)) {
                chao_set_behavior(chao, chao_climb_out_behavior);
            }
        } else if (rand() * RAND_SCALE < 0.002F) {
            chao_set_behavior(chao, choose_swim);
        }
    }
    if (chao_target_distance_squared(chao) < SWIM_REACH * SWIM_REACH) {
        chao_set_behavior(chao, choose_swim);
    }
    if (behavior->count++ > SWIM_STEPS) {
        chao_set_behavior(chao, choose_swim);
    }
    mover_turn_to_target_by(chao, SWIM_TURN);
    if (work->pos.y <= mover->flat_y - SWIM_DEPTH) {
        speed = chao_tuning.swim_speed +
                (points * chao_tuning.swim_speed_per_point);
        work->pos.y += 0.1F;
        if (work->pos.y > mover->flat_y - SWIM_DEPTH) {
            work->pos.y = mover->flat_y - SWIM_DEPTH;
        }
        mover->acc.y = -mover->gravity - (mover->spd.y * 0.1F);
        mover->acc.x = (njSin(work->ang.y) * speed) - (mover->spd.x * 0.05F);
        mover->acc.z = (njCos(work->ang.y) * speed) - (mover->spd.z * 0.05F);
    }
    if (chao_take_upkeep_tick(chao)) {
        chao_emotion_add(chao, CHAO_THIRST, -chao_tuning.swim_thirst_loss);
        chao_emotion_add(chao, CHAO_BOREDOM, -chao_tuning.swim_boredom_loss);
        chao_emotion_add(chao, CHAO_TIREDNESS, chao_tuning.swim_tiredness_gain);
    }
    sound_play_at_for(SWIM_SOUND, chao, 1, -25, 90, &work->pos);
}

static int paddle_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    Float choice;

    switch (behavior->mode) {
    case 0:
        chao_set_timed_eye(chao, CHAO_EYE_DISTRESSED, CHAO_FACE_HOLD);
        /* @hack type: the mouth cast to Sint16, where the int
         * chao_default_mouth returns would do */
        if ((Sint16)chao_default_mouth(chao) == 1) {
            choice = rand() * RAND_SCALE;
            if (choice < 0.25F) {
                chao_set_timed_mouth(chao, 9, CHAO_FACE_HOLD);
            }
        } else {
            choice = rand() * RAND_SCALE;
            if (choice < 0.25F) {
                chao_set_timed_mouth(chao, 6, CHAO_FACE_HOLD);
            } else if (choice < 0.5F) {
                chao_set_timed_mouth(chao, CHAO_MOUTH_DISTRESSED,
                                     CHAO_FACE_HOLD);
            } else if (choice < 0.75F) {
                chao_set_timed_mouth(chao, 11, CHAO_FACE_HOLD);
            } else {
                chao_set_timed_mouth(chao, 4, CHAO_FACE_HOLD);
            }
        }
        chao_link_motion(chao, CHAO_ANIM_OBORERU);
        behavior->mode++;
        /* fall through */
    case 1:
        break;
    }
    if (CHAO_WORK(chao)->frames % 120 == 0 && rand() * RAND_SCALE < 0.2F) {
        sound_play_at(PADDLE_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                      &chao->work->pos);
    }
    if (chao_take_upkeep_tick(chao)) {
        chao_emotion_add(chao, CHAO_THIRST,
                         -(chao_tuning.swim_thirst_loss * 10));
    }
    swim_step(chao);
    return BEHAVIOR_CONTINUE;
}

/* @hack spelling: the switch written into each stroke by a macro, where a
 * function the three call would do */
#define START_STROKE(chao, behavior, choice, motion)                           \
    switch ((behavior)->mode) {                                                \
    case 0:                                                                    \
        (choice) = rand() * RAND_SCALE;                                        \
        if ((choice) < 0.2F) {                                                 \
            chao_set_timed_eye((chao), 1, CHAO_FACE_HOLD);                     \
        } else if ((choice) < 0.4F) {                                          \
            chao_set_timed_eye((chao), CHAO_EYE_GLAD, CHAO_FACE_HOLD);         \
        }                                                                      \
        chao_link_motion((chao), (motion));                                    \
        (behavior)->mode++;                                                    \
        /* fall through */                                                     \
    case 1:                                                                    \
        break;                                                                 \
    }

static int intermediate_swim(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    Float choice;

    START_STROKE(chao, behavior, choice, CHAO_ANIM_BATAASHI_B);
    swim_step(chao);
    return BEHAVIOR_CONTINUE;
}

static int advanced_swim(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    Float choice;

    START_STROKE(chao, behavior, choice, CHAO_ANIM_CRAWL);
    swim_step(chao);
    return BEHAVIOR_CONTINUE;
}

static int otter_swim(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    Float choice;

    START_STROKE(chao, behavior, choice, CHAO_ANIM_SEOYOGI_KAITEN);
    swim_step(chao);
    return BEHAVIOR_CONTINUE;
}

static int float_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    Float choice;

    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_PUHA_PUHA, 20);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer++ > 30) {
            chao_set_timed_eye(chao, CHAO_EYE_DISTRESSED, 120);
            chao_set_timed_mouth(chao, 2, 120);
            behavior->mode++;
            sound_play_at(GASP_VOICE, NULL, 0, 0, &chao->work->pos);
        }
        /* fall through */
    case 2:
        if (chao_motion_ended(chao)) {
            choice = rand() * RAND_SCALE;
            if (choice < 0.5F) {
                chao_set_timed_eye(chao, 3, 120);
            } else if (choice < 0.8F) {
                chao_set_timed_eye(chao, CHAO_EYE_GLAD, 120);
            }
            chao_link_motion(chao, CHAO_ANIM_DRIFT);
            /* @hack type: the cast to Uint16, where the int would do */
            behavior->timer =
                (Uint16)((int)((rand() * RAND_SCALE) * 601.0F) + 600);
            behavior->mode++;
        }
        break;
    case 3:
        if (behavior->timer-- <= 0) {
            chao_set_behavior(chao, choose_swim);
        }
        break;
    }
    float_step(chao);
    return BEHAVIOR_CONTINUE;
}

static int choose_swim(struct task *chao)
{
#ifdef NONMATCHING
#else
    /* @hack redundant: word and behavior, locals that nothing reads */
    Sint32 word = *(Sint32 *)CHAO_BEHAVIOR(chao);
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
#endif
    Uint16 points;

    points = chao_skill_points(chao, CHAO_SWIM);
    choose_swim_target(chao);
    chao_disable_swim_shape(chao);
    if (points < chao_tuning.swim_intermediate_points) {
        chao_set_behavior(chao, paddle_behavior);
    } else {
        if (points < chao_tuning.swim_advanced_points) {
            chao_set_behavior(chao, intermediate_swim);
        } else {
            chao_set_behavior(chao, advanced_swim);
        }
        if (chao_has_eaten_minimal(chao, ANIMAL_RAKO)) {
            if (rand() * RAND_SCALE < 0.5F) {
                chao_set_behavior(chao, otter_swim);
            }
        }
        if (rand() * RAND_SCALE < 0.2F) {
            chao_set_behavior(chao, float_behavior);
        }
    }
    swim_step(chao);
    return BEHAVIOR_CONTINUE;
}
