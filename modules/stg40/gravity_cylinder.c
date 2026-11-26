/* the gravity cylinders, FCG_CYL0 and FCG_CYL1 in Final Chase's object list,
 * and FCG_CYL_PARTS and FCG_CYL_SPARK.
 *
 * a cylinder's field is its child task. the field measures player 0 by the
 * center of player 0's first collision shape. a field claims player 0 at each
 * step in which that center lies inside it. the manager keeps the lowest
 * number claimed in a step. the field of that number holds player 0 at the
 * next step. only the holder points world_gravity, and only while no field
 * has carried player 0 earlier in the step.
 *
 * a field's count runs up by a step while the center lies between the
 * field's two radii. the step counts only where the field holds player 0, or
 * a field carried player 0 in it. any other step zeroes the count, and
 * the count stops at COUNT_MAX. while it isn't 0, the inside reaches
 * FIELD_TOP_REACH past the shell's top end. above LINE_AFTER, the field's
 * shape is a SHAPE_ID_LINE line along the axis. the line is on only in a step
 * in which the field points world_gravity */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "draw/texture_anim.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "sound/sound.h"
#include "stg40/gravity_cylinder.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"

#include "stg40/gravity_cylinder/text.inc"

/* the manager's task keeps two claims in unk_3c, each a field's number or
 * NO_CLAIM. the low CLAIM_BITS hold the lowest of this step. the high half,
 * HOLDER_CLAIM, holds the lowest of the step before */
#define CLAIMS(task) ((task)->unk_3c.u)
#define CLAIM_BITS 16
#define HOLDER_CLAIM 0xffff0000
/* the claim of a step in which no field claimed player 0. its low byte is 0,
 * so field 0 holds player 0 after such a step */
#define NO_CLAIM 0x100
/* gravity_cylinder_holder's result without a manager. field 255 then holds
 * player 0 */
#define NO_MANAGER 255
#define MANAGER_LEVEL 0

/* the field keeps its number, its turn and its count in its work */
#define NUMBER(work) ((work)->submode)
#define TURN(work) ((work)->timer)
#define COUNT(work) ((work)->tick)
/* a cylinder's number is the low byte of its ang.y. the field's init clears
 * that byte, and ang.z's, from its turn. the cylinder's models and FCG_CYL1's
 * lid turn by the next byte of ang.y alone */
#define NUMBER_BITS 0xff
#define MODEL_TURN_BITS 0xff00

/* the shell's radius and half height, which a cylinder gives its field in
 * the field's scl.x and scl.y */
#define SHELL_RADIUS 30.0F
#define SHELL_HALF_HEIGHT 78.0F
/* the field's inner radius is FIELD_INSIDE less than the shell's. the inside
 * stops FIELD_END_GAP short of either end of the shell */
#define FIELD_INSIDE 5.0F
#define FIELD_END_GAP 6.0F
#define FIELD_TOP_REACH 13.5F
/* the carry turns player 0 round the axis by the field's turn. it also pulls
 * player 0 CARRY_PULL along world_gravity */
#define CARRY_PULL 0.1F
#define COUNT_MAX 100
#define LINE_AFTER 1
/* the field plays the hum while a player is within HUM_RANGE of the cylinder.
 * each play lasts HUM_DURATION + 1 sound updates. the hum sounds at the point
 * of the axis, within the shell, nearest player 0. every field plays it under
 * one key */
#define HUM_SOUND 0
#define HUM_RANGE 300.0F
#define HUM_PRIORITY 1
#define HUM_VOLUME 0
#define HUM_DURATION 80
/* a cylinder's model turns MODEL_TURN_SHARE of ang.x a step */
#define MODEL_TURN_SHARE 0.1F
/* the shell's texture is set by halfword SHELL_TID of its polygon list, the
 * second of its NJD_CT_TID chunk. the draw keeps that halfword's filter mode,
 * its NJD_FFM_MASK bits, and replaces the rest with a texture index. the index
 * moves on every 1 << SHELL_TEXTURE_SHIFT steps. it runs through
 * SHELL_TEXTURES textures from SHELL_0_FIRST_TEXTURE or SHELL_1_FIRST_TEXTURE,
 * miu64_fcanime01 to miu64_fcanime08 of each list */
