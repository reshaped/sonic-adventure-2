/* the seagulls, KAMOME in Metal Harbor's object list: a flock that flies
 * about a box. each bird steers toward targets of its own */
#include <mathf.h>
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "object/set.h"
#include "shared/common_object.h"
#include "sound/sound.h"
#include "stg10/seagulls.h"
#include "stg10/seagulls/text.inc"
#include "system/angle.h"
#include "system/cache.h"
#include "system/task.h"
#include "system/util.h"

/* the low byte of the work's ang.x is the count of birds less 1. each step
 * caps the byte at COUNT_BYTE_CAP, but the init has counted the birds by
 * then */
#define COUNT_BYTE 0xff
#define COUNT_BYTE_CAP 20

/* a bird, in the flock's axes: about the work's pos, turned by its ang.y */
struct seagull {
    NJS_POINT3 pos;
    NJS_POINT3 target; /* the point the bird steers for */
    NJS_VECTOR spd;
    Float accel; /* the speed the steer adds toward the target each step */
    /* the pitch in x, the heading in y and the bank in z. the steer sets the
     * bank to minus its turn */
    struct angles ang;
    Float frame; /* the frame of the wings' motion */
    /* the steps left before the bird picks another target */
    Sint32 target_steps;
    /* the steps left in the bird's burst, each of which adds to accel */
    Sint32 burst_steps;
};

/* the flock's record, in task->motion */
struct seagull_flock {
    struct seagull *gulls;
    Sint32 count;
};

#define FLOCK(task) ((struct seagull_flock *)(task)->motion.p)

/* a bird's target lies about the box's middle. it starts at x 0, y within
 * TARGET_SPREAD_Y / 2 and z within TARGET_SPREAD_Z / 2. it then turns about y
 * by fewer than TARGET_TURNS quarter turns, at random. scl then scales it */
#define TARGET_SPREAD_Y 2.0F
#define TARGET_SPREAD_Z 1.9F
#define TARGET_TURNS 16
/* a bird keeps a target TARGET_STEPS steps, and fewer than
 * TARGET_STEPS_RANDOM more */
#define TARGET_STEPS 120
#define TARGET_STEPS_RANDOM 1000.0F
/* a bird starts within POS_SPREAD / 2 times scl of the box's middle along each
 * axis. its first burst lasts FIRST_BURST steps. beside what its first target
 * adds, its accel starts with less than FIRST_ACCEL_RANDOM */
#define POS_SPREAD 2.5F
#define FIRST_BURST 20
#define FIRST_BURST_RANDOM 50.0F
#define FIRST_ACCEL_RANDOM 0.1F
/* a step plays CRY_SOUND with a chance of CRY_CHANCE, at a random point of
 * the box */
#define CRY_CHANCE 0.03F
#define CRY_SOUND 12
/* the wings' frame moves accel times seagull_flap_rate a step. past frame 1 it
 * moves at least MIN_FRAME_STEP a step, so only frames 0 to 1 play slower.
 * the frames wrap at FRAME_WRAP */
#define MIN_FRAME_STEP 0.01F
#define FRAME_WRAP 9.0F
/* a slow bird out of a burst starts one with a chance of BURST_CHANCE a step.
 * the burst lasts BURST_STEPS steps, and fewer than BURST_STEPS_RANDOM more. a
 * new target starts a burst of fewer than RETARGET_BURST_RANDOM steps */
#define BURST_CHANCE 0.1F
#define BURST_STEPS 6
#define BURST_STEPS_RANDOM 12.0F
#define RETARGET_BURST_RANDOM 10.0F
/* a bird within TARGET_REACHED of its target picks another */
#define TARGET_REACHED 70.0F
/* a bird turns its heading and pitch only while it moves faster than
 * MOVING_SPEED */
#define MOVING_SPEED 0.001F
/* the steepest pitch, and the most each angle turns in a step */
#define MAX_PITCH DEGREES(60)
#define MAX_TURN DEGREES(8)
/* the steer turns a bird's way to its target to within MAX_STEER of its
 * heading. it steers so only while the bird's speed across x and z, squared,
 * passes STEER_SPEED_SQ. a slower bird's bank keeps BANK_EASE of itself a
 * step */
#define STEER_SPEED_SQ 0.0001F
#define MAX_STEER DEGREES(50)
#define BANK_EASE 0.97F
/* the steer adds speed toward a target only while it lies farther than
 * STEER_MIN_DISTANCE. that keeps the division by the distance off 0 */
#define STEER_MIN_DISTANCE 0.0001F
/* the steer scales the speed it adds along y by CLIMB upward and by DIVE
 * downward */
