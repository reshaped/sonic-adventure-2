/* the sign, KANBAN in the cart's object list. its first step picks one of the
 * CART_SIGN_KINDS kinds at random, and keeps it in scl.x */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "cart/course_art.h"
#include "cart/objects.h"
#include "cart/texture_remap.h"
#include "object/set.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the model's poster, poster1 in cart_object_textures. each kind of sign shows
 * its texture of sign_textures there: chao_p, poster1, sonic or sonicspin */
#define POSTER_TEXTURE 39

static Sint32 sign_textures[CART_SIGN_KINDS] = {12, 39, 67, 68};
static void sign_draw(struct task *task);
static void sign_destroy(struct task *task);

struct task *cart_sign_create(NJS_POINT3 *pos, struct angles *ang)
{
    struct task *task =
        task_create(TASK_WORK, 2, cart_sign_step, "CartKanbanExec");
    task->draw = sign_draw;
    task->destructor = sign_destroy;
    task->work->pos = *pos;
    task->work->ang = *ang;
    return task;
}
void cart_sign_step(struct task *task)
{
    struct task_work *work = task->work;
    if (placement_try_leave(task)) {
        return;
    }
    if (work->mode == 0) {
        work->mode++;
        task->draw = sign_draw;
        task->destructor = sign_destroy;
        work->scl.x = rand() * (CART_SIGN_KINDS * RAND_SCALE);
    }
}
static void sign_draw(struct task *task)
{
    struct task_work *work = task->work;
    if (cart_out_of_view(&work->pos)) {
        return;
    }
    cart_replace_texture(cart_texture_remaps, &cart_sign_model, POSTER_TEXTURE,
                         sign_textures[(int)work->scl.x]);
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&cart_object_textures);
    njCnkEasyDrawObject(&cart_sign_model);
    njPopMatrixEx();
}
static void sign_destroy(struct task *task)
{
}
void cart_draw_sign(struct cart_sign *sign, int kind)
{
    cart_replace_texture(cart_texture_remaps, &cart_sign_model, POSTER_TEXTURE,
                         sign_textures[kind]);
    njPushMatrixEx();
    njTranslateV(NULL, &sign->pos);
    njRotateY(NULL, sign->yaw);
    njSetTexture(&cart_object_textures);
    njCnkEasyDrawObject(&cart_sign_model);
    njPopMatrixEx();
}
