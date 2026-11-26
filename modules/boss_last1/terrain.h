#ifndef BOSS_LAST1_TERRAIN_H
#define BOSS_LAST1_TERRAIN_H
#include <ninja.h>

#include "collision/world.h"

/* the chamber's water. biolizard_water_draw sets its texture id to one of 16
 * frames by the step count, then draws it */
extern NJS_CNK_OBJECT biolizard_water_object;
extern struct land_table biolizard_land;
extern const char biolizard_land_filenames[306];

#endif
