#include <ninja.h>
#include <sg_xpt.h>
#include <string.h>

#include "chao/race.h"
#include "chao/race_record.h"
#include "chao/garden_save.h"
#include "chao/race_state.h"

#define LAST_MINUTE 99
#define LAST_SECOND 59
#define LAST_FRAME 59

struct chao_save *garden_race_entrants[RACE_ENTRANTS];
struct garden_race_state garden_race_state;

void garden_race_restore(void)
{
    const struct garden_race_save *saved;
    int i;

    memset(&garden_race_state, 0, sizeof(garden_race_state));
    saved = garden_file_race_save();
    njMemCopy(&garden_race_state.saved, saved, sizeof(*saved));
    for (i = 0; i < RACE_ENTRANTS; i++) {
        garden_race_entrants[i] = NULL;
    }
}

struct garden_race_save *garden_race_save(void)
{
    return &garden_race_state.saved;
}

void garden_race_clear_entrants(void)
{
    garden_race_state.entrants = 0;
}

int garden_race_add_entrant(void)
{
    struct garden_race_state *state = &garden_race_state;

    state->entrants++;
    if (state->group != RACE_MULTI) {
        return 1;
    }
    return state->entrants;
}

int garden_race_remove_entrant(void)
{
    struct garden_race_state *state = &garden_race_state;

    state->entrants--;
    if (state->group != RACE_MULTI) {
        return 1;
    }
    return state->entrants;
}

int garden_race_entrant_count(void)
{
    struct garden_race_state *state = &garden_race_state;

    if (state->group != RACE_MULTI) {
        return 1;
    }
    return state->entrants;
}

void garden_race_unlock_group(int group)
{
    if (group >= 0 && group < RACE_GROUPS) {
        if (garden_race_state.saved.group_status[group] == RACE_GROUP_HIDDEN) {
            garden_race_state.saved.group_status[group] = RACE_GROUP_NEW;
        }
    }
}

void garden_race_visit_group(int group)
{
    if (group >= 0 && group < RACE_GROUPS) {
        garden_race_state.saved.group_status[group] = RACE_GROUP_VISITED;
    }
}

