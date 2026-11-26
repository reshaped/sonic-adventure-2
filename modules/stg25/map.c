/* Death Chamber's map, MAP in the stage's object list: a map on a wall, of
 * three kinds. its overlay cycles through three pictures. two layers of
 * letters scroll and rest by turns. a dot pulses where the placement puts
 * it */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "draw/color.h"
#include "draw/texture_anim.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "shared/tint_pulse.h"
#include "stg25/chamber_util.h"
#include "stg25/death_chamber.h"
#include "stg25/map.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

enum chamber_map_kind { KIND_RED, KIND_GREEN, KIND_BLUE, KINDS };
#define KIND(work) ((int)_builtin_fabsf((work)->scl.z) % KINDS)

/* the overlay shows death_chamber_map_texlist's PICTURES textures in turn, each
 * for PICTURE_STEPS of game_state.steps. OVERLAY_TID indexes the halfword of
 * the overlay's polygon list that holds its texture's id. it is the second
 * halfword of the list's first NJD_CT_TID chunk. the draw puts the picture
 * there, and keeps the halfword's NJD_FFM_MASK bits */
#define PICTURES 3
#define PICTURE_STEPS 5
#define OVERLAY_TID 7

/* a layer of letters over the map, which scrolls and rests by turns. the step
 * counts timer down, and the draw starts the next spell once timer reaches 0 */
struct chamber_map_layer {
    Sint16 resting;
    Sint16 timer;
    /* the texture's v offset that texture_anim_scroll moves the letters by. it
     * gains SCROLL_STEP, modulo LAYER_SCROLL_WRAP, each step the layer
     * scrolls */
    Sint16 scroll;
};
/* task's extra: the layers of chamber_map_letters_1_model and
 * chamber_map_letters_2_model */
struct chamber_map_layers {
    struct chamber_map_layer letters_1;
    struct chamber_map_layer letters_2;
};
#define LAYERS(task) ((struct chamber_map_layers *)(task)->extra.p)
#define SCROLL_STEP 2
#define LAYER_SCROLL_WRAP 256
/* a spell's steps: base, plus up to spread more at random */
#define SPELL_STEPS(base, spread)                                              \
    (((Float)rand() * (spread) * RAND_SCALE) + (base))

/* the dot pulses with the scenery's tint, as shared/tint_pulse.h times it.
 * its gray, from 0 to 1, times CHANNEL_MAX is each byte of its color */
#define CHANNEL_MAX 255.0F
/* the gray rises from DOT_DIM by DOT_RISE a step, over the pulse's first
 * TINT_PULSE_RISE steps. it then falls from 1 by DOT_FALL a step, to DOT_DIM
 * at the pulse's last step */
#define DOT_DIM 0.2F
#define DOT_RISE 0.2F
#define DOT_FALL ((1.0F - DOT_DIM) / ((TINT_PULSE_STEPS - 1) - TINT_PULSE_RISE))
/* the dot lies at the work's scl.x and scl.y, DOT_Z in front of the map. it
 * is DOT_SIZE times its model's size across */
#define DOT_Z 4.001F
#define DOT_SIZE 7.0F

#define MAP_SHAPES 1

#include "stg25/map/text.inc"

static NJS_TEXNAME chamber_map_red_texture_names[3] = {
#include "stg25/map/chamber_map_red_texture_names.inc"
};

static NJS_TEXLIST chamber_map_red_texlist = {
#include "stg25/map/chamber_map_red_texlist.inc"
};

static Sint16 chamber_map_red_polygons[511] = {
#include "stg25/map/chamber_map_red_polygons.inc"
};

static Sint32 chamber_map_red_vertices[515] = {
#include "stg25/map/chamber_map_red_vertices.inc"
};

static NJS_CNK_MODEL chamber_map_red_model = {
#include "stg25/map/chamber_map_red_model.inc"
};

static NJS_CNK_OBJECT chamber_map_red_object = {
#include "stg25/map/chamber_map_red_object.inc"
};

static Sint16 chamber_map_red_overlay_polygons[25] = {
#include "stg25/map/chamber_map_red_overlay_polygons.inc"
};

static Sint32 chamber_map_red_overlay_vertices[19] = {
#include "stg25/map/chamber_map_red_overlay_vertices.inc"
};

static NJS_CNK_MODEL chamber_map_red_overlay = {
#include "stg25/map/chamber_map_red_overlay.inc"
};

