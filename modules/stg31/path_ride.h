/* Green Hill's rides, and the paths they carry players along */
#ifndef STG31_PATH_RIDE_H
#define STG31_PATH_RIDE_H

#include "stage/path.h"

struct task;

extern struct path *green_hill_paths[];
extern struct path green_hill_ride_z;
extern struct path green_hill_ride_x_upper;
extern struct path green_hill_ride_x_lower;

/* make a ride that carries players along path, as a child of parent. a player
 * who comes near path's last point rides back along it to the first point. the
 * player leaves there along the line from the second point through the first.
 * returns the ride, or null where task_create_child fails.
 *
 * the ride keeps path and reads it each step, so path must outlive the ride.
 * path's flags must be PATH_LAYOUT_POS alone, and it must have three points or
 * more. each point must lie apart from the next. so must the first point from
 * the third, and the last from the third from last */
struct task *green_hill_path_ride_create(struct path *path,
                                         struct task *parent);

#endif
