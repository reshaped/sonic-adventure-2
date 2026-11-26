#ifndef STG30_SPACE_MOTION_H
#define STG30_SPACE_MOTION_H
#include <ninja.h>
#include <sg_xpt.h>

extern Float spark_size;
extern Float spark_size_range;
extern Float meteor_fall_speed;
extern Float meteor_drift;
extern Float meteor_reach;
extern Float meteor_spread;
extern Float meteor_height;
extern Float meteor_drop;
extern Float meteor_chance;
extern Sint16 sky_scroll_rate;
extern Uint32 sky_pulse_dim;
extern Uint32 sky_pulse_bright;
extern Angle sky_pulse_rate;
extern Float fragment_sound_chance;
extern Sint32 fragment_sound_volumes[3];
extern NJS_POINT3 rockfall_start;
extern NJS_POINT3 rockfall_end;
extern Angle orbit_128_pitch;
extern Angle orbit_128_yaw;
extern Angle orbit_64_pitch;
extern Angle orbit_64_yaw;
extern Float earth_height;
extern Float earth_scale;
extern Float stars_height;
extern Float stars_scale;
extern Float orbit_scale;

#endif
