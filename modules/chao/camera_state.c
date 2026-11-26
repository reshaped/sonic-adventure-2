/* garden_camera_fallback is the camera work, all 0, that the camera mode
 * reads while no camera body exists. the camera's start and reset clear
 * garden_camera_reset_flag, and no code reads it */
#include <sg_xpt.h>
#include "chao/garden_camera.h"

Sint32 garden_camera_reset_flag;
struct garden_camera garden_camera_fallback;
