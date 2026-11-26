/* FLOWER in Dry Lagoon uses two independently writable models. Each has
 * 171 colored vertices and eight shapes for the same 20-frame sway. */
#include "stg18/flower.h"

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/chunk_polygon.h"
#include "stg18/flower/text.inc"

struct flower_strip3 {
    Sint16 length;
    struct part_uv corners[3];
};

struct flower_strip5 {
    Sint16 length;
    struct part_uv corners[5];
};

struct flower_strip6 {
    Sint16 length;
    struct part_uv corners[6];
};

struct flower_strip_pair {
    struct flower_strip3 triangle;
    struct flower_strip5 strip;
};

/* Each group's header gives its strip count. The aligned halfword at the
 * end of the undergrowth group precedes the stems' texture command. */

static NJS_TEXNAME flower_bright_names[3] = {
#include "stg18/flower/flower_bright_names.inc"
};

static NJS_TEXLIST foliage_bright_texlist = {
#include "stg18/flower/foliage_bright_texlist.inc"
};

static Sint16 flower_bright_polygons[855] = {
#include "stg18/flower/flower_bright_polygons.inc"
};

static Sint32 flower_bright_vertices[687] = {
#include "stg18/flower/flower_bright_vertices.inc"
};

static NJS_CNK_MODEL flower_bright_model = {
#include "stg18/flower/flower_bright_model.inc"
};

static NJS_CNK_OBJECT foliage_bright_object = {
#include "stg18/flower/foliage_bright_object.inc"
};

static NJS_POINT3 flower_bright_pose_0[171] = {
#include "stg18/flower/flower_bright_pose_0.inc"
};

static NJS_POINT3 flower_bright_pose_4[171] = {
#include "stg18/flower/flower_bright_pose_4.inc"
};

static NJS_POINT3 flower_bright_pose_6[171] = {
#include "stg18/flower/flower_bright_pose_6.inc"
};

static NJS_POINT3 flower_bright_pose_10[171] = {
#include "stg18/flower/flower_bright_pose_10.inc"
};

static NJS_POINT3 flower_bright_pose_12[171] = {
#include "stg18/flower/flower_bright_pose_12.inc"
};

static NJS_POINT3 flower_bright_pose_15[171] = {
#include "stg18/flower/flower_bright_pose_15.inc"
};

static NJS_POINT3 flower_bright_pose_17[171] = {
#include "stg18/flower/flower_bright_pose_17.inc"
};

static NJS_POINT3 flower_bright_pose_19[171] = {
#include "stg18/flower/flower_bright_pose_19.inc"
};

static NJS_MKEY_P flower_bright_shape_keys[8] = {
#include "stg18/flower/flower_bright_shape_keys.inc"
};

static NJS_MDATA2 flower_bright_shape_tracks = {
#include "stg18/flower/flower_bright_shape_tracks.inc"
};

static NJS_MOTION foliage_bright_shape = {
#include "stg18/flower/foliage_bright_shape.inc"
};

static NJS_MDATA2 flower_bright_motion_tracks = {
#include "stg18/flower/flower_bright_motion_tracks.inc"
};

static NJS_MOTION foliage_bright_motion = {
#include "stg18/flower/foliage_bright_motion.inc"
};

static NJS_TEXNAME flower_dark_names[3] = {
#include "stg18/flower/flower_dark_names.inc"
};

static NJS_TEXLIST foliage_dark_texlist = {
#include "stg18/flower/foliage_dark_texlist.inc"
};

static Sint16 flower_dark_polygons[855] = {
#include "stg18/flower/flower_dark_polygons.inc"
};

static Sint32 flower_dark_vertices[687] = {
#include "stg18/flower/flower_dark_vertices.inc"
};

static NJS_CNK_MODEL flower_dark_model = {
#include "stg18/flower/flower_dark_model.inc"
};

static NJS_CNK_OBJECT foliage_dark_object = {
#include "stg18/flower/foliage_dark_object.inc"
};

static NJS_POINT3 flower_dark_pose_0[171] = {
#include "stg18/flower/flower_dark_pose_0.inc"
};

static NJS_POINT3 flower_dark_pose_4[171] = {
#include "stg18/flower/flower_dark_pose_4.inc"
};

static NJS_POINT3 flower_dark_pose_6[171] = {
#include "stg18/flower/flower_dark_pose_6.inc"
};

static NJS_POINT3 flower_dark_pose_10[171] = {
#include "stg18/flower/flower_dark_pose_10.inc"
};

static NJS_POINT3 flower_dark_pose_12[171] = {
#include "stg18/flower/flower_dark_pose_12.inc"
};

static NJS_POINT3 flower_dark_pose_15[171] = {
#include "stg18/flower/flower_dark_pose_15.inc"
};

static NJS_POINT3 flower_dark_pose_17[171] = {
#include "stg18/flower/flower_dark_pose_17.inc"
};

static NJS_POINT3 flower_dark_pose_19[171] = {
#include "stg18/flower/flower_dark_pose_19.inc"
};

static NJS_MKEY_P flower_dark_shape_keys[8] = {
#include "stg18/flower/flower_dark_shape_keys.inc"
};

static NJS_MDATA2 flower_dark_shape_tracks = {
#include "stg18/flower/flower_dark_shape_tracks.inc"
};

static NJS_MOTION foliage_dark_shape = {
#include "stg18/flower/foliage_dark_shape.inc"
};

static NJS_MDATA2 flower_dark_motion_tracks = {
#include "stg18/flower/flower_dark_motion_tracks.inc"
};

static NJS_MOTION foliage_dark_motion = {
#include "stg18/flower/foliage_dark_motion.inc"
};

static NJS_CNK_OBJECT *sasa_objects[2] = {
#include "stg18/flower/sasa_objects.inc"
};
NJS_TEXLIST *flower_texlists[3] = {
#include "stg18/flower/flower_texlists.inc"
};
/* NOLINTBEGIN(readability-identifier-naming) */
#define sasa_build flower_build
#define sasa_init flower_init
#define sasa_texlists flower_texlists
/* NOLINTEND(readability-identifier-naming) */
#define SASA_HEIGHT 12.0F
#define SASA_RADIUS 7.0F
#define SASA_START(task) ((task)->unk_3c.l = 0)

#include "stg18/foliage_controller.h" /* IWYU pragma: keep -- the data it defines here */
#include "stg18/foliage_impl.h" /* IWYU pragma: keep -- the code it compiles here */
