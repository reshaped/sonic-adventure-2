/* Ground panel in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_GROUND_PANEL_H
#define SHARED_STREET_FIXTURES_GROUND_PANEL_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME ground_panel_textures[1] = {
#include "shared/street_fixtures/ground_panel/ground_panel_textures.inc"
};

static NJS_TEXLIST ground_panel_texlist = {
#include "shared/street_fixtures/ground_panel/ground_panel_texlist.inc"
};

static Sint16 ground_panel_polygons[25] = {
#include "shared/street_fixtures/ground_panel/ground_panel_polygons.inc"
};

static Sint32 ground_panel_vertices[19] = {
#include "shared/street_fixtures/ground_panel/ground_panel_vertices.inc"
};

static NJS_CNK_MODEL ground_panel_mesh = {
#include "shared/street_fixtures/ground_panel/ground_panel_mesh.inc"
};

static NJS_CNK_OBJECT ground_panel_object = {
#include "shared/street_fixtures/ground_panel/ground_panel_object.inc"
};

#endif
