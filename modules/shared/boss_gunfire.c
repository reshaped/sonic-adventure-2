/* the mech bosses' gunfire: a flash at a gun's muzzle, and fire that follows
 * the muzzle. Big Foot, Hot Shot and Flying Dog fire them */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "effect/powder.h"
#include "shared/gun_mech/effect_art.h"
#include "shared/boss_gunfire.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the level of a fire's task */
#define FIRE_LEVEL 3

/* the step kills a muzzle flash once its frame passes MUZZLE_LAST */
#define MUZZLE_LAST 3
/* a muzzle flash scales its models by MUZZLE_WIDTH in x and z and by
 * MUZZLE_HEIGHT in y. it then scales them by its own width and height. its
 * width runs from 1 up to 2, and its height is MUZZLE_FLAT of its width */
#define MUZZLE_WIDTH 1.7F
#define MUZZLE_HEIGHT 1.5F
#define MUZZLE_FLAT 0.7F

/* a fire's looks, in its work's look: 0 and 2 draw one model, 1 and 3 the
 * other */
#define FIRE_LOOKS 4
/* a fire is killed at the step that finds its timer above FIRE_LAST */
#define FIRE_LAST 1
/* a fire's scale runs from FIRE_SCALE_MIN up to FIRE_SCALE_MIN +
 * FIRE_SCALE_RANGE along each axis */
#define FIRE_SCALE_MIN 0.6F
#define FIRE_SCALE_RANGE 0.6F

/* a muzzle flash's work, laid over the struct task_work that
 * task_create_child allocates. frame lies at submode, origin at ang.x and yaw
 * at ang.y. width and height lie at scl.x and scl.y */
struct muzzle_work {
    Sint8 mode;
    Sint8 frame;
    Uint8 unk_02[6];
    NJS_POINT3 *origin; /* the call's pos, which the flash doesn't follow */
    Angle yaw;
    Angle unk_10;
    NJS_POINT3 pos;
    Float width;
    Float height;
};

/* a fire's work, laid over the struct task_work that task_create allocates.
 * look lies at submode, and pitch, yaw and follow at ang.x, ang.y and
 * ang.z */
struct fire_work {
    Sint8 mode;
    Sint8 look;
    Uint8 unk_02[4];
    Uint16 timer;
    Angle pitch;
    Angle yaw;
    NJS_POINT3 *follow; /* the point the fire follows */
    NJS_POINT3 pos;
    NJS_VECTOR scl;
};

/* the fire's offset from the point it follows, in its task's motion */
#define OFFSET(task) ((NJS_VECTOR *)(task)->motion.p)

static void boss_gunfire_muzzle_destroy(struct task *task);
static void boss_gunfire_muzzle_step(struct task *task);
static void boss_gunfire_muzzle_draw(struct task *task);
static void boss_gunfire_destroy(struct task *task);
static void boss_gunfire_step(struct task *task);
static void boss_gunfire_draw(struct task *task);

struct task *boss_gunfire_muzzle_create(struct task *parent, NJS_POINT3 *pos,
                                        struct angles *ang)
{
    struct task *task;
    struct muzzle_work *work;
    /* @hack redundant: motion, a read of the task's record that nothing
     * uses */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    void *motion;

    if (parent == NULL) {
        return NULL;
    }
    task = task_create_child(TASK_MOTION | TASK_WORK, boss_gunfire_muzzle_step,
                             parent);
    if (task == NULL) {
        return NULL;
    }
    work = (struct muzzle_work *)task->work;
    motion = task->motion.p;
    task->draw = boss_gunfire_muzzle_draw;
    task->destructor = boss_gunfire_muzzle_destroy;
    work->origin = pos;
    work->pos = *pos;
    work->width = (rand() * RAND_SCALE) + 1.0F;
    work->height = work->width * MUZZLE_FLAT;
    return task;
}

static void boss_gunfire_muzzle_destroy(struct task *task)
{
}

static void boss_gunfire_muzzle_step(struct task *task)
{
    struct muzzle_work *work = (struct muzzle_work *)task->work;

    if (game_state.time_stopped) {
        return;
    }
    if (++work->frame > MUZZLE_LAST) {
        task_kill(task);
    }
}

