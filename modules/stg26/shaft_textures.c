#include <ninja.h>

#include "shared/shaft.h"
#include "stg26/shaft_textures/text.inc"

static NJS_TEXNAME textures[2] = {
#include "stg26/shaft_textures/textures.inc"
};

NJS_TEXLIST shaft_texlist = {
#include "stg26/shaft_textures/shaft_texlist.inc"
};

#include "shared/shaft_mesh.h"

NJS_CNK_OBJECT shaft_object = {
#include "stg26/shaft_textures/shaft_object.inc"
};

#include "shared/shaft_motion_keys.h"

NJS_MOTION shaft_motion[1] = {
#include "stg26/shaft_textures/shaft_motion.inc"
};
