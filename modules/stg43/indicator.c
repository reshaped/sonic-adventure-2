/* the sign, INDCTR in Cosmic Wall's object list: one of two housings, each
 * with a panel that scrolls */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg43/indicator/text.inc"

#include "draw/texture_anim.h"
#include "object/set.h"
#include "stg43/objects.h"
#include "system/game_state.h"
#include "system/task.h"

static NJS_TEXLIST indicator_texlist;
static NJS_TEXLIST indicator_2_texlist;
static NJS_CNK_OBJECT indicator_object;
static NJS_CNK_OBJECT indicator_2_object;
static NJS_CNK_OBJECT indicator_panel[1];
static NJS_CNK_OBJECT indicator_2_panel[1];
static struct texture_anim indicator_scroll;
static struct texture_anim indicator_2_scroll;

/* a panel's scroll at rate a step, game_state.steps times rate, modulo
 * SCROLL_WRAP */
#define SCROLL_OFFSET(rate) ((game_state.steps * (rate)) & (SCROLL_WRAP - 1))
/* a scl.x of SECOND_SIGN or more picks the second sign */
#define SECOND_SIGN 1.0F

static Sint32 scroll_u_speed;
static Sint32 scroll_v_speed;

static void indicator_destroy(struct task *task);
static void indicator_step(struct task *task);
static void indicator_draw(struct task *task);

void cosmic_indicator_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = indicator_draw;
        task->step = indicator_step;
        task->destructor = indicator_destroy;
    }
}

static void indicator_destroy(struct task *task)
{
}

static void indicator_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
}

static void indicator_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *housing;
    NJS_CNK_OBJECT *panel;

    /* @hack redundant: a block no path reaches, kept for the set editor's
     * labels, which SHC still places in this source's constants */
    if (0) {
        (void)INDICATOR_EDITOR_ANGLE_Y;
        (void)INDICATOR_EDITOR_MODEL;
    }
    if (work->scl.x >= SECOND_SIGN) {
        njSetTexture(&indicator_2_texlist);
        housing = &indicator_2_object;
        texture_anim_scroll(indicator_2_panel->model, &indicator_2_scroll,
                            SCROLL_OFFSET(scroll_u_speed),
                            SCROLL_OFFSET(scroll_v_speed));
    } else {
        njSetTexture(&indicator_texlist);
        housing = &indicator_object;
        texture_anim_scroll(indicator_panel->model, &indicator_scroll,
                            SCROLL_OFFSET(scroll_u_speed),
                            SCROLL_OFFSET(scroll_v_speed));
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(housing->model);
    panel = housing->child;
    njCnkEasyDrawModel(panel->model);
    njPopMatrixEx();
}

#include "stg43/foreground/indicator.h"

NJS_TEXLIST *cosmic_indicator_texlists[3] = {
#include "stg43/indicator/cosmic_indicator_texlists.inc"
};
/* the panels' scroll a step, along u and along v */
static Sint32 scroll_u_speed = {
#include "stg43/indicator/scroll_u_speed.inc"
};
static Sint32 scroll_v_speed = {
#include "stg43/indicator/scroll_v_speed.inc"
};
