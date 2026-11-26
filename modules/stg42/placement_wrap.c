/* the placements of the second fight of Sonic and Shadow, which follow
 * player 0 along x in laps */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/placement.h"
#include "object/set.h"
#include "player/player.h"
#include "stg42/placement_wrap.h"
#include "system/task.h"

/* object/set.c's object list and placements. no header declares them: an
 * extern there would reorder set.c's bss, as set.c says */
extern struct object_list *set_list;
extern Sint16 placement_count;
extern struct placement placements[];

/* a lap's length along x */
#define WRAP_LAP 2400.0F
/* no placement moves while player 0's pos.x is below WRAP_START */
#define WRAP_START 300.0F

void placements_wrap(void)
{
    struct placement *placement;
    struct set_entry *entry;
    /* @hack spelling: kind, a local for the entry's kind, where the kind
     * read in place would do */
    struct object_kind *kind;
    /* @hack redundant: x a register variable, where a plain local would do */
    register Float x;
    Float high;
    Float low;
    int i;

    /* @hack redundant: the read of x in a block of if (1), where the
     * statement alone would do */
    if (1) {
        x = player_work[0]->pos.x;
    }
    placement = placements;
    for (i = placement_count - 1; i >= 0; i--, placement++) {
        entry = placement->entry;
#ifdef NONMATCHING
        /* this side passes over a placement whose kind set_entry_kind doesn't
         * find, as for an id past the list's kinds. the matching side indexes
         * kinds unchecked */
        kind = set_entry_kind(set_list, entry);
        if (kind == NULL) {
            continue;
        }
#else
        kind = &set_list->kinds[entry->id & SET_KIND];
#endif
        if (kind->attr & KIND_ALWAYS) {
            continue;
        }
        if (placement->flags & (PLACEMENT_LIVE | PLACEMENT_IN_RANGE)) {
            continue;
        }
        if (x < WRAP_START) {
            continue;
        }
        high = x + (WRAP_LAP / 2);
        low = x - (WRAP_LAP / 2);
        if (entry->pos.x > high || entry->pos.x <= low) {
            for (;;) {
                if (entry->pos.x > high) {
                    entry->pos.x -= WRAP_LAP;
                } else if (entry->pos.x < low) {
                    entry->pos.x += WRAP_LAP;
                } else {
                    break;
                }
            }
            placement->flags = PLACEMENT_ACTIVE;
            placement->tasks = 0;
            placement->state = 0;
        }
    }
}
