/* the dash panel, DASH PANEL in the cart's object list. its launch pushes a
 * racer toward the panel's -z, with no rise. no code calls cart_dash_create */
#include <ninja.h>
#include "cart/course_art.h"
#include "cart/launch.h"
#include "cart/objects.h"
#include "cart/texture_remap.h"
#include "collision/collision.h"
#include "object/set.h"
#include "system/angle.h"
#include "system/task.h"

/* the belt's texture, sikake_07_64 in cart_object_textures. it scrolls in v
 * by BELT_STEP a step */
#define BELT_TEXTURE 66
#define BELT_STEP 60

#ifdef DREAMCAST
#pragma inline(launch_contacts)
#endif
/* clang-format off */
static DEFINE_LAUNCH_CONTACTS(0.0F, -work->scl.x)

static struct collision_shape panel_shape[] = {
    {0, SHAPE_CAPSULE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_HURTS_ALL, 0,
     {0, 0, 0}, 20, 20, 0, 0, {QUARTER_TURN, 0, 0}}};
/* clang-format on */
static void panel_draw(struct task *task);
static void panel_destroy(struct task *task);
static void scroll_belt(struct task *task);
struct task *cart_dash_create(NJS_POINT3 *pos, struct angles *ang)
{
    struct task *task =
        task_create(TASK_WORK, 2, cart_dash_step, "CartDashPanelExec");
    task->draw = panel_draw;
    task->destructor = panel_destroy;
    task->work->pos = *pos;
    task->work->ang = *ang;
    return task;
}
void cart_dash_step(struct task *task)
{
    struct task_work *work = task->work;
    if (placement_try_leave(task)) {
        return;
    }
    if (work->mode == 0) {
        work->mode++;
        task->draw = panel_draw;
        task->destructor = panel_destroy;
        collision_init(task, panel_shape, 1, COLLISION_OBJECT);
        work->scl.z = 0.0F;
    } else {
        launch_contacts(work);
    }
    collision_register(task);
    scroll_belt(task);
    /* @hack redundant: a goto to the next statement */
    goto advance;
advance:
    /* @hack spelling: the timer stepped through task->work, where work would
     * do */
    task->work->timer += BELT_STEP;
}
static void panel_draw(struct task *task)
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
    njCnkEasyDrawObject(&cart_dash_frame);
    njCnkEasyDrawObject(&cart_dash_belt);
    njPopMatrixEx();
}
static void panel_destroy(struct task *task)
{
}
static void scroll_belt(struct task *task)
{
    NJS_TEX offset;
    task->work->timer &= CART_SCROLL_MASK;
    offset.u = 0;
    offset.v = task->work->timer;
    cart_scroll_texture(cart_texture_remaps, &cart_dash_belt, BELT_TEXTURE,
                        &offset);
}
