/* the wooden box, CONTWOOD in a stage's object list. a player breaks it for
 * points. a broken box can release one of the stage's small animals. a box
 * falls when a box under it breaks */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/wood_box.h"
#include "shared/crate_mesh.h"
#include "shared/wood_box/text.inc"

static NJS_TEXNAME wood_box_textures[2] = {
#include "shared/wood_box/wood_box_textures.inc"
};

NJS_TEXLIST wood_box_texlist = {
#include "shared/wood_box/wood_box_texlist.inc"
};

static struct crate_polygon_list wood_box_polygons = {
#include "shared/wood_box/wood_box_polygons.inc"
};

static Sint32 wood_box_vertices[51] = {
#include "shared/wood_box/wood_box_vertices.inc"
};

NJS_CNK_MODEL wood_box_model = {
#include "shared/wood_box/wood_box_model.inc"
};

NJS_CNK_OBJECT wood_box_object = {
#include "shared/wood_box/wood_box_object.inc"
};

static NJS_TEXNAME wood_box_piece1_textures[1] = {
#include "shared/wood_box/wood_box_piece1_textures.inc"
};

NJS_TEXLIST wood_box_piece1_texlist = {
#include "shared/wood_box/wood_box_piece1_texlist.inc"
};

static struct wood_box_piece1_polygon_list wood_box_piece1_polygons = {
#include "shared/wood_box/wood_box_piece1_polygons.inc"
};

static Sint32 wood_box_piece1_vertices[159] = {
#include "shared/wood_box/wood_box_piece1_vertices.inc"
};

NJS_CNK_MODEL wood_box_piece1_model = {
#include "shared/wood_box/wood_box_piece1_model.inc"
};

NJS_CNK_OBJECT wood_box_piece1_object = {
#include "shared/wood_box/wood_box_piece1_object.inc"
};

static NJS_TEXNAME wood_box_piece2_textures[1] = {
#include "shared/wood_box/wood_box_piece2_textures.inc"
};

NJS_TEXLIST wood_box_piece2_texlist = {
#include "shared/wood_box/wood_box_piece2_texlist.inc"
};

static struct wood_box_piece2_polygon_list wood_box_piece2_polygons = {
#include "shared/wood_box/wood_box_piece2_polygons.inc"
};

static Sint32 wood_box_piece2_vertices[99] = {
#include "shared/wood_box/wood_box_piece2_vertices.inc"
};

NJS_CNK_MODEL wood_box_piece2_model = {
#include "shared/wood_box/wood_box_piece2_model.inc"
};

NJS_CNK_OBJECT wood_box_piece2_object = {
#include "shared/wood_box/wood_box_piece2_object.inc"
};

static NJS_TEXNAME wood_box_piece3_textures[1] = {
#include "shared/wood_box/wood_box_piece3_textures.inc"
};

NJS_TEXLIST wood_box_piece3_texlist = {
#include "shared/wood_box/wood_box_piece3_texlist.inc"
};

static struct wood_box_piece3_polygon_list wood_box_piece3_polygons = {
#include "shared/wood_box/wood_box_piece3_polygons.inc"
};

static Sint32 wood_box_piece3_vertices[135] = {
#include "shared/wood_box/wood_box_piece3_vertices.inc"
};

NJS_CNK_MODEL wood_box_piece3_model = {
#include "shared/wood_box/wood_box_piece3_model.inc"
};

NJS_CNK_OBJECT wood_box_piece3_object = {
#include "shared/wood_box/wood_box_piece3_object.inc"
};

NJS_TEXLIST *wood_box_texlists[5] = {
#include "shared/wood_box/wood_box_texlists.inc"
};

#include "shared/wood_box_impl.h" /* IWYU pragma: keep -- the code it compiles here */
