/* Exercise the current stage's kind list, including held placements. */
#include <assert.h>
#include <stdlib.h>

#include "object/set.c"
#include "stg42/placement_wrap.c"

struct game_state game_state;
static struct snapshot snapshot;
struct snapshot *snapshot_current = &snapshot;
struct task_work *player_work[PLAYER_SLOTS];
struct task_work *camera_task_work;

static struct task actors[2];
static int spawned;

struct task *task_create(Uint16 flags, int level, void (*step)(struct task *),
                         const char *name)
{
    assert(spawned < 2);
    return &actors[spawned++];
}

void task_destroy(struct task *task)
{
    task->placement->flags &= ~PLACEMENT_LIVE;
    task->placement = NULL;
}

void syFree(void *ptr)
{
    free(ptr);
}

void ring_total_restore(void)
{
    snapshot_current->ring_total = game_state.kept_ring_total;
    game_state.ring_total = game_state.kept_ring_total;
}

int main(void)
{
    static struct {
        struct set_head head;
        struct set_entry entries[PLACEMENTS_MAX];
    } file;
    static struct object_kind kinds[2] = {
        {0, 0, KIND_ALWAYS | RINGS_ONE, 0, NULL, "ring"},
        {0, 0, RINGS_SCL_Z, 0, NULL, "ring chain"},
    };
    struct object_list list = OBJECT_LIST(kinds);
    struct object_list next_list = {1, kinds};
    struct task_work manager = {0};
    struct task_work player = {0};
    struct set_head empty = {0};

    set_list = &list;
    set_take_file(&empty);
    assert(placement_count == 0 && placements[0].entry == NULL);
    file.head.count = PLACEMENTS_MAX;
    set_take_file(&file.head);
    assert(placement_count == PLACEMENTS_MAX);
    assert(placements[PLACEMENTS_MAX - 1].entry ==
           &file.entries[PLACEMENTS_MAX - 1]);

    file.head.count = 2;
    file.entries[0].id = 0x8000; /* high bit does not select the kind */
    file.entries[1].id = 1;
    file.entries[1].scl.z = 5.0F;
    game_state.kept_ring_total = 2;
    set_take_file(&file.head);
    assert(game_state.ring_total == 8 && snapshot.ring_total == 8);
    assert(placement_count == 2 && placements[2].entry == NULL);
    player_work[0] = &player;
    set_pass();
    assert(spawned == 2 && actors[1].placement == &placements[1]);
    placement_hold(&actors[1]);

    set_queue(&next_list, &empty);
    set_start_queued_swap(&manager);
    set_swap(&manager); /* release the unheld actor */
    actors[0].step(&actors[0]);
    set_swap(&manager); /* wait */
    set_swap(&manager); /* compact the held actor */
    set_swap(&manager); /* add the empty file */
    assert(placement_count == 1 && actors[1].placement == &placements[0]);
    assert(placements[0].entry == &file.entries[1]);
    task_destroy(&actors[1]);

    set_pass();
    player_work[1] = &player;
    set_pass();
    player.pos.x = WRAP_LAP;
    placements_wrap();
    assert(spawned == 2 && file.entries[1].pos.x == 0.0F);

    set_take_file(&file.head);
    assert(game_state.ring_total == 3 && snapshot.ring_total == 3);
    return 0;
}
