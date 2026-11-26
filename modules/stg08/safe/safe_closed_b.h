/* Safe closed b geometry. */
#ifndef STG08_SAFE_SAFE_CLOSED_B_H
#define STG08_SAFE_SAFE_CLOSED_B_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME safe_closed_b_names[4] = {
#include "stg08/safe/safe_closed_b/safe_closed_b_names.inc"
};

static NJS_TEXLIST security_safe_closed_b_texlist = {
#include "stg08/safe/safe_closed_b/security_safe_closed_b_texlist.inc"
};

static Sint16 safe_closed_b_part_2_polygons[25] = {
#include "stg08/safe/safe_closed_b/safe_closed_b_part_2_polygons.inc"
};

static Sint32 safe_closed_b_part_2_vertices[19] = {
#include "stg08/safe/safe_closed_b/safe_closed_b_part_2_vertices.inc"
};

static NJS_CNK_MODEL safe_closed_b_part_2_model = {
#include "stg08/safe/safe_closed_b/safe_closed_b_part_2_model.inc"
};

static NJS_CNK_OBJECT safe_closed_b_part_2 = {
#include "stg08/safe/safe_closed_b/safe_closed_b_part_2.inc"
};

static Sint16 safe_closed_b_part_1_polygons[297] = {
#include "stg08/safe/safe_closed_b/safe_closed_b_part_1_polygons.inc"
};

static Sint32 safe_closed_b_part_1_vertices[219] = {
#include "stg08/safe/safe_closed_b/safe_closed_b_part_1_vertices.inc"
};

static NJS_CNK_MODEL safe_closed_b_part_1_model = {
#include "stg08/safe/safe_closed_b/safe_closed_b_part_1_model.inc"
};

static NJS_CNK_OBJECT safe_closed_b_part_1 = {
#include "stg08/safe/safe_closed_b/safe_closed_b_part_1.inc"
};

static Sint16 safe_closed_b_polygons[577] = {
#include "stg08/safe/safe_closed_b/safe_closed_b_polygons.inc"
};

static Sint32 safe_closed_b_vertices[427] = {
#include "stg08/safe/safe_closed_b/safe_closed_b_vertices.inc"
};

static NJS_CNK_MODEL safe_closed_b_model = {
#include "stg08/safe/safe_closed_b/safe_closed_b_model.inc"
};

static NJS_CNK_OBJECT security_safe_closed_b_object = {
#include "stg08/safe/safe_closed_b/security_safe_closed_b_object.inc"
};

#endif
