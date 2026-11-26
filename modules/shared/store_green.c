#include <ninja.h>
#include <sg_xpt.h>

#include "shared/store_art.h"
#include "shared/store_mesh.h"
#include "shared/store_textures.h"

static NJS_TEXNAME textures[3] = {
#include "shared/store_green/textures.inc"
};
NJS_TEXLIST store_box_green_texlist = {
#include "shared/store_green/store_box_green_texlist.inc"
};

static struct store_box_polygons store_box_green_polygons = {
#include "shared/store_green/store_box_green_polygons.inc"
};

static Sint32 store_box_green_vertices[343] = {
#include "shared/store_green/store_box_green_vertices.inc"
};

NJS_CNK_MODEL store_box_green_model = {
#include "shared/store_green/store_box_green_model.inc"
};

NJS_CNK_OBJECT store_box_green_object = {
#include "shared/store_green/store_box_green_object.inc"
};
