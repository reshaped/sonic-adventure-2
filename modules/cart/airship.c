/* the airship, HIKOUSEN in the cart's object list */
#include <ninja.h>

#include "cart/course_art.h"
#include "cart/objects.h"
#include "object/set.h"
#include "system/task.h"

static void airship_draw(struct task *task);
static void airship_destroy(struct task *task);

void cart_airship_step(struct task *task)
{
    struct task_work *work = task->work;
    if (!placement_try_leave(task) && work->mode == 0) {
        work->mode++;
        task->draw = airship_draw;
        task->destructor = airship_destroy;
    }
}
static void airship_draw(struct task *task)
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
    if (work->scl.x < 1.0F) {
        njCnkEasyDrawObject(&cart_airship_first);
    } else {
        njCnkEasyDrawObject(&cart_airship_second);
    }
    njPopMatrixEx();
}
static void airship_destroy(struct task *task)
{
}
