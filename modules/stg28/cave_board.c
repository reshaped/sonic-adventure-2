/* the board, BOARD in Pyramid Cave's object list: a blue or green board under
 * a glow that pulses */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/chunk.h"
#include "draw/color.h"
#include "draw/lines.h"
#include "object/set.h"
#include "shared/tint_pulse.h"
#include "stg28/cave_board.h"
#include "stg28/cave_util.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

enum cave_board_kind { BOARD_BLUE, BOARD_GREEN, CAVE_BOARD_KINDS };
#define KIND(work) ((Sint32)njAbs((work)->scl.z) % CAVE_BOARD_KINDS)
#define SCALE(work) ((work)->scl.x + 1.0F)

#include "stg28/cave_board/text.inc"
#include "stg28/models/cave_board.h"

static void cave_board_destroy(struct task *task);
static void cave_board_step(struct task *task);
static void cave_board_draw(struct task *task);

KEPT static NJS_CNK_MODEL *cave_board_models[CAVE_BOARD_KINDS] = {
#include "stg28/cave_board/cave_board_models.inc"
};
KEPT static NJS_CNK_MODEL *cave_board_glow_models[CAVE_BOARD_KINDS] = {
#include "stg28/cave_board/cave_board_glow_models.inc"
};
NJS_TEXLIST *cave_board_texlists[CAVE_BOARD_KINDS + 1] = {
#include "stg28/cave_board/cave_board_texlists.inc"
};
static char *cave_board_kind_names[CAVE_BOARD_KINDS] = {
#include "stg28/cave_board/cave_board_kind_names.inc"
};
/* the glow pulses with the scenery's tint, as shared/tint_pulse.h times it.
 * its level rises from 0 to cave_board_glow, out of 255, and falls back to 0.
 * alpha, red, green and blue each take the level */
static Sint32 cave_board_glow = 208;

void cave_board_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = cave_board_draw;
        task->step = cave_board_step;
        task->destructor = cave_board_destroy;
    }
}

static void cave_board_destroy(struct task *task)
{
}

static void cave_board_step(struct task *task)
{
    /* @hack redundant: the test of placement_try_leave's result, and its
     * return, where the call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

static void cave_board_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;
    int step;
    Uint8 level;
    Uint32 color;

    /* @hack spelling: task->work read again for scl, where work would do */
    scale.x = scale.y = SCALE(task->work);
    scale.z = 1.0F;
    step = game_state.steps % TINT_PULSE_STEPS;
    if (step < TINT_PULSE_RISE) {
        level = (int)((Float)step * cave_board_glow *
                      (1.0F / (TINT_PULSE_RISE - 1)));
        color = (level << ALPHA_SHIFT) | (level << RED_SHIFT) |
                (level << GREEN_SHIFT) | level;
    } else {
        level = (int)((Float)(TINT_PULSE_STEPS - 1 - step) * cave_board_glow *
                      (1.0F / (TINT_PULSE_STEPS - 1 - TINT_PULSE_RISE)));
        color = (level << ALPHA_SHIFT) | (level << RED_SHIFT) |
                (level << GREEN_SHIFT) | level;
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(cave_board_texlists[KIND(task->work)]);
    njPushMatrixEx();
    cave_model_draw_scaled_v(cave_board_models[KIND(task->work)], &scale);
    njPopMatrixEx();
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(ADDITIVE_BLEND);
    njSetConstantAttr(CHUNK_TYPE_BITS, STRIP_FLAGS);
    cave_model_draw_tinted_scaled_v(cave_board_glow_models[KIND(task->work)],
                                    color, &scale);
    njSetCnkBlendMode(ALPHA_BLEND);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    njPopMatrixEx();
}
