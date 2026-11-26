/* the stopper's texture list, model and solid, as stopper_model.h declares
 * them */
#include <ninja.h>

#include "shared/stopper_art/text.inc"
#include "shared/stopper_model.h"

static NJS_TEXNAME stopper_textures[3] = {
#include "shared/stopper_art/stopper_textures.inc"
};

NJS_TEXLIST stopper_texlist = {
#include "shared/stopper_art/stopper_texlist.inc"
};

#include "shared/stopper_mesh.h"

NJS_CNK_OBJECT stopper_object[1] = {
#include "shared/stopper_art/stopper_object.inc"
};

#include "shared/stopper_solid.h"

NJS_OBJECT stopper_collision = {
#include "shared/stopper_art/stopper_collision.inc"
};
