#include <ninja.h>
#include <sg_xpt.h>

#include "stg32/scenery_shapes.h"
#include "stg32/scenery/boulder/text.inc"

static NJS_TEXNAME large_meteor_names[8] = {
#include "stg32/scenery/boulder/large_meteor_names.inc"
};

NJS_TEXLIST large_meteor_texlist = {
#include "stg32/scenery/boulder/large_meteor_texlist.inc"
};

static Sint16 large_meteor_beacon_polygons[39] = {
#include "stg32/scenery/boulder/large_meteor_beacon_polygons.inc"
};

static Sint32 large_meteor_beacon_vertices[35] = {
#include "stg32/scenery/boulder/large_meteor_beacon_vertices.inc"
};

static NJS_CNK_MODEL large_meteor_beacon_model = {
#include "stg32/scenery/boulder/large_meteor_beacon_model.inc"
};

NJS_CNK_OBJECT large_meteor_beacon[1] = {
#include "stg32/scenery/boulder/large_meteor_beacon.inc"
};

static Sint16 large_meteor_lamp_polygons[55] = {
#include "stg32/scenery/boulder/large_meteor_lamp_polygons.inc"
};

static Sint32 large_meteor_lamp_vertices[51] = {
#include "stg32/scenery/boulder/large_meteor_lamp_vertices.inc"
};

static NJS_CNK_MODEL large_meteor_lamp_model = {
#include "stg32/scenery/boulder/large_meteor_lamp_model.inc"
};

NJS_CNK_OBJECT large_meteor_lamp[1] = {
#include "stg32/scenery/boulder/large_meteor_lamp.inc"
};

static Sint16 large_meteor_body_polygons[853] = {
#include "stg32/scenery/boulder/large_meteor_body_polygons.inc"
};

static Sint32 large_meteor_body_vertices[619] = {
#include "stg32/scenery/boulder/large_meteor_body_vertices.inc"
};

static NJS_CNK_MODEL large_meteor_body_model = {
#include "stg32/scenery/boulder/large_meteor_body_model.inc"
};

NJS_CNK_OBJECT large_meteor_body[1] = {
#include "stg32/scenery/boulder/large_meteor_body.inc"
};

static NJS_TEXNAME large_meteor_shard_names[2] = {
#include "stg32/scenery/boulder/large_meteor_shard_names.inc"
};

NJS_TEXLIST large_meteor_shard_texlist = {
#include "stg32/scenery/boulder/large_meteor_shard_texlist.inc"
};

static Sint16 large_meteor_shard_polygons[179] = {
#include "stg32/scenery/boulder/large_meteor_shard_polygons.inc"
};

static Sint32 large_meteor_shard_vertices[91] = {
#include "stg32/scenery/boulder/large_meteor_shard_vertices.inc"
};

static NJS_CNK_MODEL large_meteor_shard_model = {
#include "stg32/scenery/boulder/large_meteor_shard_model.inc"
};

NJS_CNK_OBJECT large_meteor_shard[1] = {
#include "stg32/scenery/boulder/large_meteor_shard.inc"
};

static NJS_TEXNAME large_meteor_chip_names[1] = {
#include "stg32/scenery/boulder/large_meteor_chip_names.inc"
};

NJS_TEXLIST large_meteor_chip_texlist = {
#include "stg32/scenery/boulder/large_meteor_chip_texlist.inc"
};

static Sint16 large_meteor_chip_polygons[165] = {
#include "stg32/scenery/boulder/large_meteor_chip_polygons.inc"
};

static Sint32 large_meteor_chip_vertices[127] = {
#include "stg32/scenery/boulder/large_meteor_chip_vertices.inc"
};

static NJS_CNK_MODEL large_meteor_chip_model = {
#include "stg32/scenery/boulder/large_meteor_chip_model.inc"
};

NJS_CNK_OBJECT large_meteor_chip[1] = {
#include "stg32/scenery/boulder/large_meteor_chip.inc"
};
