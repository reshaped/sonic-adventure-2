/* the near model of the enemy figure, an aircraft. a hit breaks it into its
 * nodes, and the shadow stays where it lies */
#ifndef SHARED_DECK_FIGURE_ART_DECK_ENEMY_H
#define SHARED_DECK_FIGURE_ART_DECK_ENEMY_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME deck_enemy_textures[9] = {
#include "shared/deck_figure_art/enemy/deck_enemy_textures.inc"
};

static NJS_TEXLIST deck_enemy_texlist = {
#include "shared/deck_figure_art/enemy/deck_enemy_texlist.inc"
};

static Sint16 deck_enemy_right_nacelle_polygons[279] = {
#include "shared/deck_figure_art/enemy/deck_enemy_right_nacelle_polygons.inc"
};

static Sint32 deck_enemy_right_nacelle_vertices[175] = {
#include "shared/deck_figure_art/enemy/deck_enemy_right_nacelle_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_right_nacelle_model = {
#include "shared/deck_figure_art/enemy/deck_enemy_right_nacelle_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_right_nacelle_node = {
#include "shared/deck_figure_art/enemy/deck_enemy_right_nacelle_node.inc"
};

static Sint16 deck_enemy_right_wing_polygons[85] = {
#include "shared/deck_figure_art/enemy/deck_enemy_right_wing_polygons.inc"
};

static Sint32 deck_enemy_right_wing_vertices[71] = {
#include "shared/deck_figure_art/enemy/deck_enemy_right_wing_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_right_wing_model = {
#include "shared/deck_figure_art/enemy/deck_enemy_right_wing_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_right_wing_node = {
#include "shared/deck_figure_art/enemy/deck_enemy_right_wing_node.inc"
};

static Sint16 deck_enemy_shadow_polygons[191] = {
#include "shared/deck_figure_art/enemy/deck_enemy_shadow_polygons.inc"
};

static Sint32 deck_enemy_shadow_vertices[131] = {
#include "shared/deck_figure_art/enemy/deck_enemy_shadow_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_shadow_model = {
#include "shared/deck_figure_art/enemy/deck_enemy_shadow_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_shadow_node = {
#include "shared/deck_figure_art/enemy/deck_enemy_shadow_node.inc"
};

static Sint16 deck_enemy_forward_panel_polygons[39] = {
#include "shared/deck_figure_art/enemy/deck_enemy_forward_panel_polygons.inc"
};

static Sint32 deck_enemy_forward_panel_vertices[35] = {
#include "shared/deck_figure_art/enemy/deck_enemy_forward_panel_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_forward_panel_model = {
#include "shared/deck_figure_art/enemy/deck_enemy_forward_panel_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_forward_panel_node = {
#include "shared/deck_figure_art/enemy/deck_enemy_forward_panel_node.inc"
};

static Sint16 deck_enemy_aft_fuselage_polygons[507] = {
#include "shared/deck_figure_art/enemy/deck_enemy_aft_fuselage_polygons.inc"
};

static Sint32 deck_enemy_aft_fuselage_vertices[487] = {
#include "shared/deck_figure_art/enemy/deck_enemy_aft_fuselage_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_aft_fuselage_model = {
#include "shared/deck_figure_art/enemy/deck_enemy_aft_fuselage_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_aft_fuselage_node = {
#include "shared/deck_figure_art/enemy/deck_enemy_aft_fuselage_node.inc"
};

static Sint16 deck_enemy_left_nacelle_polygons[279] = {
#include "shared/deck_figure_art/enemy/deck_enemy_left_nacelle_polygons.inc"
};

static Sint32 deck_enemy_left_nacelle_vertices[175] = {
#include "shared/deck_figure_art/enemy/deck_enemy_left_nacelle_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_left_nacelle_model = {
#include "shared/deck_figure_art/enemy/deck_enemy_left_nacelle_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_left_nacelle_node = {
#include "shared/deck_figure_art/enemy/deck_enemy_left_nacelle_node.inc"
};

