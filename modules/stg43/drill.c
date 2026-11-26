/* Cosmic Wall's drill rides, which stg43/drill.h declares. this source holds
 * the platforms, the drills that follow them, the drills' teeth and the
 * DRILLCOL triggers */
#include <mathf.h>
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/flash.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stg43/drill.h"
#include "stg43/drill_assets.h"
#include "stg43/egg_jump.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* a platform's mode, in its work's mode. a drill starts in its platform's
 * mode, and is RIDE_FINISHED once it has run its path. nothing sets
 * RIDE_REVERSE, so a platform at its path's end stays there */
enum ride_mode {
    RIDE_START,
    RIDE_FORWARD,
    RIDE_END,
    RIDE_REVERSE,
    RIDE_FINISHED
};
/* a bit of a platform's work's tick: player 0 sits in the seat */
#define RIDER_SEATED 1
/* a bit of a DRILLCOL's ang.x: the trigger eases speeds, and releases no
 * drill */
#define TRIGGER_SPEED 1
/* the trigger a platform or a drill lies in this step. it is none, one that
 * releases a drill, or one that eases speeds */
enum { NO_TRIGGER = -1, RELEASE_TRIGGER, SPEED_TRIGGER };
/* a speed trigger eases a speed SPEED_EASE of the way to its scl.z each step */
#define SPEED_EASE 0.05F
/* a platform draws player 0 into its seat over BOARD_STEPS steps. at its
 * start, it takes player 0 aboard only once its cooldown is 0. the cooldown
 * is BOARD_COOLDOWN while player 0 stands on it, and counts down while player
 * 0 doesn't */
#define BOARD_STEPS 65
#define BOARD_COOLDOWN 120
/* each boarding step eases player 0's angles BOARD_EASE of the way to the
 * platform's */
#define BOARD_EASE 0.1F
/* the platform and the drill draw only within 2000 of the current camera */
#define DRAW_RANGE_SQUARED 4000000.0F
/* the drill's head turns DRILL_SPIN each step */
#define DRILL_SPIN 0x200
/* the light the drill draws in, an index of lights */
#define DRILL_LIGHT 4
/* the platform's copy in the world takes the size class that its model's
 * radius plus CLASS_MARGIN fits */
#define CLASS_MARGIN 15.0F
/* the platform's copy in the world collides only while a player lies within
 * COLLIDE_RANGE of it */
#define COLLIDE_RANGE 100.0F
/* a hit's flash, which tints the platform and the drill. it starts at 1, and
 * each step keeps FLASH_FADE of it. below FLASH_LEAST, nothing is tinted */
#define FLASH_FADE 0.97F
#define FLASH_LEAST 0.01F
/* the platform's tint at a flash of 1. the drill's light loses the flash
 * times FLASH_SHADE of its green and blue */
#define FLASH_RED 0xffff8080
#define FLASH_SHADE 0.5F
/* the platform's light pulses between drill_light_on and drill_light_off,
 * its phase turning PULSE_RATE each step */
#define PULSE_RATE 0x500
/* the clamps' opening is the boarding timer over CLAMP_STEPS, up to
 * CLAMPS_OPEN. as the opening goes to CLAMPS_RISEN, the clamps rise to
 * CLAMP_RISE. as it goes on to CLAMPS_OPEN, they spread to CLAMP_SPREAD either
 * way along x */
#define CLAMP_STEPS 32.0F
#define CLAMPS_RISEN 1.0F
#define CLAMPS_OPEN 2.0F
#define CLAMP_RISE 3.0F
#define CLAMP_SPREAD 6.0F
/* a running platform's speed stays from PLATFORM_SPEED_LEAST to
 * PLATFORM_SPEED_MOST */
#define PLATFORM_SPEED_LEAST 3.5F
#define PLATFORM_SPEED_MOST 13.0F
/* the ride's sounds. BOARD_SOUND plays as player 0 starts to board, and
 * CLAMP_SOUND at the boarding step CLAMP_SOUND_STEP. SEATED_SOUND plays as
 * player 0 sits, and END_SOUND at the path's end. each of the four plays at
 * SOUND_PRIORITY and SOUND_VOLUME. RIDE_SOUND plays at the platform while it
 * runs, with a priority, volume and time of its own */
#define BOARD_SOUND 0
#define CLAMP_SOUND 1
#define SEATED_SOUND 2
#define RIDE_SOUND 3
#define END_SOUND 4
#define CLAMP_SOUND_STEP 30
#define SOUND_PRIORITY 0
#define SOUND_VOLUME 0
#define RIDE_SOUND_PRIORITY 1
#define RIDE_SOUND_VOLUME 0
#define RIDE_SOUND_STEPS 30
/* vibration_start's power and freq as player 0 starts to board, and its
 * power, freq and time at CLAMP_SOUND_STEP */
#define BOARD_POWER 5
#define BOARD_FREQ 35
#define CLAMP_POWER 2
#define CLAMP_FREQ 55
#define CLAMP_TIME 1
/* vibration_start_fading's power, freq and length as the second ride
 * throws player 0. the throw starts THROW_LIFT above player 0's pos */
#define THROW_POWER 5
#define THROW_FREQ 50
#define THROW_LENGTH 2
#define THROW_LIFT 5.0F
/* a tooth's bit in a drill's work's submode, set once the tooth breaks. the
 * work's tick holds the bits at the step before */
#define TOOTH_BIT(i) (1 << (i))
/* a drill with BROKEN_TEETH teeth broken or more takes the platform's drive
 * and drag. while it isn't stunned, its speed then stays from
 * BROKEN_SPEED_LEAST to BROKEN_SPEED_MOST. with fewer broken, it stays from
 * DRILL_SPEED_LEAST to DRILL_SPEED_MOST */
#define BROKEN_TEETH 5
#define BROKEN_SPEED_LEAST 1.0F
#define BROKEN_SPEED_MOST 10.0F
#define DRILL_SPEED_LEAST 2.5F
#define DRILL_SPEED_MOST 16.0F
/* a drill within NEAR_RANGE of a player takes drill_near_drag too */
#define NEAR_RANGE 335.0F
/* a drill that has run its path takes FINISHED_SPEED back along it as each
 * step's speed */
#define FINISHED_SPEED 0.1F
/* a tooth's break or a player's touch stuns a drill. it adds STUN_TOUCH or
 * STUN_TOOTH steps to the stun, as drill_step's tagged test picks. it keeps
 * STUN_SLOW of the drill's speed */
#define STUN_TOUCH 30
#define STUN_TOOTH 13
#define STUN_SLOW 0.8F
/* a stunned drill flashes while its stun is above STUN_FLASH. each stunned
 * step keeps STUN_SLOW of a speed above STUNNED_SLOWING. it lifts a speed
 * below STUNNED_SPEED_LEAST to it */
