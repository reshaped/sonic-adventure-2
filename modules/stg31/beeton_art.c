/* Beeton's texture list, its body and the pieces it breaks into, and its
 * motions */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg31/flier_art.h"
#include "stg31/beeton_art/text.inc"

enum beeton_material {
    BEETON_MATERIAL_GORA2,
    BEETON_MATERIAL_G_EYE,
    BEETON_MATERIAL_UNI1,
    BEETON_MATERIAL_SARU_BODY,
    BEETON_MATERIAL_BEE,
    MATERIAL_COUNT
};

static NJS_TEXNAME materials[MATERIAL_COUNT] = {
#include "stg31/beeton_art/materials.inc"
};

NJS_TEXLIST green_hill_beeton_texlist = {
#include "stg31/beeton_art/green_hill_beeton_texlist.inc"
};

static Sint16 piece_0_polygons[313] = {
#include "stg31/beeton_art/piece_0_polygons.inc"
};

static Sint32 piece_0_vertices[303] = {
#include "stg31/beeton_art/piece_0_vertices.inc"
};

NJS_CNK_MODEL green_hill_beeton_piece_0 = {
#include "stg31/beeton_art/green_hill_beeton_piece_0.inc"
};

static NJS_CNK_OBJECT piece_0_node = {
#include "stg31/beeton_art/piece_0_node.inc"
};

static Sint16 piece_1_polygons[47] = {
#include "stg31/beeton_art/piece_1_polygons.inc"
};

static Sint32 piece_1_vertices[27] = {
#include "stg31/beeton_art/piece_1_vertices.inc"
};

NJS_CNK_MODEL green_hill_beeton_piece_1 = {
#include "stg31/beeton_art/green_hill_beeton_piece_1.inc"
};

static NJS_CNK_OBJECT piece_1_node = {
#include "stg31/beeton_art/piece_1_node.inc"
};

static Sint16 piece_2_polygons[47] = {
#include "stg31/beeton_art/piece_2_polygons.inc"
};

static Sint32 piece_2_vertices[27] = {
#include "stg31/beeton_art/piece_2_vertices.inc"
};

NJS_CNK_MODEL green_hill_beeton_piece_2 = {
#include "stg31/beeton_art/green_hill_beeton_piece_2.inc"
};

static NJS_CNK_OBJECT piece_2_node = {
#include "stg31/beeton_art/piece_2_node.inc"
};

static Sint16 piece_3_polygons[199] = {
#include "stg31/beeton_art/piece_3_polygons.inc"
};

static Sint32 piece_3_vertices[135] = {
#include "stg31/beeton_art/piece_3_vertices.inc"
};

NJS_CNK_MODEL green_hill_beeton_piece_3 = {
#include "stg31/beeton_art/green_hill_beeton_piece_3.inc"
};

static NJS_CNK_OBJECT piece_3_node = {
#include "stg31/beeton_art/piece_3_node.inc"
};

static Sint16 body_polygons[289] = {
#include "stg31/beeton_art/body_polygons.inc"
};

static Sint32 body_vertices[231] = {
#include "stg31/beeton_art/body_vertices.inc"
};

NJS_CNK_MODEL green_hill_beeton_body_model = {
#include "stg31/beeton_art/green_hill_beeton_body_model.inc"
};

NJS_CNK_OBJECT green_hill_beeton_body = {
#include "stg31/beeton_art/green_hill_beeton_body.inc"
};

static NJS_MKEY_A flight_piece_3_rotation[3] = {
#include "stg31/beeton_art/flight_piece_3_rotation.inc"
};

static NJS_MKEY_A flight_piece_2_rotation[3] = {
#include "stg31/beeton_art/flight_piece_2_rotation.inc"
};

static NJS_MKEY_A flight_piece_1_rotation[3] = {
#include "stg31/beeton_art/flight_piece_1_rotation.inc"
};

static NJS_MDATA2 flight_joints[5] = {
#include "stg31/beeton_art/flight_joints.inc"
};

NJS_MOTION green_hill_beeton_flight_motion = {
#include "stg31/beeton_art/green_hill_beeton_flight_motion.inc"
};

static NJS_MKEY_A unfold_body_rotation[4] = {
#include "stg31/beeton_art/unfold_body_rotation.inc"
};

static NJS_MKEY_A unfold_piece_3_rotation[4] = {
#include "stg31/beeton_art/unfold_piece_3_rotation.inc"
};

static NJS_MKEY_A unfold_piece_2_rotation[4] = {
#include "stg31/beeton_art/unfold_piece_2_rotation.inc"
};

static NJS_MKEY_A unfold_piece_1_rotation[4] = {
#include "stg31/beeton_art/unfold_piece_1_rotation.inc"
};

static NJS_MKEY_A unfold_piece_0_rotation[4] = {
#include "stg31/beeton_art/unfold_piece_0_rotation.inc"
};

static NJS_MDATA2 unfold_joints[5] = {
#include "stg31/beeton_art/unfold_joints.inc"
};

NJS_MOTION green_hill_beeton_unfold_motion = {
#include "stg31/beeton_art/green_hill_beeton_unfold_motion.inc"
};

static NJS_MKEY_A aim_body_rotation[2] = {
#include "stg31/beeton_art/aim_body_rotation.inc"
};

static NJS_MKEY_A aim_piece_3_rotation[3] = {
#include "stg31/beeton_art/aim_piece_3_rotation.inc"
};

static NJS_MKEY_A aim_piece_2_rotation[3] = {
#include "stg31/beeton_art/aim_piece_2_rotation.inc"
};

static NJS_MKEY_A aim_piece_1_rotation[3] = {
#include "stg31/beeton_art/aim_piece_1_rotation.inc"
};

static NJS_MKEY_A aim_piece_0_rotation[2] = {
#include "stg31/beeton_art/aim_piece_0_rotation.inc"
};

static NJS_MDATA2 aim_joints[5] = {
#include "stg31/beeton_art/aim_joints.inc"
};

NJS_MOTION green_hill_beeton_aim_motion = {
#include "stg31/beeton_art/green_hill_beeton_aim_motion.inc"
};
