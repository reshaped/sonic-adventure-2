/* a model of eight pieces under one root, which no code reads */
#include <ninja.h>
#include <sg_xpt.h>

enum effect_texture { TEX_SHOT_EFF03 = 5 };

static Sint16 piece_8_polygons[101] = {
#include "shared/gun_mech/fragments/piece_8_polygons.inc"
};

static Sint32 piece_8_vertices[75] = {
#include "shared/gun_mech/fragments/piece_8_vertices.inc"
};

static NJS_CNK_MODEL piece_8_mesh = {
#include "shared/gun_mech/fragments/piece_8_mesh.inc"
};

static NJS_CNK_OBJECT piece_8 = {
#include "shared/gun_mech/fragments/piece_8.inc"
};

static Sint16 piece_7_polygons[101] = {
#include "shared/gun_mech/fragments/piece_7_polygons.inc"
};

static Sint32 piece_7_vertices[75] = {
#include "shared/gun_mech/fragments/piece_7_vertices.inc"
};

static NJS_CNK_MODEL piece_7_mesh = {
#include "shared/gun_mech/fragments/piece_7_mesh.inc"
};

static NJS_CNK_OBJECT piece_7 = {
#include "shared/gun_mech/fragments/piece_7.inc"
};

static Sint16 piece_6_polygons[101] = {
#include "shared/gun_mech/fragments/piece_6_polygons.inc"
};

static Sint32 piece_6_vertices[75] = {
#include "shared/gun_mech/fragments/piece_6_vertices.inc"
};

static NJS_CNK_MODEL piece_6_mesh = {
#include "shared/gun_mech/fragments/piece_6_mesh.inc"
};

static NJS_CNK_OBJECT piece_6 = {
#include "shared/gun_mech/fragments/piece_6.inc"
};

static Sint16 piece_5_polygons[119] = {
#include "shared/gun_mech/fragments/piece_5_polygons.inc"
};

static Sint32 piece_5_vertices[87] = {
#include "shared/gun_mech/fragments/piece_5_vertices.inc"
};

static NJS_CNK_MODEL piece_5_mesh = {
#include "shared/gun_mech/fragments/piece_5_mesh.inc"
};

static NJS_CNK_OBJECT piece_5 = {
#include "shared/gun_mech/fragments/piece_5.inc"
};

static Sint16 piece_4_polygons[101] = {
#include "shared/gun_mech/fragments/piece_4_polygons.inc"
};

static Sint32 piece_4_vertices[75] = {
#include "shared/gun_mech/fragments/piece_4_vertices.inc"
};

static NJS_CNK_MODEL piece_4_mesh = {
#include "shared/gun_mech/fragments/piece_4_mesh.inc"
};

static NJS_CNK_OBJECT piece_4 = {
#include "shared/gun_mech/fragments/piece_4.inc"
};

static Sint16 piece_3_polygons[101] = {
#include "shared/gun_mech/fragments/piece_3_polygons.inc"
};

static Sint32 piece_3_vertices[75] = {
#include "shared/gun_mech/fragments/piece_3_vertices.inc"
};

static NJS_CNK_MODEL piece_3_mesh = {
#include "shared/gun_mech/fragments/piece_3_mesh.inc"
};

static NJS_CNK_OBJECT piece_3 = {
#include "shared/gun_mech/fragments/piece_3.inc"
};

static Sint16 piece_2_polygons[101] = {
#include "shared/gun_mech/fragments/piece_2_polygons.inc"
};

static Sint32 piece_2_vertices[75] = {
#include "shared/gun_mech/fragments/piece_2_vertices.inc"
};

static NJS_CNK_MODEL piece_2_mesh = {
#include "shared/gun_mech/fragments/piece_2_mesh.inc"
};

static NJS_CNK_OBJECT piece_2 = {
#include "shared/gun_mech/fragments/piece_2.inc"
};

static Sint16 piece_1_polygons[101] = {
#include "shared/gun_mech/fragments/piece_1_polygons.inc"
};

static Sint32 piece_1_vertices[75] = {
#include "shared/gun_mech/fragments/piece_1_vertices.inc"
};

static NJS_CNK_MODEL piece_1_mesh = {
#include "shared/gun_mech/fragments/piece_1_mesh.inc"
};

static NJS_CNK_OBJECT piece_1 = {
#include "shared/gun_mech/fragments/piece_1.inc"
};

static NJS_CNK_OBJECT fragments = {
#include "shared/gun_mech/fragments/fragments.inc"
};
