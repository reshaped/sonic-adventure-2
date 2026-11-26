/* Safe right c geometry. */
#ifndef STG08_SAFE_SAFE_RIGHT_C_H
#define STG08_SAFE_SAFE_RIGHT_C_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME safe_right_c_names[4] = {
#include "stg08/safe/safe_right_c/safe_right_c_names.inc"
};

static NJS_TEXLIST security_safe_right_c_texlist = {
#include "stg08/safe/safe_right_c/security_safe_right_c_texlist.inc"
};

static Sint16 safe_right_c_polygons[471] = {
#include "stg08/safe/safe_right_c/safe_right_c_polygons.inc"
};

static Sint32 safe_right_c_vertices[391] = {
#include "stg08/safe/safe_right_c/safe_right_c_vertices.inc"
};

static NJS_CNK_MODEL safe_right_c_model = {
#include "stg08/safe/safe_right_c/safe_right_c_model.inc"
};

static NJS_CNK_OBJECT security_safe_right_c_object = {
#include "stg08/safe/safe_right_c/security_safe_right_c_object.inc"
};

#endif
