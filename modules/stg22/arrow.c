/* ARROW in Crazy Gadget's object list: an arrow whose chevrons scroll, turned
 * by ang.y */
#include "stg22/fixture_models.h"
#include <sg_xpt.h>
#include <ninja.h>

#include "draw/texture_anim.h"
#include "object/set.h"
#include "stg22/prop_textures.h"
#include "stg22/scenery.h"
#include "system/game_state.h"
#include "system/task.h"

/* the chevrons scroll SCROLL_STEP in v every SCROLL_PERIOD steps */
#define SCROLL_STEP 16
#define SCROLL_PERIOD 18

static Sint16 chevron_uvs[12] = {
#include "stg22/arrow/chevron_uvs.inc"
};
static struct texture_anim chevron_scroll = {
    TEXTURE_ANIM_SCROLL, 1, 1, NULL, chevron_uvs, NULL, NULL, NULL, 0, 0,
};
NJS_TEXLIST *gadget_arrow_texlists[] = {&gadget_arrow_texlist, NULL};

static void arrow_destroy(struct task *task);
static void arrow_step(struct task *task);
static void arrow_draw(struct task *task);

void gadget_arrow_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = arrow_draw;
        task->step = arrow_step;
        task->destructor = arrow_destroy;
    }
}

static void arrow_destroy(struct task *task)
{
}

static void arrow_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: the set editor's label, in a block no path reaches */
    if (0) {
        (void)"<- ROT Y";
    }
}

static void arrow_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *part;

    njSetTexture(&gadget_arrow_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    part = &gadget_arrow_object;
    njCnkEasyDrawModel(part->model);
    part = part->child;
    texture_anim_scroll(part->model, &chevron_scroll, 0,
                        game_state.steps / SCROLL_PERIOD * SCROLL_STEP %
                            SCROLL_WRAP);
    njCnkEasyDrawModel(part->model);
    njPopMatrixEx();
}
