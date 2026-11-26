/* Safe left b geometry. */
#ifndef STG08_SAFE_SAFE_LEFT_B_H
#define STG08_SAFE_SAFE_LEFT_B_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME safe_left_b_names[3] = {
#include "stg08/safe/safe_left_b/safe_left_b_names.inc"
};

static NJS_TEXLIST security_safe_left_b_texlist = {
#include "stg08/safe/safe_left_b/security_safe_left_b_texlist.inc"
};

static Sint16 safe_left_b_polygons[703] = {
#include "stg08/safe/safe_left_b/safe_left_b_polygons.inc"
};

static Sint32 safe_left_b_vertices[575] = {
#include "stg08/safe/safe_left_b/safe_left_b_vertices.inc"
};

static NJS_CNK_MODEL safe_left_b_model = {
#include "stg08/safe/safe_left_b/safe_left_b_model.inc"
};

static NJS_CNK_OBJECT security_safe_left_b_object = {
#include "stg08/safe/safe_left_b/security_safe_left_b_object.inc"
};

#endif
