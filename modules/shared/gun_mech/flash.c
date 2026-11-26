/* a flash of colored vertices, which no code reads */
#include <ninja.h>
#include <sg_xpt.h>

static Sint16 colored_flash_polygons[37] = {
#include "shared/gun_mech/flash/colored_flash_polygons.inc"
};

static Sint32 colored_flash_vertices[75] = {
#include "shared/gun_mech/flash/colored_flash_vertices.inc"
};

static NJS_CNK_MODEL colored_flash_mesh = {
#include "shared/gun_mech/flash/colored_flash_mesh.inc"
};

static NJS_CNK_OBJECT colored_flash = {
#include "shared/gun_mech/flash/colored_flash.inc"
};
