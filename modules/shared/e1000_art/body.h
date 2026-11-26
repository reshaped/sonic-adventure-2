/* the E-1000's model, e1000_object, a node for each part. the high gun and leg
 * are the pair on the body's +z side */
#ifndef SHARED_E1000_ART_BODY_H
#define SHARED_E1000_ART_BODY_H

#include <ninja.h>
#include <sg_xpt.h>

enum e1000_texture_slot {
    E1000_TEX_GORA1,
    E1000_TEX_SARU_ALL,
    E1000_TEX_SARU_BODY,
    E1000_TEX_E1000,
    E1000_TEX_E1000BB,
    E1000_TEX_E1000EYE,
    E1000_TEX_E1000GUN,
    E1000_TEXTURE_COUNT
};

static NJS_TEXNAME e1000_textures[7] = {
#include "shared/e1000_art/body/e1000_textures.inc"
};

static NJS_TEXLIST e1000_texlist = {
#include "shared/e1000_art/body/e1000_texlist.inc"
};

static Sint16 e1000_barrel_low_polygons[191] = {
#include "shared/e1000_art/body/e1000_barrel_low_polygons.inc"
};

static Sint32 e1000_barrel_low_vertices[99] = {
#include "shared/e1000_art/body/e1000_barrel_low_vertices.inc"
};

static NJS_CNK_MODEL e1000_barrel_low_model = {
#include "shared/e1000_art/body/e1000_barrel_low_model.inc"
};

static NJS_CNK_OBJECT e1000_barrel_low_object = {
#include "shared/e1000_art/body/e1000_barrel_low_object.inc"
};

static Sint16 e1000_gun_low_polygons[37] = {
#include "shared/e1000_art/body/e1000_gun_low_polygons.inc"
};

static Sint32 e1000_gun_low_vertices[27] = {
#include "shared/e1000_art/body/e1000_gun_low_vertices.inc"
};

static NJS_CNK_MODEL e1000_gun_low_model = {
#include "shared/e1000_art/body/e1000_gun_low_model.inc"
};

static NJS_CNK_OBJECT e1000_gun_low_object = {
#include "shared/e1000_art/body/e1000_gun_low_object.inc"
};

static Sint16 e1000_low_foot_polygons[183] = {
#include "shared/e1000_art/body/e1000_low_foot_polygons.inc"
};

static Sint32 e1000_low_foot_vertices[81] = {
#include "shared/e1000_art/body/e1000_low_foot_vertices.inc"
};

static NJS_CNK_MODEL e1000_low_foot_model = {
#include "shared/e1000_art/body/e1000_low_foot_model.inc"
};

static NJS_CNK_OBJECT e1000_low_foot_node = {
#include "shared/e1000_art/body/e1000_low_foot_node.inc"
};

static Sint16 e1000_low_shin_polygons[103] = {
#include "shared/e1000_art/body/e1000_low_shin_polygons.inc"
};

static Sint32 e1000_low_shin_vertices[63] = {
#include "shared/e1000_art/body/e1000_low_shin_vertices.inc"
};

static NJS_CNK_MODEL e1000_low_shin_model = {
#include "shared/e1000_art/body/e1000_low_shin_model.inc"
};

static NJS_CNK_OBJECT e1000_low_shin_node = {
#include "shared/e1000_art/body/e1000_low_shin_node.inc"
};

static Sint16 e1000_low_thigh_polygons[47] = {
#include "shared/e1000_art/body/e1000_low_thigh_polygons.inc"
};

static Sint32 e1000_low_thigh_vertices[39] = {
#include "shared/e1000_art/body/e1000_low_thigh_vertices.inc"
};

static NJS_CNK_MODEL e1000_low_thigh_model = {
#include "shared/e1000_art/body/e1000_low_thigh_model.inc"
};

static NJS_CNK_OBJECT e1000_low_thigh_node = {
#include "shared/e1000_art/body/e1000_low_thigh_node.inc"
};

static Sint16 e1000_barrel_high_polygons[191] = {
#include "shared/e1000_art/body/e1000_barrel_high_polygons.inc"
};

static Sint32 e1000_barrel_high_vertices[99] = {
#include "shared/e1000_art/body/e1000_barrel_high_vertices.inc"
};

static NJS_CNK_MODEL e1000_barrel_high_model = {
#include "shared/e1000_art/body/e1000_barrel_high_model.inc"
};

static NJS_CNK_OBJECT e1000_barrel_high_object = {
#include "shared/e1000_art/body/e1000_barrel_high_object.inc"
};

