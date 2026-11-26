/* the models of the Egg Golem's weak points, its rubble and its pieces. the
 * weak points are three shells, and the mechanism with its two lights */

#include <ninja.h>
#include <sg_xpt.h>

#include "boss_golem_e/attachment_art.h"
#include "shared/golem_material/names.h"
#include "shared/golem_statue/materials.h"

const char golem_flash_texture[12] = {
#include "boss_golem_e/weak_point_art/golem_flash_texture.inc"
};

/* Texture filenames in the golem object archive. */
const struct golem_mechanism_names golem_mechanism_materials = {
#include "boss_golem_e/weak_point_art/golem_mechanism_materials.inc"
};

static NJS_TEXNAME rubble_textures[1] = {
#include "boss_golem_e/weak_point_art/rubble_textures.inc"
};

NJS_TEXLIST golem_e_rubble_texlist = {
#include "boss_golem_e/weak_point_art/golem_e_rubble_texlist.inc"
};

#include "shared/golem_material/rubble_mesh.h"

NJS_CNK_MODEL golem_e_rubble_model[1] = {
#include "boss_golem_e/weak_point_art/golem_e_rubble_model.inc"
};

NJS_CNK_OBJECT golem_e_rubble_object = {
#include "boss_golem_e/weak_point_art/golem_e_rubble_object.inc"
};

static NJS_TEXNAME mechanism_textures[4] = {
#include "boss_golem_e/weak_point_art/mechanism_textures.inc"
};

NJS_TEXLIST golem_e_mechanism_texlist = {
#include "boss_golem_e/weak_point_art/golem_e_mechanism_texlist.inc"
};

static Sint16 mechanism_joint_2_polygons[25] = {
#include "boss_golem_e/weak_point_art/mechanism_joint_2_polygons.inc"
};

static Sint32 mechanism_joint_2_vertices[27] = {
#include "boss_golem_e/weak_point_art/mechanism_joint_2_vertices.inc"
};

NJS_CNK_MODEL golem_e_light_a_model = {
#include "boss_golem_e/weak_point_art/golem_e_light_a_model.inc"
};

static NJS_CNK_OBJECT mechanism_joint_2 = {
#include "boss_golem_e/weak_point_art/mechanism_joint_2.inc"
};

static Sint16 mechanism_joint_1_polygons[25] = {
#include "boss_golem_e/weak_point_art/mechanism_joint_1_polygons.inc"
};

static Sint32 mechanism_joint_1_vertices[27] = {
#include "boss_golem_e/weak_point_art/mechanism_joint_1_vertices.inc"
};

NJS_CNK_MODEL golem_e_light_b_model = {
#include "boss_golem_e/weak_point_art/golem_e_light_b_model.inc"
};

static NJS_CNK_OBJECT mechanism_joint_1 = {
#include "boss_golem_e/weak_point_art/mechanism_joint_1.inc"
};

static Sint16 mechanism_polygons[433] = {
#include "boss_golem_e/weak_point_art/mechanism_polygons.inc"
};

static Sint32 mechanism_vertices[399] = {
#include "boss_golem_e/weak_point_art/mechanism_vertices.inc"
};

NJS_CNK_MODEL golem_e_mechanism_model = {
#include "boss_golem_e/weak_point_art/golem_e_mechanism_model.inc"
};

NJS_CNK_OBJECT golem_e_mechanism_object = {
#include "boss_golem_e/weak_point_art/golem_e_mechanism_object.inc"
};

static NJS_TEXNAME piece_textures[1] = {
#include "boss_golem_e/weak_point_art/piece_textures.inc"
};

NJS_TEXLIST golem_e_piece_texlist = {
#include "boss_golem_e/weak_point_art/golem_e_piece_texlist.inc"
};

static Sint16 piece_polygons[83] = {
#include "boss_golem_e/weak_point_art/piece_polygons.inc"
};

static Sint32 piece_vertices[51] = {
#include "boss_golem_e/weak_point_art/piece_vertices.inc"
};

NJS_CNK_MODEL golem_e_piece_model[1] = {
#include "boss_golem_e/weak_point_art/golem_e_piece_model.inc"
};

NJS_CNK_OBJECT golem_e_piece_object = {
#include "boss_golem_e/weak_point_art/golem_e_piece_object.inc"
};

static NJS_TEXNAME weak_point_a_textures[2] = {
#include "boss_golem_e/weak_point_art/weak_point_a_textures.inc"
};

NJS_TEXLIST golem_e_weak_point_a_texlist = {
#include "boss_golem_e/weak_point_art/golem_e_weak_point_a_texlist.inc"
};

static Sint16 weak_point_a_polygons[55] = {
#include "boss_golem_e/weak_point_art/weak_point_a_polygons.inc"
};

static Sint32 weak_point_a_vertices[75] = {
#include "boss_golem_e/weak_point_art/weak_point_a_vertices.inc"
};

NJS_CNK_MODEL golem_e_weak_point_a_model = {
#include "boss_golem_e/weak_point_art/golem_e_weak_point_a_model.inc"
};

NJS_CNK_OBJECT golem_e_weak_point_a_object = {
#include "boss_golem_e/weak_point_art/golem_e_weak_point_a_object.inc"
};

static NJS_TEXNAME weak_point_b_textures[2] = {
#include "boss_golem_e/weak_point_art/weak_point_b_textures.inc"
};

NJS_TEXLIST golem_e_weak_point_b_texlist = {
#include "boss_golem_e/weak_point_art/golem_e_weak_point_b_texlist.inc"
};

static Sint16 weak_point_b_polygons[67] = {
#include "boss_golem_e/weak_point_art/weak_point_b_polygons.inc"
};

static Sint32 weak_point_b_vertices[99] = {
#include "boss_golem_e/weak_point_art/weak_point_b_vertices.inc"
};

NJS_CNK_MODEL golem_e_weak_point_b_model = {
#include "boss_golem_e/weak_point_art/golem_e_weak_point_b_model.inc"
};

NJS_CNK_OBJECT golem_e_weak_point_b_object = {
#include "boss_golem_e/weak_point_art/golem_e_weak_point_b_object.inc"
};

static NJS_TEXNAME weak_point_c_textures[2] = {
#include "boss_golem_e/weak_point_art/weak_point_c_textures.inc"
};

NJS_TEXLIST golem_e_weak_point_c_texlist = {
#include "boss_golem_e/weak_point_art/golem_e_weak_point_c_texlist.inc"
};

static Sint16 weak_point_c_polygons[71] = {
#include "boss_golem_e/weak_point_art/weak_point_c_polygons.inc"
};

static Sint32 weak_point_c_vertices[75] = {
#include "boss_golem_e/weak_point_art/weak_point_c_vertices.inc"
};

NJS_CNK_MODEL golem_e_weak_point_c_model = {
#include "boss_golem_e/weak_point_art/golem_e_weak_point_c_model.inc"
};

NJS_CNK_OBJECT golem_e_weak_point_c_object = {
#include "boss_golem_e/weak_point_art/golem_e_weak_point_c_object.inc"
};
