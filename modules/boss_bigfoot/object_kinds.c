/* the kinds of object that the set files of Big Foot's fight name */
#include "boss_bigfoot/object_kinds/text.inc"
#include "boss_bigfoot/object_kinds.h"
#include "object/set.h"

/* The generated SET roster names these actor constructors. */
#include "shared/boss_set_kinds.h"
#include "boss_bigfoot/prop.h"
#include "boss_bigfoot/signboard.h"
#include "shared/glow_light.h"
#include "shared/messenger.h"
#include "shared/parked_car.h"
#include "shared/plain_tree.h"

struct object_kind bigfoot_object_kinds[BIGFOOT_KIND_COUNT] = {
#include "boss_bigfoot/object_kinds/bigfoot_object_kinds.inc"
};
