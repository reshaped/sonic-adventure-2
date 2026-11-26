/* Exercise the garden's one child slot with a scheduler that can run out of
 * task records and a loader that can fail independently of the scheduler. */
#include <assert.h>
#include <string.h>

#include "modules/chao/garden.c"

int garden_previous = CHAO_AREA_NONE;
int garden_current = CHAO_AREA_NONE;
int garden_next = CHAO_AREA_NONE;
struct task *garden_player_control;
struct task_work *player_work[PLAYER_SLOTS];
Sint8 pad_action_held[GAME_PLAYERS], pad_action_pressed[GAME_PLAYERS];
Sint8 pad_jump_held[GAME_PLAYERS], pad_jump_pressed[GAME_PLAYERS];

static struct task child;
static struct task control;
static struct task_work control_work;
static int load_ok, child_ok, control_ok;
static int reads, destroyed, activated;
static const char *wanted_name, *wanted_tag;
static Uint32 wanted_capacity;

static void child_step(struct task *task)
{
    (void)task;
}

Bool loader_install_chao_area(const char *name, const char *tag, void *slot,
                              Uint32 capacity)
{
    struct chao_area_head *area = slot;
    ++reads;
    assert(strcmp(name, wanted_name) == 0);
    assert(strcmp(tag, wanted_tag) == 0 && capacity == wanted_capacity);
    if (load_ok) {
        area->step = child_step;
    }
    return load_ok;
}

struct task *task_create(Uint16 flags, int level, void (*step)(struct task *),
                         const char *name)
{
    (void)name;
    assert(flags == TASK_WORK);
    if (level == AREA_TASK_LEVEL) {
        assert(step == child_step);
        return child_ok ? &child : NULL;
    }
    assert(level == 0 && step == player_control_step);
    control.work = &control_work;
    return control_ok ? &control : NULL;
}

void task_destroy(struct task *task)
{
    assert(task == &child);
    ++destroyed;
}

void particle_core_stop(void)
{
}
void particle_core_start(void)
{
}
void garden_spawn_grid_init(void)
{
}
void garden_area_sound_load(void)
{
    ++activated;
}

static void request(int id, const char *name, const char *tag, Uint32 capacity)
{
    garden_request_area(id);
    wanted_name = name;
    wanted_tag = tag;
    wanted_capacity = capacity;
}

static Bool try_area(struct chao_area_head *area)
{
    static struct task_work work;
    static struct task manager;

    manager.work = &work;
    work.mode = GARDEN_LOAD_AREA;
    garden_load_area(&manager, area);
    if (work.mode == GARDEN_LOAD_AREA) {
        assert(garden_task == NULL);
        return FALSE;
    }
    assert(work.mode == GARDEN_PLAY && garden_task == &manager);
    return TRUE;
}

int main(void)
{
    struct chao_area_head area;
    int i;
    const Uint32 normal = 0x210000;
    static const char *const names[] = {
        "ChaoStgHero.PRS",     "ChaoStgDark.PRS",   "ChaoStgRace.PRS",
        "ChaoStgEntrance.PRS", "ChaoStgKinder.PRS", "ChaoStgLobby.PRS"};
    static const char *const tags[] = {"ALSTHERO", "ALSTDARK", "ALSTRACE",
                                       "ALSTRACE", "ALSTKNDR", "ALSTLOBY"};

    request(CHAO_AREA_NEUTRAL, "ChaoStgNeut.PRS", "ALSTNEUT", normal);
    assert(!try_area(&area));
    assert(garden_current == CHAO_AREA_NONE &&
           garden_next == CHAO_AREA_NEUTRAL);
    load_ok = 1;
    assert(!try_area(&area));
    assert(reads == 2 && activated == 0 && garden_player_control == NULL);
    child_ok = 1;
    assert(!try_area(&area));
    assert(reads == 2 && activated == 0 && destroyed == 1);
    control_ok = 1;
    assert(try_area(&area));
    assert(reads == 2 && activated == 1 && garden_current == CHAO_AREA_NEUTRAL);
    assert(garden_previous == CHAO_AREA_NONE &&
           control.destructor == player_control_destroy);

    request(CHAO_AREA_HERO, names[0], tags[0], normal);
    load_ok = 0;
    assert(!try_area(&area));
    request(CHAO_AREA_NEUTRAL, "ChaoStgNeut.PRS", "ALSTNEUT", normal);
    load_ok = 1;
    assert(try_area(&area));
    assert(reads == 4 && garden_current == CHAO_AREA_NEUTRAL);
    assert(garden_previous == CHAO_AREA_NONE);

    /* Hero successfully replaced the slot, but its task could not start.
     * Returning to the still-current Neutral Garden must install it again. */
    request(CHAO_AREA_HERO, names[0], tags[0], normal);
    child_ok = 0;
    assert(!try_area(&area));
    assert(reads == 5 && garden_current == CHAO_AREA_NEUTRAL);
    request(CHAO_AREA_NEUTRAL, "ChaoStgNeut.PRS", "ALSTNEUT", normal);
    child_ok = 1;
    assert(try_area(&area));
    assert(reads == 6 && garden_previous == CHAO_AREA_NONE);

    for (i = CHAO_AREA_HERO; i <= CHAO_AREA_LOBBY; ++i) {
        Uint32 capacity = i == CHAO_AREA_RACE    ? 0x80000
                          : i == CHAO_AREA_LOBBY ? 0xc0000
                                                 : normal;
        request(i, names[i - CHAO_AREA_HERO], tags[i - CHAO_AREA_HERO],
                capacity);
        assert(try_area(&area));
        assert(garden_current == i && garden_previous == i - 1);
    }
    request(CHAO_AREA_RACE_ENTRY, names[3], tags[3], normal);
    assert(try_area(&area));
    i = reads;
    request(CHAO_AREA_ENTRANCE, names[3], tags[3], normal);
    assert(try_area(&area));
    assert(reads == i && garden_previous == CHAO_AREA_RACE_ENTRY);
    return 0;
}
