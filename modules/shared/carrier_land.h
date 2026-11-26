/* the land of the carrier stages: its table and its texture list.
 * carrier_land_filenames holds the file names that both point at */
#ifndef SHARED_CARRIER_LAND_H
#define SHARED_CARRIER_LAND_H
#include <ninja.h>

#include "collision/world.h"

extern struct land_table carrier_land;
extern const char carrier_land_filenames[381];
extern NJS_TEXLIST carrier_land_texlist;

#endif
