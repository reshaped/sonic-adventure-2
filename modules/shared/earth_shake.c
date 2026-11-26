/* the earth shake: a quake that shakes each camera within its range by a damped
 * spring */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "shared/earth_shake.h"
#include "system/task.h"
#include "system/util.h"

/* the task level of both tasks */
#define SHAKE_LEVEL 3
#define SPEED(work) ((work)->scl.x)
#define STRETCH(work) ((work)->scl.y)
#define RANGE(work) ((work)->scl.z)
/* what ang.y, the shake's turn about y, gains each step */
#define TURN_STEP(work) ((work)->timer)
#define TURN_LEAST 0x300
#define TURN_RANDOM 128.0F
/* a quake's spring is stiffer than shake_stiffness by a random part below
 * STIFFNESS_RANDOM. its task's unk_3c keeps the part */
#define STIFFNESS_RANDOM 0.05F
/* @hack type: the task's unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define STIFFNESS(task) (*(Float *)&(task)->unk_3c)
/* the part of the spring's speed that shakes along x and z */
#define SWAY 0.35F
#define SPEED_LEAST 0.1F
/* the manager ends once both sums' squared lengths are below this */
#define SHAKE_LEAST 0.001F
/* the manager's sums of the step's shakes, for camera 0 and for the rest */
#define SHAKE_0(work) ((work)->pos)
#define SHAKE_1(work) ((work)->scl)

static void shake_manager_step(struct task *task);
static void shake_manager_destroy(struct task *task);
static void shake_add(int camera, NJS_VECTOR *shake);
static void shake_step(struct task *task);

/* the manager's task while it runs, or null */
static struct task *shake_manager = NULL;
static NJS_POINT3 shake_none = {0.0F, 0.0F, 0.0F};
static Float shake_damping = 0.98F;
static Float shake_rebound = 0.9F;
static Float shake_stiffness = 0.5F;

static void shake_manager_step(struct task *task)
{
    struct task_work *work = task->work;

    if (njScalor2(&SHAKE_0(work)) < SHAKE_LEAST &&
        njScalor2(&SHAKE_1(work)) < SHAKE_LEAST) {
        task_destroy(task);
    } else {
        if (camera_active_count >= 1) {
            camera_set_shake(0, &SHAKE_0(work));
        }
        SHAKE_0(work) = shake_none;
        if (camera_active_count >= 2) {
            camera_set_shake(1, &SHAKE_1(work));
        }
        SHAKE_1(work) = shake_none;
    }
}

static void shake_manager_destroy(struct task *task)
{
    int i;

    for (i = 0; i < camera_active_count; i++) {
        camera_stop_shake(i);
    }
    if (shake_manager == task) {
        shake_manager = NULL;
    }
}

/* add shake to camera's sum, and start the manager unless one runs. a camera
 * past 0 adds to camera 1's sum */
static void shake_add(int camera, NJS_VECTOR *shake)
{
    struct task *task;
    struct task_work *work;
    Float *sum;
    Float *part;

    if (shake_manager == NULL) {
        shake_manager = task_create(TASK_WORK, SHAKE_LEVEL, shake_manager_step,
                                    "EarthShakeManExec");
        if (shake_manager != NULL) {
            shake_manager->destructor = shake_manager_destroy;
        }
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = shake_manager) != NULL) {
        work = task->work;
        sum = camera == 0 ? &SHAKE_0(work).x : &SHAKE_1(work).x;
        /* @hack spelling: the sums through walking pointers, where
         * sum->x += shake->x and the rest through an NJS_POINT3 would do */
        part = &shake->x;
        *sum++ += *part++;
        *sum++ += *part++;
        *sum += *part;
    }
}

static void shake_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR shake;
    Float distance;
    Float weight;
    Float sway;
    int i;

    SPEED(work) = -(STRETCH(work) * (shake_stiffness + STIFFNESS(task))) +
                  (SPEED(work) * shake_damping);
    STRETCH(work) += SPEED(work);
    if (STRETCH(work) < 0.0F) {
        STRETCH(work) = 0.0F;
        SPEED(work) = njAbs(SPEED(work)) * shake_rebound;
    }
    for (i = 0; i < camera_active_count; i++) {
        distance = njDistanceP2P(&camera_views[i]->pos, &work->pos);
        if (distance < RANGE(work)) {
            weight = 1.0F - (distance / RANGE(work));
            /* @hack spelling: sway, a local for the shake's part along x and
             * z before SWAY, where the product written in place would do */
            sway = weight * njAbs(SPEED(work));
            shake.y = weight * STRETCH(work);
            shake.x = shake.z = sway * SWAY;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, work->ang.y);
            njCalcVector(NULL, &shake, &shake);
            njPopMatrixEx();
            shake_add(i, &shake);
        }
    }
    work->ang.y += TURN_STEP(work);
    if (njAbs(SPEED(work)) < SPEED_LEAST) {
        task_destroy(task);
    }
}

struct task *shake_create(NJS_POINT3 *pos, Float range, Float power)
{
    struct task *task;
    struct task_work *work;

    task = task_create(TASK_WORK, SHAKE_LEVEL, shake_step, "EsShakeRad");
    if (task != NULL) {
        work = task->work;
        work->pos = *pos;
        RANGE(work) = range;
        SPEED(work) = power;
        TURN_STEP(work) =
            (int)((Float)rand() * (TURN_RANDOM * RAND_SCALE)) + TURN_LEAST;
        STIFFNESS(task) = (Float)rand() * (STIFFNESS_RANDOM * RAND_SCALE);
    }
    return task;
}
