/* Safe front b geometry. */
#ifndef STG08_SAFE_SAFE_FRONT_B_H
#define STG08_SAFE_SAFE_FRONT_B_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME safe_front_b_names[1] = {
#include "stg08/safe/safe_front_b/safe_front_b_names.inc"
};

static NJS_TEXLIST security_safe_front_b_texlist = {
#include "stg08/safe/safe_front_b/security_safe_front_b_texlist.inc"
};

static Sint16 safe_front_b_polygons[25] = {
#include "stg08/safe/safe_front_b/safe_front_b_polygons.inc"
};

static Sint32 safe_front_b_vertices[19] = {
#include "stg08/safe/safe_front_b/safe_front_b_vertices.inc"
};

static NJS_CNK_MODEL safe_front_b_model = {
#include "stg08/safe/safe_front_b/safe_front_b_model.inc"
};

static NJS_CNK_OBJECT security_safe_front_b_object = {
#include "stg08/safe/safe_front_b/security_safe_front_b_object.inc"
};

#endif
