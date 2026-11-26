#include <ninja.h>
#include <sg_xpt.h>

#include "stg08/fixture_art.h"

static const char miu128_ig009[16] = {
#include "stg08/tank_art/miu128_ig009.inc"
};

static const char miu64_ig008[12] = {
#include "stg08/tank_art/miu64_ig008.inc"
};

static const char miu64_ig011[12] = {
#include "stg08/tank_art/miu64_ig011.inc"
};

static const char miu64_ig015[12] = {
#include "stg08/tank_art/miu64_ig015.inc"
};

static const char miu64_sh001[12] = {
#include "stg08/tank_art/miu64_sh001.inc"
};

static const char pr128_14[9] = {
#include "stg08/tank_art/pr128_14.inc"
};

static NJS_TEXNAME security_tank_materials[6] = {
#include "stg08/tank_art/security_tank_materials.inc"
};

NJS_TEXLIST security_tank_texlist = {
#include "stg08/tank_art/security_tank_texlist.inc"
};

static Sint16 security_tank_valve_polygons[373] = {
#include "stg08/tank_art/security_tank_valve_polygons.inc"
};

static Sint32 security_tank_valve_vertices[367] = {
#include "stg08/tank_art/security_tank_valve_vertices.inc"
};

static NJS_CNK_MODEL security_tank_valve_model = {
#include "stg08/tank_art/security_tank_valve_model.inc"
};

static NJS_CNK_OBJECT security_tank_valve = {
#include "stg08/tank_art/security_tank_valve.inc"
};

static Sint16 security_tank_object_polygons[167] = {
#include "stg08/tank_art/security_tank_object_polygons.inc"
};

static Sint32 security_tank_object_vertices[107] = {
#include "stg08/tank_art/security_tank_object_vertices.inc"
};

static NJS_CNK_MODEL security_tank_body_model = {
#include "stg08/tank_art/security_tank_body_model.inc"
};

NJS_CNK_OBJECT security_tank_object[1] = {
#include "stg08/tank_art/security_tank_object.inc"
};

static NJS_TEXNAME security_tank_rim_materials[2] = {
#include "stg08/tank_art/security_tank_rim_materials.inc"
};

NJS_TEXLIST security_tank_rim_texlist = {
#include "stg08/tank_art/security_tank_rim_texlist.inc"
};

static Sint16 security_tank_rim_polygons[109] = {
#include "stg08/tank_art/security_tank_rim_polygons.inc"
};

static Sint32 security_tank_rim_vertices[83] = {
#include "stg08/tank_art/security_tank_rim_vertices.inc"
};

NJS_CNK_MODEL security_tank_rim_model = {
#include "stg08/tank_art/security_tank_rim_model.inc"
};

static NJS_CNK_OBJECT security_tank_rim_object = {
#include "stg08/tank_art/security_tank_rim_object.inc"
};

static NJS_TEXNAME security_tank_base_materials[2] = {
#include "stg08/tank_art/security_tank_base_materials.inc"
};

NJS_TEXLIST security_tank_base_texlist = {
#include "stg08/tank_art/security_tank_base_texlist.inc"
};

static Sint16 security_tank_base_polygons[167] = {
#include "stg08/tank_art/security_tank_base_polygons.inc"
};

static Sint32 security_tank_base_vertices[103] = {
#include "stg08/tank_art/security_tank_base_vertices.inc"
};

NJS_CNK_MODEL security_tank_base_model = {
#include "stg08/tank_art/security_tank_base_model.inc"
};

static NJS_CNK_OBJECT security_tank_base_object = {
#include "stg08/tank_art/security_tank_base_object.inc"
};

static NJS_TEXNAME security_tank_side_materials[2] = {
#include "stg08/tank_art/security_tank_side_materials.inc"
};

NJS_TEXLIST security_tank_side_texlist = {
#include "stg08/tank_art/security_tank_side_texlist.inc"
};

static Sint16 security_tank_side_polygons[189] = {
#include "stg08/tank_art/security_tank_side_polygons.inc"
};

static Sint32 security_tank_side_vertices[195] = {
#include "stg08/tank_art/security_tank_side_vertices.inc"
};

NJS_CNK_MODEL security_tank_side_model = {
#include "stg08/tank_art/security_tank_side_model.inc"
};

static NJS_CNK_OBJECT security_tank_side_object = {
#include "stg08/tank_art/security_tank_side_object.inc"
};
