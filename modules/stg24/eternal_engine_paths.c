/* STG24.PRS's path list, from the disc. its D lies between the section's
 * start and sky_tex.c */
#include "stage/path.h"
#include "stg24/eternal_engine.h"

struct path *eternal_engine_paths[1] = {
#include "stg24/eternal_engine_paths/eternal_engine_paths.inc"
};
