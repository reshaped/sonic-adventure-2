#include <ninja.h>
#include <sg_xpt.h>

#include "shared/tank_textures.h"
#include "stg32/scenery_shapes.h"
#include "stg32/scenery/tank/text.inc"

static NJS_TEXNAME meteor_tank_names[9] = {
#include "stg32/scenery/tank/meteor_tank_names.inc"
};

NJS_TEXLIST meteor_tank_texlist = {
#include "stg32/scenery/tank/meteor_tank_texlist.inc"
};

static Sint16 meteor_tank_object_polygons[757] = {
#include "stg32/scenery/tank/meteor_tank_object_polygons.inc"
};

static Sint32 meteor_tank_object_vertices[587] = {
#include "stg32/scenery/tank/meteor_tank_object_vertices.inc"
};

static NJS_CNK_MODEL meteor_tank_object_model = {
#include "stg32/scenery/tank/meteor_tank_object_model.inc"
};

NJS_CNK_OBJECT meteor_tank_object[1] = {
#include "stg32/scenery/tank/meteor_tank_object.inc"
};

static NJS_TEXNAME tank_base_names[3] = {
#include "stg32/scenery/tank/tank_base_names.inc"
};

NJS_TEXLIST tank_base_texlist = {
#include "stg32/scenery/tank/tank_base_texlist.inc"
};

static Sint16 tank_base_polygons[225] = {
#include "stg32/scenery/tank/tank_base_polygons.inc"
};

static Sint32 tank_base_vertices[155] = {
#include "stg32/scenery/tank/tank_base_vertices.inc"
};

static NJS_CNK_MODEL tank_base_model = {
#include "stg32/scenery/tank/tank_base_model.inc"
};

NJS_CNK_OBJECT tank_base[1] = {
#include "stg32/scenery/tank/tank_base.inc"
};

static NJS_TEXNAME tank_top_names[3] = {
#include "stg32/scenery/tank/tank_top_names.inc"
};

NJS_TEXLIST tank_top_texlist = {
#include "stg32/scenery/tank/tank_top_texlist.inc"
};

static Sint16 tank_top_piece_polygons[133] = {
#include "stg32/scenery/tank/tank_top_piece_polygons.inc"
};

static Sint32 tank_top_piece_vertices[87] = {
#include "stg32/scenery/tank/tank_top_piece_vertices.inc"
};

NJS_CNK_MODEL tank_top_piece = {
#include "stg32/scenery/tank/tank_top_piece.inc"
};

static NJS_CNK_OBJECT tank_top_object = {
#include "stg32/scenery/tank/tank_top_object.inc"
};

static NJS_TEXNAME tank_side_names[3] = {
#include "stg32/scenery/tank/tank_side_names.inc"
};

NJS_TEXLIST tank_side_texlist = {
#include "stg32/scenery/tank/tank_side_texlist.inc"
};

static Sint16 tank_side_piece_polygons[95] = {
#include "stg32/scenery/tank/tank_side_piece_polygons.inc"
};

static Sint32 tank_side_piece_vertices[51] = {
#include "stg32/scenery/tank/tank_side_piece_vertices.inc"
};

NJS_CNK_MODEL tank_side_piece = {
#include "stg32/scenery/tank/tank_side_piece.inc"
};

static NJS_CNK_OBJECT tank_side_object = {
#include "stg32/scenery/tank/tank_side_object.inc"
};

static NJS_TEXNAME tank_center_names[1] = {
#include "stg32/scenery/tank/tank_center_names.inc"
};

NJS_TEXLIST tank_center_texlist = {
#include "stg32/scenery/tank/tank_center_texlist.inc"
};

static Sint16 tank_center_piece_polygons[125] = {
#include "stg32/scenery/tank/tank_center_piece_polygons.inc"
};

static Sint32 tank_center_piece_vertices[95] = {
#include "stg32/scenery/tank/tank_center_piece_vertices.inc"
};

NJS_CNK_MODEL tank_center_piece = {
#include "stg32/scenery/tank/tank_center_piece.inc"
};

static NJS_CNK_OBJECT tank_center_object = {
#include "stg32/scenery/tank/tank_center_object.inc"
};

static Sint16 tank_shadow_polygons[113] = {
#include "stg32/scenery/tank/tank_shadow_polygons.inc"
};

struct tank_shadow_vertex_list {
    Sint32 head[2];
    NJS_POINT3 vertices[20];
    Sint32 end;
};
static struct tank_shadow_vertex_list tank_shadow_vertices = {
#include "stg32/scenery/tank/tank_shadow_vertices.inc"
};

static NJS_CNK_MODEL tank_shadow_model = {
#include "stg32/scenery/tank/tank_shadow_model.inc"
};

NJS_CNK_OBJECT tank_shadow[1] = {
#include "stg32/scenery/tank/tank_shadow.inc"
};
