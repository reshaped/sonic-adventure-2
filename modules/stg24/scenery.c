#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "shared/common_object.h"
#include "stg24/scenery.h"

/* each scroll's uvs are one key, as texture_anim.h lays them out: an entry
 * for each strip. a scroll moves v 1 a frame, and eternal_engine.c plays
 * engine_scroll_speed frames a step */

static Sint16 scroll_uvs_0[16] = {
#include "stg24/scenery/scroll_uvs_0.inc"
};

static struct texture_anim scroll_0 = {
#include "stg24/scenery/scroll_0.inc"
};

static Sint16 scroll_uvs_1[42] = {
#include "stg24/scenery/scroll_uvs_1.inc"
};

static struct texture_anim scroll_1 = {
#include "stg24/scenery/scroll_1.inc"
};

static Sint16 scroll_uvs_2[52] = {
#include "stg24/scenery/scroll_uvs_2.inc"
};

static struct texture_anim scroll_2 = {
#include "stg24/scenery/scroll_2.inc"
};

static Sint16 scroll_uvs_3[42] = {
#include "stg24/scenery/scroll_uvs_3.inc"
};

static struct texture_anim scroll_3 = {
#include "stg24/scenery/scroll_3.inc"
};

static Sint16 scroll_uvs_4[34] = {
#include "stg24/scenery/scroll_uvs_4.inc"
};

static struct texture_anim scroll_4 = {
#include "stg24/scenery/scroll_4.inc"
};

static Sint16 scroll_uvs_5[32] = {
#include "stg24/scenery/scroll_uvs_5.inc"
};

static struct texture_anim scroll_5 = {
#include "stg24/scenery/scroll_5.inc"
};

static Sint16 scroll_uvs_6[44] = {
#include "stg24/scenery/scroll_uvs_6.inc"
};

static struct texture_anim scroll_6 = {
#include "stg24/scenery/scroll_6.inc"
};

static Sint16 scroll_uvs_7[16] = {
#include "stg24/scenery/scroll_uvs_7.inc"
};

static struct texture_anim scroll_7 = {
#include "stg24/scenery/scroll_7.inc"
};

static Sint16 scroll_uvs_8[16] = {
#include "stg24/scenery/scroll_uvs_8.inc"
};

static struct texture_anim scroll_8 = {
#include "stg24/scenery/scroll_8.inc"
};

static Sint16 scroll_uvs_9[126] = {
#include "stg24/scenery/scroll_uvs_9.inc"
};

static struct texture_anim scroll_9 = {
#include "stg24/scenery/scroll_9.inc"
};

struct scroll_entry engine_scrolls[10] = {
#include "stg24/scenery/engine_scrolls.inc"
};
Uint32 engine_scroll_speed = 4;
Bool engine_sky_built = FALSE;
struct model_build engine_sky_builds[2] = {
#include "stg24/scenery/engine_sky_builds.inc"
};
/* the skies' turns a step: the pitch of both about x, and the outer sky's
 * spin about y */
Angle engine_sky_pitch = 1;
Angle engine_outer_sky_spin = 2;
/* the inner sky's own pitch, which no code reads: engine_sky_pitch pitches
 * both */
static Angle unused_inner_sky_pitch = 2;
Angle engine_inner_sky_spin = 4;
