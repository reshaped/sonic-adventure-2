/* The low flag in the stage object list. Each wave frame has its own mesh and
 * texture slots. */
#include <ninja.h>

#include "shared/flag_low.h"
#include "shared/flag_frames.h"

static const char editor_angle_x[20] = "<- ANGLE X (   )";

static const char editor_angle_y[20] = "<- ANGLE Y (   )";

static const char editor_flag_color[24] = "<- even:Red odd:Blue";

static const char flag_cloth_filename[20] = {
#include "shared/flag_low/flag_cloth_filename.inc"
};

static const char flag_hook_filename[20] = {
#include "shared/flag_low/flag_hook_filename.inc"
};

static struct flag_low_frame flag_blue_frames[FLAG_FRAMES] = {
#include "shared/flag_low/flag_blue_frames.inc"
};

static struct flag_low_frame flag_red_frames[FLAG_FRAMES] = {
#include "shared/flag_low/flag_red_frames.inc"
};

static NJS_CNK_MODEL *flag_blue_models[FLAG_FRAMES] = {
#include "shared/flag_low/flag_blue_models.inc"
};

static NJS_CNK_MODEL *flag_red_models[FLAG_FRAMES] = {
#include "shared/flag_low/flag_red_models.inc"
};

NJS_TEXLIST *flag_low_blue_texlists[FLAG_FRAMES + 1] = {
#include "shared/flag_low/flag_low_blue_texlists.inc"
};

NJS_TEXLIST *flag_low_red_texlists[FLAG_FRAMES + 1] = {
#include "shared/flag_low/flag_low_red_texlists.inc"
};

/* NOLINTBEGIN(readability-identifier-naming) */
#define flag_init flag_low_init
#define flag_blue_texlists flag_low_blue_texlists
#define flag_red_texlists flag_low_red_texlists
/* NOLINTEND(readability-identifier-naming) */

#include "shared/flag_task.inc"
