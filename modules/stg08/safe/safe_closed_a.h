/* Safe closed a geometry. */
#ifndef STG08_SAFE_SAFE_CLOSED_A_H
#define STG08_SAFE_SAFE_CLOSED_A_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME safe_closed_a_names[4] = {
#include "stg08/safe/safe_closed_a/safe_closed_a_names.inc"
};

static NJS_TEXLIST security_safe_closed_a_texlist = {
#include "stg08/safe/safe_closed_a/security_safe_closed_a_texlist.inc"
};

static Sint16 safe_closed_a_part_2_polygons[25] = {
#include "stg08/safe/safe_closed_a/safe_closed_a_part_2_polygons.inc"
};

static Sint32 safe_closed_a_part_2_vertices[19] = {
#include "stg08/safe/safe_closed_a/safe_closed_a_part_2_vertices.inc"
};

static NJS_CNK_MODEL safe_closed_a_part_2_model = {
#include "stg08/safe/safe_closed_a/safe_closed_a_part_2_model.inc"
};

static NJS_CNK_OBJECT safe_closed_a_part_2 = {
#include "stg08/safe/safe_closed_a/safe_closed_a_part_2.inc"
};

static Sint16 safe_closed_a_part_1_polygons[291] = {
#include "stg08/safe/safe_closed_a/safe_closed_a_part_1_polygons.inc"
};

static Sint32 safe_closed_a_part_1_vertices[203] = {
#include "stg08/safe/safe_closed_a/safe_closed_a_part_1_vertices.inc"
};

static NJS_CNK_MODEL safe_closed_a_part_1_model = {
#include "stg08/safe/safe_closed_a/safe_closed_a_part_1_model.inc"
};

static NJS_CNK_OBJECT safe_closed_a_part_1 = {
#include "stg08/safe/safe_closed_a/safe_closed_a_part_1.inc"
};

static Sint16 safe_closed_a_polygons[585] = {
#include "stg08/safe/safe_closed_a/safe_closed_a_polygons.inc"
};

static Sint32 safe_closed_a_vertices[423] = {
#include "stg08/safe/safe_closed_a/safe_closed_a_vertices.inc"
};

static NJS_CNK_MODEL safe_closed_a_model = {
#include "stg08/safe/safe_closed_a/safe_closed_a_model.inc"
};

static NJS_CNK_OBJECT security_safe_closed_a_object = {
#include "stg08/safe/safe_closed_a/security_safe_closed_a_object.inc"
};

#endif