#define SHELL_TID 7
#define SHELL_TEXTURE_SHIFT 1
#define SHELL_TEXTURES 8
#define SHELL_0_FIRST_TEXTURE 7
#define SHELL_1_FIRST_TEXTURE 8
/* FCG_CYL1's lid stands LID_HEIGHT up the cylinder's axis from pos */
#define LID_HEIGHT 73.0F
/* the spark plays SPARK_SOUND at each step, for SPARK_DURATION + 1 sound
 * updates */
#define SPARK_RANGE 150.0F
#define SPARK_SOUND 1
#define SPARK_PRIORITY 1
#define SPARK_VOLUME 0
#define SPARK_DURATION 30
/* the spark's parts' strips are NJD_CS_UVN, whose u and v count 256ths of a
 * texture. each part's texture scrolls SPARK_SCROLL a step */
#define SPARK_SCROLL 32

#define CYLINDER_SPARK_POINTS 10
struct cylinder_spark_vertices {
    Sint32 head[2];
    struct chunk_vertex_d8 vertices[CYLINDER_SPARK_POINTS];
    Sint32 end;
};

/* the objects' own texture lists, models and scrolls. a cylinder draws its
 * body, its shell and its rings. FCG_CYL1's lid, an NJS_OBJECT, goes only
 * into the collision world */
static NJS_TEXLIST gravity_cylinder_0_texlist;
static NJS_TEXLIST gravity_cylinder_1_texlist;
static NJS_TEXLIST gravity_cylinder_parts_texlist;
static NJS_TEXLIST gravity_cylinder_spark_texlist;
static NJS_TEXLIST gravity_cylinder_spark_1_texlist;
static NJS_TEXLIST gravity_cylinder_spark_2_texlist;
static NJS_TEXLIST gravity_cylinder_spark_3_texlist;
static NJS_TEXLIST gravity_cylinder_spark_4_texlist;
static NJS_CNK_OBJECT gravity_cylinder_0_shell[1];
static NJS_CNK_OBJECT gravity_cylinder_0_rings[1];
static NJS_CNK_OBJECT gravity_cylinder_0_body[1];
static NJS_CNK_OBJECT gravity_cylinder_1_rings[1];
static NJS_CNK_OBJECT gravity_cylinder_1_shell[1];
static NJS_CNK_OBJECT gravity_cylinder_1_body[1];
static NJS_CNK_OBJECT gravity_cylinder_parts_object[1];
static NJS_CNK_OBJECT gravity_cylinder_spark_object[1];
static NJS_CNK_OBJECT gravity_cylinder_spark_1_object;
static NJS_CNK_OBJECT gravity_cylinder_spark_2_object;
static NJS_CNK_OBJECT gravity_cylinder_spark_3_object;
static NJS_CNK_OBJECT gravity_cylinder_spark_4_object;
static struct texture_anim gravity_cylinder_spark_1_anim;
static struct texture_anim gravity_cylinder_spark_2_anim;
static struct texture_anim gravity_cylinder_spark_3_anim;
static struct texture_anim gravity_cylinder_spark_4_anim;
static NJS_OBJECT gravity_cylinder_1_lid;
/* the vertex list of a model nothing draws. its CYLINDER_SPARK_POINTS
 * NJD_CV_D8 vertices are where the spark's spheres stand */
static struct cylinder_spark_vertices gravity_cylinder_spark_points;

static struct collision_shape field_shape;
static struct task *manager;
static Bool carried;
static NJS_VECTOR axis;
static Angle cylinder_0_ring_speed;
static Angle cylinder_1_ring_speed;
static struct collision_shape parts_shape;
static struct collision_shape spark_shapes[1];
static NJS_VECTOR spark_facing;

static void cylinder_field_step(struct task *task);
static void cylinder_field_draw(struct task *task);
static void cylinder_field_destroy(struct task *task);

static void cylinder_manager_destroy(struct task *task)
{
    manager = NULL;
}

static void cylinder_manager_step(struct task *task)
{
    if ((Uint16)CLAIMS(manager) >= NO_CLAIM) {
        world_reset_gravity();
    }
    CLAIMS(manager) = (CLAIMS(manager) << CLAIM_BITS) | NO_CLAIM;
    carried = FALSE;
}

