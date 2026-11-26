/* the Chao race's groups and courses, and the selected race that the
 * entrance and the circuit share */
#ifndef CHAO_RACE_H
#define CHAO_RACE_H
#include <sg_xpt.h>

enum garden_race_group {
    RACE_BEGINNER,
    RACE_JEWEL,
    RACE_CHALLENGE,
    /* RACE_DARK_COURSES and RACE_HERO_COURSES are named for the land their
     * courses run in. neither takes a Chao of its land's alignment */
    RACE_DARK_COURSES,
    RACE_HERO_COURSES,
    RACE_MULTI,
    RACE_GROUPS
};

enum garden_race_land { RACE_LAND_NEUTRAL, RACE_LAND_HERO, RACE_LAND_DARK };
#define RACE_LANDS 3

#define RACE_BEGINNER_COURSES 4
#define RACE_JEWEL_COURSES 6
#define RACE_BEGINNER_LEVELS 3
#define RACE_JEWEL_LEVELS 5
#define RACE_CHALLENGE_LEVELS 12
#define RACE_ALIGNMENT_LEVELS 4
#define RACE_NO_AWARD (-1)
/* bits of garden_save.awards, each named for its label on the garden
 * editor's award page. while a bit is set, one garden places its toy. no
 * garden places the piano or the organ */
#define RACE_AWARD_TELEVISION 0
#define RACE_AWARD_RADIO 1
#define RACE_AWARD_JACK_IN_BOX 2
#define RACE_AWARD_NEUTRAL_BALL 3
#define RACE_AWARD_HERO_BALL 4
#define RACE_AWARD_DARK_BALL 5
#define RACE_AWARD_ROCKING_HORSE 6
#define RACE_AWARD_PIANO 10
#define RACE_AWARD_ORGAN 11

struct garden_race_course {
    Sint32 opponents; /* -1 selects the course's usual field */
    Sint32 land;
    Sint32 track;
    Sint32 camera_track;
    Sint32 award; /* RACE_NO_AWARD or a bit number in the garden save */
};

extern struct garden_race_course challenge_races[RACE_CHALLENGE_LEVELS];
extern struct garden_race_course dark_land_races[RACE_ALIGNMENT_LEVELS];
extern struct garden_race_course hero_land_races[RACE_ALIGNMENT_LEVELS];

/* the course of the selected group, event and level, which must index that
 * group's tables, unchecked. RACE_MULTI takes the first level of its
 * beginner or jewel course */
const struct garden_race_course *garden_race_course(void);

int garden_race_group(void);
int garden_race_event(void);
int garden_race_level(void);

#endif
