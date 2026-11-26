/* Second fragment of the Final Rush boulder. */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg30/rockfall_art.h"

static NJS_TEXNAME textures[2] = {
#include "stg30/rocks/shard_b/textures.inc"
};

NJS_TEXLIST shard_b_texlist = {
#include "stg30/rocks/shard_b/shard_b_texlist.inc"
};

static Sint16 shard_b_polygons[251] = {
#include "stg30/rocks/shard_b/shard_b_polygons.inc"
};

static Sint32 shard_b_vertices[171] = {
#include "stg30/rocks/shard_b/shard_b_vertices.inc"
};

static NJS_CNK_MODEL shard_b_mesh = {
#include "stg30/rocks/shard_b/shard_b_mesh.inc"
};

NJS_CNK_OBJECT shard_b_object = {
#include "stg30/rocks/shard_b/shard_b_object.inc"
};
