/* the sandglass, SANDGLASS in a stage's object list. Death Chamber and King
 * Boom Boo's fight link this source. the boss's code holds and throws its
 * sandglasses. Pyramid Cave's copy is stg28/sandglass.c */
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
#include "shared/sandglass_registry.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/matrix.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

#include "shared/sandglass/text.inc"
#include "shared/sandglass_art.h"

/* a created sandglass's task flags and level, and each collision child's
 * task flags */
#define SANDGLASS_TASK (TASK_MOTION | TASK_WORK)
#define SANDGLASS_LEVEL 2
#define CHILD_TASK TASK_WORK

/* a created sandglass lands at CREATED_FLOOR */
#define CREATED_FLOOR (-40.0F)
/* the frame's center stands FRAME_LIFT plus the record's lift above the
 * work's pos */
#define FRAME_LIFT 10.0F
/* the button's shape drops BUTTON_DROP and is no target, unless the sandglass
 * is a placed one that stands */
#define BUTTON_DROP 10.0F

/* each step an end's speed keeps END_DAMPING of itself, less END_PULL of
 * the offset */
#define END_DAMPING 0.82F
#define END_PULL 0.63F
/* a touch on the button sets the top end's offset to -END_KICK and the bottom
 * end's to END_KICK. the run's end sets both to END_KICK. a landing sets the
 * bottom end's to LAND_KICK. each step of the run sets the top end's to
 * RUN_PRESS */
#define END_KICK 0.57F
#define LAND_KICK (-0.285F)
#define RUN_PRESS (-2.0F)
/* a strike sets rise_spd to STRIKE_SINK, which lowers rise to 0. the run's
 * end sets it to RUN_END_RISE, which raises rise back to 1 */
#define STRIKE_SINK (-0.1F)
#define RUN_END_RISE 0.2F

/* a touch on the button sets the sandglass's spd.y to STEP_JUMP, and a blow
 * sets it to BLOW_JUMP. either sets its ang_spd.x to TUMBLE_START. a blow also
 * sets its speed across the ground to BLOW_SPEED, which the tumbling step
 * never reads */
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

/* where player 0's touch strikes the button, player 0 bounces up at
 * BOUNCE_LIFT. BOUNCE_SOUND plays, and player 0's pad vibrates */
#define BOUNCE_SOUND 0x1000
#define BOUNCE_LIFT 2.0F
#define VIBRATION_POWER 4
#define VIBRATION_FREQ 15

/* a run shows the pile's shape from frame 0 to SAND_LAST_FRAME */
#define SAND_LAST_FRAME 59.0F

/* the ends' shade while the sandglass isn't standing. a created one that
 * runs draws them translucent instead */
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

NJS_TEXLIST *sandglass_texlists[5] = {
#include "shared/sandglass/sandglass_texlists.inc"
};
/* the body's shape, and the button's on top of it. the body pushes the shapes
 * of players, enemies and objects, and none of theirs pushes either one */
#define BODY_SHAPES 1
#define BUTTON_SHAPES 1
static struct collision_shape body_shape = {
#include "shared/sandglass/body_shape.inc"
};
static struct collision_shape button_shape = {
#include "shared/sandglass/button_shape.inc"
};
/* the alphas of a created sandglass that stands or runs: its frame's and its
 * ends', and its sand's. sandglass_frame_shadow_draw picks
 * created_shadow_alpha in those modes too. it runs only while the sandglass
 * tumbles or is thrown, so no shadow takes that alpha */
static Float created_alpha = {
#include "shared/sandglass/created_alpha.inc"
};
static Float created_sand_alpha = {
#include "shared/sandglass/created_sand_alpha.inc"
};
static Float created_shadow_alpha = {
#include "shared/sandglass/created_shadow_alpha.inc"
};
typedef void (*sandglass_mode_step)(struct task *task);