#define STUN_FLASH 10
#define STUNNED_SLOWING 3.0F
#define STUNNED_SPEED_LEAST 2.0F
/* a tooth's target sits TOOTH_REACH from the head's axis, TOOTH_Z along it */
#define TOOTH_REACH 90.0F
#define TOOTH_Z 170.0F
/* a tooth's work's submode, TOOTH_STUNNED while its drill is stunned and 0
 * otherwise. tooth_step reads the drill's stun itself, not the submode */
#define TOOTH_STUNNED 1
/* the drill's two sounds play while it has a tooth left and its speed is
 * SOUND_SPEED or more. each plays at a point of the head HEAD_X along x and
 * HEAD_Z along z. LOW_HEAD_SOUND's point lies HEAD_Y below the axis, and
 * HIGH_HEAD_SOUND's as far above. each takes that priority, volume and time */
#define LOW_HEAD_SOUND 6
#define HIGH_HEAD_SOUND 7
#define HEAD_SOUND_PRIORITY 1
#define HEAD_SOUND_VOLUME 30
#define HEAD_SOUND_STEPS 15
#define SOUND_SPEED 2.0F
#define HEAD_X 20.0F
#define HEAD_Y 20.0F
#define HEAD_Z 180.0F
/* the sound a tooth's break plays */
#define TOOTH_SOUND 8
/* a broken tooth flies off along the head's axes, at SHARD_X along x and
 * SHARD_Y along y. along z it takes the drill's speed plus SHARD_Z. it falls
 * to PIECE_FLOOR, far below the stage */
#define SHARD_X 2.0F
#define SHARD_Y 8.0F
#define SHARD_Z 12.0F
#define PIECE_FLOOR (-100000.0F)
/* a tooth's break makes a FlashManager SPARK_Z along the head's axis. its
 * flash's size is SPARK_SIZE plus up to SPARK_SIZE_RANGE more, and its depth
 * bias SPARK_DEPTH_BIAS. it throws SPARKS sparks a step */
#define SPARK_Z 150.0F
#define SPARK_SIZE 55.0F
#define SPARK_SIZE_RANGE 15.0F
#define SPARKS 1
#define SPARK_DEPTH_BIAS 120.0F
/* drill_radial_point's hexagon has sides HEAD_APOTHEM from the head's axis */
#define HEAD_APOTHEM 140.0F
#define TWELFTH_TURN 0x1555

/* the record a platform and a drill each keep in task->motion. collision is
 * a platform's copy of platform_collision, which a drill leaves unused.
 * segment and fraction are the task's place on its track, and speed its
 * move along it a step */
struct drill_motion {
    NJS_OBJECT collision;
    Float fraction;
    int segment;
    Float speed;
    union {
        int cooldown; /* the platform's cooldown, in steps */
        int broken_teeth; /* the drill's broken teeth */
    } state;
    union {
        /* the platform's boarding timer. it counts up to BOARD_STEPS as
         * player 0 boards. at the path's end, it counts back below 0 before
         * the platform lets player 0 go */
        int boarding;
        int stun; /* the steps the drill stays stunned */
    } timer;
    Float flash;
};
#define DRILL(task) ((struct drill_motion *)(task)->motion.p)
/* the path a platform's or a drill's task follows, by its track word */
#ifdef NONMATCHING
/* this side reads the track word as the ride's number, as drill.h says */
#define TRACK(task)                                                            \
    ((task)->unk_3c.l == COSMIC_DRILL_SECOND_TRACK ? &cosmic_wall_second_path  \
                                                   : &cosmic_wall_first_path)
#else
#define TRACK(task) ((struct path *)(task)->unk_3c.p)
#endif
Bool cosmic_drill_active[COSMIC_DRILL_RIDES];

static void platform_destroy(struct task *task);
static void platform_draw(struct task *task);
static void platform_step(struct task *task);
static void drill_radial_point(Angle rotation, NJS_VECTOR *tooth,
                               NJS_VECTOR *point);
static Bool drill_start(struct task *task, int segment, Float fraction,
                        Float speed);
static void drill_destroy(struct task *owner);
static void drill_draw(struct task *task);
static void drill_step(struct task *task);
static struct task *tooth_create(struct task *parent, int index,
                                 NJS_POINT3 *pos, struct collision_shape *shape,
                                 Uint32 count);
static void tooth_step(struct task *task);
static void tooth_destroy(struct task *task);

/* the track the platforms and drills follow: an open PATH_LAYOUT_SLOPES path.
 * track_point extends it with points past either end. a place on it is a
 * segment and a fraction. the segment is the index of the point the segment
 * starts at. the fraction is the share of the way from that point to the
 * next */

/* the point at an index past either end lies OUTSIDE_STEP further out for
 * each index past it. it lies on the line through the end point and the point
 * LINE_SPAN in from it */
#define OUTSIDE_STEP 100.0F
#define LINE_SPAN 2
/* how far ahead of a task's place track_move looks */
#define LOOK_AHEAD 30.0F
/* the share of the way track_move turns its task's ang each step */
#define STEER 0.3F

/* @hack type: turn's low 16 bits scaled through a Sint16 pointer, where
 * turn = (Sint16)(turn) * (factor) would do. the pointer leaves turn's high 16
 * bits as they were. the two forms agree in the low 16 bits, which are all
 * that Ninja's turns read of an angle. read as a Sint16, the low half of a
 * difference of two angles is the turn between them the shorter way round */
#define SCALE_TURN(turn, factor)                                               \
    {                                                                          \
        Sint16 *low = (Sint16 *)&(turn);                                       \
        Float scaled = *low;                                                   \
        scaled *= (factor);                                                    \
        *low = scaled;                                                         \
    }

/* @hack spelling: the vectors as Float pointers, walked a component at a
 * time, where NJS_POINT3 pointers read by field would do */
#pragma inline(track_difference, track_scale, track_offset)
static void track_difference(Float *from, Float *to, Float *out)
{
    *out++ = *to++ - *from++;
    *out++ = *to++ - *from++;
    *out = *to - *from;
}

static void track_scale(Float *vector, Float factor)
{
    *vector++ *= factor;
    *vector++ *= factor;
    *vector *= factor;
}

static void track_offset(Float *origin, Float *offset, Float *out)
{
    *out++ = *offset++ + *origin++;
    *out++ = *offset++ + *origin++;
    *out = *offset + *origin;
}

/* set *pos, *slope_x and *slope_z to the point at index of a track of count
 * points. count must be LINE_SPAN + 1 or more. return TRUE where index lies
 * outside the track, else FALSE. outside it, the slopes are the end point's */