/* the number of the field that holds player 0, the lowest claim of the step
 * before. the result is NO_CLAIM where no field claimed player 0, and
 * NO_MANAGER without a manager */
static int gravity_cylinder_holder(void)
{
    if (manager) {
        /* @hack spelling: the high half masked before the shift, where the
         * shift alone would do */
        return (Sint16)((CLAIMS(manager) & HOLDER_CLAIM) >> CLAIM_BITS);
    }
    return NO_MANAGER;
}

static void gravity_cylinder_claim(Uint8 number)
{
    struct task *task = manager;
    int claim;

    if (task) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((Uint16)CLAIMS(task) > (claim = number)) {
            CLAIMS(task) = (CLAIMS(task) & HOLDER_CLAIM) | claim;
        }
    }
}

static void cylinder_manager_start(void)
{
    struct task *task;

    if (!manager) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((task = task_create(0, MANAGER_LEVEL, cylinder_manager_step,
                                "ManGCylExecutor")) != 0) {
            task->destructor = cylinder_manager_destroy;
            manager = task;
            CLAIMS(manager) = (NO_CLAIM << CLAIM_BITS) | NO_CLAIM;
        }
    }
}

/* task's work must hold its cylinder's, with the shell's radius and half
 * height in scl.x and scl.y. the init moves the number and the turn out of
 * the angles */
static void cylinder_field_init(struct task *task)
{
    struct task_work *work = task->work;

    cylinder_manager_start();
    NUMBER(work) = work->ang.y;
    TURN(work) = work->ang.x;
    work->ang.x = 0;
    work->ang.y = work->ang.y & ~NUMBER_BITS;
    work->ang.z = work->ang.z & ~NUMBER_BITS;
    collision_init(task, &field_shape, 1, COLLISION_OBJECT);
    /* @bug memory: the init reads the collision record through null where
     * collision_init failed, at 0x8c502dc6. the step tests the record before
     * it reads it, at 0x8c5031f8 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it destroys the field where
     * collision_init failed */
    if (!work->collision) {
        task_destroy(task);
        return;
    }
#endif
    work->collision->shapes->radius = work->scl.x;
    work->collision->shapes->size_y = work->scl.y;
    collision_fit_record_radius(work->collision);
    task->step = cylinder_field_step;
    task->draw = cylinder_field_draw;
    task->destructor = cylinder_field_destroy;
}

static void cylinder_field_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    Float radius;
    Float half_height;

    /* @hack redundant: a block no path reaches. SHC still places its strings
     * in this source's constants, and saves registers and a frame for its
     * locals. the image holds none of the block's code. its statements stand
     * in only to keep the registers and the frame SHC saves */
    if (0) {
        (void)"<- ROT SPEED";
        (void)"<- ANG Y : ID";
        (void)"<- ANG Z";
        (void)"<- R(For Gravity)";
        (void)"<- ANG X";
        (void)"<- ANG Y";
        radius = work->scl.x;
        half_height = work->scl.y;
        pos = work->pos;
        njPushMatrixEx();
        njTranslateEx(&pos);
        njRotateY(NULL, work->ang.y);
        pos.x = radius;
        pos.y = half_height;
        njTranslateEx(&pos);
        njPopMatrixEx();
        pos.x = radius * half_height;
        njTranslateEx(&pos);
    }
}

static void cylinder_field_destroy(struct task *task)
{
}

