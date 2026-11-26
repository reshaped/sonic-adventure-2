/* the door light, DOOR LIGHT in Death Chamber's object list: a lamp of six
 * kinds beside a door. a glow over it pulses */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/color.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "shared/tint_pulse.h"
#include "stg25/chamber_util.h"
#include "stg25/door_light.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

enum chamber_door_light_kind {
    KIND_BLUE_3,
    KIND_GREEN_3,
    KIND_RED_3,
    KIND_BLUE_4,
    KIND_GREEN_4,
    KIND_RED_4,
    KINDS
};
#define KIND(work) ((int)_builtin_fabsf((work)->scl.z) % KINDS)

#include "stg25/door_light/text.inc"

static NJS_TEXNAME chamber_door_light_blue_3_texture_names[3] = {
#include "stg25/door_light/chamber_door_light_blue_3_texture_names.inc"
};

static NJS_TEXLIST chamber_door_light_blue_3_texlist = {
#include "stg25/door_light/chamber_door_light_blue_3_texlist.inc"
};

static Sint16 chamber_door_light_blue_3_glow_polygons[125] = {
#include "stg25/door_light/chamber_door_light_blue_3_glow_polygons.inc"
};

static Sint32 chamber_door_light_blue_3_glow_vertices[103] = {
#include "stg25/door_light/chamber_door_light_blue_3_glow_vertices.inc"
};

static NJS_CNK_MODEL chamber_door_light_blue_3_glow = {
#include "stg25/door_light/chamber_door_light_blue_3_glow.inc"
};

static NJS_CNK_OBJECT chamber_door_light_blue_3_object_sibling = {
#include "stg25/door_light/chamber_door_light_blue_3_object_sibling.inc"
};

static Sint16 chamber_door_light_blue_3_polygons[193] = {
#include "stg25/door_light/chamber_door_light_blue_3_polygons.inc"
};

static Sint32 chamber_door_light_blue_3_vertices[131] = {
#include "stg25/door_light/chamber_door_light_blue_3_vertices.inc"
};

static NJS_CNK_MODEL chamber_door_light_blue_3_model = {
#include "stg25/door_light/chamber_door_light_blue_3_model.inc"
};

static NJS_CNK_OBJECT chamber_door_light_blue_3_object = {
#include "stg25/door_light/chamber_door_light_blue_3_object.inc"
};

static NJS_CNK_OBJECT chamber_door_light_blue_3_assembly = {
#include "stg25/door_light/chamber_door_light_blue_3_assembly.inc"
};

static NJS_TEXNAME chamber_door_light_green_3_texture_names[3] = {
#include "stg25/door_light/chamber_door_light_green_3_texture_names.inc"
};

static NJS_TEXLIST chamber_door_light_green_3_texlist = {
#include "stg25/door_light/chamber_door_light_green_3_texlist.inc"
};

static Sint16 chamber_door_light_green_3_glow_polygons[125] = {
#include "stg25/door_light/chamber_door_light_green_3_glow_polygons.inc"
};

static Sint32 chamber_door_light_green_3_glow_vertices[103] = {
#include "stg25/door_light/chamber_door_light_green_3_glow_vertices.inc"
};

static NJS_CNK_MODEL chamber_door_light_green_3_glow = {
#include "stg25/door_light/chamber_door_light_green_3_glow.inc"
};

static NJS_CNK_OBJECT chamber_door_light_green_3_object_sibling = {
#include "stg25/door_light/chamber_door_light_green_3_object_sibling.inc"
};

static Sint16 chamber_door_light_green_3_polygons[193] = {
#include "stg25/door_light/chamber_door_light_green_3_polygons.inc"
};

static Sint32 chamber_door_light_green_3_vertices[131] = {
#include "stg25/door_light/chamber_door_light_green_3_vertices.inc"
};

static NJS_CNK_MODEL chamber_door_light_green_3_model = {
#include "stg25/door_light/chamber_door_light_green_3_model.inc"
};

static NJS_CNK_OBJECT chamber_door_light_green_3_object = {
#include "stg25/door_light/chamber_door_light_green_3_object.inc"
};

static NJS_CNK_OBJECT chamber_door_light_green_3_assembly = {
#include "stg25/door_light/chamber_door_light_green_3_assembly.inc"
};

static NJS_TEXNAME chamber_door_light_red_3_texture_names[3] = {
#include "stg25/door_light/chamber_door_light_red_3_texture_names.inc"
};

static NJS_TEXLIST chamber_door_light_red_3_texlist = {
#include "stg25/door_light/chamber_door_light_red_3_texlist.inc"
};

static Sint16 chamber_door_light_red_3_polygons[193] = {
#include "stg25/door_light/chamber_door_light_red_3_polygons.inc"
};

