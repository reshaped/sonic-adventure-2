/* Batabata's texture list, its body and the pieces it breaks into, and its
 * flight motion */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg31/flier_art.h"
#include "stg31/batabata_art/text.inc"

enum batabata_material {
    BATABATA_MATERIAL_GORA2,
    BATABATA_MATERIAL_BEE,
    BATABATA_MATERIAL_BATA1,
    BATABATA_MATERIAL_TT_EYE,
    MATERIAL_COUNT
};

static NJS_TEXNAME materials[MATERIAL_COUNT] = {
#include "stg31/batabata_art/materials.inc"
};

NJS_TEXLIST green_hill_batabata_texlist = {
#include "stg31/batabata_art/green_hill_batabata_texlist.inc"
};

static Sint16 piece_0_polygons[93] = {
#include "stg31/batabata_art/piece_0_polygons.inc"
};

static Sint32 piece_0_vertices[93] = {
#include "stg31/batabata_art/piece_0_vertices.inc"
};

NJS_CNK_MODEL green_hill_batabata_piece_0 = {
#include "stg31/batabata_art/green_hill_batabata_piece_0.inc"
};

static NJS_CNK_OBJECT piece_0_node = {
#include "stg31/batabata_art/piece_0_node.inc"
};

static Sint16 piece_1_polygons[95] = {
#include "stg31/batabata_art/piece_1_polygons.inc"
};

static Sint32 piece_1_vertices[45] = {
#include "stg31/batabata_art/piece_1_vertices.inc"
};

NJS_CNK_MODEL green_hill_batabata_piece_1 = {
#include "stg31/batabata_art/green_hill_batabata_piece_1.inc"
};

static NJS_CNK_OBJECT piece_1_node = {
#include "stg31/batabata_art/piece_1_node.inc"
};

static Sint16 piece_2_polygons[359] = {
#include "stg31/batabata_art/piece_2_polygons.inc"
};

static Sint32 piece_2_vertices[351] = {
#include "stg31/batabata_art/piece_2_vertices.inc"
};

NJS_CNK_MODEL green_hill_batabata_piece_2 = {
#include "stg31/batabata_art/green_hill_batabata_piece_2.inc"
};

static NJS_CNK_OBJECT piece_2_node = {
#include "stg31/batabata_art/piece_2_node.inc"
};

static Sint16 body_polygons[483] = {
#include "stg31/batabata_art/body_polygons.inc"
};

static Sint32 body_vertices[369] = {
#include "stg31/batabata_art/body_vertices.inc"
};

NJS_CNK_MODEL green_hill_batabata_body_model = {
#include "stg31/batabata_art/green_hill_batabata_body_model.inc"
};

NJS_CNK_OBJECT green_hill_batabata_body = {
#include "stg31/batabata_art/green_hill_batabata_body.inc"
};

static NJS_MKEY_A flight_piece_2_rotation[3] = {
#include "stg31/batabata_art/flight_piece_2_rotation.inc"
};

static NJS_MKEY_A flight_piece_1_rotation[3] = {
#include "stg31/batabata_art/flight_piece_1_rotation.inc"
};

static NJS_MDATA2 flight_joints[4] = {
#include "stg31/batabata_art/flight_joints.inc"
};

NJS_MOTION green_hill_batabata_flight_motion = {
#include "stg31/batabata_art/green_hill_batabata_flight_motion.inc"
};