static Bool track_point(struct path_point *points, int count, int index,
                        NJS_POINT3 *pos, Angle *slope_x, Angle *slope_z)
{
    Angle roll;
    Angle pitch;
    NJS_VECTOR direction;
    Float fraction;
    Bool outside;

    if (index >= count) {
        track_difference(&points[count - 1 - LINE_SPAN].pos.x,
                         &points[count - 1].pos.x, &direction.x);
        track_scale(&direction.x,
                    (index - count + 1) * OUTSIDE_STEP / njScalor(&direction));
        track_offset(&points[count - 1].pos.x, &direction.x, &pos->x);
        /* @hack redundant: pitch and roll, the slopes' change from the point
         * LINE_SPAN in from the end to the end, scaled by half of index less
         * count, which nothing reads */
        pitch =
            points[count - 1].slope_x - points[count - 1 - LINE_SPAN].slope_x;
        roll =
            points[count - 1].slope_z - points[count - 1 - LINE_SPAN].slope_z;
        fraction = (index - count) * 0.5F;
        SCALE_TURN(pitch, fraction);
        SCALE_TURN(roll, fraction);
        *slope_x = points[count - 1].slope_x;
        *slope_z = points[count - 1].slope_z;
        outside = TRUE;
    } else if (index < 0) {
        track_difference(&points[LINE_SPAN].pos.x, &points[0].pos.x,
                         &direction.x);
        track_scale(&direction.x, -index * OUTSIDE_STEP / njScalor(&direction));
        *slope_x = points[0].slope_x;
        *slope_z = points[0].slope_z;
        track_offset(&points[0].pos.x, &direction.x, &pos->x);
        outside = TRUE;
    } else {
        *pos = points[index].pos;
        *slope_x = points[index].slope_x;
        *slope_z = points[index].slope_z;
        outside = FALSE;
    }
    return outside;
}

/* move the place *segment and *fraction distance along path. the move is
 * forward where distance is above 0. set *pos to the new place. *slope_x and
 * *slope_z blend the slopes of the ends of the place's segment. *direction,
 * unless null, is the segment from its start to its end, not a unit vector.
 * return TRUE where the move read a point outside the track, else FALSE. a
 * path other than an open PATH_LAYOUT_SLOPES path changes nothing, and returns
 * FALSE */
static Bool track_advance(struct path *path, int *segment, Float *fraction,
                          Float distance, NJS_POINT3 *pos,
                          NJS_VECTOR *direction, Angle *slope_x, Angle *slope_z)
{
    Angle from_x;
    Angle from_z;
    Angle to_x;
    Angle to_z;
    NJS_POINT3 from;
    NJS_POINT3 to;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_VECTOR unused; /* NOLINT(clang-diagnostic-unused-variable) */
    int count;
    struct path_point *points;
    int index;
    int next;
    Float length;
    Float remaining;
    Float travel;
    Float along;
    Float before;
    Bool outside;

    travel = distance;
    outside = FALSE;
    if (path->flags == PATH_LAYOUT_SLOPES) {
        points = path->points;
        count = path->count;
        if (travel > 0.0F) {
            /* @hack spelling: forward_count, a local for count, where count
             * would do */
            int forward_count = count;

            index = *segment;
            next = index + 1;
            along = *fraction;
            for (;;) {
                if (track_point(points, forward_count, index, &from, &from_x,
                                &from_z)) {
                    outside = TRUE;
                }
                if (track_point(points, forward_count, next, &to, &to_x,
                                &to_z)) {
                    outside = TRUE;
                }
                length = njDistanceP2P(&from, &to);
                remaining = length * along;
                remaining = length - remaining;
                if (remaining < 0.0F) {
                    remaining = 0.0F;
                }
                if (travel > remaining) {
                    travel -= remaining;
                } else {
                    Angle roll;
                    Angle pitch;

                    before = (remaining - travel) / length;
                    along = 1.0F - before;
                    if (along < 0.0F) {
                        along = 0.0F;
                        before = 1.0F;
                    }
                    *segment = index;
                    *fraction = along;
                    pos->x = (from.x * before) + (to.x * along);
                    pos->y = (from.y * before) + (to.y * along);
                    pos->z = (from.z * before) + (to.z * along);
                    pitch = to_x - from_x;
                    roll = to_z - from_z;
                    SCALE_TURN(pitch, along);
                    SCALE_TURN(roll, along);
                    *slope_x = from_x + pitch;
                    *slope_z = from_z + roll;
                    if (direction != NULL) {
                        track_difference(&from.x, &to.x, &direction->x);
                    }
                    return outside;
                }
                index++;
                along = 0.0F;
                next++;
            }
        } else {
            index = *segment;
            next = index + 1;
            along = *fraction;
            for (;;) {
                if (track_point(points, count, index, &from, &from_x,
                                &from_z)) {
                    outside = TRUE;
                }
                if (track_point(points, count, next, &to, &to_x, &to_z)) {
                    outside = TRUE;
                }
                length = njDistanceP2P(&from, &to);
                remaining = length * along;
                before = length - remaining;
                if (before < 0.0F) {
                    remaining = length;
                }
                if (-travel > remaining) {
                    travel += remaining;
                } else {
                    Angle roll;
                    Angle pitch;

                    along = (remaining + travel) / length;
                    before = 1.0F - along;
                    if (before < 0.0F) {
                        along = 1.0F;
                        before = 0.0F;
                    }
                    *segment = index;
                    *fraction = along;
                    pos->x = (from.x * before) + (to.x * along);
                    pos->y = (from.y * before) + (to.y * along);
                    pos->z = (from.z * before) + (to.z * along);
                    pitch = to_x - from_x;
                    roll = to_z - from_z;
                    SCALE_TURN(pitch, along);
                    SCALE_TURN(roll, along);
                    *slope_x = from_x + pitch;
                    *slope_z = from_z + roll;
                    if (direction != NULL) {
                        track_difference(&from.x, &to.x, &direction->x);
                    }
                    return outside;
                }
                index--;
                along = 1.0F;
                next--;
            }
        }
    }
    return FALSE;
}

/* move task speed along its track, backward where speed is below 0. return
 * track_advance's result for the move. *direction, unless null, is the unit
 * vector from the task's place to the place LOOK_AHEAD on. the task's pos lies
 * halfway between the two. its ang turns STEER of the way to the track's
 * there. where speed is 0, it turns the whole way */
