#include <ninja.h>
#include <sg_xpt.h>

#include "shared/common_object.h"
#include "stage/path.h"
#include "stg26/guard_patrol.h"

/* the guard's patrol, 301 points. patrol_path_move rings points 0 to 299,
 * and never reads the last */
static NJS_POINT3 guard_path_points[301] = {
#include "stg26/guard_patrol/guard_path_points.inc"
};

struct path guard_path = {
#include "stg26/guard_patrol/guard_path.inc"
};
Float guard_patrol_speed = -0.7F;
NJS_TEXLIST *guard_texlists[5] = {
#include "stg26/guard_patrol/guard_texlists.inc"
};
Sint32 guard_lock_steps = 150;
Sint32 guard_attack_volume = 25;
Sint32 guard_engine_volume = 50;
Sint32 guard_lock_volume = 22;
Sint32 guard_radar_volume = 100;
Sint32 guard_radar_period = 30;
Float guard_pitch_weight = 0.05F;
Float guard_yaw_weight = 0.025F;
Float guard_aim_weight = 0.1F;
Float guard_stop_distance = 35.0F;
Float guard_view_distance = 500.0F;
extern Float guard_lock_speed_scale;
Float guard_lock_scale = 7.0F;
Float guard_lock_speed_scale = 3.0F;
Float guard_shadow_y = -14.0F;
Bool guard_built = FALSE;
struct model_build guard_model_builds[1] = {
#include "stg26/guard_patrol/guard_model_builds.inc"
};
