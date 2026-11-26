/* the stages' sound objects: a siren, SE_KAZE's wind and SE_TOPPU's gust. Big
 * Foot, Mission Street and Mission Street's two-player stage hold them */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/stage_sounds.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

#define SIREN_SOUND 7
#define WIND_SOUND 15
#define GUST_SOUND 16
#define PRIORITY 1
/* each step plays the siren for SIREN_DURATION + 1 sound updates, and the
 * gust for GUST_DURATION + 1 */
#define SIREN_DURATION 10
#define GUST_DURATION 120

/* the camera's speed away from the siren is the step's change in DISTANCE,
 * held within doppler_speed either way. the siren's pitch is that speed over
 * doppler_speed, times -PITCH_RANGE, so it falls as the camera draws away.
 * the pitch then stays within PITCH_MAX either way */
#define PITCH_RANGE 16384.0F
#define PITCH_MAX 16383
/* the wind plays within WIND_SPREAD / 2 of player 0's pos along each axis */
#define WIND_SPREAD 530.0F

/* the camera's distance from the siren this step and the last, in its
 * work's scl.x and scl.y */
#define DISTANCE(work) ((work)->scl.x)
#define LAST_DISTANCE(work) ((work)->scl.y)

static Sint32 siren_volume = 0;
/* the speed toward or away from the camera, in a step, at which the siren's
 * pitch bends furthest */
static Float doppler_speed = 6.5F;
static Float wind_chance = 0.03F; /* the chance of a wind at each step */
static Sint32 wind_volume = 0;
static Sint32 gust_volume = 0;

static void siren_destroy(struct task *task);
static void siren_step(struct task *task);
static void siren_draw(struct task *task);
static void wind_sound_destroy(struct task *task);
static void wind_sound_step(struct task *task);
static void wind_sound_draw(struct task *task);
static void gust_sound_destroy(struct task *task);
static void gust_sound_step(struct task *task);
static void gust_sound_draw(struct task *task);

void siren_init(struct task *task)
{
    struct task_work *work = task->work;

    if (work->submode || !placement_try_leave(task)) {
        task->draw = siren_draw;
        task->step = siren_step;
        task->destructor = siren_destroy;
        work->mode = 0;
    }
}

static void siren_destroy(struct task *task)
{
}

static void siren_step(struct task *task)
{
    struct task_work *work = task->work;
    Float receding;
    int pitch;

    if (work->submode || !placement_try_leave(task)) {
        sound_play_at_for(SIREN_SOUND, work, PRIORITY, siren_volume,
                          SIREN_DURATION, &work->pos);
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
}

static void siren_draw(struct task *task)
{
    /* @hack redundant: seven pointers, of which one store fills the first
     * and nothing reads any */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    struct task_work *unused[7];

    unused[0] = task->work;
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

    /* @hack spelling: the return in an arm of its own, where a test of the
     * call's result around the rest would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        if (player_work[0] != NULL && (rand() * RAND_SCALE) < wind_chance) {
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
    /* @hack redundant: points nothing writes or reads */
    NJS_POINT3 unused[3]; /* NOLINT(clang-diagnostic-unused-variable) */

    /* @hack redundant: a block no path reaches, kept for its four strings,
     * which SHC still places in this source's constants. they name the
     * fields that give the wind's box. the block's calls only shape the code
     * around the block: the image holds none of the block's code */
    if (0) {
        (void)"<- ROT Y";
        (void)"<- SCL X";
        (void)"<- SCL Y";
        (void)"<- SCL Z";
        njPushMatrixEx();
        njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    }
}

void gust_sound_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = gust_sound_draw;
        task->step = gust_sound_step;
        task->destructor = gust_sound_destroy;
        work->mode = 0;
    }
}

static void gust_sound_destroy(struct task *task)
{
}

static void gust_sound_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: the return in an arm of its own, where a test of the
     * call's result around the rest would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        sound_play_at_for(GUST_SOUND, task, PRIORITY, gust_volume,
                          GUST_DURATION, &work->pos);
    }
}

static void gust_sound_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: points nothing writes or reads */
    NJS_POINT3 first;
    NJS_POINT3 second;

    /* @hack redundant: calls no path makes */
    if (0) {
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njScale(NULL, 1.0F, 1.0F, 1.0F);
        njCalcPoint(NULL, &first, &second);
        njPopMatrixEx();
    }
}
