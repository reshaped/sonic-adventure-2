#include <ninja.h>
#include <sg_xpt.h>

#include "shared/ring.h"

#include "shared/ring_art/text.inc"

/* the ring's two texture names, the second on a four-byte boundary */
const char ring_texture_filenames[33] = TEXT_1 TEXT_2;

/* the texture lists and chunk models that 54 images place, from the disc.
 * its D precedes ring_chain_art.c */
static NJS_TEXNAME ring_textures[1] = {
#include "shared/ring_art/ring_textures.inc"
};
NJS_TEXLIST ring_texlist = {
#include "shared/ring_art/ring_texlist.inc"
};
static Sint16 ring_polygons[89] = {
#include "shared/ring_art/ring_polygons.inc"
};
static Sint32 ring_vertices[195] = {
#include "shared/ring_art/ring_vertices.inc"
};
NJS_CNK_MODEL ring_model[1] = {
#include "shared/ring_art/ring_model.inc"
};
NJS_CNK_OBJECT ring_object[1] = {
#include "shared/ring_art/ring_object.inc"
};
static NJS_TEXNAME ring_middle_textures[1] = {
#include "shared/ring_art/ring_middle_textures.inc"
};
NJS_TEXLIST ring_middle_texlist = {
#include "shared/ring_art/ring_middle_texlist.inc"
};
static Sint16 ring_middle_polygons[95] = {
#include "shared/ring_art/ring_middle_polygons.inc"
};
static Sint32 ring_middle_vertices[219] = {
#include "shared/ring_art/ring_middle_vertices.inc"
};
static NJS_CNK_MODEL ring_middle_model = {
#include "shared/ring_art/ring_middle_model.inc"
};
NJS_CNK_OBJECT ring_middle_object[1] = {
#include "shared/ring_art/ring_middle_object.inc"
};
static NJS_TEXNAME ring_near_textures[1] = {
#include "shared/ring_art/ring_near_textures.inc"
};
NJS_TEXLIST ring_near_texlist = {
#include "shared/ring_art/ring_near_texlist.inc"
};
static Sint16 ring_near_polygons[113] = {
#include "shared/ring_art/ring_near_polygons.inc"
};
static Sint32 ring_near_vertices[243] = {
#include "shared/ring_art/ring_near_vertices.inc"
};
static NJS_CNK_MODEL ring_near_model = {
#include "shared/ring_art/ring_near_model.inc"
};
NJS_CNK_OBJECT ring_near_object[1] = {
#include "shared/ring_art/ring_near_object.inc"
};
static NJS_TEXNAME ring_shadow_textures[1] = {
#include "shared/ring_art/ring_shadow_textures.inc"
};
NJS_TEXLIST ring_shadow_texlist = {
#include "shared/ring_art/ring_shadow_texlist.inc"
};
static Sint16 ring_shadow_polygons[25] = {
#include "shared/ring_art/ring_shadow_polygons.inc"
};
static Sint32 ring_shadow_vertices[27] = {
#include "shared/ring_art/ring_shadow_vertices.inc"
};
NJS_CNK_MODEL ring_shadow_model = {
#include "shared/ring_art/ring_shadow_model.inc"
};
NJS_CNK_OBJECT ring_shadow_object = {
#include "shared/ring_art/ring_shadow_object.inc"
};
