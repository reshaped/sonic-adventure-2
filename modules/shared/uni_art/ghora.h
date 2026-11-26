/* the GHORA uni's texture list, model and spike. the draw scrolls the spike's
 * texture as it circles, and the spike's glow shows the PUPURU textures */
#ifndef SHARED_UNI_ART_GHORA_H
#define SHARED_UNI_ART_GHORA_H

#include <ninja.h>
#include <sg_xpt.h>

enum ghora_texture_slot {
    GHORA_TEX_GORA1,
    GHORA_TEX_GORA2,
    GHORA_TEX_G_EYE,
    GHORA_TEX_F_BALL,
    GHORA_TEX_PUPURU1,
    GHORA_TEX_PUPURU2,
    GHORA_TEX_PUPURU3,
    GHORA_TEX_PUPURU4,
    GHORA_TEX_PUPURU5,
    GHORA_TEX_PUPURU6,
    GHORA_TEX_PUPURU7,
    GHORA_TEX_PUPURU8,
    GHORA_TEXTURE_COUNT
};

static NJS_TEXNAME ghora_textures[12] = {
#include "shared/uni_art/ghora/ghora_textures.inc"
};

static NJS_TEXLIST ghora_texlist = {
#include "shared/uni_art/ghora/ghora_texlist.inc"
};

static Sint16 ghora_shell_polygons[141] = {
#include "shared/uni_art/ghora/ghora_shell_polygons.inc"
};

static Sint32 ghora_shell_vertices[93] = {
#include "shared/uni_art/ghora/ghora_shell_vertices.inc"
};

static NJS_CNK_MODEL ghora_shell_model = {
#include "shared/uni_art/ghora/ghora_shell_model.inc"
};

static NJS_CNK_OBJECT ghora_shell = {
#include "shared/uni_art/ghora/ghora_shell.inc"
};

static Sint16 ghora_rear_spines_polygons[291] = {
#include "shared/uni_art/ghora/ghora_rear_spines_polygons.inc"
};

static Sint32 ghora_rear_spines_vertices[255] = {
#include "shared/uni_art/ghora/ghora_rear_spines_vertices.inc"
};

static NJS_CNK_MODEL ghora_rear_spines_model = {
#include "shared/uni_art/ghora/ghora_rear_spines_model.inc"
};

static NJS_CNK_OBJECT ghora_rear_spines = {
#include "shared/uni_art/ghora/ghora_rear_spines.inc"
};

static Sint16 ghora_body_polygons[251] = {
#include "shared/uni_art/ghora/ghora_body_polygons.inc"
};

static Sint32 ghora_body_vertices[279] = {
#include "shared/uni_art/ghora/ghora_body_vertices.inc"
};

static NJS_CNK_MODEL ghora_body_model = {
#include "shared/uni_art/ghora/ghora_body_model.inc"
};

static NJS_CNK_OBJECT ghora_object = {
#include "shared/uni_art/ghora/ghora_object.inc"
};

static Sint16 ghora_spike_polygons[131] = {
#include "shared/uni_art/ghora/ghora_spike_polygons.inc"
};

static Sint32 ghora_spike_vertices[99] = {
#include "shared/uni_art/ghora/ghora_spike_vertices.inc"
};

static NJS_CNK_MODEL ghora_spike_model = {
#include "shared/uni_art/ghora/ghora_spike_model.inc"
};

static NJS_CNK_OBJECT ghora_spike_object = {
#include "shared/uni_art/ghora/ghora_spike_object.inc"
};

#endif
