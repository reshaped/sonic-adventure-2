/* The rubble mesh and the six moving platforms on the Egg Golem. */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_golem/attachment_art.h"
#include "shared/golem_material/names.h"
#include "shared/golem_statue/materials.h"

/* The falling rubble has its own texture list and model. */
static NJS_TEXNAME rubble_textures[1] = {
#include "boss_golem/platform_art/rubble_textures.inc"
};

NJS_TEXLIST golem_rubble_texlist = {
#include "boss_golem/platform_art/golem_rubble_texlist.inc"
};

#include "shared/golem_material/rubble_mesh.h"

NJS_CNK_MODEL golem_rubble_model = {
#include "boss_golem/platform_art/golem_rubble_model.inc"
};

/* The original bank also keeps the standalone rubble object. */
static NJS_CNK_OBJECT golem_rubble_object = {
#include "boss_golem/platform_art/golem_rubble_object.inc"
};

/* Each head platform has a separate joint for the indicator light. */
static NJS_TEXNAME head_platform_a_textures[4] = {
#include "boss_golem/platform_art/head_platform_a_textures.inc"
};

NJS_TEXLIST golem_head_platform_a_texlist = {
#include "boss_golem/platform_art/golem_head_platform_a_texlist.inc"
};

static Sint16 head_platform_a_joint_1_polygons[25] = {
#include "boss_golem/platform_art/head_platform_a_joint_1_polygons.inc"
};

static Sint32 head_platform_a_joint_1_vertices[27] = {
#include "boss_golem/platform_art/head_platform_a_joint_1_vertices.inc"
};

static NJS_CNK_MODEL head_platform_a_joint_1_model = {
#include "boss_golem/platform_art/head_platform_a_joint_1_model.inc"
};

static NJS_CNK_OBJECT head_platform_a_joint_1 = {
#include "boss_golem/platform_art/head_platform_a_joint_1.inc"
};

static Sint16 head_platform_a_polygons[179] = {
#include "boss_golem/platform_art/head_platform_a_polygons.inc"
};

static Sint32 head_platform_a_vertices[123] = {
#include "boss_golem/platform_art/head_platform_a_vertices.inc"
};

static NJS_CNK_MODEL head_platform_a_model = {
#include "boss_golem/platform_art/head_platform_a_model.inc"
};

NJS_CNK_OBJECT golem_head_platform_a_object = {
#include "boss_golem/platform_art/golem_head_platform_a_object.inc"
};

static NJS_TEXNAME head_platform_b_textures[4] = {
#include "boss_golem/platform_art/head_platform_b_textures.inc"
};

NJS_TEXLIST golem_head_platform_b_texlist = {
#include "boss_golem/platform_art/golem_head_platform_b_texlist.inc"
};

static Sint16 head_platform_b_joint_1_polygons[25] = {
#include "boss_golem/platform_art/head_platform_b_joint_1_polygons.inc"
};

static Sint32 head_platform_b_joint_1_vertices[27] = {
#include "boss_golem/platform_art/head_platform_b_joint_1_vertices.inc"
};

static NJS_CNK_MODEL head_platform_b_joint_1_model = {
#include "boss_golem/platform_art/head_platform_b_joint_1_model.inc"
};

static NJS_CNK_OBJECT head_platform_b_joint_1 = {
#include "boss_golem/platform_art/head_platform_b_joint_1.inc"
};

static Sint16 head_platform_b_polygons[179] = {
#include "boss_golem/platform_art/head_platform_b_polygons.inc"
};

static Sint32 head_platform_b_vertices[123] = {
#include "boss_golem/platform_art/head_platform_b_vertices.inc"
};

static NJS_CNK_MODEL head_platform_b_model = {
#include "boss_golem/platform_art/head_platform_b_model.inc"
};

NJS_CNK_OBJECT golem_head_platform_b_object = {
#include "boss_golem/platform_art/golem_head_platform_b_object.inc"
};

