#include <ninja.h>
#include <sg_xpt.h>

#include "boss_bigbogy/door.h"
#include "boss_bigbogy/door_art.h"
#include "camera/camera.h"
#include "collision/collision.h"
#include "object/set.h"
#include "shared/sandglass.h"
#include "shared/sandglass_registry.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* a door's modes, in its work's mode, which index mode_steps. its submode
 * holds the mode its next step takes, or NO_NEXT_MODE */
enum sandglass_door_mode {
    MODE_SHUT,
    MODE_OPENING,
    MODE_BOUNCING,
    MODE_CLOSING
};
#define NO_NEXT_MODE (-1)

#define WALL_DOOR(work) (!(work)->ang.x)
/* where the door stands shut, in its motion record's acc */
#define DOOR_HOME(task) (TASK_MOTION_OF(task)->acc)
/* how far the door lies open, in its work's scl.x */
#define TRAVEL(work) ((work)->scl.x)

/* the door opens by OPEN_TRAVEL over the first OPEN_STEPS steps of its
 * sandglass's run. its travel follows 1 - cos of OPEN_TURN a step, a quarter
 * turn in all */
#define OPEN_TRAVEL 48.0F
#define OPEN_STEPS 12
#define OPEN_TURN (QUARTER_TURN / OPEN_STEPS)
/* the open door bounces. each step its travel swings to the other side by
 * BOUNCE(work), which then keeps BOUNCE_DECAY of itself. the swing starts at
 * BOUNCE_START. the bounce ends BOUNCE_SHARE of the sandglass's run after
 * the door opened */
#define BOUNCE(work) ((work)->scl.y)
#define BOUNCE_START 0.2F
#define BOUNCE_DECAY 0.9F
#define BOUNCE_SHARE 0.0667F
/* the door shuts over the rest of the sandglass's run, in NOTCHES notches.
 * each lowers it by NOTCH_DEPTH, along a quarter turn of 1 - cos */
#define NOTCHES 16
#define NOTCH_DEPTH (OPEN_TRAVEL / NOTCHES)

/* the doors' sounds, by their ids in bigbogy_sounds. a door keys its sounds
 * by its step, door_step, so every door's sounds share one key */
#define DOOR_SOUND_KEY(task) ((const void *)(Uint32)(task)->step)
#define OPEN_SOUND 6
#define OPEN_SOUND_PRIORITY 1
#define OPEN_SOUND_DURATION 120
#define LOWER_SOUND 7
#define LOWER_SOUND_PRIORITY 1
#define LOWER_SOUND_DURATION 10
#define SHUT_SOUND 8
#define SHUT_SOUND_PRIORITY 0

/* clang-format off */
static struct collision_shape wall_shape = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000,
    {0.0F, 25.0F, 5.0F}, 25.0F, 25.0F, 5.0F, 0, {0, 0, 0}};
static struct collision_shape floor_shape = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000,
    {0.0F, -5.0F, 0.0F}, 25.0F, 5.0F, 25.0F, 0, {0, 0, 0}};
/* clang-format on */

/* each sound's volume. a sound plays at its near volume where the camera is
 * level with the door. its volume moves evenly with the camera's height
 * above or below the door. it reaches its far volume at its range, and goes
 * on past it */
static Sint32 open_near = 125;
static Sint32 open_far = 100;
static Float open_range = 200.0F;
static Sint32 lower_near = 120;
static Sint32 lower_far = 75;
static Float lower_range = 200.0F;
static Sint32 shut_near = 120;
static Sint32 shut_far = 75;
static Float shut_range = 200.0F;

static void door_destroy(struct task *task);
static void door_step(struct task *task);
static void door_draw(struct task *task);
static void take_mode(struct task *task);
static void door_shut(struct task *task);
static void door_opening(struct task *task);
static void door_bouncing(struct task *task);
static void door_closing(struct task *task);
static struct task *door_glass(int id);
static void play_open(struct task *task);
static void play_lower(struct task *task);
static void play_shut(struct task *task);
static void (*mode_steps[])(struct task *) = {door_shut, door_opening,
                                              door_bouncing, door_closing};

