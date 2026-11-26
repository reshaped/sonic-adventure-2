#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_scenery_art.h"

#include "shared/core_shaft_rotor/text.inc"

const char shaft_rotor_texture_name[12] = TEXT_1;

/* the texture lists, chunk poses and chunk models of STG37.PRS and
 * STG38.PRS, from the disc. its D's neighbours differ by image */
struct panels_pose {
    Sint16 polygons[263];
    Sint32 vertices[247];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static NJS_TEXNAME panel_texture[1];
NJS_TEXLIST rotor_textures;
static struct panels_pose panels[7];

static NJS_TEXNAME panel_texture[1] = {
#include "shared/core_shaft_rotor/panel_texture.inc"
};
NJS_TEXLIST rotor_textures = {
#include "shared/core_shaft_rotor/rotor_textures.inc"
};
static struct panels_pose panels[7] = {
#include "shared/core_shaft_rotor/panels.inc"
};
NJS_CNK_OBJECT shaft_rotor = {
#include "shared/core_shaft_rotor/shaft_rotor.inc"
};
