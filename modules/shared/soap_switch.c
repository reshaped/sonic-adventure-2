/* the soap switch, SOAP SW in a stage's object list. once a player has
 * passed through it, it sets its placement's state. it draws nothing */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "object/set.h"
#include "shared/soap_switch.h"
#include "system/task.h"

enum soap_switch_mode { MODE_WAIT, MODE_TOUCHED, MODE_PASSED };
/* the state the switch gives its placement as it enters MODE_PASSED */
#define STATE_PASSED 1

/* the index of the switch's shape in soap_switch_shapes, from the low bit
 * of ang.z */
#define SWITCH_SHAPE(work) ((work)->submode)
#define SWITCH_SHAPE_BIT 0x1
#define SWITCH_BOX 0
#define SWITCH_SHAPES 2
/* On a SOAP jump, Sonic copies the low 16 bits of ang.x from timer to
 * stick_lock. A clear low bit of ang.y in tick enables the grade A or B
 * camera, including on a previously passed switch. */
#define NO_CON_TIME(work) ((work)->timer)
#define CAM(work) ((work)->tick)
#define CAM_BIT 0x1

static void soap_switch_draw(struct task *task);
static void soap_switch_destroy(struct task *task);
static void soap_switch_step(struct task *task);

/* The integer parts of scl size the collision shape. On a fresh switch
 * the fractional part of scl.z adjusts a grade A jump's speed along x,
 * and the fractional part of scl.y adjusts a grade B jump's speed. */
static struct collision_shape soap_switch_shapes[SWITCH_SHAPES] = {
#include "shared/soap_switch/soap_switch_shapes.inc"
};
static char *soap_switch_shape_names[SWITCH_SHAPES] = {"CUBE", "SPHERE"};
/* each shape's model in debug_draw_model's table */
static Sint8 soap_switch_models[SWITCH_SHAPES] = {
#include "shared/soap_switch/soap_switch_models.inc"
};

void soap_switch_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    SWITCH_SHAPE(work) = work->ang.z & SWITCH_SHAPE_BIT;
    CAM(work) = work->ang.y & CAM_BIT;
    NO_CON_TIME(work) = work->ang.x;
    work->ang.x = 0;
    /* @hack spelling: the two masks written out, where &= would do */
    work->ang.y = work->ang.y & ~0xff;
    work->ang.z = work->ang.z & ~0xff;
    collision_init(task, &soap_switch_shapes[SWITCH_SHAPE(work)], 1,
                   COLLISION_OBJECT);
    work->collision->shapes->radius = njRoundOff(work->scl.x);
    work->collision->shapes->size_y = njRoundOff(work->scl.y);
    work->collision->shapes->size_z = njRoundOff(work->scl.z);
    if (SWITCH_SHAPE(work) == SWITCH_BOX) {
        work->collision->shapes->ang.z = work->ang.z;
    }
    collision_fit_record_radius(work->collision);
    work->id = WORK_ID_SOAP_SWITCH;
    task->step = soap_switch_step;
    task->draw = soap_switch_draw;
    task->destructor = soap_switch_destroy;
}

static void soap_switch_draw(struct task *task)
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
    (void)"<- ANG Y & ";
    (void)"CAM ON";
    (void)"CAM OFF";
    (void)"<- ANG Z & SHAPE:CUBE";
    (void)"<- SCL X/RANKC SPD OFFSET";
    (void)"<- SCL Y/RANKB SPD OFFSET";
    (void)"<- SCL Z/RANKA SPD OFFSET";
    (void)"-> CAM OFF";
    (void)"-> CAM ON";
    (void)"<- SHAPE:SPHERE";
    (void)"<-      /RANKB SPD OFFSET";
    (void)"<-      /RANKA SPD OFFSET";
    pos = work->pos;
    scale.x = njRoundOff(work->scl.x);
    scale.y = njRoundOff(work->scl.y);
    scale.z = njRoundOff(work->scl.z);
    njPushMatrixEx();
    njTranslateEx(&pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njScaleEx(&scale);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslateEx(&pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njScaleEx(&scale);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslateEx(&pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njScaleEx(&scale);
    njPopMatrixEx();
}

static void soap_switch_destroy(struct task *task)
{
}

static void soap_switch_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    if (work->mode == MODE_WAIT) {
        if (collision_next_player(task) != NULL) {
            work->mode = MODE_TOUCHED;
        }
    } else if (work->mode == MODE_TOUCHED) {
        if (collision_next_player(task) == NULL) {
            work->mode = MODE_PASSED;
            placement_set_state(task, STATE_PASSED);
        }
    }
    collision_register(task);
}