static NJS_CNK_OBJECT chamber_map_red_overlay_object = {
#include "stg25/map/chamber_map_red_overlay_object.inc"
};

static NJS_CNK_OBJECT chamber_map_red_overlay_assembly = {
#include "stg25/map/chamber_map_red_overlay_assembly.inc"
};

static NJS_TEXNAME chamber_map_green_texture_names[3] = {
#include "stg25/map/chamber_map_green_texture_names.inc"
};

static NJS_TEXLIST chamber_map_green_texlist = {
#include "stg25/map/chamber_map_green_texlist.inc"
};

static Sint16 chamber_map_green_overlay_polygons[25] = {
#include "stg25/map/chamber_map_green_overlay_polygons.inc"
};

static Sint32 chamber_map_green_overlay_vertices[19] = {
#include "stg25/map/chamber_map_green_overlay_vertices.inc"
};

static NJS_CNK_MODEL chamber_map_green_overlay = {
#include "stg25/map/chamber_map_green_overlay.inc"
};

static NJS_CNK_OBJECT chamber_map_green_object_sibling = {
#include "stg25/map/chamber_map_green_object_sibling.inc"
};

static Sint16 chamber_map_green_polygons[511] = {
#include "stg25/map/chamber_map_green_polygons.inc"
};

static Sint32 chamber_map_green_vertices[515] = {
#include "stg25/map/chamber_map_green_vertices.inc"
};

static NJS_CNK_MODEL chamber_map_green_model = {
#include "stg25/map/chamber_map_green_model.inc"
};

static NJS_CNK_OBJECT chamber_map_green_object = {
#include "stg25/map/chamber_map_green_object.inc"
};

static NJS_CNK_OBJECT chamber_map_green_assembly = {
#include "stg25/map/chamber_map_green_assembly.inc"
};

static NJS_TEXNAME chamber_map_blue_texture_names[3] = {
#include "stg25/map/chamber_map_blue_texture_names.inc"
};

static NJS_TEXLIST chamber_map_blue_texlist = {
#include "stg25/map/chamber_map_blue_texlist.inc"
};

static Sint16 chamber_map_blue_overlay_polygons[25] = {
#include "stg25/map/chamber_map_blue_overlay_polygons.inc"
};

static Sint32 chamber_map_blue_overlay_vertices[19] = {
#include "stg25/map/chamber_map_blue_overlay_vertices.inc"
};

static NJS_CNK_MODEL chamber_map_blue_overlay = {
#include "stg25/map/chamber_map_blue_overlay.inc"
};

static NJS_CNK_OBJECT chamber_map_blue_object_sibling = {
#include "stg25/map/chamber_map_blue_object_sibling.inc"
};

static Sint16 chamber_map_blue_polygons[517] = {
#include "stg25/map/chamber_map_blue_polygons.inc"
};

static Sint32 chamber_map_blue_vertices[519] = {
#include "stg25/map/chamber_map_blue_vertices.inc"
};

static NJS_CNK_MODEL chamber_map_blue_model = {
#include "stg25/map/chamber_map_blue_model.inc"
};

static NJS_CNK_OBJECT chamber_map_blue_object = {
#include "stg25/map/chamber_map_blue_object.inc"
};

static NJS_CNK_OBJECT chamber_map_blue_assembly = {
#include "stg25/map/chamber_map_blue_assembly.inc"
};

static NJS_TEXNAME chamber_map_dot_texture_names[1] = {
#include "stg25/map/chamber_map_dot_texture_names.inc"
};

static NJS_TEXLIST chamber_map_dot_texlist = {
#include "stg25/map/chamber_map_dot_texlist.inc"
};

static Sint16 chamber_map_dot_polygons[25] = {
#include "stg25/map/chamber_map_dot_polygons.inc"
};

static Sint32 chamber_map_dot_vertices[19] = {
#include "stg25/map/chamber_map_dot_vertices.inc"
};

static NJS_CNK_MODEL chamber_map_dot_model = {
#include "stg25/map/chamber_map_dot_model.inc"
};

static NJS_CNK_OBJECT chamber_map_dot_object = {
#include "stg25/map/chamber_map_dot_object.inc"
};

static Sint16 chamber_map_letters_1_uv_keys[12] = {
#include "stg25/map/chamber_map_letters_1_uv_keys.inc"
};

static struct texture_anim chamber_map_letters_1_anim = {
#include "stg25/map/chamber_map_letters_1_anim.inc"
};

