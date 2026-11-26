/* Pyramid Cave's sandglass, SANDGLASS in the stage's object list: a copy of
 * shared/sandglass.c's sandglass. the stage's sounds are written in where that
 * one calls hooks. unlike that one, it draws at any distance. it draws its own
 * shadow, its frame flattened, in flight too. its button stands higher and is
 * wider */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "object/mover.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/sandglass.h"
#include "sound/sound.h"
#include "stg28/cave_util.h"
#include "stg28/sandglass.h"
#include "stg28/sandglass_registry.h"
#include "system/angle.h"
#include "system/matrix.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

#include "stg28/sandglass/text.inc"
#include "shared/sandglass_art.h"

/* a created sandglass's task flags and level, and each collision child's
 * task flags */
#define SANDGLASS_TASK (TASK_MOTION | TASK_WORK)
#define SANDGLASS_LEVEL 2
#define CHILD_TASK TASK_WORK

/* a created sandglass lands at CREATED_FLOOR */
#define CREATED_FLOOR (-40.0F)
/* the frame's center lies FRAME_LIFT above the work's pos, plus the record's
 * lift */
#define FRAME_LIFT 10.0F
/* the button's shape lies BUTTON_DROP lower, and is no target, while the
 * sandglass isn't a placed one standing */
#define BUTTON_DROP 10.0F

/* an end's offset moves it out along the sandglass's axis, or in where
 * negative. each step an end's speed keeps END_DAMPING of itself, less
 * END_PULL of the offset */
#define END_DAMPING 0.82F
#define END_PULL 0.63F
/* a touch on the button sets the top end's offset to -END_KICK and the bottom
 * end's to END_KICK. the run's end sets both to END_KICK. a landing sets the
 * bottom end's to LAND_KICK. each step of the run sets the top end's to
 * RUN_PRESS */
#define END_KICK 0.57F
#define LAND_KICK (-0.285F)
#define RUN_PRESS (-2.0F)
/* a strike lowers rise to 0 by the size of STRIKE_SINK a step. the run's end
 * raises it to 1 by RUN_END_RISE a step */
#define STRIKE_SINK (-0.1F)
#define RUN_END_RISE 0.2F

/* a touch on the button sets the sandglass's spd.y to STEP_JUMP, and a blow
 * sets it to BLOW_JUMP. either starts its tumble at TUMBLE_START a step. a blow
 * also sets its speed across the ground to BLOW_SPEED. the tumbling step moves
 * it only along y */
#define STEP_JUMP 2.0F
#define BLOW_JUMP 1.5F
#define BLOW_SPEED 0.5F
#define TUMBLE_START (-0x800)
/* each step of a flight takes GRAVITY from spd.y, and keeps AIR_DRAG of spd
 * and TUMBLE_DRAG of ang_spd.x */
#define GRAVITY 0.08F
#define AIR_DRAG 0.997F
#define TUMBLE_DRAG 0.95F
/* the landing's point lies LANDING_RISE above the floor */
#define LANDING_RISE 2.5F

/* ids in pyramid_cave_sound_list */
#define HIT_SOUND 3
#define TICK_SOUND 4
/* where player 0's touch strikes the button, player 0 bounces up at
 * BOUNCE_LIFT. BOUNCE_SOUND plays, and player 0's pad vibrates */
#define BOUNCE_SOUND 0x1000
#define BOUNCE_LIFT 2.0F
#define VIBRATION_POWER 4
#define VIBRATION_FREQ 15

/* a run shows the pile's shape from frame 0 to SAND_LAST_FRAME */
#define SAND_LAST_FRAME 59.0F

/* sum is the run's steps left plus tick_lead. after a tick, the next waits
 * (int)(sum * TICK_RATE) + 1 steps. so the ticks quicken as the run ends. where
 * sum is at most TICK_WAIT * TICK_RATE, the wait is TICK_WAIT steps instead.
 * only a negative SANDGLASS_RUN_STEPS allows that */
#define TICK_WAIT 15
#define TICK_RATE 0.1F

/* the shade of the ends while the sandglass isn't standing */
#define SHADE 0.67F
#define SHADOW_ALPHA 0.5F
/* the frame's shadow lies SHADOW_LIFT above the floor */
#define SHADOW_LIFT 0.1F

