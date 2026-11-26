/* Water beneath the carrier deck. The upward-facing strip lets the kill-area
 * test find the sea above a falling player. The last kill-area entry is null.
 */
#ifndef SHARED_CARRIER_WATER_H
#define SHARED_CARRIER_WATER_H

#include <ninja.h>
#include <sg_xpt.h>

#include "player/player.h"

static NJS_MATERIAL water_material = {
#include "shared/carrier_water/water_material.inc"
};
static Sint16 water_indices[5] = {
#include "shared/carrier_water/water_indices.inc"
};
static NJS_MESHSET water_mesh = {
#include "shared/carrier_water/water_mesh.inc"
};
static NJS_POINT3 water_corners[4] = {
#include "shared/carrier_water/water_corners.inc"
};
static NJS_VECTOR water_normals[4] = {
#include "shared/carrier_water/water_normals.inc"
};
static NJS_MODEL water_plane = {
#include "shared/carrier_water/water_plane.inc"
};
static NJS_OBJECT water_object = {
#include "shared/carrier_water/water_object.inc"
};
static struct kill_area kill_areas_on_deck[2] = {
#include "shared/carrier_water/kill_areas_on_deck.inc"
};

#endif
