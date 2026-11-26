#ifdef NONMATCHING
/* NULL, which this side's garden_race_course returns */
#include <sg_xpt.h>
#endif

#include "chao/race.h"

/* a beginner or jewel course runs one track at every level. only its last
 * level gives an award */
static struct garden_race_course beginner_races[RACE_BEGINNER_COURSES]
                                               [RACE_BEGINNER_LEVELS] = {
#include "chao/race_courses/beginner_races.inc"
};
static struct garden_race_course jewel_races[RACE_JEWEL_COURSES]
                                            [RACE_JEWEL_LEVELS] = {
#include "chao/race_courses/jewel_races.inc"
};

/* clang-format off */
struct garden_race_course challenge_races[RACE_CHALLENGE_LEVELS] = {
#include "chao/race_courses/challenge_races.inc"
};
struct garden_race_course dark_land_races[RACE_ALIGNMENT_LEVELS] = {
#include "chao/race_courses/dark_land_races.inc"
};
struct garden_race_course hero_land_races[RACE_ALIGNMENT_LEVELS] = {
#include "chao/race_courses/hero_land_races.inc"
};
/* clang-format on */

const struct garden_race_course *garden_race_course(void)
{
    const struct garden_race_course *course;

    /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
    switch (garden_race_group()) {
    case RACE_BEGINNER:
        course = &beginner_races[garden_race_event()][garden_race_level()];
        break;
    case RACE_JEWEL:
        course = &jewel_races[garden_race_event() - RACE_BEGINNER_COURSES]
                             [garden_race_level()];
        break;
    case RACE_CHALLENGE:
        course = &challenge_races[garden_race_level()];
        break;
    case RACE_DARK_COURSES:
        course = &dark_land_races[garden_race_level()];
        break;
    case RACE_HERO_COURSES:
        course = &hero_land_races[garden_race_level()];
        break;
    case RACE_MULTI:
        if (garden_race_event() < RACE_BEGINNER_COURSES) {
            course = beginner_races[garden_race_event()];
        } else {
            course = jewel_races[garden_race_event() - RACE_BEGINNER_COURSES];
        }
        break;
        /* @bug uninitialized: returns course unset for any other group:
         * 0x0c51618a branches to the return at 0x0c516222 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: any other group returns null */
    default:
        return NULL;
#endif
    }
    return course;
    /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
}
