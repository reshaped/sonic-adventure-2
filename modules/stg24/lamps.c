/* the lamps L_BLUE and L_ORANGE in Eternal Engine's object list */
#include <ninja.h>

#include "draw/texture_anim.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg24/machinery_textures.h"
#include "stg24/machinery.h"
#include "stg24/lamp_art.h"
#include "system/game_state.h"
#include "system/task.h"

/* the orange lamp blinks between no tint and LAMP_DARK, as
 * blink_level(BLINK_PERIOD, BLINK_REST, BLINK_FADE) says */
#define LAMP_DARK 0xff101010
#define BLINK_PERIOD 38
#define BLINK_REST 20
#define BLINK_FADE 6

/* the set editor's label for the lamps' ang.y */
static const char editor_rotation[12] = "<- ROT Y";

static const char miu32_cg004[12] = {
#include "stg24/lamps/miu32_cg004.inc"
};
static NJS_TEXNAME textures[1] = {
#include "stg24/lamps/textures.inc"
};
NJS_TEXLIST orange_lamp_texlist = {textures, 1};

static void lamp_destroy(struct task *task);
static void lamp_step(struct task *task);
static void orange_lamp_draw(struct task *task);
static void blue_lamp_draw(struct task *task);

void blue_lamp_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = blue_lamp_draw;
        task->step = lamp_step;
        task->destructor = lamp_destroy;
    }
}

void orange_lamp_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = orange_lamp_draw;
        task->step = lamp_step;
        task->destructor = lamp_destroy;
    }
}

static void lamp_destroy(struct task *task)
{
}

static void lamp_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
}

static void orange_lamp_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack type: color an int, where the Uint32 color_blend returns would
     * do */
    int color;

    njSetTexture(&orange_lamp_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    color = color_blend(LAMP_DARK, COLOR_NONE,
                        blink_level(BLINK_PERIOD, BLINK_REST, BLINK_FADE));
    model_draw_tinted(&orange_lamp_model, color);
    njPopMatrixEx();
}

static void blue_lamp_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&blue_lamp_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    texture_anim_scroll(
        &blue_lamp_model, &blue_lamp_scroll,
        (game_state.steps * (int)work->scl.y) & (SCROLL_WRAP - 1), 0);
    njCnkEasyDrawModel(&blue_lamp_model);
    njPopMatrixEx();
}
