/* the hornet's art, drawn with E_NAMITEX's textures. it holds the bomb, the
 * four kinds' models, and the pieces each breaks into */
#include <ninja.h>
#include <sg_xpt.h>
#include "draw/texture_anim.h"
#include "shared/hornet_art.h"

static const char hornet_texture_filenames[58] = {
#include "shared/hornet_art/hornet_texture_filenames.inc"
};

static NJS_TEXNAME hornet_textures[6] = {
#include "shared/hornet_art/hornet_textures.inc"
};

NJS_TEXLIST hornet_texlist = {
#include "shared/hornet_art/hornet_texlist.inc"
};

static Sint16 hornet_bomb_polygons[127] = {
#include "shared/hornet_art/hornet_bomb_polygons.inc"
};

static Sint32 hornet_bomb_vertices[87] = {
#include "shared/hornet_art/hornet_bomb_vertices.inc"
};

NJS_CNK_MODEL bomb_model = {
#include "shared/hornet_art/bomb_model.inc"
};

NJS_CNK_OBJECT hornet_bomb_object = {
#include "shared/hornet_art/hornet_bomb_object.inc"
};

static Sint16 hornet_bomb_uvs[32] = {
#include "shared/hornet_art/hornet_bomb_uvs.inc"
};

struct texture_anim bomb_anim = {
#include "shared/hornet_art/bomb_anim.inc"
};

static Sint16 hornet_3_rack_polygons[701] = {
#include "shared/hornet_art/hornet_3_rack_polygons.inc"
};

static Sint32 hornet_3_rack_vertices[405] = {
#include "shared/hornet_art/hornet_3_rack_vertices.inc"
};

NJS_CNK_MODEL hornet_3_piece_0 = {
#include "shared/hornet_art/hornet_3_piece_0.inc"
};

static NJS_CNK_OBJECT hornet_3_rack = {
#include "shared/hornet_art/hornet_3_rack.inc"
};

static Sint16 hornet_3_canopy_polygons[119] = {
#include "shared/hornet_art/hornet_3_canopy_polygons.inc"
};

static Sint32 hornet_3_canopy_vertices[69] = {
#include "shared/hornet_art/hornet_3_canopy_vertices.inc"
};

NJS_CNK_MODEL hornet_3_piece_1 = {
#include "shared/hornet_art/hornet_3_piece_1.inc"
};

static NJS_CNK_OBJECT hornet_3_canopy = {
#include "shared/hornet_art/hornet_3_canopy.inc"
};

static Sint16 hornet_3_nozzle_polygons[113] = {
#include "shared/hornet_art/hornet_3_nozzle_polygons.inc"
};

static Sint32 hornet_3_nozzle_vertices[57] = {
#include "shared/hornet_art/hornet_3_nozzle_vertices.inc"
};

NJS_CNK_MODEL hornet_3_piece_2 = {
#include "shared/hornet_art/hornet_3_piece_2.inc"
};

static NJS_CNK_OBJECT hornet_3_nozzle = {
#include "shared/hornet_art/hornet_3_nozzle.inc"
};

static Sint16 hornet_3_fuselage_polygons[303] = {
#include "shared/hornet_art/hornet_3_fuselage_polygons.inc"
};

static Sint32 hornet_3_fuselage_vertices[237] = {
#include "shared/hornet_art/hornet_3_fuselage_vertices.inc"
};

NJS_CNK_MODEL hornet_3_model = {
#include "shared/hornet_art/hornet_3_model.inc"
};

NJS_CNK_OBJECT hornet_3_object = {
#include "shared/hornet_art/hornet_3_object.inc"
};

static Sint16 hornet_6_canopy_polygons[119] = {
#include "shared/hornet_art/hornet_6_canopy_polygons.inc"
};

static Sint32 hornet_6_canopy_vertices[69] = {
#include "shared/hornet_art/hornet_6_canopy_vertices.inc"
};

NJS_CNK_MODEL hornet_6_piece_0 = {
#include "shared/hornet_art/hornet_6_piece_0.inc"
};

static NJS_CNK_OBJECT hornet_6_canopy = {
#include "shared/hornet_art/hornet_6_canopy.inc"
};

static Sint16 hornet_6_nozzle_polygons[113] = {
#include "shared/hornet_art/hornet_6_nozzle_polygons.inc"
};

static Sint32 hornet_6_nozzle_vertices[57] = {
#include "shared/hornet_art/hornet_6_nozzle_vertices.inc"
};

NJS_CNK_MODEL hornet_6_piece_1 = {
#include "shared/hornet_art/hornet_6_piece_1.inc"
};

static NJS_CNK_OBJECT hornet_6_nozzle = {
#include "shared/hornet_art/hornet_6_nozzle.inc"
};

static Sint16 hornet_6_rack_polygons[419] = {
#include "shared/hornet_art/hornet_6_rack_polygons.inc"
};

static Sint32 hornet_6_rack_vertices[315] = {
#include "shared/hornet_art/hornet_6_rack_vertices.inc"
};

NJS_CNK_MODEL hornet_6_piece_2 = {
#include "shared/hornet_art/hornet_6_piece_2.inc"
};

static NJS_CNK_OBJECT hornet_6_rack = {
#include "shared/hornet_art/hornet_6_rack.inc"
};

static Sint16 hornet_6_fuselage_polygons[303] = {
#include "shared/hornet_art/hornet_6_fuselage_polygons.inc"
};

