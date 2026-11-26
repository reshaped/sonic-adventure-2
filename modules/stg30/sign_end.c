/* Bobbing end sign. The stage owns the lighting parameters and collision
 * sphere. */

#include <ninja.h>

#include "shared/sign_end_art.h"
#include "stg40/sign_end.h"

/* the SET editor's captions. only sign_end_task.h's unreached block names
 * them, so that SHC places them in this source's constants */
#define SIGN_END_EDITOR_ANGLE_X "<- ANGLE X"
#define SIGN_END_EDITOR_ANGLE_Y "<- ANGLE Y"
#define SIGN_END_EDITOR_ANGLE_Z "<- ANGLE Z"
#define SIGN_END_EDITOR_MODEL "<- MODEL"

NJS_TEXLIST *sign_end_texlists[3] = {
#include "stg30/sign_end/sign_end_texlists.inc"
};

#define FINAL_RUSH_SIGN
#include "shared/orbital/sign_end_task.h" /* IWYU pragma: keep -- the code it compiles here */
