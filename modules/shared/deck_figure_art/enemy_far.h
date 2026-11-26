/* the enemy figure's far model, with fewer nodes than the near one */
#ifndef SHARED_DECK_FIGURE_ART_DECK_ENEMY_FAR_H
#define SHARED_DECK_FIGURE_ART_DECK_ENEMY_FAR_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME deck_enemy_far_textures[8] = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_textures.inc"
};

static NJS_TEXLIST deck_enemy_far_texlist = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_texlist.inc"
};

static Sint16 deck_enemy_far_nose_marking_polygons[35] = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_nose_marking_polygons.inc"
};

static Sint32 deck_enemy_far_nose_marking_vertices[27] = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_nose_marking_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_far_nose_marking_model = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_nose_marking_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_far_nose_marking_node = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_nose_marking_node.inc"
};

static Sint16 deck_enemy_far_wing_markings_polygons[67] = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_wing_markings_polygons.inc"
};

static Sint32 deck_enemy_far_wing_markings_vertices[67] = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_wing_markings_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_far_wing_markings_model = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_wing_markings_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_far_wing_markings_node = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_wing_markings_node.inc"
};

static Sint16 deck_enemy_far_undercarriage_polygons[197] = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_undercarriage_polygons.inc"
};

static Sint32 deck_enemy_far_undercarriage_vertices[179] = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_undercarriage_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_far_undercarriage_model = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_undercarriage_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_far_undercarriage_node = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_undercarriage_node.inc"
};

static Sint16 deck_enemy_far_shadow_polygons[185] = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_shadow_polygons.inc"
};

static Sint32 deck_enemy_far_shadow_vertices[123] = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_shadow_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_far_shadow_model = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_shadow_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_far_shadow_node = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_shadow_node.inc"
};

static Sint16 deck_enemy_far_airframe_polygons[1051] = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_airframe_polygons.inc"
};

static Sint32 deck_enemy_far_airframe_vertices[827] = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_airframe_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_far_airframe_model = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_airframe_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_far_object = {
#include "shared/deck_figure_art/enemy_far/deck_enemy_far_object.inc"
};

#endif
