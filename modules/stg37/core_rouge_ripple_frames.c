/* STG37.PRS's chunk model pointers and chunk object pointers, from the disc.
 * its D lies between background_textures.c and core_scenery_art.c */
#include <ninja.h>

#include "stg37/water_cover.h"
#include "stg37/water_poses.h"
#include "stg37/water_ripple_meshes.h"
#include "stg37/water_scenery.h"

NJS_CNK_MODEL *cover_ripple_frames_a[COVER_RIPPLE_FRAMES] = {
#include "stg37/core_rouge_ripple_frames/cover_ripple_frames_a.inc"
};
NJS_CNK_MODEL *cover_ripple_frames_b[COVER_RIPPLE_FRAMES] = {
#include "stg37/core_rouge_ripple_frames/cover_ripple_frames_b.inc"
};
NJS_CNK_OBJECT *surface_frames[SURFACE_FRAMES] = {
#include "stg37/core_rouge_ripple_frames/surface_frames.inc"
};
NJS_CNK_MODEL *rotor_water_frames[ROTOR_WATER_FRAMES] = {
#include "stg37/core_rouge_ripple_frames/rotor_water_frames.inc"
};
