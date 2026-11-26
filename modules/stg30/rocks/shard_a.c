/* First fragment of the Final Rush boulder. */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg30/rockfall_art.h"

static NJS_TEXNAME textures[2] = {
#include "stg30/rocks/shard_a/textures.inc"
};

NJS_TEXLIST shard_a_texlist = {
#include "stg30/rocks/shard_a/shard_a_texlist.inc"
};

static Sint16 shard_a_polygons[403] = {
#include "stg30/rocks/shard_a/shard_a_polygons.inc"
};

static Sint32 shard_a_vertices[267] = {
#include "stg30/rocks/shard_a/shard_a_vertices.inc"
};

static NJS_CNK_MODEL shard_a_mesh = {
#include "stg30/rocks/shard_a/shard_a_mesh.inc"
};

NJS_CNK_OBJECT shard_a_object = {
#include "stg30/rocks/shard_a/shard_a_object.inc"
};