static Bool track_move(struct task *task, Float speed, NJS_VECTOR *direction)
{
    struct task_work *work = task->work;
    Angle near_x;
    Angle near_z;
    Angle far_x;
    Angle far_z;
    Angle roll;
    Angle pitch;
    Angle yaw;
    Float fraction;
    int segment;
    Bool outside;
    NJS_POINT3 near_point;
    NJS_POINT3 far_point;
    NJS_VECTOR near_direction;
    NJS_VECTOR far_direction;
    NJS_VECTOR ahead;
    struct angles orientation;

    outside = track_advance(TRACK(task), &DRILL(task)->segment,
                            &DRILL(task)->fraction, speed, &near_point,
                            &near_direction, &near_x, &near_z);
    segment = DRILL(task)->segment;
    fraction = DRILL(task)->fraction;
    track_advance(TRACK(task), &segment, &fraction, LOOK_AHEAD, &far_point,
                  &far_direction, &far_x, &far_z);
    track_difference(&near_point.x, &far_point.x, &ahead.x);
    if (direction != NULL) {
        *direction = ahead;
        njUnitVector(direction);
    }
    /* @hack spelling: ahead halved through a pointer a component at a time,
     * where track_scale(&ahead.x, 0.5F) would do */
    {
        Float *component = &ahead.x;
        *component++ *= 0.5F;
        *component++ *= 0.5F;
        *component *= 0.5F;
    }
    track_offset(&near_point.x, &ahead.x, &work->pos.x);
    pitch = far_x - near_x;
    roll = far_z - near_z;
    SCALE_TURN(pitch, 0.5F);
    SCALE_TURN(roll, 0.5F);
    orientation.x = near_x + pitch;
    orientation.z = near_z + roll;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateX(NULL, -orientation.x);
    njRotateZ(NULL, -orientation.z);
    njCalcVector(NULL, &ahead, &ahead);
    njPopMatrixEx();
    orientation.y = atan2f(ahead.x, ahead.z) * ANGLE_PER_RADIAN;
    if (speed == 0.0F) {
        work->ang = orientation;
    } else {
        pitch = orientation.x - work->ang.x;
        yaw = orientation.y - work->ang.y;
        roll = orientation.z - work->ang.z;
        SCALE_TURN(pitch, STEER);
        SCALE_TURN(yaw, STEER);
        SCALE_TURN(roll, STEER);
        work->ang.x += pitch;
        work->ang.y += yaw;
        work->ang.z += roll;
    }
    return outside;
}

/* drill_triggers, of drill_assets.h, lists the triggers through their tasks'
 * extra, newest first */
static void trigger_link(struct task *task)
{
    if (task->extra.p == NULL) {
        task->extra.p = drill_triggers;
        drill_triggers = task;
    }
}

static void trigger_unlink(struct task *task)
{
    struct task *trigger;
    struct task *previous;

    previous = NULL;
    for (trigger = drill_triggers; trigger != NULL;
         trigger = trigger->extra.p) {
        if (trigger == task) {
            if (trigger == drill_triggers) {
                drill_triggers = trigger->extra.p;
            } else {
                previous->extra.p = trigger->extra.p;
            }
            break;
        }
        previous = trigger;
    }
    task->extra.p = NULL;
}

static struct task *trigger_near(NJS_POINT3 *pos)
{
    struct task *trigger;

    for (trigger = drill_triggers; trigger != NULL;
         trigger = trigger->extra.p) {
        if (njDistanceP2P(&trigger->work->pos, pos) < trigger->work->scl.x) {
            return trigger;
        }
    }
    return NULL;
}

static void trigger_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR offset;

    /* @hack redundant: a block no path reaches, of the set editor's labels
     * and its preview of the sphere, whose strings SHC still places in this
     * source's constants */
    if (0) {
        (void)"<- TYPE[SPEED]";
        (void)"<- TYPE[COL]";
        (void)"<- COL SCL";
        (void)"<- START TIME";
        (void)"<- SET SPD";
        (void)"<- ANGSPD Y";
        offset.x = work->scl.x;
        offset.y = work->scl.y;
        offset.z = work->scl.z;
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njTranslateEx(&offset);
        njPopMatrixEx();
    }
}

static void trigger_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 *pos;

    /* @hack redundant: the code after the return, which no path reaches */
    if (1) {
        return;
    }
    pos = &work->pos;
    matrix_load_parent();
    pos->x = pos->y;
    pos->y = pos->z;
    work->mode = 0;
    matrix_load_parent();
}

static void trigger_destroy(struct task *task)
{
    trigger_unlink(task);
}

void cosmic_drill_trigger_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        trigger_link(task);
        task->step = trigger_step;
        task->draw = trigger_draw;
        task->destructor = trigger_destroy;
    }
}

void cosmic_drill_platform_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float radius;

    task->motion.p = syCalloc(1, sizeof(struct drill_motion));
    if (task->motion.p != NULL) {
        task->step = platform_step;
        task->draw = platform_draw;
        task->destructor = platform_destroy;
        DRILL(task)->fraction = 0.0F;
        DRILL(task)->segment = 0;
        track_move(task, 0.0F, NULL);
        DRILL(task)->collision = platform_collision;
        object = task->motion.p;
        object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
        object->ang[2] = 0;
        object->ang[0] = 0;
        object->ang[1] = work->ang.y;
        object->pos[0] = work->pos.x;
        object->pos[1] = work->pos.y;
        object->pos[2] = work->pos.z;
        flags = WORLD_MOVING | WORLD_SOLID;
        radius = object->model->r + CLASS_MARGIN;
        if (radius <= WORLD_SIZE_CLASS_1_MAX) {
            if (radius > WORLD_SIZE_CLASS_2_MAX) {
                flags = WORLD_MOVING | WORLD_SOLID | WORLD_SIZE_CLASS(1);
            } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
                flags = WORLD_MOVING | WORLD_SOLID | WORLD_SIZE_CLASS(2);
            } else {
                flags = WORLD_MOVING | WORLD_SOLID | WORLD_SIZE_CLASS(3);
            }
        }
        world_add(flags, task, object);
    }
}

