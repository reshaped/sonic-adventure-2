#include <ninja.h>

#include "shared/core_clear_switch.h"
#include "shared/core_switch_parts.h"

struct task *core_clear_switch = NULL;
#include "shared/core_switch_textures/text.inc"

static NJS_TEXNAME switch_names[5] = {
#include "shared/core_switch_textures/switch_names.inc"
};
NJS_TEXLIST core_switch_texlist = {switch_names, sizeof(switch_names) /
                                                     sizeof(switch_names[0])};
