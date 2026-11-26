/* the gravity field of Meteor Herd: a sphere or a capsule. it points
 * world_gravity from a player within its reach toward itself. world_gravity is
 * the one way things fall, which every player shares. no object list names
 * it */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg32/gravity.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* a bit of ang.x: the field is a capsule, not a sphere */
#define CAPSULE 1
/* a field reaches GRAVITY_MARGIN past its radius, scl.x */
#define GRAVITY_MARGIN 10.0F

static void gravity_destroy(struct task *task);
static void gravity_step(struct task *task);
static void gravity_draw(struct task *task);

void herd_gravity_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = gravity_draw;
        task->step = gravity_step;
        task->destructor = gravity_destroy;
    }
}

static void gravity_destroy(struct task *task)
{
}

/* @hack spelling: the difference as an inline walk of three Floats, with
 * from++ a statement of its own, where three statements on the fields would
 * do */
#pragma inline(point_difference)
static void point_difference(Float *to, Float *from, Float *out)
{
    *out++ = *to++ - *from;
    from++;
    *out++ = *to++ - *from;
    from++;
    *out = *to - *from;
}

static void gravity_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *player;
    NJS_VECTOR toward;
    Float distance;
    int slot;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, which stores CAPSULE in the
     * work's mode and submode */
    if (0) {
        work->mode = CAPSULE;
        work->submode = CAPSULE;
    }
    for (slot = 0; slot < GAME_PLAYERS; slot++) {
        if (player_work[slot]) {
            /* @hack redundant: a block no path reaches, which calls
             * njTranslateEx with toward */
            if (0) {
                njTranslateEx(&toward);
            }
            player = player_work[slot];
            point_difference(&work->pos.x, &player->pos.x, &toward.x);
            if (work->ang.x & CAPSULE) {
                /* in the capsule's frame the segment runs scl.y each way along
                 * y. the way to its nearest point is toward with y moved scl.y
                 * toward 0, and no further */
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njRotateZ(NULL, -work->ang.z);
                njRotateY(NULL, -work->ang.y);
                njCalcVector(NULL, &toward, &toward);
                if (work->scl.y > njAbs(toward.y)) {
                    toward.y = 0.0F;
                } else if (toward.y > 0.0F) {
                    toward.y -= work->scl.y;
                } else {
                    toward.y += work->scl.y;
                }
                distance = njScalor(&toward);
                if (distance < work->scl.x + GRAVITY_MARGIN) {
                    njUnitMatrix(NULL);
                    njRotateY(NULL, work->ang.y);
                    njRotateZ(NULL, work->ang.z);
                    njCalcVector(NULL, &toward, &toward);
                    njUnitVector(&toward);
                    world_gravity = toward;
                    world_gravity_ang_x =
                        (Angle)(asinf(-toward.z) * ANGLE_PER_RADIAN);
                    world_gravity_ang_z = -(
                        Angle)(atan2f(-toward.x, -toward.y) * ANGLE_PER_RADIAN);
                }
                njPopMatrixEx();
            } else {
                distance = njScalor(&toward);
                if (distance < work->scl.x + GRAVITY_MARGIN) {
                    njUnitVector(&toward);
                    world_gravity = toward;
                    world_gravity_ang_x =
                        (Angle)(asinf(-toward.z) * ANGLE_PER_RADIAN);
                    world_gravity_ang_z = -(
                        Angle)(atan2f(-toward.x, -toward.y) * ANGLE_PER_RADIAN);
                }
            }
        }
    }
}

static void gravity_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_VECTOR scale;

    /* @hack redundant: a draw after the return, which no path reaches. it is
     * kept for its strings, which SHC still places in this source's constants,
     * and for the frame SHC still gives its locals */
    if (1) {
        return;
    }
    (void)"<- CAPSULE";
    (void)"<- SPHERE";
    (void)"<- ROT Y";
    (void)"<- ROT Z";
    (void)"<- DIST";
    (void)"<- RANGE";
    pos = work->pos;
    scale = work->scl;
    njPushMatrixEx();
    njTranslateEx(&pos);
    njRotateY(NULL, work->ang.y);
    njScaleEx(&scale);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslateEx(&pos);
    njPopMatrixEx();
    njPushMatrixEx();
    njPopMatrixEx();
}
