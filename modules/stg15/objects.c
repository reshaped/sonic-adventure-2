/* The SET roster for two-player Weapons Bed. */
#define CARRIER_2P
#include "object/set.h"
#include "shared/carrier_objects.h"
#include "shared/carrier_set_constructors.h" /* IWYU pragma: keep -- names the bodies take */
#include "stg15/objects/text.inc"

struct object_kind carrier_kinds[WEAPONS_BED_2P_KIND_COUNT] = {
#include "stg15/objects/carrier_kinds.inc"
};
