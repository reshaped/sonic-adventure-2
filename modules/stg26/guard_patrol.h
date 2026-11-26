/* Egg Quarters' guard: its path, texture lists, model build and tuning */
#ifndef STG26_GUARD_PATROL_H
#define STG26_GUARD_PATROL_H
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/common_object.h"
#include "stage/path.h"

extern NJS_CNK_OBJECT quarters_guard_object;
extern NJS_TEXLIST guard_texlist;
extern NJS_TEXLIST guard_lens_texlist;
extern NJS_TEXLIST guard_sensor_texlist;
extern NJS_TEXLIST guard_exhaust_texlist;

extern struct path guard_path;
extern Float guard_patrol_speed;
extern NJS_TEXLIST *guard_texlists[];

extern Sint32 guard_lock_steps;
extern Sint32 guard_attack_volume;
extern Sint32 guard_engine_volume;
extern Sint32 guard_lock_volume;
extern Sint32 guard_radar_volume;
extern Sint32 guard_radar_period;
extern Float guard_pitch_weight;
extern Float guard_yaw_weight;
extern Float guard_aim_weight;
extern Float guard_stop_distance;

extern Float guard_view_distance;
extern Float guard_lock_scale;
extern Float guard_shadow_y;
extern Bool guard_built;
extern struct model_build guard_model_builds[];

#endif
