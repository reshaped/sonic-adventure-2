/* The FIREPOT object in Death Chamber. */
#define FIRE_POT(name) chamber_fire_pot_##name

#include <ninja.h>

#include "stg25/fire_pot.h"
#include "stg25/fire_pot/text.inc"
#include "stg25/fire_pot_art.h"

NJS_TEXLIST *chamber_fire_pot_texlists[4] = {
#include "stg25/fire_pot/chamber_fire_pot_texlists.inc"
};

#include "stg25/fire_pot_impl.h" /* IWYU pragma: keep -- the code it compiles here */
