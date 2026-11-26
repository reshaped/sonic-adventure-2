/* Safe front c geometry. */
#ifndef STG08_SAFE_SAFE_FRONT_C_H
#define STG08_SAFE_SAFE_FRONT_C_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME safe_front_c_names[1] = {
#include "stg08/safe/safe_front_c/safe_front_c_names.inc"
};

static NJS_TEXLIST security_safe_front_c_texlist = {
#include "stg08/safe/safe_front_c/security_safe_front_c_texlist.inc"
};

static Sint16 safe_front_c_polygons[25] = {
#include "stg08/safe/safe_front_c/safe_front_c_polygons.inc"
};

static Sint32 safe_front_c_vertices[19] = {
#include "stg08/safe/safe_front_c/safe_front_c_vertices.inc"
};

static NJS_CNK_MODEL safe_front_c_model = {
#include "stg08/safe/safe_front_c/safe_front_c_model.inc"
};

static NJS_CNK_OBJECT security_safe_front_c_object = {
#include "stg08/safe/safe_front_c/security_safe_front_c_object.inc"
};

#endif
