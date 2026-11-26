/* the waterfall, TAKI in White Jungle's object list: a model of two parts.
 * the first part's texture scrolls along v */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg04/taki/text.inc"

#include "draw/texture_anim.h"
#include "object/set.h"
#include "stg04/taki.h"
#include "system/game_state.h"
#include "system/task.h"

#include "stg04/waterfall.h"

NJS_TEXLIST *taki_texlists[2] = {
#include "stg04/taki/taki_texlists.inc"
};

static void taki_destroy(struct task *task);
static void taki_step(struct task *task);
static void taki_draw(struct task *task);

void taki_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = taki_draw;
        task->step = taki_step;
        task->destructor = taki_destroy;
    }
}

static void taki_destroy(struct task *task)
{
}

static void taki_step(struct task *task)
{
    /* @hack redundant: a test of placement_try_leave's result whose branch
     * does what falling through does */
    if (placement_try_leave(task)) {
        return;
    }
}

static void taki_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object = &taki_object;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)WATERFALL_EDITOR_ROT_Y;
        (void)WATERFALL_EDITOR_SPD;
    }
    njSetTexture(&taki_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    texture_anim_scroll(object->model, &taki_anim, 0,
                        ((Sint16)game_state.steps * (Sint16)work->scl.x) &
                            (SCROLL_WRAP - 1));
    njCnkEasyDrawModel(object->model);
    object = object->child;
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
}
