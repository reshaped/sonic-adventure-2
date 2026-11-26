/* Third fragment of the Final Rush boulder. */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg30/rockfall_art.h"

static NJS_TEXNAME textures[2] = {
#include "stg30/rocks/shard_c/textures.inc"
};

NJS_TEXLIST shard_c_texlist = {
#include "stg30/rocks/shard_c/shard_c_texlist.inc"
};

static Sint16 shard_c_polygons[265] = {
#include "stg30/rocks/shard_c/shard_c_polygons.inc"
};

static Sint32 shard_c_vertices[167] = {
#include "stg30/rocks/shard_c/shard_c_vertices.inc"
};

static NJS_CNK_MODEL shard_c_mesh = {
#include "stg30/rocks/shard_c/shard_c_mesh.inc"
};

NJS_CNK_OBJECT shard_c_object = {
#include "stg30/rocks/shard_c/shard_c_object.inc"
};
