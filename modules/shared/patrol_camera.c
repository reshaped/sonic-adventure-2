/* the patrol cameras, CAMV and CAMH, in Eternal Engine's object list. Lost
 * Colony's are this source compiled again with COLONY_PATROL_CAMERA */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "effect/particle.h"
#include "hud/hud.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/patrol_camera.h"
#include "shared/patrol_camera_tex.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* the set editor's labels, which no code reads */
static const char editor_reverse[16] = {
#include "shared/patrol_camera/editor_reverse.inc"
};

static const char editor_normal[16] = {
#include "shared/patrol_camera/editor_normal.inc"
};

static const char editor_heading[12] = {
#include "shared/patrol_camera/editor_heading.inc"
};

static const char editor_sway_speed[12] = {
#include "shared/patrol_camera/editor_sway_speed.inc"
};

static const char editor_sway_width[16] = {
#include "shared/patrol_camera/editor_sway_width.inc"
};

#define BREAK_POINTS 20
#define BREAK_SOUND 1
#define FALL_FLAGS (DEBRIS_TRAIL | DEBRIS_LANDING_BURST | DEBRIS_VANISH)
/* the falling head's upward speed as it starts */
#define HEAD_RISE 0.2F
/* a lamp's tint runs from LAMP_DIM to LAMP_BRIGHT. CAMV's lamp blinks by
 * blink_level of BLINK_PERIOD, BLINK_REST and BLINK_FADE */
#define LAMP_DIM 0xff101010
#define LAMP_BRIGHT 0xffffffff
#define BLINK_PERIOD 38
#define BLINK_REST 20
#define BLINK_FADE 6
/* how far CAMV bobs above and below where it started */
#define BOB_HEIGHT 20.0F
/* a cycle of CAMH's light takes SCAN_STAGES stages. the light drops SCAN_DROP
 * a stage through the first SCAN_TURN, and climbs back through the rest. a
 * stage takes SCAN_STAGE_STEPS steps. the light's level in each is 1 less
 * camera_blink(frame, SCAN_STAGE_STEPS, SCAN_REST, SCAN_FADE) */
#define SCAN_STAGES 8
#define SCAN_TURN 5
#define SCAN_STAGE_STEPS 20
#define SCAN_DROP 3.5F
#define SCAN_REST 15
#define SCAN_FADE 3

/* the bit of ang.x that turns the head half a turn from ang.y */
#define FACES_BACK 1
/* a hurt to the weak point sets bit WEAK_POINT_INDEX of the work's submode */
#define WEAK_POINT_INDEX 0
/* the head's heading, in the work's timer. it stays fixed as the base sways */
#define HEADING(work) ((work)->timer)
/* where the camera started: CAMV's y in scl.y, and CAMH's x and z in scl.y
 * and scl.z */
#define HOME_Y(work) ((work)->scl.y)
#define HOME_X(work) ((work)->scl.y)
#define HOME_Z(work) ((work)->scl.z)

/* a word of 128 that no code reads */
static Sint32 unused_head_turn_limit = {
#include "shared/patrol_camera/unused_head_turn_limit.inc"
};

/* both cameras' sphere, about the base's pos */
static struct collision_shape base_shape = {
#include "shared/patrol_camera/base_shape.inc"
};

static void vertical_camera_destroy(struct task *task);
static void vertical_camera_step(struct task *task);
static void vertical_camera_draw(struct task *task);
static void horizontal_camera_step(struct task *task);
static void horizontal_camera_destroy(struct task *task);
static void horizontal_camera_draw(struct task *task);

/* set *point to where the head stands in the world. the head is the base's
 * first child, placed by the work's pos and ang.y */
#define HEAD_POSITION(matrix, work, object, point)                             \
    do {                                                                       \
        njPushMatrixEx();                                                      \
        njUnitMatrix(matrix);                                                  \
        njTranslateV(matrix, &(work)->pos);                                    \
        njRotateY(matrix, (work)->ang.y);                                      \
        njCalcPoint(matrix, (NJS_POINT3 *)(object)->child->pos, (point));      \
        njPopMatrix(1);                                                        \
    } while (0)

/* give the camera its weak point at the head, its sphere and its heading. a
 * placement whose state isn't 0 leaves the camera broken */