static void sandglass_destroy(struct task *task);
static void sandglass_step(struct task *task);
static void sandglass_draw(struct task *task);
static struct sandglass *sandglass_alloc(struct task *task);
static void sandglass_free(struct task *task);
static void sandglass_button_init(struct task *task);
static void sandglass_button_step(struct task *task);
static void sandglass_body_init(struct task *task);
static void sandglass_body_step(struct task *task);
static void sandglass_standing_step(struct task *task);
static void sandglass_tumbling_step(struct task *task);
static void sandglass_thrown_step(struct task *task);
static void sandglass_running_step(struct task *task);
static void sandglass_held_step(struct task *task);
static void sandglass_frame_draw(struct task *task);
static void sandglass_ends_draw(struct task *task);
static void sandglass_sand_draw(struct task *task);
static void sandglass_frame_shadow_draw(struct task *task);
static void sandglass_tick_timer_set(struct task *task);

NJS_TEXLIST *cave_sandglass_texlists[5] = {
#include "stg28/sandglass/cave_sandglass_texlists.inc"
};
/* clang-format off */
/* the body's shape, and the button's on top of it. the button pushes no group's
 * shapes, and none of a player's, an enemy's or an object's pushes it */
static struct collision_shape body_shape = {
    0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
    SHAPE_UNK_2000 | SHAPE_UNTURNED, {0.0F, 10.0F, 0.0F}, 7.5F, 10.0F, 0.0F, 0,
    {0, 0, 0}};
static struct collision_shape button_shape = {
    0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_HURTS_ALL,
    SHAPE_UNTURNED, {0.0F, 27.5F, 0.0F}, 6.25F, 2.5F, 0.0F, 0, {0, 0, 0}};
/* clang-format on */
/* the alphas of a created sandglass while it stands or runs: the frame's and
 * the ends', the sand's and the shadow's. the sand's never shows, by the bug
 * in sandglass_sand_draw */
static Float created_alpha = 0.5F;
static Float created_sand_alpha = 0.75F;
static Float created_shadow_alpha = 0.5F;

static Sint32 hit_volume = 100;
static Sint32 tick_volume = 100;
typedef void (*sandglass_mode_step)(struct task *task);

KEPT static sandglass_mode_step mode_steps[5] = {
#include "stg28/sandglass/mode_steps.inc"
};
static Sint32 dat_8c5da390 = {
#include "stg28/sandglass/dat_8c5da390.inc"
};
/* the stream grows from nothing to its full width and alpha over the pile's
 * first fade_frames frames. it shrinks back over the pile's last fade_frames
 * frames */
static Sint32 fade_frames = 15;
/* flattens what it multiplies onto y = 0 */
/* clang-format off */
static NJS_MATRIX flatten_matrix = {
    1.0F, 0.0F, 0.0F, 0.0F,
    0.0F, 0.0F, 0.0F, 0.0F,
    0.0F, 0.0F, 1.0F, 0.0F,
    0.0F, 0.0F, 0.0F, 1.0F,
};
/* clang-format on */
static Sint32 tick_lead = 15;

/* @hack spelling: the hit's sound as an inline function, where its two calls
 * written out at each use would do */
#pragma inline(sandglass_play_hit_sound)
static void sandglass_play_hit_sound(struct task *task)
{
    sound_play(HIT_SOUND, task, 0, 0);
    sound_set_volume(task, SOUND_ANY_ID, hit_volume);
}

struct task *cave_sandglass_create(NJS_POINT3 *pos, Angle ang, int id, int time)
{
    struct task *task = task_create(SANDGLASS_TASK, SANDGLASS_LEVEL,
                                    sandglass_step, SANDGLASS_TASK_NAME);

