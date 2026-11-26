#include <ninja.h>

#include "shared/dynamite_bridge.h"

/* Each texture name starts at its original four-byte boundary. */
static const char et64_3[8] = {
#include "shared/bridge_tex/et64_3.inc"
};

static const char et64_4[8] = {
#include "shared/bridge_tex/et64_4.inc"
};

static const char miu128_cg022[16] = {
#include "shared/bridge_tex/miu128_cg022.inc"
};

static const char miu512_cg001[16] = {
#include "shared/bridge_tex/miu512_cg001.inc"
};

static const char miu512_cg003[16] = {
#include "shared/bridge_tex/miu512_cg003.inc"
};

static const char miu64_cg007[12] = {
#include "shared/bridge_tex/miu64_cg007.inc"
};

static const char miu64_cg010[12] = {
#include "shared/bridge_tex/miu64_cg010.inc"
};

static NJS_TEXNAME textures[7] = {
#include "shared/bridge_tex/textures.inc"
};

NJS_TEXLIST dynamite_bridge_texlist = {
#include "shared/bridge_tex/dynamite_bridge_texlist.inc"
};