static Sint16 deck_enemy_nose_marking_polygons[35] = {
#include "shared/deck_figure_art/enemy/deck_enemy_nose_marking_polygons.inc"
};

static Sint32 deck_enemy_nose_marking_vertices[27] = {
#include "shared/deck_figure_art/enemy/deck_enemy_nose_marking_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_nose_marking_model = {
#include "shared/deck_figure_art/enemy/deck_enemy_nose_marking_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_nose_marking_node = {
#include "shared/deck_figure_art/enemy/deck_enemy_nose_marking_node.inc"
};

static Sint16 deck_enemy_wing_markings_polygons[67] = {
#include "shared/deck_figure_art/enemy/deck_enemy_wing_markings_polygons.inc"
};

static Sint32 deck_enemy_wing_markings_vertices[67] = {
#include "shared/deck_figure_art/enemy/deck_enemy_wing_markings_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_wing_markings_model = {
#include "shared/deck_figure_art/enemy/deck_enemy_wing_markings_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_wing_markings_node = {
#include "shared/deck_figure_art/enemy/deck_enemy_wing_markings_node.inc"
};

static Sint16 deck_enemy_belly_guns_polygons[39] = {
#include "shared/deck_figure_art/enemy/deck_enemy_belly_guns_polygons.inc"
};

static Sint32 deck_enemy_belly_guns_vertices[35] = {
#include "shared/deck_figure_art/enemy/deck_enemy_belly_guns_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_belly_guns_model = {
#include "shared/deck_figure_art/enemy/deck_enemy_belly_guns_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_belly_guns_node = {
#include "shared/deck_figure_art/enemy/deck_enemy_belly_guns_node.inc"
};

static Sint16 deck_enemy_belly_panel_polygons[105] = {
#include "shared/deck_figure_art/enemy/deck_enemy_belly_panel_polygons.inc"
};

static Sint32 deck_enemy_belly_panel_vertices[83] = {
#include "shared/deck_figure_art/enemy/deck_enemy_belly_panel_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_belly_panel_model = {
#include "shared/deck_figure_art/enemy/deck_enemy_belly_panel_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_belly_panel_node = {
#include "shared/deck_figure_art/enemy/deck_enemy_belly_panel_node.inc"
};

static Sint16 deck_enemy_undercarriage_polygons[441] = {
#include "shared/deck_figure_art/enemy/deck_enemy_undercarriage_polygons.inc"
};

static Sint32 deck_enemy_undercarriage_vertices[371] = {
#include "shared/deck_figure_art/enemy/deck_enemy_undercarriage_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_undercarriage_model = {
#include "shared/deck_figure_art/enemy/deck_enemy_undercarriage_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_undercarriage_node = {
#include "shared/deck_figure_art/enemy/deck_enemy_undercarriage_node.inc"
};

static Sint16 deck_enemy_left_wing_polygons[85] = {
#include "shared/deck_figure_art/enemy/deck_enemy_left_wing_polygons.inc"
};

static Sint32 deck_enemy_left_wing_vertices[71] = {
#include "shared/deck_figure_art/enemy/deck_enemy_left_wing_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_left_wing_model = {
#include "shared/deck_figure_art/enemy/deck_enemy_left_wing_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_left_wing_node = {
#include "shared/deck_figure_art/enemy/deck_enemy_left_wing_node.inc"
};

static Sint16 deck_enemy_forward_fuselage_polygons[797] = {
#include "shared/deck_figure_art/enemy/deck_enemy_forward_fuselage_polygons.inc"
};

static Sint32 deck_enemy_forward_fuselage_vertices[563] = {
#include "shared/deck_figure_art/enemy/deck_enemy_forward_fuselage_vertices.inc"
};

static NJS_CNK_MODEL deck_enemy_forward_fuselage_model = {
#include "shared/deck_figure_art/enemy/deck_enemy_forward_fuselage_model.inc"
};

static NJS_CNK_OBJECT deck_enemy_object = {
#include "shared/deck_figure_art/enemy/deck_enemy_object.inc"
};

#endif