    if (task == NULL || sandglass_alloc(task) == NULL) {
        /* @bug memory: leaves the task alive where sandglass_alloc fails, at
         * 0x8c507028. the task's step then passes it to
         * cave_sandglass_take_mode, at 0x8c507130, which reads its record
         * through null */
#ifdef NONMATCHING
        /* this side repairs the tag above: it destroys the task. the failed
         * sandglass_alloc has freed what it made */
        if (task != NULL) {
            task_destroy(task);
        }
#endif
        return NULL;
    }
    task->draw = sandglass_draw;
    task->destructor = sandglass_destroy;
    task->work->pos = *pos;
    task->work->ang.y = ang;
    task->work->ang.z = (Uint8)id;
    cave_sandglass_register(task);
    SANDGLASS_RUN_STEPS(task) = (Float)time;
    task->work->mode = SANDGLASS_MODE_HELD;
    SANDGLASS(task)->next_mode = SANDGLASS_NO_MODE;
    SANDGLASS(task)->floor = CREATED_FLOOR;
    SANDGLASS(task)->rise = 1.0F;
    SANDGLASS(task)->rise_spd = 0.0F;
    SANDGLASS(task)->created = TRUE;
    return task;
}

void cave_sandglass_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task) || sandglass_alloc(task) == NULL) {
        return;
    }
    task->draw = sandglass_draw;
    task->step = sandglass_step;
    task->destructor = sandglass_destroy;
    cave_sandglass_register(task);
    /* @hack spelling: task->work read again for the mode, where work would do
     */
    task->work->mode = SANDGLASS_MODE_STANDING;
    SANDGLASS(task)->next_mode = SANDGLASS_NO_MODE;
    SANDGLASS(task)->floor = work->pos.y;
    SANDGLASS(task)->rise = 1.0F;
    SANDGLASS(task)->rise_spd = 0.0F;
    SANDGLASS(task)->created = FALSE;
}

static void sandglass_destroy(struct task *task)
{
    cave_sandglass_unregister(task);
    sandglass_free(task);
}

static void sandglass_step(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement == NULL || placement_try_leave(task) == FALSE) {
        cave_sandglass_take_mode(task);
        /* @hack spelling: task->work read again for the mode, where work would
         * do */
        mode_steps[task->work->mode](task);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        SANDGLASS(task)->lift = SANDGLASS(task)->rise * SANDGLASS_RISE;
        SANDGLASS(task)->lift += SANDGLASS(task)->bottom_offset;
        {
            /* @hack spelling: y, a local for the frame's height, where the
             * sum in the call would do */
            Float y = work->pos.y + SANDGLASS(task)->lift + FRAME_LIFT;

            njTranslate(NULL, work->pos.x, y, work->pos.z);
        }
        njRotateY(NULL, work->ang.y + SANDGLASS(task)->tumble_turn);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, -SANDGLASS(task)->tumble_turn);
        /* @hack spelling: the two calls joined by a comma, where two
         * statements would do */
        matrix_get(SANDGLASS(task)->matrix), njPopMatrixEx();
    }
}

static void sandglass_draw(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)SANDGLASS_EDITOR_ID;
        (void)SANDGLASS_EDITOR_DROP_TIME;
    }
    njPushMatrixEx();
    matrix_multiply_before(SANDGLASS(task)->matrix);
    sandglass_frame_draw(task);
    draw_control_set(DRAW_CONTROL_SHADOW);
    sandglass_ends_draw(task);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    sandglass_sand_draw(task);
    njPopMatrixEx();
    if (!SANDGLASS(task)->created || task->work->mode != SANDGLASS_MODE_HELD) {
        sandglass_frame_shadow_draw(task);
    }
}

/* give task its record, the record's matrix and its two collision children,
 * and build the matrix. it returns the record. where one of the four can't be
 * had, it frees the others and returns null */
