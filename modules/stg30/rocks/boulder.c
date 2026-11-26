/* Boulder released by the Final Rush rockfall. */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg30/rockfall_art.h"

const char rock_miu128_fr004[16] = {
#include "stg30/rocks/boulder/rock_miu128_fr004.inc"
};

const char rock_miu128_fr005[16] = {
#include "stg30/rocks/boulder/rock_miu128_fr005.inc"
};

const char rock_miu128_fr006[16] = {
#include "stg30/rocks/boulder/rock_miu128_fr006.inc"
};

const char rock_miu128_fr007[13] = {
#include "stg30/rocks/boulder/rock_miu128_fr007.inc"
};

static NJS_TEXNAME textures[4] = {
#include "stg30/rocks/boulder/textures.inc"
};

NJS_TEXLIST boulder_texlist = {
#include "stg30/rocks/boulder/boulder_texlist.inc"
};

static Sint16 boulder_polygons[1017] = {
#include "stg30/rocks/boulder/boulder_polygons.inc"
};

static Sint32 boulder_vertices[879] = {
#include "stg30/rocks/boulder/boulder_vertices.inc"
};

static NJS_CNK_MODEL boulder_mesh = {
#include "stg30/rocks/boulder/boulder_mesh.inc"
};

NJS_CNK_OBJECT boulder_object = {
#include "stg30/rocks/boulder/boulder_object.inc"
};
