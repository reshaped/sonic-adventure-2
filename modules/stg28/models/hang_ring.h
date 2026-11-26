#ifndef STG28_MODELS_HANG_RING_H
#define STG28_MODELS_HANG_RING_H

#include <ninja.h>
#include <sg_xpt.h>

enum { HANG_RING_TEX_DZ_PC_PLANT_ALPHA128 };

enum { HANG_RING_END_TEX_DZ_PC_CHAIN8, HANG_RING_END_TEX_DZ_PC_PLANT_ALPHA128 };

enum {
    HANG_RING_AXLE_TEX_DZ_PC_CHAIN8,
    HANG_RING_AXLE_TEX_DZ_PC_PLANT_ALPHA128
};

static NJS_TEXNAME cave_hang_ring_texture_names[1] = {
#include "stg28/models/hang_ring/cave_hang_ring_texture_names.inc"
};

static NJS_TEXLIST cave_hang_ring_texlist = {
#include "stg28/models/hang_ring/cave_hang_ring_texlist.inc"
};

static Sint16 cave_hang_ring_polygons[559] = {
#include "stg28/models/hang_ring/cave_hang_ring_polygons.inc"
};

static Sint32 cave_hang_ring_vertices[299] = {
#include "stg28/models/hang_ring/cave_hang_ring_vertices.inc"
};

static NJS_CNK_MODEL cave_hang_ring_model = {
#include "stg28/models/hang_ring/cave_hang_ring_model.inc"
};

static NJS_CNK_OBJECT cave_hang_ring_object = {
#include "stg28/models/hang_ring/cave_hang_ring_object.inc"
};

static NJS_TEXNAME cave_hang_ring_end_texture_names[2] = {
#include "stg28/models/hang_ring/cave_hang_ring_end_texture_names.inc"
};

static NJS_TEXLIST cave_hang_ring_end_texlist = {
#include "stg28/models/hang_ring/cave_hang_ring_end_texlist.inc"
};

static Sint16 cave_hang_ring_end_polygons[319] = {
#include "stg28/models/hang_ring/cave_hang_ring_end_polygons.inc"
};

static Sint32 cave_hang_ring_end_vertices[155] = {
#include "stg28/models/hang_ring/cave_hang_ring_end_vertices.inc"
};

static NJS_CNK_MODEL cave_hang_ring_end_model = {
#include "stg28/models/hang_ring/cave_hang_ring_end_model.inc"
};

static NJS_CNK_OBJECT cave_hang_ring_end_object = {
#include "stg28/models/hang_ring/cave_hang_ring_end_object.inc"
};

static NJS_TEXNAME cave_hang_ring_axle_texture_names[2] = {
#include "stg28/models/hang_ring/cave_hang_ring_axle_texture_names.inc"
};

static NJS_TEXLIST cave_hang_ring_axle_texlist = {
#include "stg28/models/hang_ring/cave_hang_ring_axle_texlist.inc"
};

static Sint16 cave_hang_ring_axle_polygons[41] = {
#include "stg28/models/hang_ring/cave_hang_ring_axle_polygons.inc"
};

static Sint32 cave_hang_ring_axle_vertices[35] = {
#include "stg28/models/hang_ring/cave_hang_ring_axle_vertices.inc"
};

static NJS_CNK_MODEL cave_hang_ring_axle_model = {
#include "stg28/models/hang_ring/cave_hang_ring_axle_model.inc"
};

static NJS_CNK_OBJECT cave_hang_ring_axle_object = {
#include "stg28/models/hang_ring/cave_hang_ring_axle_object.inc"
};

#endif