static struct sandglass *sandglass_alloc(struct task *task)
{
    struct task_work *work = task->work;
    NJS_MATRIX *matrix;
    struct task *button;
    struct task *body;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (!(matrix = syMalloc(sizeof(NJS_MATRIX)))) {
        return NULL;
    }
    task->extra.p = syMalloc(sizeof(struct sandglass));
    if (!task->extra.p) {
        syFree(matrix);
        return NULL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (!(button =
              task_create_child(CHILD_TASK, sandglass_button_init, task))) {
        syFree(matrix);
        /* @hack redundant: the test of extra, which the test above has passed,
         * where the free alone would do */
        if (task->extra.p) {
            syFree(task->extra.p);
            task->extra.p = NULL;
        }
        return NULL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (!(body = task_create_child(CHILD_TASK, sandglass_body_init, task))) {
        syFree(matrix);
        /* @hack redundant: the test of extra, which the test above has passed,
         * where the free alone would do */
        if (task->extra.p) {
            syFree(task->extra.p);
            task->extra.p = NULL;
        }
        task_destroy(button);
        return NULL;
    }
    SANDGLASS(task)->time = 0;
    SANDGLASS(task)->button = button;
    SANDGLASS(task)->body = body;
    SANDGLASS(task)->matrix = matrix;
    SANDGLASS(task)->held_turn = 0;
    SANDGLASS(task)->top_offset = 0.0F;
    SANDGLASS(task)->bottom_offset = 0.0F;
    SANDGLASS(task)->top_offset_spd = 0.0F;
    SANDGLASS(task)->bottom_offset_spd = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y + FRAME_LIFT, work->pos.z);
    /* @bug uninitialized: turns by tumble_turn, which syMalloc left as the
     * heap had it, at 0x8c507370 and 0x8c507386. each step reads it the same
     * way until a strike or a landing sets it. the two turns cancel only
     * while ang.x is 0 */
#ifdef NONMATCHING
    /* this side repairs the tag above and the one in sandglass_running_step:
     * tumble_turn starts at 0, as a landing sets it, and tick_timer at 0, as
     * shared/sandglass.c's alloc sets it */
    SANDGLASS(task)->tumble_turn = 0;
    SANDGLASS(task)->tick_timer = 0;
#endif
    njRotateY(NULL, work->ang.y + SANDGLASS(task)->tumble_turn);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, -SANDGLASS(task)->tumble_turn);
    matrix_get(SANDGLASS(task)->matrix);
    njPopMatrixEx();
    return task->extra.p;
}

static void sandglass_free(struct task *task)
{
    if (task->extra.p) {
        if (SANDGLASS(task)->matrix) {
            syFree(SANDGLASS(task)->matrix);
        }
        if (SANDGLASS(task)->button) {
            task_kill(SANDGLASS(task)->button);
        }
        if (SANDGLASS(task)->body) {
            task_kill(SANDGLASS(task)->body);
        }
        /* @hack redundant: the second test of extra, which the first has
         * passed, where the free alone would do */
        if (task->extra.p) {
            syFree(task->extra.p);
            task->extra.p = NULL;
        }
    }
}

static void sandglass_button_init(struct task *task)
{
    task->step = sandglass_button_step;
    collision_init(task, &button_shape, 1, COLLISION_OBJECT);
}

/* the button's child follows the sandglass, and turns it over at a touch, as
 * cave_sandglass_init says. it also moves the ends and the rise by their
 * speeds */
