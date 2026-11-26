/* Safe left a geometry. */
#ifndef STG08_SAFE_SAFE_LEFT_A_H
#define STG08_SAFE_SAFE_LEFT_A_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME safe_left_a_names[3] = {
#include "stg08/safe/safe_left_a/safe_left_a_names.inc"
};

static NJS_TEXLIST security_safe_left_a_texlist = {
#include "stg08/safe/safe_left_a/security_safe_left_a_texlist.inc"
};

static Sint16 safe_left_a_polygons[689] = {
#include "stg08/safe/safe_left_a/safe_left_a_polygons.inc"
};

static Sint32 safe_left_a_vertices[551] = {
#include "stg08/safe/safe_left_a/safe_left_a_vertices.inc"
};

static NJS_CNK_MODEL safe_left_a_model = {
#include "stg08/safe/safe_left_a/safe_left_a_model.inc"
};

static NJS_CNK_OBJECT security_safe_left_a_object = {
#include "stg08/safe/safe_left_a/security_safe_left_a_object.inc"
};

#endif
