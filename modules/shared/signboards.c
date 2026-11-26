/* the signboards, SB_S and SB_L in Mission Street's object lists. an SB_S
 * board shows one of three pictures */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "shared/common_object.h"
#include "shared/signboards.h"
#include "system/task.h"

#include "shared/signboards/text.inc"

/* clang-format off */
#include "shared/mission_street/small_signboard_art.h"
#include "shared/mission_street/large_signboard_art.h"
/* clang-format on */

/* the small board's polygon list holds its texture id in word PICTURE_WORD,
 * in the bits below NJD_FFM_MASK. the pictures are the PICTURES textures of
 * signboard_small_texlist from FIRST_PICTURE on */
#define PICTURE_WORD 7
#define FIRST_PICTURE 5
#define PICTURES 3

static void signboard_destroy(struct task *task);
static void signboard_small_step(struct task *task);
static void signboard_small_draw(struct task *task);

NJS_TEXLIST *signboards_texlists[3] = {
#include "shared/signboards/signboards_texlists.inc"
};

void signboard_small_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = signboard_small_draw;
        task->step = signboard_small_step;
        task->destructor = signboard_destroy;
        work->mode = 0;
    }
}

static void signboard_destroy(struct task *task)
{
}

static void signboard_small_step(struct task *task)
{
    /* @hack redundant: the test of the result, which changes nothing */
    if (placement_try_leave(task)) {
        return;
    }
}

static void signboard_small_draw(struct task *task)
{
    struct task_work *work = task->work;
    Sint16 picture;

    njSetTexture(&signboard_small_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: the picture built in two statements, where one would
     * do */
    picture = signboard_small_model->plist[PICTURE_WORD] & NJD_FFM_MASK;
    picture |= FIRST_PICTURE + ((Uint8)work->ang.x % PICTURES);
    signboard_small_model->plist[PICTURE_WORD] = picture;
    njCnkEasyDrawModel(signboard_small_model);
    njPopMatrix(1);
}

void signboard_large_init(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- PIC(0-2)";
        (void)"<- ANGLE Y";
    }
    model_object_init(task, &signboard_large_texlist, &signboard_large_model,
                      NULL, 0, OBJECT_MAY_LEAVE);
}
