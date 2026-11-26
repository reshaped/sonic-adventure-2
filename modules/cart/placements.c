#include "cart/course.h"
#include "cart/objects.h"
#include "cart/stage.h"
#include "object/set.h"
#include "shared/dummy_object.h"
#include "shared/light_switch.h"
#include "cart/placements/text.inc"

struct object_kind cart_kinds[13] = {
#include "cart/placements/cart_kinds.inc"
};
union cart_set_buffer cart_set;
struct cart_setup cart_setup;
