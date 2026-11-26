/* the land of the Egg Golem's two fights: its table and texture list.
 * golem_land_filenames holds the file names that both point at */
#ifndef SHARED_GOLEM_LAND_H
#define SHARED_GOLEM_LAND_H

#include <ninja.h>

#include "collision/world.h"

extern NJS_TEXLIST golem_land_texlist;
extern struct land_table golem_land;
extern const char golem_land_filenames[127];

#endif
