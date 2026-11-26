/* the wall lamp's model. its glow is a mesh of its own, entrance_lamp_glow,
 * whose texture lamps.c steps through the blue lights */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgentrance/fixture_art.h"

static Sint16 lamp_glow_polygons[87] = {
#include "chaostgentrance/lamp_art/lamp_glow_polygons.inc"
};

static Sint32 lamp_glow_vertices[67] = {
#include "chaostgentrance/lamp_art/lamp_glow_vertices.inc"
};

NJS_CNK_MODEL entrance_lamp_glow = {
#include "chaostgentrance/lamp_art/entrance_lamp_glow.inc"
};

static NJS_CNK_OBJECT lamp_glow = {
#include "chaostgentrance/lamp_art/lamp_glow.inc"
};

static Sint16 lamp_fixture_polygons[607] = {
#include "chaostgentrance/lamp_art/lamp_fixture_polygons.inc"
};

static Sint32 lamp_fixture_vertices[547] = {
#include "chaostgentrance/lamp_art/lamp_fixture_vertices.inc"
};

static NJS_CNK_MODEL lamp_fixture_geometry = {
#include "chaostgentrance/lamp_art/lamp_fixture_geometry.inc"
};

NJS_CNK_OBJECT entrance_lamp_model = {
#include "chaostgentrance/lamp_art/entrance_lamp_model.inc"
};
