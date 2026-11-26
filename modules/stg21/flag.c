#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "object/set.h"
#include "shared/ocean_flag_frames.h"
#include "shared/ocean_util.h"
#include "stg21/flag.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#ifndef HIDDEN_BASE_FLAG
#define TEN_FRAMES(first, member)                                              \
    &flag_frames[first].member, &flag_frames[(first) + 1].member,              \
        &flag_frames[(first) + 2].member, &flag_frames[(first) + 3].member,    \
        &flag_frames[(first) + 4].member, &flag_frames[(first) + 5].member,    \
        &flag_frames[(first) + 6].member, &flag_frames[(first) + 7].member,    \
        &flag_frames[(first) + 8].member, &flag_frames[(first) + 9].member
#define ALL_FRAMES(member)                                                     \
    TEN_FRAMES(0, member), TEN_FRAMES(10, member), TEN_FRAMES(20, member),     \
        TEN_FRAMES(30, member), TEN_FRAMES(40, member),                        \
        TEN_FRAMES(50, member), TEN_FRAMES(60, member),                        \
        TEN_FRAMES(70, member), TEN_FRAMES(80, member)

NJS_CNK_MODEL *ocean_flag_models[FLAG_FRAMES] = {ALL_FRAMES(cloth.model)};
NJS_TEXLIST *ocean_flag_texlists[] = {ALL_FRAMES(texlist),
                                      &flag_support_texlist, NULL};
#endif

static void ocean_flag_destroy(struct task *task);
static void ocean_flag_step(struct task *task);
static void ocean_flag_draw(struct task *task);

void ocean_flag_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = ocean_flag_draw;
        task->step = ocean_flag_step;
        task->destructor = ocean_flag_destroy;
        FLAG_PHASE(task) = rand() * (FLAG_FRAMES * RAND_SCALE);
    }
}

static void ocean_flag_destroy(struct task *task)
{
}

static void ocean_flag_step(struct task *task)
{
    /* @hack redundant: a test of the result that returns either way, where the
     * call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

static void ocean_flag_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 frame = (FLAG_PHASE(task) + game_state.steps) % FLAG_FRAMES;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njSetTexture(FLAG_CLOTH_TEXLIST(frame));
    /* @hack spelling: the scale read through task at each use, where work
     * would do */
    ocean_model_draw_scaled(ocean_flag_models[frame], task->work->scl.x + 1.0F);
    njPopMatrixEx();
    njSetTexture(&flag_support_texlist);
    ocean_model_draw_scaled(&flag_support_model, task->work->scl.x + 1.0F);
    njPopMatrixEx();
}
