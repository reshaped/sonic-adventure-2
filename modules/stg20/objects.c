/* The SET roster for the first walker duel. */
#define CARRIER_DUEL
#include "object/set.h"
#include "shared/carrier_objects.h"
#include "shared/carrier_set_constructors.h" /* IWYU pragma: keep -- names the bodies take */
#include "stg20/objects/text.inc"

struct object_kind carrier_kinds[CARRIER_DUEL_KIND_COUNT] = {
#include "stg20/objects/carrier_kinds.inc"
};
