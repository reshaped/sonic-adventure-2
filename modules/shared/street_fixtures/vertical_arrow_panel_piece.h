/* Vertical arrow panel piece in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_VERTICAL_ARROW_PANEL_PIECE_H
#define SHARED_STREET_FIXTURES_VERTICAL_ARROW_PANEL_PIECE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME vertical_arrow_panel_piece_textures[1] = {
#include "shared/street_fixtures/vertical_arrow_panel_piece/vertical_arrow_panel_piece_textures.inc"
};

static NJS_TEXLIST vertical_arrow_panel_piece_texlist = {
#include "shared/street_fixtures/vertical_arrow_panel_piece/vertical_arrow_panel_piece_texlist.inc"
};

static Sint16 vertical_arrow_panel_piece_polygons[43] = {
#include "shared/street_fixtures/vertical_arrow_panel_piece/vertical_arrow_panel_piece_polygons.inc"
};

static Sint32 vertical_arrow_panel_piece_vertices[31] = {
#include "shared/street_fixtures/vertical_arrow_panel_piece/vertical_arrow_panel_piece_vertices.inc"
};

static NJS_CNK_MODEL vertical_arrow_panel_piece_model = {
#include "shared/street_fixtures/vertical_arrow_panel_piece/vertical_arrow_panel_piece_model.inc"
};

static NJS_CNK_OBJECT vertical_arrow_panel_piece_object = {
#include "shared/street_fixtures/vertical_arrow_panel_piece/vertical_arrow_panel_piece_object.inc"
};

#endif