static NJS_TEXNAME head_platform_c_textures[4] = {
#include "boss_golem/platform_art/head_platform_c_textures.inc"
};

NJS_TEXLIST golem_head_platform_c_texlist = {
#include "boss_golem/platform_art/golem_head_platform_c_texlist.inc"
};

static Sint16 head_platform_c_joint_1_polygons[25] = {
#include "boss_golem/platform_art/head_platform_c_joint_1_polygons.inc"
};

static Sint32 head_platform_c_joint_1_vertices[27] = {
#include "boss_golem/platform_art/head_platform_c_joint_1_vertices.inc"
};

static NJS_CNK_MODEL head_platform_c_joint_1_model = {
#include "boss_golem/platform_art/head_platform_c_joint_1_model.inc"
};

static NJS_CNK_OBJECT head_platform_c_joint_1 = {
#include "boss_golem/platform_art/head_platform_c_joint_1.inc"
};

static Sint16 head_platform_c_polygons[171] = {
#include "boss_golem/platform_art/head_platform_c_polygons.inc"
};

static Sint32 head_platform_c_vertices[123] = {
#include "boss_golem/platform_art/head_platform_c_vertices.inc"
};

static NJS_CNK_MODEL head_platform_c_model = {
#include "boss_golem/platform_art/head_platform_c_model.inc"
};

NJS_CNK_OBJECT golem_head_platform_c_object = {
#include "boss_golem/platform_art/golem_head_platform_c_object.inc"
};

/* The three lower platforms each have a fixed model. */
static NJS_TEXNAME platform_a_textures[3] = {
#include "boss_golem/platform_art/platform_a_textures.inc"
};

NJS_TEXLIST golem_platform_a_texlist = {
#include "boss_golem/platform_art/golem_platform_a_texlist.inc"
};

static Sint16 platform_a_polygons[183] = {
#include "boss_golem/platform_art/platform_a_polygons.inc"
};

static Sint32 platform_a_vertices[111] = {
#include "boss_golem/platform_art/platform_a_vertices.inc"
};

static NJS_CNK_MODEL platform_a_model = {
#include "boss_golem/platform_art/platform_a_model.inc"
};

NJS_CNK_OBJECT golem_platform_a_object = {
#include "boss_golem/platform_art/golem_platform_a_object.inc"
};

static NJS_TEXNAME platform_b_textures[3] = {
#include "boss_golem/platform_art/platform_b_textures.inc"
};

NJS_TEXLIST golem_platform_b_texlist = {
#include "boss_golem/platform_art/golem_platform_b_texlist.inc"
};

static Sint16 platform_b_polygons[97] = {
#include "boss_golem/platform_art/platform_b_polygons.inc"
};

static Sint32 platform_b_vertices[51] = {
#include "boss_golem/platform_art/platform_b_vertices.inc"
};

static NJS_CNK_MODEL platform_b_model = {
#include "boss_golem/platform_art/platform_b_model.inc"
};

NJS_CNK_OBJECT golem_platform_b_object = {
#include "boss_golem/platform_art/golem_platform_b_object.inc"
};

static NJS_TEXNAME platform_c_textures[3] = {
#include "boss_golem/platform_art/platform_c_textures.inc"
};

NJS_TEXLIST golem_platform_c_texlist = {
#include "boss_golem/platform_art/golem_platform_c_texlist.inc"
};

static Sint16 platform_c_polygons[121] = {
#include "boss_golem/platform_art/platform_c_polygons.inc"
};

static Sint32 platform_c_vertices[75] = {
#include "boss_golem/platform_art/platform_c_vertices.inc"
};

static NJS_CNK_MODEL platform_c_model = {
#include "boss_golem/platform_art/platform_c_model.inc"
};

NJS_CNK_OBJECT golem_platform_c_object = {
#include "boss_golem/platform_art/golem_platform_c_object.inc"
};
