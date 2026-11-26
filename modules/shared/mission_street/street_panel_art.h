/* Stationary roadway panel. */
#ifndef SHARED_MISSION_STREET_STREET_PANEL_ART_H
#define SHARED_MISSION_STREET_STREET_PANEL_ART_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME street_panel_textures[1] = {
#include "shared/mission_street/street_panel_art/street_panel_textures.inc"
};

static NJS_TEXLIST street_panel_texlist = {
#include "shared/mission_street/street_panel_art/street_panel_texlist.inc"
};

static Sint16 street_panel_polygons[77] = {
#include "shared/mission_street/street_panel_art/street_panel_polygons.inc"
};

static Sint32 street_panel_vertices[83] = {
#include "shared/mission_street/street_panel_art/street_panel_vertices.inc"
};

static NJS_CNK_MODEL street_panel_model = {
#include "shared/mission_street/street_panel_art/street_panel_model.inc"
};

static NJS_CNK_OBJECT street_panel_object = {
#include "shared/mission_street/street_panel_art/street_panel_object.inc"
};

#endif
