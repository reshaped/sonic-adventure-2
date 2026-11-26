/* Trash can in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_TRASH_CAN_H
#define SHARED_STREET_FIXTURES_TRASH_CAN_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME trash_can_textures[3] = {
#include "shared/street_fixtures/trash_can/trash_can_textures.inc"
};

static NJS_TEXLIST trash_can_texlist = {
#include "shared/street_fixtures/trash_can/trash_can_texlist.inc"
};

static Sint16 trash_can_object_node_1_polygons[213] = {
#include "shared/street_fixtures/trash_can/trash_can_object_node_1_polygons.inc"
};

static Sint32 trash_can_object_node_1_vertices[151] = {
#include "shared/street_fixtures/trash_can/trash_can_object_node_1_vertices.inc"
};

static NJS_CNK_MODEL trash_can_piece_model = {
#include "shared/street_fixtures/trash_can/trash_can_piece_model.inc"
};

static NJS_CNK_OBJECT trash_can_object_node_1 = {
#include "shared/street_fixtures/trash_can/trash_can_object_node_1.inc"
};

static Sint16 trash_can_polygons[123] = {
#include "shared/street_fixtures/trash_can/trash_can_polygons.inc"
};

static Sint32 trash_can_vertices[99] = {
#include "shared/street_fixtures/trash_can/trash_can_vertices.inc"
};

static NJS_CNK_MODEL trash_can_mesh = {
#include "shared/street_fixtures/trash_can/trash_can_mesh.inc"
};

static NJS_CNK_OBJECT trash_can_object = {
#include "shared/street_fixtures/trash_can/trash_can_object.inc"
};

#endif
