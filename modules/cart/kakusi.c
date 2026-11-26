/* the object KAKUSI in the cart's object list */
#include <ninja.h>

#include "cart/course_art.h"
#include "cart/objects.h"
#include "object/set.h"
#include "system/task.h"

static void kakusi_draw(struct task *task);
static void kakusi_destroy(struct task *task);

void cart_kakusi_step(struct task *task)
{
    struct task_work *work = task->work;
    if (!placement_try_leave(task) && work->mode == 0) {
        work->mode++;
        work->scl.x = 1.0F;
        work->scl.y = 1.0F;
        work->scl.z = 1.0F;
        task->draw = kakusi_draw;
        task->destructor = kakusi_destroy;
    }
}
static void kakusi_draw(struct task *task)
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
    njScale(NULL, work->scl.x, work->scl.y, work->scl.z);
    njSetTexture(&cart_object_textures);
    njCnkEasyDrawObject(&cart_kakusi_model);
    njPopMatrixEx();
}
static void kakusi_destroy(struct task *task)
{
}
