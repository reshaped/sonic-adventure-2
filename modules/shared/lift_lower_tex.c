#include <ninja.h>

#include "shared/core_block.h"
#include "shared/lift_textures.h"

/* the names that a texture list's textures point at, each starting on a 4-byte
 * boundary */
const char core_lift_cg001[16] = "miu128_cg001";
const char core_lift_cg022[16] = "miu128_cg022";
const char core_lift_cc002[16] = "miu256_cc002";
const char core_lift_poison[15] = "tx_ccpoison001";

NJS_TEXLIST core_lift_lower_texlist = {textures,
                                       sizeof(textures) / sizeof(textures[0])};
