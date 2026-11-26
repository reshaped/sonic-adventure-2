#ifndef SHARED_SANDGLASS_ART_H
#define SHARED_SANDGLASS_ART_H

#include <ninja.h>
#include <sg_xpt.h>

enum {
    SANDGLASS_FRAME_TEX_DZ_KANKYOU128,
    SANDGLASS_FRAME_TEX_DZ_SANDGLASS_BODY128,
    SANDGLASS_FRAME_TEX_DZ_SANDGLASS_EDGE32
};

enum {
    SANDGLASS_END_TEX_DZ_SANDGLASS_BODY128,
    SANDGLASS_END_TEX_DZ_SANDGLASS_BUTTAN128
};

enum { SANDGLASS_SAND_TEX_DZ_SANDGLASS_SAND64 };

enum { SANDGLASS_PILE_TEX_DZ_SANDGLASS_SAND64 };

enum { SANDGLASS_TUMBLE_TEX_DZ_SANDGLASS_SAND64 };

enum { SANDGLASS_STREAM_TEX_DZ_SANDGLASS_SAND64 };

static NJS_TEXNAME sandglass_frame_texture_names[3] = {
#include "shared/sandglass_art/sandglass_frame_texture_names.inc"
};

static NJS_TEXLIST sandglass_frame_texlist = {
#include "shared/sandglass_art/sandglass_frame_texlist.inc"
};

static Sint16 sandglass_frame_polygons[651] = {
#include "shared/sandglass_art/sandglass_frame_polygons.inc"
};

static Sint32 sandglass_frame_vertices[435] = {
#include "shared/sandglass_art/sandglass_frame_vertices.inc"
};

static NJS_CNK_MODEL sandglass_frame_model[1] = {
#include "shared/sandglass_art/sandglass_frame_model.inc"
};

static NJS_CNK_OBJECT sandglass_frame_object = {
#include "shared/sandglass_art/sandglass_frame_object.inc"
};

static NJS_TEXNAME sandglass_end_texture_names[2] = {
#include "shared/sandglass_art/sandglass_end_texture_names.inc"
};

static NJS_TEXLIST sandglass_end_texlist = {
#include "shared/sandglass_art/sandglass_end_texlist.inc"
};

static Sint16 sandglass_end_polygons[245] = {
#include "shared/sandglass_art/sandglass_end_polygons.inc"
};

static Sint32 sandglass_end_vertices[153] = {
#include "shared/sandglass_art/sandglass_end_vertices.inc"
};

static NJS_CNK_MODEL sandglass_end_model = {
#include "shared/sandglass_art/sandglass_end_model.inc"
};

static NJS_CNK_OBJECT sandglass_end_object = {
#include "shared/sandglass_art/sandglass_end_object.inc"
};

static NJS_MDATA3 sandglass_end_motion_tracks[1] = {
#include "shared/sandglass_art/sandglass_end_motion_tracks.inc"
};

static NJS_MOTION sandglass_end_motion = {
#include "shared/sandglass_art/sandglass_end_motion.inc"
};

static NJS_POINT3 sandglass_end_shape_vertex_frame_0[25] = {
#include "shared/sandglass_art/sandglass_end_shape_vertex_frame_0.inc"
};

static NJS_POINT3 sandglass_end_shape_vertex_frame_4[25] = {
#include "shared/sandglass_art/sandglass_end_shape_vertex_frame_4.inc"
};

static NJS_POINT3 sandglass_end_shape_vertex_frame_59[25] = {
#include "shared/sandglass_art/sandglass_end_shape_vertex_frame_59.inc"
};

static NJS_MKEY_P sandglass_end_shape_vertex_keys[3] = {
#include "shared/sandglass_art/sandglass_end_shape_vertex_keys.inc"
};

static NJS_MDATA2 sandglass_end_shape_tracks[1] = {
#include "shared/sandglass_art/sandglass_end_shape_tracks.inc"
};

static NJS_MOTION sandglass_end_shape = {
#include "shared/sandglass_art/sandglass_end_shape.inc"
};

static NJS_TEXNAME sandglass_sand_texture_names[1] = {
#include "shared/sandglass_art/sandglass_sand_texture_names.inc"
};

static NJS_TEXLIST sandglass_sand_texlist = {
#include "shared/sandglass_art/sandglass_sand_texlist.inc"
};

static Sint16 sandglass_falling_sand_polygons[65] = {
#include "shared/sandglass_art/sandglass_falling_sand_polygons.inc"
};

static Sint32 sandglass_falling_sand_vertices[39] = {
#include "shared/sandglass_art/sandglass_falling_sand_vertices.inc"
};

static NJS_CNK_MODEL sandglass_falling_sand_model = {
#include "shared/sandglass_art/sandglass_falling_sand_model.inc"
};

static NJS_CNK_OBJECT sandglass_falling_sand_object = {
#include "shared/sandglass_art/sandglass_falling_sand_object.inc"
};

static NJS_MKEY_F sandglass_falling_sand_motion_scale_keys[60] = {
#include "shared/sandglass_art/sandglass_falling_sand_motion_scale_keys.inc"
};

static NJS_MDATA3 sandglass_falling_sand_motion_tracks[1] = {
#include "shared/sandglass_art/sandglass_falling_sand_motion_tracks.inc"
};

static NJS_MOTION sandglass_falling_sand_motion = {
#include "shared/sandglass_art/sandglass_falling_sand_motion.inc"
};

static NJS_MDATA2 sandglass_falling_sand_shape_tracks[1] = {
#include "shared/sandglass_art/sandglass_falling_sand_shape_tracks.inc"
};

static NJS_MOTION sandglass_falling_sand_shape = {
#include "shared/sandglass_art/sandglass_falling_sand_shape.inc"
};

