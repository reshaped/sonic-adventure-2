/* the snapshots: two copies of the game_state fields that struct snapshot
 * names */
#include <sg_xpt.h>

#include "system/game_state.h"
#include "system/sbinit.h"
#include "system/snapshot.h"

static struct snapshot snapshots[2];
/* the initializer keeps snapshot_current in section D, where the image holds
 * it */
struct snapshot *snapshot_current = NULL;

/* 0 where snapshots[which] matches game_state, and -1 where a field
 * differs */
static int snapshot_check(int which)
{
    struct snapshot *snapshot = &snapshots[which];

    if (snapshot->minute != game_state.minute) {
        return -1;
    }
    if (snapshot->second != game_state.second) {
        return -1;
    }
    if (snapshot->frame != game_state.frame) {
        return -1;
    }
    if (snapshot->rank != game_state.rank) {
        return -1;
    }
    if (snapshot->kept_minute != game_state.kept_minute) {
        return -1;
    }
    if (snapshot->kept_second != game_state.kept_second) {
        return -1;
    }
    if (snapshot->kept_frame != game_state.kept_frame) {
        return -1;
    }
    if (snapshot->lives[0] != game_state.lives[0]) {
        return -1;
    }
    if (snapshot->lives[1] != game_state.lives[1]) {
        return -1;
    }
    if (snapshot->rings[0] != game_state.rings[0]) {
        return -1;
    }
    if (snapshot->rings[1] != game_state.rings[1]) {
        return -1;
    }
    if (snapshot->ring_total != game_state.ring_total) {
        return -1;
    }
    if (snapshot->total_steps != game_state.total_steps) {
        return -1;
    }
    if (snapshot->score != game_state.score) {
        return -1;
    }
    return 0;
}

static void snapshot_take(int which)
{
    struct snapshot *snapshot = &snapshots[which];

    snapshot->minute = game_state.minute;
    snapshot->second = game_state.second;
    snapshot->frame = game_state.frame;
    snapshot->rank = game_state.rank;
    snapshot->kept_minute = game_state.kept_minute;
    snapshot->kept_second = game_state.kept_second;
    snapshot->kept_frame = game_state.kept_frame;
    snapshot->lives[0] = game_state.lives[0];
    snapshot->lives[1] = game_state.lives[1];
    snapshot->rings[0] = game_state.rings[0];
    snapshot->rings[1] = game_state.rings[1];
    snapshot->ring_total = game_state.ring_total;
    snapshot->total_steps = game_state.total_steps;
    snapshot->score = game_state.score;
}

static void snapshot_clear(int which)
{
    Sint8 *byte = (Sint8 *)&snapshots[which];
    int left = sizeof(struct snapshot);

    /* @hack spelling: a while loop, where a for loop like snapshots_swap's
     * would do */
    while (left > 0) {
        *byte = SNAPSHOT_CLEAR;
        byte++;
        left--;
    }
}

void snapshots_start(void)
{
    if (snapshot_current != NULL) {
        sbExitSystem();
        return;
    }
    snapshot_current = &snapshots[0];
    snapshot_take(0);
    snapshot_clear(1);
}

int snapshots_swap(void)
{
    int current;
    int other;
    Sint8 *byte;
    int left;

    if (snapshot_current == &snapshots[0]) {
        current = 0;
        other = 1;
    } else if (snapshot_current == &snapshots[1]) {
        current = 1;
        other = 0;
    } else {
        return -1;
    }
    byte = (Sint8 *)&snapshots[other];
    for (left = sizeof(struct snapshot); left > 0; left--) {
        if (*byte != SNAPSHOT_CLEAR) {
            return -1;
        }
        byte++;
    }
    if (snapshot_check(current) == -1) {
        return -1;
    }
    snapshot_take(other);
    snapshot_clear(current);
    snapshot_current = &snapshots[other];
    return 0;
}
