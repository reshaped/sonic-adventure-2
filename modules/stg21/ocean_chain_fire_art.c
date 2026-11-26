#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/fire_fixture.h"

static Sint16 chain_fire_polygons[231] = {
#include "stg21/ocean_chain_fire_art/chain_fire_polygons.inc"
};

static Sint32 chain_fire_vertices[215] = {
#include "stg21/ocean_chain_fire_art/chain_fire_vertices.inc"
};

NJS_CNK_MODEL chain_fire_model = {
#include "stg21/ocean_chain_fire_art/chain_fire_model.inc"
};

static NJS_CNK_OBJECT chain_fire_object = {
#include "stg21/ocean_chain_fire_art/chain_fire_object.inc"
};