static Sint16 e1000_gun_high_polygons[37] = {
#include "shared/e1000_art/body/e1000_gun_high_polygons.inc"
};

static Sint32 e1000_gun_high_vertices[27] = {
#include "shared/e1000_art/body/e1000_gun_high_vertices.inc"
};

static NJS_CNK_MODEL e1000_gun_high_model = {
#include "shared/e1000_art/body/e1000_gun_high_model.inc"
};

static NJS_CNK_OBJECT e1000_gun_high_object = {
#include "shared/e1000_art/body/e1000_gun_high_object.inc"
};

static Sint16 e1000_high_foot_polygons[183] = {
#include "shared/e1000_art/body/e1000_high_foot_polygons.inc"
};

static Sint32 e1000_high_foot_vertices[81] = {
#include "shared/e1000_art/body/e1000_high_foot_vertices.inc"
};

static NJS_CNK_MODEL e1000_high_foot_model = {
#include "shared/e1000_art/body/e1000_high_foot_model.inc"
};

static NJS_CNK_OBJECT e1000_high_foot_node = {
#include "shared/e1000_art/body/e1000_high_foot_node.inc"
};

static Sint16 e1000_high_shin_polygons[103] = {
#include "shared/e1000_art/body/e1000_high_shin_polygons.inc"
};

static Sint32 e1000_high_shin_vertices[63] = {
#include "shared/e1000_art/body/e1000_high_shin_vertices.inc"
};

static NJS_CNK_MODEL e1000_high_shin_model = {
#include "shared/e1000_art/body/e1000_high_shin_model.inc"
};

static NJS_CNK_OBJECT e1000_high_shin_node = {
#include "shared/e1000_art/body/e1000_high_shin_node.inc"
};

static Sint16 e1000_high_thigh_polygons[47] = {
#include "shared/e1000_art/body/e1000_high_thigh_polygons.inc"
};

static Sint32 e1000_high_thigh_vertices[39] = {
#include "shared/e1000_art/body/e1000_high_thigh_vertices.inc"
};

static NJS_CNK_MODEL e1000_high_thigh_model = {
#include "shared/e1000_art/body/e1000_high_thigh_model.inc"
};

static NJS_CNK_OBJECT e1000_high_thigh_node = {
#include "shared/e1000_art/body/e1000_high_thigh_node.inc"
};

static Sint16 e1000_jet_polygons[57] = {
#include "shared/e1000_art/body/e1000_jet_polygons.inc"
};

static Sint32 e1000_jet_vertices[51] = {
#include "shared/e1000_art/body/e1000_jet_vertices.inc"
};

static NJS_CNK_MODEL e1000_jet_model = {
#include "shared/e1000_art/body/e1000_jet_model.inc"
};

/* @hack type: the flight node declared as an array of one, where the node alone
 * would do */
static NJS_CNK_OBJECT e1000_flight_node[1] = {
#include "shared/e1000_art/body/e1000_flight_node.inc"
};

static Sint16 e1000_housing_link_polygons[127] = {
#include "shared/e1000_art/body/e1000_housing_link_polygons.inc"
};

static Sint32 e1000_housing_link_vertices[63] = {
#include "shared/e1000_art/body/e1000_housing_link_vertices.inc"
};

static NJS_CNK_MODEL e1000_housing_link_model = {
#include "shared/e1000_art/body/e1000_housing_link_model.inc"
};

static NJS_CNK_OBJECT e1000_housing_link_node = {
#include "shared/e1000_art/body/e1000_housing_link_node.inc"
};

static Sint16 e1000_housing_polygons[429] = {
#include "shared/e1000_art/body/e1000_housing_polygons.inc"
};

static Sint32 e1000_housing_vertices[363] = {
#include "shared/e1000_art/body/e1000_housing_vertices.inc"
};

static NJS_CNK_MODEL e1000_housing_model = {
#include "shared/e1000_art/body/e1000_housing_model.inc"
};

static NJS_CNK_OBJECT e1000_housing_node = {
#include "shared/e1000_art/body/e1000_housing_node.inc"
};

static Sint16 e1000_body_polygons[935] = {
#include "shared/e1000_art/body/e1000_body_polygons.inc"
};

static Sint32 e1000_body_vertices[579] = {
#include "shared/e1000_art/body/e1000_body_vertices.inc"
};

static NJS_CNK_MODEL e1000_body_model = {
#include "shared/e1000_art/body/e1000_body_model.inc"
};

static NJS_CNK_OBJECT e1000_object = {
#include "shared/e1000_art/body/e1000_object.inc"
};

#endif
