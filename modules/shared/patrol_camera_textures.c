#include <ninja.h>

/* The generated registry takes the addresses of both camera texture lists. */
#include "shared/patrol_camera_tex.h"
#include "shared/patrol_camera_textures.h"

NJS_TEXLIST *patrol_camera_texlists[3] = {
#include "shared/patrol_camera_textures/patrol_camera_texlists.inc"
};
