/* the kinds of object that the set files of the Egg Golem's fight name. a reel
 * here is in range out to 10000, where BOSS_GOLEM_E.PRS keeps the default
 * 400 */
#include "boss_golem/object_kinds/text.inc"
#include "object/set.h"
#include "shared/golem_object_kinds.h"

/* The generated SET roster names these actor constructors. */
#include "shared/boss_set_kinds.h"
#include "shared/dummy_object.h"
#include "shared/golem_reel.h"
#include "shared/ugokidai.h"

struct object_kind golem_object_kinds[GOLEM_KIND_COUNT] = {
#include "boss_golem/object_kinds/golem_object_kinds.inc"
};
