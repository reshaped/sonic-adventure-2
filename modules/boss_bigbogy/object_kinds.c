/* the kinds of object that the set files of King Boom Boo's fight name */
#include "boss_bigbogy/object_kinds/text.inc"
#include "object/set.h"
#include "boss_bigbogy/object_kinds.h"

/* The generated SET roster names these actor constructors. */
#include "shared/boss_set_kinds.h"
#include "boss_bigbogy/door.h"
#include "shared/big_the_cat.h"
#include "shared/lens_flare.h"
#include "shared/messenger.h"
#include "shared/sandglass.h"

struct object_kind bigbogy_object_kinds[BIGBOGY_KIND_COUNT] = {
#include "boss_bigbogy/object_kinds/bigbogy_object_kinds.inc"
};
