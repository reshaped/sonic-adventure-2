/* a garden Chao wandering, stumbling, climbing and gliding, and running to a
 * player's call or the ball */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "chao/activities.h"
#include "chao/behavior.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/garden_ball.h"
#include "chao/icon.h"
#include "chao/knowledge.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/race_chao.h"
#include "chao/sound.h"
#include "chao/spawn_grid.h"
#include "chao/swim.h"
#include "chao/tuning.h"
#include "chao/walk.h"
#include "chao/work.h"
#include "chao/world.h"
#include "collision/surface.h"
#include "object/mover.h"
#include "player/player.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* a random wait of 20 to span + 19 frames */
/* @hack type: the wait cast to Uint16 before 20 is added, where its int
 * would do */
#define CALL_WAIT(span) ((Uint16)(int)(rand() * ((span) * RAND_SCALE)) + 20)
#define STUMBLE_SOUND 0x100a
#define ANSWER_SOUND 0x6019
/* the steps of a turn, in angle units a frame */
#define CLIMB_TURN 0x800
#define GLIDE_TURN 0x100
#define RUN_TURN 0x100
#define CALL_TURN 0x300
#define BALL_TURN 0x180
/* a Chao answering a call faces the player once it turns less than this */
#define FACING_PLAYER 0x100
/* a floor whose normal has more than this of up is one a climb ends on */
#define LEVEL_FLOOR 0.7F
/* a climb ends once it rises more than CLIMB_HEIGHT, or after CLIMB_FRAMES
 * frames. a Chao still climbing after CLIMB_LIMIT frames gives up */
#define CLIMB_HEIGHT 2.5F
#define CLIMB_FRAMES 30
#define CLIMB_LIMIT 240
/* a glider never rises past GLIDE_CEILING */
#define GLIDE_CEILING 100.0F
/* a chase ends within REACH of its target, or after CHASE_FRAMES frames */
#define REACH 8.0F
#define CHASE_FRAMES 3000
/* a Chao within CALL_REACH of a calling player hears the call */
#define CALL_REACH 100.0F
#define WALL_CONTACT (MOVER_FACING_WALL | MOVER_AT_WALL)
#define ACCELERATE_AHEAD(acceleration)                                         \
    mover->acc.x = njSin(work->ang.y) * (acceleration);                        \
    mover->acc.z = njCos(work->ang.y) * (acceleration)

void chao_pick_wander_target(struct task *chao)
{
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    struct mover *mover = chao->motion.p;
    garden_spawn_pick(TERRAIN_WANDER, &mover->target);
#else
    /* @hack redundant: position, which nothing reads, and a pick into
     * work->pos after the return, where the pick into the target alone would
     * do */
    struct task_work *work = chao->work;
    struct mover *mover = chao->motion.p;
    NJS_POINT3 position;
    if (1) {
        garden_spawn_pick(TERRAIN_WANDER, &mover->target);
        return;
    }
    garden_spawn_pick(TERRAIN_WANDER, &work->pos);
#endif
}

int chao_choose_destination(struct task *chao)
{
    chao_pick_wander_target(chao);
    chao_start_skill_walk(chao);
    return BEHAVIOR_CONTINUE;
}