#define ATTACH_HEAD(task, work, object, point)                                 \
    do {                                                                       \
        HEAD_POSITION(NULL, work, object, point);                              \
        weak_point_create(task, WEAK_POINT_INDEX, point);                      \
        collision_init(task, &base_shape, 1, COLLISION_OBJECT);                \
        if ((task)->placement && placement_get_state(task)) {                  \
            (work)->mode = CAMERA_BROKEN;                                      \
        }                                                                      \
        if ((work)->ang.x & FACES_BACK) {                                      \
            HEADING(work) = (work)->ang.y + HALF_TURN;                         \
        } else {                                                               \
            HEADING(work) = (work)->ang.y;                                     \
        }                                                                      \
    } while (0)

/* the break's sound plays at the base in Lost Colony, and at the falling head
 * in Eternal Engine */
#ifdef COLONY_PATROL_CAMERA
#define BREAK_SOUND_POSITION(work, head) (&(work)->pos)
#else
#define BREAK_SOUND_POSITION(work, head) (&(head))
#endif
/* knock the head off from head, its place in the world. the camera scores
 * BREAK_POINTS and throws the head as debris. both cameras throw CAMV's head
 * model */
#define DROP_HEAD(work, head, ang, slope, speed, floor)                        \
    do {                                                                       \
        (ang).x = 0;                                                           \
        (ang).y = HEADING(work);                                               \
        (ang).z = 0;                                                           \
        (speed).x = 0.0F;                                                      \
        (speed).y = HEAD_RISE;                                                 \
        (speed).z = 0.0F;                                                      \
        (work)->mode = CAMERA_BROKEN;                                          \
        (floor) = surface_probe_floor((head).x, (head).y, (head).z, &(slope)); \
        score_add(BREAK_POINTS);                                               \
        debris_launch_flags(&(head), &(ang), (floor), &(speed),                \
                            vertical_camera_object->child->model,              \
                            &vertical_camera_texlist, FALL_FLAGS);             \
        object_sound_play(BREAK_SOUND, BREAK_SOUND_POSITION(work, head));      \
    } while (0)

void vertical_camera_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 head;

    if (!placement_try_leave(task)) {
        task->draw = vertical_camera_draw;
        task->step = vertical_camera_step;
        task->destructor = vertical_camera_destroy;
        ATTACH_HEAD(task, work, vertical_camera_object, &head);
        HOME_Y(work) = work->pos.y;
    }
}

static void vertical_camera_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement && work->mode == CAMERA_BROKEN) {
        placement_set_state(task, CAMERA_BROKEN);
    }
}

static void vertical_camera_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 head;
    struct angles ang;
    struct angles slope;
    NJS_VECTOR speed;
    Float floor;

    if (placement_try_leave(task)) {
        return;
    }
    if (work->mode != CAMERA_BROKEN && work->submode) {
        HEAD_POSITION(NULL, work, vertical_camera_object, &head);
        DROP_HEAD(work, head, ang, slope, speed, floor);
    }
    collision_register(task);
#ifdef COLONY_PATROL_CAMERA
    /* @hack redundant: two stores of CAMERA_BROKEN, in a block no path
     * reaches */
    if (0) {
        work->mode = CAMERA_BROKEN;
        work->submode = CAMERA_BROKEN;
    }
#endif
    if (work->mode != CAMERA_BROKEN) {
        work->pos.y =
            (njSin((Angle)(work->scl.x * ANGLE_PER_DEGREE) * game_state.steps) *
             BOB_HEIGHT) +
            HOME_Y(work);
        if (task->child) {
            task->child->work->pos.y = work->pos.y;
        }
    }
#ifndef COLONY_PATROL_CAMERA
    /* @hack redundant: a store of CAMERA_BROKEN, in a loop that never runs */
    while (0) {
        work->mode = CAMERA_BROKEN;
    }
#endif
}

/* point holds the lens's place in the base's frame. set it to node's place
 * less the lens's: the offset from the lens of node, its sibling light */
#ifdef NONMATCHING
/* the plain form the hack tag names */
#define LIGHT_OFFSET(point, node)                                              \
    do {                                                                       \
        (point).x = (node)->pos[0] - (point).x;                                \
        (point).y = (node)->pos[1] - (point).y;                                \
        (point).z = (node)->pos[2] - (point).z;                                \
    } while (0)
#else
/* @hack spelling: the differences walked by pointers, where named components
 * would do */
