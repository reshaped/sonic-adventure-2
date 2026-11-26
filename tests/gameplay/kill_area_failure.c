/* Drive an area twice on the same task ring, first with a failing heap. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <shinobi.h>

#include "camera/stack.h"
#include "collision/collision.h"
#include "collision/polygon.h"
#include "hud/hud.h"
#include "player/action.h"
#include "player/player.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/task.h"

struct game_state game_state;
struct player *player_records[PLAYER_SLOTS];
struct task_work *player_work[PLAYER_SLOTS];
struct task *kill_areas_task;
struct kill_area *kill_areas;

static NJS_OBJECT model;
static struct task_work work;
static struct player player;
static int active_slot;
static int fail_at;
static int attempts;
static int allocations;
static int probes;
static char events[16];

static void *allocate(Uint32 count, Uint32 size)
{
    void *result;

    if (++attempts == fail_at) {
        return NULL;
    }
    result = calloc(count, size);
    assert(result != NULL);
    allocations++;
    return result;
}

void *syMalloc(Uint32 size)
{
    void *result = allocate(1, size);

    if (result != NULL) {
        memset(result, 0xa5, size);
    }
    return result;
}
void *syCalloc(Uint32 count, Uint32 size)
{
    return allocate(count, size);
}

void syFree(void *block)
{
    assert(block != NULL && allocations > 0);
    allocations--;
    free(block);
}

void sound_detach_task(const struct task *task)
{
    assert(task->step == NULL);
}
void collision_free(struct task *task)
{
    abort();
}
void njCnkEasyDrawObject(NJS_CNK_OBJECT *object)
{
    abort();
}
int stage_current_id(void)
{
    return STAGE_SAND_OCEAN;
}

int stage_character(int slot)
{
    assert(slot == active_slot);
    return PLAYER_KNUCKLES;
}

Bool polygon_find_model_heights(struct height_probe *probe, NJS_OBJECT *object)
{
    assert(object == &model && probe->pos.y == work.pos.y);
    probes++;
    probe->above.found = TRUE;
    probe->above.pos = 42.0F;
    probe->below.found = FALSE;
    return TRUE;
}

/* The child steps start on a later scheduler visit. This test calls the
 * area's installed step and inspects the tasks it admits. */
static void miss_step(struct task *task)
{
    abort();
}
static void water_sink_step(struct task *task)
{
    abort();
}
static void sink_miss_step(struct task *task)
{
    abort();
}
static void sink_step(struct task *task)
{
    abort();
}
static void sunk_step(struct task *task)
{
    abort();
}

#include "kill_areas.inc"

static void event(char code)
{
    size_t len = strlen(events);
    struct task *child = kill_areas_task->child;

    assert(child != NULL && MISS(child)->slot == active_slot);
    if (kill_areas->flags & (KILL_AREA_WATER | KILL_AREA_SINK)) {
        assert(child->child != NULL && MISS(child->child)->slot == active_slot);
    }
    assert(len + 1 < sizeof(events));
    events[len] = code;
    events[len + 1] = '\0';
}

void player_splash(int slot, NJS_POINT3 *pos, struct angles *ang)
{
    assert(slot == active_slot && ang == NULL && pos->y == work.pos.y);
    event('W');
}

void player_request(int slot, int request, int arg)
{
    assert(slot == active_slot && arg == 0);
    if (kill_areas->flags & KILL_AREA_SINK) {
        assert(request == PLAYER_REQUEST_SINK);
        assert(MISS(kill_areas_task->child->child)->surface == 42.0F);
        event('S');
    } else {
        assert(request == PLAYER_REQUEST_RELEASE);
        event('R');
    }
}

struct camera_slot *camera_push_mode(int slot, int mode)
{
    assert(slot == active_slot && mode == CAMERA_MODE_LEAVE);
    event('C');
    return NULL;
}

int camera_top(int slot)
{
    assert(slot == active_slot);
    return 1;
}

int camera_set_adjust(int slot, int index, int number)
{
    assert(slot == active_slot && index == 1 && number == CAMERA_ADJUST_NONE);
    event('A');
    return 0;
}

void voice_play_priority_2(int id)
{
    assert(id == VOICE_MISS_KNUCKLES);
    event('V');
}

void pause_disable(void)
{
    event('P');
}
void stage_clock_stop(void)
{
    event('T');
}

static void check_route(Uint32 kind, int failure, const char *expected)
{
    struct kill_area areas[2] = {{0}};
    struct task *parent;
    struct task *child;

    areas[0].object = &model;
    areas[0].flags = kind | KILL_AREA_ABOVE | KILL_AREA_ALL_CHARACTERS;
    assert(kill_areas_start(areas));
    parent = kill_areas_task;
    assert(task_levels[0] == parent && allocations == 2);
    fail_at = failure;
    attempts = 0;
    probes = 0;
    events[0] = '\0';

    parent->step(parent);
    assert(probes == 1 && attempts == failure && events[0] == '\0');
    assert(parent->child == NULL && allocations == 2);
    assert(kill_areas_task == parent && kill_areas == areas);

    fail_at = 0;
    attempts = 0;
    parent->step(parent);
    child = parent->child;
    assert(probes == 2 && strcmp(events, expected) == 0);
    assert(child != NULL && child->parent == parent && child->next == child);
    assert(MISS(child)->slot == active_slot &&
           child->destructor == miss_destroy);
    if (kind == 0) {
        assert(child->step == miss_step && child->child == NULL &&
               attempts == 2);
    } else {
        assert(attempts == 4 && child->child->parent == child);
        if (kind == KILL_AREA_WATER) {
            assert(child->step == miss_step &&
                   child->child->step == water_sink_step);
        } else {
            assert(child->step == sink_miss_step &&
                   child->child->step == sink_step);
        }
    }
    assert(allocations == 2 + attempts);
    parent->step(parent);
    assert(parent->child == child && probes == 2 &&
           strcmp(events, expected) == 0);
    task_destroy(parent);
    assert(task_levels[0] == NULL && kill_areas_task == NULL &&
           kill_areas == NULL);
    assert(allocations == 0);
}

int main(void)
{
    int failure;

    task_init();
    player.kind = PLAYER_KNUCKLES;
    work.pos.y = 36.0F;
    for (active_slot = 0; active_slot < 2; active_slot++) {
        player_records[active_slot] = &player;
        player_work[active_slot] = &work;
        for (failure = 1; failure <= 4; failure++) {
            if (failure <= 2) {
                check_route(0, failure, "CARVPT");
            }
            check_route(KILL_AREA_WATER, failure, "WCARVPT");
            check_route(KILL_AREA_SINK, failure, "S");
        }
        player_records[active_slot] = NULL;
        player_work[active_slot] = NULL;
    }
    return 0;
}