int chao_stumble_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    struct task_work *work = chao->work;
    struct mover *mover = chao->motion.p;
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_KOKE, 15);
        behavior->mode++;
        behavior->timer = 0;
        /* fall through */
    case 1:
        if (behavior->timer == 40) {
            sound_play_at(STUMBLE_SOUND, NULL, 0, 0, &chao->work->pos);
        }
        if (behavior->timer++ > 60) {
            chao_show_mark(chao, CHAO_MARK_ANGER, 240);
            chao_set_eye(chao, CHAO_EYE_DISTRESSED);
            behavior->mode++;
        }
        if (mover->flags & MOVER_ON_FLOOR) {
            ACCELERATE_AHEAD(chao_tuning.walk_speed);
        }
        break;
    case 2:
        if (behavior->timer++ > 300) {
            chao_link_motion_frames(chao, CHAO_ANIM_KOKEOKI, 15);
            behavior->mode++;
        }
        break;
    case 3:
        if (chao_motion_ended(chao)) {
            chao_resume_behavior(chao);
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_climb_out_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    struct mover *mover = chao->motion.p;
    NJS_POINT3 *position = &chao->work->pos;
    struct chao_work *work;
    Angle heading;
    /* @hack redundant: a call whose result nothing reads */
    chao_skill_points(chao, CHAO_POWER);
    work = CHAO_WORK(chao);
    fun_0c501218(chao);
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_NOBORU);
        mover_clear_spd(chao);
        behavior->mode++;
        /* fall through */
    case 1:
        if (mover->flags & WALL_CONTACT) {
            heading =
                (Angle)(atan2f(mover->facing_wall.vx, mover->facing_wall.vz) *
                        ANGLE_PER_RADIAN);
            chao->work->ang.y = angle_approach(chao->work->ang.y,
                                               heading + HALF_TURN, CLIMB_TURN);
            mover->flags &= ~MOVER_FACING_WALL;
            behavior->timer = 0;
        } else if (behavior->timer++ > 2) {
            behavior->timer = 0;
            work->climb_start_y = chao->work->pos.y;
            behavior->mode++;
        }
        break;
    case 2:
        behavior->timer++;
        if (behavior->timer > 3 && (mover->flags & MOVER_ON_FLOOR)) {
            NJS_VECTOR up = {0.0F, 1.0F, 0.0F};
            if (njInnerProduct(&mover->probe.sides[SURFACE_NEG_Y].normal, &up) >
                LEVEL_FLOOR) {
                chao_set_behavior(chao, chao_choose_destination);
            }
        } else if (mover->flags & WALL_CONTACT) {
            behavior->mode--;
        }
        if (behavior->timer > CLIMB_FRAMES ||
            chao->work->pos.y - work->climb_start_y > CLIMB_HEIGHT) {
            chao_set_behavior(chao, chao_choose_destination);
        }
        break;
    }
    if (behavior->count++ > CLIMB_LIMIT) {
        return BEHAVIOR_FINISH;
    }
    heading = chao->work->ang.y;
    position->x += njSin(heading) * 0.1F;
    position->y += 0.08F;
    position->z += njCos(heading) * 0.1F;
    mover->acc.y = -mover->gravity;
    return BEHAVIOR_CONTINUE;
}

int chao_glide_behavior(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    struct task_work *work = chao->work;
    struct mover *mover = chao->motion.p;
    Angle heading;
    Float speed;
    Float drag;
    switch (behavior->mode) {
    case 0:
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_link_motion_frames(chao, CHAO_ANIM_TOBU, 15);
        heading = chao->work->ang.y;
        mover->spd.x = njSin(heading) * 0.4F;
        mover->spd.y = 0.35F;
        mover->spd.z = njCos(heading) * 0.4F;
        behavior->timer = RANDOM_COUNT(300, 301);
        behavior->mode++;
        break;
    case 1:
        if (mover->flags & MOVER_ON_FLOOR) {
            chao_set_behavior(chao, chao_choose_destination);
        }
        break;
    }
    mover_turn_to_target_by(chao, GLIDE_TURN);
    drag = 0.1F;
    speed = 0.05F;
    mover->acc.x = (njSin(work->ang.y) * speed) - (mover->spd.x * drag);
    mover->acc.z = (njCos(work->ang.y) * speed) - (mover->spd.z * drag);
    if (behavior->timer > 0) {
        behavior->timer--;
        mover->acc.y = -(mover->gravity * 0.95F);
    } else {
        mover->acc.y = -(mover->gravity * 0.92F);
    }
    if (work->pos.y > GLIDE_CEILING) {
        work->pos.y = GLIDE_CEILING;
    }
    return BEHAVIOR_CONTINUE;
}

