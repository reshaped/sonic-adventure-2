/* the akahige's art, drawn with E_AKAHIGETEX's textures. it holds the bomb,
 * the two kinds' models, and the pieces each breaks into */
#include <ninja.h>
#include <sg_xpt.h>
#include "draw/texture_anim.h"
#include "shared/hornet_art.h"

#include "shared/akahige_art/text.inc"

static NJS_TEXNAME akahige_textures[7] = {
#include "shared/akahige_art/akahige_textures.inc"
};

NJS_TEXLIST akahige_texlist = {
#include "shared/akahige_art/akahige_texlist.inc"
};

static Sint16 hornet_bomb_polygons[127] = {
#include "shared/akahige_art/hornet_bomb_polygons.inc"
};

static Sint32 hornet_bomb_vertices[87] = {
#include "shared/akahige_art/hornet_bomb_vertices.inc"
};

NJS_CNK_MODEL akahige_bomb_model = {
#include "shared/akahige_art/akahige_bomb_model.inc"
};

NJS_CNK_OBJECT akahige_bomb_object = {
#include "shared/akahige_art/akahige_bomb_object.inc"
};

static Sint16 akahige_bomb_uvs[32] = {
#include "shared/akahige_art/akahige_bomb_uvs.inc"
};

struct texture_anim akahige_bomb_anim = {
#include "shared/akahige_art/akahige_bomb_anim.inc"
};

static Sint16 akahige_3_rack_polygons[701] = {
#include "shared/akahige_art/akahige_3_rack_polygons.inc"
};

static Sint32 akahige_3_rack_vertices[405] = {
#include "shared/akahige_art/akahige_3_rack_vertices.inc"
};

NJS_CNK_MODEL akahige_3_piece_0 = {
#include "shared/akahige_art/akahige_3_piece_0.inc"
};

static NJS_CNK_OBJECT akahige_3_rack = {
#include "shared/akahige_art/akahige_3_rack.inc"
};

static Sint16 akahige_3_canopy_polygons[111] = {
#include "shared/akahige_art/akahige_3_canopy_polygons.inc"
};

static Sint32 akahige_3_canopy_vertices[69] = {
#include "shared/akahige_art/akahige_3_canopy_vertices.inc"
};

NJS_CNK_MODEL akahige_3_piece_1 = {
#include "shared/akahige_art/akahige_3_piece_1.inc"
};

static NJS_CNK_OBJECT akahige_3_canopy = {
#include "shared/akahige_art/akahige_3_canopy.inc"
};

static Sint16 akahige_3_nozzle_polygons[113] = {
#include "shared/akahige_art/akahige_3_nozzle_polygons.inc"
};

static Sint32 akahige_3_nozzle_vertices[57] = {
#include "shared/akahige_art/akahige_3_nozzle_vertices.inc"
};

NJS_CNK_MODEL akahige_3_piece_2 = {
#include "shared/akahige_art/akahige_3_piece_2.inc"
};

static NJS_CNK_OBJECT akahige_3_nozzle = {
#include "shared/akahige_art/akahige_3_nozzle.inc"
};

static Sint16 akahige_3_fuselage_polygons[277] = {
#include "shared/akahige_art/akahige_3_fuselage_polygons.inc"
};

static Sint32 akahige_3_fuselage_vertices[237] = {
#include "shared/akahige_art/akahige_3_fuselage_vertices.inc"
};

NJS_CNK_MODEL akahige_3_model = {
#include "shared/akahige_art/akahige_3_model.inc"
};

NJS_CNK_OBJECT akahige_3_object = {
#include "shared/akahige_art/akahige_3_object.inc"
};

static Sint16 akahige_6_canopy_polygons[111] = {
#include "shared/akahige_art/akahige_6_canopy_polygons.inc"
};

static Sint32 akahige_6_canopy_vertices[69] = {
#include "shared/akahige_art/akahige_6_canopy_vertices.inc"
};

NJS_CNK_MODEL akahige_6_piece_0 = {
#include "shared/akahige_art/akahige_6_piece_0.inc"
};

static NJS_CNK_OBJECT akahige_6_canopy = {
#include "shared/akahige_art/akahige_6_canopy.inc"
};

static Sint16 akahige_6_nozzle_polygons[113] = {
#include "shared/akahige_art/akahige_6_nozzle_polygons.inc"
};

static Sint32 akahige_6_nozzle_vertices[57] = {
#include "shared/akahige_art/akahige_6_nozzle_vertices.inc"
};

NJS_CNK_MODEL akahige_6_piece_1 = {
#include "shared/akahige_art/akahige_6_piece_1.inc"
};

static NJS_CNK_OBJECT akahige_6_nozzle = {
#include "shared/akahige_art/akahige_6_nozzle.inc"
};

static Sint16 akahige_6_rack_polygons[419] = {
#include "shared/akahige_art/akahige_6_rack_polygons.inc"
};

static Sint32 akahige_6_rack_vertices[315] = {
#include "shared/akahige_art/akahige_6_rack_vertices.inc"
};

NJS_CNK_MODEL akahige_6_piece_2 = {
#include "shared/akahige_art/akahige_6_piece_2.inc"
};

static NJS_CNK_OBJECT akahige_6_rack = {
#include "shared/akahige_art/akahige_6_rack.inc"
};

static Sint16 akahige_6_fuselage_polygons[277] = {
#include "shared/akahige_art/akahige_6_fuselage_polygons.inc"
};

static Sint32 akahige_6_fuselage_vertices[237] = {
#include "shared/akahige_art/akahige_6_fuselage_vertices.inc"
};

NJS_CNK_MODEL akahige_6_model = {
#include "shared/akahige_art/akahige_6_model.inc"
};

NJS_CNK_OBJECT akahige_6_object = {
#include "shared/akahige_art/akahige_6_object.inc"
};
