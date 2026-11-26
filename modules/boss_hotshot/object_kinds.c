/* the kinds of object that the set files of Hot Shot's fight name */
#include "boss_hotshot/object_kinds/text.inc"
#include "boss_hotshot/object_kinds.h"
#include "object/set.h"

/* The generated SET roster names these actor constructors. */
#include "shared/boss_set_kinds.h"
#include "shared/dummy_object.h"
#include "shared/messenger.h"

struct object_kind hotshot_object_kinds[HOTSHOT_KIND_COUNT] = {
#include "boss_hotshot/object_kinds/hotshot_object_kinds.inc"
};
