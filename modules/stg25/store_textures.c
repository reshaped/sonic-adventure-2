#include <ninja.h>
#include <sg_xpt.h>

#include "shared/store_art.h"
#include "shared/store_mesh.h"
#include "shared/store_textures.h"

/* a sized array pads its name, so the name after it starts on a 4-byte
 * boundary */
const char store_box_shadow_name[20] = {
#include "stg25/store_textures/store_box_shadow_name.inc"
};
const char store_box_side_name[20] = {
#include "stg25/store_textures/store_box_side_name.inc"
};
const char store_box_lid_name[22] = {
#include "stg25/store_textures/store_box_lid_name.inc"
};

static NJS_TEXNAME textures[3] = {
#include "stg25/store_textures/textures.inc"
};
NJS_TEXLIST store_box_blue_texlist = {
#include "stg25/store_textures/store_box_blue_texlist.inc"
};

static struct store_box_polygons store_box_blue_polygons = {
#include "stg25/store_textures/store_box_blue_polygons.inc"
};

static Sint32 store_box_blue_vertices[359] = {
#include "stg25/store_textures/store_box_blue_vertices.inc"
};

NJS_CNK_MODEL store_box_blue_model = {
#include "stg25/store_textures/store_box_blue_model.inc"
};

NJS_CNK_OBJECT store_box_blue_object = {
#include "stg25/store_textures/store_box_blue_object.inc"
};