int garden_race_group_status(int group)
{
    if (group >= 0 && group < RACE_GROUPS) {
        return garden_race_state.saved.group_status[group];
    }
    /* @bug uninitialized: returns R0 as the caller left it for a group
     * outside 0 to RACE_GROUPS - 1: 0x0c516376 and 0x0c51637c branch
     * to the return at 0x0c516382 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: any other group returns
     * RACE_GROUP_HIDDEN */
    return RACE_GROUP_HIDDEN;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

void garden_race_set_group(Sint8 group)
{
    garden_race_state.group = group;
}

int garden_race_group(void)
{
    return garden_race_state.group;
}

void garden_race_set_event(Sint8 event)
{
    garden_race_state.event = event;
}

int garden_race_event(void)
{
    return garden_race_state.event;
}

void garden_race_set_level(Sint8 level)
{
    garden_race_state.level = level;
}

int garden_race_level(void)
{
    return garden_race_state.level;
}

int garden_race_unlocked(int event)
{
    if (event >= 0 && event < RACE_EVENTS) {
        return garden_race_state.saved.unlocked[event];
    }
    /* @bug uninitialized: returns R0 as the caller left it for an event
     * outside 0 to RACE_EVENTS - 1: 0x0c5163ac and 0x0c5163b2 branch
     * to the return at 0x0c5163b8 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: any other event returns -1 */
    return -1;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

void garden_race_unlock_to(int event, int level)
{
    Sint8 *unlocked = &garden_race_state.saved.unlocked[event];

    if (level > *unlocked) {
        switch (event) {
        case 0:
        case 1:
        case 2:
        case 3:
            if (level <= RACE_BEGINNER_LEVELS) {
                *unlocked = level;
            }
            break;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            if (level <= RACE_JEWEL_LEVELS) {
                *unlocked = level;
            }
            break;
        case RACE_CHALLENGE_EVENT:
            if (level <= RACE_CHALLENGE_LEVELS) {
                *unlocked = level;
            }
            break;
        case RACE_DARK_EVENT:
            if (level <= RACE_ALIGNMENT_LEVELS) {
                *unlocked = level;
            }
            break;
        /* @hack spelling: RACE_HERO_EVENT's own case, where sharing
         * RACE_DARK_EVENT's would do */
        case RACE_HERO_EVENT:
            if (level <= RACE_ALIGNMENT_LEVELS) {
                *unlocked = level;
            }
            break;
        }
    }
}

void garden_race_unlock_next(int event)
{
    Sint8 *unlocked = &garden_race_state.saved.unlocked[event];

    switch (event) {
    case 0:
    case 1:
    case 2:
    case 3:
        if (*unlocked < RACE_BEGINNER_LEVELS) {
            (*unlocked)++;
        }
        break;
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        if (*unlocked < RACE_JEWEL_LEVELS) {
            (*unlocked)++;
        }
        break;
    case RACE_CHALLENGE_EVENT:
        if (*unlocked < RACE_CHALLENGE_LEVELS) {
            (*unlocked)++;
        }
        break;
    case RACE_DARK_EVENT:
        if (*unlocked < RACE_ALIGNMENT_LEVELS) {
            (*unlocked)++;
        }
        break;
    /* @hack spelling: RACE_HERO_EVENT's own case, where sharing
     * RACE_DARK_EVENT's would do */
    case RACE_HERO_EVENT:
        if (*unlocked < RACE_ALIGNMENT_LEVELS) {
            (*unlocked)++;
        }
        break;
    }
}

int garden_race_best_level(int event)
{
    if (event >= 0 && event < RACE_EVENTS) {
        return garden_race_state.saved.best[event];
    }
    /* @bug uninitialized: returns R0 as the caller left it for an event
     * outside 0 to RACE_EVENTS - 1: 0x0c5164d6 and 0x0c5164dc branch
     * to the return at 0x0c5164e2 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: any other event returns -1 */
    return -1;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

void garden_race_record_level(int event, int level)
{
    Sint8 *best = &garden_race_state.saved.best[event];

    if (level > *best) {
        switch (event) {
        case 0:
        case 1:
        case 2:
        case 3:
            if (level < RACE_BEGINNER_LEVELS) {
                *best = level;
            }
            break;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            if (level < RACE_JEWEL_LEVELS) {
                *best = level;
            }
            break;
        case RACE_CHALLENGE_EVENT:
            if (level < RACE_CHALLENGE_LEVELS) {
                *best = level;
            }
            break;
        case RACE_DARK_EVENT:
            if (level < RACE_ALIGNMENT_LEVELS) {
                *best = level;
            }
            break;
        /* @hack spelling: RACE_HERO_EVENT's own case, where sharing
         * RACE_DARK_EVENT's would do */
        case RACE_HERO_EVENT:
            if (level < RACE_ALIGNMENT_LEVELS) {
                *best = level;
            }
            break;
        }
    }
}

void garden_race_advance_record(int event)
{
    Sint8 *best = &garden_race_state.saved.best[event];

    switch (event) {
    case 0:
    case 1:
    case 2:
    case 3:
        if (*best < RACE_BEGINNER_LEVELS) {
            (*best)++;
        }
        break;
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        if (*best < RACE_JEWEL_LEVELS) {
            (*best)++;
        }
        break;
    case RACE_CHALLENGE_EVENT:
        if (*best < RACE_CHALLENGE_LEVELS) {
            (*best)++;
        }
        break;
    case RACE_DARK_EVENT:
        if (*best < RACE_ALIGNMENT_LEVELS) {
            (*best)++;
        }
        break;
    /* @hack spelling: RACE_HERO_EVENT's own case, where sharing
     * RACE_DARK_EVENT's would do */
    case RACE_HERO_EVENT:
        if (*best < RACE_ALIGNMENT_LEVELS) {
            (*best)++;
        }
        break;
    }
}

void garden_race_get_record(int track, struct garden_race_record *record)
{
    if (track >= 0 && track < RACE_RECORD_TRACKS) {
        *record = garden_race_state.saved.records[track];
    }
}

void garden_race_set_record(int track, const struct garden_race_record *record)
{
    if (track >= 0 && track < RACE_RECORD_TRACKS) {
        garden_race_state.saved.records[track] = *record;
    }
}

void garden_race_clear_time(void)
{
    struct garden_race_time *clock = &garden_race_state.clock;

    clock->minutes = 0;
    clock->seconds = 0;
    clock->frames = 0;
}

void garden_race_tick(void)
{
    struct garden_race_time *clock = &garden_race_state.clock;

    clock->frames++;
    if (clock->frames > LAST_FRAME) {
        if (clock->minutes < LAST_MINUTE || clock->seconds < LAST_SECOND) {
            clock->frames = 0;
            clock->seconds++;
            if (clock->seconds > LAST_SECOND) {
                if (clock->minutes < LAST_MINUTE) {
                    clock->seconds = 0;
                    clock->minutes++;
                    /* @hack redundant: a clamp of minutes, which the test
                     * above keeps within LAST_MINUTE */
                    if (clock->minutes > LAST_MINUTE) {
                        clock->minutes = LAST_MINUTE;
                    }
                } else {
                    clock->seconds = LAST_SECOND;
                }
            }
        } else {
            clock->frames = LAST_FRAME;
        }
    }
}

void garden_race_get_time(struct garden_race_time *time)
{
    *time = garden_race_state.clock;
}
