/* Road barricade in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_ROAD_BARRICADE_H
#define SHARED_STREET_FIXTURES_ROAD_BARRICADE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME road_barricade_textures[4] = {
#include "shared/street_fixtures/road_barricade/road_barricade_textures.inc"
};

static NJS_TEXLIST road_barricade_texlist = {
#include "shared/street_fixtures/road_barricade/road_barricade_texlist.inc"
};

static Sint16 road_barricade_object_node_3_polygons[97] = {
#include "shared/street_fixtures/road_barricade/road_barricade_object_node_3_polygons.inc"
};

static Sint32 road_barricade_object_node_3_vertices[39] = {
#include "shared/street_fixtures/road_barricade/road_barricade_object_node_3_vertices.inc"
};

static NJS_CNK_MODEL road_barricade_object_node_3_mesh = {
#include "shared/street_fixtures/road_barricade/road_barricade_object_node_3_mesh.inc"
};

static NJS_CNK_OBJECT road_barricade_object_node_3 = {
#include "shared/street_fixtures/road_barricade/road_barricade_object_node_3.inc"
};

static Sint16 road_barricade_object_node_2_polygons[97] = {
#include "shared/street_fixtures/road_barricade/road_barricade_object_node_2_polygons.inc"
};

static Sint32 road_barricade_object_node_2_vertices[39] = {
#include "shared/street_fixtures/road_barricade/road_barricade_object_node_2_vertices.inc"
};

static NJS_CNK_MODEL road_barricade_object_node_2_mesh = {
#include "shared/street_fixtures/road_barricade/road_barricade_object_node_2_mesh.inc"
};

static NJS_CNK_OBJECT road_barricade_object_node_2 = {
#include "shared/street_fixtures/road_barricade/road_barricade_object_node_2.inc"
};

static Sint16 road_barricade_object_node_1_polygons[97] = {
#include "shared/street_fixtures/road_barricade/road_barricade_object_node_1_polygons.inc"
};

static Sint32 road_barricade_object_node_1_vertices[39] = {
#include "shared/street_fixtures/road_barricade/road_barricade_object_node_1_vertices.inc"
};

static NJS_CNK_MODEL road_barricade_object_node_1_mesh = {
#include "shared/street_fixtures/road_barricade/road_barricade_object_node_1_mesh.inc"
};

static NJS_CNK_OBJECT road_barricade_object_node_1 = {
#include "shared/street_fixtures/road_barricade/road_barricade_object_node_1.inc"
};

static Sint16 road_barricade_polygons[1113] = {
#include "shared/street_fixtures/road_barricade/road_barricade_polygons.inc"
};

static Sint32 road_barricade_vertices[923] = {
#include "shared/street_fixtures/road_barricade/road_barricade_vertices.inc"
};

static NJS_CNK_MODEL road_barricade_mesh = {
#include "shared/street_fixtures/road_barricade/road_barricade_mesh.inc"
};

static NJS_CNK_OBJECT road_barricade_object = {
#include "shared/street_fixtures/road_barricade/road_barricade_object.inc"
};

#endif
