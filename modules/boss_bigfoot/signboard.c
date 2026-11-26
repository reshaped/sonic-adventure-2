/* the signboards, SB_S and SIGNBOARDS in the Big Foot stage's object list */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_bigfoot/signboard.h"
#include "object/set.h"
#include "system/task.h"

/* the SIGNBOARDS board's polygon list holds its picture's texture id in word
 * PICTURE_WORD, in the bits outside NJD_FFM_MASK. the pictures are the
 * PICTURES textures of signboard_small_texlist from FIRST_PICTURE on */
#define PICTURE_WORD 7
#define FIRST_PICTURE 5
#define PICTURES 3

/* the SET editor's labels, which no code reads. each size rounds its label
 * up to a multiple of 4 bytes */
static const char editor_large_picture[12] = "<- PIC(0-1)";
static const char editor_angle_y[12] = "<- ANGLE Y";
static const char editor_small_picture[12] = "<- PIC(0-2)";

#include "boss_bigfoot/signboard/text.inc"

/* clang-format off */
#include "shared/mission_street/large_signboard_art.h"
#include "shared/mission_street/small_signboard_art.h"
/* clang-format on */

NJS_TEXLIST *signboard_texlists[4] = {
#include "boss_bigfoot/signboard/signboard_texlists.inc"
};

static void signboard_destroy(struct task *task);
static void signboard_s_step(struct task *task);
static void signboards_step(struct task *task);
static void signboard_s_draw(struct task *task);
static void signboards_draw(struct task *task);

void signboard_s_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = signboard_s_draw;
        task->step = signboard_s_step;
        task->destructor = signboard_destroy;
        work->mode = 0;
    }
}

void signboards_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = signboards_draw;
        task->step = signboards_step;
        task->destructor = signboard_destroy;
        work->mode = 0;
    }
}

static void signboard_destroy(struct task *task)
{
}

static void signboard_s_step(struct task *task)
{
    /* @hack redundant: the test of the result, which changes nothing */
    if (placement_try_leave(task)) {
        return;
    }
}

static void signboards_step(struct task *task)
{
    /* @hack redundant: the test of the result, which changes nothing */
    if (placement_try_leave(task)) {
        return;
    }
}

static void signboard_s_draw(struct task *task)
{
    struct task_work *work = task->work;

    if (work->ang.x & 1) {
        njSetTexture(&signboard_multiplayer_texlist);
    } else {
        njSetTexture(&signboard_large_texlist);
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(&signboard_large_model);
    njPopMatrix(1);
}

static void signboards_draw(struct task *task)
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
