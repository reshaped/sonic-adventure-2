/* the chimney, CHIMNEY in a stage's object list. now and then it shows ten
 * frames of its model in turn, and an explosion above it */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "object/set.h"
#include "shared/chimney.h"
#include "shared/ocean_util.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

/* the chimney's modes, in its work's mode. MODE_BLAST shows BLAST_FRAMES
 * frames, moving on a frame on each even game_state.steps, then goes back to
 * MODE_STILL */
enum chimney_mode { MODE_STILL, MODE_BLAST };
#define BLAST_FRAMES 10
/* each MODE_STILL step starts a blast with a chance of BLAST_CHANCE */
#define BLAST_CHANCE (1.0F / 300.0F)
/* the blast's frame, in task->unk_3c */
#define FRAME(task) ((task)->unk_3c.l)
/* the explosion draws BLAST_HEIGHT times the scale above the chimney's
 * pos */
#define BLAST_HEIGHT 300.0F

/* the chimney still, then in each frame of a blast */
KEPT static NJS_CNK_MODEL *chimney_models[BLAST_FRAMES + 1] = {
#include "shared/chimney/chimney_models.inc"
};
NJS_TEXLIST *chimney_texlists[12] = {
#include "shared/chimney/chimney_texlists.inc"
};
KEPT static NJS_CNK_MODEL *chimney_blast_models[BLAST_FRAMES] = {
#include "shared/chimney/chimney_blast_models.inc"
};
NJS_TEXLIST *chimney_blast_texlists[11] = {
#include "shared/chimney/chimney_blast_texlists.inc"
};
/* a word that no code reads */
static Sint32 dat_chimney_0 = 0;

static void chimney_destroy(struct task *task);
static void chimney_step(struct task *task);
static void chimney_draw(struct task *task);

void chimney_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = chimney_draw;
        task->step = chimney_step;
        task->destructor = chimney_destroy;
        FRAME(task) = 0;
        task->work->mode = MODE_STILL;
    }
}

static void chimney_destroy(struct task *task)
{
}

static void chimney_step(struct task *task)
{
    if (!placement_try_leave(task)) {
        if (task->work->mode == MODE_BLAST && !(game_state.steps & 1)) {
            if (++FRAME(task) == BLAST_FRAMES) {
                task->work->mode = MODE_STILL;
                FRAME(task) = 0;
            }
        }
        if (task->work->mode == MODE_STILL) {
            if (rand() * RAND_SCALE < BLAST_CHANCE) {
                task->work->mode = MODE_BLAST;
                FRAME(task) = 0;
            }
        }
    }
}

static void chimney_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: task->work read again, where work would do */
    if (task->work->mode == MODE_BLAST) {
        njSetTexture(chimney_texlists[FRAME(task) + 1]);
        ocean_model_draw_scaled(chimney_models[FRAME(task) + 1],
                                task->work->scl.x + 1.0F);
        njTranslate(NULL, 0.0F, BLAST_HEIGHT, 0.0F);
        njGetTranslation(NULL, &pos);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&pos);
        njRotateX(NULL, HALF_TURN);
        njFogDisable();
        draw_control_save();
        draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
        njSetCnkBlendMode(NJD_FBS_ONE | NJD_FBD_ONE);
        njSetTexture(chimney_blast_texlists[FRAME(task)]);
        ocean_model_draw_scaled(chimney_blast_models[FRAME(task)],
                                task->work->scl.x + 1.0F);
        njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ISA);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        draw_control_restore();
        njFogEnable();
        njPopMatrixEx();
    } else {
        njSetTexture(chimney_texlists[0]);
        ocean_model_draw_scaled(chimney_models[0], task->work->scl.x + 1.0F);
    }
    njPopMatrixEx();
}
