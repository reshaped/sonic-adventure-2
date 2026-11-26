/* a frightened Chao trembles. where it sees a player it fears, it then flees.
 * it keeps to the kind of terrain it stands on */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/behavior.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/intention.h"
#include "chao/knowledge.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/needs.h"
#include "chao/perception.h"
#include "chao/race_chao.h"
#include "chao/sound.h"
#include "chao/spawn_grid.h"
#include "chao/tuning.h"
#include "chao/walk.h"
#include "chao/work.h"
#include "object/mover.h"
#include "player/player.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

/* a trait plus TRAIT_OFFSET, times TRAIT_SCALE, runs from 0 to 1 */
#define TRAIT_OFFSET 100
#define TRAIT_SCALE 0.005F

/* a tremble lasts TREMBLE_STEPS and a random part of TREMBLE_SPAN more.
 * every VOICE_INTERVAL steps, it sounds TREMBLE_VOICE one time in two */
#define TREMBLE_STEPS 180
#define TREMBLE_SPAN 301.0F
#define VOICE_INTERVAL 150
#define TREMBLE_VOICE 0x602d
#define FLEE_VOICE 0x602e

/* how far each try at an escape point lies from the Chao */
#define ESCAPE_STRIDE 10.0F

/* a flight ends after FLEE_STEPS, or within ARRIVAL of its escape point
 * where player 0 lies beyond SAFE_DISTANCE. both distances are squared */
#define FLEE_STEPS 1000
#define ARRIVAL 64.0F
#define SAFE_DISTANCE 2500.0F
#define TURN_STEP 0x120
/* the frames a flight's motion links over. it plays at RUN_MOTION_RATE times
 * the Chao's speed */
#define FLEE_LINK_FRAMES 10
#define RUN_MOTION_RATE 8.0F
/* a fleeing Chao may stumble on each STUMBLE_INTERVAL steps, with chance
 * STUMBLE */
#define STUMBLE_INTERVAL 100
#define STUMBLE 0.03F

static int tremble(struct task *chao);
static int flee_player(struct task *chao);

void score_player_fear(struct task *chao, Float *best)
{
    struct chao_tuning *tuning;
    Float score;
    Uint32 fear;
    struct task *player;
    Uint32 remembered;
    Uint32 threshold;
    Bool saw_player;

    score = 0.0F;
    tuning = &chao_tuning;
    fear = chao_emotion_get(chao, CHAO_FEAR);
    if (fear > tuning->fear_threshold) {
        saw_player = FALSE;
        player = chao_visible_player_task(chao);
        if (player) {
            remembered = chao_player_fear(chao);
            threshold = tuning->player_fear_threshold;
            if (remembered > threshold) {
                score = chao_feeling_above(fear, threshold);
                score *= tuning->fear_player_weight;
                chao_vary_score(&score);
            }
            saw_player = TRUE;
            if (score > *best) {
                chao_set_behavior(chao, tremble);
                chao_queue_behavior(chao, flee_player);
                chao_clear_intention(chao);
                *best = score;
            }
        }
        if (!saw_player) {
            score = 1.0F -
                    ((chao_emotion_get(chao, CHAO_AGGRESSION) + TRAIT_OFFSET) *
                     (1.0F - tuning->fear_idle_weight) * TRAIT_SCALE);
            if (score > *best) {
                chao_set_behavior(chao, tremble);
                chao_clear_intention(chao);
                *best = score;
            }
        }
    }
}

