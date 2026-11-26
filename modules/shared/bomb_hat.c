/* the bomb hat, a task named TBombHatExec: a ring of the bombfire texture.
 * the ring grows about a point, and fades as it grows. item_box.c's
 * explosions draw the same ring through bomb_hat_draw_ring */
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"
#include "shared/bomb_hat.h"
#include "shared/common_object.h"
#include "system/game_state.h"
#include "system/task.h"
#include "shared/bomb_hat/text.inc"

#define BOMB_HAT_LEVEL 2
/* the ring's turn about y for each step game_state.steps counts */
#define TURN_PER_STEP 128
/* while the ring draws, its model's r is the ring's radius plus R_MARGIN.
 * Ninja's model clip tests that r */
#define R_MARGIN 10.0F

/* a hat's work keeps the ring's radius in scl.x. it starts at 0 and grows by
 * GROWTH a step. the hat ends once the radius passes LIMIT. its mode keeps
 * game_state.time_stopped as the hat found it */
#define RADIUS(work) ((work)->scl.x)
#define GROWTH 10.0F
#define LIMIT(work) ((work)->scl.y)
#define MADE_IN_TIME_STOP(work) ((work)->mode)

/* make a hat at pos, whose ring grows until its radius passes limit. the ring's
 * fade is its radius over limit. it returns the task, or null where
 * task_create does. nothing calls bomb_hat_create. it isn't static, since SHC
 * leaves out a static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
struct task *bomb_hat_create(NJS_POINT3 *pos, Float limit);
/* NOLINTEND(misc-use-internal-linkage) */

static NJS_TEXNAME bomb_hat_texnames[1] = {
#include "shared/bomb_hat/bomb_hat_texnames.inc"
};
NJS_TEXLIST bomb_hat_texlist = {
#include "shared/bomb_hat/bomb_hat_texlist.inc"
};
/* the ring's model: a band of RING_POINTS points about the y axis. its outer
 * edge has a radius of 10.6 at a y of 0. its inner edge has a radius of 9.7
 * at a y of 0.31 */
#define RING_POINTS 32
static Sint16 bomb_hat_polygons[115] = {
#include "shared/bomb_hat/bomb_hat_polygons.inc"
};
static Sint32 bomb_hat_vertices[195] = {
#include "shared/bomb_hat/bomb_hat_vertices.inc"
};
static NJS_CNK_MODEL bomb_hat_model = {
#include "shared/bomb_hat/bomb_hat_model.inc"
};
static NJS_CNK_OBJECT bomb_hat_object = {
#include "shared/bomb_hat/bomb_hat_object.inc"
};
/* the ring's packed ARGB colors at a fade of 0 and of 1 */
static Uint32 bomb_hat_color_start = 0xffffffff;
static Uint32 bomb_hat_color_end = 0x10ffff80;

void bomb_hat_draw_ring(Float radius, Float fade)
{
    Float model_r;
    /* @hack spelling: bound, a local for radius + R_MARGIN, where the sum
     * stored in place would do */
    Float bound;
    Float scale;

    model_r = bomb_hat_model.r;
    bound = radius + R_MARGIN;
    bomb_hat_model.r = bound;
    scale = radius / model_r;
    njSetTexture(&bomb_hat_texlist);
    njPushMatrixEx();
    njScale(NULL, scale, scale, scale);
    model_draw_colored(&bomb_hat_model, color_blend(bomb_hat_color_start,
                                                    bomb_hat_color_end, fade));
    njPopMatrixEx();
    bomb_hat_model.r = model_r;
}

static void bomb_hat_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, game_state.steps * TURN_PER_STEP);
    bomb_hat_draw_ring(RADIUS(work), RADIUS(work) / LIMIT(work));
    njPopMatrixEx();
}

/* a hat made during a time stop grows through it. any other hat holds still
 * during one */
static void bomb_hat_step(struct task *task)
{
    struct task_work *work = task->work;

    if (MADE_IN_TIME_STOP(work) || !game_state.time_stopped) {
        RADIUS(work) += GROWTH;
    }
    if (RADIUS(work) > LIMIT(work)) {
        task_kill(task);
    }
}

struct task *bomb_hat_create(NJS_POINT3 *pos, Float limit)
{
    struct task *task;
    struct task_work *work;

    task =
        task_create(TASK_WORK, BOMB_HAT_LEVEL, bomb_hat_step, "TBombHatExec");
    if (task != NULL) {
        work = task->work;
        task->draw = bomb_hat_draw;
        /* @hack redundant: the step set again, though task_create set it */
        task->step = bomb_hat_step;
        work->pos = *pos;
        LIMIT(work) = limit;
        RADIUS(work) = 0.0F;
        MADE_IN_TIME_STOP(work) = game_state.time_stopped;
    }
    return task;
}
