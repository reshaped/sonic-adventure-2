/* the four Chaos Drives' models, seven meshes in each, with the same
 * vertices in every drive. chaos_drive_objects, which the pickups and the
 * garden draw, holds their roots */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/chaos_drive.h"

static Sint16 chaos_drive_green_caps_polygons[31] = {
#include "object/chaos_drive_models/chaos_drive_green_caps_polygons.inc"
};
static Sint32 chaos_drive_green_caps_vertices[75] = {
#include "object/chaos_drive_models/chaos_drive_green_caps_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_green_caps_model = {
#include "object/chaos_drive_models/chaos_drive_green_caps_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_green_caps = {
#include "object/chaos_drive_models/chaos_drive_green_caps.inc"
};

static Sint16 chaos_drive_green_facet_right_polygons[17] = {
#include "object/chaos_drive_models/chaos_drive_green_facet_right_polygons.inc"
};
static Sint32 chaos_drive_green_facet_right_vertices[27] = {
#include "object/chaos_drive_models/chaos_drive_green_facet_right_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_green_facet_right_model = {
#include "object/chaos_drive_models/chaos_drive_green_facet_right_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_green_facet_right = {
#include "object/chaos_drive_models/chaos_drive_green_facet_right.inc"
};

static Sint16 chaos_drive_green_facet_left_polygons[17] = {
#include "object/chaos_drive_models/chaos_drive_green_facet_left_polygons.inc"
};
static Sint32 chaos_drive_green_facet_left_vertices[27] = {
#include "object/chaos_drive_models/chaos_drive_green_facet_left_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_green_facet_left_model = {
#include "object/chaos_drive_models/chaos_drive_green_facet_left_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_green_facet_left = {
#include "object/chaos_drive_models/chaos_drive_green_facet_left.inc"
};

static Sint16 chaos_drive_green_facet_front_polygons[17] = {
#include "object/chaos_drive_models/chaos_drive_green_facet_front_polygons.inc"
};
static Sint32 chaos_drive_green_facet_front_vertices[27] = {
#include "object/chaos_drive_models/chaos_drive_green_facet_front_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_green_facet_front_model = {
#include "object/chaos_drive_models/chaos_drive_green_facet_front_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_green_facet_front = {
#include "object/chaos_drive_models/chaos_drive_green_facet_front.inc"
};

static Sint16 chaos_drive_green_core_x_polygons[29] = {
#include "object/chaos_drive_models/chaos_drive_green_core_x_polygons.inc"
};
static Sint32 chaos_drive_green_core_x_vertices[39] = {
#include "object/chaos_drive_models/chaos_drive_green_core_x_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_green_core_x_model = {
#include "object/chaos_drive_models/chaos_drive_green_core_x_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_green_core_x = {
#include "object/chaos_drive_models/chaos_drive_green_core_x.inc"
};

static Sint16 chaos_drive_green_core_z_polygons[29] = {
#include "object/chaos_drive_models/chaos_drive_green_core_z_polygons.inc"
};
static Sint32 chaos_drive_green_core_z_vertices[39] = {
#include "object/chaos_drive_models/chaos_drive_green_core_z_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_green_core_z_model = {
#include "object/chaos_drive_models/chaos_drive_green_core_z_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_green_core_z = {
#include "object/chaos_drive_models/chaos_drive_green_core_z.inc"
};

static Sint16 chaos_drive_green_body_polygons[113] = {
#include "object/chaos_drive_models/chaos_drive_green_body_polygons.inc"
};
static Sint32 chaos_drive_green_body_vertices[75] = {
#include "object/chaos_drive_models/chaos_drive_green_body_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_green_body_model = {
#include "object/chaos_drive_models/chaos_drive_green_body_model.inc"
};
NJS_CNK_OBJECT chaos_drive_green_object = {
#include "object/chaos_drive_models/chaos_drive_green_object.inc"
};

static Sint16 chaos_drive_purple_caps_polygons[31] = {
#include "object/chaos_drive_models/chaos_drive_purple_caps_polygons.inc"
};
static Sint32 chaos_drive_purple_caps_vertices[75] = {
#include "object/chaos_drive_models/chaos_drive_purple_caps_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_purple_caps_model = {
#include "object/chaos_drive_models/chaos_drive_purple_caps_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_purple_caps = {
#include "object/chaos_drive_models/chaos_drive_purple_caps.inc"
};

static Sint16 chaos_drive_purple_facet_right_polygons[17] = {
#include "object/chaos_drive_models/chaos_drive_purple_facet_right_polygons.inc"
};
static Sint32 chaos_drive_purple_facet_right_vertices[27] = {
#include "object/chaos_drive_models/chaos_drive_purple_facet_right_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_purple_facet_right_model = {
#include "object/chaos_drive_models/chaos_drive_purple_facet_right_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_purple_facet_right = {
#include "object/chaos_drive_models/chaos_drive_purple_facet_right.inc"
};