static int tremble(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);

    switch (behavior->mode) {
    case 0:
        if (RANDOM_UNIT() < 0.5F) {
            chao_link_motion(chao, CHAO_ANIM_BURUBURU);
        } else {
            chao_link_motion(chao, CHAO_ANIM_BURUBURU_AGO);
        }
        behavior->mode++;
        chao_set_eye(chao, CHAO_EYE_DISTRESSED);
        chao_set_mouth(chao, CHAO_MOUTH_DISTRESSED);
        behavior->timer = RANDOM_COUNT(TREMBLE_STEPS, TREMBLE_SPAN);
        /* fall through */
    case 1:
        if (behavior->timer % VOICE_INTERVAL == 0) {
            if (RANDOM_UNIT() < 0.5F) {
                sound_play_at(TREMBLE_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                              &chao->work->pos);
            }
        }
        if (--behavior->timer == 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

static void choose_escape_point(struct task *chao)
{
    struct task_work *work = chao->work;
    struct mover *mover = chao->motion.p;
    int terrain;
    Float stride;
    NJS_POINT3 point;
    NJS_VECTOR away;

    /* @bug uninitialized: a Chao on terrain 0 flees toward a target whose y
     * is never set. garden_spawn_pick writes only x and z of point, in the
     * call at 0x0c52dfa0 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: point starts at the Chao's
     * position, so its y is set */
    point = work->pos;
#endif
    terrain = garden_spawn_kind(&work->pos);
    stride = ESCAPE_STRIDE;
    if (terrain) {
        away.x = work->pos.x - player_work[0]->pos.x;
        away.y = 0.0F;
        away.z = work->pos.z - player_work[0]->pos.z;
        njUnitVector(&away);
        point.x = work->pos.x + (away.x * stride);
        point.y = work->pos.y;
        point.z = work->pos.z + (away.z * stride);
        if (terrain != garden_spawn_kind(&point)) {
            if (RANDOM_UNIT() < 0.5F) {
                point.x = work->pos.x + (stride * away.z);
                point.y = work->pos.y;
                point.z = work->pos.z - (stride * away.x);
            } else {
                point.x = work->pos.x - (stride * away.z);
                point.y = work->pos.y;
                point.z = work->pos.z + (stride * away.x);
            }
            if (terrain != garden_spawn_kind(&point)) {
                garden_spawn_pick(terrain, &point);
            }
        }
    } else {
        garden_spawn_pick(TERRAIN_WANDER, &point);
    }
    mover->target = point;
}

static int flee_player(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    struct task_work *work = chao->work;
    struct mover *mover = chao->motion.p;
    Float choice;
    /* @hack redundant: a skill read whose value nothing uses */
    chao_skill_points(chao, CHAO_SWIM);
    switch (behavior->mode) {
    case 0:
        /* @hack spelling: the sample held in choice, where the test could
         * take RANDOM_UNIT() as tremble's does */
        choice = RANDOM_UNIT();
        if (choice < 0.5F) {
            chao_link_motion_frames(chao, CHAO_ANIM_OIKAKE, FLEE_LINK_FRAMES);
        } else {
            chao_link_motion_frames(chao, CHAO_ANIM_RUN_BANZAI,
                                    FLEE_LINK_FRAMES);
        }
        chao_set_eye(chao, CHAO_EYE_DISTRESSED);
        chao_set_mouth(chao, CHAO_MOUTH_DISTRESSED);
        sound_play_at(FLEE_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                      &chao->work->pos);
        choose_escape_point(chao);
        behavior->mode++;
        behavior->timer = 0;
        behavior->count = 0;
    case 1:
        break;
    }
    if (chao_take_upkeep_tick(chao)) {
        chao_emotion_add(chao, CHAO_TIREDNESS, chao_tuning.flee_tiredness_gain);
    }
    if (behavior->timer++ > FLEE_STEPS) {
        return BEHAVIOR_FINISH;
    }
    if (behavior->timer % STUMBLE_INTERVAL == 0) {
        if (RANDOM_UNIT() < STUMBLE) {
            chao_set_behavior(chao, chao_stumble_behavior);
        }
    }
    if (chao_target_distance_squared(chao) < ARRIVAL) {
        if (mover_player_distance2(chao, 0) < SAFE_DISTANCE) {
            behavior->mode = 0;
        } else {
            return BEHAVIOR_FINISH;
        }
    }
    if (mover->flags & MOVER_FACING_WALL) {
        behavior->mode = 0;
    }
    mover_turn_to_target_by(chao, TURN_STEP);
    if (mover->flags & MOVER_ON_FLOOR) {
        mover->acc.x = njSin(work->ang.y) * chao_tuning.flee_acceleration;
        mover->acc.z = njCos(work->ang.y) * chao_tuning.flee_acceleration;
    }
    chao_set_motion_speed(chao, njScalor(&mover->spd) * RUN_MOTION_RATE);
    return BEHAVIOR_CONTINUE;
}
