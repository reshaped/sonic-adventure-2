/* Safe right a geometry. */
#ifndef STG08_SAFE_SAFE_RIGHT_A_H
#define STG08_SAFE_SAFE_RIGHT_A_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME safe_right_a_names[4] = {
#include "stg08/safe/safe_right_a/safe_right_a_names.inc"
};

static NJS_TEXLIST security_safe_right_a_texlist = {
#include "stg08/safe/safe_right_a/security_safe_right_a_texlist.inc"
};

static Sint16 safe_right_a_polygons[485] = {
#include "stg08/safe/safe_right_a/safe_right_a_polygons.inc"
};

static Sint32 safe_right_a_vertices[427] = {
#include "stg08/safe/safe_right_a/safe_right_a_vertices.inc"
};

static NJS_CNK_MODEL safe_right_a_model = {
#include "stg08/safe/safe_right_a/safe_right_a_model.inc"
};

static NJS_CNK_OBJECT security_safe_right_a_object = {
#include "stg08/safe/safe_right_a/security_safe_right_a_object.inc"
};

#endif
