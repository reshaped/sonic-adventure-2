#include <ninja.h>
#include "shared/carrier_textures.h"
#include "shared/sea.h"

const char carrier_sea_names[CARRIER_SEA_FRAMES][12] = {
#include "shared/carrier_sea_textures/carrier_sea_names.inc"
};
static NJS_TEXNAME sea_frames[20] = {
#include "shared/carrier_sea_textures/sea_frames.inc"
};
NJS_TEXLIST sea_texlist = {
#include "shared/carrier_sea_textures/sea_texlist.inc"
};

#include "shared/sea_mesh.h"

NJS_CNK_OBJECT sea_object[1] = {
#include "shared/carrier_sea_textures/sea_object.inc"
};