#define CLIMB 0.5F
#define DIVE 0.7F
/* a bird draws at its pitch plus DRAW_PITCH */
#define DRAW_PITCH 0x200
/* a bird doesn't draw where a ball of DRAW_RADIUS about it lies wholly behind
 * the camera */
#define DRAW_RADIUS 25.0F

/* Seagull body and jointed wings. */

static NJS_TEXNAME seagull_textures[1] = {
#include "stg10/seagulls/seagull_textures.inc"
};

static NJS_TEXLIST seagull_texlist = {
#include "stg10/seagulls/seagull_texlist.inc"
};

static Sint16 seagull_negative_x_wingtip_polygons[25] = {
#include "stg10/seagulls/seagull_negative_x_wingtip_polygons.inc"
};

static Sint32 seagull_negative_x_wingtip_vertices[19] = {
#include "stg10/seagulls/seagull_negative_x_wingtip_vertices.inc"
};

static NJS_CNK_MODEL seagull_negative_x_wingtip_mesh = {
#include "stg10/seagulls/seagull_negative_x_wingtip_mesh.inc"
};

static NJS_CNK_OBJECT seagull_negative_x_wingtip = {
#include "stg10/seagulls/seagull_negative_x_wingtip.inc"
};

static Sint16 seagull_negative_x_wing_polygons[25] = {
#include "stg10/seagulls/seagull_negative_x_wing_polygons.inc"
};

static Sint32 seagull_negative_x_wing_vertices[19] = {
#include "stg10/seagulls/seagull_negative_x_wing_vertices.inc"
};

static NJS_CNK_MODEL seagull_negative_x_wing_mesh = {
#include "stg10/seagulls/seagull_negative_x_wing_mesh.inc"
};

static NJS_CNK_OBJECT seagull_negative_x_wing = {
#include "stg10/seagulls/seagull_negative_x_wing.inc"
};

static Sint16 seagull_positive_x_wingtip_polygons[25] = {
#include "stg10/seagulls/seagull_positive_x_wingtip_polygons.inc"
};

static Sint32 seagull_positive_x_wingtip_vertices[19] = {
#include "stg10/seagulls/seagull_positive_x_wingtip_vertices.inc"
};

static NJS_CNK_MODEL seagull_positive_x_wingtip_mesh = {
#include "stg10/seagulls/seagull_positive_x_wingtip_mesh.inc"
};

static NJS_CNK_OBJECT seagull_positive_x_wingtip = {
#include "stg10/seagulls/seagull_positive_x_wingtip.inc"
};

static Sint16 seagull_positive_x_wing_polygons[25] = {
#include "stg10/seagulls/seagull_positive_x_wing_polygons.inc"
};

static Sint32 seagull_positive_x_wing_vertices[19] = {
#include "stg10/seagulls/seagull_positive_x_wing_vertices.inc"
};

static NJS_CNK_MODEL seagull_positive_x_wing_mesh = {
#include "stg10/seagulls/seagull_positive_x_wing_mesh.inc"
};

static NJS_CNK_OBJECT seagull_positive_x_wing = {
#include "stg10/seagulls/seagull_positive_x_wing.inc"
};

static Sint16 seagull_polygons[179] = {
#include "stg10/seagulls/seagull_polygons.inc"
};

static Sint32 seagull_vertices[91] = {
#include "stg10/seagulls/seagull_vertices.inc"
};

static NJS_CNK_MODEL seagull_mesh = {
#include "stg10/seagulls/seagull_mesh.inc"
};

static NJS_CNK_OBJECT seagull_object = {
#include "stg10/seagulls/seagull_object.inc"
};

/* The flight tracks follow the body, then each wing and its tip. */
static NJS_MKEY_F seagull_position[2] = {
#include "stg10/seagulls/seagull_position.inc"
};

static NJS_MKEY_A seagull_rotation[2] = {
#include "stg10/seagulls/seagull_rotation.inc"
};

static NJS_MKEY_A seagull_positive_x_wing_rotation[10] = {
#include "stg10/seagulls/seagull_positive_x_wing_rotation.inc"
};

static NJS_MKEY_A seagull_positive_x_wingtip_rotation[10] = {
#include "stg10/seagulls/seagull_positive_x_wingtip_rotation.inc"
};

static NJS_MKEY_A seagull_negative_x_wing_rotation[10] = {
#include "stg10/seagulls/seagull_negative_x_wing_rotation.inc"
};

