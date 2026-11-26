#ifndef SHARED_DECK_TRUCK_DAMAGED_H
#define SHARED_DECK_TRUCK_DAMAGED_H

#include <ninja.h>

static NJS_TEXNAME deck_truck_damaged_textures[10] = {
#include "shared/deck_truck/damaged/deck_truck_damaged_textures.inc"
};

static NJS_TEXLIST deck_truck_damaged_texlist = {
#include "shared/deck_truck/damaged/deck_truck_damaged_texlist.inc"
};

static Sint16 deck_truck_damaged_polygons[681] = {
#include "shared/deck_truck/damaged/deck_truck_damaged_polygons.inc"
};

static Sint32 deck_truck_damaged_vertices[619] = {
#include "shared/deck_truck/damaged/deck_truck_damaged_vertices.inc"
};

static NJS_CNK_MODEL deck_truck_damaged_model = {
#include "shared/deck_truck/damaged/deck_truck_damaged_model.inc"
};

static NJS_CNK_OBJECT deck_truck_damaged[1] = {
#include "shared/deck_truck/damaged/deck_truck_damaged.inc"
};

#endif
