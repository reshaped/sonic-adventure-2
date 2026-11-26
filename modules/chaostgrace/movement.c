/* a racer's behaviors on the course. its push in each terrain grows with its
 * skill there. a racer with no stamina left gets TIRED_SCALE of that push. one
 * a player cheers gets CHEER_SCALE of it, or FLY_CHEER_SCALE in the air */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "chao/behavior.h"
#include "chao/face.h"
#include "chao/icon.h"
#include "chao/idle.h"
#include "chao/life.h"
#include "chao/minimal.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chaostgrace/movement.h"
#include "chaostgrace/race.h"
#include "collision/surface.h"
#include "draw/motion.h"
#include "object/mover.h"
#include "shared/water_ripple.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define AT_WALL (MOVER_AT_WALL | MOVER_FACING_WALL)
/* each frame a runner pushes forward by RUN_PUSH. it pushes RUN_PER_POINT more
 * for each point of its run skill. its motion plays at RUN_RATE times its
 * speed */
#define RUN_PUSH 0.07F
#define RUN_PER_POINT 0.000008F
#define RUN_RATE 16.0F
/* a flier, a swimmer and a climber push as a runner does, by the skill of
 * their terrain. a climber's motion plays at CLIMB_RATE times its speed */
#define FLY_PUSH 0.025F
#define FLY_PER_POINT (0.06F / 10000.0F)
#define SWIM_PUSH 0.0035F
#define SWIM_PER_POINT 0.000001F
#define CLIMB_PUSH 0.05F
#define CLIMB_PER_POINT 0.000009F
#define CLIMB_RATE 15.0F
/* a flier's motion plays at FLY_MOTION_SPEED */
#define FLY_MOTION_SPEED 1.6F
/* each frame a flier or a swimmer loses RISE_DRAG of its speed up or down. it
 * loses FLY_DRAG or SWIM_DRAG of its speed across. a flier also sinks FLY_SINK
 * a frame. it sinks FLY_SINK_PER_POINT more for each point its fly skill falls
 * short of SKILL_CEILING */
#define RISE_DRAG 0.05F
#define FLY_DRAG 0.1F
#define SWIM_DRAG 0.06F
#define FLY_SINK 0.01F
#define FLY_SINK_PER_POINT 0.0000005F
#define SKILL_CEILING 10000
/* a climber keeps CLIMB_SLIDE of its speed across each frame. it goes
 * CLIMB_REACH times as far forward as up */
#define CLIMB_SLIDE 0.95F
#define CLIMB_REACH 1.1F
/* a jump multiplies the racer's speed across by JUMP_BOOST and starts it up
 * at JUMP_RISE. a jumper falls at JUMP_GRAVITY of gravity */
#define JUMP_BOOST 1.9F
#define JUMP_RISE 0.38F
#define JUMP_GRAVITY 0.75F
/* a flier takes off at FLY_RISE. a swimmer keeps SWIM_ENTRY of its speed up or
 * down as it enters the water */
#define FLY_RISE 0.2F
#define SWIM_ENTRY 0.4F
/* a swimmer's waterline lies WATERLINE_DEPTH below the water's surface. one
 * below it rises SURFACE_RISE a frame */
#define WATERLINE_DEPTH 1.4F
#define SURFACE_RISE 0.05F
#define SWIM_SOUND 0x101f
#define SWIM_PRIORITY 1
#define SWIM_VOLUME (-25)
#define SWIM_SOUND_FRAMES 90
/* a swimmer starts a ripple at a chance of RIPPLE_CHANCE every RIPPLE_PERIOD
 * frames. the ripple starts at RIPPLE_SIZE, grows RIPPLE_GROWTH a step, and
 * fades once past RIPPLE_FADE_SIZE */
#define RIPPLE_PERIOD 15
#define RIPPLE_CHANCE 0.2F
#define RIPPLE_SIZE 0.2F
#define RIPPLE_GROWTH 0.03F
#define RIPPLE_FADE_SIZE 0.7F
/* the steps of a turn, in angle units a frame */
#define SWIM_TURN 0x100
#define FLY_TURN 0x100
#define RUN_TURN 0xa0
#define CLIMB_TURN 0x100
/* a racer waiting for the start stands up after more than START_WAIT
 * frames */
#define START_WAIT 1200
/* a runner above chao_tuning.walk_slow_points may trip, at a chance drawn
 * every TRIP_PERIOD frames. the chance is TRIP_CHANCE, and TRIP_PER_POINT more
 * for each point its run skill falls short of SKILL_CEILING. on the way to the
 * scavenger hunt's toys, ITEMS_TRIP_PER_POINT stands for TRIP_PER_POINT */
