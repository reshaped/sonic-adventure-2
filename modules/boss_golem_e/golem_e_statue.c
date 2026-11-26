/* BOSS_GOLEM_E.PRS's texture lists and chunk models, from the disc. its D
 * lies between golem_title.c and weak_point_art.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/golem_material/names.h"
#include "shared/golem_statue.h"
#include "shared/golem_statue/materials.h"

static NJS_TEXNAME statue_textures[8] = {
#include "boss_golem_e/golem_e_statue/statue_textures.inc"
};
NJS_TEXLIST golem_statue_texlist = {
#include "boss_golem_e/golem_e_statue/golem_statue_texlist.inc"
};
static Sint16 golem_statue_head_polygons[1465] = {
#include "boss_golem_e/golem_e_statue/golem_statue_head_polygons.inc"
};
static Sint32 golem_statue_head_vertices[975] = {
#include "boss_golem_e/golem_e_statue/golem_statue_head_vertices.inc"
};
static NJS_CNK_MODEL golem_statue_head_model = {
#include "boss_golem_e/golem_e_statue/golem_statue_head_model.inc"
};
NJS_CNK_OBJECT golem_statue_head = {
#include "boss_golem_e/golem_e_statue/golem_statue_head.inc"
};
static Sint16 statue_joint_19_polygons[383] = {
#include "boss_golem_e/golem_e_statue/statue_joint_19_polygons.inc"
};
static Sint32 statue_joint_19_vertices[291] = {
#include "boss_golem_e/golem_e_statue/statue_joint_19_vertices.inc"
};
static NJS_CNK_MODEL statue_joint_19_model = {
#include "boss_golem_e/golem_e_statue/statue_joint_19_model.inc"
};
static NJS_CNK_OBJECT statue_joint_19 = {
#include "boss_golem_e/golem_e_statue/statue_joint_19.inc"
};
static Sint16 statue_joint_18_polygons[383] = {
#include "boss_golem_e/golem_e_statue/statue_joint_18_polygons.inc"
};
static Sint32 statue_joint_18_vertices[291] = {
#include "boss_golem_e/golem_e_statue/statue_joint_18_vertices.inc"
};
static NJS_CNK_MODEL statue_joint_18_model = {
#include "boss_golem_e/golem_e_statue/statue_joint_18_model.inc"
};
static NJS_CNK_OBJECT statue_joint_18 = {
#include "boss_golem_e/golem_e_statue/statue_joint_18.inc"
};
static Sint16 statue_joint_17_polygons[107] = {
#include "boss_golem_e/golem_e_statue/statue_joint_17_polygons.inc"
};
static Sint32 statue_joint_17_vertices[75] = {
#include "boss_golem_e/golem_e_statue/statue_joint_17_vertices.inc"
};
static NJS_CNK_MODEL statue_joint_17_model = {
#include "boss_golem_e/golem_e_statue/statue_joint_17_model.inc"
};
static NJS_CNK_OBJECT statue_joint_17 = {
#include "boss_golem_e/golem_e_statue/statue_joint_17.inc"
};
static Sint16 statue_joint_16_polygons[101] = {
#include "boss_golem_e/golem_e_statue/statue_joint_16_polygons.inc"
};
static Sint32 statue_joint_16_vertices[75] = {
#include "boss_golem_e/golem_e_statue/statue_joint_16_vertices.inc"
};
static NJS_CNK_MODEL statue_joint_16_model = {
#include "boss_golem_e/golem_e_statue/statue_joint_16_model.inc"
};
static NJS_CNK_OBJECT statue_joint_16 = {
#include "boss_golem_e/golem_e_statue/statue_joint_16.inc"
};
static Sint16 golem_statue_hand_b_polygons[245] = {
#include "boss_golem_e/golem_e_statue/golem_statue_hand_b_polygons.inc"
};
static Sint32 golem_statue_hand_b_vertices[159] = {
#include "boss_golem_e/golem_e_statue/golem_statue_hand_b_vertices.inc"
};
static NJS_CNK_MODEL golem_statue_hand_b_model = {
#include "boss_golem_e/golem_e_statue/golem_statue_hand_b_model.inc"
};
NJS_CNK_OBJECT golem_statue_hand_b = {
#include "boss_golem_e/golem_e_statue/golem_statue_hand_b.inc"
};
static Sint16 statue_joint_14_polygons[373] = {
#include "boss_golem_e/golem_e_statue/statue_joint_14_polygons.inc"
};
static Sint32 statue_joint_14_vertices[255] = {
#include "boss_golem_e/golem_e_statue/statue_joint_14_vertices.inc"
};
static NJS_CNK_MODEL statue_joint_14_model = {
#include "boss_golem_e/golem_e_statue/statue_joint_14_model.inc"
};
NJS_CNK_OBJECT golem_statue_joint_14 = {
#include "boss_golem_e/golem_e_statue/golem_statue_joint_14.inc"
};
static Sint16 statue_joint_13_polygons[345] = {
#include "boss_golem_e/golem_e_statue/statue_joint_13_polygons.inc"
};
static Sint32 statue_joint_13_vertices[255] = {
#include "boss_golem_e/golem_e_statue/statue_joint_13_vertices.inc"
};
static NJS_CNK_MODEL statue_joint_13_model = {
#include "boss_golem_e/golem_e_statue/statue_joint_13_model.inc"
};
NJS_CNK_OBJECT golem_statue_joint_13 = {
#include "boss_golem_e/golem_e_statue/golem_statue_joint_13.inc"
};
static Sint16 statue_joint_12_polygons[389] = {
#include "boss_golem_e/golem_e_statue/statue_joint_12_polygons.inc"
};
static Sint32 statue_joint_12_vertices[291] = {
#include "boss_golem_e/golem_e_statue/statue_joint_12_vertices.inc"
};
static NJS_CNK_MODEL statue_joint_12_model = {
#include "boss_golem_e/golem_e_statue/statue_joint_12_model.inc"
};
static NJS_CNK_OBJECT statue_joint_12 = {
#include "boss_golem_e/golem_e_statue/statue_joint_12.inc"
};
static Sint16 statue_joint_11_polygons[383] = {
#include "boss_golem_e/golem_e_statue/statue_joint_11_polygons.inc"
};
static Sint32 statue_joint_11_vertices[291] = {
#include "boss_golem_e/golem_e_statue/statue_joint_11_vertices.inc"
};
static NJS_CNK_MODEL statue_joint_11_model = {
#include "boss_golem_e/golem_e_statue/statue_joint_11_model.inc"
};
static NJS_CNK_OBJECT statue_joint_11 = {
#include "boss_golem_e/golem_e_statue/statue_joint_11.inc"
};
static Sint16 statue_joint_10_polygons[101] = {
#include "boss_golem_e/golem_e_statue/statue_joint_10_polygons.inc"
};
static Sint32 statue_joint_10_vertices[75] = {
#include "boss_golem_e/golem_e_statue/statue_joint_10_vertices.inc"
};
static NJS_CNK_MODEL statue_joint_10_model = {
#include "boss_golem_e/golem_e_statue/statue_joint_10_model.inc"
};
static NJS_CNK_OBJECT statue_joint_10 = {
#include "boss_golem_e/golem_e_statue/statue_joint_10.inc"
};
static Sint16 statue_joint_9_polygons[101] = {
#include "boss_golem_e/golem_e_statue/statue_joint_9_polygons.inc"
};
static Sint32 statue_joint_9_vertices[75] = {
#include "boss_golem_e/golem_e_statue/statue_joint_9_vertices.inc"
};
static NJS_CNK_MODEL statue_joint_9_model = {
#include "boss_golem_e/golem_e_statue/statue_joint_9_model.inc"
};
static NJS_CNK_OBJECT statue_joint_9 = {
#include "boss_golem_e/golem_e_statue/statue_joint_9.inc"
};
static Sint16 golem_statue_hand_a_polygons[245] = {
#include "boss_golem_e/golem_e_statue/golem_statue_hand_a_polygons.inc"
};
static Sint32 golem_statue_hand_a_vertices[159] = {
#include "boss_golem_e/golem_e_statue/golem_statue_hand_a_vertices.inc"
};
static NJS_CNK_MODEL golem_statue_hand_a_model = {
#include "boss_golem_e/golem_e_statue/golem_statue_hand_a_model.inc"
};
NJS_CNK_OBJECT golem_statue_hand_a = {
#include "boss_golem_e/golem_e_statue/golem_statue_hand_a.inc"
};
static Sint16 statue_joint_7_polygons[375] = {
#include "boss_golem_e/golem_e_statue/statue_joint_7_polygons.inc"
};
static Sint32 statue_joint_7_vertices[255] = {
#include "boss_golem_e/golem_e_statue/statue_joint_7_vertices.inc"
};
static NJS_CNK_MODEL statue_joint_7_model = {
#include "boss_golem_e/golem_e_statue/statue_joint_7_model.inc"
};
NJS_CNK_OBJECT golem_statue_joint_7 = {
#include "boss_golem_e/golem_e_statue/golem_statue_joint_7.inc"
};
static Sint16 statue_joint_6_polygons[407] = {
#include "boss_golem_e/golem_e_statue/statue_joint_6_polygons.inc"
};
static Sint32 statue_joint_6_vertices[255] = {
#include "boss_golem_e/golem_e_statue/statue_joint_6_vertices.inc"
};
static NJS_CNK_MODEL statue_joint_6_model = {
#include "boss_golem_e/golem_e_statue/statue_joint_6_model.inc"
};
NJS_CNK_OBJECT golem_statue_joint_6 = {
#include "boss_golem_e/golem_e_statue/golem_statue_joint_6.inc"
};
static Sint16 statue_joint_5_polygons[1297] = {
#include "boss_golem_e/golem_e_statue/statue_joint_5_polygons.inc"
};
static Sint32 statue_joint_5_vertices[909] = {
#include "boss_golem_e/golem_e_statue/statue_joint_5_vertices.inc"
};
static NJS_CNK_MODEL statue_joint_5_model = {
#include "boss_golem_e/golem_e_statue/statue_joint_5_model.inc"
};
NJS_CNK_OBJECT golem_statue_joint_5 = {
#include "boss_golem_e/golem_e_statue/golem_statue_joint_5.inc"
};
static NJS_CNK_OBJECT statue_joint_4 = {
#include "boss_golem_e/golem_e_statue/statue_joint_4.inc"
};
static Sint16 statue_joint_3_polygons[141] = {
#include "boss_golem_e/golem_e_statue/statue_joint_3_polygons.inc"
};
static Sint32 statue_joint_3_vertices[147] = {
#include "boss_golem_e/golem_e_statue/statue_joint_3_vertices.inc"
};
static NJS_CNK_MODEL statue_joint_3_model = {
#include "boss_golem_e/golem_e_statue/statue_joint_3_model.inc"
};
static NJS_CNK_OBJECT statue_joint_3 = {
#include "boss_golem_e/golem_e_statue/statue_joint_3.inc"
};
static Sint16 statue_joint_2_polygons[205] = {
#include "boss_golem_e/golem_e_statue/statue_joint_2_polygons.inc"
};
static Sint32 statue_joint_2_vertices[147] = {
#include "boss_golem_e/golem_e_statue/statue_joint_2_vertices.inc"
};
static NJS_CNK_MODEL statue_joint_2_model = {
#include "boss_golem_e/golem_e_statue/statue_joint_2_model.inc"
};
static NJS_CNK_OBJECT statue_joint_2 = {
#include "boss_golem_e/golem_e_statue/statue_joint_2.inc"
};
static Sint16 statue_joint_1_polygons[609] = {
#include "boss_golem_e/golem_e_statue/statue_joint_1_polygons.inc"
};
static Sint32 statue_joint_1_vertices[423] = {
#include "boss_golem_e/golem_e_statue/statue_joint_1_vertices.inc"
};
static NJS_CNK_MODEL statue_joint_1_model = {
#include "boss_golem_e/golem_e_statue/statue_joint_1_model.inc"
};
NJS_CNK_OBJECT golem_statue_joint_1 = {
#include "boss_golem_e/golem_e_statue/golem_statue_joint_1.inc"
};
NJS_CNK_OBJECT golem_statue = {
#include "boss_golem_e/golem_e_statue/golem_statue.inc"
};