static Sint32 hornet_6_fuselage_vertices[237] = {
#include "shared/hornet_art/hornet_6_fuselage_vertices.inc"
};

NJS_CNK_MODEL hornet_6_model = {
#include "shared/hornet_art/hornet_6_model.inc"
};

NJS_CNK_OBJECT hornet_6_object = {
#include "shared/hornet_art/hornet_6_object.inc"
};

static Sint16 hornet_9_canopy_polygons[119] = {
#include "shared/hornet_art/hornet_9_canopy_polygons.inc"
};

static Sint32 hornet_9_canopy_vertices[69] = {
#include "shared/hornet_art/hornet_9_canopy_vertices.inc"
};

NJS_CNK_MODEL hornet_9_piece_0 = {
#include "shared/hornet_art/hornet_9_piece_0.inc"
};

static NJS_CNK_OBJECT hornet_9_canopy = {
#include "shared/hornet_art/hornet_9_canopy.inc"
};

static Sint16 hornet_9_rear_rack_polygons[297] = {
#include "shared/hornet_art/hornet_9_rear_rack_polygons.inc"
};

static Sint32 hornet_9_rear_rack_vertices[195] = {
#include "shared/hornet_art/hornet_9_rear_rack_vertices.inc"
};

NJS_CNK_MODEL hornet_9_piece_1 = {
#include "shared/hornet_art/hornet_9_piece_1.inc"
};

static NJS_CNK_OBJECT hornet_9_rear_rack = {
#include "shared/hornet_art/hornet_9_rear_rack.inc"
};

static Sint16 hornet_9_rack_polygons[419] = {
#include "shared/hornet_art/hornet_9_rack_polygons.inc"
};

static Sint32 hornet_9_rack_vertices[315] = {
#include "shared/hornet_art/hornet_9_rack_vertices.inc"
};

NJS_CNK_MODEL hornet_9_piece_2 = {
#include "shared/hornet_art/hornet_9_piece_2.inc"
};

static NJS_CNK_OBJECT hornet_9_rack = {
#include "shared/hornet_art/hornet_9_rack.inc"
};

static Sint16 hornet_9_fuselage_polygons[303] = {
#include "shared/hornet_art/hornet_9_fuselage_polygons.inc"
};

static Sint32 hornet_9_fuselage_vertices[237] = {
#include "shared/hornet_art/hornet_9_fuselage_vertices.inc"
};

NJS_CNK_MODEL hornet_9_model = {
#include "shared/hornet_art/hornet_9_model.inc"
};

NJS_CNK_OBJECT hornet_9_object = {
#include "shared/hornet_art/hornet_9_object.inc"
};

static Sint16 laser_hornet_laser_array_polygons[387] = {
#include "shared/hornet_art/laser_hornet_laser_array_polygons.inc"
};

static Sint32 laser_hornet_laser_array_vertices[255] = {
#include "shared/hornet_art/laser_hornet_laser_array_vertices.inc"
};

NJS_CNK_MODEL laser_hornet_piece_0 = {
#include "shared/hornet_art/laser_hornet_piece_0.inc"
};

static NJS_CNK_OBJECT laser_hornet_laser_array = {
#include "shared/hornet_art/laser_hornet_laser_array.inc"
};

static Sint16 laser_hornet_rack_polygons[701] = {
#include "shared/hornet_art/laser_hornet_rack_polygons.inc"
};

static Sint32 laser_hornet_rack_vertices[405] = {
#include "shared/hornet_art/laser_hornet_rack_vertices.inc"
};

NJS_CNK_MODEL laser_hornet_piece_1 = {
#include "shared/hornet_art/laser_hornet_piece_1.inc"
};

static NJS_CNK_OBJECT laser_hornet_rack = {
#include "shared/hornet_art/laser_hornet_rack.inc"
};

static Sint16 laser_hornet_canopy_polygons[119] = {
#include "shared/hornet_art/laser_hornet_canopy_polygons.inc"
};

static Sint32 laser_hornet_canopy_vertices[69] = {
#include "shared/hornet_art/laser_hornet_canopy_vertices.inc"
};

NJS_CNK_MODEL laser_hornet_piece_2 = {
#include "shared/hornet_art/laser_hornet_piece_2.inc"
};

static NJS_CNK_OBJECT laser_hornet_canopy = {
#include "shared/hornet_art/laser_hornet_canopy.inc"
};

static Sint16 laser_hornet_nozzle_polygons[113] = {
#include "shared/hornet_art/laser_hornet_nozzle_polygons.inc"
};

static Sint32 laser_hornet_nozzle_vertices[57] = {
#include "shared/hornet_art/laser_hornet_nozzle_vertices.inc"
};

NJS_CNK_MODEL laser_hornet_piece_3 = {
#include "shared/hornet_art/laser_hornet_piece_3.inc"
};

static NJS_CNK_OBJECT laser_hornet_nozzle = {
#include "shared/hornet_art/laser_hornet_nozzle.inc"
};

static Sint16 laser_hornet_fuselage_polygons[303] = {
#include "shared/hornet_art/laser_hornet_fuselage_polygons.inc"
};

static Sint32 laser_hornet_fuselage_vertices[237] = {
#include "shared/hornet_art/laser_hornet_fuselage_vertices.inc"
};

NJS_CNK_MODEL laser_hornet_model = {
#include "shared/hornet_art/laser_hornet_model.inc"
};

NJS_CNK_OBJECT laser_hornet_object = {
#include "shared/hornet_art/laser_hornet_object.inc"
};
