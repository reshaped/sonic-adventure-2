/* the kinds of object that the set files of the Finalhazard's fight name */
#include "boss_last2/object_kinds/text.inc"
#include "object/set.h"
#include "boss_last2/object_kinds.h"

/* The generated SET roster names these actor constructors. */
#include "shared/big_the_cat.h"
#include "shared/effect_object.h"
#include "shared/goal_ring.h"
#include "shared/item_box.h"
#include "shared/item_box_air.h"
#include "shared/lens_flare.h"
#include "shared/light_switch.h"
#include "shared/modifier_volume.h"
#include "shared/ring.h"
#include "shared/ring_group.h"
#include "shared/spring.h"

struct object_kind last2_object_kinds[FINALHAZARD_KIND_COUNT] = {
#include "boss_last2/object_kinds/last2_object_kinds.inc"
};
