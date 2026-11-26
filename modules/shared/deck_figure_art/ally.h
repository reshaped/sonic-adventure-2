/* the model of the first ally figure, which draws at every distance. a hit
 * breaks it into its six nodes: the body, the upper body, both legs and both
 * guns */
#ifndef SHARED_DECK_FIGURE_ART_DECK_ALLY_H
#define SHARED_DECK_FIGURE_ART_DECK_ALLY_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME deck_ally_textures[5] = {
#include "shared/deck_figure_art/ally/deck_ally_textures.inc"
};

static NJS_TEXLIST deck_ally_texlist = {
#include "shared/deck_figure_art/ally/deck_ally_texlist.inc"
};

static Sint16 deck_ally_left_leg_polygons[217] = {
#include "shared/deck_figure_art/ally/deck_ally_left_leg_polygons.inc"
};

static Sint32 deck_ally_left_leg_vertices[123] = {
#include "shared/deck_figure_art/ally/deck_ally_left_leg_vertices.inc"
};

static NJS_CNK_MODEL deck_ally_left_leg_model = {
#include "shared/deck_figure_art/ally/deck_ally_left_leg_model.inc"
};

static NJS_CNK_OBJECT deck_ally_left_leg_node = {
#include "shared/deck_figure_art/ally/deck_ally_left_leg_node.inc"
};

static Sint16 deck_ally_left_gun_polygons[317] = {
#include "shared/deck_figure_art/ally/deck_ally_left_gun_polygons.inc"
};

static Sint32 deck_ally_left_gun_vertices[235] = {
#include "shared/deck_figure_art/ally/deck_ally_left_gun_vertices.inc"
};

static NJS_CNK_MODEL deck_ally_left_gun_model = {
#include "shared/deck_figure_art/ally/deck_ally_left_gun_model.inc"
};

static NJS_CNK_OBJECT deck_ally_left_gun_node = {
#include "shared/deck_figure_art/ally/deck_ally_left_gun_node.inc"
};

static Sint16 deck_ally_right_gun_polygons[269] = {
#include "shared/deck_figure_art/ally/deck_ally_right_gun_polygons.inc"
};

static Sint32 deck_ally_right_gun_vertices[223] = {
#include "shared/deck_figure_art/ally/deck_ally_right_gun_vertices.inc"
};

static NJS_CNK_MODEL deck_ally_right_gun_model = {
#include "shared/deck_figure_art/ally/deck_ally_right_gun_model.inc"
};

static NJS_CNK_OBJECT deck_ally_right_gun_node = {
#include "shared/deck_figure_art/ally/deck_ally_right_gun_node.inc"
};

static Sint16 deck_ally_upper_body_polygons[543] = {
#include "shared/deck_figure_art/ally/deck_ally_upper_body_polygons.inc"
};

static Sint32 deck_ally_upper_body_vertices[359] = {
#include "shared/deck_figure_art/ally/deck_ally_upper_body_vertices.inc"
};

static NJS_CNK_MODEL deck_ally_upper_body_model = {
#include "shared/deck_figure_art/ally/deck_ally_upper_body_model.inc"
};

static NJS_CNK_OBJECT deck_ally_upper_body_node = {
#include "shared/deck_figure_art/ally/deck_ally_upper_body_node.inc"
};

static Sint16 deck_ally_right_leg_polygons[217] = {
#include "shared/deck_figure_art/ally/deck_ally_right_leg_polygons.inc"
};

static Sint32 deck_ally_right_leg_vertices[155] = {
#include "shared/deck_figure_art/ally/deck_ally_right_leg_vertices.inc"
};

static NJS_CNK_MODEL deck_ally_right_leg_model = {
#include "shared/deck_figure_art/ally/deck_ally_right_leg_model.inc"
};

static NJS_CNK_OBJECT deck_ally_right_leg_node = {
#include "shared/deck_figure_art/ally/deck_ally_right_leg_node.inc"
};

static Sint16 deck_ally_body_polygons[279] = {
#include "shared/deck_figure_art/ally/deck_ally_body_polygons.inc"
};

static Sint32 deck_ally_body_vertices[163] = {
#include "shared/deck_figure_art/ally/deck_ally_body_vertices.inc"
};

static NJS_CNK_MODEL deck_ally_body_model = {
#include "shared/deck_figure_art/ally/deck_ally_body_model.inc"
};

static NJS_CNK_OBJECT deck_ally_object = {
#include "shared/deck_figure_art/ally/deck_ally_object.inc"
};

#endif