#define TRIP_PERIOD 50
#define TRIP_CHANCE 0.001F
#define TRIP_PER_POINT 0.000001F
#define ITEMS_TRIP_PER_POINT 0.0000008F
/* a tripped racer lies TRIP_FALL frames, then shows CHAO_MARK_ANGER for
 * TRIP_MARK frames. it gets up once more than TRIP_REST frames have passed
 * since it showed the mark. while it is cheered, each frame counts twice. each
 * of its motions links in TRIP_LINK frames */
#define TRIP_FALL 70
#define TRIP_LINK 15
#define TRIP_MARK 240
#define TRIP_REST 200
/* a tripped racer on the floor keeps TRIP_FRICTION of its speed each
 * frame */
#define TRIP_FRICTION 0.985F
/* a runner climbs a wall whose normal has less than STEEP of race_wall_up */
#define STEEP 0.3F
/* a racer running to the scavenger hunt's toys spends stamina every
 * ITEMS_STAMINA_PERIOD frames */
#define ITEMS_STAMINA_PERIOD 6
/* a climber turns to face the wall until more than CLIMB_FACE frames pass
 * without facing it. it then climbs CLIMB_STEP forward and CLIMB_LIFT up a
 * frame */
#define CLIMB_FACE 2
#define CLIMB_STEP 0.1F
#define CLIMB_LIFT 0.05F
/* a floor whose normal has more than LEVEL_FLOOR of race_ledge_up is level. a
 * climber that has climbed more than CLIMB_LAND frames runs on from a level
 * floor. after more than CLIMB_FRAMES frames it runs on from anywhere. so it
 * does once it is more than CLIMB_HEIGHT above where it began */
#define CLIMB_LAND 3
#define LEVEL_FLOOR 0.7F
#define CLIMB_FRAMES 25
#define CLIMB_HEIGHT 2.5F
#define TIRED_SCALE 0.7F
#define CHEER_SCALE 1.3F
#define FLY_CHEER_SCALE 1.2F

#define APPLY_STAMINA(speed, cheer_scale)                                      \
    if (!parameters->race_stamina) {                                           \
        (speed) *= TIRED_SCALE;                                                \
    } else if (race_is_cheering(chao)) {                                       \
        (speed) *= (cheer_scale);                                              \
    }

void race_run_forward(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct mover *mover = chao->motion.p;
    Uint16 points = chao_skill_points(chao, CHAO_RUN);
    struct chao_parameters *parameters = &CHAO_WORK(chao)->profile.parameters;
    Float acceleration = RUN_PUSH + (points * RUN_PER_POINT);

    APPLY_STAMINA(acceleration, CHEER_SCALE);
    mover->acc.x = njSin(work->base.ang.y) * acceleration;
    mover->acc.z = njCos(work->base.ang.y) * acceleration;
    chao_set_motion_speed(chao, njScalor(&mover->spd) * RUN_RATE);
}

static void fly_forward(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct mover *mover = chao->motion.p;
    Uint16 points = chao_skill_points(chao, CHAO_FLY);
    struct chao_parameters *parameters = &CHAO_WORK(chao)->profile.parameters;
    Float acceleration = FLY_PUSH + (points * FLY_PER_POINT);

    APPLY_STAMINA(acceleration, FLY_CHEER_SCALE);
    mover->acc.y = -mover->gravity - (mover->spd.y * RISE_DRAG) -
                   ((SKILL_CEILING - points) * FLY_SINK_PER_POINT) - FLY_SINK;
    mover->acc.x =
        (njSin(work->base.ang.y) * acceleration) - (mover->spd.x * FLY_DRAG);
    mover->acc.z =
        (njCos(work->base.ang.y) * acceleration) - (mover->spd.z * FLY_DRAG);
    chao_set_motion_speed(chao, FLY_MOTION_SPEED);
}

static void swim_forward(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct mover *mover = chao->motion.p;
    Uint16 points = chao_skill_points(chao, CHAO_SWIM);
    struct chao_parameters *parameters = &CHAO_WORK(chao)->profile.parameters;
    Float acceleration = SWIM_PUSH + (points * SWIM_PER_POINT);

    APPLY_STAMINA(acceleration, CHEER_SCALE);
    mover->acc.y = -mover->gravity - (mover->spd.y * RISE_DRAG);
    mover->acc.x =
        (njSin(work->base.ang.y) * acceleration) - (mover->spd.x * SWIM_DRAG);
    mover->acc.z =
        (njCos(work->base.ang.y) * acceleration) - (mover->spd.z * SWIM_DRAG);
}

