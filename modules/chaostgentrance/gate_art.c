/* the eight lanes' starting gates as one model, and its motion. the motion
 * lifts every gate together over its eight frames */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgentrance/fixture_art.h"

static Sint16 gate_lane_8_polygons[25] = {
#include "chaostgentrance/gate_art/gate_lane_8_polygons.inc"
};

static Sint32 gate_lane_8_vertices[19] = {
#include "chaostgentrance/gate_art/gate_lane_8_vertices.inc"
};

static NJS_CNK_MODEL gate_lane_8_geometry = {
#include "chaostgentrance/gate_art/gate_lane_8_geometry.inc"
};

static NJS_CNK_OBJECT gate_lane_8 = {
#include "chaostgentrance/gate_art/gate_lane_8.inc"
};

static Sint16 gate_lane_7_polygons[25] = {
#include "chaostgentrance/gate_art/gate_lane_7_polygons.inc"
};

static Sint32 gate_lane_7_vertices[19] = {
#include "chaostgentrance/gate_art/gate_lane_7_vertices.inc"
};

static NJS_CNK_MODEL gate_lane_7_geometry = {
#include "chaostgentrance/gate_art/gate_lane_7_geometry.inc"
};

static NJS_CNK_OBJECT gate_lane_7 = {
#include "chaostgentrance/gate_art/gate_lane_7.inc"
};

static Sint16 gate_lane_6_polygons[25] = {
#include "chaostgentrance/gate_art/gate_lane_6_polygons.inc"
};

static Sint32 gate_lane_6_vertices[19] = {
#include "chaostgentrance/gate_art/gate_lane_6_vertices.inc"
};

static NJS_CNK_MODEL gate_lane_6_geometry = {
#include "chaostgentrance/gate_art/gate_lane_6_geometry.inc"
};

static NJS_CNK_OBJECT gate_lane_6 = {
#include "chaostgentrance/gate_art/gate_lane_6.inc"
};

static Sint16 gate_lane_5_polygons[25] = {
#include "chaostgentrance/gate_art/gate_lane_5_polygons.inc"
};

static Sint32 gate_lane_5_vertices[19] = {
#include "chaostgentrance/gate_art/gate_lane_5_vertices.inc"
};

static NJS_CNK_MODEL gate_lane_5_geometry = {
#include "chaostgentrance/gate_art/gate_lane_5_geometry.inc"
};

static NJS_CNK_OBJECT gate_lane_5 = {
#include "chaostgentrance/gate_art/gate_lane_5.inc"
};

static Sint16 gate_lane_4_polygons[25] = {
#include "chaostgentrance/gate_art/gate_lane_4_polygons.inc"
};

static Sint32 gate_lane_4_vertices[19] = {
#include "chaostgentrance/gate_art/gate_lane_4_vertices.inc"
};

static NJS_CNK_MODEL gate_lane_4_geometry = {
#include "chaostgentrance/gate_art/gate_lane_4_geometry.inc"
};

static NJS_CNK_OBJECT gate_lane_4 = {
#include "chaostgentrance/gate_art/gate_lane_4.inc"
};

static Sint16 gate_lane_3_polygons[25] = {
#include "chaostgentrance/gate_art/gate_lane_3_polygons.inc"
};

static Sint32 gate_lane_3_vertices[19] = {
#include "chaostgentrance/gate_art/gate_lane_3_vertices.inc"
};

static NJS_CNK_MODEL gate_lane_3_geometry = {
#include "chaostgentrance/gate_art/gate_lane_3_geometry.inc"
};

static NJS_CNK_OBJECT gate_lane_3 = {
#include "chaostgentrance/gate_art/gate_lane_3.inc"
};

static Sint16 gate_lane_2_polygons[25] = {
#include "chaostgentrance/gate_art/gate_lane_2_polygons.inc"
};

static Sint32 gate_lane_2_vertices[19] = {
#include "chaostgentrance/gate_art/gate_lane_2_vertices.inc"
};

static NJS_CNK_MODEL gate_lane_2_geometry = {
#include "chaostgentrance/gate_art/gate_lane_2_geometry.inc"
};

static NJS_CNK_OBJECT gate_lane_2 = {
#include "chaostgentrance/gate_art/gate_lane_2.inc"
};

static Sint16 gate_lane_1_polygons[25] = {
#include "chaostgentrance/gate_art/gate_lane_1_polygons.inc"
};

static Sint32 gate_lane_1_vertices[19] = {
#include "chaostgentrance/gate_art/gate_lane_1_vertices.inc"
};

static NJS_CNK_MODEL gate_lane_1_geometry = {
#include "chaostgentrance/gate_art/gate_lane_1_geometry.inc"
};

static NJS_CNK_OBJECT gate_lane_1 = {
#include "chaostgentrance/gate_art/gate_lane_1.inc"
};

static Sint16 gate_frame_polygons[369] = {
#include "chaostgentrance/gate_art/gate_frame_polygons.inc"
};

static Sint32 gate_frame_vertices[291] = {
#include "chaostgentrance/gate_art/gate_frame_vertices.inc"
};

static NJS_CNK_MODEL gate_frame_geometry = {
#include "chaostgentrance/gate_art/gate_frame_geometry.inc"
};

NJS_CNK_OBJECT entrance_gate_model = {
#include "chaostgentrance/gate_art/entrance_gate_model.inc"
};

static NJS_MKEY_F gate_lane_1_lift[8] = {
#include "chaostgentrance/gate_art/gate_lane_1_lift.inc"
};

static NJS_MKEY_F gate_lane_2_lift[8] = {
#include "chaostgentrance/gate_art/gate_lane_2_lift.inc"
};

static NJS_MKEY_F gate_lane_3_lift[8] = {
#include "chaostgentrance/gate_art/gate_lane_3_lift.inc"
};

static NJS_MKEY_F gate_lane_4_lift[8] = {
#include "chaostgentrance/gate_art/gate_lane_4_lift.inc"
};

static NJS_MKEY_F gate_lane_5_lift[8] = {
#include "chaostgentrance/gate_art/gate_lane_5_lift.inc"
};

static NJS_MKEY_F gate_lane_6_lift[8] = {
#include "chaostgentrance/gate_art/gate_lane_6_lift.inc"
};

static NJS_MKEY_F gate_lane_7_lift[8] = {
#include "chaostgentrance/gate_art/gate_lane_7_lift.inc"
};

static NJS_MKEY_F gate_lane_8_lift[8] = {
#include "chaostgentrance/gate_art/gate_lane_8_lift.inc"
};

static NJS_MDATA2 gate_tracks[9] = {
#include "chaostgentrance/gate_art/gate_tracks.inc"
};

NJS_MOTION entrance_gate_motion = {
#include "chaostgentrance/gate_art/entrance_gate_motion.inc"
};
