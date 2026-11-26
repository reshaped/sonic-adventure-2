#ifndef STG07_SCENERY_WATERWEED_H
#define STG07_SCENERY_WATERWEED_H

#include <ninja.h>

static NJS_TEXNAME waterweed_texlist_names[1] = {
#include "stg07/scenery/waterweed/waterweed_texlist_names.inc"
};

static NJS_TEXLIST waterweed_texlist = {
#include "stg07/scenery/waterweed/waterweed_texlist.inc"
};

static Sint16 waterweed_polygons[351] = {
#include "stg07/scenery/waterweed/waterweed_polygons.inc"
};

static Sint32 waterweed_vertices[275] = {
#include "stg07/scenery/waterweed/waterweed_vertices.inc"
};

static NJS_CNK_MODEL waterweed_model = {
#include "stg07/scenery/waterweed/waterweed_model.inc"
};

static NJS_CNK_OBJECT waterweed_object = {
#include "stg07/scenery/waterweed/waterweed_object.inc"
};

static NJS_MKEY_F waterweed_positions[2] = {
#include "stg07/scenery/waterweed/waterweed_positions.inc"
};

static NJS_MKEY_A waterweed_angles[2] = {
#include "stg07/scenery/waterweed/waterweed_angles.inc"
};

static NJS_MDATA2 waterweed_motion_tracks = {
#include "stg07/scenery/waterweed/waterweed_motion_tracks.inc"
};

static NJS_MOTION waterweed_motion = {
#include "stg07/scenery/waterweed/waterweed_motion.inc"
};

struct waterweed_action {
    NJS_CNK_OBJECT *object;
    NJS_MOTION *motion;
};

/* The original model also stores this standalone object and motion pair. */
static struct waterweed_action waterweed_action = {
#include "stg07/scenery/waterweed/waterweed_action.inc"
};

static NJS_POINT3 waterweed_pose_0[68] = {
#include "stg07/scenery/waterweed/waterweed_pose_0.inc"
};

static NJS_POINT3 waterweed_pose_4[68] = {
#include "stg07/scenery/waterweed/waterweed_pose_4.inc"
};

static NJS_POINT3 waterweed_pose_7[68] = {
#include "stg07/scenery/waterweed/waterweed_pose_7.inc"
};

static NJS_MKEY_P waterweed_shape_keys[3] = {
#include "stg07/scenery/waterweed/waterweed_shape_keys.inc"
};

static NJS_MDATA2 waterweed_shape_tracks = {
#include "stg07/scenery/waterweed/waterweed_shape_tracks.inc"
};

static NJS_MOTION waterweed_shape = {
#include "stg07/scenery/waterweed/waterweed_shape.inc"
};

#endif