static Sint16 chaos_drive_purple_facet_left_polygons[17] = {
#include "object/chaos_drive_models/chaos_drive_purple_facet_left_polygons.inc"
};
static Sint32 chaos_drive_purple_facet_left_vertices[27] = {
#include "object/chaos_drive_models/chaos_drive_purple_facet_left_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_purple_facet_left_model = {
#include "object/chaos_drive_models/chaos_drive_purple_facet_left_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_purple_facet_left = {
#include "object/chaos_drive_models/chaos_drive_purple_facet_left.inc"
};

static Sint16 chaos_drive_purple_facet_front_polygons[17] = {
#include "object/chaos_drive_models/chaos_drive_purple_facet_front_polygons.inc"
};
static Sint32 chaos_drive_purple_facet_front_vertices[27] = {
#include "object/chaos_drive_models/chaos_drive_purple_facet_front_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_purple_facet_front_model = {
#include "object/chaos_drive_models/chaos_drive_purple_facet_front_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_purple_facet_front = {
#include "object/chaos_drive_models/chaos_drive_purple_facet_front.inc"
};

static Sint16 chaos_drive_purple_core_x_polygons[29] = {
#include "object/chaos_drive_models/chaos_drive_purple_core_x_polygons.inc"
};
static Sint32 chaos_drive_purple_core_x_vertices[39] = {
#include "object/chaos_drive_models/chaos_drive_purple_core_x_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_purple_core_x_model = {
#include "object/chaos_drive_models/chaos_drive_purple_core_x_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_purple_core_x = {
#include "object/chaos_drive_models/chaos_drive_purple_core_x.inc"
};

static Sint16 chaos_drive_purple_core_z_polygons[29] = {
#include "object/chaos_drive_models/chaos_drive_purple_core_z_polygons.inc"
};
static Sint32 chaos_drive_purple_core_z_vertices[39] = {
#include "object/chaos_drive_models/chaos_drive_purple_core_z_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_purple_core_z_model = {
#include "object/chaos_drive_models/chaos_drive_purple_core_z_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_purple_core_z = {
#include "object/chaos_drive_models/chaos_drive_purple_core_z.inc"
};

static Sint16 chaos_drive_purple_body_polygons[113] = {
#include "object/chaos_drive_models/chaos_drive_purple_body_polygons.inc"
};
static Sint32 chaos_drive_purple_body_vertices[75] = {
#include "object/chaos_drive_models/chaos_drive_purple_body_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_purple_body_model = {
#include "object/chaos_drive_models/chaos_drive_purple_body_model.inc"
};
NJS_CNK_OBJECT chaos_drive_purple_object = {
#include "object/chaos_drive_models/chaos_drive_purple_object.inc"
};

static Sint16 chaos_drive_red_caps_polygons[31] = {
#include "object/chaos_drive_models/chaos_drive_red_caps_polygons.inc"
};
static Sint32 chaos_drive_red_caps_vertices[75] = {
#include "object/chaos_drive_models/chaos_drive_red_caps_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_red_caps_model = {
#include "object/chaos_drive_models/chaos_drive_red_caps_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_red_caps = {
#include "object/chaos_drive_models/chaos_drive_red_caps.inc"
};

static Sint16 chaos_drive_red_facet_right_polygons[17] = {
#include "object/chaos_drive_models/chaos_drive_red_facet_right_polygons.inc"
};
static Sint32 chaos_drive_red_facet_right_vertices[27] = {
#include "object/chaos_drive_models/chaos_drive_red_facet_right_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_red_facet_right_model = {
#include "object/chaos_drive_models/chaos_drive_red_facet_right_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_red_facet_right = {
#include "object/chaos_drive_models/chaos_drive_red_facet_right.inc"
};

static Sint16 chaos_drive_red_facet_left_polygons[17] = {
#include "object/chaos_drive_models/chaos_drive_red_facet_left_polygons.inc"
};
static Sint32 chaos_drive_red_facet_left_vertices[27] = {
#include "object/chaos_drive_models/chaos_drive_red_facet_left_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_red_facet_left_model = {
#include "object/chaos_drive_models/chaos_drive_red_facet_left_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_red_facet_left = {
#include "object/chaos_drive_models/chaos_drive_red_facet_left.inc"
};

static Sint16 chaos_drive_red_facet_front_polygons[17] = {
#include "object/chaos_drive_models/chaos_drive_red_facet_front_polygons.inc"
};
static Sint32 chaos_drive_red_facet_front_vertices[27] = {
#include "object/chaos_drive_models/chaos_drive_red_facet_front_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_red_facet_front_model = {
#include "object/chaos_drive_models/chaos_drive_red_facet_front_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_red_facet_front = {
#include "object/chaos_drive_models/chaos_drive_red_facet_front.inc"
};

static Sint16 chaos_drive_red_core_x_polygons[29] = {
#include "object/chaos_drive_models/chaos_drive_red_core_x_polygons.inc"
};
static Sint32 chaos_drive_red_core_x_vertices[39] = {
#include "object/chaos_drive_models/chaos_drive_red_core_x_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_red_core_x_model = {
#include "object/chaos_drive_models/chaos_drive_red_core_x_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_red_core_x = {
#include "object/chaos_drive_models/chaos_drive_red_core_x.inc"
};

