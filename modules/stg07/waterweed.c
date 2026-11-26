/* the waterweed, MIZUKUSA in Aquatic Mine's object list. it sways by running
 * its shape motion forth and back */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "object/set.h"
#include "stg07/waterweed.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#include "stg07/waterweed/text.inc"
#include "stg07/scenery/waterweed.h"

/* the frame of the motions, from 0 to LAST_FRAME, in the word of
 * task->extra */
/* @hack type: the frame read as a Float through extra's address, where
 * (task)->extra.f would do */
#define FRAME(task) (*(Float *)&(task)->extra)
#define LAST_FRAME 7.0F
/* the work's submode is TRUE while the frame falls by waterweed_back a step.
 * it is FALSE while the frame climbs by waterweed_forth */
#define SWAYING_BACK(work) ((work)->submode)

NJS_TEXLIST *waterweed_texlists[2] = {
#include "stg07/waterweed/waterweed_texlists.inc"
};
static Float waterweed_forth = 0.1F;
static Float waterweed_back = 0.07F;

static void waterweed_destroy(struct task *task);
static void waterweed_step(struct task *task);
static void waterweed_draw(struct task *task);

void waterweed_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        /* @hack spelling: the draw and the destructor set before the step,
         * where struct task's order would set the step first */
        task->draw = waterweed_draw;
        task->destructor = waterweed_destroy;
        task->step = waterweed_step;
        FRAME(task) = (Float)rand() * (LAST_FRAME * RAND_SCALE);
    }
}

/* task_destroy frees task->extra once the destructor returns. the waterweed's
 * holds the frame, not a block of the heap, so the destructor clears it */
static void waterweed_destroy(struct task *task)
{
    task->extra.p = NULL;
}

static void waterweed_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a debug view no path reaches, kept for its strings,
     * which SHC still places in this source's constants */
    if (0) {
        njPrintC(NJM_LOCATION(37, 15), "<- ANGLE X");
        njPrintC(NJM_LOCATION(37, 16), "<- ANGLE Y");
        njPrintC(NJM_LOCATION(37, 17), "<- ANGLE Z");
    }
    if (!placement_try_leave(task)) {
        if (SWAYING_BACK(work)) {
            FRAME(task) -= waterweed_back;
            if (FRAME(task) < 0.0F) {
                FRAME(task) = 0.0F;
                SWAYING_BACK(work) = FALSE;
            }
        } else {
            FRAME(task) += waterweed_forth;
            if (FRAME(task) > LAST_FRAME) {
                FRAME(task) = LAST_FRAME;
                SWAYING_BACK(work) = TRUE;
            }
        }
    }
}

static void waterweed_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    /* @hack redundant: a call of njSin on the frame's angle, a quarter turn
     * at LAST_FRAME, whose result nothing reads */
    njSin(FRAME(task) * (QUARTER_TURN / LAST_FRAME));
    njSetTexture(&waterweed_texlist);
    njCnkEasyDrawShapeMotion(&waterweed_object, &waterweed_motion,
                             &waterweed_shape, FRAME(task));
    njPopMatrix(1);
}