void bigbogy_door_init(struct task *task)
{
    /* @hack spelling: task->work read again for all but pos, where work would
     * do */
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = door_draw;
    task->step = door_step;
    task->destructor = door_destroy;
    if (WALL_DOOR(task->work)) {
        collision_init(task, &wall_shape, 1, COLLISION_OBJECT);
    } else {
        collision_init(task, &floor_shape, 1, COLLISION_OBJECT);
    }
    TRAVEL(task->work) = 0.0F;
    task->work->mode = MODE_SHUT;
    task->work->submode = NO_NEXT_MODE;
    DOOR_HOME(task) = work->pos;
}

static void door_destroy(struct task *task)
{
}

static void door_step(struct task *task)
{
    /* @hack spelling: task->work read again for all but pos and ang.y, where
     * work would do */
    struct task_work *work = task->work;
    Float dx;
    Float dz;

    if (placement_try_leave(task)) {
        return;
    }
    take_mode(task);
    mode_steps[task->work->mode](task);
    if (WALL_DOOR(task->work)) {
        work->pos.y = DOOR_HOME(task).y + TRAVEL(task->work);
    } else {
        sandglass_polar_xz(work->ang.y, TRAVEL(task->work), &dx, &dz);
        work->pos.x = DOOR_HOME(task).x + dx;
        work->pos.z = DOOR_HOME(task).z + dz;
    }
    collision_register(task);
}

