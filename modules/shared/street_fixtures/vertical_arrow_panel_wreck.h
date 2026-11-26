/* Vertical arrow panel wreck in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_VERTICAL_ARROW_PANEL_WRECK_H
#define SHARED_STREET_FIXTURES_VERTICAL_ARROW_PANEL_WRECK_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME vertical_arrow_panel_wreck_textures[2] = {
#include "shared/street_fixtures/vertical_arrow_panel_wreck/vertical_arrow_panel_wreck_textures.inc"
};

static NJS_TEXLIST vertical_arrow_panel_wreck_texlist = {
#include "shared/street_fixtures/vertical_arrow_panel_wreck/vertical_arrow_panel_wreck_texlist.inc"
};

static Sint16 vertical_arrow_panel_wreck_polygons[179] = {
#include "shared/street_fixtures/vertical_arrow_panel_wreck/vertical_arrow_panel_wreck_polygons.inc"
};

static Sint32 vertical_arrow_panel_wreck_vertices[147] = {
#include "shared/street_fixtures/vertical_arrow_panel_wreck/vertical_arrow_panel_wreck_vertices.inc"
};

static NJS_CNK_MODEL vertical_arrow_panel_wreck_mesh = {
#include "shared/street_fixtures/vertical_arrow_panel_wreck/vertical_arrow_panel_wreck_mesh.inc"
};

static NJS_CNK_OBJECT vertical_arrow_panel_wreck_object = {
#include "shared/street_fixtures/vertical_arrow_panel_wreck/vertical_arrow_panel_wreck_object.inc"
};

#endif
