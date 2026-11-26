#include <ninja.h>
#include <sg_xpt.h>

#include "shared/carrier_stage.h"
#include "shared/carrier_textures.h"

const char carrier_sky_clouds[12] = "prnbg8_1";
const char carrier_sky_horizon[8] = "prnbg_2";
const char carrier_background_pvm[8] = "bgtex12";

static NJS_TEXNAME sky_names[2] = {
#include "shared/carrier_sky_textures/sky_names.inc"
};

NJS_TEXLIST carrier_sky_texlist = {
#include "shared/carrier_sky_textures/carrier_sky_texlist.inc"
};

static Sint16 sky_polygons[823] = {
#include "shared/carrier_sky_textures/sky_polygons.inc"
};

static Sint32 sky_vertices[459] = {
#include "shared/carrier_sky_textures/sky_vertices.inc"
};

static NJS_CNK_MODEL sky_model = {
#include "shared/carrier_sky_textures/sky_model.inc"
};

NJS_CNK_OBJECT carrier_sky[1] = {
#include "shared/carrier_sky_textures/carrier_sky.inc"
};
