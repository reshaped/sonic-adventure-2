/* Horizontal arrow panel wreck in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_HORIZONTAL_ARROW_PANEL_WRECK_H
#define SHARED_STREET_FIXTURES_HORIZONTAL_ARROW_PANEL_WRECK_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME horizontal_arrow_panel_wreck_textures[1] = {
#include "shared/street_fixtures/horizontal_arrow_panel_wreck/horizontal_arrow_panel_wreck_textures.inc"
};

static NJS_TEXLIST horizontal_arrow_panel_wreck_texlist = {
#include "shared/street_fixtures/horizontal_arrow_panel_wreck/horizontal_arrow_panel_wreck_texlist.inc"
};

static Sint16 horizontal_arrow_panel_wreck_polygons[417] = {
#include "shared/street_fixtures/horizontal_arrow_panel_wreck/horizontal_arrow_panel_wreck_polygons.inc"
};

static Sint32 horizontal_arrow_panel_wreck_vertices[383] = {
#include "shared/street_fixtures/horizontal_arrow_panel_wreck/horizontal_arrow_panel_wreck_vertices.inc"
};

static NJS_CNK_MODEL horizontal_arrow_panel_wreck_mesh = {
#include "shared/street_fixtures/horizontal_arrow_panel_wreck/horizontal_arrow_panel_wreck_mesh.inc"
};

static NJS_CNK_OBJECT horizontal_arrow_panel_wreck_object = {
#include "shared/street_fixtures/horizontal_arrow_panel_wreck/horizontal_arrow_panel_wreck_object.inc"
};

#endif
