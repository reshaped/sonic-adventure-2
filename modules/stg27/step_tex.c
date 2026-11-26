#include <ninja.h>

#include "shared/engine_step.h"

/* a sized array pads its name, so the name after it starts on a 4-byte
 * boundary */
static const char et64_3[8] = {
#include "stg27/step_tex/et64_3.inc"
};
static const char miu128_cg005[16] = {
#include "stg27/step_tex/miu128_cg005.inc"
};
static const char miu128_cg016[16] = {
#include "stg27/step_tex/miu128_cg016.inc"
};
static const char miu256_cg001[16] = {
#include "stg27/step_tex/miu256_cg001.inc"
};
static const char miu64_cg005[12] = {
#include "stg27/step_tex/miu64_cg005.inc"
};
static const char miu64_cg006[12] = {
#include "stg27/step_tex/miu64_cg006.inc"
};
static const char miu64_cg007[12] = {
#include "stg27/step_tex/miu64_cg007.inc"
};
static const char miu64_cg010[12] = {
#include "stg27/step_tex/miu64_cg010.inc"
};
static const char miu64_cg020[12] = {
#include "stg27/step_tex/miu64_cg020.inc"
};

static NJS_TEXNAME textures[9] = {
#include "stg27/step_tex/textures.inc"
};

NJS_TEXLIST step_texlist = {
#include "stg27/step_tex/step_texlist.inc"
};
