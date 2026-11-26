/* the Egg Golem's explosion in BOSS_GOLEM.PRS, the task
 * EffectGolemExplosionExec, which the golem's death creates. it draws
 * shared/blast.c's second shell twice, turned about y to face the camera. the
 * outer shell draws at half alpha in full color. the inner draws at full alpha
 * in half color. it throws a ring of black smoke at first_burst and
 * second_burst of its life.
 *
 * stg21/explosion.c compiles this text a second time, with switches of its
 * own. BOSS_GOLEM_E.PRS holds no copy of it */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "draw/lines.h"
#include "draw/texture_anim.h"
#include "shared/black_smoke.h"
#include "shared/blast.h"
#include "system/task.h"
#include "system/util.h"

/* the text's switches, which take BOSS_GOLEM's values where the source that
 * compiles the text defines none. EXPLOSION_DRAW_SCALED draws a shell scaled
 * by its size. EXPLOSION_RESTORE_BLEND, where defined, puts alpha blending
 * back after each shell's draw */
#ifndef EXPLOSION_CREATE
#include "boss_golem/explosion.h"
#define EXPLOSION_CREATE golem_explosion_create
#endif
#ifndef EXPLOSION_DRAW_SCALED
#include "shared/golem_util.h"
#define EXPLOSION_DRAW_SCALED golem_simple_draw_scaled
#endif
#ifndef EXPLOSION_TASK_NAME
#define EXPLOSION_TASK_NAME "EffectGolemExplosionExec"
#endif
#define EXPLOSION_LEVEL 1
#define GROWTH_KEEP 0.95F
/* blast_model_1's radius. a burst's ring takes the outer shell's radius plus
 * burst_lead steps of its present growth */
#define SHELL_RADIUS 5.0F
/* the explosion's steps to a unit of texture_anim_apply's time, for each
 * shell */
#define OUTER_TIME_STEPS 2
#define INNER_TIME_STEPS 4

struct golem_explosion {
    Uint8 unk_00[8];
    Sint32 steps;
    Float outer_size;
    Float inner_size;
    Float outer_growth;
    Float inner_growth;
    Float life; /* in steps */
};
#define EXPLOSION(task) ((struct golem_explosion *)(task)->extra.p)

static void golem_explosion_destroy(struct task *task);
static void golem_explosion_step(struct task *task);
static void golem_explosion_draw(struct task *task);

static Float inner_share = 0.95F;
/* the shares of its life at which the explosion throws a burst of smoke */
static Float first_burst = 0.125F;
static Float second_burst = 0.5F;
static Float burst_rise = 0.15F;
static Float burst_lead = 1.0F;
static Float burst_scale = 15.0F;
static Sint32 burst_count = 4;

struct task *EXPLOSION_CREATE(NJS_POINT3 *pos, Float start, Float end,
                              Float life)
{
    struct task *task;
    struct task_work *work;

    task = task_create(TASK_WORK, EXPLOSION_LEVEL, golem_explosion_step,
                       EXPLOSION_TASK_NAME);
    if (task) {
        work = task->work;
        task->extra.p = syMalloc(sizeof(struct golem_explosion));
        if (task->extra.p == NULL) {
            /* @bug memory: where the heap can't give the record, the call
             * returns null and leaves the task to step without it. the return
             * is at 0x38 of the object's code, and the step dereferences the
             * null at 0xd2 */
#ifdef NONMATCHING
            /* this side repairs the tag above: it destroys the task before it
             * returns null */
            task_destroy(task);
#endif
            return NULL;
        }
        task->draw = golem_explosion_draw;
        task->destructor = golem_explosion_destroy;
        work->pos = *pos;
        EXPLOSION(task)->steps = 0;
        EXPLOSION(task)->outer_size = start;
        EXPLOSION(task)->inner_size = inner_share * start;
        EXPLOSION(task)->outer_growth = (end - start) / life;
        EXPLOSION(task)->inner_growth = (end - start) * inner_share / life;
        EXPLOSION(task)->life = life;
    }
    return task;
}

static void golem_explosion_destroy(struct task *task)
{
    /* @hack redundant: extra tested twice, where once would do */
    if (task->extra.p) {
        if (task->extra.p) {
            syFree(task->extra.p);
            task->extra.p = NULL;
        }
    }
}

static void golem_explosion_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR spd;
    Float radius;

    EXPLOSION(task)->outer_size += EXPLOSION(task)->outer_growth;
    EXPLOSION(task)->inner_size += EXPLOSION(task)->inner_growth;
    EXPLOSION(task)->outer_growth *= GROWTH_KEEP;
    EXPLOSION(task)->inner_growth *= GROWTH_KEEP;
    texture_anim_apply(&blast_model_1, &blast_texture_anim,
                       EXPLOSION(task)->steps / OUTER_TIME_STEPS);
    if (EXPLOSION(task)->steps == (int)(EXPLOSION(task)->life * first_burst) ||
        EXPLOSION(task)->steps == (int)(EXPLOSION(task)->life * second_burst)) {
        /* @hack spelling: radius set ahead of spd, where its value in place
         * would do */
        radius = (EXPLOSION(task)->outer_size +
                  (EXPLOSION(task)->outer_growth * burst_lead)) *
                 SHELL_RADIUS;
        spd.x = 0.0F;
        spd.y = burst_rise;
        spd.z = 0.0F;
        black_smoke_rising_burst(&work->pos, &spd, radius, burst_scale,
                                 burst_count);
    }
    if (++EXPLOSION(task)->steps >= EXPLOSION(task)->life) {
        task_kill(task);
    }
}

static void golem_explosion_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float fade;
    /* @hack spelling: the draws use a local model, while the first texture
     * frame is set through the model's address */
    NJS_CNK_MODEL *model;

    fade = (EXPLOSION(task)->life - EXPLOSION(task)->steps) /
           EXPLOSION(task)->life;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, camera_views[camera_current_index]->ang.y);
    njSetTexture(&blast_texlist);
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(NJD_FBS_ONE | NJD_FBD_ISA);
    njSetConstantAttr(0xff, NJD_FST_UA | NJD_FST_IA | NJD_FST_IS | NJD_FST_IL);
    /* @hack redundant: fade times 1 for each color, where fade would do */
    constant_material_set(fade * 0.5F, fade * 1.0F, fade * 1.0F, fade * 1.0F);
    njPushMatrixEx();
    model = &blast_model_1;
    texture_anim_apply(&blast_model_1, &blast_texture_anim,
                       EXPLOSION(task)->steps / OUTER_TIME_STEPS);
    EXPLOSION_DRAW_SCALED(model, EXPLOSION(task)->outer_size);
    njPopMatrixEx();
#ifdef EXPLOSION_RESTORE_BLEND
    njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ISA);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
#endif
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(NJD_FBS_ONE | NJD_FBD_ISA);
    njSetConstantAttr(0xff, NJD_FST_UA | NJD_FST_IA | NJD_FST_IS | NJD_FST_IL);
    /* @hack redundant: fade times 1 for the alpha, where fade would do */
    constant_material_set(fade * 1.0F, fade * 0.5F, fade * 0.5F, fade * 0.5F);
    texture_anim_apply(model, &blast_texture_anim,
                       EXPLOSION(task)->steps / INNER_TIME_STEPS);
    EXPLOSION_DRAW_SCALED(model, EXPLOSION(task)->inner_size);
#ifdef EXPLOSION_RESTORE_BLEND
    njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ISA);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
#endif
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    njPopMatrixEx();
}
