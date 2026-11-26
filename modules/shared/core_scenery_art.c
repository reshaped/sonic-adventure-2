/* the chunk model pointers of STG37.PRS and STG38.PRS, from the disc. its
 * D's neighbours differ by image */
#include <ninja.h>

#include "shared/core_scenery_art.h"
#include "shared/shaft_wall_art.h"

NJS_CNK_MODEL *inner_wall_frames[WALL_FRAMES] = {
#include "shared/core_scenery_art/inner_wall_frames.inc"
};
NJS_CNK_MODEL *middle_wall_frames[WALL_FRAMES] = {
#include "shared/core_scenery_art/middle_wall_frames.inc"
};
NJS_CNK_MODEL *outer_wall_frames[WALL_FRAMES] = {
#include "shared/core_scenery_art/outer_wall_frames.inc"
};
