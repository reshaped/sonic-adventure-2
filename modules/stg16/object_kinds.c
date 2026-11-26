/* The two Wild Canyon stages share their SET order and constructor names. */
#include "object/set.h"
#include "stg16/object_kinds.h"
#include "stg16/set_constructors.h" /* IWYU pragma: keep -- names the bodies take */

#include "stg16/object_kinds/text.inc"

struct object_kind wild_canyon_kinds[WILD_CANYON_KIND_COUNT] = {
#include "stg16/object_kinds/wild_canyon_kinds.inc"
};
