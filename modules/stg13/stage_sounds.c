/* City Escape's sound objects, SE_PATCAR and SE_KAZE in its object list: a
 * patrol car's sound and a wind */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "object/set.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stg13/stage_sounds.h"
#include "system/task.h"
#include "system/util.h"

#define PATROL_CAR_SOUND 12
#define WIND_SOUND 16
#define PATROL_CAR_PRIORITY 1
/* each step plays the car's sound for PATROL_CAR_DURATION + 1 sound
 * updates */
#define PATROL_CAR_DURATION 10

/* the camera's speed away from the car is the step's change in DISTANCE,
 * held within doppler_speed either way. the sound's pitch is that speed over
 * doppler_speed, times -PITCH_RANGE. so the pitch falls as the camera draws
 * away, and stays within PITCH_MAX either way */
#define PITCH_RANGE 16384.0F
#define PITCH_MAX 16383
/* the wind plays within WIND_SPREAD / 2 of player 0's pos along each axis */
#define WIND_SPREAD 530.0F

/* the camera's distance from the car this step and the last, in its work's
 * scl.x and scl.y */
#define DISTANCE(work) ((work)->scl.x)
#define LAST_DISTANCE(work) ((work)->scl.y)

static Sint32 patrol_car_volume = 0;
/* the speed toward or away from the camera, in a step, at which the car's
 * pitch bends furthest */
static Float doppler_speed = 6.5F;
static Float wind_chance = 0.03F; /* the chance of a wind at each step */
static Sint32 wind_volume = 0;

static void patrol_car_sound_destroy(struct task *task);
static void patrol_car_sound_step(struct task *task);
static void patrol_car_sound_draw(struct task *task);
static void wind_sound_destroy(struct task *task);
static void wind_sound_step(struct task *task);
static void wind_sound_draw(struct task *task);

void patrol_car_sound_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = patrol_car_sound_draw;
        task->step = patrol_car_sound_step;
        task->destructor = patrol_car_sound_destroy;
        work->mode = 0;
    }
}

static void patrol_car_sound_destroy(struct task *task)
{
}

static void patrol_car_sound_step(struct task *task)
{
    struct task_work *work = task->work;
    Float receding;
    int pitch;

    if (placement_try_leave(task)) {
        return;
    }
    sound_play_at_for(PATROL_CAR_SOUND, work, PATROL_CAR_PRIORITY,
                      patrol_car_volume, PATROL_CAR_DURATION, &work->pos);
    LAST_DISTANCE(work) = DISTANCE(work);
    DISTANCE(work) = njDistanceP2P(&camera_views[0]->pos, &work->pos);
    receding = DISTANCE(work) - LAST_DISTANCE(work);
    if (receding >= doppler_speed) {
        receding = doppler_speed;
    }
    if (receding <= -doppler_speed) {
        receding = -doppler_speed;
    }
    /* @hack spelling: the pitch worked out in three statements, where one
     * expression would do */
    receding /= doppler_speed;
    receding = -receding;
    receding *= PITCH_RANGE;
    pitch = receding;
    if (pitch > PITCH_MAX) {
        pitch = PITCH_MAX;
    }
    if (pitch < -PITCH_MAX) {
        pitch = -PITCH_MAX;
    }
    sound_set_pitch(work, SOUND_ANY_ID, pitch);
}

static void patrol_car_sound_draw(struct task *task)
{
    /* @hack type: work, a volatile local nothing reads, where an empty body
     * would do */
    struct task_work *volatile work = task->work;
    /* @hack redundant: unused, a local that only sizes the frame */
    Uint8 unused[24];
}

void wind_sound_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = wind_sound_draw;
        task->step = wind_sound_step;
        task->destructor = wind_sound_destroy;
        work->mode = 0;
    }
}

static void wind_sound_destroy(struct task *task)
{
}

static void wind_sound_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 local;
    NJS_POINT3 pos;
    /* @hack spelling: spread, scale and offset, locals for constants, where
     * the constants in place would do */
    Float spread;
    Float scale;
    /* @hack redundant: offset in a register, where a plain local would do */
    register Float offset;

    /* @hack spelling: an else after the return, where the rest of the step
     * after the if would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        if (player_work[0] != NULL && rand() * RAND_SCALE < wind_chance) {
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, -work->ang.y);
            njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
            njCalcPoint(NULL, &player_work[0]->pos, &local);
            njPopMatrixEx();
            if (njAbs(local.x) < work->scl.x && njAbs(local.z) < work->scl.z &&
                njAbs(local.y) < work->scl.y) {
                pos = player_work[0]->pos;
                spread = WIND_SPREAD;
                scale = RAND_SCALE;
                offset = -0.5F;
                pos.x += spread * ((rand() * scale) + offset);
                pos.y += spread * ((rand() * scale) + offset);
                pos.z += spread * ((rand() * scale) + offset);
                sound_play_at(WIND_SOUND, NULL, 0, wind_volume, &pos);
            }
        }
    }
}

static void wind_sound_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused[3];

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants, and for the registers SHC
     * gives the draw. the image holds none of its code, so its calls are
     * stand-ins */
    if (0) {
        (void)"<- ROT Y";
        (void)"<- SCL X";
        (void)"<- SCL Y";
        (void)"<- SCL Z";
        njPushMatrixEx();
        njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    }
}