#define LIGHT_OFFSET(point, node)                                              \
    do {                                                                       \
        Float *out = &(point).x;                                               \
        Float *to = (node)->pos;                                               \
        Float *from = &(point).x;                                              \
        *out = *to - *from;                                                    \
        out++;                                                                 \
        to++;                                                                  \
        from++;                                                                \
        *out = *to - *from;                                                    \
        out++;                                                                 \
        to++;                                                                  \
        from++;                                                                \
        *out = *to - *from;                                                    \
    } while (0)

#endif

static void vertical_camera_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_POINT3 offset;
    Uint32 color;

    njSetTexture(&vertical_camera_texlist);
    object = vertical_camera_object;
    njPushMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    if (work->mode != CAMERA_BROKEN) {
        object = object->child;
        offset = *(NJS_POINT3 *)object->pos;
        njPushMatrixEx();
        njTranslateEx(&offset);
        njRotateY(NULL, HEADING(work) - work->ang.y);
        njCnkEasyDrawModel(object->model);
        object = object->sibling;
#ifdef COLONY_PATROL_CAMERA
        LIGHT_OFFSET(offset, object);
#else
#ifdef NONMATCHING
        /* the plain form the hack tag names: LIGHT_OFFSET */
        LIGHT_OFFSET(offset, object);
#else
        {
            /* @hack spelling: the differences walked by post-incremented
             * pointers, where LIGHT_OFFSET would do */
            Float *out = &offset.x;
            Float *to = object->pos;
            Float *from = &offset.x;
            *out++ = *to++ - *from++;
            *out++ = *to++ - *from++;
            *out = *to - *from;
        }
#endif
#endif
        color = color_blend(LAMP_DIM, LAMP_BRIGHT,
                            blink_level(BLINK_PERIOD, BLINK_REST, BLINK_FADE));
        njTranslateEx(&offset);
#ifdef COLONY_PATROL_CAMERA
        njFogDisable();
#endif
        model_draw_tinted(object->model, color);
#ifdef COLONY_PATROL_CAMERA
        njFogEnable();
#endif
        njPopMatrixEx();
#ifdef COLONY_PATROL_CAMERA
        /* @hack redundant: a translation, two draws and a copy of pos.y, in a
         * block no path reaches */
        if (0) {
            Float y = work->pos.y;
            njTranslateEx(&offset);
            njCnkEasyDrawModel(object->model);
            njCnkEasyDrawModel(object->child->model);
            work->pos.y = y;
        }
#endif
    }
#ifndef COLONY_PATROL_CAMERA
    /* @hack redundant: two translations, a draw and a copy of pos.y, in a
     * block no path reaches */
    if (0) {
        Float y = work->pos.y;
        njTranslateEx(&offset);
        njCnkEasyDrawModel(object->model);
        njTranslateEx(&offset);
        work->pos.y = y;
    }
#endif
    njPopMatrix(1);
}

void horizontal_camera_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 head;

    if (!placement_try_leave(task)) {
        task->draw = horizontal_camera_draw;
        task->step = horizontal_camera_step;
        task->destructor = horizontal_camera_destroy;
        ATTACH_HEAD(task, work, horizontal_camera_object, &head);
        HOME_X(work) = work->pos.x;
        HOME_Z(work) = work->pos.z;
    }
}

static void horizontal_camera_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 offset;
    struct angles ang;
    struct angles slope;
    NJS_VECTOR speed;
    Float floor;

    if (placement_try_leave(task)) {
        return;
    }
#ifndef COLONY_PATROL_CAMERA
    /* @hack redundant: a store of CAMERA_BROKEN, in a block no path reaches */
    if (0) {
        work->mode = CAMERA_BROKEN;
    }
#endif
    if (work->mode != CAMERA_BROKEN && work->submode) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.y);
        /* @bug logic: CAMH's head falls from CAMV's head's place, and draws
         * as CAMV's head model. CAMV's head stands at (20, 0, 1) from the
         * base, and CAMH's at (13, -14.5, 0), where ATTACH_HEAD put CAMH's
         * weak point. the step loads vertical_camera_object at 0x4fa and
         * 0x546 of the object's code, in both stages */
        njCalcPoint(NULL, (NJS_POINT3 *)vertical_camera_object->child->pos,
                    &offset);
#ifndef COLONY_PATROL_CAMERA
        /* @hack redundant: a store of CAMERA_BROKEN, in a block no path
         * reaches */
        if (0) {
            work->mode = CAMERA_BROKEN;
        }
#endif
        njPopMatrix(1);
        DROP_HEAD(work, offset, ang, slope, speed, floor);
    }
