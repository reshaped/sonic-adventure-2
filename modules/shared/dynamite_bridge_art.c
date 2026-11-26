/* Eternal Engine bridge deck, lamp markings and collision. */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/dynamite_bridge.h"

static Sint16 bridge_polygons[479] = {
#include "shared/dynamite_bridge_art/bridge_polygons.inc"
};

static Sint32 bridge_vertices[339] = {
#include "shared/dynamite_bridge_art/bridge_vertices.inc"
};

NJS_CNK_MODEL bridge_model[1] = {
#include "shared/dynamite_bridge_art/bridge_model.inc"
};

static NJS_CNK_OBJECT bridge_object = {
#include "shared/dynamite_bridge_art/bridge_object.inc"
};

#include "shared/bridge_art/collision.h"

NJS_OBJECT dynamite_bridge_collision = {
#include "shared/dynamite_bridge_art/dynamite_bridge_collision.inc"
};

static NJS_OBJECT dynamite_bridge_collision_root = {
#include "shared/dynamite_bridge_art/dynamite_bridge_collision_root.inc"
};
