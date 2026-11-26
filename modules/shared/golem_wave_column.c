/* the shock wave's column, and two ring models that no code draws */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/golem_material/names.h"
#include "shared/golem_wave.h"

const char golem_magic_ring_textures[2][12] = {"magicring01", "magicring02"};

static NJS_TEXNAME ring_a_texture[1] = {
#include "shared/golem_wave_column/ring_a_texture.inc"
};

static NJS_TEXLIST ring_a_texlist = {
#include "shared/golem_wave_column/ring_a_texlist.inc"
};

static Sint16 ring_a_polygons[169] = {
#include "shared/golem_wave_column/ring_a_polygons.inc"
};

static Sint32 ring_a_vertices[147] = {
#include "shared/golem_wave_column/ring_a_vertices.inc"
};

static NJS_CNK_MODEL ring_a_model = {
#include "shared/golem_wave_column/ring_a_model.inc"
};

static NJS_CNK_OBJECT ring_a_object = {
#include "shared/golem_wave_column/ring_a_object.inc"
};

static NJS_TEXNAME ring_b_texture[1] = {
#include "shared/golem_wave_column/ring_b_texture.inc"
};

static NJS_TEXLIST ring_b_texlist = {
#include "shared/golem_wave_column/ring_b_texlist.inc"
};

static Sint16 ring_b_polygons[169] = {
#include "shared/golem_wave_column/ring_b_polygons.inc"
};

static Sint32 ring_b_vertices[147] = {
#include "shared/golem_wave_column/ring_b_vertices.inc"
};

static NJS_CNK_MODEL ring_b_model = {
#include "shared/golem_wave_column/ring_b_model.inc"
};

static NJS_CNK_OBJECT ring_b_object = {
#include "shared/golem_wave_column/ring_b_object.inc"
};

static NJS_TEXNAME column_texture[1] = {
#include "shared/golem_wave_column/column_texture.inc"
};

NJS_TEXLIST golem_wave_column_texlist = {
#include "shared/golem_wave_column/golem_wave_column_texlist.inc"
};

static Sint16 column_polygons[383] = {
#include "shared/golem_wave_column/column_polygons.inc"
};

static Sint32 column_vertices[363] = {
#include "shared/golem_wave_column/column_vertices.inc"
};

static NJS_CNK_MODEL column_model = {
#include "shared/golem_wave_column/column_model.inc"
};

NJS_CNK_OBJECT golem_wave_column_object = {
#include "shared/golem_wave_column/golem_wave_column_object.inc"
};

static NJS_MDATA2 column_transform_tracks[1] = {
#include "shared/golem_wave_column/column_transform_tracks.inc"
};

NJS_MOTION golem_wave_column_motion = {
#include "shared/golem_wave_column/golem_wave_column_motion.inc"
};

static NJS_POINT3 column_frame_0[60] = {
#include "shared/golem_wave_column/column_frame_0.inc"
};

static NJS_POINT3 column_frame_1[60] = {
#include "shared/golem_wave_column/column_frame_1.inc"
};

static NJS_POINT3 column_frame_2[60] = {
#include "shared/golem_wave_column/column_frame_2.inc"
};

static NJS_POINT3 column_frame_3[60] = {
#include "shared/golem_wave_column/column_frame_3.inc"
};

static NJS_POINT3 column_frame_4[60] = {
#include "shared/golem_wave_column/column_frame_4.inc"
};

static NJS_POINT3 column_frame_5[60] = {
#include "shared/golem_wave_column/column_frame_5.inc"
};

static NJS_POINT3 column_frame_6[60] = {
#include "shared/golem_wave_column/column_frame_6.inc"
};

static NJS_MKEY_P column_shape_keys[7] = {
#include "shared/golem_wave_column/column_shape_keys.inc"
};

static NJS_MDATA2 column_shape_tracks[1] = {
#include "shared/golem_wave_column/column_shape_tracks.inc"
};

NJS_MOTION golem_wave_column_shape = {
#include "shared/golem_wave_column/golem_wave_column_shape.inc"
};
