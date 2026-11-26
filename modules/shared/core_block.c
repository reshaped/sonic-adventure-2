#include <ninja.h>
#include <sg_xpt.h>

#include "collision/surface.h"
#include "shared/block_parts.h"
#include "shared/core_block.h"
#include "shared/stage_util.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the block's travel is scl.z plus BLOCK_TRAVEL_MARGIN times its scale. the
 * margin takes it into each portal */
#define BLOCK_TRAVEL_MARGIN 60.0F
/* the shadows SHADOW_KIND gives, the set editor's 2D and MD. a 2D shadow
 * lies SHADOW_LIFT above the floor that a probe finds from SHADOW_LIFT below
 * the block's base. the base lies BLOCK_HALF_SIZE below pos at the scale */
#define SHADOW_FLOOR 1
#define SHADOW_MODIFIER 2
#define SHADOW_LIFT 0.01F

/* the set editor's names for the shadow and the travel, which no code reads */
static char *shadow_names[] = {"OF", "2D", "MD"};
static char *direction_names[] = {"FORWARD ", "BACKWARD"};
static NJS_CNK_MODEL *core_block_models[] = {&core_block_body};
NJS_TEXLIST *core_block_texlists[] = {&core_block_body_texlist,
                                      &core_block_shadow_texlist,
                                      &core_block_portal_texlist, NULL};
static NJS_OBJECT *core_block_collision = &core_block_land_node;
static char *block_names[] = {" "};

#include "shared/block_motion_impl.h"

static void core_block_draw(struct task *task)
{
    struct task_work *work;
    struct angles normal;
    /* @hack redundant: twelve bytes nothing uses, which size the frame */
    Uint8 unused[12];
    Float probe_y;
    Float origin_y;
    Float origin_z;
    struct block_anchor *anchor;
    NJS_POINT3 *origin;
    Sint32 first_tick;
    Sint32 second_tick;

    work = task->work;
    /* @hack redundant: a block no path reaches, of reads of the clock, calls
     * and empty tests */
    if (0) {
        first_tick = game_state.steps;
        second_tick = game_state.steps;
        njPopMatrixEx();
        njPopMatrixEx();
        njPopMatrixEx();
        njPopMatrixEx();
        njSetTexture(NULL);
        njSetTexture(NULL);
        model_draw_scaled(NULL, 0.5F);
        model_draw_scaled(NULL, 0.5F);
        model_draw_scaled(NULL, 0.5F);
        njTranslate(NULL, 0.0F, 1.0F, 15.0F);
        njTranslate(NULL, 0.0F, 1.0F, 15.0F);
        njTranslate(NULL, 0.0F, 1.0F, 0.0F);
        njPushMatrixEx();
        njPushMatrixEx();
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, 1.0F, 0.0F);
        if (SHADOW_KIND(task)) {
        }
        if (work) {
        }
        if (work) {
        }
        if (work) {
        }
    }
    (void)unused;
    njPushMatrixEx();
    anchor = ANCHOR(task);
    origin = &anchor->origin;
    origin_z = origin->z;
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    origin_y = origin->y;
#else
    /* @hack type: origin's y read as the second Float of an array, where
     * origin->y would do */
    origin_y = ((Float *)origin)[1];
#endif
    njTranslate(NULL, origin->x, origin_y, origin_z);
    njRotateY(NULL, HEADING(task));
    njPushMatrixEx();
    if (!HIDE_NEAR_PORTAL(task)) {
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, 0.0F, task->work->scl.z * 0.5F);
        njRotateY(NULL, HALF_TURN);
        draw_control_set(DRAW_CONTROL_SHADOW);
        njSetTexture(&core_block_portal_texlist);
        model_draw_scaled(&core_block_portal, BLOCK_SCALE(task));
        draw_control_clear(DRAW_CONTROL_SHADOW);
        core_block_portal_glow();
        njPopMatrixEx();
    }
    if (!HIDE_FAR_PORTAL(task)) {
        njTranslate(NULL, 0.0F, 0.0F, -(task->work->scl.z * 0.5F));
        draw_control_set(DRAW_CONTROL_SHADOW);
        njSetTexture(&core_block_portal_texlist);
        model_draw_scaled(&core_block_portal, BLOCK_SCALE(task));
        draw_control_clear(DRAW_CONTROL_SHADOW);
        core_block_portal_glow();
    }
    njPopMatrixEx();
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, HEADING(task));
    njPushMatrixEx();
    draw_control_set(DRAW_CONTROL_SHADOW);
    if (REVERSED(task)) {
        njRotateX(NULL, QUARTER_TURN);
    } else {
        njRotateX(NULL, -QUARTER_TURN);
    }
    njSetTexture(core_block_texlists[0]);
    model_draw_scaled(core_block_models[0], BLOCK_SCALE(task));
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njFogDisable();
    core_block_flash(task, &core_block_flash_early,
                     core_block_flash_period(task) * EARLY_FLASH_START);
    core_block_flash(task, &core_block_flash_late,
                     core_block_flash_period(task) * LATE_FLASH_START);
    njFogEnable();
    njPopMatrixEx();
    if (SHADOW_KIND(task) == SHADOW_FLOOR) {
        if (FLOOR_Y(task) == NO_FLOOR) {
            probe_y = work->pos.y - (BLOCK_SCALE(task) * BLOCK_HALF_SIZE) +
                      -SHADOW_LIFT;
            FLOOR_Y(task) =
                surface_probe_floor(work->pos.x, probe_y, work->pos.z, &normal);
        }
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, FLOOR_Y(task) - work->pos.y + SHADOW_LIFT,
                    0.0F);
        njSetTexture(&core_block_shadow_texlist);
        model_draw_scaled(&core_block_shadow, BLOCK_SCALE(task));
        njPopMatrixEx();
    } else if (SHADOW_KIND(task) == SHADOW_MODIFIER) {
        njPushMatrixEx();
        modifier_draw_scaled(&core_block_modifier, BLOCK_SCALE(task));
        njPopMatrixEx();
    }
    njPopMatrixEx();
    /* @hack redundant: a test of anchor, in a block no path reaches */
    if (0) {
        if (anchor) {
        }
    }
    /* @hack redundant: tests of both clock reads, in a block no path
     * reaches */
    if (0) {
        if (first_tick && second_tick) {
        }
        if (first_tick && second_tick) {
        }
        if (first_tick && second_tick) {
        }
        if (first_tick && second_tick) {
        }
    }
}

#include "shared/block_cycle_impl.h"