static int run_to_player(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    struct task_work *work = chao->work;
    struct mover *mover = chao->motion.p;
    struct chao_parameters *parameters;
    Float choice;

    fun_0c501218(chao);
    switch (behavior->mode) {
    case 0:
        choice = RANDOM_UNIT();
        if (choice < 0.2F) {
            chao_link_motion_frames(chao, CHAO_ANIM_OIKAKE, 10);
        } else if (choice < 0.6F) {
            chao_link_motion_frames(chao, CHAO_ANIM_RUN, 10);
        } else {
            chao_link_motion_frames(chao, CHAO_ANIM_RUN_BANZAI, 10);
        }
        chao_set_eye(chao, CHAO_EYE_GLAD);
        if (RANDOM_UNIT() < 0.4F) {
            chao_set_mouth(chao, 1);
        } else {
            chao_set_mouth(chao, CHAO_MOUTH_GLAD);
        }
        chao_show_mark(chao, CHAO_MARK_HEART, 120);
        behavior->mode++;
        behavior->timer = 0;
        /* fall through */
    case 1:
        mover_set_target(chao, &player_work[0]->pos);
        break;
    }
    behavior->timer++;
    /* @hack redundant: a test of the timer with an empty body. chase_ball's
     * same test ends its chase */
    if (behavior->timer > CHASE_FRAMES) {
    }
    if (behavior->timer % 100 == 0) {
        if (RANDOM_UNIT() < 0.1F) {
            chao_set_behavior(chao, chao_stumble_behavior);
        }
    }
    if (mover_target_distance2(chao) < REACH * REACH) {
        return BEHAVIOR_FINISH;
    }
    mover_turn_to_target_by(chao, RUN_TURN);
    if (mover->flags & MOVER_ON_FLOOR) {
        parameters = &CHAO_WORK(chao)->profile.parameters;
        mover->acc.x = njSin(work->ang.y) *
                       (1.0F + (parameters->stats[CHAO_RUN] * 0.001F)) * 0.05F;
        mover->acc.z = njCos(work->ang.y) *
                       (1.0F + (parameters->stats[CHAO_RUN] * 0.001F)) * 0.05F;
    }
    return BEHAVIOR_CONTINUE;
}

