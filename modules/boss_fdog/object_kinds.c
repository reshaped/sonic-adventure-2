/* the kinds of object that the set files of Flying Dog's fight name */
#include "boss_fdog/object_kinds/text.inc"
#include "boss_fdog/object_kinds.h"
#include "object/set.h"

/* The generated SET roster names these actor constructors. */
#include "shared/boss_set_kinds.h"
#include "shared/messenger.h"

struct object_kind fdog_object_kinds[FDOG_KIND_COUNT] = {
#include "boss_fdog/object_kinds/fdog_object_kinds.inc"
};