static void climb_forward(struct task *chao)
{
    struct mover *mover = chao->motion.p;
    NJS_POINT3 *position = &chao->work->pos;
    Uint16 points = chao_skill_points(chao, CHAO_POWER);
    struct chao_parameters *parameters = &CHAO_WORK(chao)->profile.parameters;
    Float speed = CLIMB_PUSH + (points * CLIMB_PER_POINT);
    Float forward;
    Angle heading;

    APPLY_STAMINA(speed, CHEER_SCALE);
    mover->spd.x *= CLIMB_SLIDE;
    mover->spd.z *= CLIMB_SLIDE;
    forward = speed * CLIMB_REACH;
    heading = CHAO_WORK(chao)->base.ang.y;
    position->x += forward * njSin(heading);
    position->y += speed;
    position->z += forward * njCos(heading);
    mover->acc.y = -mover->gravity;
    chao_set_motion_speed(chao, speed * CLIMB_RATE);
}

int race_jump(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    struct mover *mover = chao->motion.p;

    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_TOBU);
        mover->spd.x *= JUMP_BOOST;
        mover->spd.y = JUMP_RISE;
        mover->spd.z *= JUMP_BOOST;
        behavior->timer = 0;
        behavior->mode++;
        break;
    case 1:
        if (mover->flags & MOVER_FACING_WALL) {
            chao_set_behavior(chao, race_climb);
            mover_clear_spd(chao);
        }
        if (mover->flags & MOVER_ON_FLOOR) {
            chao_set_behavior(chao, race_run);
        }
        break;
    }
    mover->acc.y = -mover->gravity * JUMP_GRAVITY;
    return BEHAVIOR_CONTINUE;
}

int race_swim(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    struct mover *mover = chao->motion.p;
    Uint16 points = chao_skill_points(chao, CHAO_SWIM);

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        mover->spd.y *= SWIM_ENTRY;
        if (points < chao_tuning.swim_intermediate_points) {
            chao_link_motion(chao, CHAO_ANIM_OBORERU);
        } else if (points < chao_tuning.swim_advanced_points) {
            chao_link_motion(chao, CHAO_ANIM_BATAASHI_B);
        } else {
            chao_link_motion(chao, CHAO_ANIM_CRAWL);
        }
        behavior->mode++;
        behavior->timer = 0;
        /* @hack spelling: the flags through CHAO_WORK(chao), where
         * work->flags would do */
        CHAO_WORK(chao)->flags &= ~CHAO_COURSE_CONSTRAIN;
        CHAO_WORK(chao)->flags |= CHAO_RACE_STAGE_COLLISION;
        /* fall through */
    case 1:
        break;
    }
    if (behavior->count++ % RIPPLE_PERIOD == 0 &&
        rand() * RAND_SCALE < RIPPLE_CHANCE) {
        NJS_POINT3 position;
        NJS_VECTOR growth;
        position.x = work->base.pos.x;
        position.y = mover->flat_y;
        position.z = work->base.pos.z;
        growth.x = RIPPLE_GROWTH;
        growth.y = 0.0F;
        growth.z = RIPPLE_FADE_SIZE;
        ripple_create(&position, &growth, RIPPLE_SIZE);
    }
    swim_forward(chao);
    if (mover->flags & AT_WALL) {
        chao_set_behavior(chao, race_climb);
        mover_clear_spd(chao);
    } else if (work->base.pos.y <= mover->flat_y - WATERLINE_DEPTH) {
        work->base.pos.y += SURFACE_RISE;
        if (work->base.pos.y > mover->flat_y - WATERLINE_DEPTH) {
            work->base.pos.y = mover->flat_y - WATERLINE_DEPTH;
        }
    } else if (mover->flags & MOVER_ON_FLOOR) {
        chao_set_behavior(chao, race_run);
    } else {
        work->base.pos.y = mover->flat_y - WATERLINE_DEPTH;
    }
    mover_turn_to_target_by(chao, SWIM_TURN);
    sound_play_at_for(SWIM_SOUND, chao, SWIM_PRIORITY, SWIM_VOLUME,
                      SWIM_SOUND_FRAMES, &work->base.pos);
    return BEHAVIOR_CONTINUE;
}

