/* Pulse timing, stream colors and floor-glow tuning for both routes. */
int core_strip_rise = 15;
int core_strip_fall = 45;
int core_strip_delay = 12;
Float core_strip_red = 1.0F;
Float core_strip_green = 0.3F;
Float core_strip_blue = 0.3F;
Float core_strip_reverse_red = 1.0F;
Float core_strip_reverse_green = 0.3F;
Float core_strip_reverse_blue = 0.3F;

#ifndef CORE_TAILS_LIGHTS
int eggman_lamp_rise = 15;
int eggman_lamp_fall = 45;
int eggman_lamp_delay = 12;
Float eggman_lamp_red = 1.0F;
Float eggman_lamp_green = 0.0F;
Float eggman_lamp_blue = 0.25F;
Uint32 eggman_glass_tint = 0x80ffffff;
Uint32 eggman_motion_tint = 0x60606060;
#endif

Uint32 core_glow_center_color = 0xffffffff;
Uint32 core_glow_edge_color = 0x0080ffff;
Float core_glow_uv_scale = 0.2F;
int core_stream_trails = 8;
int core_stream_trail_step = 5;
Float core_stream_glow_gain = 0.1F;
int core_stream_color_speed = 4;
Float core_segment_base_light = 0.2F;
