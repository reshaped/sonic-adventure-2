/* the lands of the kindergarten's six rooms. each is a table of the room's
 * entries, which hold its collision and scenery */
#ifndef CHAOSTGKINDER_LAND_DATA_H
#define CHAOSTGKINDER_LAND_DATA_H
#include "collision/world.h"

extern struct land_entry kinder_house_entries[57], kinder_corridor_entries[28];
extern struct land_entry kinder_classroom_entries[23],
    kinder_health_center_entries[18];
extern struct land_entry kinder_library_entries[19],
    kinder_principal_entries[13];
extern struct land_table kinder_house_land, kinder_corridor_land;
extern struct land_table kinder_classroom_land, kinder_health_center_land;
extern struct land_table kinder_library_land, kinder_principal_land;

#endif
