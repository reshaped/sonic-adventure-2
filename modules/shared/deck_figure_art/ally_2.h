/* the model of the second ally figure, which draws at every distance. it has
 * the first's nodes and a shadow. a hit leaves the shadow where it lies */
#ifndef SHARED_DECK_FIGURE_ART_DECK_ALLY_2_H
#define SHARED_DECK_FIGURE_ART_DECK_ALLY_2_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME deck_ally_2_textures[5] = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_textures.inc"
};

static NJS_TEXLIST deck_ally_2_texlist = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_texlist.inc"
};

static Sint16 deck_ally_2_left_leg_polygons[217] = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_left_leg_polygons.inc"
};

static Sint32 deck_ally_2_left_leg_vertices[143] = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_left_leg_vertices.inc"
};

static NJS_CNK_MODEL deck_ally_2_left_leg_model = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_left_leg_model.inc"
};

static NJS_CNK_OBJECT deck_ally_2_left_leg_node = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_left_leg_node.inc"
};

static Sint16 deck_ally_2_shadow_polygons[27] = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_shadow_polygons.inc"
};

static Sint32 deck_ally_2_shadow_vertices[47] = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_shadow_vertices.inc"
};

static NJS_CNK_MODEL deck_ally_2_shadow_model = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_shadow_model.inc"
};

static NJS_CNK_OBJECT deck_ally_shadow_node = {
#include "shared/deck_figure_art/ally_2/deck_ally_shadow_node.inc"
};

static Sint16 deck_ally_2_left_gun_polygons[331] = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_left_gun_polygons.inc"
};

static Sint32 deck_ally_2_left_gun_vertices[299] = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_left_gun_vertices.inc"
};

static NJS_CNK_MODEL deck_ally_2_left_gun_model = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_left_gun_model.inc"
};

static NJS_CNK_OBJECT deck_ally_2_left_gun_node = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_left_gun_node.inc"
};

static Sint16 deck_ally_2_right_gun_polygons[267] = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_right_gun_polygons.inc"
};

static Sint32 deck_ally_2_right_gun_vertices[287] = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_right_gun_vertices.inc"
};

static NJS_CNK_MODEL deck_ally_2_right_gun_model = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_right_gun_model.inc"
};

static NJS_CNK_OBJECT deck_ally_2_right_gun_node = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_right_gun_node.inc"
};

static Sint16 deck_ally_2_upper_body_polygons[551] = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_upper_body_polygons.inc"
};

static Sint32 deck_ally_2_upper_body_vertices[435] = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_upper_body_vertices.inc"
};

static NJS_CNK_MODEL deck_ally_2_upper_body_model = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_upper_body_model.inc"
};

static NJS_CNK_OBJECT deck_ally_2_upper_body_node = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_upper_body_node.inc"
};

static Sint16 deck_ally_2_right_leg_polygons[217] = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_right_leg_polygons.inc"
};

static Sint32 deck_ally_2_right_leg_vertices[143] = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_right_leg_vertices.inc"
};

static NJS_CNK_MODEL deck_ally_2_right_leg_model = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_right_leg_model.inc"
};

static NJS_CNK_OBJECT deck_ally_2_right_leg_node = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_right_leg_node.inc"
};

static Sint16 deck_ally_2_body_polygons[287] = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_body_polygons.inc"
};

static Sint32 deck_ally_2_body_vertices[211] = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_body_vertices.inc"
};

static NJS_CNK_MODEL deck_ally_2_body_model = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_body_model.inc"
};

static NJS_CNK_OBJECT deck_ally_2_object = {
#include "shared/deck_figure_art/ally_2/deck_ally_2_object.inc"
};

#endif
