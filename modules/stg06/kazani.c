/* KAZANI in Sky Rail's object list: a flock of flyers about a box. each steers
 * toward targets of its own. the flight follows stg03/dragonfly_swarm.c's,
 * with tunings and turns of its own */
#include <mathf.h>
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "stg06/kazani.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the low byte of the work's ang.x, plus 1, is the count of flyers */
#define COUNT_BYTE 0xff
/* the step caps the low byte of ang.x at COUNT_BYTE_CAP. the count, which the
 * init fixed, stays */
#define COUNT_BYTE_CAP 20

/* a flyer, in the flock's axes: about the work's pos, turned by its ang.y */
struct flyer {
    NJS_POINT3 pos;
    NJS_POINT3 target;
    NJS_VECTOR spd;
    /* what the flyer's speed gains toward its target each step */
    Float accel;
    /* its pitch, heading and bank, which each step turns toward their aims */
    struct angles ang;
    /* a frame, which the step runs on and nothing reads */
    Float frame;
    /* the steps it keeps its target before it picks another */
    Sint32 target_steps;
    /* the steps left of its burst. while it isn't 0, accel grows each step */
    Sint32 burst_steps;
};

/* the flock's record, in task->motion */
struct kazani {
    struct flyer *flyers;
    Sint32 count;
};

#define KAZANI(task) ((struct kazani *)(task)->motion.p)

/* the set editor's labels for the flock's placement */
static const char kazani_edit_kamomenum[16] = "<- KAMOMENUM";
static const char kazani_edit_angle_y[12] = "<- ANGLE Y";
static const char kazani_edit_scl_x[12] = "<- SCL X";
static const char kazani_edit_scl_y[12] = "<- SCL Y";
static const char kazani_edit_scl_z[12] = "<- SCL Z";

#include "stg06/kazani/text.inc"
#include "stg06/scenery_art/kazani.h"

/* a flyer's target starts at 0 in x. in y it lies within half TARGET_SPREAD_Y
 * of 0, and in z within half TARGET_SPREAD_Z. it then turns about y by a random
 * whole count of quarter turns, below TARGET_TURNS. only then does it scale by
 * scl */
#define TARGET_SPREAD_Y 2.0F
#define TARGET_SPREAD_Z 1.9F
#define TARGET_TURNS 16
/* a flyer keeps a target TARGET_STEPS steps, plus up to TARGET_STEPS_RANDOM
 * more */
#define TARGET_STEPS 120
#define TARGET_STEPS_RANDOM 1000.0F
/* a flyer starts in a spread of POS_SPREAD times scl about the box's middle. it
 * starts in a burst of FIRST_BURST steps. its accel starts up to
 * FIRST_ACCEL_RANDOM above what its first target gives */
#define POS_SPREAD 2.5F
#define FIRST_BURST 20.0F
#define FIRST_BURST_RANDOM 50.0F
#define FIRST_ACCEL_RANDOM 0.1F
/* once the frame passes MIN_STEP_FRAME, it runs no slower than MIN_FRAME_STEP
 * a step. it wraps at FRAME_WRAP */
#define MIN_STEP_FRAME 1.0F
#define MIN_FRAME_STEP 0.01F
#define FRAME_WRAP 9.0F
/* a slow flyer out of a burst starts one with a chance of BURST_CHANCE a step.
 * the burst lasts BURST_STEPS steps, plus up to BURST_STEPS_RANDOM more. a new
 * target sets a burst of up to RETARGET_BURST_RANDOM steps */
#define BURST_CHANCE 0.1F
#define BURST_STEPS 6
#define BURST_STEPS_RANDOM 12.0F
#define RETARGET_BURST_RANDOM 10.0F
/* on each step that starts no burst, a flyer keeps kazani_drag of its speed.
 * along y it keeps CLIMB_DRAG of that again */
#define CLIMB_DRAG 0.99F
/* a flyer within TARGET_REACHED of its target picks another */
#define TARGET_REACHED 70.0F
/* a flyer turns its heading and pitch only while it moves faster than
 * MOVING_SPEED */
#define MOVING_SPEED 0.001F
/* the steepest pitch, up or down */
#define MAX_PITCH NJM_DEG_ANG(30)
/* each step turns the pitch and the heading toward their aims. the turn is
 * capped at MAX_TURN for the pitch and at MAX_HEADING_TURN for the heading. it
 * is then cut to TURN_SHARE of itself, its fraction dropped. the bank turns the
 * whole way to its aim, but no more than MAX_TURN a step */
