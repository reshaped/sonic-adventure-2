/* CONT in Security Hall's object list: a container that slides to and fro
 * along its own x. it carries what rides on it. it slides from -scl.x to
 * scl.x at scl.y + SPEED_MARGIN a step, and back. it spends PAUSE_STEPS
 * steps at each end. the slide keeps time by game_state.steps, and bit 0 of
 * ang.x starts it half a cycle on. a container whose scl.y is below 0 stands
 * still. any other has a child, whose box pushes what it meets */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "stg08/fixture_art.h"
#include "stg08/background.h"
#include "stg08/floor_object.h"
#include "stg08/security_hall.h"
#include "stg08/solid_props.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the work's mode: SLIDE_OUT toward scl.x, and SLIDE_BACK toward -scl.x */
enum container_mode { SLIDE_OUT = 0, SLIDE_BACK = 2 };
#define PAUSE_STEPS 30
#define SPEED_MARGIN 0.1F
/* the slide spans SPAN times scl.x */
#define SPAN 2.0F
/* at an end, each step moves the container by its speed and sets the speed
 * anew. the new speed is SPRING times the distance left to the end, plus
 * DAMPING times the old. a container that passes the end goes back to it,
 * and its speed becomes -REBOUND times itself */
#define SPRING 0.5F
#define DAMPING 0.95F
#define REBOUND 0.6F
/* players meet the world's copy only while one is near. near is within the
 * copy's radius + SIZE_MARGIN of its center. world_add classes the copy by
 * that sum too */
#define SIZE_MARGIN 5.0F

/* a container's record, in task->motion */
struct container {
    /* the copy of security_container_collision that the collision world
     * holds, placed where the container lies */
    NJS_OBJECT object;
    /* the container's place along its own x, from its pos */
    union vector_floats offset;
};
#define CONTAINER(task) ((struct container *)(task)->motion.p)
/* task's TASK_FORCES records, by which riders move with the container. the
 * player in slot 0 rides by RIDE_PLAYER_0's, and every other player by
 * RIDE_OTHERS' */
#define RIDERS(task) ((struct task_force *)(task)->force.p)
#define RIDE_PLAYER_0 0
#define RIDE_OTHERS 1
/* the container's speed, a Float kept in the pointer task->extra. the task
 * has no TASK_EXTRA record. task_destroy frees a non-null extra, so the
 * destructor clears the word */
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define SPEED(task) (*(Float *)&(task)->extra)

NJS_TEXLIST *security_container_textures[] = {
    &security_container_texlist,
    NULL,
};

/* clang-format off */
static struct collision_shape child_shape = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
    {0.0F, -6.0F, 0.0F}, 38.0F, 4.0F, 28.0F, 0, {0, 0, 0}};
/* clang-format on */

static void container_destroy(struct task *task);
static void container_step(struct task *task);
static void container_draw(struct task *task);

void security_container_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float size;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct container));
    /* @hack spelling: the callbacks set where the allocation succeeds, and
     * the return in an else, where an early return on failure would do */
    if (task->motion.p) {
        task->draw = container_draw;
        task->step = container_step;
        task->destructor = container_destroy;
        work->mode = SLIDE_OUT;
    } else {
        return;
    }
    object = task->motion.p;
    *object = security_container_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID | WORLD_MOVING;
    size = object->model->r + SIZE_MARGIN;
    CLASSIFY_WORLD(flags, size, WORLD_MOVING | WORLD_SOLID);
    world_add(flags, task, object);
    if (work->scl.y >= 0.0F) {
        model_object_create(task, NULL, NULL, &child_shape, 1, 0);
    }
    if (task->child != NULL) {
        task->child->work->pos = work->pos;
        task->child->work->ang = work->ang;
    }
    SPEED(task) = 0.0F;
}

