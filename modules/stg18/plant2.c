/* Narrow-leaved aquatic plant and its shape motion. */
#include "stg18/plant2.h"

#include <ninja.h>
#include <sg_xpt.h>

#include "stg18/plant2/text.inc"

static NJS_TEXNAME plant2_names[2] = {
#include "stg18/plant2/plant2_names.inc"
};

static NJS_TEXLIST sway_texlist = {
#include "stg18/plant2/sway_texlist.inc"
};

static Sint16 plant2_polygons[317] = {
#include "stg18/plant2/plant2_polygons.inc"
};

static Sint32 plant2_vertices[263] = {
#include "stg18/plant2/plant2_vertices.inc"
};

static NJS_CNK_MODEL plant2_model = {
#include "stg18/plant2/plant2_model.inc"
};

static NJS_CNK_OBJECT sway_object[1] = {
#include "stg18/plant2/sway_object.inc"
};

static NJS_POINT3 plant2_pose_0[65] = {
#include "stg18/plant2/plant2_pose_0.inc"
};

static NJS_POINT3 plant2_pose_9[65] = {
#include "stg18/plant2/plant2_pose_9.inc"
};

static NJS_POINT3 plant2_pose_18[65] = {
#include "stg18/plant2/plant2_pose_18.inc"
};

static NJS_MKEY_P plant2_shape_keys[3] = {
#include "stg18/plant2/plant2_shape_keys.inc"
};

static NJS_MDATA2 plant2_shape_tracks = {
#include "stg18/plant2/plant2_shape_tracks.inc"
};

static NJS_MOTION sway_shape[1] = {
#include "stg18/plant2/sway_shape.inc"
};

static NJS_MDATA2 plant2_motion_tracks = {
#include "stg18/plant2/plant2_motion_tracks.inc"
};

static NJS_MOTION sway_motion = {
#include "stg18/plant2/sway_motion.inc"
};

NJS_TEXLIST *plant2_texlists[2] = {
#include "stg18/plant2/plant2_texlists.inc"
};

#include "stg18/sway_plant_impl.h" /* IWYU pragma: keep -- the code it compiles here */
