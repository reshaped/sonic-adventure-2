#include <ninja.h>
#include <sg_xpt.h>

#include "shared/spring.h"

#include "shared/spring_art/text.inc"

const char spring_texture_filenames[14] = TEXT_1;

/* the texture lists and chunk models that 54 images place, from the disc.
 * its D precedes spring.c */
static NJS_TEXNAME spring_a_texture[1] = {
#include "shared/spring_art/spring_a_texture.inc"
};
NJS_TEXLIST spring_texlist = {
#include "shared/spring_art/spring_texlist.inc"
};
static Sint16 spring_a_top_polygons[169] = {
#include "shared/spring_art/spring_a_top_polygons.inc"
};
static Sint32 spring_a_top_vertices[105] = {
#include "shared/spring_art/spring_a_top_vertices.inc"
};
static NJS_CNK_MODEL spring_a_top_model = {
#include "shared/spring_art/spring_a_top_model.inc"
};
NJS_CNK_OBJECT spring_a_top_object = {
#include "shared/spring_art/spring_a_top_object.inc"
};
static Sint16 spring_a_coil_polygons[119] = {
#include "shared/spring_art/spring_a_coil_polygons.inc"
};
static Sint32 spring_a_coil_vertices[99] = {
#include "shared/spring_art/spring_a_coil_vertices.inc"
};
static NJS_CNK_MODEL spring_a_coil_model = {
#include "shared/spring_art/spring_a_coil_model.inc"
};
NJS_CNK_OBJECT spring_a_coil_object = {
#include "shared/spring_art/spring_a_coil_object.inc"
};
static Sint16 spring_a_base_polygons[279] = {
#include "shared/spring_art/spring_a_base_polygons.inc"
};
static Sint32 spring_a_base_vertices[111] = {
#include "shared/spring_art/spring_a_base_vertices.inc"
};
NJS_CNK_MODEL spring_a_base_model = {
#include "shared/spring_art/spring_a_base_model.inc"
};
NJS_CNK_OBJECT spring_a_base_object = {
#include "shared/spring_art/spring_a_base_object.inc"
};
static NJS_TEXNAME spring_b_texture[1] = {
#include "shared/spring_art/spring_b_texture.inc"
};
NJS_TEXLIST spring_b_texlist = {
#include "shared/spring_art/spring_b_texlist.inc"
};
static Sint16 spring_b_top_polygons[213] = {
#include "shared/spring_art/spring_b_top_polygons.inc"
};
static Sint32 spring_b_top_vertices[111] = {
#include "shared/spring_art/spring_b_top_vertices.inc"
};
static NJS_CNK_MODEL spring_b_top_model = {
#include "shared/spring_art/spring_b_top_model.inc"
};
NJS_CNK_OBJECT spring_b_top_object = {
#include "shared/spring_art/spring_b_top_object.inc"
};
static Sint16 spring_b_coil_polygons[119] = {
#include "shared/spring_art/spring_b_coil_polygons.inc"
};
static Sint32 spring_b_coil_vertices[99] = {
#include "shared/spring_art/spring_b_coil_vertices.inc"
};
static NJS_CNK_MODEL spring_b_coil_model = {
#include "shared/spring_art/spring_b_coil_model.inc"
};
NJS_CNK_OBJECT spring_b_coil_object = {
#include "shared/spring_art/spring_b_coil_object.inc"
};
static Sint16 spring_b_base_polygons[213] = {
#include "shared/spring_art/spring_b_base_polygons.inc"
};
static Sint32 spring_b_base_vertices[87] = {
#include "shared/spring_art/spring_b_base_vertices.inc"
};
NJS_CNK_MODEL spring_b_base_model = {
#include "shared/spring_art/spring_b_base_model.inc"
};
NJS_CNK_OBJECT spring_b_base_object = {
#include "shared/spring_art/spring_b_base_object.inc"
};
