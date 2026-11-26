#include <ninja.h>
#include <sg_xpt.h>

#include "shared/dynamite_bridge.h"

/* Bridge deck with its lit side mesh. */

static Sint16 bridge_light_polygons[309] = {
#include "stg27/dynamite_bridge_art/bridge_light_polygons.inc"
};

static Sint32 bridge_light_vertices[227] = {
#include "stg27/dynamite_bridge_art/bridge_light_vertices.inc"
};

NJS_CNK_MODEL bridge_light_model = {
#include "stg27/dynamite_bridge_art/bridge_light_model.inc"
};

static NJS_CNK_OBJECT bridge_light = {
#include "stg27/dynamite_bridge_art/bridge_light.inc"
};

static Sint16 bridge_polygons[305] = {
#include "stg27/dynamite_bridge_art/bridge_polygons.inc"
};

static Sint32 bridge_vertices[243] = {
#include "stg27/dynamite_bridge_art/bridge_vertices.inc"
};

NJS_CNK_MODEL bridge_model[1] = {
#include "stg27/dynamite_bridge_art/bridge_model.inc"
};

static NJS_CNK_OBJECT bridge_object = {
#include "stg27/dynamite_bridge_art/bridge_object.inc"
};
#include "shared/bridge_art/collision.h"

NJS_OBJECT dynamite_bridge_collision = {
#include "stg27/dynamite_bridge_art/dynamite_bridge_collision.inc"
};

static NJS_OBJECT dynamite_bridge_collision_root = {
#include "stg27/dynamite_bridge_art/dynamite_bridge_collision_root.inc"
};