static void boss_gunfire_muzzle_draw(struct task *task)
{
    struct muzzle_work *work = (struct muzzle_work *)task->work;
    NJS_CNK_OBJECT *object;

    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->yaw);
    njScale(NULL, MUZZLE_WIDTH, MUZZLE_HEIGHT, MUZZLE_WIDTH);
#ifdef NONMATCHING
    /* GCC can't see that object is always set. a drawn flash's frame runs from
     * 0 to MUZZLE_LAST + 1. the switch has a case for each */
    object = &muzzle_object_a;
#endif
    switch (work->frame) {
    case 0:
    case 1:
        object = &muzzle_object_a;
        break;
    case 2:
        object = &muzzle_object_b;
        break;
    case 4:
    case 3:
        object = &muzzle_object_c;
        break;
    }
    njScale(NULL, work->width, work->height, work->width);
    njCnkSimpleDrawObject(object);
    njPopMatrixEx();
}

static void boss_gunfire_destroy(struct task *task)
{
}

static void boss_gunfire_step(struct task *task)
{
    struct fire_work *work = (struct fire_work *)task->work;
    NJS_VECTOR *offset = OFFSET(task);
    /* @hack spelling: follow, a local for the point, where work->follow
     * read in place would do */
    NJS_POINT3 *follow;

    if (game_state.time_stopped || game_state.unk_09) {
        return;
    }
    follow = work->follow;
    work->pos = *follow;
    work->pos.x += offset->x;
    work->pos.y += offset->y;
    work->pos.z += offset->z;
    if (work->timer++ > FIRE_LAST) {
        task_kill(task);
    }
}

static void boss_gunfire_draw(struct task *task)
{
    struct fire_work *work = (struct fire_work *)task->work;
    NJS_CNK_OBJECT *object;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->yaw);
    /* @bug logic: turns the fire by its pitch about z. powder.c's
     * gunfire_draw turns the same models by the same pitch about x (JSR @R2
     * at 0x206 of the object's code) */
    njRotateZ(NULL, work->pitch);
    switch (work->look) {
    case 0:
    case 2:
        object = &gunfire_object_a;
        break;
    /* @hack spelling: an arm of its own for 1, and for 3 with the default,
     * where one arm would do */
    case 1:
        object = gunfire_object_b;
        break;
    case 3:
    default:
        object = gunfire_object_b;
        break;
    }
    njScaleEx(&work->scl);
    njCnkEasyDrawObject(object);
    njPopMatrixEx();
}

void boss_gunfire_create(NJS_POINT3 *pos, NJS_VECTOR *dir, NJS_VECTOR *offset)
{
    struct task *task;
    struct fire_work *work;
    NJS_VECTOR *copy;

    if ((task = task_create(TASK_MOTION | TASK_WORK, FIRE_LEVEL,
                            boss_gunfire_step, "GroboGunFireExecute")) !=
        NULL) {
        work = (struct fire_work *)task->work;
        copy = OFFSET(task);
        work->pos = *pos;
        work->scl.x =
            (rand() * (FIRE_SCALE_RANGE * RAND_SCALE)) + FIRE_SCALE_MIN;
        work->scl.y =
            (rand() * (FIRE_SCALE_RANGE * RAND_SCALE)) + FIRE_SCALE_MIN;
        work->scl.z =
            (rand() * (FIRE_SCALE_RANGE * RAND_SCALE)) + FIRE_SCALE_MIN;
        work->yaw = (Angle)(atan2f(dir->x, dir->z) * ANGLE_PER_RADIAN);
        work->pitch = (Angle)(asinf(dir->y / njScalor(dir)) * ANGLE_PER_RADIAN);
        work->follow = pos;
        *copy = *offset;
        work->look =
            (game_state.steps + (int)(rand() * (FIRE_LOOKS * RAND_SCALE))) &
            (FIRE_LOOKS - 1);
        task->draw = boss_gunfire_draw;
        task->destructor = boss_gunfire_destroy;
    }
}
