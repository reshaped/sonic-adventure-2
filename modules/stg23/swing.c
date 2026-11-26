#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/ocean_util.h"
#include "stg23/platforms.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"

#define PITCH(task) ((Sint16)(task)->work->timer)
#define RADIAL_RATE(work) (((work)->ang.x & 0xff) * 16)
#define VERTICAL_RATE(work) (((work)->ang.x & 0xff00) >> 4)
#define RADIAL_SWING(work)                                                     \
    njCos((game_state.steps * RADIAL_RATE(work)) + START_PHASE(work))
#define VERTICAL_SWING(work)                                                   \
    njCos((game_state.steps * VERTICAL_RATE(work)) + START_PHASE(work))
#define EVAL_UNITS (NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL)
/* while player 0 or 1 is within CONTACT_RADIUS of the swing's world object,
 * the swing's work keeps WORLD_ADDED. world_collect_near_players takes the
 * object only then */
#define CONTACT_RADIUS 86.0F
/* while player 0 stands on a swing, tilted under SWING_SPEED about x and z,
 * each swing's pitch holds. otherwise each step moves it by SWING_SPEED times
 * the change ang.z makes in its sine. while ang.z isn't 0, that change counts
 * as at least MIN_STEP */
#define SWING_SPEED 4096.0F
#define MIN_STEP 0.001F
/* player 0's tilt past LEVEL_TILT about x or z eases a held pitch toward the
 * nearest quarter turn. each step the pitch keeps EASE_KEEP / (EASE_KEEP +
 * EASE_MOVE) of its way from there */
#define LEVEL_TILT 64.0F
#define EASE_KEEP 32.0F
#define EASE_MOVE 1.0F

static void platform_destroy(struct task *task);
static void swing_step(struct task *task);
static void swing_draw(struct task *task);

/* the set editor's labels for the placement's fields, which no code reads.
 * the arrays' sizes pad each string but the last to a multiple of 4 bytes */
static const char rate_label[20] = "<- SPEED UD / FB";
static const char heading_label[20] = "<- ANG Y / RUG(  %)";
static const char spin_label[16] = "<- XROT SPEED";
static const char vertical_label[12] = "<- UD RANGE";
static const char radial_label[] = "<- FB RANGE";

NJS_TEXLIST *base_swing_texlists[] = {&base_swing_texlist, NULL};

void base_swing_init(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float x;
    Float z;
    Float radius;
    Float height;
    int zero;

    ORIGIN(task)->x = work->pos.x;
    ORIGIN(task)->y = work->pos.y;
    ORIGIN(task)->z = work->pos.z;
    radius = RADIAL_SWING(task->work) * task->work->scl.z;
    height = VERTICAL_SWING(task->work) * task->work->scl.y;
    ocean_polar_xz(YAW(task->work), radius, &x, &z);
    work->pos.x += x;
    work->pos.y += height;
    work->pos.z += z;
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = swing_draw;
    task->step = swing_step;
    task->destructor = platform_destroy;
    /* @hack spelling: zero, a local for the 0 that ang[0], ang[2] and timer
     * take, where 0 would do */
    zero = 0;
    object = world_pool_alloc();
    *object = swing_collision;
    task->motion.p = object;
    object->evalflags &= ~EVAL_UNITS;
    object->scl[0] = object->scl[2] = object->scl[1] = 1.0F;
    object->ang[0] = zero;
    object->ang[1] = YAW(task->work);
    object->ang[2] = zero;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(WORLD_SIZE_CLASS(1) | WORLD_TURNED | WORLD_MOVING | WORLD_SOLID,
              task, object);
    task->work->timer = zero;
    task->work->id = WORK_ID_SWING;
}

#include "stg23/platform_destroy_impl.h"

