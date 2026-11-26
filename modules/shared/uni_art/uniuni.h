/* the UNIUNI uni's texture list, model and spike, whose motion spins it as it
 * circles */
#ifndef SHARED_UNI_ART_UNIUNI_H
#define SHARED_UNI_ART_UNIUNI_H

#include <ninja.h>
#include <sg_xpt.h>

enum uniuni_texture_slot {
    UNIUNI_TEX_GORA2,
    UNIUNI_TEX_G_EYE,
    UNIUNI_TEX_UNI1,
    UNIUNI_TEXTURE_COUNT
};

static NJS_TEXNAME uniuni_textures[3] = {
#include "shared/uni_art/uniuni/uniuni_textures.inc"
};

static NJS_TEXLIST uniuni_texlist = {
#include "shared/uni_art/uniuni/uniuni_texlist.inc"
};

static Sint16 uniuni_shell_polygons[141] = {
#include "shared/uni_art/uniuni/uniuni_shell_polygons.inc"
};

static Sint32 uniuni_shell_vertices[93] = {
#include "shared/uni_art/uniuni/uniuni_shell_vertices.inc"
};

static NJS_CNK_MODEL uniuni_shell_model = {
#include "shared/uni_art/uniuni/uniuni_shell_model.inc"
};

static NJS_CNK_OBJECT uniuni_shell = {
#include "shared/uni_art/uniuni/uniuni_shell.inc"
};

static Sint16 uniuni_rear_spines_polygons[291] = {
#include "shared/uni_art/uniuni/uniuni_rear_spines_polygons.inc"
};

static Sint32 uniuni_rear_spines_vertices[255] = {
#include "shared/uni_art/uniuni/uniuni_rear_spines_vertices.inc"
};

static NJS_CNK_MODEL uniuni_rear_spines_model = {
#include "shared/uni_art/uniuni/uniuni_rear_spines_model.inc"
};

static NJS_CNK_OBJECT uniuni_rear_spines = {
#include "shared/uni_art/uniuni/uniuni_rear_spines.inc"
};

static Sint16 uniuni_body_polygons[231] = {
#include "shared/uni_art/uniuni/uniuni_body_polygons.inc"
};

static Sint32 uniuni_body_vertices[279] = {
#include "shared/uni_art/uniuni/uniuni_body_vertices.inc"
};

static NJS_CNK_MODEL uniuni_body_model = {
#include "shared/uni_art/uniuni/uniuni_body_model.inc"
};

static NJS_CNK_OBJECT uniuni_object = {
#include "shared/uni_art/uniuni/uniuni_object.inc"
};

static Sint16 uniuni_spike_polygons[305] = {
#include "shared/uni_art/uniuni/uniuni_spike_polygons.inc"
};

static Sint32 uniuni_spike_vertices[231] = {
#include "shared/uni_art/uniuni/uniuni_spike_vertices.inc"
};

static NJS_CNK_MODEL uniuni_spike_model = {
#include "shared/uni_art/uniuni/uniuni_spike_model.inc"
};

static NJS_CNK_OBJECT uniuni_spike_object = {
#include "shared/uni_art/uniuni/uniuni_spike_object.inc"
};

static NJS_MKEY_A uniuni_spike_rotations[4] = {
#include "shared/uni_art/uniuni/uniuni_spike_rotations.inc"
};

static NJS_MDATA2 uniuni_spike_track[1] = {
#include "shared/uni_art/uniuni/uniuni_spike_track.inc"
};

static NJS_MOTION uniuni_spike_motion = {
#include "shared/uni_art/uniuni/uniuni_spike_motion.inc"
};

#endif
