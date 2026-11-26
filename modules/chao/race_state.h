/* the selected race, its entrants and clock, and the race progress the
 * garden save keeps */
#ifndef CHAO_RACE_STATE_H
#define CHAO_RACE_STATE_H
#include <sg_xpt.h>

#include "chao/race.h"
#include "chao/race_record.h"

struct chao_save;

/* a group's status in the garden save. a group open from the start is
 * RACE_GROUP_OPEN. garden_race_unlock_group makes a hidden group
 * RACE_GROUP_NEW, and garden_race_visit_group makes any group
 * RACE_GROUP_VISITED */
enum garden_race_group_status {
    RACE_GROUP_HIDDEN,
    RACE_GROUP_OPEN,
    RACE_GROUP_NEW,
    RACE_GROUP_VISITED
};

/* the events the garden save keeps levels for: the beginner courses, the
 * jewel courses, then these */
enum garden_race_event {
    RACE_CHALLENGE_EVENT = RACE_BEGINNER_COURSES + RACE_JEWEL_COURSES,
    RACE_DARK_EVENT,
    RACE_HERO_EVENT
};

struct garden_race_save {
    Sint8 group_status[RACE_GROUPS];
    Sint8 unlocked[RACE_EVENTS];
    Sint8 best[RACE_EVENTS];
    struct garden_race_record records[RACE_RECORD_TRACKS];
};

struct garden_race_state {
    struct garden_race_save saved;
    struct garden_race_time clock;
    Sint8 group, event, level, entrants;
    Uint8 unk_22f[0x2170 - 0x22f];
};

extern struct chao_save *garden_race_entrants[RACE_ENTRANTS];
extern struct garden_race_state garden_race_state;

void garden_race_restore(void);
struct garden_race_save *garden_race_save(void);
void garden_race_clear_entrants(void);
/* garden_race_add_entrant, garden_race_remove_entrant and
 * garden_race_entrant_count return the entrants selected: the count for
 * RACE_MULTI, and 1 for any other group. adding and removing change the
 * count in any group */
int garden_race_add_entrant(void);
int garden_race_remove_entrant(void);
int garden_race_entrant_count(void);
void garden_race_unlock_group(int group);
void garden_race_visit_group(int group);
/* group's status, or an undefined value for a group outside 0 to
 * RACE_GROUPS - 1 */
int garden_race_group_status(int group);
void garden_race_set_group(Sint8 group);
void garden_race_set_event(Sint8 event);
void garden_race_set_level(Sint8 level);
/* event's unlocked level, or -1 for none. an event outside 0 to
 * RACE_EVENTS - 1 returns an undefined value */
int garden_race_unlocked(int event);
/* garden_race_unlock_to raises event's unlocked level to level, and
 * garden_race_unlock_next raises it by one. neither raises it past the number
 * of levels of event's group */
void garden_race_unlock_to(int event, int level);
void garden_race_unlock_next(int event);

#endif