static Sint32 chamber_door_light_red_3_vertices[131] = {
#include "stg25/door_light/chamber_door_light_red_3_vertices.inc"
};

static NJS_CNK_MODEL chamber_door_light_red_3_model = {
#include "stg25/door_light/chamber_door_light_red_3_model.inc"
};

static NJS_CNK_OBJECT chamber_door_light_red_3_object = {
#include "stg25/door_light/chamber_door_light_red_3_object.inc"
};

static Sint16 chamber_door_light_red_3_glow_polygons[125] = {
#include "stg25/door_light/chamber_door_light_red_3_glow_polygons.inc"
};

static Sint32 chamber_door_light_red_3_glow_vertices[103] = {
#include "stg25/door_light/chamber_door_light_red_3_glow_vertices.inc"
};

static NJS_CNK_MODEL chamber_door_light_red_3_glow = {
#include "stg25/door_light/chamber_door_light_red_3_glow.inc"
};

static NJS_CNK_OBJECT chamber_door_light_red_3_glow_object = {
#include "stg25/door_light/chamber_door_light_red_3_glow_object.inc"
};

static NJS_CNK_OBJECT chamber_door_light_red_3_glow_assembly = {
#include "stg25/door_light/chamber_door_light_red_3_glow_assembly.inc"
};

static NJS_TEXNAME chamber_door_light_blue_4_texture_names[3] = {
#include "stg25/door_light/chamber_door_light_blue_4_texture_names.inc"
};

static NJS_TEXLIST chamber_door_light_blue_4_texlist = {
#include "stg25/door_light/chamber_door_light_blue_4_texlist.inc"
};

static Sint16 chamber_door_light_blue_4_glow_polygons[167] = {
#include "stg25/door_light/chamber_door_light_blue_4_glow_polygons.inc"
};

static Sint32 chamber_door_light_blue_4_glow_vertices[131] = {
#include "stg25/door_light/chamber_door_light_blue_4_glow_vertices.inc"
};

static NJS_CNK_MODEL chamber_door_light_blue_4_glow = {
#include "stg25/door_light/chamber_door_light_blue_4_glow.inc"
};

static NJS_CNK_OBJECT chamber_door_light_blue_4_object_sibling = {
#include "stg25/door_light/chamber_door_light_blue_4_object_sibling.inc"
};

static Sint16 chamber_door_light_blue_4_polygons[265] = {
#include "stg25/door_light/chamber_door_light_blue_4_polygons.inc"
};

static Sint32 chamber_door_light_blue_4_vertices[163] = {
#include "stg25/door_light/chamber_door_light_blue_4_vertices.inc"
};

static NJS_CNK_MODEL chamber_door_light_blue_4_model = {
#include "stg25/door_light/chamber_door_light_blue_4_model.inc"
};

static NJS_CNK_OBJECT chamber_door_light_blue_4_object = {
#include "stg25/door_light/chamber_door_light_blue_4_object.inc"
};

static NJS_CNK_OBJECT chamber_door_light_blue_4_assembly = {
#include "stg25/door_light/chamber_door_light_blue_4_assembly.inc"
};

static NJS_TEXNAME chamber_door_light_green_4_texture_names[3] = {
#include "stg25/door_light/chamber_door_light_green_4_texture_names.inc"
};

static NJS_TEXLIST chamber_door_light_green_4_texlist = {
#include "stg25/door_light/chamber_door_light_green_4_texlist.inc"
};

static Sint16 chamber_door_light_green_4_polygons[265] = {
#include "stg25/door_light/chamber_door_light_green_4_polygons.inc"
};

static Sint32 chamber_door_light_green_4_vertices[163] = {
#include "stg25/door_light/chamber_door_light_green_4_vertices.inc"
};

static NJS_CNK_MODEL chamber_door_light_green_4_model = {
#include "stg25/door_light/chamber_door_light_green_4_model.inc"
};

static NJS_CNK_OBJECT chamber_door_light_green_4_object = {
#include "stg25/door_light/chamber_door_light_green_4_object.inc"
};

static Sint16 chamber_door_light_green_4_glow_polygons[167] = {
#include "stg25/door_light/chamber_door_light_green_4_glow_polygons.inc"
};

static Sint32 chamber_door_light_green_4_glow_vertices[131] = {
#include "stg25/door_light/chamber_door_light_green_4_glow_vertices.inc"
};

static NJS_CNK_MODEL chamber_door_light_green_4_glow = {
#include "stg25/door_light/chamber_door_light_green_4_glow.inc"
};

static NJS_CNK_OBJECT chamber_door_light_green_4_glow_object = {
#include "stg25/door_light/chamber_door_light_green_4_glow_object.inc"
};