static NJS_MKEY_A seagull_negative_x_wingtip_rotation[10] = {
#include "stg10/seagulls/seagull_negative_x_wingtip_rotation.inc"
};

static NJS_MDATA2 seagull_flight_tracks[5] = {
#include "stg10/seagulls/seagull_flight_tracks.inc"
};

static NJS_MOTION seagull_motion = {
#include "stg10/seagulls/seagull_motion.inc"
};

/* the wings' frames a step for each unit of accel */
static Float seagull_flap_rate = 5.0F;
/* a new target adds seagull_accel_base to accel, and less than
 * seagull_accel_random more */
static Float seagull_accel_base = 0.003F;
static Float seagull_accel_random = 0.003F;
static Float seagull_burst_accel = 0.007F; /* the accel a burst's step adds */
/* the slow speed. a step that brings a bird nearer its target by less than
 * this is slow. a bird starts with less than this speed along each axis */
static Float seagull_slow_speed = 0.2F;
/* the share of its speed a bird keeps each step. on the step its burst
 * starts, it keeps all of it */
static Float seagull_drag = 0.993F;
/* the share of its accel a bird keeps each step */
static Float seagull_accel_keep = 0.965F;
static Float seagull_gravity = -0.001F;
static Float seagull_top_speed = 2.5F;
NJS_TEXLIST *seagulls_texlists[2] = {
#include "stg10/seagulls/seagulls_texlists.inc"
};

static void seagulls_destroy(struct task *task);
static void seagull_steer(struct task *task, struct seagull *gull);
static void seagulls_step(struct task *task);
static void seagulls_draw(struct task *task);

/* give gull a new target, kept for a random count of steps. the new target
 * adds to gull's accel */
static void seagull_pick_target(struct task *task, struct seagull *gull)
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
    gull->target = target;
    gull->accel +=
        seagull_accel_base + ((seagull_accel_random * rand()) * RAND_SCALE);
    gull->target_steps =
        (int)((Float)rand() * (TARGET_STEPS_RANDOM * RAND_SCALE)) +
        TARGET_STEPS;
}

void seagulls_init(struct task *task)
{
    struct task_work *work = task->work;
    struct seagull *gull;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct seagull_flock));
    if (task->motion.p == NULL) {
        return;
    }
    FLOCK(task)->gulls =
        syCalloc((work->ang.x & COUNT_BYTE) + 1, sizeof(struct seagull));
    if (FLOCK(task)->gulls == NULL) {
        syFree(task->motion.p);
        task->motion.p = NULL;
        return;
    }
    FLOCK(task)->count = (work->ang.x & COUNT_BYTE) + 1;
    for (i = 0, gull = FLOCK(task)->gulls; i < FLOCK(task)->count;
         i++, gull++) {
        seagull_pick_target(task, gull);
        gull->pos.x =
            (((Float)rand() * RAND_SCALE) - 0.5F) * work->scl.x * POS_SPREAD;
        gull->pos.y =
            (((Float)rand() * RAND_SCALE) - 0.5F) * work->scl.y * POS_SPREAD;
        gull->pos.z =
            (((Float)rand() * RAND_SCALE) - 0.5F) * work->scl.z * POS_SPREAD;
        gull->spd.x =
            (((Float)rand() * RAND_SCALE) - 0.5F) * seagull_slow_speed * 2.0F;
        gull->spd.y =
            (((Float)rand() * RAND_SCALE) - 0.5F) * seagull_slow_speed * 2.0F;
        gull->spd.z =
            (((Float)rand() * RAND_SCALE) - 0.5F) * seagull_slow_speed * 2.0F;
        /* @bug logic: each bird's first burst lasts FIRST_BURST steps, never
         * more. here, at 0x8c505c0e, the cast truncates rand() * RAND_SCALE,
         * below 1, to 0 before FIRST_BURST_RANDOM scales it.
         * seagull_pick_target scales its turn before the cast, at
         * 0x8c5059fa */
        gull->burst_steps += FIRST_BURST + ((int)((Float)rand() * RAND_SCALE) *
                                            FIRST_BURST_RANDOM);
        gull->accel += (Float)rand() * (FIRST_ACCEL_RANDOM * RAND_SCALE);
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = seagulls_draw;
    task->step = seagulls_step;
    task->destructor = seagulls_destroy;
    work->timer = 0;
}