#define MAX_TURN NJM_DEG_ANG(8)
#define MAX_HEADING_TURN NJM_DEG_ANG(6)
#define TURN_SHARE 0.3F
/* the steer turns the direction to a flyer's target to within MAX_STEER of the
 * direction it moves. it does so only while the flyer's squared speed across x
 * and z passes STEER_SPEED_SQ. it then aims the bank at minus half of that
 * capped turn. for a slower flyer, it aims the bank at BANK_EASE of where the
 * bank is */
#define STEER_SPEED_SQ 0.0001F
#define MAX_STEER EIGHTH_TURN
#define BANK_EASE 0.97F
/* the steer adds speed toward a target only while it lies farther than
 * STEER_MIN_DISTANCE. that keeps the division by the distance off 0 */
#define STEER_MIN_DISTANCE 0.0001F
/* the steer adds CLIMB of the flyer's accel up, and DIVE of it down */
#define CLIMB 0.5F
#define DIVE 0.7F
/* a flyer draws turned down by DRAW_PITCH more than its pitch. it doesn't draw
 * where a ball of DRAW_RADIUS about it lies wholly behind the camera */
#define DRAW_PITCH 0x200
#define DRAW_RADIUS 25.0F
/* a flyer pitches and rolls by up to SWAY either way. each sways on a sine of
 * the flyer's steps times its _RATE. a flyer's steps are game_state.steps plus
 * SWAY_LEAD for each flyer before it. the flyer bobs by up to BOB, a quarter
 * turn ahead of the pitch */
#define SWAY_LEAD 55
#define BOB 3.0F
#define SWAY (3.0F * ANGLE_PER_DEGREE)
#define PITCH_RATE 0x100
#define ROLL_RATE 0xe0

/* a flyer's frame runs at kazani_frame_rate times its accel a step */
static Float kazani_frame_rate = 5.0F;
/* a new target adds kazani_accel_base to accel, and up to kazani_accel_random
 * more. a burst adds kazani_burst_accel each step */
static Float kazani_accel_base = 0.01F;
static Float kazani_accel_random = 0.008F;
static Float kazani_burst_accel = 0.001F;
/* a flyer whose step brings it nearer its target by less than kazani_slow_speed
 * is slow. a flyer starts with up to that speed either way along each axis */
static Float kazani_slow_speed = 0.03F;
static Float kazani_drag = 0.99F;
/* each step a flyer keeps kazani_accel_keep of its accel */
static Float kazani_accel_keep = 0.95F;
/* what each step adds to a flyer's speed along y. it is -0.0, so the add
 * changes no speed */
static Float kazani_gravity = -0.0F;
/* a flyer's speed is held to kazani_top_speed */
static Float kazani_top_speed = 3.0F;
NJS_TEXLIST *kazani_texlists[] = {&kazani_texlist, NULL};

static void kazani_destroy(struct task *task);
static void kazani_steer(struct task *task, struct flyer *flyer);
static void kazani_step(struct task *task);
static void kazani_draw(struct task *task);

/* give flyer a new target and the steps it keeps it, and add to flyer's
 * accel */
static void kazani_pick_target(struct task *task, struct flyer *flyer)
{
    struct task_work *work = task->work;
    NJS_VECTOR target;

    target.z = (((Float)rand() * RAND_SCALE) - 0.5F) * TARGET_SPREAD_Z;
    target.y = (((Float)rand() * RAND_SCALE) - 0.5F) * TARGET_SPREAD_Y;
    target.x = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njScale(NULL, work->scl.x, work->scl.y, work->scl.z);
    njRotateY(NULL, (int)((Float)rand() * (TARGET_TURNS * RAND_SCALE)) *
                        (Float)QUARTER_TURN);
    njCalcVector(NULL, &target, &target);
    njPopMatrix(1);
    flyer->target = target;
    flyer->accel +=
        ((kazani_accel_random * rand()) * RAND_SCALE) + kazani_accel_base;
    flyer->target_steps =
        (int)((Float)rand() * (TARGET_STEPS_RANDOM * RAND_SCALE)) +
        TARGET_STEPS;
}

