/* Sand Ocean's pot and the five pieces it scatters when it breaks. */
#include <ninja.h>

#include "shared/debris.h"
#include "shared/ocean_pot.h"

static struct debris_part ocean_pot_pieces[OCEAN_POT_PIECES] = {
#include "shared/ocean_pot/ocean_pot_pieces.inc"
};
NJS_TEXLIST *ocean_pot_texlists[OCEAN_POT_PIECES + 2] = {
#include "shared/ocean_pot/ocean_pot_texlists.inc"
};

#include "shared/ocean_pot_impl.h" /* IWYU pragma: keep -- the code it compiles here */
