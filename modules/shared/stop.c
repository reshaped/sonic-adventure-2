/* the stops, STOPLSD and STOPLOCKON in a stage's object list. a set file
 * places each as a task that holds a collision record. neither draws
 * anything */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "shared/stop.h"
#include "system/task.h"

#define LOCKON_BOX_BIT 0x1

/* the index of a STOPLSD's shape in stop_lsd_shapes, from the low bit of
 * ang.z */
#define STOP_SHAPE(work) ((work)->submode)
#define STOP_SHAPE_BIT 0x1
#define STOP_BOX 0
#define STOP_SHAPES 2
/* a STOPLSD keeps ang.x's low 16 bits in its timer, which nothing reads. the
 * strings in its draw name the field NO CON TIME */
#define NO_CON_TIME(work) ((work)->timer)

static void stop_lsd_draw(struct task *task);
static void stop_lsd_destroy(struct task *task);
static void stop_lsd_step(struct task *task);
static void stop_lockon_draw(struct task *task);
static void stop_lockon_destroy(struct task *task);
static void stop_lockon_step(struct task *task);

/* a STOPLSD's shapes, a box and a sphere. stop_lsd_init copies one into the
 * stop's record, and sizes the copy by scl */
static struct collision_shape stop_lsd_shapes[STOP_SHAPES] = {
#include "shared/stop/stop_lsd_shapes.inc"
};
static char *stop_lsd_shape_names[STOP_SHAPES] = {"CUBE", "SPHERE"};
/* each shape's model in debug_draw_model's table */
static Sint8 stop_lsd_models[STOP_SHAPES] = {
#include "shared/stop/stop_lsd_models.inc"
};
/* a STOPLOCKON's shape, the one lockon_box_create gives the box it makes */
static struct collision_shape stop_lockon_shape = {
#include "shared/stop/stop_lockon_shape.inc"
};

void stop_lsd_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    STOP_SHAPE(work) = work->ang.z & STOP_SHAPE_BIT;
    NO_CON_TIME(work) = work->ang.x;
    work->ang.x = 0;
    /* @hack spelling: the two masks written out, where &= would do */
    work->ang.y = work->ang.y & ~0xff;
    work->ang.z = work->ang.z & ~0xff;
    collision_init(task, &stop_lsd_shapes[STOP_SHAPE(work)], 1,
                   COLLISION_OBJECT);
    work->collision->shapes->radius = work->scl.x;
    work->collision->shapes->size_y = work->scl.y;
    work->collision->shapes->size_z = work->scl.z;
    if (STOP_SHAPE(work) == STOP_BOX) {
        work->collision->shapes->ang.z = work->ang.z;
    }
    collision_fit_record_radius(work->collision);
    work->id = WORK_ID_STOP_LSD;
    task->step = stop_lsd_step;
    task->draw = stop_lsd_draw;
    task->destructor = stop_lsd_destroy;
}

static void stop_lsd_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_POINT3 scale;

    /* @hack redundant: a body behind a return, where an empty function would
     * do. SHC writes none of its code, but places its strings and gives the
     * draw the frame its locals need. the statements after the strings are
     * stand-ins that only size that frame */
    if (1) {
        return;
    }
    (void)"<- NO CON TIME:";
    (void)"<- ANG Y";
    (void)"<- ANG Z & SHAPE:CUBE";
    (void)"<- SCL X";
    (void)"<- SCL Y";
    (void)"<- SCL Z";
    (void)"<- SHAPE:SPHERE";
    pos = work->pos;
    scale = work->scl;
    njPushMatrixEx();
    njTranslateEx(&pos);
    if (STOP_SHAPE(work) == STOP_BOX) {
        njRotateZ(NULL, work->ang.z);
    }
    njScaleEx(&scale);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslateEx(&pos);
    njScaleEx(&scale);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslateEx(&pos);
    njScaleEx(&scale);
    njPopMatrixEx();
}

static void stop_lsd_destroy(struct task *task)
{
}

static void stop_lsd_step(struct task *task)
{
    if (!placement_try_leave(task)) {
        collision_register(task);
    }
}

void stop_lockon_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @bug memory: the destructor's enemy_destroy reads an enemy_work that
     * isn't there where enemy_work_alloc finds no room. the failed alloc
     * kills the task, but the step set below undoes the kill: 0xee, 0x110 and
     * 0x182 of the object's code */
#ifdef NONMATCHING
    /* this side repairs the tag above: where enemy_work_alloc fails, the init
     * returns, so its kill stands */
    if (enemy_work_alloc(task) == NULL) {
        return;
    }
#else
    enemy_work_alloc(task);
#endif
    collision_init(task, &stop_lockon_shape, 1, COLLISION_ITEM);
    /* @hack spelling: the clear bit tested, where a test of the set bit would
     * do */
    work->id = !(work->ang.x & LOCKON_BOX_BIT) ? WORK_ID_STOP_LOCKON
                                               : WORK_ID_STOP_LOCKON_BOX;
    task->step = stop_lockon_step;
    task->draw = stop_lockon_draw;
    task->destructor = stop_lockon_destroy;
}

static void stop_lockon_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a body behind a return, where an empty function would
     * do. SHC writes none of its code, but places its strings and gives the
     * draw the frame its locals need. the statements after the strings are
     * stand-ins that only size that frame */
    if (1) {
        return;
    }
    (void)"<- NEW TYPE";
    (void)"<- SIZE X";
    (void)"<- SIZE Y";
    (void)"<- SIZE Z";
    (void)"<- OLD TYPE";
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njPushMatrixEx();
    njRotateZ(NULL, work->ang.z);
    njPopMatrixEx();
    njPushMatrixEx();
    njRotateX(NULL, work->ang.x);
    njPopMatrixEx();
    njPopMatrixEx();
}

static void stop_lockon_destroy(struct task *task)
{
    enemy_destroy(task);
}

static void stop_lockon_step(struct task *task)
{
    if (!placement_try_leave(task)) {
        collision_register(task);
    }
}
