#include <ninja.h>

#include "shared/engine_step.h"

#include "shared/step_textures.h"

/* Each original name starts on a four-byte boundary. */
const char step_et64_3[8] = {
#include "stg24/step_tex/step_et64_3.inc"
};

const char step_miu128_cg005[16] = {
#include "stg24/step_tex/step_miu128_cg005.inc"
};

const char step_miu128_cg016[16] = {
#include "stg24/step_tex/step_miu128_cg016.inc"
};

const char step_miu64_cg003[12] = {
#include "stg24/step_tex/step_miu64_cg003.inc"
};

const char step_miu64_cg005[12] = {
#include "stg24/step_tex/step_miu64_cg005.inc"
};

const char step_miu64_cg006[12] = {
#include "stg24/step_tex/step_miu64_cg006.inc"
};

const char step_miu64_cg007[12] = {
#include "stg24/step_tex/step_miu64_cg007.inc"
};

const char step_miu64_cg010[12] = {
#include "stg24/step_tex/step_miu64_cg010.inc"
};

const char step_miu64_cg020[12] = {
#include "stg24/step_tex/step_miu64_cg020.inc"
};

#include "shared/steps/deck_texture_names.h"

NJS_TEXLIST step_texlist = {textures, sizeof(textures) / sizeof(textures[0])};
