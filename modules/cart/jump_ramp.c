/* the jump ramp, JUMP PANEL in the cart's object list: a solid ramp. its
 * launch pushes a racer toward the ramp's +z, and up. no code calls
 * cart_jump_create */
#include <ninja.h>
#include <shinobi.h>

#include "cart/launch.h"
#include "cart/ramp_art.h"
#include "cart/objects.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "system/angle.h"
#include "system/task.h"

#ifdef DREAMCAST
#pragma inline(launch_contacts)
#endif
/* clang-format off */
static DEFINE_LAUNCH_CONTACTS(work->scl.y, work->scl.x)

static struct collision_shape ramp_shape[] = {
    {0, SHAPE_CAPSULE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_HURTS_ALL, 0,
     {0, 50, 50}, 20, 1, 0, 0, {QUARTER_TURN, 0, 0}}};
/* clang-format on */
static void ramp_draw(struct task *task);
static void ramp_destroy(struct task *task);

struct task *cart_jump_create(NJS_POINT3 *pos, struct angles *ang)
{
    struct task *task =
        task_create(TASK_WORK, 2, cart_jump_step, "CartJumpDaiExec");
    task->draw = ramp_draw;
    task->destructor = ramp_destroy;
    task->work->pos = *pos;
    task->work->ang = *ang;
    return task;
}

void cart_jump_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *land;
    if (placement_try_leave(task)) {
        return;
    }
    if (work->mode == 0) {
        work->mode++;
        task->draw = ramp_draw;
        task->destructor = ramp_destroy;
        land = syCalloc(1, sizeof(*land));
#ifdef NONMATCHING
        /* this side kills the task where the heap has no land. the matching
         * side copies into the land through the null */
        if (!land) {
            task_kill(task);
            return;
        }
#endif
        task->motion.p = land;
        *land = cart_jump_land;
        land->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
        land->ang[0] = 0;
        land->ang[1] = work->ang.y;
        land->ang[2] = 0;
        land->pos[0] = work->pos.x;
        land->pos[1] = work->pos.y;
        land->pos[2] = work->pos.z;
        world_add(WORLD_SIZE_CLASS(1) | WORLD_SOLID, task, land);
        collision_init(task, ramp_shape, 1, COLLISION_OBJECT);
        work->scl.z = 0.0F;
    } else {
        launch_contacts(work);
    }
    collision_register(task);
}
static void ramp_draw(struct task *task)
{
    struct task_work *work = task->work;
    if (cart_out_of_view(&work->pos)) {
        return;
    }
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&cart_object_textures);
    njCnkEasyDrawObject(&cart_jump_model);
    njPopMatrixEx();
}
static void ramp_destroy(struct task *task)
{
    if (task->motion.p) {
        world_remove(task, task->motion.p);
    }
}
