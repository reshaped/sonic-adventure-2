#include <ninja.h>

#include "shared/last_step.h"

/* a sized array pads its name, so the name after it starts on a 4-byte
 * boundary */
static const char miu128_cg005[16] = {
#include "stg27/last_step_tex/miu128_cg005.inc"
};
static const char miu128_cg009[16] = {
#include "stg27/last_step_tex/miu128_cg009.inc"
};
static const char miu128_cg016[16] = {
#include "stg27/last_step_tex/miu128_cg016.inc"
};
static const char miu128_cg017[16] = {
#include "stg27/last_step_tex/miu128_cg017.inc"
};
static const char miu256_cg001[16] = {
#include "stg27/last_step_tex/miu256_cg001.inc"
};
static const char miu64_cg003[12] = {
#include "stg27/last_step_tex/miu64_cg003.inc"
};
static const char miu64_cg005[12] = {
#include "stg27/last_step_tex/miu64_cg005.inc"
};
static const char miu64_cg006[12] = {
#include "stg27/last_step_tex/miu64_cg006.inc"
};
static const char miu64_cg007[12] = {
#include "stg27/last_step_tex/miu64_cg007.inc"
};
static const char miu64_cg009[12] = {
#include "stg27/last_step_tex/miu64_cg009.inc"
};
static const char miu64_cg020[12] = {
#include "stg27/last_step_tex/miu64_cg020.inc"
};
static const char miu64_cg021[12] = {
#include "stg27/last_step_tex/miu64_cg021.inc"
};
static const char miu64_ee001[12] = {
#include "stg27/last_step_tex/miu64_ee001.inc"
};

static NJS_TEXNAME textures[13] = {
#include "stg27/last_step_tex/textures.inc"
};

NJS_TEXLIST last_step_texlist = {
#include "stg27/last_step_tex/last_step_texlist.inc"
};
