/* Safe right b geometry. */
#ifndef STG08_SAFE_SAFE_RIGHT_B_H
#define STG08_SAFE_SAFE_RIGHT_B_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME safe_right_b_names[4] = {
#include "stg08/safe/safe_right_b/safe_right_b_names.inc"
};

static NJS_TEXLIST security_safe_right_b_texlist = {
#include "stg08/safe/safe_right_b/security_safe_right_b_texlist.inc"
};

static Sint16 safe_right_b_polygons[457] = {
#include "stg08/safe/safe_right_b/safe_right_b_polygons.inc"
};

static Sint32 safe_right_b_vertices[399] = {
#include "stg08/safe/safe_right_b/safe_right_b_vertices.inc"
};

static NJS_CNK_MODEL safe_right_b_model = {
#include "stg08/safe/safe_right_b/safe_right_b_model.inc"
};

static NJS_CNK_OBJECT security_safe_right_b_object = {
#include "stg08/safe/safe_right_b/security_safe_right_b_object.inc"
};

#endif
