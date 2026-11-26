/* Safe front a geometry. */
#ifndef STG08_SAFE_SAFE_FRONT_A_H
#define STG08_SAFE_SAFE_FRONT_A_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME safe_front_a_names[1] = {
#include "stg08/safe/safe_front_a/safe_front_a_names.inc"
};

static NJS_TEXLIST security_safe_front_a_texlist = {
#include "stg08/safe/safe_front_a/security_safe_front_a_texlist.inc"
};

static Sint16 safe_front_a_polygons[25] = {
#include "stg08/safe/safe_front_a/safe_front_a_polygons.inc"
};

static Sint32 safe_front_a_vertices[19] = {
#include "stg08/safe/safe_front_a/safe_front_a_vertices.inc"
};

static NJS_CNK_MODEL safe_front_a_model = {
#include "stg08/safe/safe_front_a/safe_front_a_model.inc"
};

static NJS_CNK_OBJECT security_safe_front_a_object = {
#include "stg08/safe/safe_front_a/security_safe_front_a_object.inc"
};

#endif
