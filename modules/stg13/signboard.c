/* the signboard, SIGNBOARD in City Escape's object list: a board that shows
 * one of five pictures */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "stg13/signboard.h"
#include "system/task.h"
#include "stg13/signboard/text.inc"

/* the board's polygon list holds its picture's texture id in word
 * PICTURE_WORD, in the bits outside NJD_FFM_MASK. the pictures are the
 * PICTURES textures of signboard_texlist from FIRST_PICTURE on */
#define PICTURE_WORD 7
#define FIRST_PICTURE 5
#define PICTURES 5

static void signboard_destroy(struct task *task);
static void signboard_step(struct task *task);
static void signboard_draw(struct task *task);

static NJS_TEXNAME signboard_texnames[10] = {
#include "stg13/signboard/signboard_texnames.inc"
};
static NJS_TEXLIST signboard_texlist = {
#include "stg13/signboard/signboard_texlist.inc"
};
static Sint16 signboard_polygons[301] = {
#include "stg13/signboard/signboard_polygons.inc"
};
static Sint32 signboard_vertices[323] = {
#include "stg13/signboard/signboard_vertices.inc"
};
/* @hack type: signboard_model an array of one, where a single model would do */
static NJS_CNK_MODEL signboard_model[1] = {
#include "stg13/signboard/signboard_model.inc"
};
static NJS_CNK_OBJECT signboard_object = {
#include "stg13/signboard/signboard_object.inc"
};
NJS_TEXLIST *signboard_texlists[2] = {
#include "stg13/signboard/signboard_texlists.inc"
};

void signboard_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = signboard_draw;
    task->step = signboard_step;
    task->destructor = signboard_destroy;
    work->mode = 0;
}

static void signboard_destroy(struct task *task)
{
}

static void signboard_step(struct task *task)
{
    /* @hack redundant: the test of placement_try_leave's result, where the
     * call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

static void signboard_draw(struct task *task)
{
    struct task_work *work = task->work;
    Sint16 picture;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- PIC(0-4)";
        (void)"<- ANGLE Y";
    }
    njSetTexture(&signboard_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: the picture built in two statements, where one would
     * do */
    picture = signboard_model->plist[PICTURE_WORD] & NJD_FFM_MASK;
    picture |= ((Uint8)work->ang.x % PICTURES) + FIRST_PICTURE;
    signboard_model->plist[PICTURE_WORD] = picture;
    njCnkEasyDrawModel(signboard_model);
    njPopMatrix(1);
}
