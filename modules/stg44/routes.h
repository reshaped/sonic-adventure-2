/* Mad Space's paths: the carriages' and the platform's, which the carriage
 * manager hands out. mad_space_paths, for path_create_manager, holds only the
 * null that ends it */
#ifndef STG44_ROUTES_H
#define STG44_ROUTES_H

#include "stage/path.h"

extern struct path mad_space_first_carriage_path;
extern struct path mad_space_second_carriage_path;
extern struct path mad_space_platform_path;
extern struct path *mad_space_paths[];
#endif
