#include <ninja.h>
#include <sg_xpt.h>
#include "shared/plain_tree.h"
#include "stg13/plain_tree_art/text.inc"

static NJS_TEXNAME plain_tree_textures[3] = {
#include "stg13/plain_tree_art/plain_tree_textures.inc"
};

NJS_TEXLIST plain_tree_texlist = {
#include "stg13/plain_tree_art/plain_tree_texlist.inc"
};

static Sint16 plain_tree_object_child_polygons[199] = {
#include "stg13/plain_tree_art/plain_tree_object_child_polygons.inc"
};

static Sint32 plain_tree_object_child_vertices[167] = {
#include "stg13/plain_tree_art/plain_tree_object_child_vertices.inc"
};

static NJS_CNK_MODEL plain_tree_object_child_model = {
#include "stg13/plain_tree_art/plain_tree_object_child_model.inc"
};

static NJS_CNK_OBJECT plain_tree_object_child = {
#include "stg13/plain_tree_art/plain_tree_object_child.inc"
};

static Sint16 plain_tree_polygons[25] = {
#include "stg13/plain_tree_art/plain_tree_polygons.inc"
};

static Sint32 plain_tree_vertices[19] = {
#include "stg13/plain_tree_art/plain_tree_vertices.inc"
};

static NJS_CNK_MODEL plain_tree_model = {
#include "stg13/plain_tree_art/plain_tree_model.inc"
};

NJS_CNK_OBJECT plain_tree_object = {
#include "stg13/plain_tree_art/plain_tree_object.inc"
};