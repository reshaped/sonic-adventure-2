#include <ninja.h>

#include "shared/machinery_assets.h"
#include "stg25/mecha.h"

static NJS_CNK_MODEL *models[MACHINERY_KINDS] = {
#include "stg25/mecha/models.inc"
};
NJS_TEXLIST *chamber_mecha_texlists[MACHINERY_KINDS + 1] = {
#include "stg25/mecha/chamber_mecha_texlists.inc"
};

#define MACHINERY_INIT chamber_mecha_init
#define MACHINERY_TEXLISTS chamber_mecha_texlists
#include "shared/machinery_prop.h" /* IWYU pragma: keep -- the code it compiles here */
