#include <ninja.h>
#include <sg_xpt.h>

#include "stg30/space_motion.h"

Float spark_size = 3.0F;
Float spark_size_range = 3.5F;
Float meteor_fall_speed = -50.0F;
Float meteor_drift = 40.0F;
Float meteor_reach = 3000.0F;
Float meteor_spread = 500.0F;
Float meteor_height = 2500.0F;
Float meteor_drop = 5000.0F;
Float meteor_chance = 0.05F;
Sint16 sky_scroll_rate = 35;
Uint32 sky_pulse_dim = 0xff101010;
Uint32 sky_pulse_bright = 0xffffffff;
Angle sky_pulse_rate = 512;
Float fragment_sound_chance = 0.05F;
Sint32 fragment_sound_volumes[3] = {50, 30, -10};
NJS_POINT3 rockfall_start = {2532.0F, -21343.0F, 11232.0F};
NJS_POINT3 rockfall_end = {2532.0F, -26571.0F, 8211.0F};
Angle orbit_128_pitch = 1;
Angle orbit_128_yaw = 2;
Angle orbit_64_pitch = 2;
Angle orbit_64_yaw = 4;
Float earth_height = 0.0F;
Float earth_scale = 2.0F;
Float stars_height = 0.0F;
Float stars_scale = 1.0F;
Float orbit_scale = 2.0F;
