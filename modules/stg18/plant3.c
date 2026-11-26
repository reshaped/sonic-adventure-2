/* Round-leaved aquatic plant and its shape motion. */
#include "stg18/plant3.h"

#include <ninja.h>
#include <sg_xpt.h>

#include "stg18/plant3/text.inc"

static NJS_TEXNAME plant3_names[2] = {
#include "stg18/plant3/plant3_names.inc"
};

static NJS_TEXLIST sway_texlist = {
#include "stg18/plant3/sway_texlist.inc"
};

static Sint16 plant3_polygons[437] = {
#include "stg18/plant3/plant3_polygons.inc"
};

static Sint32 plant3_vertices[339] = {
#include "stg18/plant3/plant3_vertices.inc"
};

static NJS_CNK_MODEL plant3_model = {
#include "stg18/plant3/plant3_model.inc"
};

static NJS_CNK_OBJECT sway_object[1] = {
#include "stg18/plant3/sway_object.inc"
};

static NJS_POINT3 plant3_pose_0[84] = {
#include "stg18/plant3/plant3_pose_0.inc"
};

static NJS_POINT3 plant3_pose_9[84] = {
#include "stg18/plant3/plant3_pose_9.inc"
};

static NJS_POINT3 plant3_pose_18[84] = {
#include "stg18/plant3/plant3_pose_18.inc"
};

static NJS_MKEY_P plant3_shape_keys[3] = {
#include "stg18/plant3/plant3_shape_keys.inc"
};

static NJS_MDATA2 plant3_shape_tracks = {
#include "stg18/plant3/plant3_shape_tracks.inc"
};

static NJS_MOTION sway_shape[1] = {
#include "stg18/plant3/sway_shape.inc"
};

static NJS_MDATA2 plant3_motion_tracks = {
#include "stg18/plant3/plant3_motion_tracks.inc"
};

static NJS_MOTION sway_motion = {
#include "stg18/plant3/sway_motion.inc"
};

NJS_TEXLIST *plant3_texlists[2] = {
#include "stg18/plant3/plant3_texlists.inc"
};

/* NOLINTBEGIN(readability-identifier-naming) */
#define plant2_init plant3_init
/* NOLINTEND(readability-identifier-naming) */
#include "stg18/sway_plant_impl.h" /* IWYU pragma: keep -- the code it compiles here */
