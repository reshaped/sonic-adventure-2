#include "chao/garden_field.h"
#include "chaostgrace/garden.h"
#include "chaostgrace/props.h"
#include "object/set.h"
#include "shared/collision_object.h"
#include "chaostgrace/objects/text.inc"

struct object_kind chaostgrace_kinds[12] = {
#include "chaostgrace/objects/chaostgrace_kinds.inc"
};
