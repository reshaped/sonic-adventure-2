#include <ninja.h>

#include "shared/core_switch_parts.h"

#include "shared/core_switch_parts/text.inc"
#include "shared/core_switch/visible.h"
#include "shared/core_switch/collision.h"

NJS_OBJECT core_switch_collision = {
#include "shared/core_switch_parts/core_switch_collision.inc"
};
NJS_CNK_MODEL *core_switch_plates[1] = {
#include "shared/core_switch_parts/core_switch_plates.inc"
};
NJS_CNK_MODEL *core_switch_bases[1] = {
#include "shared/core_switch_parts/core_switch_bases.inc"
};
NJS_TEXLIST *core_switch_texlists[2] = {
#include "shared/core_switch_parts/core_switch_texlists.inc"
};
/* the set editor's names of the switch's kinds, which no code reads */
static char *switch_kinds[1] = {
#include "shared/core_switch_parts/switch_kinds.inc"
};
