/* NIGHTS in Radical Highway's object list: a model of six parts that light
 * in turn */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "object/set.h"
#include "shared/common_object.h"
#include "stg14/nights/text.inc"
#include "stg14/nights.h"
#include "system/game_state.h"
#include "system/task.h"

#define PARTS 6
/* nothing draws where a ball of DRAW_RADIUS about pos lies wholly behind the
 * camera */
#define DRAW_RADIUS 300.0F
#define MOTION_RECORD_SIZE 8
/* LIT counts the parts lit, from the first. the rest draw tinted unlit_color.
 * ALL_LIT, a count past PARTS, lights them all */
#define LIT(work) ((work)->submode)
#define ALL_LIT (PARTS + 1)
/* the work's tick counts the steps since the lit parts last changed. a step
 * that finds it past WAIT_TICKS changes them */
#define WAIT_TICKS 30
/* the model turns about y by ang.y plus the work's timer, which each step
 * raises by TURN_RATE */
#define TURN_RATE(work) ((work)->ang.x)
/* a Float in task->unk_3c, which no code but the step that raises it reads.
 * each change of the lit parts sets it to 0. each step then raises it by
 * UNK_3C_STEP, up to UNK_3C_MAX */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define UNK_3C(task) (*(Float *)&(task)->unk_3c)
#define UNK_3C_STEP 0.12F
#define UNK_3C_MAX 1.0F

/* MODE_FILL lights one more part at each change. MODE_FLASH holds every part
 * dark, then lights them all, then darkens them for MODE_FILL */
enum nights_mode { MODE_FILL, MODE_FLASH };

/* The six pieces of the illuminated NiGHTS sign. */

static NJS_TEXNAME nights_textures[4] = {
#include "stg14/nights/nights_textures.inc"
};

static NJS_TEXLIST nights_texlist = {
#include "stg14/nights/nights_texlist.inc"
};

static Sint16 nights_part_2_polygons[211] = {
#include "stg14/nights/nights_part_2_polygons.inc"
};

static Sint32 nights_part_2_vertices[139] = {
#include "stg14/nights/nights_part_2_vertices.inc"
};

static NJS_CNK_MODEL nights_part_2_model = {
#include "stg14/nights/nights_part_2_model.inc"
};

static NJS_CNK_OBJECT nights_part_2_object = {
#include "stg14/nights/nights_part_2_object.inc"
};

static Sint16 nights_part_1_polygons[385] = {
#include "stg14/nights/nights_part_1_polygons.inc"
};

static Sint32 nights_part_1_vertices[295] = {
#include "stg14/nights/nights_part_1_vertices.inc"
};

static NJS_CNK_MODEL nights_part_1_model = {
#include "stg14/nights/nights_part_1_model.inc"
};

static NJS_CNK_OBJECT nights_part_1_object = {
#include "stg14/nights/nights_part_1_object.inc"
};

static Sint16 nights_part_3_polygons[281] = {
#include "stg14/nights/nights_part_3_polygons.inc"
};

static Sint32 nights_part_3_vertices[159] = {
#include "stg14/nights/nights_part_3_vertices.inc"
};

static NJS_CNK_MODEL nights_part_3_model = {
#include "stg14/nights/nights_part_3_model.inc"
};

static NJS_CNK_OBJECT nights_part_3_object = {
#include "stg14/nights/nights_part_3_object.inc"
};

static Sint16 nights_part_4_polygons[121] = {
#include "stg14/nights/nights_part_4_polygons.inc"
};

static Sint32 nights_part_4_vertices[91] = {
#include "stg14/nights/nights_part_4_vertices.inc"
};

static NJS_CNK_MODEL nights_part_4_model = {
#include "stg14/nights/nights_part_4_model.inc"
};

static NJS_CNK_OBJECT nights_part_4_object = {
#include "stg14/nights/nights_part_4_object.inc"
};

static Sint16 nights_part_5_polygons[113] = {
#include "stg14/nights/nights_part_5_polygons.inc"
};

static Sint32 nights_part_5_vertices[51] = {
#include "stg14/nights/nights_part_5_vertices.inc"
};

static NJS_CNK_MODEL nights_part_5_model = {
#include "stg14/nights/nights_part_5_model.inc"
};

static NJS_CNK_OBJECT nights_part_5_object = {
#include "stg14/nights/nights_part_5_object.inc"
};

static Sint16 nights_part_0_polygons[181] = {
#include "stg14/nights/nights_part_0_polygons.inc"
};

static Sint32 nights_part_0_vertices[131] = {
#include "stg14/nights/nights_part_0_vertices.inc"
};

static NJS_CNK_MODEL nights_part_0_model = {
#include "stg14/nights/nights_part_0_model.inc"
};

static NJS_CNK_OBJECT nights_part_0_object = {
#include "stg14/nights/nights_part_0_object.inc"
};

NJS_TEXLIST *nights_texlists[2] = {
#include "stg14/nights/nights_texlists.inc"
};
static NJS_CNK_MODEL *nights_parts[PARTS] = {
#include "stg14/nights/nights_parts.inc"
};
static Uint32 unlit_color = 0xff308030;

static void nights_destroy(struct task *task);
static void nights_draw(struct task *task);
static void nights_step(struct task *task);

void nights_init(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- SPD";
        (void)"<- ANGLE Y";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, MOTION_RECORD_SIZE);
    if (task->motion.p == NULL) {
        return;
    }
    task->step = nights_step;
    task->draw = nights_draw;
    task->destructor = nights_destroy;
}

static void nights_destroy(struct task *task)
{
    /* @hack redundant: work, a local nothing reads */
    struct task_work *work = task->work;

    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void nights_draw(struct task *task)
{
    struct task_work *work = task->work;
    int i;

    if (!sphere_in_front_no_depth(&work->pos, DRAW_RADIUS)) {
        return;
    }
    njSetTexture(&nights_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y + work->timer);
    for (i = 0; i < PARTS; i++) {
        if (LIT(work) > i) {
            njFogDisable();
            njCnkEasyDrawModel(nights_parts[i]);
            njFogEnable();
        } else {
            model_draw_tinted(nights_parts[i], unlit_color);
        }
    }
    njPopMatrixEx();
}

static void nights_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    if (game_state.time_stopped) {
        return;
    }
    work->timer += TURN_RATE(work);
    switch (work->mode) {
    case MODE_FILL:
        if (work->tick++ > WAIT_TICKS) {
            work->tick = 0;
            UNK_3C(task) = 0.0F;
            LIT(work)++;
        }
        if (LIT(work) > PARTS) {
            LIT(work) = 0;
            UNK_3C(task) = 0.0F;
            work->mode = MODE_FLASH;
        }
        break;
    case MODE_FLASH:
        if (LIT(work) == 0 && work->tick++ > WAIT_TICKS) {
            UNK_3C(task) = 0.0F;
            LIT(work) = ALL_LIT;
            work->tick = 0;
        }
        if (LIT(work) >= ALL_LIT && work->tick++ > WAIT_TICKS) {
            UNK_3C(task) = 0.0F;
            LIT(work) = 0;
            work->tick = 0;
            work->mode = MODE_FILL;
        }
        break;
    }
    UNK_3C(task) += UNK_3C_STEP;
    if (UNK_3C(task) > UNK_3C_MAX) {
        UNK_3C(task) = UNK_3C_MAX;
    }
}
