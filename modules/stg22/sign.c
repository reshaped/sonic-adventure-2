#include "stg22/fixture_models.h"
#include <sg_xpt.h>
#include <stdlib.h>
#include <ninja.h>

#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg22/prop_textures.h"
#include "stg22/scenery.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

enum sign_kind { SIGN_B, SIGN_C, SIGN_D, SIGN_E, SIGN_KINDS };
/* the light moves from a sign's last panel to its first, one panel every
 * PANEL_STEPS steps. a panel brightens over its first FADE_STEPS steps lit.
 * the panel lit before it dims over the same steps. a panel draws tinted from
 * PANEL_DARK to PANEL_LIT, and not at all at UNLIT_LEVEL or below */
#define PANEL_STEPS 30
#define FADE_STEPS 8
#define PANEL_DARK 0xff000000
#define PANEL_LIT 0xffffffff
#define UNLIT_LEVEL 0.001F

NJS_TEXLIST *gadget_sign_texlists[] = {
    &gadget_sign_frames[0].list,
    &gadget_sign_frames[1].list,
    &gadget_sign_frames[2].list,
    &gadget_sign_panel_texlist,
    NULL,
};
static NJS_CNK_OBJECT *sign_models[SIGN_KINDS] = {
#include "stg22/sign/sign_models.inc"
};
static int panel_counts[SIGN_KINDS] = {4, 2, 2, 4};
/* the sign shows one of its texture lists for frame_interval + 2 steps at a
 * time. it picks the list by a whole number drawn below FRAME_DRAW, mod
 * GADGET_LIGHT_FRAMES */
#define FRAME_DRAW 100.0F
static int frame_interval = 2;

static void sign_destroy(struct task *task);
static void sign_step(struct task *task);
static void sign_draw(struct task *task);

void gadget_sign_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = sign_draw;
        task->step = sign_step;
        task->destructor = sign_destroy;
    }
}

static void sign_destroy(struct task *task)
{
}

static void sign_step(struct task *task)
{
    struct task_work *work = task->work;
    int frame;

    if (placement_try_leave(task)) {
        return;
    }
    work->submode = work->scl.x;
    if (work->submode < SIGN_B) {
        work->submode = SIGN_B;
    }
    if (work->submode > SIGN_E) {
        work->submode = SIGN_E;
    }
    if (work->timer > frame_interval) {
        work->timer = 0;
        frame = (int)(rand() * (FRAME_DRAW * RAND_SCALE)) % GADGET_LIGHT_FRAMES;
        work->tick = frame;
    } else {
        work->timer++;
    }
    /* @hack redundant: the set editor's labels, in a block no path reaches */
    if (0) {
        (void)"<- ROT X";
        (void)"<- ROT Y";
        (void)"<- ROT Z";
        (void)"<- MODEL[0-3]";
    }
}

static void sign_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *sign;
    NJS_CNK_OBJECT *panel;
    int panel_count;
    int phase;
    int lit;
    int index;
    Float level;

    sign = sign_models[work->submode];
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    if (sign->model) {
        njSetTexture(gadget_sign_texlists[work->tick]);
        njCnkEasyDrawModel(sign->model);
    } else {
        njSetTexture(&gadget_sign_panel_texlist);
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((panel = sign->child) != NULL) {
        panel_count = panel_counts[work->submode];
        phase = game_state.steps % (panel_count * PANEL_STEPS);
        lit = panel_count - (phase / PANEL_STEPS) - 1;
        phase = phase % PANEL_STEPS;
        for (index = 0; index < panel_count && panel;
             index++, panel = panel->sibling) {
            if (index == lit) {
                if (phase < FADE_STEPS) {
                    level = phase * (1.0F / FADE_STEPS);
                } else {
                    level = 1.0F;
                }
            } else if (index == (lit + 1) % panel_count) {
                if (phase < FADE_STEPS) {
                    level = (FADE_STEPS - phase) * (1.0F / FADE_STEPS);
                } else {
                    level = 0.0F;
                }
            } else {
                level = 0.0F;
            }
            if (level > UNLIT_LEVEL) {
                model_draw_tinted(panel->model,
                                  color_blend(PANEL_DARK, PANEL_LIT, level));
            }
        }
    }
    njPopMatrixEx();
}