void kazani_init(struct task *task)
{
    struct task_work *work = task->work;
    struct flyer *flyer;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct kazani));
    if (task->motion.p == NULL) {
        return;
    }
    KAZANI(task)->flyers =
        syCalloc((work->ang.x & COUNT_BYTE) + 1, sizeof(struct flyer));
    if (KAZANI(task)->flyers == NULL) {
        syFree(task->motion.p);
        task->motion.p = NULL;
        return;
    }
    KAZANI(task)->count = (work->ang.x & COUNT_BYTE) + 1;
    for (i = 0, flyer = KAZANI(task)->flyers; i < KAZANI(task)->count;
         i++, flyer++) {
        kazani_pick_target(task, flyer);
        flyer->pos.x =
            (((Float)rand() * RAND_SCALE) - 0.5F) * work->scl.x * POS_SPREAD;
        flyer->pos.y =
            (((Float)rand() * RAND_SCALE) - 0.5F) * work->scl.y * POS_SPREAD;
        flyer->pos.z =
            (((Float)rand() * RAND_SCALE) - 0.5F) * work->scl.z * POS_SPREAD;
        flyer->spd.x =
            (((Float)rand() * RAND_SCALE) - 0.5F) * kazani_slow_speed * 2.0F;
        flyer->spd.y =
            (((Float)rand() * RAND_SCALE) - 0.5F) * kazani_slow_speed * 2.0F;
        flyer->spd.z =
            (((Float)rand() * RAND_SCALE) - 0.5F) * kazani_slow_speed * 2.0F;
        /* @bug logic: the first burst never gets its random steps. the cast
         * to int comes before the scale by FIRST_BURST_RANDOM, at
         * 0x8c50300e, so it cuts rand() * RAND_SCALE, which is below 1, to
         * 0. kazani_pick_target's turn scales before its cast, at
         * 0x8c502dfa */
        flyer->burst_steps += FIRST_BURST + ((int)((Float)rand() * RAND_SCALE) *
                                             FIRST_BURST_RANDOM);
        flyer->accel += (Float)rand() * (FIRST_ACCEL_RANDOM * RAND_SCALE);
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = kazani_draw;
    task->step = kazani_step;
    task->destructor = kazani_destroy;
    work->timer = 0;
}

static void kazani_destroy(struct task *task)
{
    if (KAZANI(task)->flyers != NULL) {
        syFree(KAZANI(task)->flyers);
    }
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

/* add flyer's accel to its speed, along its way to its target as the steer
 * turns it. set the bank's aim too */
static void kazani_steer(struct task *task, struct flyer *flyer)
{
    NJS_VECTOR way;
    Angle target_ang;
    Angle moving;
    Sint16 turn;
    Float length;

    if ((flyer->spd.x * flyer->spd.x) + (flyer->spd.z * flyer->spd.z) >
        STEER_SPEED_SQ) {
        /* @hack spelling: three Float pointers stepped through the vectors,
         * where the members' differences written out would do */
        {
            Float *src = &flyer->target.x;
            Float *sub = &flyer->pos.x;
            Float *dst = &way.x;

            *dst++ = *src++ - *sub++;
            *dst++ = *src++ - *sub++;
            *dst = *src - *sub;
        }
        target_ang = atan2f(way.x, way.z) * ANGLE_PER_RADIAN_F;
        moving = atan2f(flyer->spd.x, flyer->spd.z) * ANGLE_PER_RADIAN_F;
        turn = target_ang - moving;
        if (turn > MAX_STEER || turn < -MAX_STEER) {
            if (turn > MAX_STEER) {
                turn = MAX_STEER;
            }
            if (turn < -MAX_STEER) {
                turn = -MAX_STEER;
            }
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, turn - target_ang + moving);
            njCalcVector(NULL, &way, &way);
            njPopMatrix(1);
        }
        flyer->ang.z = -(turn / 2);
    } else {
        /* @hack spelling: three Float pointers stepped through the vectors,
         * where the members' differences written out would do */
        {
            Float *src = &flyer->target.x;
            Float *sub = &flyer->pos.x;
            Float *dst = &way.x;

            *dst++ = *src++ - *sub++;
            *dst++ = *src++ - *sub++;
            *dst = *src - *sub;
        }
        /* @hack type: the bank's low half read and written as a Sint16,
         * where the whole Angle would do. the bank stays within a Sint16 */
        {
            Sint16 *bank = (Sint16 *)&flyer->ang.z;
            Float value = *bank;

            *bank = value * BANK_EASE;
        }
    }
    length = njScalor(&way);
    if (length > STEER_MIN_DISTANCE) {
        length = flyer->accel / length;
        /* @hack spelling: a Float pointer stepped through way, where the
         * members scaled one by one would do */
        {
            Float *dst = &way.x;

            *dst++ *= length;
            *dst++ *= length;
            *dst *= length;
        }
        if (way.y > 0.0F) {
            way.y *= CLIMB;
        } else {
            way.y *= DIVE;
        }
        /* @hack spelling: two Float pointers stepped through the vectors,
         * where the members' sums written out would do */
        {
            Float *dst = &flyer->spd.x;
            Float *src = &way.x;

            *dst++ += *src++;
            *dst++ += *src++;
            *dst += *src;
        }
    }
}