int race_wait_for_start(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct chao_work *work = CHAO_WORK(chao);

    switch (behavior->mode) {
    case 0: {
        Float choice = rand() * RAND_SCALE;
        if (choice < 0.2F) {
            chao_link_motion(chao, CHAO_ANIM_STAND);
        } else if (choice < 0.4F) {
            chao_link_motion(chao, CHAO_ANIM_DAINOJI);
            chao_set_eye(chao, 2);
        } else if (choice < 0.6F) {
            chao_link_motion(chao, CHAO_ANIM_HIMA_SIT);
        } else if (choice < 0.8F) {
            chao_link_motion(chao, CHAO_ANIM_OSYABURI_L);
        } else {
            chao_link_motion(chao, CHAO_ANIM_HIMATATI_PATA);
        }
    }
        if (work->profile.parameters.body == CHAO_BODY_MINIMAL) {
            motion_link(&work->minimal_motion, MINIMAL_IDLE);
        }
        behavior->mode++;
        behavior->timer = 0;
        /* fall through */
    case 1:
        if (work->base.timer++ > START_WAIT) {
            chao_set_behavior(chao, chao_stand_up_behavior);
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

int race_trip(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct mover *mover;
    Float friction;

    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_KOKE, TRIP_LINK);
        behavior->mode++;
        behavior->timer = 0;
        /* fall through */
    case 1:
        if (behavior->timer++ > TRIP_FALL) {
            chao_set_eye(chao, CHAO_EYE_DISTRESSED);
            chao_show_mark(chao, CHAO_MARK_ANGER, TRIP_MARK);
            behavior->mode++;
            behavior->timer = 0;
        }
        break;
    case 2:
        if (race_is_cheering(chao)) {
            behavior->timer++;
        }
        if (behavior->timer++ > TRIP_REST) {
            chao_link_motion_frames(chao, CHAO_ANIM_KOKEOKI, TRIP_LINK);
            behavior->mode++;
        }
        break;
    case 3:
        if (chao_motion_ended(chao)) {
            chao_resume_behavior(chao);
        }
        break;
    }
    /* @hack spelling: friction, a local holding TRIP_FRICTION, where
     * TRIP_FRICTION would do */
    friction = TRIP_FRICTION;
    mover = chao->motion.p;
    mover->flags |= MOVER_SLOPE_BOUNCE;
    if (mover->flags & MOVER_ON_FLOOR) {
        mover->spd.x *= friction;
        mover->spd.y *= friction;
        mover->spd.z *= friction;
    }
    return BEHAVIOR_CONTINUE;
}

int race_fly(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    struct mover *mover = chao->motion.p;

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_TOBU);
        mover->spd.y = FLY_RISE;
        behavior->timer = 0;
        behavior->mode++;
        /* fall through */
    case 1:
        break;
    }
    mover_turn_to_target_by(chao, FLY_TURN);
    if (!race_check_water(chao) && (mover->flags & MOVER_ON_FLOOR)) {
        chao_set_behavior(chao, race_run);
    }
    if (mover->flags & AT_WALL) {
        chao_set_behavior(chao, race_climb);
        mover_clear_spd(chao);
    }
    fly_forward(chao);
    return BEHAVIOR_CONTINUE;
}

int race_run(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    struct mover *mover = chao->motion.p;
    Uint16 points = chao_skill_points(chao, CHAO_RUN);

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_walk_motion(chao);
        behavior->mode++;
        behavior->timer = 0;
        chao_restore_eyes(chao);
        /* @hack spelling: the flags through CHAO_WORK(chao), here and in
         * the heading's test, where work->flags would do */
        CHAO_WORK(chao)->flags |= CHAO_COURSE_AIM;
        if (work->profile.parameters.body == CHAO_BODY_MINIMAL) {
            motion_link(&work->minimal_motion, MINIMAL_WALK);
        }
        /* fall through */
    case 1:
        break;
    }
    if (points > chao_tuning.walk_slow_points &&
        behavior->timer % TRIP_PERIOD == 0) {
        if (rand() * RAND_SCALE <
            TRIP_CHANCE + ((SKILL_CEILING - points) * TRIP_PER_POINT)) {
            if (work->profile.parameters.body != CHAO_BODY_MINIMAL) {
                chao_set_behavior(chao, race_trip);
            }
        }
    }
    behavior->timer++;
    if (mover->flags & AT_WALL) {
        NJS_VECTOR up;
        NJS_VECTOR normal;
        up = race_wall_up;
        normal.x = mover->facing_wall.vx;
        normal.y = mover->facing_wall.vy;
        normal.z = mover->facing_wall.vz;
        if (njInnerProduct(&up, &normal) < STEEP) {
            chao_set_behavior(chao, race_climb);
            mover_clear_spd(chao);
        }
    }
    race_check_water(chao);
    if (!(CHAO_WORK(chao)->flags & CHAO_RACE_FIXED_HEADING)) {
        mover_turn_to_target_by(chao, RUN_TURN);
    }
    if (mover->flags & MOVER_ON_FLOOR) {
        race_run_forward(chao);
    }
    if (behavior->timer % chao_tuning.race_stamina_period == 0) {
        chao_add_race_stamina(chao, -chao_tuning.race_stamina_cost);
    }
    return BEHAVIOR_CONTINUE;
}