#ifdef COLONY_PATROL_CAMERA
    /* @hack redundant: stores of CAMERA_BROKEN and of offset.y, in a loop that
     * never runs */
    while (0) {
        work->mode = CAMERA_BROKEN;
        offset.y = 0.0F;
    }
#endif
    if (work->mode == CAMERA_BROKEN) {
        return;
    }
    collision_register(task);
    offset.x = njSin(work->ang.z * game_state.steps) * work->scl.x;
    offset.y = 0.0F;
    offset.z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslate(NULL, HOME_X(work), work->pos.y, HOME_Z(work));
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, &offset, &offset);
    njPopMatrixEx();
    work->pos.x = offset.x;
    work->pos.z = offset.z;
    if (task->child) {
        HEAD_POSITION(NULL, work, horizontal_camera_object,
                      &task->child->work->pos);
    }
}

static void horizontal_camera_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement && work->mode == CAMERA_BROKEN) {
        placement_set_state(task, CAMERA_BROKEN);
    }
}

/* the level blink_level of shared/common_object.h returns, with frame in
 * place of game_state.steps */
static Float camera_blink(int frame, int period, int rest, int fade)
{
    int step = frame % period;
    Float level;

    if (step < fade) {
        level = (Float)step / (Float)fade;
        level = 1.0F - level;
    } else if (step > rest) {
        step -= rest;
        if (step < fade) {
            level = (Float)step / (Float)fade;
        } else {
            level = 1.0F;
        }
    } else {
        level = 0.0F;
    }
    if (level < 0.0F) {
        level = 0.0F;
    }
    if (level > 1.0F) {
        level = 1.0F;
    }
    return level;
}

static void horizontal_camera_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_POINT3 offset;
    Uint32 color;
    int frame;
    int step;
#ifdef COLONY_PATROL_CAMERA
    NJS_CNK_OBJECT *light;
#endif

    njSetTexture(&horizontal_camera_texlist);
    object = horizontal_camera_object;
    njPushMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    if (work->mode != CAMERA_BROKEN) {
        object = object->child;
        offset = *(NJS_POINT3 *)object->pos;
        njPushMatrixEx();
        njTranslateEx(&offset);
        njRotateY(NULL, HEADING(work) - work->ang.y);
        njCnkEasyDrawModel(object->model);
#ifdef COLONY_PATROL_CAMERA
        light = object->sibling;
#ifdef NONMATCHING
        /* the plain form the hack tag names: LIGHT_OFFSET */
        LIGHT_OFFSET(offset, light);
#else
        {
            /* @hack spelling: the differences walked by pointers, from
             * stepped in a statement of its own, where LIGHT_OFFSET would do
             */
            Float *out = &offset.x;
            Float *to = light->pos;
            Float *from = &offset.x;
            *out++ = *to++ - *from++;
            *out++ = *to++ - *from;
            from++;
            *out = *to - *from;
        }
#endif
#else
        object = object->sibling;
        LIGHT_OFFSET(offset, object);
#endif
        frame = game_state.steps % (SCAN_STAGES * SCAN_STAGE_STEPS);
        step = frame / SCAN_STAGE_STEPS;
        if (step >= SCAN_TURN) {
            step = SCAN_STAGES - step;
        }
#ifndef COLONY_PATROL_CAMERA
        /* @hack redundant: a draw and a copy of pos.y, in a block no path
         * reaches */
        if (0) {
            Float y = work->pos.y;
            njCnkEasyDrawModel(object->model);
            work->pos.y = y;
        }
#endif
        color = color_blend(
            LAMP_DIM, LAMP_BRIGHT,
            1.0F - camera_blink(frame, SCAN_STAGE_STEPS, SCAN_REST, SCAN_FADE));
        njTranslate(NULL, offset.x, offset.y - (step * SCAN_DROP), offset.z);
#ifdef COLONY_PATROL_CAMERA
        njFogDisable();
        model_draw_tinted(light->model, color);
        /* @hack redundant: a translation and a draw, in a block no path
         * reaches */
        if (0) {
            Float scale = work->scl.x;
            njTranslate(NULL, scale, scale, frame);
            njCnkEasyDrawModel(light->model);
        }
        njFogEnable();
#else
        model_draw_tinted(object->model, color);
#endif
        njPopMatrixEx();
#ifdef COLONY_PATROL_CAMERA
    } else {
        /* @hack redundant: an empty else */
#endif
    }
    njPopMatrix(1);
}
