#include <sg_xpt.h>

#include "stg31/background.h"
#include "stg31/scenery_land.h"

struct terrain_scroll green_hill_terrain_scrolls[GREEN_HILL_SCROLL_MODELS] = {
#include "stg31/terrain_scroll/green_hill_terrain_scrolls.inc"
};

/* the pulsing tint swings between these ARGB colors. its sine's angle rises
 * green_hill_tint_rate a step */
Uint32 green_hill_tint_dark = 0xff101010;
Uint32 green_hill_tint_light = 0xffffffff;
Uint32 green_hill_tint_rate = 0x200;
/* the scrolling textures' phase rises green_hill_scroll_rate a step */
Sint16 green_hill_scroll_rate = 10;

/* final_chase.c's meteor settings, with the same values. no code reads them */
static Float meteor_chance = 0.05F;
static Float meteor_fall_speed = -50.0F;
static Float meteor_drift = 40.0F;
static Float meteor_reach = 3000.0F;
static Float meteor_spread = 500.0F;
static Float meteor_height = 2500.0F;
static Float meteor_drop = 5000.0F;
