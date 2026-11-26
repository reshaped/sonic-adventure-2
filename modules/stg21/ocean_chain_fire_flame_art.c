#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/fire_fixture.h"

static Sint16 chain_fire_flame_polygons[25] = {
#include "stg21/ocean_chain_fire_flame_art/chain_fire_flame_polygons.inc"
};

static Sint32 chain_fire_flame_vertices[19] = {
#include "stg21/ocean_chain_fire_flame_art/chain_fire_flame_vertices.inc"
};

NJS_CNK_MODEL chain_fire_flame_model[1] = {
#include "stg21/ocean_chain_fire_flame_art/chain_fire_flame_model.inc"
};

static NJS_CNK_OBJECT chain_fire_flame_object = {
#include "stg21/ocean_chain_fire_flame_art/chain_fire_flame_object.inc"
};
