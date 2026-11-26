#define VERTICAL_BLOCK
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/block_parts.h"
#include "shared/core_block.h"
#include "shared/stage_util.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the block's travel is scl.z plus BLOCK_TRAVEL_MARGIN times its scale. the
 * margin takes it into each portal */
#define BLOCK_TRAVEL_MARGIN 30.0F
/* BLOCK2's shadow, an index of shadow_names. 0 is none, and SHADOW_MODIFIER,
 * the set editor's MD, draws core_block_modifier */
#define LIFT_SHADOW(task) (((task)->work->ang.z & 0xff) % 2)
#define SHADOW_MODIFIER 1

/* the set editor's names for the shadow and the travel, which no code reads */
static char *shadow_names[] = {"OF", "MD"};
static char *direction_names[] = {"UP  ", "DOWN"};
static NJS_CNK_MODEL *core_block_models[] = {&core_block_body};
NJS_TEXLIST *core_lift_texlists[] = {
    &core_block_body_texlist, &core_block_shadow_texlist,
    &core_lift_upper_texlist, &core_lift_lower_texlist, NULL};
static NJS_OBJECT *core_block_collision = &core_block_land_node;
static char *block_names[] = {" "};

#include "shared/block_motion_impl.h"

static void core_block_draw(struct task *task)
{
    struct task_work *work = task->work;
#ifdef NONMATCHING
#else
    /* @hack redundant: anchor_address, task->extra held as an integer, which
     * only a test no path reaches reads, at the draw's end */
    Sint32 anchor_address;
#endif
    Float scale;

    /* @hack redundant: a block no path reaches, of matrix, model and texture
     * calls and empty tests */
    if (0) {
        njPopMatrixEx();
        njPopMatrixEx();
        njPushMatrixEx();
        njPushMatrixEx();
        model_draw_scaled(NULL, 0.0F);
        model_draw_scaled(NULL, 0.5F);
        model_draw_scaled(NULL, 0.5F);
        model_draw_scaled(NULL, 0.5F);
        njTranslate(NULL, 0.0F, 1.0F, 0.0F);
        njTranslate(NULL, 0.0F, 1.0F, 0.0F);
        njTranslate(NULL, 0.0F, 1.0F, 0.0F);
        njTranslate(NULL, 0.0F, 1.0F, 0.0F);
        njSetTexture(NULL);
        njSetTexture(NULL);
        njSetTexture(NULL);
        njSetTexture(NULL);
        if (work) {
        }
        if (work) {
        }
    }
    njPushMatrixEx();
#ifdef NONMATCHING
#else
    anchor_address = (Sint32)task->extra.p;
#endif
    njTranslate(NULL, ANCHOR(task)->origin.x, ANCHOR(task)->origin.y,
                ANCHOR(task)->origin.z);
    njRotateY(NULL, HEADING(task));
    njPushMatrixEx();
    if (!HIDE_NEAR_PORTAL(task)) {
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, -(task->work->scl.z * 0.5F), 0.0F);
        draw_control_set(DRAW_CONTROL_SHADOW);
        njSetTexture(&core_lift_lower_texlist);
        model_draw_scaled(&core_lift_lower_portal, BLOCK_SCALE(task));
        draw_control_clear(DRAW_CONTROL_SHADOW);
        core_block_portal_glow(&core_lift_lower_glow);
        njPopMatrixEx();
    }
    if (!HIDE_FAR_PORTAL(task)) {
        njTranslate(NULL, 0.0F, task->work->scl.z * 0.5F, 0.0F);
        draw_control_set(DRAW_CONTROL_SHADOW);
        njSetTexture(&core_lift_upper_texlist);
        model_draw_scaled(&core_lift_upper_portal, BLOCK_SCALE(task));
        draw_control_clear(DRAW_CONTROL_SHADOW);
        core_block_portal_glow(&core_lift_upper_glow);
    }
    njPopMatrixEx();
    njPopMatrixEx();

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, HEADING(task));
    njPushMatrixEx();
    draw_control_set(DRAW_CONTROL_SHADOW);
    if (REVERSED(task)) {
        njRotateX(NULL, HALF_TURN);
    }
    njSetTexture(core_lift_texlists[0]);
    model_draw_scaled(core_block_models[0], BLOCK_SCALE(task));
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njFogDisable();
    core_block_flash(task, &core_block_flash_early,
                     core_block_flash_period(task) * EARLY_FLASH_START);
    core_block_flash(task, &core_block_flash_late,
                     core_block_flash_period(task) * LATE_FLASH_START);
    njFogEnable();
    njPopMatrixEx();
    if (LIFT_SHADOW(task) == SHADOW_MODIFIER) {
        njPushMatrixEx();
        scale = BLOCK_SCALE(task);
        njScale(NULL, scale, scale, scale);
        njCnkModDrawModel(&core_block_modifier);
        njPopMatrixEx();
    }
    njPopMatrixEx();
#ifdef NONMATCHING
#else
    /* @hack redundant: a test of anchor_address, in a block no path
     * reaches */
    if (0) {
        if (anchor_address) {
        }
    }
#endif
}

#include "shared/block_cycle_impl.h"