static void kazani_step(struct task *task)
{
    struct task_work *work = task->work;
    struct flyer *flyer;
    int i;
    Sint16 turn;
    Float approach;
    struct angles before;
    NJS_POINT3 next;
    Float rate;
    Float length;
    Sint16 pitch;

    if (placement_try_leave(task)) {
        return;
    }
    work->ang.x &= COUNT_BYTE;
    if (work->ang.x > COUNT_BYTE_CAP) {
        work->ang.x = COUNT_BYTE_CAP;
    }
    /* @hack redundant: a block no path reaches, a stand-in that gives the
     * step the image's registers */
    if (0) {
        game_state.time_stopped = 0;
        player_tasks[0] = NULL;
        njPushMatrixEx();
        njPushMatrixEx();
        njPushMatrixEx();
        player_tasks[1] = NULL;
        player_tasks[2] = NULL;
    }
    if (game_state.time_stopped) {
        return;
    }
    /* @hack spelling: i = i + 1, where i++ would do */
    for (i = 0, flyer = KAZANI(task)->flyers; i < KAZANI(task)->count;
         i = i + 1, flyer++) {
        before = flyer->ang;
        rate = flyer->accel * kazani_frame_rate;
        /* @hack spelling: a test nested in a test, where && would do */
        if (rate < MIN_FRAME_STEP) {
            if (flyer->frame > MIN_STEP_FRAME) {
                rate = MIN_FRAME_STEP;
            }
        }
        flyer->frame += rate;
        while (flyer->frame > FRAME_WRAP) {
            flyer->frame -= FRAME_WRAP;
        }
        /* @hack spelling: three Float pointers stepped through the vectors,
         * where the members' sums written out would do */
        {
            Float *add = &flyer->pos.x;
            Float *src = &flyer->spd.x;
            Float *dst = &next.x;

            *dst++ = *src++ + *add++;
            *dst++ = *src++ + *add++;
            *dst = *src + *add;
        }
        /* @hack spelling: the minus of the next distance plus the present
         * one, where the present one less the next would do */
        approach = -njDistanceP2P(&next, &flyer->target) +
                   njDistanceP2P(&flyer->pos, &flyer->target);
        if (flyer->burst_steps) {
            flyer->accel += kazani_burst_accel;
            flyer->burst_steps--;
        }
        if (approach < kazani_slow_speed &&
            (Float)rand() * RAND_SCALE < BURST_CHANCE &&
            flyer->burst_steps == 0) {
            flyer->burst_steps =
                (int)((Float)rand() * (BURST_STEPS_RANDOM * RAND_SCALE)) +
                BURST_STEPS;
        } else {
            /* @hack spelling: a Float pointer stepped through spd, where
             * the members scaled one by one would do */
            {
                Float *dst = &flyer->spd.x;

                *dst++ *= kazani_drag;
                *dst++ *= kazani_drag;
                *dst *= kazani_drag;
            }
            flyer->spd.y *= CLIMB_DRAG;
        }
        flyer->spd.y += kazani_gravity;
        kazani_steer(task, flyer);
        flyer->accel *= kazani_accel_keep;
        /* @hack spelling: two Float pointers stepped through the vectors,
         * where the members' sums written out would do */
        {
            Float *dst = &flyer->pos.x;
            Float *src = &flyer->spd.x;

            *dst++ += *src++;
            *dst++ += *src++;
            *dst += *src;
        }
        length = njScalor(&flyer->spd);
        if (length > kazani_top_speed) {
            length = kazani_top_speed / length;
            /* @hack spelling: a Float pointer stepped through spd, where
             * the members scaled one by one would do */
            {
                Float *dst = &flyer->spd.x;

                *dst++ *= length;
                *dst++ *= length;
                *dst *= length;
            }
        }
        if (njDistanceP2P(&flyer->target, &flyer->pos) < TARGET_REACHED ||
            flyer->target_steps-- <= 0) {
            kazani_pick_target(task, flyer);
            /* @hack type: the burst's steps cast through a Sint8, where int
             * would do. the value stays below RETARGET_BURST_RANDOM */
            flyer->burst_steps =
                (Sint8)((Float)rand() * (RETARGET_BURST_RANDOM * RAND_SCALE));
        }
        if (njScalor(&flyer->spd) > MOVING_SPEED) {
            flyer->ang.x =
                atan2f(flyer->spd.y, njSqrt((flyer->spd.x * flyer->spd.x) +
                                            (flyer->spd.z * flyer->spd.z))) *
                ANGLE_PER_RADIAN_F;
            flyer->ang.y =
                atan2f(flyer->spd.x, flyer->spd.z) * ANGLE_PER_RADIAN_F;
            pitch = flyer->ang.x;
            if (pitch > MAX_PITCH) {
                pitch = MAX_PITCH;
            }
            if (pitch < -MAX_PITCH) {
                pitch = -MAX_PITCH;
            }
            flyer->ang.x = pitch;
        }
        turn = flyer->ang.x - before.x;
        if (turn > MAX_TURN) {
            turn = MAX_TURN;
        }
        if (turn < -MAX_TURN) {
            turn = -MAX_TURN;
        }
        /* @hack type: the turn scaled through a Sint16 pointer and a Float,
         * where turn *= TURN_SHARE would do */
        {
            Sint16 *at = &turn;
            Float value = *at;

            *at = value * TURN_SHARE;
        }
        flyer->ang.x = before.x + turn;
        turn = flyer->ang.y - before.y;
        if (turn > MAX_HEADING_TURN) {
            turn = MAX_HEADING_TURN;
        }
        if (turn < -MAX_HEADING_TURN) {
            turn = -MAX_HEADING_TURN;
        }
        /* @hack type: the turn scaled through a Sint16 pointer and a Float,
         * where turn *= TURN_SHARE would do */
        {
            Sint16 *at = &turn;
            Float value = *at;

            *at = value * TURN_SHARE;
        }
        flyer->ang.y = before.y + turn;
        turn = flyer->ang.z - before.z;
        if (turn > MAX_TURN) {
            turn = MAX_TURN;
        }
        if (turn < -MAX_TURN) {
            turn = -MAX_TURN;
        }
        flyer->ang.z = before.z + turn;
    }
}