static void container_destroy(struct task *task)
{
    if (CONTAINER(task)->object.model != NULL) {
        world_remove(task, task->motion.p);
    }
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void container_step(struct task *task)
{
    struct task_work *work = task->work;
    union vector_floats previous;
    NJS_POINT3 center;
    union vector_floats moved;
    NJS_OBJECT *object;
    struct container *container;
    /* @hack type: clock an int, read as a Uint32 at each use, where a Uint32
     * would do */
    int clock;
    int slide_steps;
    int half_cycle;
    int phase;
    Float portion;

    if (placement_try_leave(task)) {
        return;
    }
    SELECT_FLOOR(work, security_floor_height, ~WORLD_ADDED);
    previous.vector = CONTAINER(task)->offset.vector;
    if (work->scl.y >= 0.0F) {
        slide_steps = (int)_builtin_fabsf((work->scl.x * SPAN) /
                                          (work->scl.y + SPEED_MARGIN));
        half_cycle = slide_steps + PAUSE_STEPS;
        clock = game_state.steps;
        if (work->ang.x & 1) {
            phase = (Uint32)(clock + half_cycle) % (half_cycle * 2);
        } else {
            phase = (Uint32)clock % (half_cycle * 2);
        }

        {
            /* @hack spelling: SPEED_MARGIN and SPAN held in locals, where the
             * constants would do */
            const Float speed_margin = SPEED_MARGIN;
            const Float span = SPAN;
            /* @hack redundant: a store of 1 to tick, in a block no path
             * reaches */
            if (0) {
                work->tick = 1;
            }
            /* @hack redundant: a store of span to timer, in a block no path
             * reaches */
            if (0) {
                work->timer = span;
            }

            if (phase < half_cycle) {
                work->mode = SLIDE_OUT;
                if (phase < slide_steps) {
                    portion = (Float)phase / slide_steps;
                    CONTAINER(task)->offset.vector.x =
                        (work->scl.x * portion * span) - work->scl.x;
                    work->timer = 0;
                    SPEED(task) = work->scl.y + speed_margin;
                } else if (work->timer++ == 0) {
                    CONTAINER(task)->offset.vector.x = work->scl.x;
                }
            } else {
                phase -= half_cycle;
                work->mode = SLIDE_BACK;
                if (phase < slide_steps) {
                    portion = 1.0F - ((Float)phase / slide_steps);
                    CONTAINER(task)->offset.vector.x =
                        (work->scl.x * portion * span) - work->scl.x;
                    work->timer = 0;
                    SPEED(task) = work->scl.y + speed_margin;
                } else if (work->timer++ == 0) {
                    CONTAINER(task)->offset.vector.x = -work->scl.x;
                }
            }
        }
        if (work->mode != SLIDE_BACK) {
            if (work->timer) {
                CONTAINER(task)->offset.vector.x += SPEED(task);
                if (CONTAINER(task)->offset.vector.x > work->scl.x) {
                    CONTAINER(task)->offset.vector.x = work->scl.x;
                    SPEED(task) = -(SPEED(task) * REBOUND);
                } else {
                    SPEED(task) =
                        ((work->scl.x - CONTAINER(task)->offset.vector.x) *
                         SPRING) +
                        (SPEED(task) * DAMPING);
                }
            }
        } else if (work->timer) {
            CONTAINER(task)->offset.vector.x -= SPEED(task);
            if (CONTAINER(task)->offset.vector.x < -work->scl.x) {
                CONTAINER(task)->offset.vector.x = -work->scl.x;
                SPEED(task) = -(SPEED(task) * REBOUND);
            } else {
                SPEED(task) =
                    ((work->scl.x + CONTAINER(task)->offset.vector.x) *
                     SPRING) +
                    (SPEED(task) * DAMPING);
            }
        }
    } else {
        /* @hack redundant: an empty else, for a container that stands
         * still */
    }

    if (task->child != NULL) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &CONTAINER(task)->offset.vector,
                    &task->child->work->pos);
        njPopMatrix(1);
    }
    if (CONTAINER(task)->object.model != NULL) {
        container = task->motion.p;
        object = &container->object;
        /* @hack spelling: moved found a Float at a time through pointers,
         * where a difference of each coordinate would do */
        {
            Float *result = moved.component;
            Float *to = container->offset.component;
            Float *from = previous.component;
            *result = *to++ - *from;
            result++;
            from++;
            *result = *to++ - *from;
            result++;
            from++;
            *result = *to - *from;
        }
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.y);
        /* @hack spelling: offset read through CONTAINER(task) beside
         * container, where container would do */
        njCalcPoint(NULL, &CONTAINER(task)->offset.vector,
                    (NJS_POINT3 *)object->pos);
        njCalcVector(NULL, &moved.vector, &RIDERS(task)[RIDE_PLAYER_0].speed);
        njTranslateV(NULL, &object->model->center);
        njCalcPoint(NULL, &CONTAINER(task)->offset.vector, &center);
        njPopMatrix(1);
        RIDERS(task)[RIDE_OTHERS].speed = RIDERS(task)[RIDE_PLAYER_0].speed;
        if (player_near(&center, object->model->r + SIZE_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void container_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: statements kept for their strings, the set editor's
     * labels for ang.x, ang.y, scl.x and scl.y, which SHC places in this
     * source's constants */
    (void)"<- REV(x)";
    (void)"<- REV( )";
    (void)"<- ANGLE Y";
    (void)"<- MOVE LENGTH";
    (void)"<- MOVE SPD";

    if (work->tick) {
        njSetTexture(&security_container_texlist);
        njPushMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.y);
        njTranslateV(NULL, &CONTAINER(task)->offset.vector);
        draw_control_set(DRAW_CONTROL_SHADOW);
        njCnkEasyDrawModel(&security_container_model);
        draw_control_clear(DRAW_CONTROL_SHADOW);
        njPopMatrix(1);
    }
}