static NJS_CNK_OBJECT chamber_door_light_green_4_glow_assembly = {
#include "stg25/door_light/chamber_door_light_green_4_glow_assembly.inc"
};

static NJS_TEXNAME chamber_door_light_red_4_texture_names[3] = {
#include "stg25/door_light/chamber_door_light_red_4_texture_names.inc"
};

static NJS_TEXLIST chamber_door_light_red_4_texlist = {
#include "stg25/door_light/chamber_door_light_red_4_texlist.inc"
};

static Sint16 chamber_door_light_red_4_glow_polygons[167] = {
#include "stg25/door_light/chamber_door_light_red_4_glow_polygons.inc"
};

static Sint32 chamber_door_light_red_4_glow_vertices[131] = {
#include "stg25/door_light/chamber_door_light_red_4_glow_vertices.inc"
};

static NJS_CNK_MODEL chamber_door_light_red_4_glow = {
#include "stg25/door_light/chamber_door_light_red_4_glow.inc"
};

static NJS_CNK_OBJECT chamber_door_light_red_4_object_sibling = {
#include "stg25/door_light/chamber_door_light_red_4_object_sibling.inc"
};

static Sint16 chamber_door_light_red_4_polygons[265] = {
#include "stg25/door_light/chamber_door_light_red_4_polygons.inc"
};

static Sint32 chamber_door_light_red_4_vertices[163] = {
#include "stg25/door_light/chamber_door_light_red_4_vertices.inc"
};

static NJS_CNK_MODEL chamber_door_light_red_4_model = {
#include "stg25/door_light/chamber_door_light_red_4_model.inc"
};

static NJS_CNK_OBJECT chamber_door_light_red_4_object = {
#include "stg25/door_light/chamber_door_light_red_4_object.inc"
};

static NJS_CNK_OBJECT chamber_door_light_red_4_assembly = {
#include "stg25/door_light/chamber_door_light_red_4_assembly.inc"
};

static void chamber_door_light_destroy(struct task *task);
static void chamber_door_light_step(struct task *task);
static void chamber_door_light_draw(struct task *task);

KEPT static NJS_CNK_MODEL *chamber_door_light_models[KINDS] = {
#include "stg25/door_light/chamber_door_light_models.inc"
};
KEPT static NJS_CNK_MODEL *chamber_door_light_glows[KINDS] = {
#include "stg25/door_light/chamber_door_light_glows.inc"
};
NJS_TEXLIST *chamber_door_light_texlists[KINDS + 1] = {
#include "stg25/door_light/chamber_door_light_texlists.inc"
};
static char *chamber_door_light_kind_names[KINDS] = {
#include "stg25/door_light/chamber_door_light_kind_names.inc"
};
/* TRUE once one call of chamber_door_light_build has compiled all six
 * kinds' lamps */
static Bool chamber_door_light_built = FALSE;
static struct model_build chamber_door_light_builds[KINDS] = {
#include "stg25/door_light/chamber_door_light_builds.inc"
};

Uint8 *chamber_door_light_build(Uint8 *buf)
{
    draw_control_set(DRAW_CONTROL_SHADOW);
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, chamber_door_light_builds, KINDS) == KINDS) {
        chamber_door_light_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    return buf;
}

void chamber_door_light_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = chamber_door_light_draw;
        task->step = chamber_door_light_step;
        task->destructor = chamber_door_light_destroy;
    }
}

static void chamber_door_light_destroy(struct task *task)
{
}

static void chamber_door_light_step(struct task *task)
{
    /* @hack redundant: the test of placement_try_leave's result, and its
     * return, where the call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

static void chamber_door_light_draw(struct task *task)
{
    struct task_work *work = task->work;
    int phase = game_state.steps % TINT_PULSE_STEPS;
    Uint32 color;

    /* @hack type: the level converted through int, where a conversion to
     * Uint32 would do */
    if (phase < TINT_PULSE_RISE) {
        color = (int)(phase * TINT_PULSE_RISE_RATE);
    } else {
        color = (int)(((TINT_PULSE_STEPS - 1) - phase) * TINT_PULSE_FALL_RATE);
    }
    color = (color << ALPHA_SHIFT) | (color << RED_SHIFT) |
            (color << GREEN_SHIFT) | color;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    /* @hack spelling: task->work read again for scl, where work would do */
    njSetTexture(chamber_door_light_texlists[KIND(task->work)]);
    if (chamber_door_light_built) {
        njDirectDrawObject(chamber_door_light_builds[KIND(task->work)].built);
    } else {
        njCnkEasyDrawModel(chamber_door_light_models[KIND(task->work)]);
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    chamber_model_draw_tinted_additive(
        chamber_door_light_glows[KIND(task->work)], color);
    njPopMatrixEx();
}