static void cylinder_field_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 local;
    NJS_POINT3 hum;
    NJS_VECTOR up;
    NJS_VECTOR inward;
    /* @hack redundant: a vector nothing reads */
    NJS_VECTOR unused;
    NJS_VECTOR dir;
    NJS_VECTOR carry;
    Float distance;
    Angle turn;
    Bool within = FALSE;
    Bool holds = FALSE;
    Bool turned = FALSE;

    /* @hack redundant: a block whose test always holds */
    if (1) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njRotateZ(NULL, work->ang.z);
        njCalcVector(NULL, &axis, &up);
        njPushMatrixEx();
        njInvertMatrix(NULL);
        njCalcPoint(NULL, &player_work[0]->collision->shapes->center, &local);
        hum.y = local.y;
        hum.z = 0.0F;
        hum.x = 0.0F;
        if (hum.y > work->scl.y) {
            hum.y = work->scl.y;
        }
        if (-work->scl.y > hum.y) {
            hum.y = -work->scl.y;
        }
        inward.x = -local.x;
        inward.y = 0.0F;
        inward.z = -local.z;
        distance = njScalor(&inward);
        if (_builtin_fabsf(work->scl.z) > distance) {
            if (distance > _builtin_fabsf(work->scl.x) - FIELD_INSIDE) {
                within = TRUE;
            }
        }
        holds = (Uint8)gravity_cylinder_holder() == (Uint8)NUMBER(work);
        if ((_builtin_fabsf(local.y) <=
                 _builtin_fabsf(work->scl.y) - FIELD_END_GAP ||
             (COUNT(work) && local.y > 0.0F &&
              local.y <= _builtin_fabsf(work->scl.y) + FIELD_TOP_REACH)) &&
            within) {
            gravity_cylinder_claim(NUMBER(work));
            if (holds && !carried) {
                njPushMatrixEx();
                njInvertMatrix(NULL);
                njCalcVector(NULL, &inward, &dir);
                njUnitVector(&dir);
                world_gravity = dir;
                world_gravity_ang_x = RADIANS(asinf(-dir.z));
                world_gravity_ang_z = -RADIANS(atan2f(-dir.x, -dir.y));
                if (player_tasks[0] && player_records[0] &&
                    collision_next_player(task) == player_tasks[0]) {
                    turn = TURN(work);
                    carry.x = (local.x * (njCos(turn) - 1.0F)) -
                              (local.z * njSin(turn));
                    carry.y = 0.0F;
                    carry.z = (local.x * njSin(turn)) +
                              (local.z * (njCos(turn) - 1.0F));
                    njCalcVector(NULL, &carry, &carry);
                    carry.x += CARRY_PULL * world_gravity.x;
                    carry.y += CARRY_PULL * world_gravity.y;
                    carry.z += CARRY_PULL * world_gravity.z;
                    njAddVector(&player_records[0]->push, &carry);
                    carried = TRUE;
                }
                turned = TRUE;
                njPopMatrixEx();
            }
        }
        njPopMatrixEx();
        njCalcPoint(NULL, &hum, &hum);
        if (player_near(&work->pos, HUM_RANGE)) {
            sound_play_at_for(HUM_SOUND, (void *)cylinder_field_step,
                              HUM_PRIORITY, HUM_VOLUME, HUM_DURATION, &hum);
        }
        njPopMatrixEx();
    }
    if ((holds || carried) && within) {
        if (COUNT(work) < COUNT_MAX) {
            COUNT(work)++;
        }
    } else {
        COUNT(work) = 0;
    }
    if (work->collision) {
        if (COUNT(work) > LINE_AFTER) {
            work->collision->shapes->id = SHAPE_ID_LINE;
            if (turned) {
                work->collision->shapes->flags &= ~SHAPE_OFF;
            } else {
                work->collision->shapes->flags |= SHAPE_OFF;
            }
        } else {
            work->collision->shapes->id = 0;
            work->collision->shapes->flags &= ~SHAPE_OFF;
        }
    }
    collision_register(task);
}

static void gravity_cylinder_0_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack type: the turn's speed a union of an Angle and its low half,
     * where an Angle of a tenth of ang.x would do. ang.x holds the set
     * entry's Sint16, so the two forms differ only for an ang.x from -9 to
     * -1, which the union makes -65536 and the plain form 0 */
    union {
        Angle angle;
        Sint16 low;
    } speed;
    Float scaled;
    Sint16 *low;
    int texture;

    speed.angle = work->ang.x;
    low = &speed.low;
    scaled = *low;
    scaled *= MODEL_TURN_SHARE;
    *low = scaled;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y & MODEL_TURN_BITS);
    njRotateZ(NULL, work->ang.z);
    /* @hack spelling: speed read through its address, where speed.angle
     * would do */
    njRotateY(NULL, -(&speed)->angle * game_state.steps);
    njSetTexture(&gravity_cylinder_0_texlist);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(gravity_cylinder_0_body[0].model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    /* @hack spelling: texture, a copy of the polygon list's texture word,
     * where the word changed in place would do */
    texture = gravity_cylinder_0_shell->model->plist[SHELL_TID];
    texture =
        ((texture & NJD_FFM_MASK) |
         ((game_state.steps >> SHELL_TEXTURE_SHIFT) & (SHELL_TEXTURES - 1))) +
        SHELL_0_FIRST_TEXTURE;
    gravity_cylinder_0_shell->model->plist[SHELL_TID] = texture;
    njCnkEasyDrawModel(gravity_cylinder_0_shell[0].model);
    njRotateY(NULL, game_state.steps * cylinder_0_ring_speed);
    njCnkEasyDrawModel(gravity_cylinder_0_rings[0].model);
    njPopMatrixEx();
}

