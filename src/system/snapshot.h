/* the snapshots: two copies of the game_state fields that struct snapshot
 * names. snapshots_swap fails where game_state no longer matches the current
 * copy. each write to one of those fields must write snapshot_current's too */
#ifndef SYSTEM_SNAPSHOT_H
#define SYSTEM_SNAPSHOT_H
#include <sg_xpt.h>

/* a snapshot. each field copies the field of game_state with its name */
struct snapshot {
    Sint8 minute;
    Sint8 second;
    Sint8 frame;
    Sint8 rank;
    Sint8 kept_minute;
    Sint8 kept_second;
    Sint8 kept_frame;
    Sint16 lives[2];
    Sint16 rings[2];
    Sint16 ring_total;
    Uint32 total_steps;
    Uint32 score;
};

/* the current snapshot, one of two, or null until snapshots_start */
extern struct snapshot *snapshot_current;

/* the value of every byte of a clear snapshot */
#define SNAPSHOT_CLEAR (-2)
/* make the first snapshot current, as a copy of game_state, and clear the
 * other. a second call stops the system with sbExitSystem */
void snapshots_start(void);
/* copy game_state into the other snapshot, clear the current one, and make
 * the other current. it returns 0, or -1 with nothing changed where
 * snapshot_current is neither snapshot, the other isn't clear, or the
 * current one no longer matches game_state */
int snapshots_swap(void);

#endif