static void sandglass_button_step(struct task *task)
{
    struct task *parent = task->parent;
    struct collision_work *collision = task->work->collision;

    task->work->pos = parent->work->pos;
    task->work->ang = parent->work->ang;
    SANDGLASS(parent)->top_offset += SANDGLASS(parent)->top_offset_spd;
    SANDGLASS(parent)->top_offset_spd =
        (SANDGLASS(parent)->top_offset_spd * END_DAMPING) -
        (SANDGLASS(parent)->top_offset * END_PULL);
    SANDGLASS(parent)->bottom_offset += SANDGLASS(parent)->bottom_offset_spd;
    SANDGLASS(parent)->bottom_offset_spd =
        (SANDGLASS(parent)->bottom_offset_spd * END_DAMPING) -
        (SANDGLASS(parent)->bottom_offset * END_PULL);
    SANDGLASS(parent)->rise += SANDGLASS(parent)->rise_spd;
    if (SANDGLASS(parent)->rise < 0.0F) {
        SANDGLASS(parent)->rise = 0.0F;
        SANDGLASS(parent)->rise_spd = 0.0F;
    }
    if (SANDGLASS(parent)->rise > 1.0F) {
        SANDGLASS(parent)->rise = 1.0F;
        SANDGLASS(parent)->rise_spd = 0.0F;
    }
    if (!SANDGLASS(parent)->created &&
        parent->work->mode == SANDGLASS_MODE_STANDING) {
        /* @hack spelling: task->work read again for the collision, where
         * collision would do */
        task->work->collision->flags |= COLLISION_TARGET;
        if (collision->flags & COLLISION_TOUCHED) {
            collision->flags &= ~COLLISION_TOUCHED;
            sandglass_play_hit_sound(parent);
            cave_sandglass_set_mode(parent, SANDGLASS_MODE_TUMBLING);
            /* @hack spelling: parent's motion cast at each use, where a local
             * would do */
            ((struct sandglass_motion *)parent->motion.p)->spd.y = STEP_JUMP;
            ((struct sandglass_motion *)parent->motion.p)->ang_spd.x =
                TUMBLE_START;
            if (collision->hit->owner == player_tasks[0]) {
                sound_play(BOUNCE_SOUND, NULL, 0, 0);
                player_bounce(0, 0.0F, BOUNCE_LIFT, 0.0F);
                vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ,
                                VIBRATION_UNTIMED);
            }
            SANDGLASS(parent)->top_offset = -END_KICK;
            SANDGLASS(parent)->bottom_offset = END_KICK;
            SANDGLASS(parent)->rise_spd = STRIKE_SINK;
        }
    } else {
        task->work->pos.y -= BUTTON_DROP;
        /* @hack spelling: task->work read again for the collision, where
         * collision would do */
        task->work->collision->flags &= ~COLLISION_TARGET;
    }
    collision_register(task);
}

static void sandglass_body_init(struct task *task)
{
    task->step = sandglass_body_step;
    collision_init(task, &body_shape, 1, COLLISION_OBJECT);
}

/* the body's child follows the sandglass's body, and turns it over at a blow,
 * as cave_sandglass_init says */
static void sandglass_body_step(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *parent_work = parent->work;
    struct sandglass_motion *motion = parent->motion.p;
    /* @hack redundant: the address of the work's collision, which nothing
     * reads */
    struct collision_work **collision = &task->work->collision;
    struct task *player;

    /* @hack spelling: the parent read again through task, where parent
     * would do */
    task->work->pos = task->parent->work->pos;
    /* @hack spelling: parent's work read again for its angles, where
     * parent_work would do */
    task->work->ang = parent->work->ang;
    task->work->pos.y += SANDGLASS(parent)->lift;
    /* @hack spelling: parent's work read again for its mode, where parent_work
     * would do */
    if (!SANDGLASS(parent)->created &&
        parent->work->mode == SANDGLASS_MODE_STANDING &&
        (task->work->collision->flags & COLLISION_TOUCHED) &&
        (player = collision_next_player(task)) != NULL) {
        if (cave_sonic_combo_or_bounce(player)) {
            if (cave_sonic_in_combo(player)) {
                player_request(0, PLAYER_REQUEST_RELEASE, 0);
                player_set_speed(0, 0.0F, 0.0F, 0.0F);
                player_set_acc(0, 0.0F, 0.0F, 0.0F);
            }
            sandglass_play_hit_sound(parent);
            cave_sandglass_set_mode(parent, SANDGLASS_MODE_TUMBLING);
            SANDGLASS(parent)->tumble_turn = cave_angle_turn(
                parent_work->ang.y,
                mover_player_heading(parent, player_slot_of(player)) ^
                    HALF_TURN);
            cave_polar_xz(parent_work->ang.y + SANDGLASS(parent)->tumble_turn,
                          BLOW_SPEED, &motion->spd.x, &motion->spd.z);
            motion->spd.y = BLOW_JUMP;
            motion->ang_spd.x = TUMBLE_START;
            motion->ang_spd.y = 0;
            motion->ang_spd.z = 0;
            SANDGLASS(parent)->rise_spd = STRIKE_SINK;
        }
    }
    /* @bug logic: a created sandglass never registers its body. the test
     * returns where mode isn't SANDGLASS_MODE_STANDING, at 0x8c5077dc, and
     * where it isn't SANDGLASS_MODE_RUNNING, at 0x8c5077e4. no mode is
     * both */
    /* @hack spelling: parent's work read again for its mode, where parent_work
     * would do */
    /* NOLINTBEGIN(misc-redundant-expression) */
    /* NOLINTBEGIN(clang-diagnostic-tautological-overlap-compare) */
    if (SANDGLASS(parent)->created &&
        (parent->work->mode != SANDGLASS_MODE_STANDING ||
         parent->work->mode != SANDGLASS_MODE_RUNNING)) {
        return;
    }
    /* NOLINTEND(clang-diagnostic-tautological-overlap-compare) */
    /* NOLINTEND(misc-redundant-expression) */
    collision_register(task);
}

