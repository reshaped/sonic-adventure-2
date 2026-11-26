/* the key's loose step where body_init found no room for its body: the step
 * skips body_step. it registers the collision of a key no player holds, and
 * not of a held one */
#include <assert.h>
#include <string.h>

#include "modules/stg28/key.c"

struct task_work *player_work[PLAYER_SLOTS];
struct camera_place *camera_views[CAMERAS];
int camera_current_index;

/* the times the step registered the key's collision */
static int registered;

Bool collision_register(struct task *task)
{
    (void)task;
    registered++;
    return TRUE;
}

/* nothing a body would need runs */
static void unreached(void)
{
    assert(!"the step used the body it lacks");
}

void body_step(struct task *task)
{
    (void)task;
    unreached();
}

void body_destroy(struct task *task)
{
    (void)task;
    unreached();
}

void collision_free(struct task *task)
{
    (void)task;
    unreached();
}

/* the player stands far from the key */
Float njDistanceP2P(NJS_POINT3 *p1, NJS_POINT3 *p2)
{
    (void)p1;
    (void)p2;
    return 1.0e6F;
}

Float cave_xz_distance(NJS_POINT3 *from, NJS_POINT3 *to)
{
    (void)from;
    (void)to;
    return 1.0e6F;
}

Bool cave_on_screen(NJS_POINT3 *point)
{
    (void)point;
    return FALSE;
}

int player_holding(struct task *task)
{
    (void)task;
    return -1;
}

Sint8 sound_play_task_at_for(int id, struct task *task, Sint8 priority,
                             Sint16 volume, Sint16 duration)
{
    (void)id;
    (void)task;
    (void)priority;
    (void)volume;
    (void)duration;
    return 0;
}

int main(void)
{
    struct task_work player = {0};
    struct task_work work = {0};
    struct key record;
    struct task key = {0};

    memset(&record, 0, sizeof(record));
    /* in KEY_MODE_FLY, key_on_keyhole and key_shows_stand return FALSE, so
     * the step needs no keyhole or camera */
    record.mode = KEY_MODE_FLY;
    key.work = &work;
    key.extra.p = &record;
    player_work[0] = &player;
    key_loose_step(&key);
    assert(registered == 1);
    work.flags |= WORK_HELD_BY_PLAYER;
    key_loose_step(&key);
    assert(registered == 1);
    return 0;
}