void gravity_cylinder_0_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *child_work;
    struct task *child;

    if (!placement_try_leave(task)) {
        /* @hack spelling: radius and half_height, locals for the shell's
         * size, where the constants in place would do */
        Float radius = SHELL_RADIUS;
        Float half_height = SHELL_HALF_HEIGHT;

        if (!task->child) {
            child = task_create_child(TASK_WORK, cylinder_field_init, task);
            if (child) {
                /* @hack spelling: the child's work read through task->child,
                 * where child->work would do */
                child_work = task->child->work;
                child_work->pos = work->pos;
                child_work->scl = work->scl;
                child_work->ang = work->ang;
                child_work->scl.x = radius;
                child_work->scl.y = half_height;
            }
            task->draw = gravity_cylinder_0_draw;
        }
        /* @hack redundant: the test of task->child and the read of its work,
         * which nothing uses */
        if (task->child) {
            child_work = task->child->work;
            /* @hack redundant: a block no path reaches, which reads
             * child_work */
            if (0) {
                if (child_work) {
                }
            }
        }
    }
}

static void gravity_cylinder_1_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack type: the turn's speed a union of an Angle and its low half,
     * where an Angle of a tenth of ang.x would do. ang.x holds the set
     * entry's Sint16, so the two forms differ only for an ang.x from -9 to
     * -1, which the union makes -65536 and the plain form 0 */
    union {
        Angle angle;
        Sint16 low;
    } speed;
    Float scaled;
    Sint16 *low;
    int texture;

    speed.angle = work->ang.x;
    low = &speed.low;
    scaled = *low;
    scaled *= MODEL_TURN_SHARE;
    *low = scaled;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y & MODEL_TURN_BITS);
    njRotateZ(NULL, work->ang.z);
    /* @hack spelling: speed read through its address, where speed.angle
     * would do */
    njRotateY(NULL, -(&speed)->angle * game_state.steps);
    njSetTexture(&gravity_cylinder_1_texlist);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(gravity_cylinder_1_body[0].model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    /* @hack spelling: texture, a copy of the polygon list's texture word,
     * where the word changed in place would do */
    texture = gravity_cylinder_1_shell->model->plist[SHELL_TID];
    texture =
        ((texture & NJD_FFM_MASK) |
         ((game_state.steps >> SHELL_TEXTURE_SHIFT) & (SHELL_TEXTURES - 1))) +
        SHELL_1_FIRST_TEXTURE;
    gravity_cylinder_1_shell->model->plist[SHELL_TID] = texture;
    njCnkEasyDrawModel(gravity_cylinder_1_shell[0].model);
    njRotateY(NULL, game_state.steps * cylinder_1_ring_speed);
    njCnkEasyDrawModel(gravity_cylinder_1_rings[0].model);
    njPopMatrixEx();
}

static void gravity_cylinder_1_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        /* @hack redundant: a test of work, in a block no path reaches */
        if (0) {
            if (work) {
            }
        }
        /* @hack redundant: the same test again, in a block of its own */
        if (0) {
            if (work) {
            }
        }
        collision_register(task);
    }
}

