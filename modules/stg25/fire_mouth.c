/* The MOUTH object in Death Chamber. */
#define FIRE_MOUTH(name) chamber_fire_mouth_##name

#include <ninja.h>

#include "stg25/fire_mouth.h"
#include "stg25/fire_mouth/text.inc"
#include "stg25/fire_mouth_art.h"

NJS_TEXLIST *chamber_fire_mouth_texlists[4] = {
#include "stg25/fire_mouth/chamber_fire_mouth_texlists.inc"
};

#include "stg25/fire_mouth_impl.h" /* IWYU pragma: keep -- the code it compiles here */