static void seagulls_destroy(struct task *task)
{
    if (FLOCK(task)->gulls != NULL) {
        syFree(FLOCK(task)->gulls);
    }
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

/* add gull's accel to its speed, along its way to its target, and set its
 * bank */
static void seagull_steer(struct task *task, struct seagull *gull)
{
    NJS_VECTOR way;
    Float length;
    Angle target_ang;
    Angle heading;
    Sint16 turn;

    if ((gull->spd.x * gull->spd.x) + (gull->spd.z * gull->spd.z) >
        STEER_SPEED_SQ) {
        /* @hack spelling: three Float pointers stepped through the vectors,
         * where the members' differences written out would do */
        {
            Float *src = &gull->target.x;
            Float *sub = &gull->pos.x;
            Float *dst = &way.x;

            *dst++ = *src++ - *sub++;
            *dst++ = *src++ - *sub++;
            *dst = *src - *sub;
        }
        target_ang = atan2f(way.x, way.z) * ANGLE_PER_RADIAN;
        heading = atan2f(gull->spd.x, gull->spd.z) * ANGLE_PER_RADIAN;
        turn = target_ang - heading;
        if (turn > MAX_STEER || turn < -MAX_STEER) {
            if (turn > MAX_STEER) {
                turn = MAX_STEER;
            }
            if (turn < -MAX_STEER) {
                turn = -MAX_STEER;
            }
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, turn - target_ang + heading);
            njCalcVector(NULL, &way, &way);
            njPopMatrix(1);
        }
        gull->ang.z = -turn;
    } else {
        /* @hack spelling: three Float pointers stepped through the vectors,
         * where the members' differences written out would do */
        {
            Float *src = &gull->target.x;
            Float *sub = &gull->pos.x;
            Float *dst = &way.x;

            *dst++ = *src++ - *sub++;
            *dst++ = *src++ - *sub++;
            *dst = *src - *sub;
        }
        /* @hack type: the bank's low half read and written as a Sint16, where
         * the whole Angle would do. the bank stays within a Sint16 */
        {
            Sint16 *bank = (Sint16 *)&gull->ang.z;
            Float value = *bank;

            value *= BANK_EASE;
            *bank = value;
        }
    }
    length = njScalor(&way);
    if (length > STEER_MIN_DISTANCE) {
        length = gull->accel / length;
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
        /* @hack spelling: two Float pointers stepped through the vectors, where
         * the members' sums written out would do */
        {
            Float *dst = &gull->spd.x;
            Float *src = &way.x;

            *dst++ += *src++;
            *dst++ += *src++;
            *dst += *src;
        }
    }
}

