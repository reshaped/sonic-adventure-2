/* the orange lamp's model. no code reads orange_lamp_object */
#include <ninja.h>
#include <sg_xpt.h>
#include "stg24/lamp_art.h"

static Sint16 orange_lamp_polygons[53] = {
#include "stg24/orange_lamp_art/orange_lamp_polygons.inc"
};

static Sint32 orange_lamp_vertices[51] = {
#include "stg24/orange_lamp_art/orange_lamp_vertices.inc"
};

NJS_CNK_MODEL orange_lamp_model = {
#include "stg24/orange_lamp_art/orange_lamp_model.inc"
};

static NJS_CNK_OBJECT orange_lamp_object = {
#include "stg24/orange_lamp_art/orange_lamp_object.inc"
};