KEPT static sandglass_mode_step mode_steps[5] = {
#include "shared/sandglass/mode_steps.inc"
};
static Sint32 dat_sandglass_0 = {
#include "shared/sandglass/dat_sandglass_0.inc"
};
/* a placed sandglass draws only within 250 of player 0 */
static Float draw_range2 = {
#include "shared/sandglass/draw_range2.inc"
};
/* the stream grows from nothing to its full width and alpha over the pile's
 * first fade_frames frames. it shrinks back over the pile's last fade_frames
 * frames */
static Sint32 fade_frames = {
#include "shared/sandglass/fade_frames.inc"
};
/* flattens what it multiplies onto y = 0 */
static NJS_MATRIX flatten_matrix = {
#include "shared/sandglass/flatten_matrix.inc"
};
/* left is the steps a run has left. after each tick of the run, the next
 * waits (int)((left + tick_lead) * tick_rate) + 1 steps. so the ticks quicken
 * toward the run's end. the wait is tick_wait steps instead where
 * left + tick_lead is at most tick_wait * tick_rate. only a negative
 * SANDGLASS_RUN_STEPS makes it so */
static Sint32 tick_lead = {
#include "shared/sandglass/tick_lead.inc"
};
static Sint32 tick_wait = {
#include "shared/sandglass/tick_wait.inc"
};
static Float tick_rate = {
#include "shared/sandglass/tick_rate.inc"
};

struct task *sandglass_create(NJS_POINT3 *pos, Angle ang, int id, int time)
{
    struct task *task;

    task = task_create(SANDGLASS_TASK, SANDGLASS_LEVEL, sandglass_step,
                       SANDGLASS_TASK_NAME);
    if (task == NULL || sandglass_alloc(task) == NULL) {
        /* @bug memory: leaves the task alive where sandglass_alloc fails, at
         * 0x28 of the object's code. the task's step then passes it to
         * sandglass_take_mode, at 0x130, which reads its record through
         * null */
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
    sandglass_register(task);
    SANDGLASS_RUN_STEPS(task) = time;
    task->work->mode = SANDGLASS_MODE_HELD;
    SANDGLASS(task)->next_mode = SANDGLASS_NO_MODE;
    SANDGLASS(task)->floor = CREATED_FLOOR;
    SANDGLASS(task)->rise = 1.0F;
    SANDGLASS(task)->rise_spd = 0.0F;
    SANDGLASS(task)->created = TRUE;
    return task;
}

void sandglass_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    if (sandglass_alloc(task) == NULL) {
        return;
    }
    task->draw = sandglass_draw;
    task->step = sandglass_step;
    task->destructor = sandglass_destroy;
    sandglass_register(task);
    /* @hack spelling: task->work read again for the mode, where work would
     * do */
    task->work->mode = SANDGLASS_MODE_STANDING;
    SANDGLASS(task)->next_mode = SANDGLASS_NO_MODE;
    SANDGLASS(task)->floor = work->pos.y;
    SANDGLASS(task)->rise = 1.0F;
    SANDGLASS(task)->rise_spd = 0.0F;
    SANDGLASS(task)->created = FALSE;
}

static void sandglass_destroy(struct task *task)
{
    sandglass_unregister(task);
    sandglass_free(task);
}