static void sandglass_standing_step(struct task *task)
{
    SANDGLASS(task)->time = 0;
}

static void sandglass_tumbling_step(struct task *task)
{
    struct sandglass_motion *motion = task->motion.p;
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    motion->spd.y -= GRAVITY;
    motion->spd.y *= AIR_DRAG;
    motion->ang_spd.x *= TUMBLE_DRAG;
    work->pos.y += motion->spd.y;
    work->ang.x += motion->ang_spd.x;
    if (work->ang.x < -HALF_TURN) {
        work->ang.x = 0;
        motion->ang_spd.x = 0;
    }
    if (work->pos.y < SANDGLASS(task)->floor) {
        work->pos.y = SANDGLASS(task)->floor;
        motion->spd.y = 0.0F;
        work->ang.x = 0;
        work->ang.y = 0;
        SANDGLASS(task)->tumble_turn = 0;
        SANDGLASS(task)->time = 0;
        SANDGLASS(task)->bottom_offset = LAND_KICK;
        /* @hack redundant: pos, the landing's point, which nothing reads */
        pos = work->pos;
        pos.y += LANDING_RISE;
        cave_sandglass_set_mode(task, SANDGLASS_MODE_RUNNING);
    }
}

static void sandglass_thrown_step(struct task *task)
{
    struct sandglass_motion *motion = task->motion.p;
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    motion->spd.y -= GRAVITY;
    motion->spd.x *= AIR_DRAG;
    motion->spd.y *= AIR_DRAG;
    motion->spd.z *= AIR_DRAG;
    motion->ang_spd.x *= TUMBLE_DRAG;
    work->pos.x += motion->spd.x;
    work->pos.y += motion->spd.y;
    work->pos.z += motion->spd.z;
    work->ang.x += motion->ang_spd.x;
    if (work->ang.x < -HALF_TURN) {
        work->ang.x = 0;
        motion->ang_spd.x = 0;
    }
    if (work->pos.y < SANDGLASS(task)->floor) {
        work->pos.y = SANDGLASS(task)->floor;
        motion->spd.x = 0.0F;
        motion->spd.y = 0.0F;
        motion->spd.z = 0.0F;
        work->ang.x = 0;
        work->ang.y = 0;
        SANDGLASS(task)->tumble_turn = 0;
        SANDGLASS(task)->time = 0;
        SANDGLASS(task)->bottom_offset = LAND_KICK;
        /* @hack redundant: pos, the landing's point, which nothing reads */
        pos = work->pos;
        pos.y += LANDING_RISE;
        cave_sandglass_set_mode(task, SANDGLASS_MODE_RUNNING);
    }
}

static void sandglass_running_step(struct task *task)
{
    /* @bug uninitialized: counts down tick_timer, which syMalloc left as the
     * heap had it and no landing clears, at 0x8c507a22. the run's first tick
     * waits on that value. shared/sandglass.c clears it in its alloc */
    if (--SANDGLASS(task)->tick_timer <= 0) {
        sound_play(TICK_SOUND, task, 0, 0);
        sound_set_volume(task, SOUND_ANY_ID, tick_volume);
        sandglass_tick_timer_set(task);
    }
    SANDGLASS(task)->top_offset = RUN_PRESS;
    if (++SANDGLASS(task)->time >= SANDGLASS_RUN_STEPS(task)) {
        cave_sandglass_set_mode(task, SANDGLASS_MODE_STANDING);
        SANDGLASS(task)->rise_spd = RUN_END_RISE;
        SANDGLASS(task)->top_offset = END_KICK;
        SANDGLASS(task)->bottom_offset = END_KICK;
    }
}

static void sandglass_held_step(struct task *task)
{
}

