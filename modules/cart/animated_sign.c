/* a sign of three bands. the middle band's rows scroll one way, and the rows
 * above and below it the other. its frame is cart_sign_second from a scl.x of
 * 1, and cart_sign_first below it. no code calls cart_animated_sign_create */
#include <ninja.h>
#include <sg_xpt.h>
#include "cart/course_art.h"
#include "cart/objects.h"
#include "cart/texture_remap.h"
#include "object/set.h"
#include "system/angle.h"
#include "system/task.h"

/* the bands' texture, m_bd in cart_object_textures. its UVs whose v lies from
 * MIDDLE_FIRST to MIDDLE_LAST, within one V_SPAN, make the middle band */
#define BAND_TEXTURE 38
#define V_SPAN 256
#define MIDDLE_FIRST 82
#define MIDDLE_LAST 168
/* a v from -LOWEST_V up wraps into one V_SPAN */
#define LOWEST_V (8 * V_SPAN)
/* the bands' scroll in u grows by SCROLL_STEP a step */
#define SCROLL_STEP 10
/* the most UVs of BAND_TEXTURE that original_v holds */
#define MOST_UVS 64

static Bool collect_uvs = 1;
static Sint32 uv_index;
/* each UV's v within one V_SPAN, which the first scroll records. a call of
 * select_outer or select_middle takes the next, in the order
 * cart_scroll_selected visits the UVs */
static Sint32 original_v[MOST_UVS];
static void sign_step(struct task *task);
static void sign_draw(struct task *task);
static void sign_destroy(struct task *task);
static void scroll_bands(struct task *task);

struct task *cart_animated_sign_create(NJS_POINT3 *pos, struct angles *ang)
{
    struct task *task = task_create(TASK_WORK, 2, sign_step, "CartKanban2Exec");
    task->draw = sign_draw;
    task->destructor = sign_destroy;
    task->work->pos = *pos;
    task->work->ang = *ang;
    return task;
}
static void sign_step(struct task *task)
{
    struct task_work *work = task->work;
    if (!placement_try_leave(task)) {
        if (work->mode == 0) {
            work->mode++;
            work->timer = 0;
            task->draw = sign_draw;
            task->destructor = sign_destroy;
        }
        /* @hack spelling: the timer stepped through task->work, where work
         * would do */
        task->work->timer += SCROLL_STEP;
    }
}
static void sign_draw(struct task *task)
{
    struct task_work *work = task->work;
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&cart_object_textures);
    scroll_bands(task);
    njCnkEasyDrawObject(&cart_sign_panel);
    if (work->scl.x >= 1.0F) {
        njCnkEasyDrawObject(&cart_sign_second);
    } else {
        njCnkEasyDrawObject(&cart_sign_first);
    }
    njPopMatrixEx();
}
static void sign_destroy(struct task *task)
{
}
static Bool select_outer(const NJS_TEX *uv)
{
    Bool selected;
    int row;
    if (collect_uvs) {
        original_v[uv_index] = (uv->v + LOWEST_V) % V_SPAN;
    }
    row = original_v[uv_index];
    if (row < MIDDLE_FIRST || row > MIDDLE_LAST) {
        selected = TRUE;
    } else {
        selected = FALSE;
    }
    uv_index++;
    return selected;
}
static Bool select_middle(const NJS_TEX *uv)
{
    Bool selected;
    if (original_v[uv_index] >= MIDDLE_FIRST &&
        original_v[uv_index] <= MIDDLE_LAST) {
        selected = TRUE;
    } else {
        selected = FALSE;
    }
    uv_index++;
    return selected;
}
static void scroll_bands(struct task *task)
{
    NJS_TEX offset;
    task->work->timer &= CART_SCROLL_MASK;
    offset.u = task->work->timer;
    offset.v = 0;
    uv_index = 0;
    cart_scroll_selected(cart_texture_remaps, &cart_sign_panel, BAND_TEXTURE,
                         &offset, select_outer);
    collect_uvs = 0;
    offset.u = -task->work->timer;
    uv_index = 0;
    cart_scroll_selected(cart_texture_remaps, &cart_sign_panel, BAND_TEXTURE,
                         &offset, select_middle);
}