int race_run_to_items(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    struct mover *mover = chao->motion.p;
    /* @hack redundant: velocity, a local nothing reads */
    NJS_VECTOR *velocity = &mover->spd;
    Uint16 points = chao_skill_points(chao, CHAO_RUN);

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_walk_motion(chao);
        behavior->mode++;
        behavior->timer = 0;
        chao_restore_eyes(chao);
        /* @hack spelling: the flags through CHAO_WORK(chao), here and in
         * the heading's test, where work->flags would do */
        CHAO_WORK(chao)->flags |= CHAO_COURSE_AIM;
        /* fall through */
    case 1:
        break;
    }
    if (points > chao_tuning.walk_slow_points) {
        if (behavior->timer % TRIP_PERIOD == 0) {
            if (rand() * RAND_SCALE < TRIP_CHANCE + ((SKILL_CEILING - points) *
                                                     ITEMS_TRIP_PER_POINT)) {
                chao_set_behavior(chao, race_trip);
            }
        }
    }
    behavior->timer++;
    if (mover->flags & AT_WALL) {
        chao_set_behavior(chao, race_climb);
        mover_clear_spd(chao);
    }
    race_check_water(chao);
    if (!(CHAO_WORK(chao)->flags & CHAO_RACE_FIXED_HEADING)) {
        mover_turn_to_target_by(chao, RUN_TURN);
    }
    if (mover->flags & MOVER_ON_FLOOR) {
        race_run_forward(chao);
    }
    if (behavior->timer % ITEMS_STAMINA_PERIOD == 0) {
        chao_add_race_stamina(chao, -chao_tuning.race_stamina_cost);
    }
    return BEHAVIOR_CONTINUE;
}

int race_climb(struct task *chao)
{
    Angle direction;
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    struct mover *mover = chao->motion.p;

    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_NOBORU);
        /* @hack spelling: the flags through CHAO_WORK(chao), and the yaw
         * and position through chao->work, here and below, where work
         * would do */
        CHAO_WORK(chao)->flags &= ~CHAO_COURSE_CONSTRAIN;
        behavior->mode++;
        /* fall through */
    case 1:
        if (mover->flags & MOVER_FACING_WALL) {
            Float wall_z = mover->facing_wall.vz;
            Float wall_x = mover->facing_wall.vx;
            direction = (Angle)(atan2f(wall_x, wall_z) * ANGLE_PER_RADIAN);
            chao->work->ang.y = angle_approach(
                chao->work->ang.y, direction + HALF_TURN, CLIMB_TURN);
            behavior->timer = 0;
            mover->flags &= ~AT_WALL;
        } else if (behavior->timer++ > CLIMB_FACE) {
            behavior->timer = 0;
            work->climb_start_y = chao->work->pos.y;
            behavior->mode++;
        }
        break;
    case 2: {
        Angle heading;
        NJS_POINT3 *position;
        Float forward;
        behavior->timer++;
        /* @hack spelling: forward, a local holding CLIMB_STEP, where
         * CLIMB_STEP would do */
        forward = CLIMB_STEP;
        heading = chao->work->ang.y;
        position = &chao->work->pos;
        position->x += forward * njSin(heading);
        position->y += CLIMB_LIFT;
        position->z += forward * njCos(heading);
        if (behavior->timer > CLIMB_LAND && (mover->flags & MOVER_ON_FLOOR)) {
            NJS_VECTOR up;
            up = race_ledge_up;
            if (njInnerProduct(&mover->probe.sides[SURFACE_NEG_Y].normal, &up) >
                LEVEL_FLOOR) {
                CHAO_WORK(chao)->flags |= CHAO_COURSE_CONSTRAIN;
                CHAO_WORK(chao)->flags &= ~CHAO_RACE_STAGE_COLLISION;
                chao_set_behavior(chao, race_run);
            }
        } else if (mover->flags & AT_WALL) {
            behavior->mode--;
        }
        if (behavior->timer > CLIMB_FRAMES ||
            chao->work->pos.y - work->climb_start_y > CLIMB_HEIGHT) {
            CHAO_WORK(chao)->flags |= CHAO_COURSE_CONSTRAIN;
            CHAO_WORK(chao)->flags &= ~CHAO_RACE_STAGE_COLLISION;
            chao_set_behavior(chao, race_run);
        }
    } break;
    }
    climb_forward(chao);
    return BEHAVIOR_CONTINUE;
}
