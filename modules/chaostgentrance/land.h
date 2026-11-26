/* the race entrance hall's land, and its backdrop. entrance.c draws the
 * backdrop apart from the land, at 0.7 of the camera's position */
#ifndef CHAOSTGENTRANCE_LAND_H
#define CHAOSTGENTRANCE_LAND_H
#include <ninja.h>

#include "collision/world.h"

extern const char entrance_land_filenames[624];
extern NJS_TEXLIST entrance_land_texlist;
extern NJS_CNK_OBJECT entrance_backdrop;
extern struct land_table entrance_land;

#endif
