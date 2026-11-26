/* The SET roster for Weapons Bed. */
#include "object/set.h"
#include "shared/carrier_objects.h"
#include "shared/carrier_set_constructors.h" /* IWYU pragma: keep -- names the bodies take */
#include "shared/carrier_objects/text.inc"

struct object_kind carrier_kinds[WEAPONS_BED_KIND_COUNT] = {
#include "shared/carrier_objects/carrier_kinds.inc"
};