static void sandglass_frame_draw(struct task *task)
{
    njSetTexture(&sandglass_frame_texlist);
    if (SANDGLASS(task)->created &&
        (task->work->mode == SANDGLASS_MODE_STANDING ||
         task->work->mode == SANDGLASS_MODE_RUNNING)) {
        cave_model_draw_material(sandglass_frame_model, created_alpha, 1.0F,
                                 1.0F, 1.0F);
    } else {
        njCnkSimpleDrawModel(sandglass_frame_model);
    }
}

/* draw the two ends at their offsets along the sandglass's axis, the bottom
 * one turned over. a created sandglass draws them translucent while it stands
 * or runs. any other that isn't standing draws them shaded */
static void sandglass_ends_draw(struct task *task)
{
    /* @hack spelling: shape, a local for the ends' shape, where the shape
     * itself would do */
    NJS_MOTION *shape;

    if (SANDGLASS(task)->created &&
        (task->work->mode == SANDGLASS_MODE_STANDING ||
         task->work->mode == SANDGLASS_MODE_RUNNING)) {
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(DRAW_CONTROL_CONSTANT);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        constant_material_set(created_alpha, 1.0F, 1.0F, 1.0F);
    } else if (task->work->mode != SANDGLASS_MODE_STANDING) {
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
        constant_material_set(1.0F, SHADE, SHADE, SHADE);
    }
    njSetTexture(&sandglass_end_texlist);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, SANDGLASS(task)->top_offset, 0.0F);
    shape = &sandglass_end_shape;
    njCnkSimpleDrawShapeMotion(
        &sandglass_end_object, &sandglass_end_motion, shape,
        (1.0F - SANDGLASS(task)->rise) * (shape->nbFrame - 1.0F));
    njPopMatrixEx();
    njPushMatrixEx();
    njRotateX(NULL, HALF_TURN);
    njTranslate(NULL, 0.0F, SANDGLASS(task)->bottom_offset, 0.0F);
    njCnkSimpleDrawShapeMotion(
        &sandglass_end_object, &sandglass_end_motion, shape,
        (1.0F - SANDGLASS(task)->rise) * (shape->nbFrame - 1.0F));
    njPopMatrixEx();
    if ((SANDGLASS(task)->created &&
         (task->work->mode == SANDGLASS_MODE_STANDING ||
          task->work->mode == SANDGLASS_MODE_RUNNING)) ||
        task->work->mode != SANDGLASS_MODE_STANDING) {
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    }
}

/* draw the sand. while the sandglass tumbles or flies, the tumbling sand
 * follows its turn. otherwise the pile and the falling sand show the share of
 * the run gone. the stream falls while the sandglass runs */
