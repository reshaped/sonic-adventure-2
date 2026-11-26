#include <ninja.h>
#include <sg_xpt.h>

#include "stg08/fixture_art.h"

static const char miu32_sh001[12] = {
#include "stg08/laser_art/miu32_sh001.inc"
};

static const char miu64_ig015[12] = {
#include "stg08/laser_art/miu64_ig015.inc"
};

static const char miu64_ig017[12] = {
#include "stg08/laser_art/miu64_ig017.inc"
};

static const char miu64_sh001[12] = {
#include "stg08/laser_art/miu64_sh001.inc"
};

static NJS_TEXNAME security_laser_materials[4] = {
#include "stg08/laser_art/security_laser_materials.inc"
};

NJS_TEXLIST security_laser_texlist = {
#include "stg08/laser_art/security_laser_texlist.inc"
};

static Sint16 security_laser_left_polygons[387] = {
#include "stg08/laser_art/security_laser_left_polygons.inc"
};

static Sint32 security_laser_left_vertices[399] = {
#include "stg08/laser_art/security_laser_left_vertices.inc"
};

static NJS_CNK_MODEL security_laser_left_model = {
#include "stg08/laser_art/security_laser_left_model.inc"
};

NJS_CNK_OBJECT security_laser_left[1] = {
#include "stg08/laser_art/security_laser_left.inc"
};

static Sint16 security_laser_right_polygons[387] = {
#include "stg08/laser_art/security_laser_right_polygons.inc"
};

static Sint32 security_laser_right_vertices[399] = {
#include "stg08/laser_art/security_laser_right_vertices.inc"
};

static NJS_CNK_MODEL security_laser_right_model = {
#include "stg08/laser_art/security_laser_right_model.inc"
};

NJS_CNK_OBJECT security_laser_right[1] = {
#include "stg08/laser_art/security_laser_right.inc"
};

static Sint16 security_laser_body_polygons[43] = {
#include "stg08/laser_art/security_laser_body_polygons.inc"
};

static Sint32 security_laser_body_vertices[35] = {
#include "stg08/laser_art/security_laser_body_vertices.inc"
};

static NJS_CNK_MODEL security_laser_body_model = {
#include "stg08/laser_art/security_laser_body_model.inc"
};

NJS_CNK_OBJECT security_laser_body[1] = {
#include "stg08/laser_art/security_laser_body.inc"
};
