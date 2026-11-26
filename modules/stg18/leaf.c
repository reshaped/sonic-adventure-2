/* Broad-leaf foliage and its shape motion. */
#include "stg18/leaf.h"

#include <ninja.h>
#include <sg_xpt.h>

#include "shared/common_object.h"
#include "stg18/leaf/text.inc"

static NJS_TEXNAME leaf_bright_names[1] = {
#include "stg18/leaf/leaf_bright_names.inc"
};

static NJS_TEXLIST foliage_bright_texlist = {
#include "stg18/leaf/foliage_bright_texlist.inc"
};

static Sint16 leaf_bright_polygons[297] = {
#include "stg18/leaf/leaf_bright_polygons.inc"
};

static Sint32 leaf_bright_vertices[243] = {
#include "stg18/leaf/leaf_bright_vertices.inc"
};

static NJS_CNK_MODEL leaf_bright_model = {
#include "stg18/leaf/leaf_bright_model.inc"
};

static NJS_CNK_OBJECT foliage_bright_object = {
#include "stg18/leaf/foliage_bright_object.inc"
};

static NJS_MDATA2 leaf_bright_motion_tracks = {
#include "stg18/leaf/leaf_bright_motion_tracks.inc"
};

static NJS_MOTION foliage_bright_motion = {
#include "stg18/leaf/foliage_bright_motion.inc"
};

static NJS_POINT3 leaf_bright_pose_0[60] = {
#include "stg18/leaf/leaf_bright_pose_0.inc"
};

static NJS_POINT3 leaf_bright_pose_5[60] = {
#include "stg18/leaf/leaf_bright_pose_5.inc"
};

static NJS_POINT3 leaf_bright_pose_11[60] = {
#include "stg18/leaf/leaf_bright_pose_11.inc"
};

static NJS_POINT3 leaf_bright_pose_15[60] = {
#include "stg18/leaf/leaf_bright_pose_15.inc"
};

static NJS_POINT3 leaf_bright_pose_19[60] = {
#include "stg18/leaf/leaf_bright_pose_19.inc"
};

static NJS_MKEY_P leaf_bright_shape_keys[5] = {
#include "stg18/leaf/leaf_bright_shape_keys.inc"
};

static NJS_MDATA2 leaf_bright_shape_tracks = {
#include "stg18/leaf/leaf_bright_shape_tracks.inc"
};

static NJS_MOTION foliage_bright_shape = {
#include "stg18/leaf/foliage_bright_shape.inc"
};

static NJS_TEXNAME leaf_dark_names[1] = {
#include "stg18/leaf/leaf_dark_names.inc"
};

static NJS_TEXLIST foliage_dark_texlist = {
#include "stg18/leaf/foliage_dark_texlist.inc"
};

static Sint16 leaf_dark_polygons[297] = {
#include "stg18/leaf/leaf_dark_polygons.inc"
};

static Sint32 leaf_dark_vertices[243] = {
#include "stg18/leaf/leaf_dark_vertices.inc"
};

static NJS_CNK_MODEL leaf_dark_model = {
#include "stg18/leaf/leaf_dark_model.inc"
};

static NJS_CNK_OBJECT foliage_dark_object = {
#include "stg18/leaf/foliage_dark_object.inc"
};

static NJS_MDATA2 leaf_dark_motion_tracks = {
#include "stg18/leaf/leaf_dark_motion_tracks.inc"
};

static NJS_MOTION foliage_dark_motion = {
#include "stg18/leaf/foliage_dark_motion.inc"
};

static NJS_POINT3 leaf_dark_pose_0[60] = {
#include "stg18/leaf/leaf_dark_pose_0.inc"
};

static NJS_POINT3 leaf_dark_pose_5[60] = {
#include "stg18/leaf/leaf_dark_pose_5.inc"
};

static NJS_POINT3 leaf_dark_pose_11[60] = {
#include "stg18/leaf/leaf_dark_pose_11.inc"
};

static NJS_POINT3 leaf_dark_pose_15[60] = {
#include "stg18/leaf/leaf_dark_pose_15.inc"
};

static NJS_POINT3 leaf_dark_pose_19[60] = {
#include "stg18/leaf/leaf_dark_pose_19.inc"
};

static NJS_MKEY_P leaf_dark_shape_keys[5] = {
#include "stg18/leaf/leaf_dark_shape_keys.inc"
};

static NJS_MDATA2 leaf_dark_shape_tracks = {
#include "stg18/leaf/leaf_dark_shape_tracks.inc"
};

static NJS_MOTION foliage_dark_shape = {
#include "stg18/leaf/foliage_dark_shape.inc"
};

static NJS_CNK_OBJECT *sasa_objects[2] = {
#include "stg18/leaf/sasa_objects.inc"
};
NJS_TEXLIST *leaf_texlists[3] = {
#include "stg18/leaf/leaf_texlists.inc"
};
static NJS_MOTION *sasa_motions[2] = {
#include "stg18/leaf/sasa_motions.inc"
};
static NJS_MOTION *sasa_shapes[2] = {
#include "stg18/leaf/sasa_shapes.inc"
};
static char *sasa_kind_names[2] = {
#include "stg18/leaf/sasa_kind_names.inc"
};
static Float sasa_height = 6.0F;
static Float sasa_radius = 7.0F;
static Sint32 sasa_volume = 50;
static Bool sasa_built = FALSE;
static struct model_build sasa_builds[2] = {
#include "stg18/leaf/sasa_builds.inc"
};

/* NOLINTBEGIN(readability-identifier-naming) */
#define sasa_build leaf_build
#define sasa_init leaf_init
#define sasa_texlists leaf_texlists
/* NOLINTEND(readability-identifier-naming) */
#define SASA_START(task) ((task)->unk_3c.l = 0)

#include "stg18/foliage_impl.h" /* IWYU pragma: keep -- the code it compiles here */
