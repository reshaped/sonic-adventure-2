/* the big jump, BIGJUMP in a stage's object list: a platform that launches
 * a player standing on it */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "draw/texture_anim.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/big_jump.h"
#include "shared/common_object.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* a big jump's modes, in its work's mode */
enum big_jump_mode { MODE_INIT, MODE_READY };

/* how far past its shape's model radius a player counts as near */
#define REACH_MARGIN 32.0F
/* the radius of the ball the draw tests against the camera */
#define DRAW_RADIUS 30.0F
/* the speed up a launch adds to scl.z */
#define LAUNCH_UP_SPEED 3.2F
#define LAUNCH_SOUND 0x1003
#define VIBRATION_POWER 4
#define VIBRATION_FREQ 15
/* the steps after a player's first launch that launch it again */
#define LAUNCH_REPEATS 20

/* the launches left for players 0 and 1. a big jump has no motion record, so
 * task->motion holds the counts in place of a pointer. the destructor clears
 * task->motion, so task_destroy frees nothing */
/* @hack type: motion read as bytes through its address, where
 * (task)->motion.b would do */
#define LAUNCHES_LEFT(task) ((Sint8 *)&(task)->motion)

#include "shared/big_jump/text.inc"

/* The launch ramp, scrolling arrows and its two collision meshes. */
static NJS_TEXNAME big_jump_anim_textures[1] = {
#include "shared/big_jump/big_jump_anim_textures.inc"
};

NJS_TEXLIST big_jump_anim_texlist = {
#include "shared/big_jump/big_jump_anim_texlist.inc"
};

static Sint16 big_jump_anim_polygons[51] = {
#include "shared/big_jump/big_jump_anim_polygons.inc"
};

static Sint32 big_jump_anim_vertices[35] = {
#include "shared/big_jump/big_jump_anim_vertices.inc"
};

static NJS_CNK_MODEL big_jump_anim_model = {
#include "shared/big_jump/big_jump_anim_model.inc"
};

static NJS_CNK_OBJECT big_jump_anim_object = {
#include "shared/big_jump/big_jump_anim_object.inc"
};

/* Three strips, each with a polygon offset and its four starting UV pairs. */

static Sint16 big_jump_scroll_uvs[32] = {
#include "shared/big_jump/big_jump_scroll_uvs.inc"
};

static struct texture_anim big_jump_anim = {
#include "shared/big_jump/big_jump_anim.inc"
};

static NJS_TEXNAME big_jump_textures[2] = {
#include "shared/big_jump/big_jump_textures.inc"
};

NJS_TEXLIST big_jump_texlist = {
#include "shared/big_jump/big_jump_texlist.inc"
};

static Sint16 big_jump_polygons[237] = {
#include "shared/big_jump/big_jump_polygons.inc"
};

static Sint32 big_jump_vertices[171] = {
#include "shared/big_jump/big_jump_vertices.inc"
};

static NJS_CNK_MODEL big_jump_model = {
#include "shared/big_jump/big_jump_model.inc"
};

static NJS_CNK_OBJECT big_jump_object = {
#include "shared/big_jump/big_jump_object.inc"
};

static NJS_TEXNAME big_jump_floor_textures[2] = {
#include "shared/big_jump/big_jump_floor_textures.inc"
};

static NJS_TEXLIST big_jump_floor_texlist = {
#include "shared/big_jump/big_jump_floor_texlist.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL big_jump_floor_materials[4] = {
#include "shared/big_jump/big_jump_floor_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 big_jump_floor_indices_0[57] = {
#include "shared/big_jump/big_jump_floor_indices_0.inc"
};

static Sint16 big_jump_floor_indices_1[5] = {
#include "shared/big_jump/big_jump_floor_indices_1.inc"
};

static Sint16 big_jump_floor_indices_2[13] = {
#include "shared/big_jump/big_jump_floor_indices_2.inc"
};

static Sint16 big_jump_floor_indices_3[5] = {
#include "shared/big_jump/big_jump_floor_indices_3.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_COLOR big_jump_floor_uvs_0[46] = {
#include "shared/big_jump/big_jump_floor_uvs_0.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_COLOR big_jump_floor_uvs_1[4] = {
#include "shared/big_jump/big_jump_floor_uvs_1.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_COLOR big_jump_floor_uvs_2[10] = {
#include "shared/big_jump/big_jump_floor_uvs_2.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_COLOR big_jump_floor_uvs_3[4] = {
#include "shared/big_jump/big_jump_floor_uvs_3.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static NJS_MESHSET big_jump_floor_meshsets[4] = {
#include "shared/big_jump/big_jump_floor_meshsets.inc"
};

static NJS_POINT3 big_jump_floor_points[26] = {
#include "shared/big_jump/big_jump_floor_points.inc"
};

static NJS_VECTOR big_jump_floor_normals[26] = {
#include "shared/big_jump/big_jump_floor_normals.inc"
};

static NJS_MODEL big_jump_floor_model = {
#include "shared/big_jump/big_jump_floor_model.inc"
};

static NJS_OBJECT big_jump_collision = {
#include "shared/big_jump/big_jump_collision.inc"
};

static NJS_TEXNAME big_jump_face_textures[1] = {
#include "shared/big_jump/big_jump_face_textures.inc"
};

static NJS_TEXLIST big_jump_face_texlist = {
#include "shared/big_jump/big_jump_face_texlist.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL big_jump_face_materials[1] = {
#include "shared/big_jump/big_jump_face_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 big_jump_face_indices_0[15] = {
#include "shared/big_jump/big_jump_face_indices_0.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_COLOR big_jump_face_uvs_0[12] = {
#include "shared/big_jump/big_jump_face_uvs_0.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static NJS_MESHSET big_jump_face_meshsets[1] = {
#include "shared/big_jump/big_jump_face_meshsets.inc"
};