static Sint16 chaos_drive_red_core_z_polygons[29] = {
#include "object/chaos_drive_models/chaos_drive_red_core_z_polygons.inc"
};
static Sint32 chaos_drive_red_core_z_vertices[39] = {
#include "object/chaos_drive_models/chaos_drive_red_core_z_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_red_core_z_model = {
#include "object/chaos_drive_models/chaos_drive_red_core_z_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_red_core_z = {
#include "object/chaos_drive_models/chaos_drive_red_core_z.inc"
};

static Sint16 chaos_drive_red_body_polygons[113] = {
#include "object/chaos_drive_models/chaos_drive_red_body_polygons.inc"
};
static Sint32 chaos_drive_red_body_vertices[75] = {
#include "object/chaos_drive_models/chaos_drive_red_body_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_red_body_model = {
#include "object/chaos_drive_models/chaos_drive_red_body_model.inc"
};
NJS_CNK_OBJECT chaos_drive_red_object = {
#include "object/chaos_drive_models/chaos_drive_red_object.inc"
};

static Sint16 chaos_drive_yellow_caps_polygons[31] = {
#include "object/chaos_drive_models/chaos_drive_yellow_caps_polygons.inc"
};
static Sint32 chaos_drive_yellow_caps_vertices[75] = {
#include "object/chaos_drive_models/chaos_drive_yellow_caps_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_yellow_caps_model = {
#include "object/chaos_drive_models/chaos_drive_yellow_caps_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_yellow_caps = {
#include "object/chaos_drive_models/chaos_drive_yellow_caps.inc"
};

static Sint16 chaos_drive_yellow_facet_right_polygons[17] = {
#include "object/chaos_drive_models/chaos_drive_yellow_facet_right_polygons.inc"
};
static Sint32 chaos_drive_yellow_facet_right_vertices[27] = {
#include "object/chaos_drive_models/chaos_drive_yellow_facet_right_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_yellow_facet_right_model = {
#include "object/chaos_drive_models/chaos_drive_yellow_facet_right_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_yellow_facet_right = {
#include "object/chaos_drive_models/chaos_drive_yellow_facet_right.inc"
};

static Sint16 chaos_drive_yellow_facet_left_polygons[17] = {
#include "object/chaos_drive_models/chaos_drive_yellow_facet_left_polygons.inc"
};
static Sint32 chaos_drive_yellow_facet_left_vertices[27] = {
#include "object/chaos_drive_models/chaos_drive_yellow_facet_left_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_yellow_facet_left_model = {
#include "object/chaos_drive_models/chaos_drive_yellow_facet_left_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_yellow_facet_left = {
#include "object/chaos_drive_models/chaos_drive_yellow_facet_left.inc"
};

static Sint16 chaos_drive_yellow_facet_front_polygons[17] = {
#include "object/chaos_drive_models/chaos_drive_yellow_facet_front_polygons.inc"
};
static Sint32 chaos_drive_yellow_facet_front_vertices[27] = {
#include "object/chaos_drive_models/chaos_drive_yellow_facet_front_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_yellow_facet_front_model = {
#include "object/chaos_drive_models/chaos_drive_yellow_facet_front_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_yellow_facet_front = {
#include "object/chaos_drive_models/chaos_drive_yellow_facet_front.inc"
};

static Sint16 chaos_drive_yellow_core_x_polygons[29] = {
#include "object/chaos_drive_models/chaos_drive_yellow_core_x_polygons.inc"
};
static Sint32 chaos_drive_yellow_core_x_vertices[39] = {
#include "object/chaos_drive_models/chaos_drive_yellow_core_x_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_yellow_core_x_model = {
#include "object/chaos_drive_models/chaos_drive_yellow_core_x_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_yellow_core_x = {
#include "object/chaos_drive_models/chaos_drive_yellow_core_x.inc"
};

static Sint16 chaos_drive_yellow_core_z_polygons[29] = {
#include "object/chaos_drive_models/chaos_drive_yellow_core_z_polygons.inc"
};
static Sint32 chaos_drive_yellow_core_z_vertices[39] = {
#include "object/chaos_drive_models/chaos_drive_yellow_core_z_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_yellow_core_z_model = {
#include "object/chaos_drive_models/chaos_drive_yellow_core_z_model.inc"
};
static NJS_CNK_OBJECT chaos_drive_yellow_core_z = {
#include "object/chaos_drive_models/chaos_drive_yellow_core_z.inc"
};

static Sint16 chaos_drive_yellow_body_polygons[113] = {
#include "object/chaos_drive_models/chaos_drive_yellow_body_polygons.inc"
};
static Sint32 chaos_drive_yellow_body_vertices[75] = {
#include "object/chaos_drive_models/chaos_drive_yellow_body_vertices.inc"
};
static NJS_CNK_MODEL chaos_drive_yellow_body_model = {
#include "object/chaos_drive_models/chaos_drive_yellow_body_model.inc"
};
NJS_CNK_OBJECT chaos_drive_yellow_object = {
#include "object/chaos_drive_models/chaos_drive_yellow_object.inc"
};
