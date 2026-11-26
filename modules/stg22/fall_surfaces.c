/* the objects of Crazy Gadget's kill areas, each a flat quad. they stand in
 * the image's order. each name's number is its place in crazy_gadget.c's
 * kill area list */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg22/fall_surfaces.h"

static NJS_MATERIAL kill_1_material = {
#include "stg22/fall_surfaces/kill_1_material.inc"
};
static Sint16 kill_1_strips[5] = {
#include "stg22/fall_surfaces/kill_1_strips.inc"
};
static NJS_MESHSET kill_1_meshset = {
#include "stg22/fall_surfaces/kill_1_meshset.inc"
};
static NJS_POINT3 kill_1_points[4] = {
#include "stg22/fall_surfaces/kill_1_points.inc"
};
static NJS_VECTOR kill_1_normals[4] = {
#include "stg22/fall_surfaces/kill_1_normals.inc"
};
static NJS_MODEL kill_1_model = {
#include "stg22/fall_surfaces/kill_1_model.inc"
};
NJS_OBJECT gadget_kill_1 = {
#include "stg22/fall_surfaces/gadget_kill_1.inc"
};

static NJS_MATERIAL kill_5_material = {
#include "stg22/fall_surfaces/kill_5_material.inc"
};
static Sint16 kill_5_strips[5] = {
#include "stg22/fall_surfaces/kill_5_strips.inc"
};
static NJS_MESHSET kill_5_meshset = {
#include "stg22/fall_surfaces/kill_5_meshset.inc"
};
static NJS_POINT3 kill_5_points[4] = {
#include "stg22/fall_surfaces/kill_5_points.inc"
};
static NJS_VECTOR kill_5_normals[4] = {
#include "stg22/fall_surfaces/kill_5_normals.inc"
};
static NJS_MODEL kill_5_model = {
#include "stg22/fall_surfaces/kill_5_model.inc"
};
NJS_OBJECT gadget_kill_5 = {
#include "stg22/fall_surfaces/gadget_kill_5.inc"
};

static NJS_MATERIAL kill_0_material = {
#include "stg22/fall_surfaces/kill_0_material.inc"
};
static Sint16 kill_0_strips[5] = {
#include "stg22/fall_surfaces/kill_0_strips.inc"
};
static NJS_MESHSET kill_0_meshset = {
#include "stg22/fall_surfaces/kill_0_meshset.inc"
};
static NJS_POINT3 kill_0_points[4] = {
#include "stg22/fall_surfaces/kill_0_points.inc"
};
static NJS_VECTOR kill_0_normals[4] = {
#include "stg22/fall_surfaces/kill_0_normals.inc"
};
static NJS_MODEL kill_0_model = {
#include "stg22/fall_surfaces/kill_0_model.inc"
};
NJS_OBJECT gadget_kill_0 = {
#include "stg22/fall_surfaces/gadget_kill_0.inc"
};

static NJS_MATERIAL kill_2_material = {
#include "stg22/fall_surfaces/kill_2_material.inc"
};
static Sint16 kill_2_strips[5] = {
#include "stg22/fall_surfaces/kill_2_strips.inc"
};
static NJS_MESHSET kill_2_meshset = {
#include "stg22/fall_surfaces/kill_2_meshset.inc"
};
static NJS_POINT3 kill_2_points[4] = {
#include "stg22/fall_surfaces/kill_2_points.inc"
};
static NJS_VECTOR kill_2_normals[4] = {
#include "stg22/fall_surfaces/kill_2_normals.inc"
};
static NJS_MODEL kill_2_model = {
#include "stg22/fall_surfaces/kill_2_model.inc"
};
NJS_OBJECT gadget_kill_2 = {
#include "stg22/fall_surfaces/gadget_kill_2.inc"
};

