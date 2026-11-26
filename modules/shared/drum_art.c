/* the art of EECONT, rolling_drum.h's drum: its texture list and model */
#include <ninja.h>
#include <sg_xpt.h>
#include "shared/rolling_drum.h"
#include "shared/drum_art/text.inc"

static NJS_TEXNAME rolling_drum_texlist_names[9] = {
#include "shared/drum_art/rolling_drum_texlist_names.inc"
};

NJS_TEXLIST rolling_drum_texlist = {
#include "shared/drum_art/rolling_drum_texlist.inc"
};

static Sint16 rolling_drum_polygons[571] = {
#include "shared/drum_art/rolling_drum_polygons.inc"
};

static Sint32 rolling_drum_vertices[551] = {
#include "shared/drum_art/rolling_drum_vertices.inc"
};

static NJS_CNK_MODEL rolling_drum_model = {
#include "shared/drum_art/rolling_drum_model.inc"
};

NJS_CNK_OBJECT rolling_drum_object = {
#include "shared/drum_art/rolling_drum_object.inc"
};

NJS_TEXLIST *rolling_drum_texlists[2] = {
#include "shared/drum_art/rolling_drum_texlists.inc"
};
