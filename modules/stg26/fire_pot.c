/* The FIREPOT object in Egg Quarters. */
#define EGG_QUARTERS
#define FIRE_POT(name) quarters_fire_pot_##name

#include <ninja.h>

#include "stg26/fire_pot/text.inc"
#include "stg25/fire_pot_art.h"

NJS_TEXLIST *quarters_fire_pot_texlists[4] = {
#include "stg26/fire_pot/quarters_fire_pot_texlists.inc"
};

#include "stg25/fire_pot_impl.h" /* IWYU pragma: keep -- the code it compiles here */