static NJS_MATERIAL kill_6_material = {
#include "stg22/fall_surfaces/kill_6_material.inc"
};
static Sint16 kill_6_strips[5] = {
#include "stg22/fall_surfaces/kill_6_strips.inc"
};
static NJS_MESHSET kill_6_meshset = {
#include "stg22/fall_surfaces/kill_6_meshset.inc"
};
static NJS_POINT3 kill_6_points[4] = {
#include "stg22/fall_surfaces/kill_6_points.inc"
};
static NJS_VECTOR kill_6_normals[4] = {
#include "stg22/fall_surfaces/kill_6_normals.inc"
};
static NJS_MODEL kill_6_model = {
#include "stg22/fall_surfaces/kill_6_model.inc"
};
NJS_OBJECT gadget_kill_6 = {
#include "stg22/fall_surfaces/gadget_kill_6.inc"
};

static NJS_MATERIAL kill_7_material = {
#include "stg22/fall_surfaces/kill_7_material.inc"
};
static Sint16 kill_7_strips[5] = {
#include "stg22/fall_surfaces/kill_7_strips.inc"
};
static NJS_MESHSET kill_7_meshset = {
#include "stg22/fall_surfaces/kill_7_meshset.inc"
};
static NJS_POINT3 kill_7_points[4] = {
#include "stg22/fall_surfaces/kill_7_points.inc"
};
static NJS_VECTOR kill_7_normals[4] = {
#include "stg22/fall_surfaces/kill_7_normals.inc"
};
static NJS_MODEL kill_7_model = {
#include "stg22/fall_surfaces/kill_7_model.inc"
};
NJS_OBJECT gadget_kill_7 = {
#include "stg22/fall_surfaces/gadget_kill_7.inc"
};

static NJS_MATERIAL kill_4_material = {
#include "stg22/fall_surfaces/kill_4_material.inc"
};
static Sint16 kill_4_strips[5] = {
#include "stg22/fall_surfaces/kill_4_strips.inc"
};
static NJS_MESHSET kill_4_meshset = {
#include "stg22/fall_surfaces/kill_4_meshset.inc"
};
static NJS_POINT3 kill_4_points[4] = {
#include "stg22/fall_surfaces/kill_4_points.inc"
};
static NJS_VECTOR kill_4_normals[4] = {
#include "stg22/fall_surfaces/kill_4_normals.inc"
};
static NJS_MODEL kill_4_model = {
#include "stg22/fall_surfaces/kill_4_model.inc"
};
NJS_OBJECT gadget_kill_4 = {
#include "stg22/fall_surfaces/gadget_kill_4.inc"
};

static NJS_MATERIAL kill_3_material = {
#include "stg22/fall_surfaces/kill_3_material.inc"
};
static Sint16 kill_3_strips[5] = {
#include "stg22/fall_surfaces/kill_3_strips.inc"
};
static NJS_MESHSET kill_3_meshset = {
#include "stg22/fall_surfaces/kill_3_meshset.inc"
};
static NJS_POINT3 kill_3_points[4] = {
#include "stg22/fall_surfaces/kill_3_points.inc"
};
static NJS_VECTOR kill_3_normals[4] = {
#include "stg22/fall_surfaces/kill_3_normals.inc"
};
static NJS_MODEL kill_3_model = {
#include "stg22/fall_surfaces/kill_3_model.inc"
};
NJS_OBJECT gadget_kill_3 = {
#include "stg22/fall_surfaces/gadget_kill_3.inc"
};

static NJS_MATERIAL kill_8_material = {
#include "stg22/fall_surfaces/kill_8_material.inc"
};
static Sint16 kill_8_strips[5] = {
#include "stg22/fall_surfaces/kill_8_strips.inc"
};
static NJS_MESHSET kill_8_meshset = {
#include "stg22/fall_surfaces/kill_8_meshset.inc"
};
static NJS_POINT3 kill_8_points[4] = {
#include "stg22/fall_surfaces/kill_8_points.inc"
};
static NJS_VECTOR kill_8_normals[4] = {
#include "stg22/fall_surfaces/kill_8_normals.inc"
};
static NJS_MODEL kill_8_model = {
#include "stg22/fall_surfaces/kill_8_model.inc"
};
NJS_OBJECT gadget_kill_8 = {
#include "stg22/fall_surfaces/gadget_kill_8.inc"
};
