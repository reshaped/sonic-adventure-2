#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "draw/chunk.h"
#include "object/set.h"
#include "shared/ocean_util.h"
#include "stg21/swaying_light.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the steps a light's sway runs ahead of game_state.steps. its init draws
 * them at random */
#define PHASE(task) ((task)->unk_3c.l)
/* how far each sway's sine turns a step. the pitch sways once in 1024 steps,
 * and the yaw once in 512 */
#define PITCH_RATE 64
#define YAW_RATE 128
#define PITCH_AMPLITUDE ((Float)NJM_DEG_ANG(5))
#define YAW_AMPLITUDE ((Float)NJM_DEG_ANG(15))
#define REST_PITCH NJM_DEG_ANG(35)

NJS_TEXLIST *swaying_light_texlists[] = {&swaying_light_texlist, NULL};
static Uint32 beam_tint = 0x28282828;

static void swaying_light_destroy(struct task *task);
static void swaying_light_step(struct task *task);
static void swaying_light_draw(struct task *task);

void swaying_light_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = swaying_light_draw;
        task->step = swaying_light_step;
        task->destructor = swaying_light_destroy;
        /* @hack type: rand() * 2.0F, a Float product, where rand() * 2 fits
         * in an int */
        PHASE(task) = rand() * 2.0F;
    }
}

static void swaying_light_destroy(struct task *task)
{
}

static void swaying_light_step(struct task *task)
{
    /* @hack redundant: a test of the result that returns either way, where the
     * call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

static void swaying_light_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 step = game_state.steps + PHASE(task);
    Angle pitch = njSin(step * PITCH_RATE) * PITCH_AMPLITUDE;
    Angle yaw = njSin(step * YAW_RATE) * YAW_AMPLITUDE;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: the half turn toggled in by ^, where + would give
     * njRotateX the same angle */
    njRotateX(NULL, (pitch - REST_PITCH) ^ HALF_TURN);
    njRotateY(NULL, yaw);
    draw_control_save();
    draw_attr_save();
    draw_control_clear(NJD_CONTROL_3D_OFFSET_MATERIAL |
                       NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL);
    draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL |
                     NJD_CONTROL_3D_CNK_CONSTANT_ATTR |
                     NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(NJD_FBS_ONE | NJD_FBD_ONE);
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA | NJD_FST_IS);
    njSetTexture(&swaying_light_texlist);
    /* @hack spelling: the scale read through task, where work would do */
    ocean_model_draw_tinted_scaled(&swaying_light_model, beam_tint,
                                   task->work->scl.x + 1.0F);
    njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ISA);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    draw_attr_restore();
    draw_control_restore();
    njPopMatrixEx();
}
