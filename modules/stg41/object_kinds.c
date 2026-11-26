/* The two Wild Canyon stages share their SET order and constructor names. */
#include "object/set.h"
#include "stg41/object_kinds.h"
#include "stg16/set_constructors.h" /* IWYU pragma: keep -- names the bodies take */

#include "stg41/object_kinds/text.inc"

struct object_kind wild_canyon_2p_kinds[WILD_CANYON_2P_KIND_COUNT] = {
#include "stg41/object_kinds/wild_canyon_2p_kinds.inc"
};
