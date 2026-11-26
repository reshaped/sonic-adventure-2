/* the guard flare that both last bosses draw, and its texture list.
 * defend_flare_shape moves the flare's vertices through keys at frames 0, 2 and
 * 4. defend_flare_motion has no keys, so the flare stays where its node puts
 * it. no code reads unused_spark */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/last_boss_defend.h"
#include "shared/last_boss_defend_art.h"

#include "shared/last_boss_defend_art/text.inc"

static Sint16 unused_spark_polygons[71] = {
#include "shared/last_boss_defend_art/unused_spark_polygons.inc"
};

static Sint32 unused_spark_vertices[69] = {
#include "shared/last_boss_defend_art/unused_spark_vertices.inc"
};

static NJS_CNK_MODEL unused_spark_mesh = {
#include "shared/last_boss_defend_art/unused_spark_mesh.inc"
};

static NJS_CNK_OBJECT unused_spark = {
#include "shared/last_boss_defend_art/unused_spark.inc"
};

static Sint16 defend_flare_polygons[213] = {
#include "shared/last_boss_defend_art/defend_flare_polygons.inc"
};

static Sint32 defend_flare_vertices[243] = {
#include "shared/last_boss_defend_art/defend_flare_vertices.inc"
};

static NJS_CNK_MODEL defend_flare_mesh = {
#include "shared/last_boss_defend_art/defend_flare_mesh.inc"
};

NJS_CNK_OBJECT defend_flare_object[1] = {
#include "shared/last_boss_defend_art/defend_flare_object.inc"
};

static NJS_POINT3 flare_points_0[40] = {
#include "shared/last_boss_defend_art/flare_points_0.inc"
};

static NJS_POINT3 flare_points_2[40] = {
#include "shared/last_boss_defend_art/flare_points_2.inc"
};

static NJS_POINT3 flare_points_4[40] = {
#include "shared/last_boss_defend_art/flare_points_4.inc"
};

static NJS_MKEY_P flare_shape_keys[3] = {
#include "shared/last_boss_defend_art/flare_shape_keys.inc"
};

static NJS_MDATA2 shape_channels[1] = {
#include "shared/last_boss_defend_art/shape_channels.inc"
};

NJS_MOTION defend_flare_shape = {
#include "shared/last_boss_defend_art/defend_flare_shape.inc"
};

static NJS_MDATA2 rest_channels[1] = {
#include "shared/last_boss_defend_art/rest_channels.inc"
};

NJS_MOTION defend_flare_motion = {
#include "shared/last_boss_defend_art/defend_flare_motion.inc"
};

static NJS_TEXNAME defend_textures[14] = {
#include "shared/last_boss_defend_art/defend_textures.inc"
};

NJS_TEXLIST defend_texlist = {
#include "shared/last_boss_defend_art/defend_texlist.inc"
};
