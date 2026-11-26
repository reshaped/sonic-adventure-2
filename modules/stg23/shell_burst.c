#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "draw/lines.h"
#include "draw/texture_anim.h"
#include "shared/blast.h"
#include "stg23/shell_burst.h"
#include "system/task.h"
#include "system/util.h"

/* a burst of two shells. each step, each shell's scale grows by its speed,
 * and drag then slows the speed. the draw shows the outer shell alone, at
 * alpha. it reads neither inner_scale nor inner_alpha, and no code reads
 * unk_00 */
struct shell_burst {
    Sint16 unk_00;
    Sint16 frame; /* the steps since the start, and the blast's texture frame */
    NJS_POINT3 pos;
    Float scale;
    Float inner_scale;
    Float speed;
    Float inner_speed;
    Float alpha;
    Float inner_alpha;
};

#define BURST(task) ((struct shell_burst *)(task)->extra.p)
#define BURST_LEVEL 3

static void burst_destroy(struct task *task);
static void burst_step(struct task *task);
static void burst_draw(struct task *task);
static void burst_init(struct task *task, struct shell_burst *burst,
                       NJS_POINT3 *pos);

static int lifetime = 30;
static Float drag = 0.95F;
static Float initial_scale = 2.0F;
static Float initial_inner_scale = 1.0F;
static Float initial_speed = 0.25F;
static Float initial_inner_speed = 0.1F;
static Float initial_alpha = 0.5F;
static Float initial_inner_alpha = 0.5F;

struct task *base_shell_burst_create(NJS_POINT3 *pos)
{
    struct task *task;
    struct shell_burst *burst;

    task = task_create(0, BURST_LEVEL, burst_step, "ExplosionExec");
    if (task) {
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((burst = syCalloc(1, sizeof(*burst))) == NULL) {
            task_destroy(task);
            return NULL;
        }
        burst_init(task, burst, pos);
        task->draw = burst_draw;
        task->destructor = burst_destroy;
        task->extra.p = burst;
    }
    return task;
}

static void burst_destroy(struct task *task)
{
    syFree(task->extra.p);
    task->extra.p = NULL;
}

static void burst_step(struct task *task)
{
    /* @hack spelling: each scale read again in its sum, where += would do */
    BURST(task)->scale = BURST(task)->scale + BURST(task)->speed;
    BURST(task)->inner_scale =
        BURST(task)->inner_scale + BURST(task)->inner_speed;
    BURST(task)->speed *= drag;
    BURST(task)->inner_speed *= drag;
    if (++BURST(task)->frame >= lifetime) {
        task_kill(task);
    }
}

static void burst_draw(struct task *task)
{
    Float scale;

    draw_control_save();
    draw_attr_save();
    draw_control_clear(NJD_CONTROL_3D_CONSTANT_ATTR |
                       NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL);
    draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
    texture_anim_apply(&blast_model_1, &blast_texture_anim, BURST(task)->frame);
    constant_material_set(BURST(task)->alpha, 1.0F, 1.0F, 1.0F);
    njPushMatrixEx();
    njTranslateEx(&BURST(task)->pos);
    scale = BURST(task)->scale;
    njScale(NULL, scale, scale, scale);
    njSetTexture(&blast_texlist);
    njCnkEasyDrawModel(&blast_model_1);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_control_restore();
    draw_attr_restore();
}

static void burst_init(struct task *task, struct shell_burst *burst,
                       NJS_POINT3 *pos)
{
    burst->frame = 0;
    burst->pos = *pos;
    burst->scale = initial_scale;
    burst->inner_scale = initial_inner_scale;
    burst->speed = initial_speed;
    burst->inner_speed = initial_inner_speed;
    burst->alpha = initial_alpha;
    burst->inner_alpha = initial_inner_alpha;
}