void gravity_cylinder_1_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *child;
    struct task_work *child_work;
    NJS_POINT3 pos;
    struct angles ang;

    if (!placement_try_leave(task)) {
        child = task_create_child(TASK_WORK, cylinder_field_init, task);
        if (child) {
            /* @hack spelling: the child's work read through task->child, where
             * child->work would do */
            child_work = task->child->work;
            child_work->pos = work->pos;
            child_work->scl = work->scl;
            child_work->ang = work->ang;
            child_work->scl.x = SHELL_RADIUS;
            child_work->scl.y = SHELL_HALF_HEIGHT;
            pos.x = 0.0F;
            pos.y = LID_HEIGHT;
            pos.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y & MODEL_TURN_BITS);
            njRotateZ(NULL, work->ang.z);
            current_matrix_angles_xyz(&ang.x, &ang.y, &ang.z);
            njCalcPoint(NULL, &pos, &pos);
            njPopMatrixEx();
            solid_object_create_at(child, &pos, &ang, NULL, NULL, NULL, 0,
                                   &gravity_cylinder_1_lid, OBJECT_TURNED, 0);
        }
        task->draw = gravity_cylinder_1_draw;
        task->step = gravity_cylinder_1_step;
    }
}

static void cylinder_parts_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&gravity_cylinder_parts_texlist);
    njCnkEasyDrawModel(gravity_cylinder_parts_object->model);
    njPopMatrixEx();
}

static void cylinder_parts_destroy(struct task *task)
{
}

/* @hack redundant: task declared register, where a plain parameter would do */
static void cylinder_parts_step(register struct task *task)
{
    /* @hack redundant: a call no path makes */
    if (0) {
        cylinder_parts_draw(task);
    }
    collision_register(task);
}

void gravity_cylinder_parts_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->step = cylinder_parts_step;
        task->draw = cylinder_parts_draw;
        task->destructor = cylinder_parts_destroy;
        collision_init(task, &parts_shape, 1, COLLISION_OBJECT);
    }
}

static void cylinder_spark_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR facing;
    Angle angle;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njSetTexture(&gravity_cylinder_spark_texlist);
    njCnkEasyDrawModel(gravity_cylinder_spark_object->model);
    njSetTexture(&gravity_cylinder_spark_1_texlist);
    njTranslateEx((NJS_VECTOR *)gravity_cylinder_spark_1_object.pos);
    njRotateEx(gravity_cylinder_spark_1_object.ang, 0);
    texture_anim_scroll(
        gravity_cylinder_spark_1_object.model, &gravity_cylinder_spark_1_anim,
        (game_state.steps * SPARK_SCROLL) & (SCROLL_WRAP - 1), 0);
    njCnkEasyDrawModel(gravity_cylinder_spark_1_object.model);
    matrix_load_parent();
    njSetTexture(&gravity_cylinder_spark_2_texlist);
    njTranslateEx((NJS_VECTOR *)gravity_cylinder_spark_2_object.pos);
    njRotateEx(gravity_cylinder_spark_2_object.ang, 0);
    texture_anim_scroll(
        gravity_cylinder_spark_2_object.model, &gravity_cylinder_spark_2_anim,
        (game_state.steps * SPARK_SCROLL) & (SCROLL_WRAP - 1), 0);
    njCalcVector(NULL, &spark_facing, &facing);
    angle = RADIANS(atan2f(facing.z, facing.y));
    njRotateX(NULL, QUARTER_TURN + angle);
    njCnkEasyDrawModel(gravity_cylinder_spark_2_object.model);
    matrix_load_parent();
    njSetTexture(&gravity_cylinder_spark_3_texlist);
    njTranslateEx((NJS_VECTOR *)gravity_cylinder_spark_3_object.pos);
    njRotateEx(gravity_cylinder_spark_3_object.ang, 0);
    texture_anim_scroll(
        gravity_cylinder_spark_3_object.model, &gravity_cylinder_spark_3_anim,
        (game_state.steps * SPARK_SCROLL) & (SCROLL_WRAP - 1), 0);
    njCalcVector(NULL, &spark_facing, &facing);
    angle = RADIANS(atan2f(facing.z, facing.y));
    njRotateX(NULL, QUARTER_TURN + angle);
    njCnkEasyDrawModel(gravity_cylinder_spark_3_object.model);
    matrix_load_parent();
    njSetTexture(&gravity_cylinder_spark_4_texlist);
    njTranslateEx((NJS_VECTOR *)gravity_cylinder_spark_4_object.pos);
    njRotateEx(gravity_cylinder_spark_4_object.ang, 0);
    texture_anim_scroll(
        gravity_cylinder_spark_4_object.model, &gravity_cylinder_spark_4_anim,
        (game_state.steps * SPARK_SCROLL) & (SCROLL_WRAP - 1), 0);
    njCalcVector(NULL, &spark_facing, &facing);
    angle = RADIANS(atan2f(facing.z, facing.y));
    njRotateX(NULL, QUARTER_TURN + angle);
    njCnkEasyDrawModel(gravity_cylinder_spark_4_object.model);
    njPopMatrix(2);
}

