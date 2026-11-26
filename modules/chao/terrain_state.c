/* the flag garden_enable_objects and garden_disable_objects set, and the
 * height map. the map holds one sample, but its reader indexes 401 by 401 */
#include <sg_xpt.h>

#include "chao/body_collision.h"
#include "chao/ground.h"

Bool garden_objects_enabled;
Uint16 garden_height_map[GARDEN_HEIGHT_SIDE * GARDEN_HEIGHT_SIDE];