static NJS_TEXNAME chamber_map_letters_1_texture_names[1] = {
#include "stg25/map/chamber_map_letters_1_texture_names.inc"
};

static NJS_TEXLIST chamber_map_letters_1_texlist = {
#include "stg25/map/chamber_map_letters_1_texlist.inc"
};

static Sint16 chamber_map_letters_1_polygons[25] = {
#include "stg25/map/chamber_map_letters_1_polygons.inc"
};

static Sint32 chamber_map_letters_1_vertices[19] = {
#include "stg25/map/chamber_map_letters_1_vertices.inc"
};

static NJS_CNK_MODEL chamber_map_letters_1_model = {
#include "stg25/map/chamber_map_letters_1_model.inc"
};

static NJS_CNK_OBJECT chamber_map_letters_1_object = {
#include "stg25/map/chamber_map_letters_1_object.inc"
};

static Sint16 chamber_map_letters_2_uv_keys[12] = {
#include "stg25/map/chamber_map_letters_2_uv_keys.inc"
};

static struct texture_anim chamber_map_letters_2_anim = {
#include "stg25/map/chamber_map_letters_2_anim.inc"
};

static NJS_TEXNAME chamber_map_letters_2_texture_names[1] = {
#include "stg25/map/chamber_map_letters_2_texture_names.inc"
};

static NJS_TEXLIST chamber_map_letters_2_texlist = {
#include "stg25/map/chamber_map_letters_2_texlist.inc"
};

static Sint16 chamber_map_letters_2_polygons[25] = {
#include "stg25/map/chamber_map_letters_2_polygons.inc"
};

static Sint32 chamber_map_letters_2_vertices[19] = {
#include "stg25/map/chamber_map_letters_2_vertices.inc"
};

static NJS_CNK_MODEL chamber_map_letters_2_model = {
#include "stg25/map/chamber_map_letters_2_model.inc"
};

static NJS_CNK_OBJECT chamber_map_letters_2_object = {
#include "stg25/map/chamber_map_letters_2_object.inc"
};

static void chamber_map_destroy(struct task *task);
static void chamber_map_step(struct task *task);
static void chamber_map_draw(struct task *task);

KEPT static NJS_CNK_MODEL *chamber_map_models[KINDS] = {
#include "stg25/map/chamber_map_models.inc"
};
KEPT static NJS_CNK_MODEL *chamber_map_overlays[KINDS] = {
#include "stg25/map/chamber_map_overlays.inc"
};
NJS_TEXLIST *chamber_map_texlists[KINDS + 4] = {
#include "stg25/map/chamber_map_texlists.inc"
};
static char *chamber_map_kind_names[KINDS] = {
#include "stg25/map/chamber_map_kind_names.inc"
};
static Sint32 chamber_map_scroll_spread = 80;
static Sint32 chamber_map_scroll_base = 40;
static Sint32 chamber_map_rest_spread = 15;
static Sint32 chamber_map_rest_base = 15;
/* clang-format off */
static struct collision_shape chamber_map_shapes[MAP_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(SHAPE_TOP_LEVEL), SHAPE_UNK_2000, {0.0F, 0.0F, 0.0F},
     20.05F, 15.05F, 6.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* TRUE once one call of chamber_map_build has compiled all three kinds'
 * maps */
static Bool chamber_map_built = FALSE;
static struct model_build chamber_map_builds[KINDS] = {
#include "stg25/map/chamber_map_builds.inc"
};

Uint8 *chamber_map_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, chamber_map_builds, KINDS) == KINDS) {
        chamber_map_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void chamber_map_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = chamber_map_draw;
    task->step = chamber_map_step;
    task->destructor = chamber_map_destroy;
    LAYERS(task)->letters_1.resting = FALSE;
    LAYERS(task)->letters_1.timer =
        SPELL_STEPS(chamber_map_scroll_base, chamber_map_scroll_spread);
    LAYERS(task)->letters_2.resting = FALSE;
    LAYERS(task)->letters_2.timer =
        SPELL_STEPS(chamber_map_scroll_base, chamber_map_scroll_spread);
    collision_init(task, chamber_map_shapes, MAP_SHAPES, COLLISION_OBJECT);
}

static void chamber_map_destroy(struct task *task)
{
}

