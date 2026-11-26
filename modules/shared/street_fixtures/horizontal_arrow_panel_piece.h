/* Horizontal arrow panel piece in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_HORIZONTAL_ARROW_PANEL_PIECE_H
#define SHARED_STREET_FIXTURES_HORIZONTAL_ARROW_PANEL_PIECE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME horizontal_arrow_panel_piece_textures[1] = {
#include "shared/street_fixtures/horizontal_arrow_panel_piece/horizontal_arrow_panel_piece_textures.inc"
};

static NJS_TEXLIST horizontal_arrow_panel_piece_texlist = {
#include "shared/street_fixtures/horizontal_arrow_panel_piece/horizontal_arrow_panel_piece_texlist.inc"
};

static Sint16 horizontal_arrow_panel_piece_polygons[45] = {
#include "shared/street_fixtures/horizontal_arrow_panel_piece/horizontal_arrow_panel_piece_polygons.inc"
};

static Sint32 horizontal_arrow_panel_piece_vertices[43] = {
#include "shared/street_fixtures/horizontal_arrow_panel_piece/horizontal_arrow_panel_piece_vertices.inc"
};

static NJS_CNK_MODEL horizontal_arrow_panel_piece_model = {
#include "shared/street_fixtures/horizontal_arrow_panel_piece/horizontal_arrow_panel_piece_model.inc"
};

static NJS_CNK_OBJECT horizontal_arrow_panel_piece_object = {
#include "shared/street_fixtures/horizontal_arrow_panel_piece/horizontal_arrow_panel_piece_object.inc"
};

#endif
