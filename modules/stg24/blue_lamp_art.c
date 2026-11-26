/* the blue lamp's model and its lens's scroll. no code reads
 * blue_lamp_object */
#include <ninja.h>
#include <sg_xpt.h>
#include "stg24/lamp_art.h"
#include "draw/texture_anim.h"

static Sint16 blue_lamp_polygons[161] = {
#include "stg24/blue_lamp_art/blue_lamp_polygons.inc"
};

static Sint32 blue_lamp_vertices[195] = {
#include "stg24/blue_lamp_art/blue_lamp_vertices.inc"
};

NJS_CNK_MODEL blue_lamp_model = {
#include "stg24/blue_lamp_art/blue_lamp_model.inc"
};

static NJS_CNK_OBJECT blue_lamp_object = {
#include "stg24/blue_lamp_art/blue_lamp_object.inc"
};

static Sint16 blue_lamp_scroll_uv_keys[106] = {
#include "stg24/blue_lamp_art/blue_lamp_scroll_uv_keys.inc"
};

struct texture_anim blue_lamp_scroll = {
#include "stg24/blue_lamp_art/blue_lamp_scroll.inc"
};