static NJS_POINT3 big_jump_face_points[8] = {
#include "shared/big_jump/big_jump_face_points.inc"
};

static NJS_VECTOR big_jump_face_normals[8] = {
#include "shared/big_jump/big_jump_face_normals.inc"
};

static NJS_MODEL big_jump_face_model = {
#include "shared/big_jump/big_jump_face_model.inc"
};

static NJS_OBJECT big_jump_collision_child = {
#include "shared/big_jump/big_jump_collision_child.inc"
};

static void big_jump_init(struct task *task);
static void big_jump_ready_step(struct task *task);
static void big_jump_draw(struct task *task);
static void big_jump_destroy(struct task *task);

void big_jump_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
        task->draw = big_jump_draw;
        task->destructor = big_jump_destroy;
        big_jump_init(task);
        break;
    case MODE_READY:
        big_jump_ready_step(task);
        break;
    default:
        work->mode = MODE_INIT;
        break;
    }
}

static void big_jump_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float radius;
    Uint32 flags;

    work->mode = MODE_READY;
    object = world_pool_alloc();
    task->extra.p = object;
    *object = big_jump_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->scl[0] = 1.0F;
    object->scl[1] = 1.0F;
    object->scl[2] = 1.0F;
    object->ang[0] = work->ang.x;
    object->ang[1] = work->ang.y;
    object->ang[2] = work->ang.z;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    /* @hack redundant: the model set again, though the copy of
     * big_jump_collision set it */
    object->model = big_jump_collision.model;
    object->child = &big_jump_collision_child;
    object->sibling = NULL;
    radius = object->model->r + REACH_MARGIN;
    flags = WORLD_SOLID | WORLD_UPRIGHT;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    LAUNCHES_LEFT(task)[0] = 0;
    LAUNCHES_LEFT(task)[1] = 0;
}

static void big_jump_launch(struct task *task, int player)
{
    struct task_work *work = task->work;
    int lock;
    struct angles ang;
    NJS_VECTOR spd;
    /* @hack redundant: unused, a point nothing reads, which only sizes the
     * stack frame */
    NJS_POINT3 unused;

    lock = ceilf(work->scl.y);
    spd.x = work->scl.x;
    spd.y = work->scl.z + LAUNCH_UP_SPEED;
    spd.z = 0.0F;
    ang.x = 0;
    ang.y = work->ang.y + HALF_TURN;
    ang.z = 0;
    if (lock < 0) {
        player_launch_speed(player, &spd, &ang, -lock);
    } else {
        fun_8c0492d0(player, &spd, &ang, lock);
    }
    sound_play(LAUNCH_SOUND, NULL, 0, 0);
    /* the count is still LAUNCH_REPEATS at the first launch and at the one
     * after it. the caller counts each repeat down only after its launch */
    if (LAUNCHES_LEFT(task)[player] == LAUNCH_REPEATS) {
        vibration_start(player, VIBRATION_POWER, VIBRATION_FREQ, 0);
    }
}

static void big_jump_ready_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object = task->extra.p;

    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    object->ang[0] = work->ang.x;
    object->ang[1] = work->ang.y;
    object->ang[2] = work->ang.z;
    if (player_near(&work->pos, object->model->r + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
        if (LAUNCHES_LEFT(task)[0] <= 0) {
            if (player_work[0] != NULL &&
                (player_work[0]->flags &
                 (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE)) &&
                player_records[0]->ground_task == task) {
                LAUNCHES_LEFT(task)[0] = LAUNCH_REPEATS;
                big_jump_launch(task, 0);
            }
        } else {
            big_jump_launch(task, 0);
            LAUNCHES_LEFT(task)[0]--;
        }
        if (LAUNCHES_LEFT(task)[1] <= 0) {
            if (player_work[1] != NULL &&
                (player_work[1]->flags &
                 (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE)) &&
                player_records[1]->ground_task == task) {
                LAUNCHES_LEFT(task)[1] = LAUNCH_REPEATS;
                big_jump_launch(task, 1);
            }
        } else {
            big_jump_launch(task, 1);
            LAUNCHES_LEFT(task)[1]--;
        }
    } else {
        work->flags &= ~WORLD_ADDED;
        LAUNCHES_LEFT(task)[0] = 0;
        LAUNCHES_LEFT(task)[1] = 0;
    }
}

static void big_jump_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)JUMP_ANGLE_X_LABEL;
        (void)JUMP_ANGLE_Y_LABEL;
        (void)JUMP_ANGLE_Z_LABEL;
        (void)JUMP_FORWARD_SPEED_LABEL;
        (void)JUMP_CONTROL_TIME_LABEL;
        (void)JUMP_UP_SPEED_LABEL;
    }

    if (sphere_in_front_no_depth(&work->pos, DRAW_RADIUS)) {
        njPushMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, work->ang.x);
        njSetTexture(&big_jump_texlist);
        njCnkEasyDrawModel(&big_jump_model);
        njSetTexture(&big_jump_anim_texlist);
        texture_anim_apply(&big_jump_anim_model, &big_jump_anim,
                           game_state.steps);
        njCnkEasyDrawModel(&big_jump_anim_model);
        njPopMatrix(1);
    }
}

static void big_jump_destroy(struct task *task)
{
    world_remove(task, task->extra.p);
    world_pool_free(task->extra.p);
    task->extra.p = NULL;
    task->motion.p = NULL;
}
