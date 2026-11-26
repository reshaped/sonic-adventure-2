/* a line of rings, G RING in the cart's object list. its first step places
 * ang.x rings, then ends its own task. the first ring stands at its position,
 * and each next one scl past the last. no code draws through
 * cart_ring_group_draw */
#include <ninja.h>

#include "cart/objects.h"
#include "object/set.h"
#include "shared/ring.h"
#include "system/task.h"
#include "system/vector.h"

void cart_ring_group_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 position;
    int i;
    /* @hack redundant: a test of task under if (0) */
    if (0) {
        if (task) {
        }
    }
    if (work->mode == 0) {
        position = work->pos;
        for (i = 0; i < work->ang.x; i++) {
            cart_ring_create(&position);
            vector_sum(&position, &work->scl, &position);
        }
        if (task->placement) {
            placement_kill(task);
        } else {
            task_destroy(task);
        }
    }
}
void cart_ring_group_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 position;
    int i;
    position = work->pos;
    if (cart_out_of_view(&work->pos)) {
        return;
    }
    njSetTexture(&ring_texlist);
    for (i = 0; i < work->ang.x; i++) {
        njPushMatrixEx();
        njTranslate(NULL, position.x, position.y, position.z);
        njCnkSimpleDrawModel(ring_model);
        njPopMatrixEx();
        vector_sum(&position, &work->scl, &position);
    }
}
