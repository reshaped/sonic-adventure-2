/* The alternate iron crate has its own texture names and debris materials. */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/iron_box_e.h"

/* NOLINTBEGIN(readability-identifier-naming) */
#define iron_box_build iron_box_e_build
#define iron_box_init iron_box_e_init
#define iron_box_texlists iron_box_e_texlists
#define iron_box_texlist iron_box_e_texlist
#define iron_box_model iron_box_e_model
#define iron_box_object iron_box_e_object
#define iron_box_piece1_texlist iron_box_e_piece1_texlist
#define iron_box_piece1_model iron_box_e_piece1_model
#define iron_box_piece1_object iron_box_e_piece1_object
#define iron_box_piece2_texlist iron_box_e_piece2_texlist
#define iron_box_piece2_model iron_box_e_piece2_model
#define iron_box_piece2_object iron_box_e_piece2_object
#define iron_box_piece3_texlist iron_box_e_piece3_texlist
#define iron_box_piece3_model iron_box_e_piece3_model
#define iron_box_piece3_object iron_box_e_piece3_object
/* NOLINTEND(readability-identifier-naming) */

#include "shared/iron_box.h"
#include "shared/crate_mesh.h"
#include "shared/iron_box_e/text.inc"

static NJS_TEXNAME iron_box_textures[2] = {
#include "shared/iron_box_e/iron_box_textures.inc"
};

NJS_TEXLIST iron_box_e_texlist = {
#include "shared/iron_box_e/iron_box_e_texlist.inc"
};

static struct crate_polygon_list iron_box_polygons = {
#include "shared/iron_box_e/iron_box_polygons.inc"
};

static Sint32 iron_box_vertices[51] = {
#include "shared/iron_box_e/iron_box_vertices.inc"
};

NJS_CNK_MODEL iron_box_e_model = {
#include "shared/iron_box_e/iron_box_e_model.inc"
};

NJS_CNK_OBJECT iron_box_e_object = {
#include "shared/iron_box_e/iron_box_e_object.inc"
};

static NJS_TEXNAME iron_box_piece1_textures[1] = {
#include "shared/iron_box_e/iron_box_piece1_textures.inc"
};

NJS_TEXLIST iron_box_e_piece1_texlist = {
#include "shared/iron_box_e/iron_box_e_piece1_texlist.inc"
};

static struct iron_box_piece1_polygon_list iron_box_piece1_polygons = {
#include "shared/iron_box_e/iron_box_piece1_polygons.inc"
};

static Sint32 iron_box_piece1_vertices[99] = {
#include "shared/iron_box_e/iron_box_piece1_vertices.inc"
};

NJS_CNK_MODEL iron_box_e_piece1_model = {
#include "shared/iron_box_e/iron_box_e_piece1_model.inc"
};

NJS_CNK_OBJECT iron_box_e_piece1_object = {
#include "shared/iron_box_e/iron_box_e_piece1_object.inc"
};

static NJS_TEXNAME iron_box_piece2_textures[2] = {
#include "shared/iron_box_e/iron_box_piece2_textures.inc"
};

NJS_TEXLIST iron_box_e_piece2_texlist = {
#include "shared/iron_box_e/iron_box_e_piece2_texlist.inc"
};

static struct iron_box_piece2_polygon_list iron_box_piece2_polygons = {
#include "shared/iron_box_e/iron_box_piece2_polygons.inc"
};

static Sint32 iron_box_piece2_vertices[117] = {
#include "shared/iron_box_e/iron_box_piece2_vertices.inc"
};

NJS_CNK_MODEL iron_box_e_piece2_model = {
#include "shared/iron_box_e/iron_box_e_piece2_model.inc"
};

NJS_CNK_OBJECT iron_box_e_piece2_object = {
#include "shared/iron_box_e/iron_box_e_piece2_object.inc"
};

static NJS_TEXNAME iron_box_piece3_textures[1] = {
#include "shared/iron_box_e/iron_box_piece3_textures.inc"
};

NJS_TEXLIST iron_box_e_piece3_texlist = {
#include "shared/iron_box_e/iron_box_e_piece3_texlist.inc"
};

static struct iron_box_piece3_polygon_list iron_box_piece3_polygons = {
#include "shared/iron_box_e/iron_box_piece3_polygons.inc"
};

static Sint32 iron_box_piece3_vertices[111] = {
#include "shared/iron_box_e/iron_box_piece3_vertices.inc"
};

NJS_CNK_MODEL iron_box_e_piece3_model = {
#include "shared/iron_box_e/iron_box_e_piece3_model.inc"
};

NJS_CNK_OBJECT iron_box_e_piece3_object = {
#include "shared/iron_box_e/iron_box_e_piece3_object.inc"
};

NJS_TEXLIST *iron_box_e_texlists[5] = {
#include "shared/iron_box_e/iron_box_e_texlists.inc"
};

#include "shared/iron_box_impl.h" /* IWYU pragma: keep -- the code it compiles here */