static void kazani_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct flyer *flyer;
    NJS_CNK_OBJECT *object;
    int i;
    Sint32 sway;
    Angle pitch;
    Angle roll;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&kazani_texlist);
    njPushMatrixEx();
    for (i = 0, flyer = KAZANI(task)->flyers; i < KAZANI(task)->count;
         i++, flyer++) {
        if (sphere_in_front_no_depth(&flyer->pos, DRAW_RADIUS)) {
            sway = (i * SWAY_LEAD) + game_state.steps;
            object = &kazani_object;
            pitch = njSin(sway * PITCH_RATE) * SWAY;
            roll = njSin(sway * ROLL_RATE) * SWAY;
            njTranslateEx(&flyer->pos);
            njTranslate(NULL, object->pos[0],
                        object->pos[1] +
                            (njSin((sway * PITCH_RATE) + QUARTER_TURN) * BOB),
                        object->pos[2]);
            njRotateY(NULL, flyer->ang.y);
            njRotateX(NULL, -flyer->ang.x + pitch - DRAW_PITCH);
            njRotateZ(NULL, flyer->ang.z + roll);
            njTranslate(NULL, -object->pos[0], -object->pos[1],
                        -object->pos[2]);
            njCnkEasyDrawModel(object->model);
            matrix_load_parent();
        }
    }
    /* @hack redundant: a block no path reaches, a stand-in that gives the
     * draw the image's frame */
    if (0) {
        njRotateZ(NULL, 0);
        njCnkEasyDrawModel(task->motion.p);
        matrix_load_parent();
        njCnkEasyDrawModel(NULL);
        matrix_load_parent();
    }
    njPopMatrix(2);
}