static void cylinder_spark_destroy(struct task *task)
{
}

static void cylinder_spark_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: the points walked by a byte pointer, where a pointer
     * to struct chunk_vertex_d8 would do */
    unsigned char *cursor;
    struct task *child;
    NJS_POINT3 pos;
    int i;
    /* @hack spelling: point, a local for the point the cursor holds, where
     * that point in the call would do */
    NJS_POINT3 *point;

    if (player_near(&work->pos, SPARK_RANGE)) {
        if (!task->child) {
            cursor = (unsigned char *)&gravity_cylinder_spark_points;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, work->ang.y);
            cursor += sizeof(gravity_cylinder_spark_points.head);
            for (i = CYLINDER_SPARK_POINTS; i != 0; i--) {
                point = &((struct chunk_vertex_d8 *)cursor)->point;
                njCalcPoint(NULL, point, &pos);
                child =
                    model_object_create(task, NULL, NULL, spark_shapes, 1, 0);
                if (child) {
                    child->work->pos = pos;
                }
                cursor += sizeof(struct chunk_vertex_d8);
            }
            njPopMatrixEx();
        }
    } else if (task->child) {
        task_destroy_children(task);
    }
    sound_play_at_for(SPARK_SOUND, task, SPARK_PRIORITY, SPARK_VOLUME,
                      SPARK_DURATION, &work->pos);
}

void gravity_cylinder_spark_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->step = cylinder_spark_step;
        task->draw = cylinder_spark_draw;
        task->destructor = cylinder_spark_destroy;
        collision_init(task, spark_shapes, 1, COLLISION_OBJECT);
    }
}

/* clang-format off */
#include "stg40/cylinder_art/first.h"
#include "stg40/cylinder_art/second.h"
#include "stg40/cylinder_art/parts.h"
#include "stg40/cylinder_art/lid.h"
#include "stg40/cylinder_art/spark.h"
#include "stg40/cylinder_art/field.h"
#include "stg40/cylinder_art/spark_1.h"
#include "stg40/cylinder_art/spark_2.h"
#include "stg40/cylinder_art/spark_3.h"
#include "stg40/cylinder_art/spark_4.h"
#include "stg40/cylinder_art/anim.h"
/* clang-format on */

NJS_TEXLIST *gravity_cylinder_texlists[9] = {
#include "stg40/gravity_cylinder/gravity_cylinder_texlists.inc"
};
static NJS_CNK_OBJECT *spark_parts[4] = {
#include "stg40/gravity_cylinder/spark_parts.inc"
};
/* the field's shape, whose radius and half height its init sets */
/* clang-format off */
static struct collision_shape field_shape = {
    SHAPE_ID_LINE, SHAPE_CYLINDER2,
    PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_HURTS_ALL,
    SHAPE_TURN_YZX | SHAPE_UNK_2000 | SHAPE_IGNORE_OBJECTS |
        SHAPE_IGNORE_PICKUPS,
    {0.0F, 0.0F, 0.0F}, 1.0F, 1.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */
/* the manager's task, or null */
static struct task *manager = NULL;
/* TRUE once a field has carried player 0 since the manager's last step */
static Bool carried = FALSE;
/* the field's axis, in its own space */
static NJS_VECTOR axis = {0.0F, 1.0F, 0.0F};
/* the rings' turn, in Angle units a step */
static Angle cylinder_0_ring_speed = 0x200;
static Angle cylinder_1_ring_speed = 0x200;
/* clang-format off */
static struct collision_shape parts_shape = {
    0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
    0, {36.0F, 0.0F, 0.0F}, 8.0F, 23.0F, 0.0F, 0, {0, 0, 0},
};
/* the sphere at each of the spark's points */
/* @hack type: spark_shapes an array of one, where the record would do */
static struct collision_shape spark_shapes[1] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_ALL, SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 12.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static NJS_VECTOR spark_facing = {0.0F, 1.0F, 0.0F};
