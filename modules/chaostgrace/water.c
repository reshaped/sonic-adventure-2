/* the race course's water. every surface draws with race_water_texlist, into
 * which each land loads its own artwork */
#include "chao/garden_scene.h"
#include "chaostgrace/course_models.h"
#include "chaostgrace/race.h"
#include "chaostgrace/scene.h"

#define WATER_SURFACES 12

/* the surfaces, and an entry whose null object ends them */
static struct garden_water race_water_surfaces[WATER_SURFACES + 1] = {
#include "chaostgrace/water/race_water_surfaces.inc"
};

void race_water_create(void)
{
    garden_water_create(race_water_surfaces);
}