static int answer_player_call(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    switch (behavior->mode) {
    case 0:
        behavior->timer = CALL_WAIT(40.0F);
        chao_link_motion_frames(chao, CHAO_ANIM_STAND, 10);
        behavior->mode = 1;
        break;
    case 1:
        if (behavior->timer-- <= 0) {
            if (RANDOM_UNIT() < 0.5F) {
                chao_link_motion_frames(chao, CHAO_ANIM_KYOROKYORO_UP, 10);
            } else {
                chao_link_motion_frames(chao, CHAO_ANIM_HATENA_HATE2, 10);
            }
            chao_show_mark(chao, CHAO_MARK_QUESTION, 180);
            behavior->timer = CALL_WAIT(60.0F);
            behavior->mode = 2;
        }
        break;
    case 2:
        if (behavior->timer-- <= 0) {
            chao_link_motion_frames(chao, CHAO_ANIM_ARUKU, 10);
            behavior->mode = 3;
        }
        break;
    case 3:
        chao_set_motion_speed(chao, 1.5F);
        if (mover_turn_to_player_by(chao, CALL_TURN, 0) < FACING_PLAYER) {
            chao_show_mark(chao, CHAO_MARK_EXCLAMATION, 60);
            behavior->timer = CALL_WAIT(20.0F);
            if (RANDOM_UNIT() < 0.7F) {
                chao_link_motion_frames(chao, CHAO_ANIM_BANZAI_JUMP, 10);
                chao_set_eye(chao, CHAO_EYE_GLAD);
                behavior->mode = 4;
                sound_play_at(ANSWER_SOUND, NULL, 0, GARDEN_VOICE_VOLUME,
                              &chao->work->pos);
            } else {
                behavior->mode = 5;
            }
        }
        break;
    case 4:
        if (chao_motion_ended(chao)) {
            behavior->mode = 5;
        }
        break;
    case 5:
        if (behavior->timer-- <= 0) {
            chao_set_behavior(chao, run_to_player);
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_try_player_call(struct task *chao)
{
    if (player_work[0]->mode == PLAYER_MODE_UNK_2A &&
        mover_player_distance2(chao, 0) < CALL_REACH * CALL_REACH) {
        if (chao_player_liking(chao) > 50) {
            chao_set_behavior(chao, answer_player_call);
        }
        return TRUE;
    }
    return FALSE;
}

static int chase_ball(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    struct task_work *work = chao->work;
    struct mover *mover = chao->motion.p;
    NJS_POINT3 target;
    Uint16 run_points;
    Float choice;
    Float acceleration;
    fun_0c501218(chao);
    switch (behavior->mode) {
    case 0:
        choice = RANDOM_UNIT();
        if (choice < 0.15F) {
            chao_link_motion_frames(chao, CHAO_ANIM_OIKAKE, 10);
        } else if (choice < 0.6F) {
            chao_link_motion_frames(chao, CHAO_ANIM_RUN, 10);
        } else {
            chao_link_motion_frames(chao, CHAO_ANIM_RUN_BANZAI, 10);
        }
        /* @hack redundant: a random number's test against 0.6 with an empty
         * body, where a bare rand() would do */
        if (RANDOM_UNIT() < 0.6F) {
        }
        chao_set_eye(chao, CHAO_EYE_GLAD);
        if (RANDOM_UNIT() < 0.4F) {
            chao_set_mouth(chao, 1);
        } else {
            chao_set_mouth(chao, CHAO_MOUTH_GLAD);
        }
        chao_show_mark(chao, CHAO_MARK_EXCLAMATION, 120);
        behavior->mode++;
        behavior->timer = 0;
        /* fall through */
    case 1:
        if (garden_ball) {
            target.x = garden_ball->work->pos.x * 1.02F;
            target.y = garden_ball->work->pos.y;
            target.z = garden_ball->work->pos.z * 1.02F;
            mover_set_target(chao, &target);
            if (garden_ball->work->flags & WORK_HELD_BY_PLAYER) {
                garden_clear_partner(chao);
                return BEHAVIOR_FINISH;
            }
        }
        break;
    }
    behavior->timer++;
    if (behavior->timer % 100 == 0) {
        if (RANDOM_UNIT() < 0.15F) {
            chao_set_behavior(chao, chao_stumble_behavior);
        }
    }
    if (garden_ball_floating) {
        chao_show_mark(chao, CHAO_MARK_ANGER, 240);
        garden_clear_partner(chao);
        return BEHAVIOR_FINISH;
    }
    /* @hack redundant: a test of the distance to the ball with an empty
     * body. run_to_player's same test ends its chase */
    if (mover_target_distance2(chao) < REACH * REACH) {
    }
    mover_turn_to_target_by(chao, BALL_TURN);
    if (mover->flags & MOVER_ON_FLOOR) {
        run_points = chao_skill_points(chao, CHAO_RUN);
        acceleration = chao_tuning.run_speed +
                       (run_points * chao_tuning.run_speed_per_point);
        ACCELERATE_AHEAD(acceleration);
    }
    if (chao_take_upkeep_tick(chao)) {
        chao_emotion_add(chao, CHAO_TIREDNESS, chao_tuning.run_tiredness_gain);
        chao_emotion_add(chao, CHAO_BOREDOM, -chao_tuning.run_boredom_loss);
    }
    if (behavior->timer > CHASE_FRAMES) {
        garden_clear_partner(chao);
        return BEHAVIOR_FINISH;
    }
    if (chao_emotion_get(chao, CHAO_BOREDOM) < 1000 ||
        chao_emotion_get(chao, CHAO_TIREDNESS) > 7000) {
        garden_clear_partner(chao);
        return BEHAVIOR_FINISH;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_try_ball(struct task *chao)
{
    Uint32 boredom;
    if (garden_ball_rolling) {
        boredom = chao_emotion_get(chao, CHAO_BOREDOM);
        if (boredom > 4000) {
            if (njDistanceP2P(&garden_ball->work->pos, &chao->work->pos) <
                50.0F) {
                chao_emotion_add(chao, CHAO_BOREDOM, -2000);
                chao_set_behavior(chao, chase_ball);
                return TRUE;
            }
        }
    }
    return FALSE;
}
