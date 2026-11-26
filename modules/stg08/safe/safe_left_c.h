/* Safe left c geometry. */
#ifndef STG08_SAFE_SAFE_LEFT_C_H
#define STG08_SAFE_SAFE_LEFT_C_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME safe_left_c_names[3] = {
#include "stg08/safe/safe_left_c/safe_left_c_names.inc"
};

static NJS_TEXLIST security_safe_left_c_texlist = {
#include "stg08/safe/safe_left_c/security_safe_left_c_texlist.inc"
};

static Sint16 safe_left_c_polygons[689] = {
#include "stg08/safe/safe_left_c/safe_left_c_polygons.inc"
};

static Sint32 safe_left_c_vertices[559] = {
#include "stg08/safe/safe_left_c/safe_left_c_vertices.inc"
};

static NJS_CNK_MODEL safe_left_c_model = {
#include "stg08/safe/safe_left_c/safe_left_c_model.inc"
};

static NJS_CNK_OBJECT security_safe_left_c_object = {
#include "stg08/safe/safe_left_c/security_safe_left_c_object.inc"
};

#endif