static NJS_TEXNAME sandglass_pile_texture_names[1] = {
#include "shared/sandglass_art/sandglass_pile_texture_names.inc"
};

static NJS_TEXLIST sandglass_pile_texlist = {
#include "shared/sandglass_art/sandglass_pile_texlist.inc"
};

static Sint16 sandglass_sand_polygons[127] = {
#include "shared/sandglass_art/sandglass_sand_polygons.inc"
};

static Sint32 sandglass_sand_vertices[69] = {
#include "shared/sandglass_art/sandglass_sand_vertices.inc"
};

static NJS_CNK_MODEL sandglass_sand_model = {
#include "shared/sandglass_art/sandglass_sand_model.inc"
};

static NJS_CNK_OBJECT sandglass_sand_object = {
#include "shared/sandglass_art/sandglass_sand_object.inc"
};

static NJS_MDATA3 sandglass_sand_motion_tracks[1] = {
#include "shared/sandglass_art/sandglass_sand_motion_tracks.inc"
};

static NJS_MOTION sandglass_sand_motion = {
#include "shared/sandglass_art/sandglass_sand_motion.inc"
};

static NJS_POINT3 sandglass_sand_shape_vertex_frame_0[11] = {
#include "shared/sandglass_art/sandglass_sand_shape_vertex_frame_0.inc"
};

static NJS_POINT3 sandglass_sand_shape_vertex_frame_44[11] = {
#include "shared/sandglass_art/sandglass_sand_shape_vertex_frame_44.inc"
};

static NJS_POINT3 sandglass_sand_shape_vertex_frame_59[11] = {
#include "shared/sandglass_art/sandglass_sand_shape_vertex_frame_59.inc"
};

static NJS_MKEY_P sandglass_sand_shape_vertex_keys[3] = {
#include "shared/sandglass_art/sandglass_sand_shape_vertex_keys.inc"
};

static NJS_MDATA2 sandglass_sand_shape_tracks[1] = {
#include "shared/sandglass_art/sandglass_sand_shape_tracks.inc"
};

static NJS_MOTION sandglass_sand_shape[1] = {
#include "shared/sandglass_art/sandglass_sand_shape.inc"
};

static NJS_TEXNAME sandglass_tumble_texture_names[1] = {
#include "shared/sandglass_art/sandglass_tumble_texture_names.inc"
};

static NJS_TEXLIST sandglass_tumble_texlist = {
#include "shared/sandglass_art/sandglass_tumble_texlist.inc"
};

static Sint16 sandglass_tumble_sand_polygons[143] = {
#include "shared/sandglass_art/sandglass_tumble_sand_polygons.inc"
};

static Sint32 sandglass_tumble_sand_vertices[69] = {
#include "shared/sandglass_art/sandglass_tumble_sand_vertices.inc"
};

static NJS_CNK_MODEL sandglass_tumble_sand_model = {
#include "shared/sandglass_art/sandglass_tumble_sand_model.inc"
};

static NJS_CNK_OBJECT sandglass_tumble_sand_object = {
#include "shared/sandglass_art/sandglass_tumble_sand_object.inc"
};

static NJS_MDATA3 sandglass_tumble_sand_motion_tracks[1] = {
#include "shared/sandglass_art/sandglass_tumble_sand_motion_tracks.inc"
};

static NJS_MOTION sandglass_tumble_sand_motion = {
#include "shared/sandglass_art/sandglass_tumble_sand_motion.inc"
};

static NJS_POINT3 sandglass_tumble_sand_shape_vertex_frame_0[11] = {
#include "shared/sandglass_art/sandglass_tumble_sand_shape_vertex_frame_0.inc"
};

static NJS_POINT3 sandglass_tumble_sand_shape_vertex_frame_29[11] = {
#include "shared/sandglass_art/sandglass_tumble_sand_shape_vertex_frame_29.inc"
};

static NJS_POINT3 sandglass_tumble_sand_shape_vertex_frame_49[11] = {
#include "shared/sandglass_art/sandglass_tumble_sand_shape_vertex_frame_49.inc"
};

static NJS_POINT3 sandglass_tumble_sand_shape_vertex_frame_59[11] = {
#include "shared/sandglass_art/sandglass_tumble_sand_shape_vertex_frame_59.inc"
};

static NJS_MKEY_P sandglass_tumble_sand_shape_vertex_keys[4] = {
#include "shared/sandglass_art/sandglass_tumble_sand_shape_vertex_keys.inc"
};

static NJS_MDATA2 sandglass_tumble_sand_shape_tracks[1] = {
#include "shared/sandglass_art/sandglass_tumble_sand_shape_tracks.inc"
};

static NJS_MOTION sandglass_tumble_sand_shape[1] = {
#include "shared/sandglass_art/sandglass_tumble_sand_shape.inc"
};

static NJS_TEXNAME sandglass_stream_texture_names[1] = {
#include "shared/sandglass_art/sandglass_stream_texture_names.inc"
};

static NJS_TEXLIST sandglass_stream_texlist = {
#include "shared/sandglass_art/sandglass_stream_texlist.inc"
};

static Sint16 sandglass_stream_polygons[59] = {
#include "shared/sandglass_art/sandglass_stream_polygons.inc"
};

static Sint32 sandglass_stream_vertices[81] = {
#include "shared/sandglass_art/sandglass_stream_vertices.inc"
};

static NJS_CNK_MODEL sandglass_stream_model = {
#include "shared/sandglass_art/sandglass_stream_model.inc"
};

static NJS_CNK_OBJECT sandglass_stream_object = {
#include "shared/sandglass_art/sandglass_stream_object.inc"
};

#endif