static void seagulls_step(struct task *task)
{
    struct task_work *work = task->work;
    struct seagull *gull;
    int i;
    Sint16 turn;
    Float approach;
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
     * step the image's registers and frame slots */
    if (0) {
        i = FLOCK(task)->count;
        work->ang.z = i + i;
        placement_try_leave(task);
        work->ang.y = i * i;
        FLOCK(task)->count = (i * i) + (i * i) + (i * i) + (i * i) + (i * i);
        work->ang.x = 0;
        work->scl.z *= 2.0F;
        rand();
        task->motion.p = NULL;
        work->scl.x *= 2.0F;
    }
    if ((Float)rand() * RAND_SCALE < CRY_CHANCE) {
        NJS_VECTOR offset;
        NJS_VECTOR point;

        offset.x = work->scl.x * (((Float)rand() * RAND_SCALE) - 0.5F) * 2.0F;
        offset.y = work->scl.y * (((Float)rand() * RAND_SCALE) - 0.5F) * 2.0F;
        offset.z = work->scl.z * (((Float)rand() * RAND_SCALE) - 0.5F) * 2.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &offset, &point);
        njPopMatrixEx();
        sound_play_at(CRY_SOUND, NULL, 0, 0, &point);
    }
    for (i = 0, gull = FLOCK(task)->gulls; i < FLOCK(task)->count;
         i++, gull++) {
        struct angles before;
        NJS_POINT3 next;

        before = gull->ang;
        rate = gull->accel * seagull_flap_rate;
        if (rate < MIN_FRAME_STEP && gull->frame > 1.0F) {
            rate = MIN_FRAME_STEP;
        }
        gull->frame += rate;
        while (gull->frame > FRAME_WRAP) {
            gull->frame -= FRAME_WRAP;
        }
        /* @hack spelling: three Float pointers stepped through the vectors,
         * where the members' sums written out would do */
        {
            Float *add = &gull->pos.x;
            Float *src = &gull->spd.x;
            Float *dst = &next.x;

            *dst++ = *src++ + *add++;
            *dst++ = *src++ + *add++;
            *dst = *src + *add;
        }
        /* @hack spelling: the minus of the next distance plus the present
         * one, where the present one less the next would do */
        approach = -njDistanceP2P(&next, &gull->target) +
                   njDistanceP2P(&gull->pos, &gull->target);
        if (gull->burst_steps != 0) {
            gull->accel += seagull_burst_accel;
            gull->burst_steps--;
        }
        if (approach < seagull_slow_speed &&
            (Float)rand() * RAND_SCALE < BURST_CHANCE &&
            gull->burst_steps == 0) {
            gull->burst_steps =
                (int)((Float)rand() * (BURST_STEPS_RANDOM * RAND_SCALE)) +
                BURST_STEPS;
        } else {
            /* @hack spelling: a Float pointer stepped through spd, where the
             * members scaled one by one would do */
            Float *dst = &gull->spd.x;

            *dst++ *= seagull_drag;
            *dst++ *= seagull_drag;
            *dst *= seagull_drag;
        }
        gull->spd.y += seagull_gravity;
        seagull_steer(task, gull);
        gull->accel *= seagull_accel_keep;
        /* @hack spelling: two Float pointers stepped through the vectors,
         * where the members' sums written out would do */
        {
            Float *dst = &gull->pos.x;
            Float *src = &gull->spd.x;

            *dst++ += *src++;
            *dst++ += *src++;
            *dst += *src;
        }
        length = njScalor(&gull->spd);
        if (length > seagull_top_speed) {
            length = seagull_top_speed / length;
            /* @hack spelling: a Float pointer stepped through spd, where the
             * members scaled one by one would do */
            {
                Float *dst = &gull->spd.x;

                *dst++ *= length;
                *dst++ *= length;
                *dst *= length;
            }
        }
        if (njDistanceP2P(&gull->target, &gull->pos) < TARGET_REACHED ||
            gull->target_steps-- <= 0) {
            seagull_pick_target(task, gull);
            /* @hack type: the burst's steps cast through a Sint8, where int
             * would do. the value stays below RETARGET_BURST_RANDOM */
            gull->burst_steps =
                (Sint8)((Float)rand() * (RETARGET_BURST_RANDOM * RAND_SCALE));
        }
        if (njScalor(&gull->spd) > MOVING_SPEED) {
            gull->ang.x =
                atan2f(gull->spd.y, njSqrt((gull->spd.x * gull->spd.x) +
                                           (gull->spd.z * gull->spd.z))) *
                ANGLE_PER_RADIAN;
            gull->ang.y = atan2f(gull->spd.x, gull->spd.z) * ANGLE_PER_RADIAN;
            pitch = gull->ang.x;
            if (pitch > MAX_PITCH) {
                pitch = MAX_PITCH;
            }
            if (pitch < -MAX_PITCH) {
                pitch = -MAX_PITCH;
            }
            gull->ang.x = pitch;
        }
        turn = gull->ang.x - before.x;
        if (turn > MAX_TURN) {
            turn = MAX_TURN;
        }
        if (turn < -MAX_TURN) {
            turn = -MAX_TURN;
        }
        gull->ang.x = before.x + turn;
        turn = gull->ang.y - before.y;
        if (turn > MAX_TURN) {
            turn = MAX_TURN;
        }
        if (turn < -MAX_TURN) {
            turn = -MAX_TURN;
        }
        gull->ang.y = before.y + turn;
        turn = gull->ang.z - before.z;
        if (turn > MAX_TURN) {
            turn = MAX_TURN;
        }
        if (turn < -MAX_TURN) {
            turn = -MAX_TURN;
        }
        gull->ang.z = before.z + turn;
    }
}

static void seagulls_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct seagull *gull;
    int i;

    /* @hack redundant: a block no path reaches, kept for its SET editor
     * captions, which SHC still places in this source's constants */
    if (0) {
        (void)"<- KAMOMENUM";
        (void)"<- ANGLE Y";
        (void)"<- SCL X";
        (void)"<- SCL Y";
        (void)"<- SCL Z";
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&seagull_texlist);
    njPushMatrixEx();
    for (i = 0, gull = FLOCK(task)->gulls; i < FLOCK(task)->count;
         i++, gull++) {
        if (sphere_in_front_no_depth(&gull->pos, DRAW_RADIUS)) {
            njTranslateEx(&gull->pos);
            njRotateY(NULL, gull->ang.y);
            njRotateX(NULL, -gull->ang.x - DRAW_PITCH);
            njRotateZ(NULL, gull->ang.z);
            njCnkEasyDrawMotion(&seagull_object, &seagull_motion, gull->frame);
            matrix_load_parent();
        }
    }
    njPopMatrix(2);
}
