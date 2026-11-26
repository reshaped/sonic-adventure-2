/* LASTSTEP's art in Lost Colony and Cannon's Core. it holds the deck, the two
 * sliding panels and the node drawn without fog. Eternal Engine holds its own
 */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/step_models.h"

/* clang-format off */
enum {
    LAST_STEP_TEX_MIU128_CG005,
    LAST_STEP_TEX_MIU128_CG009,
    LAST_STEP_TEX_MIU128_CG016,
    LAST_STEP_TEX_MIU128_CG017,
    LAST_STEP_TEX_MIU256_CG001,
    LAST_STEP_TEX_MIU64_CG003,
    LAST_STEP_TEX_MIU64_CG005,
    LAST_STEP_TEX_MIU64_CG006,
    LAST_STEP_TEX_MIU64_CG007,
    LAST_STEP_TEX_MIU64_CG009,
    LAST_STEP_TEX_MIU64_CG020,
    LAST_STEP_TEX_MIU64_CG021,
    LAST_STEP_TEX_MIU64_EE001
};

static Sint16 left_panel_polygons[243] = {
#include "shared/last_step_art/left_panel_polygons.inc"
};

static Sint32 left_panel_vertices[195] = {
#include "shared/last_step_art/left_panel_vertices.inc"
};

static NJS_CNK_MODEL left_panel_mesh = {
#include "shared/last_step_art/left_panel_mesh.inc"
};

NJS_CNK_OBJECT last_step_left_panel = {
#include "shared/last_step_art/last_step_left_panel.inc"
};

static Sint16 unfogged_polygons[167] = {
#include "shared/last_step_art/unfogged_polygons.inc"
};

static Sint32 unfogged_vertices[115] = {
#include "shared/last_step_art/unfogged_vertices.inc"
};

static NJS_CNK_MODEL unfogged_mesh = {
#include "shared/last_step_art/unfogged_mesh.inc"
};

NJS_CNK_OBJECT last_step_unfogged[1] = {
#include "shared/last_step_art/last_step_unfogged.inc"
};

static Sint16 right_panel_polygons[243] = {
#include "shared/last_step_art/right_panel_polygons.inc"
};

static Sint32 right_panel_vertices[195] = {
#include "shared/last_step_art/right_panel_vertices.inc"
};

static NJS_CNK_MODEL right_panel_mesh = {
#include "shared/last_step_art/right_panel_mesh.inc"
};

NJS_CNK_OBJECT last_step_right_panel = {
#include "shared/last_step_art/last_step_right_panel.inc"
};

static Sint16 deck_polygons[413] = {
#include "shared/last_step_art/deck_polygons.inc"
};

static Sint32 deck_vertices[311] = {
#include "shared/last_step_art/deck_vertices.inc"
};

NJS_CNK_MODEL last_step_model = {
#include "shared/last_step_art/last_step_model.inc"
};

static NJS_CNK_OBJECT deck_object = {
#include "shared/last_step_art/deck_object.inc"
};

#include "shared/steps/last_step_collision.h"

NJS_OBJECT last_step_collision = {
#include "shared/last_step_art/last_step_collision.inc"
};

/* clang-format on */