static void swing_step(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    NJS_POINT3 pos;
    NJS_POINT3 moved;
    NJS_POINT3 local;
    Angle pitch;
    Angle level;
    struct angles rotation;
    /* @hack redundant: gap_above and gap_below, which nothing reads, sizing
     * the frame around old_rotation */
    Uint8 gap_above[12];
    struct angles old_rotation;
    Uint8 gap_below[24];
    Float x;
    Float z;
    Float radius;
    Float height;
    Float step;
    Float speed;
    /* @hack spelling: minimum, a copy of MIN_STEP, where MIN_STEP would do */
    Float minimum;

    if (placement_try_leave(task)) {
        return;
    }
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: without player 0's work, the
     * swing skips its step */
    if (player_work[0] == NULL) {
        return;
    }
#endif
    minimum = MIN_STEP;
    /* @hack spelling: speed, a local for SWING_SPEED, where SWING_SPEED would
     * do */
    speed = SWING_SPEED;
    /* @hack redundant: a block no path reaches, under a label no goto names */
    if (0) {
    unreached: /* NOLINT(clang-diagnostic-unused-label) */
        speed = -speed;
        TASK_FORCES_OF(task)->turn.z = 0;
    }
    object = task->motion.p;
    old_rotation.x = PITCH(task);
    old_rotation.y = YAW(task->work);
    /* @bug memory: the test reads player 0's work before it checks
     * player_work[0] for null. the reads at 0x8c504ccc and 0x8c504ce0 come
     * before the check at 0x8c504cf4 */
    if (njAbs((Sint16)player_work[0]->ang.x) < speed &&
        njAbs((Sint16)player_work[0]->ang.z) < speed && player_work[0] &&
        (player_work[0]->flags & PLAYER_WORK_ON_GROUND) && player_records[0] &&
        player_records[0]->ground_task &&
        player_records[0]->ground_task->work &&
        player_records[0]->ground_task->work->id == WORK_ID_SWING) {
        if (njAbs((Sint16)player_work[0]->ang.x) > LEVEL_TILT ||
            njAbs((Sint16)player_work[0]->ang.z) > LEVEL_TILT) {
            pitch = PITCH(task);
            level = (Uint16)(pitch + EIGHTH_TURN) / QUARTER_TURN * QUARTER_TURN;
            pitch = angle_interpolate(level, pitch, EASE_KEEP, EASE_MOVE);
            task->work->timer = pitch;
        }
    } else {
        step =
            njAbs(njSin(PITCH(task) + task->work->ang.z) - njSin(PITCH(task)));
        if (task->work->ang.z && step < MIN_STEP) {
            step = minimum;
        }
        /* @hack redundant: an empty block under a test that can't hold */
        if (0 && speed < MIN_STEP) {
        }
        pitch = PITCH(task) + (speed * step);
        task->work->timer = pitch;
    }
    radius = RADIAL_SWING(task->work) * task->work->scl.z;
    height = (VERTICAL_SWING(task->work) - 1.0F) * task->work->scl.y;
    ocean_polar_xz(YAW(task->work), radius, &x, &z);
    pos.x = ORIGIN(task)->x + x;
    pos.y = ORIGIN(task)->y + height;
    pos.z = ORIGIN(task)->z + z;

    /* player 0's point, carried from the swing's old frame to its new one,
     * lands in moved. the turn between the frames lands in rotation. the
     * world object takes the new frame's angles */
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateX(NULL, -old_rotation.x);
    njRotateY(NULL, -old_rotation.y);
    njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
    njCalcPoint(NULL, &player_work[0]->pos, &local);
    njRotateY(NULL, YAW(task->work));
    njRotateX(NULL, PITCH(task));
    current_matrix_angles_yxz(&rotation.x, &rotation.y, &rotation.z);
    njPopMatrixEx();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&pos);
    njRotateY(NULL, YAW(task->work));
    njRotateX(NULL, PITCH(task));
    njCalcPoint(NULL, &local, &moved);
    current_matrix_angles_xyz(&object->ang[0], &object->ang[1],
                              &object->ang[2]);
    njPopMatrixEx();
    moved.x -= player_work[0]->pos.x;
    moved.y -= player_work[0]->pos.y;
    moved.z -= player_work[0]->pos.z;
    if (player_work[0]->mode == PLAYER_MODE_HANG) {
        TASK_FORCES_OF(task)->speed.x = TASK_FORCES_OF(task)->speed.y =
            TASK_FORCES_OF(task)->speed.z = 0.0F;
        TASK_FORCES_OF(task)->turn.x = TASK_FORCES_OF(task)->turn.y =
            TASK_FORCES_OF(task)->turn.z = 0;
    } else {
        TASK_FORCES_OF(task)->speed = moved;
        TASK_FORCES_OF(task)->turn.x = rotation.x;
        TASK_FORCES_OF(task)->turn.y = -rotation.y;
        TASK_FORCES_OF(task)->turn.z = rotation.z;
    }
    work->pos = pos;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    if (player_near((NJS_POINT3 *)object->pos, CONTACT_RADIUS)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void swing_draw(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    Float radius;
    Float height;

    /* @hack redundant: a block no path reaches, which offsets pos from the
     * origin as base_swing_init does */
    if (0) {
        radius = RADIAL_SWING(task->work) * task->work->scl.z;
        height = VERTICAL_SWING(task->work) * task->work->scl.y;
        njTranslate(NULL, radius, height, 0.0F);
        work->pos.y += height;
        njRotateY(NULL, YAW(task->work));
    }
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, YAW(task->work));
    njRotateX(NULL, PITCH(task));
    njSetTexture(&base_swing_texlist);
    njCnkEasyDrawModel(&swing_model);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
