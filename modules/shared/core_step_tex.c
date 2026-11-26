#include <ninja.h>

#include "shared/core_step.h"

#include "shared/step_textures.h"

/* each array rounds its name up to a multiple of four bytes */
const char step_et64_3[8] = "et64_3";

const char step_miu128_cg005[16] = "miu128_cg005";

const char step_miu128_cg016[16] = "miu128_cg016";

const char step_miu64_cg003[12] = "miu64_cg003";

const char step_miu64_cg005[12] = "miu64_cg005";

const char step_miu64_cg006[12] = "miu64_cg006";

const char step_miu64_cg007[12] = "miu64_cg007";

const char step_miu64_cg010[12] = "miu64_cg010";

const char step_miu64_cg020[12] = "miu64_cg020";

#include "shared/steps/deck_texture_names.h"

NJS_TEXLIST core_step_texlist = {textures,
                                 sizeof(textures) / sizeof(textures[0])};
