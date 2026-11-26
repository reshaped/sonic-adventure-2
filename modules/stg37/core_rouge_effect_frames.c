/* STG37.PRS's chunk model pointers, from the disc. its D lies between
 * core_scenery_art.c and water_scenery.c */
#include <ninja.h>

#include "stg37/water_poses.h"
#include "stg37/water_scenery.h"

NJS_CNK_MODEL *bubble_frames[BUBBLE_FRAMES] = {
#include "stg37/core_rouge_effect_frames/bubble_frames.inc"
};
NJS_CNK_MODEL *first_splash_frames[FIRST_SPLASH_FRAMES] = {
#include "stg37/core_rouge_effect_frames/first_splash_frames.inc"
};
NJS_CNK_MODEL *second_splash_frames[SECOND_SPLASH_FRAMES] = {
#include "stg37/core_rouge_effect_frames/second_splash_frames.inc"
};
NJS_CNK_MODEL *water_splash_frames[WATER_SPLASH_FRAMES] = {
#include "stg37/core_rouge_effect_frames/water_splash_frames.inc"
};
