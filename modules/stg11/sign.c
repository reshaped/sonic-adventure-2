/* Iron Gate culls its pulsing sign before drawing. */
#include <ninja.h>

#include "stg11/sign.h"

#define VIEW_RADIUS 25.0F

/* NOLINTBEGIN(readability-identifier-naming) */
#define sign_init iron_gate_sign_init
/* NOLINTEND(readability-identifier-naming) */
#define SIGN_VIEW_TEST(work)                                                   \
    if (!sphere_in_front_no_depth(&(work)->pos, VIEW_RADIUS)) {                \
        return;                                                                \
    }

#include "stg11/sign/text.inc"

#define SIGN_EDITOR_ROT_X "<- ROT X"
#define SIGN_EDITOR_ROT_Y "<- ROT Y"
#define SIGN_EDITOR_ROT_Z "<- ROT Z"

#include "shared/prison_machinery/sign_task.h"

#include "shared/prison_machinery/sign_art.h"

NJS_TEXLIST *iron_gate_sign_texlists[2] = {
#include "stg11/sign/iron_gate_sign_texlists.inc"
};