static void sandglass_sand_draw(struct task *task)
{
    Float frame;
    Float scale;

    if (task->work->mode == SANDGLASS_MODE_THROWN ||
        task->work->mode == SANDGLASS_MODE_TUMBLING) {
        if (task->work->ang.x == 0) {
            frame = sandglass_tumble_sand_shape->nbFrame - 1.0F;
            /* @hack redundant: a statement that names the shape's drawer,
             * which does nothing */
            (void)njCnkSimpleDrawShapeMotion;
        } else {
            frame = _builtin_fabsf((Float)task->work->ang.x) *
                    sandglass_tumble_sand_shape->nbFrame * (1.0F / HALF_TURN);
        }
        njSetTexture(&sandglass_sand_texlist);
        njPushMatrixEx();
        if (task->work->ang.x != 0) {
            njRotateX(NULL, HALF_TURN);
            njRotateY(NULL, -SANDGLASS(task)->tumble_turn - QUARTER_TURN);
        }
        njCnkSimpleDrawShapeMotion(&sandglass_tumble_sand_object,
                                   &sandglass_tumble_sand_motion,
                                   sandglass_tumble_sand_shape, frame);
        njPopMatrixEx();
    } else {
        if (task->work->mode == SANDGLASS_MODE_STANDING) {
            frame = sandglass_sand_shape->nbFrame - 1;
        } else if (SANDGLASS_RUN_STEPS(task) == 0.0F) {
            frame = 0.0F;
        } else {
            frame = SANDGLASS(task)->time / SANDGLASS_RUN_STEPS(task) *
                    SAND_LAST_FRAME;
        }
        if (SANDGLASS(task)->created &&
            (task->work->mode == SANDGLASS_MODE_STANDING ||
             task->work->mode == SANDGLASS_MODE_RUNNING)) {
            draw_control_save();
            draw_attr_save();
            draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
            draw_control_set(DRAW_CONTROL_CONSTANT);
            njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
            constant_material_set(created_sand_alpha, 1.0F, 1.0F, 1.0F);
        }
        /* @bug logic: puts the draw state back before the sand draws, by the
         * JSR at 0x8c507f2a before the draw at 0x8c507f3a. so a created
         * sandglass's sand draws opaque, not at created_sand_alpha */
        if (SANDGLASS(task)->created &&
            (task->work->mode == SANDGLASS_MODE_STANDING ||
             task->work->mode == SANDGLASS_MODE_RUNNING)) {
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            draw_attr_restore();
            draw_control_restore();
        }
        njSetTexture(&sandglass_sand_texlist);
        njCnkSimpleDrawShapeMotion(&sandglass_falling_sand_object,
                                   &sandglass_falling_sand_motion,
                                   &sandglass_falling_sand_shape, frame);
        njCnkSimpleDrawShapeMotion(&sandglass_sand_object,
                                   &sandglass_sand_motion, sandglass_sand_shape,
                                   frame);
        if (task->work->mode == SANDGLASS_MODE_RUNNING) {
            njSetTexture(&sandglass_stream_texlist);
            if (frame < fade_frames) {
                scale = frame / fade_frames;
            } else if (sandglass_sand_shape->nbFrame - frame < fade_frames) {
                scale = (sandglass_sand_shape->nbFrame - frame) / fade_frames;
            } else {
                scale = 1.0F;
            }
            if (scale == 1.0F) {
                njCnkSimpleDrawModel(&sandglass_stream_model);
            } else {
                njScale(NULL, scale, 1.0F, scale);
                draw_control_save();
                draw_attr_save();
                draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
                draw_control_set(DRAW_CONTROL_CONSTANT);
                njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
                constant_material_set(scale, 1.0F, 1.0F, 1.0F);
                njCnkSimpleDrawModel(&sandglass_stream_model);
                constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
                draw_attr_restore();
                draw_control_restore();
            }
        }
    }
}

/* draw the frame's shadow: its model flattened onto the floor, in a
 * translucent black */
static void sandglass_frame_shadow_draw(struct task *task)
{
    NJS_MATRIX matrix;
    /* @hack spelling: the matrix's elements through a Float pointer, where
     * matrix itself would do */
    Float *element = matrix;

    njPushMatrixEx();
    njSetMatrix(NULL, &flatten_matrix);
    matrix_multiply_before(SANDGLASS(task)->matrix);
    njGetMatrix(&matrix);
    njPopMatrixEx();
    element[MATRIX_Y] = SANDGLASS(task)->floor + SHADOW_LIFT;
    njPushMatrixEx();
    njMultiMatrix(NULL, &matrix);
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT);
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
    if (SANDGLASS(task)->created &&
        (task->work->mode == SANDGLASS_MODE_STANDING ||
         task->work->mode == SANDGLASS_MODE_RUNNING)) {
        constant_material_set(created_shadow_alpha, 0.0F, 0.0F, 0.0F);
    } else {
        constant_material_set(SHADOW_ALPHA, 0.0F, 0.0F, 0.0F);
    }
    njSetTexture(&sandglass_frame_texlist);
    njCnkSimpleDrawModel(sandglass_frame_model);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    njPopMatrixEx();
}

static void sandglass_tick_timer_set(struct task *task)
{
    int left = (int)(SANDGLASS_RUN_STEPS(task) - SANDGLASS(task)->time);
    Float sum = left + tick_lead;

    if (sum > TICK_WAIT * TICK_RATE) {
        SANDGLASS(task)->tick_timer = (int)(sum * TICK_RATE) + 1;
    } else {
        SANDGLASS(task)->tick_timer = TICK_WAIT;
    }
}