static void sandglass_step(struct task *task)
{
    struct task_work *work = task->work;
    Float y;

    if (task->placement == NULL || placement_try_leave(task) == FALSE) {
        sandglass_take_mode(task);
        /* @hack spelling: task->work read again for the mode, where work would
         * do */
        mode_steps[task->work->mode](task);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        SANDGLASS(task)->lift = SANDGLASS(task)->rise * SANDGLASS_RISE;
        SANDGLASS(task)->lift += SANDGLASS(task)->bottom_offset;
        /* @hack spelling: y, a local for the frame's height, where the sum in
         * the call would do */
        y = work->pos.y + SANDGLASS(task)->lift + FRAME_LIFT;
        njTranslate(NULL, work->pos.x, y, work->pos.z);
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
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    int mode;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)SANDGLASS_EDITOR_ID;
        (void)SANDGLASS_EDITOR_DROP_TIME;
    }
    pos = player_work[0]->pos;
    if (SANDGLASS(task)->created == FALSE &&
        ((pos.x - work->pos.x) * (pos.x - work->pos.x)) +
                ((pos.y - work->pos.y) * (pos.y - work->pos.y)) +
                ((pos.z - work->pos.z) * (pos.z - work->pos.z)) >
            draw_range2) {
        return;
    }
    njPushMatrixEx();
    matrix_multiply_before(SANDGLASS(task)->matrix);
    sandglass_frame_draw(task);
    draw_control_set(DRAW_CONTROL_SHADOW);
    sandglass_ends_draw(task);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    sandglass_sand_draw(task);
    njPopMatrixEx();
    /* @hack spelling: task->work read again for the mode, where work would
     * do */
    if (SANDGLASS(task)->created && task->work->mode == SANDGLASS_MODE_HELD) {
        return;
    }
    mode = task->work->mode;
    if (mode == SANDGLASS_MODE_TUMBLING || mode == SANDGLASS_MODE_THROWN) {
        sandglass_frame_shadow_draw(task);
    } else {
        sandglass_shadow_draw(task);
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
    if ((matrix = syMalloc(sizeof(NJS_MATRIX))) == NULL) {
        return NULL;
    }
    task->extra.p = syMalloc(sizeof(struct sandglass));
    if (task->extra.p == NULL) {
        syFree(matrix);
        return NULL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((button = task_create_child(CHILD_TASK, sandglass_button_init, task)) ==
        NULL) {
        syFree(matrix);
        /* @hack redundant: the test of extra, which the test above has passed,
         * where the free alone would do */
        if (task->extra.p != NULL) {
            syFree(task->extra.p);
            task->extra.p = NULL;
        }
        return NULL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((body = task_create_child(CHILD_TASK, sandglass_body_init, task)) ==
        NULL) {
        syFree(matrix);
        /* @hack redundant: the test of extra, which the test above has passed,
         * where the free alone would do */
        if (task->extra.p != NULL) {
            syFree(task->extra.p);
            task->extra.p = NULL;
        }
        task_destroy(button);
        return NULL;
    }
    SANDGLASS(task)->time = 0;
    SANDGLASS(task)->tick_timer = 0;
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
     * heap had it, at 0x45c and 0x472 of the object's code. each step reads
     * it the same way until a strike or a landing sets it. the two turns
     * cancel only while ang.x is 0 */
#ifdef NONMATCHING
    /* this side repairs the tag above: tumble_turn starts at 0, as a landing
     * sets it. the image's is what the heap held, which C can't give */
    SANDGLASS(task)->tumble_turn = 0;
#endif
    njRotateY(NULL, work->ang.y + SANDGLASS(task)->tumble_turn);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, -SANDGLASS(task)->tumble_turn);
    matrix_get(SANDGLASS(task)->matrix);
    njPopMatrixEx();
    return SANDGLASS(task);
}

static void sandglass_free(struct task *task)
{
    if (task->extra.p == NULL) {
        return;
    }
    if (SANDGLASS(task)->matrix != NULL) {
        syFree(SANDGLASS(task)->matrix);
    }
    if (SANDGLASS(task)->button != NULL) {
        task_kill(SANDGLASS(task)->button);
    }
    if (SANDGLASS(task)->body != NULL) {
        task_kill(SANDGLASS(task)->body);
    }
    /* @hack redundant: the second test of extra, which the first has passed,
     * where the free alone would do */
    if (task->extra.p != NULL) {
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

static void sandglass_button_init(struct task *task)
{
    task->step = sandglass_button_step;
    collision_init(task, &button_shape, BUTTON_SHAPES, COLLISION_OBJECT);
}

/* the button's child follows the sandglass, and turns it over at a touch, as
 * sandglass_init says. it also moves the ends and the rise by their speeds */
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
    if (SANDGLASS(parent)->created == FALSE &&
        parent->work->mode == SANDGLASS_MODE_STANDING) {
        /* @hack spelling: task->work read again for the collision, where
         * collision would do */
        task->work->collision->flags |= COLLISION_TARGET;
        if (collision->flags & COLLISION_TOUCHED) {
            collision->flags &= ~COLLISION_TOUCHED;
            sandglass_hit_sound(parent);
            sandglass_set_mode(parent, SANDGLASS_MODE_TUMBLING);
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
    collision_init(task, &body_shape, BODY_SHAPES, COLLISION_OBJECT);
}

/* the body's child follows the sandglass's body, and turns it over at a blow,
 * as sandglass_init says */
static void sandglass_body_step(struct task *task)
{
    struct task *parent = task->parent;
    /* @hack redundant: the address of the work's collision, which nothing
     * reads */
    struct collision_work **collision = &task->work->collision;
    struct task_work *parent_work = parent->work;
    struct sandglass_motion *motion = parent->motion.p;
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
    if (SANDGLASS(parent)->created == FALSE &&
        parent->work->mode == SANDGLASS_MODE_STANDING &&
        (task->work->collision->flags & COLLISION_TOUCHED) &&
        (player = collision_next_player(task)) != NULL) {
        if (sandglass_blow_strikes(player)) {
            if (sandglass_blow_stops(player)) {
                player_request(0, PLAYER_REQUEST_RELEASE, 0);
                player_set_speed(0, 0.0F, 0.0F, 0.0F);
                player_set_acc(0, 0.0F, 0.0F, 0.0F);
            }
            sandglass_hit_sound(parent);
            sandglass_set_mode(parent, SANDGLASS_MODE_TUMBLING);
            SANDGLASS(parent)->tumble_turn = sandglass_angle_turn(
                parent_work->ang.y,
                mover_player_heading(parent, player_slot_of(player)) ^
                    HALF_TURN);
            sandglass_polar_xz(parent_work->ang.y +
                                   SANDGLASS(parent)->tumble_turn,
                               BLOW_SPEED, &motion->spd.x, &motion->spd.z);
            motion->spd.y = BLOW_JUMP;
            motion->ang_spd.x = TUMBLE_START;
            motion->ang_spd.y = 0;
            motion->ang_spd.z = 0;
            SANDGLASS(parent)->rise_spd = STRIKE_SINK;
        }
    }
    /* @bug logic: a created sandglass never registers its body. the test
     * returns where mode isn't SANDGLASS_MODE_STANDING, at 0x85a of the
     * object's code, and where it isn't SANDGLASS_MODE_RUNNING, at 0x862.
     * no mode is both */
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
    struct task_work *work = task->work;
    struct sandglass_motion *motion = task->motion.p;
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
        sandglass_set_mode(task, SANDGLASS_MODE_RUNNING);
    }
}

static void sandglass_thrown_step(struct task *task)
{
    struct task_work *work = task->work;
    struct sandglass_motion *motion = task->motion.p;
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
        SANDGLASS(task)->tick_timer = 0;
        if (SANDGLASS(task)->created) {
            sandglass_land_sound(task);
        }
        sandglass_set_mode(task, SANDGLASS_MODE_RUNNING);
    }
}

static void sandglass_running_step(struct task *task)
{
    if (--SANDGLASS(task)->tick_timer <= 0) {
        sandglass_tick_sound(task);
        sandglass_tick_timer_set(task);
    }
    SANDGLASS(task)->top_offset = RUN_PRESS;
    if (++SANDGLASS(task)->time >= SANDGLASS_RUN_STEPS(task)) {
        sandglass_set_mode(task, SANDGLASS_MODE_STANDING);
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
    /* @hack spelling: model, a local for the frame's model, where the model
     * itself would do */
    NJS_CNK_MODEL *model;

    njSetTexture(&sandglass_frame_texlist);
    model = &sandglass_frame_model[0];
    if (SANDGLASS(task)->created &&
        (task->work->mode == SANDGLASS_MODE_STANDING ||
         task->work->mode == SANDGLASS_MODE_RUNNING)) {
        sandglass_model_draw_material(model, created_alpha, 1.0F, 1.0F, 1.0F);
    } else {
        njCnkSimpleDrawModel(model);
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

/* draw the sand. while the sandglass tumbles or flies, the tumbling sand's
 * frame follows how far it has turned over. otherwise the pile shows the share
 * of the run gone. the falling sand shows with it while the sandglass runs or
 * is held. the stream falls while it runs */
static void sandglass_sand_draw(struct task *task)
{
    NJS_MOTION *stream;
    NJS_MOTION *pile;
    Float frame;
    Float scale;
    Float fade;
    Sint32 (*set_texture)(NJS_TEXLIST *texlist);

    /* @hack spelling: set_texture, a local that holds njSetTexture, set in
     * the first test by a comma, where calls of njSetTexture would do */
    if (set_texture = njSetTexture,
        task->work->mode == SANDGLASS_MODE_THROWN ||
            task->work->mode == SANDGLASS_MODE_TUMBLING) {
        stream = &sandglass_tumble_sand_shape[0];
        if (task->work->ang.x == 0) {
            frame = stream->nbFrame - 1.0F;
        } else {
            frame = _builtin_fabsf((Float)task->work->ang.x) * stream->nbFrame *
                    (1.0F / HALF_TURN);
        }
        set_texture(&sandglass_sand_texlist);
        njPushMatrixEx();
        if (task->work->ang.x != 0) {
            njRotateX(NULL, HALF_TURN);
            njRotateY(NULL, -SANDGLASS(task)->tumble_turn - QUARTER_TURN);
        }
        njCnkSimpleDrawShapeMotion(&sandglass_tumble_sand_object,
                                   &sandglass_tumble_sand_motion, stream,
                                   frame);
        njPopMatrixEx();
    } else {
        pile = &sandglass_sand_shape[0];
        if (task->work->mode == SANDGLASS_MODE_STANDING) {
            frame = pile->nbFrame - 1;
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
        set_texture(&sandglass_sand_texlist);
        if (task->work->mode != SANDGLASS_MODE_STANDING) {
            njCnkSimpleDrawShapeMotion(&sandglass_falling_sand_object,
                                       &sandglass_falling_sand_motion,
                                       &sandglass_falling_sand_shape, frame);
        }
        njCnkSimpleDrawShapeMotion(&sandglass_sand_object,
                                   &sandglass_sand_motion, pile, frame);
        /* @hack redundant: a block no path reaches, which tests the three
         * callees */
        if (0) {
            if (set_texture) {
            }
            if (&njCnkSimpleDrawShapeMotion) {
            }
            if (&constant_material_set) {
            }
            if (set_texture) {
            }
        }
        if (SANDGLASS(task)->created) {
            /* @hack spelling: the two tests of the mode nested, where && would
             * do */
            if (task->work->mode != SANDGLASS_MODE_STANDING) {
                if (task->work->mode != SANDGLASS_MODE_RUNNING) {
                    return;
                }
            }
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            draw_attr_restore();
            draw_control_restore();
        }
        if (task->work->mode == SANDGLASS_MODE_RUNNING) {
            set_texture(&sandglass_stream_texlist);
            fade = fade_frames;
            if (frame < fade) {
                scale = frame / fade;
            } else {
                /* @hack type: pile read through the address of its first
                 * member, where pile would do */
                stream = (NJS_MOTION *)&pile->mdata;
                if (stream->nbFrame - frame < fade) {
                    scale = (stream->nbFrame - frame) / fade;
                } else {
                    scale = 1.0F;
                }
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
    njCnkSimpleDrawModel(&sandglass_frame_model[0]);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    njPopMatrixEx();
}

static void sandglass_tick_timer_set(struct task *task)
{
    int sum;
    int left;

    left = SANDGLASS_RUN_STEPS(task) - SANDGLASS(task)->time;
    sum = tick_lead + left;
    if (sum > tick_wait * tick_rate) {
        SANDGLASS(task)->tick_timer = (int)(sum * tick_rate) + 1;
    } else {
        SANDGLASS(task)->tick_timer = tick_wait;
    }
}