static void chamber_map_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    collision_register(task);
    /* @hack spelling: resting tested against FALSE, where ! would do */
    if (LAYERS(task)->letters_1.resting == FALSE) {
        LAYERS(task)->letters_1.scroll += SCROLL_STEP;
        LAYERS(task)->letters_1.scroll %= LAYER_SCROLL_WRAP;
    }
    if (LAYERS(task)->letters_2.resting == FALSE) {
        LAYERS(task)->letters_2.scroll += SCROLL_STEP;
        LAYERS(task)->letters_2.scroll %= LAYER_SCROLL_WRAP;
    }
    LAYERS(task)->letters_1.timer--;
    LAYERS(task)->letters_2.timer--;
}

static void chamber_map_draw(struct task *task)
{
    struct task_work *work = task->work;
    int phase = game_state.steps % TINT_PULSE_STEPS;
    Uint32 color;
    int texture;
    Sint16 chunk;

    /* @hack type: the gray converted through int, where a conversion to
     * Uint32 would do */
    if (phase < TINT_PULSE_RISE) {
        color = (int)((DOT_DIM + (phase * DOT_RISE)) * CHANNEL_MAX);
    } else {
        color =
            (int)((DOT_DIM + (((TINT_PULSE_STEPS - 1) - phase) * DOT_FALL)) *
                  CHANNEL_MAX);
    }
    color = (color << ALPHA_SHIFT) | (color << RED_SHIFT) |
            (color << GREEN_SHIFT) | color;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    /* @hack spelling: task->work read again for scl, where work would do */
    njSetTexture(chamber_map_texlists[KIND(task->work)]);
    if (chamber_map_built) {
        njDirectDrawObject(chamber_map_builds[KIND(task->work)].built);
    } else {
        njCnkEasyDrawModel(chamber_map_models[KIND(task->work)]);
    }
    njSetTexture(&death_chamber_map_texlist);
    /* @hack spelling: chunk, a local for the halfword, where the read in
     * place would do */
    chunk = chamber_map_overlays[KIND(task->work)]->plist[OVERLAY_TID];
    texture = (chunk & NJD_FFM_MASK) |
              ((game_state.steps / PICTURE_STEPS) % PICTURES);
    chamber_map_overlays[KIND(task->work)]->plist[OVERLAY_TID] = texture;
    njCnkEasyDrawModel(chamber_map_overlays[KIND(task->work)]);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njSetTexture(&chamber_map_letters_1_texlist);
    if (!LAYERS(task)->letters_1.resting) {
        texture_anim_scroll(&chamber_map_letters_1_model,
                            &chamber_map_letters_1_anim, 0,
                            LAYERS(task)->letters_1.scroll);
        if (LAYERS(task)->letters_1.timer <= 0) {
            LAYERS(task)->letters_1.resting = TRUE;
            LAYERS(task)->letters_1.timer =
                SPELL_STEPS(chamber_map_rest_base, chamber_map_rest_spread);
        }
    } else if (LAYERS(task)->letters_1.timer <= 0) {
        LAYERS(task)->letters_1.resting = FALSE;
        LAYERS(task)->letters_1.timer =
            SPELL_STEPS(chamber_map_scroll_base, chamber_map_scroll_spread);
    }
    chamber_model_draw_tinted_additive(&chamber_map_letters_1_model,
                                       COLOR_NONE);
    njSetTexture(&chamber_map_letters_2_texlist);
    if (!LAYERS(task)->letters_2.resting) {
        texture_anim_scroll(&chamber_map_letters_2_model,
                            &chamber_map_letters_2_anim, 0,
                            LAYERS(task)->letters_2.scroll);
        if (LAYERS(task)->letters_2.timer <= 0) {
            LAYERS(task)->letters_2.resting = TRUE;
            LAYERS(task)->letters_2.timer =
                SPELL_STEPS(chamber_map_rest_base, chamber_map_rest_spread);
        }
    } else if (LAYERS(task)->letters_2.timer <= 0) {
        LAYERS(task)->letters_2.resting = FALSE;
        LAYERS(task)->letters_2.timer =
            SPELL_STEPS(chamber_map_scroll_base, chamber_map_scroll_spread);
    }
    chamber_model_draw_tinted_additive(&chamber_map_letters_2_model,
                                       COLOR_NONE);
    njFogDisable();
    njTranslate(NULL, task->work->scl.x, task->work->scl.y, DOT_Z);
    njSetTexture(&chamber_map_dot_texlist);
    njScale(NULL, DOT_SIZE, DOT_SIZE, 1.0F);
    chamber_model_draw_tinted_additive(&chamber_map_dot_model, color);
    njFogEnable();
    njPopMatrixEx();
}