static void platform_destroy(struct task *task)
{
    if (DRILL(task)->collision.model != NULL) {
        world_remove(task, &DRILL(task)->collision);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void platform_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float opening;

    if (!camera_near(&work->pos, DRAW_RANGE_SQUARED)) {
        return;
    }
    njSetTexture(&platform_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njTranslateEx((NJS_VECTOR *)platform_body.pos);
    if (DRILL(task)->flash < FLASH_LEAST) {
        njCnkEasyDrawModel(platform_body.model);
    } else {
        model_draw_tinted(
            platform_body.model,
            color_blend(COLOR_NONE, FLASH_RED, DRILL(task)->flash));
    }
    njFogDisable();
    njPushMatrixEx();
    njTranslateEx((NJS_VECTOR *)platform_light->pos);
    model_draw_tinted(
        platform_light->model,
        color_blend(drill_light_on, drill_light_off,
                    (njSin(game_state.steps * PULSE_RATE) + 1.0F) * 0.5F));
    njPopMatrixEx();
    njFogEnable();
    opening = DRILL(task)->timer.boarding * (1.0F / CLAMP_STEPS);
    if (opening > CLAMPS_OPEN) {
        opening = CLAMPS_OPEN;
    }
    if (opening > CLAMPS_RISEN) {
        opening -= CLAMPS_RISEN;
        njPushMatrixEx();
        njTranslate(NULL,
                    platform_right_clamp.pos[0] + (opening * CLAMP_SPREAD),
                    platform_right_clamp.pos[1] + CLAMP_RISE,
                    platform_right_clamp.pos[2]);
        njCnkEasyDrawModel(platform_right_clamp.model);
        njPopMatrixEx();
        njTranslate(NULL, platform_left_clamp.pos[0] - (opening * CLAMP_SPREAD),
                    platform_left_clamp.pos[1] + CLAMP_RISE,
                    platform_left_clamp.pos[2]);
        njCnkEasyDrawModel(platform_left_clamp.model);
    } else {
        njPushMatrixEx();
        njTranslate(NULL, platform_right_clamp.pos[0],
                    platform_right_clamp.pos[1] + (opening * CLAMP_RISE),
                    platform_right_clamp.pos[2]);
        njCnkEasyDrawModel(platform_right_clamp.model);
        njPopMatrixEx();
        njTranslate(NULL, platform_left_clamp.pos[0],
                    platform_left_clamp.pos[1] + (opening * CLAMP_RISE),
                    platform_left_clamp.pos[2]);
        njCnkEasyDrawModel(platform_left_clamp.model);
    }
    njPopMatrixEx();
}

/* set *seat to rider_seat as the platform's pos and ang place it */
/* @hack spelling: the seat's point as a macro whose body is a bare block,
 * where a function of its own would do */
#define PLATFORM_SEAT_POINT(work, seat)                                        \
    {                                                                          \
        njPushMatrixEx();                                                      \
        njUnitMatrix(NULL);                                                    \
        njTranslateEx(&(work)->pos);                                           \
        njRotateZ(NULL, (work)->ang.z);                                        \
        njRotateX(NULL, (work)->ang.x);                                        \
        njRotateY(NULL, (work)->ang.y);                                        \
        njCalcPoint(NULL, &rider_seat, (seat));                                \
        njPopMatrixEx();                                                       \
    }

static void platform_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *trigger;
    struct task *pursuer;
    Bool standing;
    int trigger_mode;
    Bool outside;
    Angle pitch;
    Angle yaw;
    Angle roll;
    /* @hack spelling: direction, a local for &tangent, where &tangent in
     * place would do */
    NJS_VECTOR *direction;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_VECTOR unused; /* NOLINT(clang-diagnostic-unused-variable) */
    NJS_POINT3 seat;
    NJS_VECTOR tangent;
    NJS_VECTOR down;
    Float gravity;

    if (game_state.cleared) {
        if (DRILL(task)->timer.boarding || (work->tick & RIDER_SEATED)) {
            player_request(0, PLAYER_REQUEST_RELEASE, 0);
            DRILL(task)->timer.boarding = 0;
            work->tick &= ~RIDER_SEATED;
        }
        return;
    }
    standing = player_on_task(0, task);
    /* @hack spelling: the test's TRUE and FALSE spelled out by a conditional,
     * where the test's own value would do */
    cosmic_drill_active[work->submode] = (work->tick & RIDER_SEATED) ||
                                                 work->mode == RIDE_FORWARD ||
                                                 work->mode == RIDE_REVERSE
                                             ? TRUE
                                             : FALSE;
    switch (work->mode) {
    case RIDE_START:
        if (!DRILL(task)->state.cooldown && standing) {
            work->mode = RIDE_FORWARD;
        }
        break;
    case RIDE_END:
        /* @hack redundant: a test of the cooldown and of player 0 standing on
         * the platform, with nothing under it */
        if (!DRILL(task)->state.cooldown && standing) {
        }
        break;
    case RIDE_FORWARD:
    case RIDE_REVERSE:
        trigger_mode = NO_TRIGGER;
        trigger = trigger_near(&work->pos);
        if (trigger != NULL) {
            if (trigger->work->ang.x & TRIGGER_SPEED) {
                trigger_mode = SPEED_TRIGGER;
            } else {
                trigger_mode = RELEASE_TRIGGER;
            }
        }
        if (!(work->tick & RIDER_SEATED)) {
            break;
        }
        direction = &tangent;
        outside = track_move(task,
                             work->mode == RIDE_FORWARD ? DRILL(task)->speed
                                                        : -DRILL(task)->speed,
                             direction);
        if (task->child == NULL && trigger_mode == RELEASE_TRIGGER) {
            pursuer = task_create_child(TASK_WORK, drill_step, task);
            if (pursuer != NULL) {
                pursuer->unk_3c.l = task->unk_3c.l;
                pursuer->work->mode = work->mode;
                if (drill_start(pursuer, DRILL(task)->segment,
                                DRILL(task)->fraction, DRILL(task)->speed)) {
                    pursuer->work->timer = (Uint32)trigger->work->scl.y;
                }
            }
        }
        down.x = 0.0F;
        down.y = -1.0F;
        down.z = 0.0F;
        gravity = njInnerProduct(&down, &tangent);
        gravity *= platform_slope_gain;
        DRILL(task)->speed += work->mode == RIDE_FORWARD ? gravity : -gravity;
        DRILL(task)->speed += platform_drive;
        DRILL(task)->speed *= platform_drag;
        if (trigger_mode == SPEED_TRIGGER) {
            DRILL(task)->speed =
                ((trigger->work->scl.z - DRILL(task)->speed) * SPEED_EASE) +
                DRILL(task)->speed;
        }
        if (DRILL(task)->speed < PLATFORM_SPEED_LEAST) {
            DRILL(task)->speed = PLATFORM_SPEED_LEAST;
        }
        if (DRILL(task)->speed > PLATFORM_SPEED_MOST) {
            DRILL(task)->speed = PLATFORM_SPEED_MOST;
        }
        sound_play_at_for(RIDE_SOUND, task, RIDE_SOUND_PRIORITY,
                          RIDE_SOUND_VOLUME, RIDE_SOUND_STEPS, &work->pos);
        if ((work->mode == RIDE_FORWARD && DRILL(task)->segment > 1) ||
            (work->mode == RIDE_REVERSE && DRILL(task)->segment <= 0)) {
            if (outside) {
                sound_play(END_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
                if (work->mode == RIDE_FORWARD) {
                    work->mode = RIDE_END;
                } else {
                    work->mode = RIDE_START;
                }
            }
        }
        break;
    }
    if (work->tick & RIDER_SEATED) {
        switch (work->mode) {
        case RIDE_START:
        case RIDE_END:
            DRILL(task)->timer.boarding--;
            if (DRILL(task)->timer.boarding < 0 ||
                (work->submode == COSMIC_DRILL_SECOND_RIDE &&
                 work->mode == RIDE_END)) {
                DRILL(task)->timer.boarding = 0;
                player_request(0, PLAYER_REQUEST_RELEASE, 0);
                work->tick &= ~RIDER_SEATED;
                DRILL(task)->state.cooldown = BOARD_COOLDOWN;
                player_records[0]->spd.x = 0.0F;
            }
            break;
        default:
            PLATFORM_SEAT_POINT(work, &seat);
            if (player_work[0]->mode == PLAYER_MODE_HURT) {
                player_request(0, PLAYER_REQUEST_ANIM, ANIM_STAND);
                DRILL(task)->flash = 1.0F;
            }
            player_set_pos(0, seat.x, seat.y, seat.z);
            player_set_angles(0, work->ang.x, work->ang.y, work->ang.z);
            player_records[0]->spd.x = DRILL(task)->speed;
            break;
        }
    } else {
        switch (work->mode) {
        case RIDE_FORWARD:
        case RIDE_REVERSE:
            if (!DRILL(task)->timer.boarding) {
                sound_play(BOARD_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
                player_request(0, PLAYER_REQUEST_ANIM, ANIM_STAND);
                vibration_start(0, BOARD_POWER, BOARD_FREQ, VIBRATION_UNTIMED);
            }
            if (DRILL(task)->timer.boarding < BOARD_STEPS) {
                if (DRILL(task)->timer.boarding == CLAMP_SOUND_STEP) {
                    sound_play(CLAMP_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
                    vibration_start(0, CLAMP_POWER, CLAMP_FREQ, CLAMP_TIME);
                }
                PLATFORM_SEAT_POINT(work, &seat);
                /* @bug logic: each step moves player 0 to the seat plus
                 * BOARD_EASE of the seat's offset from player 0. the angles'
                 * ease adds its share to player 0's own angles. so player 0
                 * lands BOARD_EASE of its distance past the seat, nearly on it
                 * from the first step. the stores at 0x8c501d32, 0x8c501d4a
                 * and 0x8c501d66 show it */
                seat.x =
                    ((seat.x - player_work[0]->pos.x) * BOARD_EASE) + seat.x;
                seat.y =
                    ((seat.y - player_work[0]->pos.y) * BOARD_EASE) + seat.y;
                seat.z =
                    ((seat.z - player_work[0]->pos.z) * BOARD_EASE) + seat.z;
                /* player_set_angles gives a seated player 0 the ang.y
                 * QUARTER_TURN less the platform's. the yaw eases toward it */
                pitch = work->ang.x - player_work[0]->ang.x;
                yaw = -work->ang.y - player_work[0]->ang.y + QUARTER_TURN;
                roll = work->ang.z - player_work[0]->ang.z;
                SCALE_TURN(pitch, BOARD_EASE);
                SCALE_TURN(yaw, BOARD_EASE);
                SCALE_TURN(roll, BOARD_EASE);
                pitch += player_work[0]->ang.x;
                yaw += player_work[0]->ang.y;
                roll += player_work[0]->ang.z;
                player_work[0]->ang.x = pitch;
                player_work[0]->ang.y = yaw;
                player_motion[0]->aim_y = yaw;
                player_work[0]->ang.z = roll;
                if (player_work[0]->mode == PLAYER_MODE_HURT) {
                    player_request(0, PLAYER_REQUEST_ANIM, ANIM_STAND);
                }
                player_records[0]->spd.x = DRILL(task)->speed;
                player_set_pos(0, seat.x, seat.y, seat.z);
                DRILL(task)->timer.boarding++;
            } else {
                sound_play(SEATED_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
                work->tick |= RIDER_SEATED;
            }
            break;
        case RIDE_FINISHED:
            break;
        case RIDE_END:
            if (work->submode == COSMIC_DRILL_SECOND_RIDE && standing) {
                player_request(0, PLAYER_REQUEST_RELEASE, 0);
                /* @hack redundant: launch_velocity turned by the platform,
                 * and then launch_velocity as a unit vector, neither of which
                 * anything reads */
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njRotateZ(NULL, work->ang.z);
                njRotateX(NULL, work->ang.x);
                njRotateY(NULL, work->ang.y);
                njCalcVector(NULL, &launch_velocity, &seat);
                njPopMatrixEx();
                seat = launch_velocity;
                gravity = njScalor(&seat);
                track_scale(&seat.x, 1.0F / gravity);
                /* @hack spelling: tangent reused for the throw's start,
                 * where a local of its own would do */
                tangent = player_work[0]->pos;
                tangent.y += THROW_LIFT;
                egg_jump_create(0, &tangent, &launch_target);
                work->mode = RIDE_FINISHED;
                vibration_start_fading(0, THROW_POWER, THROW_FREQ,
                                       THROW_LENGTH);
            }
        }
    }
    if (standing) {
        DRILL(task)->state.cooldown = BOARD_COOLDOWN;
    } else if (DRILL(task)->state.cooldown) {
        DRILL(task)->state.cooldown--;
    }
    DRILL(task)->flash *= FLASH_FADE;
    if (DRILL(task)->collision.model != NULL) {
        DRILL(task)->collision.pos[0] = work->pos.x;
        DRILL(task)->collision.pos[1] = work->pos.y;
        DRILL(task)->collision.pos[2] = work->pos.z;
        DRILL(task)->collision.ang[0] = work->ang.x;
        DRILL(task)->collision.ang[1] = work->ang.y;
        DRILL(task)->collision.ang[2] = work->ang.z;
        if (player_near((NJS_POINT3 *)DRILL(task)->collision.pos,
                        COLLIDE_RANGE)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

/* set *point to tooth, scaled to the length from the head's axis to a
 * hexagon about it. the length is the one at tooth's angle about z once the
 * head turns by rotation. the hexagon doesn't turn with the head, so a tooth
 * reaches in and out as the head turns */
static void drill_radial_point(Angle rotation, NJS_VECTOR *tooth,
                               NJS_VECTOR *point)
{
    Float reach;
    Float factor;
    Angle side;

    side = (Angle)(atan2f(tooth->x, tooth->y) * ANGLE_PER_RADIAN) + rotation;
    /* the angle from the middle of the side the tooth faces */
    side = ((Uint16)side % SIXTH_TURN) - TWELFTH_TURN;
    reach = HEAD_APOTHEM / njCos(side);
    factor = reach / njScalor(tooth);
    point->x = tooth->x * factor;
    point->y = tooth->y * factor;
    point->z = tooth->z * factor;
}

static Bool drill_start(struct task *task, int segment, Float fraction,
                        Float speed)
{
    task->motion.p = syCalloc(1, sizeof(struct drill_motion));
    if (task->motion.p == NULL) {
        /* @bug memory: where syCalloc fails, drill_step still runs the drill
         * through its null record. platform_step made the task with
         * drill_step, and the return at 0x8c502100 after the call at
         * 0x8c5020f4 leaves it there. drill_step reads the record at
         * 0x8c502646, and writes it at 0x8c50264e */
#ifdef NONMATCHING
        /* this side repairs the tag above: it destroys the drill, so no step
         * runs it without its record */
        task_destroy(task);
#endif
        return FALSE;
    }
    task->step = drill_step;
    task->draw = drill_draw;
    task->destructor = drill_destroy;
    DRILL(task)->fraction = fraction;
    DRILL(task)->segment = segment;
    DRILL(task)->speed = speed;
    collision_init(task, &drill_shape, 1, COLLISION_OBJECT);
    track_move(task, 0.0F, NULL);
    return TRUE;
}

static void drill_destroy(struct task *owner)
{
    /* @hack spelling: task, a local for owner, where owner itself would do */
    struct task *task = owner;

    syFree(task->motion.p);
    task->motion.p = NULL;
}

#pragma inline(drill_set_light)
static void drill_set_light(struct light *light)
{
    njSetDirectLightIntensity(light->intensity, light->ambient);
    njCnkSetEasyLightIntensity(light->intensity, light->ambient);
    njCnkSetSimpleLightIntensity(light->intensity, light->ambient);
    njCnkSetEasyLightColor(light->r, light->g, light->b);
    njSetDirectLightColor(light->r, light->g, light->b);
    njCnkSetSimpleLightColor(light->r, light->g, light->b);
}

static void drill_draw(struct task *task)
{
    struct task_work *work = task->work;
    Angle rotation;
    int i;

    if (!camera_near(&work->pos, DRAW_RANGE_SQUARED)) {
        return;
    }
    /* a drill shows once its work's timer has run down */
    if (work->timer) {
        return;
    }
    light_apply(DRILL_LIGHT);
    if (DRILL(task)->flash > FLASH_LEAST) {
        struct light light;

        light = lights[DRILL_LIGHT];
        light.g *= 1.0F - (DRILL(task)->flash * FLASH_SHADE);
        light.b *= 1.0F - (DRILL(task)->flash * FLASH_SHADE);
        drill_set_light(&light);
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&drill_body_texlist);
    njTranslateEx((NJS_VECTOR *)drill_body->pos);
    njCnkEasyDrawModel(drill_body->model);
    /* @hack spelling: rotation set inside the call, where a statement of
     * its own would do */
    njRotateZ(NULL, rotation = game_state.steps * DRILL_SPIN);
    njPushMatrixEx();
    njSetTexture(&drill_head_texlist);
    njTranslateEx((NJS_VECTOR *)drill_head->pos);
    njCnkSimpleDrawModel(drill_head->model);
    matrix_load_parent();
    for (i = 0; i < DRILL_TEETH; i++) {
        NJS_VECTOR point;
        if (!(work->submode & TOOTH_BIT(i))) {
            drill_radial_point(rotation, (NJS_VECTOR *)drill_teeth[i]->pos,
                               &point);
            njTranslateEx(&point);
            njCnkSimpleDrawModel(drill_teeth[i]->model);
            matrix_load_parent();
        }
    }
    njPopMatrix(2);
    light_apply(light_after_draw);
}

static void drill_step(struct task *task)
{
    /* @hack redundant: reserved, a local that only sizes the frame */
    Uint8 reserved[12]; /* NOLINT(clang-diagnostic-unused-variable) */
    NJS_VECTOR point;
    NJS_VECTOR direction;
    NJS_VECTOR velocity;
    NJS_VECTOR impact;
    struct angles orientation;
    struct task_work *work = task->work;
    struct task *target;
    struct task *trigger;
    Angle rotation;
    int i;
    int previous_teeth;
    int trigger_mode;
    Bool outside;
    Float gravity;

    /* @hack type: game_state's cleared read as game_state_cleared, where
     * game_state.cleared would do */
    if (game_state_cleared) {
        return;
    }
    /* the work's timer counts the steps before the drill shows and moves.
     * at its end, the drill makes its teeth */
    if (work->timer) {
        work->timer--;
        if (!work->timer) {
            njPushMatrixEx();
            /* @hack spelling: 0, where NULL would do */
            njUnitMatrix(0);
            njTranslateEx(&work->pos);
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, work->ang.y);
            njTranslateEx((NJS_VECTOR *)drill_body->pos);
            njPushMatrixEx();
            /* @bug uninitialized: each tooth starts at a point turned by
             * rotation, which this path never sets. the call at 0x8c502552
             * takes the stack pointer as the angle, by the MOV R15,R4 in its
             * slot */
#ifdef NONMATCHING
            /* this side repairs the tag above: the teeth start at the angle
             * drill_draw gives them */
            rotation = game_state.steps * DRILL_SPIN;
#endif
            for (i = 0; i < DRILL_TEETH; i++) {
                /* NOLINTNEXTLINE(clang-diagnostic-uninitialized) */
                drill_radial_point(rotation, (NJS_VECTOR *)drill_teeth[i]->pos,
                                   &point);
                njCalcPoint(NULL, &point, &point);
                tooth_create(task, i, &point, tooth_shape, 1);
                matrix_load_parent();
            }
            njPopMatrix(2);
        }
        return;
    }
    previous_teeth = DRILL(task)->state.broken_teeth;
    DRILL(task)->state.broken_teeth = 0;
    for (i = 0; i < DRILL_TEETH; i++) {
        if (work->submode & TOOTH_BIT(i)) {
            DRILL(task)->state.broken_teeth++;
        }
    }
    if (task->child != NULL) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, work->ang.y);
        /* @hack spelling: rotation set inside the call, where a statement of
         * its own would do */
        njRotateZ(NULL, rotation = game_state.steps * DRILL_SPIN);
        if (DRILL(task)->speed >= SOUND_SPEED) {
            point.x = HEAD_X;
            point.z = HEAD_Z;
            point.y = -HEAD_Y;
            /* @hack spelling: direction reused for the high sound's point,
             * where a local of its own would do */
            direction.x = HEAD_X;
            direction.z = HEAD_Z;
            direction.y = HEAD_Y;
            njCalcPoint(NULL, &point, &point);
            njCalcPoint(NULL, &direction, &direction);
            sound_play_at_for(LOW_HEAD_SOUND, task, HEAD_SOUND_PRIORITY,
                              HEAD_SOUND_VOLUME, HEAD_SOUND_STEPS, &point);
            sound_play_at_for(HIGH_HEAD_SOUND, work, HEAD_SOUND_PRIORITY,
                              HEAD_SOUND_VOLUME, HEAD_SOUND_STEPS, &direction);
        }
        target = task->child;
        do {
            drill_radial_point(
                rotation, (NJS_VECTOR *)drill_teeth[target->work->tick]->pos,
                &point);
            track_scale(&point.x, TOOTH_REACH / njScalor(&point));
            point.z = TOOTH_Z;
            njCalcPoint(NULL, &point, &point);
            target->work->pos = point;
            if (DRILL(task)->timer.stun) {
                target->work->submode = TOOTH_STUNNED;
            } else {
                target->work->submode = 0;
            }
            target = target->next;
        } while (target != task->child);
        if (DRILL(task)->timer.stun > STUN_FLASH) {
            DRILL(task)->flash = 1.0F;
        }
        if (work->submode != work->tick) {
            sound_play(TOOTH_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
            /* @hack redundant: a block no path reaches, which compares
             * constants the break takes, in the order SHC's pool keeps them */
            if (0) {
                if ((DEBRIS_LIGHT | DEBRIS_TRAIL) == (Uint32)task) {
                }
                if (SPARK_SIZE == point.x) {
                }
                if (SHARD_Z == point.x) {
                }
                if (SPARK_Z == point.x) {
                }
                if (SPARK_Z == point.x) {
                }
                if (SHARD_Y == point.x) {
                }
            }
            for (i = 0; i < DRILL_TEETH; i++) {
                if ((work->submode & TOOTH_BIT(i)) !=
                    (work->tick & TOOTH_BIT(i))) {
                    velocity.x = SHARD_X;
                    velocity.y = SHARD_Y;
                    velocity.z = DRILL(task)->speed + SHARD_Z;
                    drill_radial_point(
                        rotation, (NJS_VECTOR *)drill_teeth[i]->pos, &point);
                    impact = point;
                    impact.z = SPARK_Z;
                    njCalcPoint(NULL, &point, &point);
                    njCalcPoint(NULL, &impact, &impact);
                    njCalcVector(NULL, &velocity, &velocity);
                    current_matrix_angles_xyz(&orientation.x, &orientation.y,
                                              &orientation.z);
                    flash_manager_create_sparks_depth(
                        impact.x, impact.y, impact.z,
                        SPARK_SIZE + (rand() * (SPARK_SIZE_RANGE * RAND_SCALE)),
                        SPARKS, SPARK_DEPTH_BIAS);
                    debris_launch_flags(&point, &orientation, PIECE_FLOOR,
                                        &velocity, drill_teeth[i]->model,
                                        &platform_texlist,
                                        DEBRIS_LIGHT | DEBRIS_TRAIL);
                }
            }
        }
        njPopMatrixEx();
    }
    trigger_mode = NO_TRIGGER;
    trigger = trigger_near(&work->pos);
    if (trigger != NULL) {
        if (trigger->work->ang.x & TRIGGER_SPEED) {
            trigger_mode = SPEED_TRIGGER;
        } else {
            trigger_mode = RELEASE_TRIGGER;
        }
    }
    if (work->mode == RIDE_FINISHED) {
        DRILL(task)->speed = FINISHED_SPEED;
    }
    outside = track_move(task,
                         work->mode == RIDE_FORWARD ? DRILL(task)->speed
                                                    : -DRILL(task)->speed,
                         &direction);
    /* @hack spelling: velocity reused for the down vector, where a local of
     * its own would do */
    velocity.x = 0.0F;
    velocity.y = -1.0F;
    velocity.z = 0.0F;
    gravity = njInnerProduct(&velocity, &direction) * platform_slope_gain;
    DRILL(task)->speed += work->mode == RIDE_FORWARD ? gravity : -gravity;
    if (DRILL(task)->state.broken_teeth >= BROKEN_TEETH) {
        DRILL(task)->speed += platform_drive;
        DRILL(task)->speed *= platform_drag;
    } else {
        DRILL(task)->speed += drill_drive;
        DRILL(task)->speed *= drill_drag;
    }
    if (player_near(&work->pos, NEAR_RANGE)) {
        DRILL(task)->speed *= drill_near_drag;
    }
    if (trigger_mode == SPEED_TRIGGER) {
        DRILL(task)->speed =
            ((trigger->work->scl.z - DRILL(task)->speed) * SPEED_EASE) +
            DRILL(task)->speed;
    }
    /* @bug logic: a lone touch in a step no tooth broke stuns the drill for
     * STUN_TOOTH, not STUN_TOUCH. the scan at 0x8c502ec8 that picks the stun
     * comes right after the scan at 0x8c502ebe that found the touch, so it
     * goes on past that hit. STUN_TOUCH needs a second touch, or a touch in a
     * step a tooth broke */
    if (previous_teeth != DRILL(task)->state.broken_teeth ||
        collision_next_player(task)) {
        if (collision_next_player(task)) {
            DRILL(task)->timer.stun += STUN_TOUCH;
        } else {
            DRILL(task)->timer.stun += STUN_TOOTH;
        }
        DRILL(task)->speed *= STUN_SLOW;
    }
    if (DRILL(task)->timer.stun) {
        DRILL(task)->timer.stun--;
        if (DRILL(task)->speed > STUNNED_SLOWING) {
            DRILL(task)->speed *= STUN_SLOW;
        }
        if (DRILL(task)->speed < STUNNED_SPEED_LEAST) {
            DRILL(task)->speed = STUNNED_SPEED_LEAST;
        }
    } else if (DRILL(task)->state.broken_teeth >= BROKEN_TEETH) {
        if (DRILL(task)->speed < BROKEN_SPEED_LEAST) {
            DRILL(task)->speed = BROKEN_SPEED_LEAST;
        }
        if (DRILL(task)->speed > BROKEN_SPEED_MOST) {
            DRILL(task)->speed = BROKEN_SPEED_MOST;
        }
    } else {
        if (DRILL(task)->speed < DRILL_SPEED_LEAST) {
            DRILL(task)->speed = DRILL_SPEED_LEAST;
        }
        if (DRILL(task)->speed > DRILL_SPEED_MOST) {
            DRILL(task)->speed = DRILL_SPEED_MOST;
        }
    }
    work->tick = work->submode;
    if (((work->mode == RIDE_FORWARD && DRILL(task)->segment > 1) ||
         (work->mode == RIDE_REVERSE && DRILL(task)->segment <= 0)) &&
        outside) {
        work->mode = RIDE_FINISHED;
    } else {
        collision_register(task);
    }
    DRILL(task)->flash *= FLASH_FADE;
}

static struct task *tooth_create(struct task *parent, int index,
                                 NJS_POINT3 *pos, struct collision_shape *shape,
                                 Uint32 count)
{
    struct task *task;

    if ((task = task_create_child(TASK_WORK, tooth_step, parent)) != NULL) {
        struct task_work *work = task->work;

        work->pos = *pos;
        /* @hack spelling: task->work, where work would do */
        task->work->tick = index;
        task->step = tooth_step;
        task->destructor = tooth_destroy;
        if (shape != NULL && count) {
            collision_init(task, shape, count, COLLISION_ITEM);
        }
        enemy_work_alloc(task);
    }
    /* @hack redundant: a test of task with nothing under it, which no path
     * reaches */
    if (0) {
        if (task != NULL) {
        }
    }
    return task;
}

static void tooth_step(struct task *task)
{
    NJS_VECTOR velocity;
    NJS_POINT3 pos; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    struct task_work *work = task->work;
    struct task *drill = task->parent;
    struct task_work *drill_work = drill->work;

    if (work->flags & WORK_HURT) {
        /* @hack redundant: velocity and pos, which nothing reads */
        velocity.x = 0.0F;
        velocity.y = 0.0F;
        velocity.z = 0.0F;
        pos = work->pos;
        /* a tooth breaks only while the drill isn't stunned. it breaks only
         * where no other tooth has broken since the drill's last step.
         * otherwise the tooth drops the hurt */
        if (drill_work->submode == drill_work->tick &&
            !DRILL(drill)->timer.stun) {
            drill_work->submode |= TOOTH_BIT(work->tick);
            if (task->placement != NULL) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
        } else {
            work->flags &= ~WORK_HURT;
        }
    } else if (!DRILL(drill)->timer.stun) {
        collision_register(task);
    }
}

static void tooth_destroy(struct task *task)
{
    enemy_destroy(task);
}
