/* Safe closed c geometry. */
#ifndef STG08_SAFE_SAFE_CLOSED_C_H
#define STG08_SAFE_SAFE_CLOSED_C_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME safe_closed_c_names[4] = {
#include "stg08/safe/safe_closed_c/safe_closed_c_names.inc"
};

static NJS_TEXLIST security_safe_closed_c_texlist = {
#include "stg08/safe/safe_closed_c/security_safe_closed_c_texlist.inc"
};

static Sint16 safe_closed_c_part_2_polygons[25] = {
#include "stg08/safe/safe_closed_c/safe_closed_c_part_2_polygons.inc"
};

static Sint32 safe_closed_c_part_2_vertices[19] = {
#include "stg08/safe/safe_closed_c/safe_closed_c_part_2_vertices.inc"
};

static NJS_CNK_MODEL safe_closed_c_part_2_model = {
#include "stg08/safe/safe_closed_c/safe_closed_c_part_2_model.inc"
};

static NJS_CNK_OBJECT safe_closed_c_part_2 = {
#include "stg08/safe/safe_closed_c/safe_closed_c_part_2.inc"
};

static Sint16 safe_closed_c_part_1_polygons[291] = {
#include "stg08/safe/safe_closed_c/safe_closed_c_part_1_polygons.inc"
};

static Sint32 safe_closed_c_part_1_vertices[207] = {
#include "stg08/safe/safe_closed_c/safe_closed_c_part_1_vertices.inc"
};

static NJS_CNK_MODEL safe_closed_c_part_1_model = {
#include "stg08/safe/safe_closed_c/safe_closed_c_part_1_model.inc"
};

static NJS_CNK_OBJECT safe_closed_c_part_1 = {
#include "stg08/safe/safe_closed_c/safe_closed_c_part_1.inc"
};

static Sint16 safe_closed_c_polygons[565] = {
#include "stg08/safe/safe_closed_c/safe_closed_c_polygons.inc"
};

static Sint32 safe_closed_c_vertices[399] = {
#include "stg08/safe/safe_closed_c/safe_closed_c_vertices.inc"
};

static NJS_CNK_MODEL safe_closed_c_model = {
#include "stg08/safe/safe_closed_c/safe_closed_c_model.inc"
};

static NJS_CNK_OBJECT security_safe_closed_c_object = {
#include "stg08/safe/safe_closed_c/security_safe_closed_c_object.inc"
};

#endif
