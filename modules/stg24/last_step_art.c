/* LASTSTEP's art in Eternal Engine: the deck, its collision and the two
 * sliding panels. no code reads deck_object */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/step_models.h"

static Sint16 left_panel_polygons[243] = {
#include "stg24/last_step_art/left_panel_polygons.inc"
};

static Sint32 left_panel_vertices[195] = {
#include "stg24/last_step_art/left_panel_vertices.inc"
};

static NJS_CNK_MODEL left_panel_mesh = {
#include "stg24/last_step_art/left_panel_mesh.inc"
};

NJS_CNK_OBJECT last_step_left_panel = {
#include "stg24/last_step_art/last_step_left_panel.inc"
};

static Sint16 right_panel_polygons[243] = {
#include "stg24/last_step_art/right_panel_polygons.inc"
};

static Sint32 right_panel_vertices[195] = {
#include "stg24/last_step_art/right_panel_vertices.inc"
};

static NJS_CNK_MODEL right_panel_mesh = {
#include "stg24/last_step_art/right_panel_mesh.inc"
};

NJS_CNK_OBJECT last_step_right_panel = {
#include "stg24/last_step_art/last_step_right_panel.inc"
};

static Sint16 deck_polygons[489] = {
#include "stg24/last_step_art/deck_polygons.inc"
};

static Sint32 deck_vertices[371] = {
#include "stg24/last_step_art/deck_vertices.inc"
};

NJS_CNK_MODEL last_step_model = {
#include "stg24/last_step_art/last_step_model.inc"
};

static NJS_CNK_OBJECT deck_object = {
#include "stg24/last_step_art/deck_object.inc"
};

#include "shared/steps/last_step_collision.h"

NJS_OBJECT last_step_collision = {
#include "stg24/last_step_art/last_step_collision.inc"
};
