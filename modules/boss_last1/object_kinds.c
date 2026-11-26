/* the kinds of object that the set files of the Biolizard's fight name */
#include "boss_last1/object_kinds/text.inc"
#include "object/set.h"
#include "boss_last1/object_kinds.h"

/* The generated SET roster names these actor constructors. */
#include "shared/boss_set_kinds.h"
#include "boss_last1/contacts.h"
#include "shared/big_the_cat.h"
#include "shared/lens_flare.h"
#include "shared/messenger.h"

struct object_kind last1_object_kinds[BIOLIZARD_KIND_COUNT] = {
#include "boss_last1/object_kinds/last1_object_kinds.inc"
};
