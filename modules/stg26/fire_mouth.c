/* The MOUTH object in Egg Quarters. */
#define EGG_QUARTERS
#define FIRE_MOUTH(name) quarters_fire_mouth_##name

#include <ninja.h>

#include "stg26/fire_mouth/text.inc"
#include "stg25/fire_mouth_art.h"

NJS_TEXLIST *quarters_fire_mouth_texlists[4] = {
#include "stg26/fire_mouth/quarters_fire_mouth_texlists.inc"
};

#include "stg25/fire_mouth_impl.h" /* IWYU pragma: keep -- the code it compiles here */
