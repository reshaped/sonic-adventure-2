/* SASA in Dry Lagoon. The bright and dark palmettos each have a writable mesh
 * and seven 88-point shapes for their 20-frame sway. */
#include "stg18/sasa.h"

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/chunk_polygon.h"
#include "stg18/sasa/text.inc"

struct sasa_strip_pair {
    Sint16 first_length;
    struct part_uv first[7];
    Sint16 second_length;
    struct part_uv second[5];
};

struct sasa_polygon_list {
    Sint16 material[6];
    struct part_texture texture;
    struct part_strip_header header;
    struct sasa_strip_pair strips[11];
    Uint16 strip_pad;
    Uint16 end;
};

static NJS_TEXNAME sasa_bright_names[1] = {
#include "stg18/sasa/sasa_bright_names.inc"
};

static NJS_TEXLIST foliage_bright_texlist = {
#include "stg18/sasa/foliage_bright_texlist.inc"
};

static struct sasa_polygon_list sasa_bright_polygons = {
#include "stg18/sasa/sasa_bright_polygons.inc"
};

static Sint32 sasa_bright_vertices[355] = {
#include "stg18/sasa/sasa_bright_vertices.inc"
};

static NJS_CNK_MODEL sasa_bright_model = {
#include "stg18/sasa/sasa_bright_model.inc"
};

static NJS_CNK_OBJECT foliage_bright_object = {
#include "stg18/sasa/foliage_bright_object.inc"
};

static NJS_MDATA2 sasa_bright_motion_tracks = {
#include "stg18/sasa/sasa_bright_motion_tracks.inc"
};

static NJS_MOTION foliage_bright_motion = {
#include "stg18/sasa/foliage_bright_motion.inc"
};

static NJS_POINT3 sasa_bright_pose_0[88] = {
#include "stg18/sasa/sasa_bright_pose_0.inc"
};

static NJS_POINT3 sasa_bright_pose_2[88] = {
#include "stg18/sasa/sasa_bright_pose_2.inc"
};

static NJS_POINT3 sasa_bright_pose_5[88] = {
#include "stg18/sasa/sasa_bright_pose_5.inc"
};

static NJS_POINT3 sasa_bright_pose_8[88] = {
#include "stg18/sasa/sasa_bright_pose_8.inc"
};

static NJS_POINT3 sasa_bright_pose_11[88] = {
#include "stg18/sasa/sasa_bright_pose_11.inc"
};

static NJS_POINT3 sasa_bright_pose_15[88] = {
#include "stg18/sasa/sasa_bright_pose_15.inc"
};

static NJS_POINT3 sasa_bright_pose_19[88] = {
#include "stg18/sasa/sasa_bright_pose_19.inc"
};

static NJS_MKEY_P sasa_bright_shape_keys[7] = {
#include "stg18/sasa/sasa_bright_shape_keys.inc"
};

static NJS_MDATA2 sasa_bright_shape_tracks = {
#include "stg18/sasa/sasa_bright_shape_tracks.inc"
};

static NJS_MOTION foliage_bright_shape = {
#include "stg18/sasa/foliage_bright_shape.inc"
};

static NJS_TEXNAME sasa_dark_names[1] = {
#include "stg18/sasa/sasa_dark_names.inc"
};

static NJS_TEXLIST foliage_dark_texlist = {
#include "stg18/sasa/foliage_dark_texlist.inc"
};

static struct sasa_polygon_list sasa_dark_polygons = {
#include "stg18/sasa/sasa_dark_polygons.inc"
};

static Sint32 sasa_dark_vertices[355] = {
#include "stg18/sasa/sasa_dark_vertices.inc"
};

static NJS_CNK_MODEL sasa_dark_model = {
#include "stg18/sasa/sasa_dark_model.inc"
};

static NJS_CNK_OBJECT foliage_dark_object = {
#include "stg18/sasa/foliage_dark_object.inc"
};

static NJS_MDATA2 sasa_dark_motion_tracks = {
#include "stg18/sasa/sasa_dark_motion_tracks.inc"
};

static NJS_MOTION foliage_dark_motion = {
#include "stg18/sasa/foliage_dark_motion.inc"
};

static NJS_POINT3 sasa_dark_pose_0[88] = {
#include "stg18/sasa/sasa_dark_pose_0.inc"
};

static NJS_POINT3 sasa_dark_pose_2[88] = {
#include "stg18/sasa/sasa_dark_pose_2.inc"
};

static NJS_POINT3 sasa_dark_pose_5[88] = {
#include "stg18/sasa/sasa_dark_pose_5.inc"
};

static NJS_POINT3 sasa_dark_pose_8[88] = {
#include "stg18/sasa/sasa_dark_pose_8.inc"
};

static NJS_POINT3 sasa_dark_pose_11[88] = {
#include "stg18/sasa/sasa_dark_pose_11.inc"
};

static NJS_POINT3 sasa_dark_pose_15[88] = {
#include "stg18/sasa/sasa_dark_pose_15.inc"
};

static NJS_POINT3 sasa_dark_pose_19[88] = {
#include "stg18/sasa/sasa_dark_pose_19.inc"
};

static NJS_MKEY_P sasa_dark_shape_keys[7] = {
#include "stg18/sasa/sasa_dark_shape_keys.inc"
};

static NJS_MDATA2 sasa_dark_shape_tracks = {
#include "stg18/sasa/sasa_dark_shape_tracks.inc"
};

static NJS_MOTION foliage_dark_shape = {
#include "stg18/sasa/foliage_dark_shape.inc"
};

static NJS_CNK_OBJECT *sasa_objects[2] = {
#include "stg18/sasa/sasa_objects.inc"
};
NJS_TEXLIST *sasa_texlists[3] = {
#include "stg18/sasa/sasa_texlists.inc"
};
#include "stg18/foliage_controller.h" /* IWYU pragma: keep -- the data it defines here */
#include "stg18/foliage_impl.h" /* IWYU pragma: keep -- the code it compiles here */