static void door_draw(struct task *task)
{
    /* @hack spelling: task->work read again for ang.x, where work would do */
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    if (WALL_DOOR(task->work)) {
        njSetTexture(&bigbogy_wall_door_texlist);
        njCnkEasyDrawModel(&bigbogy_wall_door_model);
    } else {
        njSetTexture(&bigbogy_floor_door_texlist);
        njCnkEasyDrawModel(&bigbogy_floor_door_model);
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

#pragma inline(request_mode)
static void request_mode(struct task *task, int mode)
{
    task->work->submode = mode;
}

static void take_mode(struct task *task)
{
    if (task->work->submode != NO_NEXT_MODE) {
        task->work->mode = task->work->submode;
        task->work->submode = NO_NEXT_MODE;
    }
}

static void door_shut(struct task *task)
{
    struct task_work *work = task->work;
    struct task *glass = door_glass(work->ang.z);
    if (glass && glass->work->mode == SANDGLASS_MODE_RUNNING) {
        request_mode(task, MODE_OPENING);
    }
}

static void door_opening(struct task *task)
{
    /* @hack spelling: task->work read again for the travel and the bounce,
     * where work would do */
    struct task_work *work = task->work;
    struct task *glass = door_glass(work->ang.z);
    if (!glass) {
        return;
    }
    TRAVEL(task->work) =
        (1.0F - njCos(SANDGLASS(glass)->time * OPEN_TURN)) * OPEN_TRAVEL;
    if (SANDGLASS(glass)->time >= OPEN_STEPS) {
        TRAVEL(task->work) = OPEN_TRAVEL;
        BOUNCE(task->work) = BOUNCE_START;
        play_open(task);
        request_mode(task, MODE_BOUNCING);
    }
    if (glass->work->mode != SANDGLASS_MODE_RUNNING) {
        TRAVEL(task->work) = 0.0F;
        play_shut(task);
        request_mode(task, MODE_SHUT);
    }
}

static void door_bouncing(struct task *task)
{
    /* @hack spelling: task->work read again for the travel and the bounce,
     * where work would do */
    struct task_work *work = task->work;
    struct task *glass = door_glass(work->ang.z);
    if (!glass) {
        return;
    }
    TRAVEL(task->work) =
        OPEN_TRAVEL +
        (njCos((SANDGLASS(glass)->time - OPEN_STEPS) * HALF_TURN) *
         BOUNCE(task->work));
    BOUNCE(task->work) *= BOUNCE_DECAY;
    if ((Float)SANDGLASS(glass)->time >=
        OPEN_STEPS + (BOUNCE_SHARE * SANDGLASS_RUN_STEPS(glass))) {
        TRAVEL(task->work) = OPEN_TRAVEL;
        request_mode(task, MODE_CLOSING);
    }
    if (glass->work->mode != SANDGLASS_MODE_RUNNING) {
        TRAVEL(task->work) = 0.0F;
        play_shut(task);
        request_mode(task, MODE_SHUT);
    }
}

static void door_closing(struct task *task)
{
    /* @hack spelling: task->work read again for the travel, where work would
     * do */
    struct task_work *work = task->work;
    struct task *glass = door_glass(work->ang.z);
    Float notch_steps;
    Float sunk;
    Float drop;
    int closing_steps;
    int elapsed;

    if (!glass) {
        return;
    }
    closing_steps = SANDGLASS_RUN_STEPS(glass) -
                    (SANDGLASS_RUN_STEPS(glass) * BOUNCE_SHARE) - OPEN_STEPS;
    notch_steps = closing_steps * (1.0F / NOTCHES);
    elapsed = (Float)SANDGLASS(glass)->time -
              (SANDGLASS_RUN_STEPS(glass) * BOUNCE_SHARE) - OPEN_STEPS;
    play_lower(task);
    sunk = (int)(elapsed / notch_steps) * NOTCH_DEPTH;
    drop = (1.0F - njCos(float_wrap(elapsed, notch_steps) / notch_steps *
                         QUARTER_TURN)) *
           NOTCH_DEPTH;
    TRAVEL(task->work) = OPEN_TRAVEL - sunk - drop;
    if (TRAVEL(task->work) <= 0.0F) {
        TRAVEL(task->work) = 0.0F;
    }
    if (glass->work->mode != SANDGLASS_MODE_RUNNING) {
        TRAVEL(task->work) = 0.0F;
        request_mode(task, MODE_SHUT);
    }
}

static struct task *door_glass(int id)
{
    struct task *glass = sandglass_find(id);
    if (!glass || !glass->work) {
        return NULL;
    }
    return glass;
}

/* @hack spelling: the volume written out at each call, where a function of
 * the door and its sound's three settings would do */
#define HEIGHT_VOLUME(result, task, near, far, range)                          \
    do {                                                                       \
        struct task_work *work = (task)->work;                                 \
        NJS_POINT3 camera;                                                     \
        Sint32 near_volume;                                                    \
        Sint32 far_volume;                                                     \
        camera = camera_views[camera_current_index]->pos;                      \
        near_volume = (near);                                                  \
        far_volume = (far);                                                    \
        (result) =                                                             \
            near_volume + (((Float)(far_volume - near_volume) / (range)) *     \
                           njAbs(camera.y - work->pos.y));                     \
    } while (0)

static void play_open(struct task *task)
{
    int volume;
    HEIGHT_VOLUME(volume, task, open_near, open_far, open_range);
    sound_play_for(OPEN_SOUND, DOOR_SOUND_KEY(task), OPEN_SOUND_PRIORITY, 0,
                   OPEN_SOUND_DURATION);
    sound_set_volume(DOOR_SOUND_KEY(task), SOUND_ANY_ID, volume);
}

static void play_lower(struct task *task)
{
    int volume;
    HEIGHT_VOLUME(volume, task, lower_near, lower_far, lower_range);
    sound_play_for(LOWER_SOUND, DOOR_SOUND_KEY(task), LOWER_SOUND_PRIORITY, 0,
                   LOWER_SOUND_DURATION);
    sound_set_volume(DOOR_SOUND_KEY(task), SOUND_ANY_ID, volume);
}

static void play_shut(struct task *task)
{
    int volume;
    HEIGHT_VOLUME(volume, task, shut_near, shut_far, shut_range);
    sound_play(SHUT_SOUND, DOOR_SOUND_KEY(task), SHUT_SOUND_PRIORITY, 0);
    sound_set_volume(DOOR_SOUND_KEY(task), SOUND_ANY_ID, volume);
}
