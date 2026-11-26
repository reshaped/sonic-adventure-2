/* Exercise the same garden, lobby and race admission helpers as the area tasks.
 */
#include <assert.h>
#include <string.h>

#include "chaostglobby/lobby.c"
#include "chaostgrace/scene.c"
#include "shared/garden_layout.h"

enum unavailable {
    AVAILABLE,
    LAND_FILE,
    SET_FILE,
    CONTROLLER,
    SET_MANAGER,
    WORLD_TASK,
    CAMERA
};

static enum unavailable unavailable;
static struct {
    struct race_land_file header;
    struct land_entry entry;
    NJS_CNK_OBJECT glass;
    struct land_table land;
} file;
static const char *expected_land, *expected_set, *expected_track_set;
static Uint32 expected_capacity, saved_doors;
static struct object_list garden_objects;
static struct set_head placements;
static struct race_controller_state controller;
static int layout_owned, manager_live, controller_owned, world_live;
static int camera_live, selected_track, radius_installed;

struct set_head *race_layout;
struct object_list chaostgrace_objects;

Sint32 loader_read_prs_checked(const char *name, void *dest, Uint32 capacity)
{
    assert(strcmp(name, expected_land) == 0);
    assert(dest == (void *)&file && capacity == expected_capacity);
    if (unavailable == LAND_FILE) {
        return -1;
    }
    file.header.land = &file.land;
    file.header.glass = &file.glass;
    file.land.count = file.land.drawn = 1;
    file.land.entries = &file.entry;
    return sizeof(file);
}

struct set_head *set_read(const char *stage, const char *track, int minimum)
{
    assert(strcmp(stage, expected_set) == 0 && !layout_owned);
    if (expected_track_set != NULL) {
        assert(track != NULL && strcmp(track, expected_track_set) == 0);
        assert(minimum == 1024);
    } else {
        assert(track == NULL && minimum == 0x4000 / sizeof(struct set_entry));
    }
    if (unavailable == SET_FILE) {
        return NULL;
    }
    layout_owned = 1;
    return &placements;
}

void loader_free(void *ptr)
{
    assert(ptr == &placements && layout_owned && !manager_live);
    layout_owned = 0;
}

Bool set_start(struct object_list *objects, struct set_head *layout)
{
    assert(objects ==
           (expected_track_set ? &chaostgrace_objects : &garden_objects));
    assert(layout == &placements && layout_owned && !manager_live);
    return manager_live = (unavailable != SET_MANAGER);
}

Bool set_end(void)
{
    assert(manager_live);
    manager_live = 0;
    return TRUE;
}

void *syCalloc(Uint32 count, Uint32 size)
{
    assert(count == 1 && size == sizeof(controller) && !controller_owned);
    if (unavailable == CONTROLLER) {
        return NULL;
    }
    controller_owned = 1;
    return &controller;
}

void syFree(void *ptr)
{
    assert(controller_owned && ptr == &controller);
    controller_owned = 0;
}

int world_land_start(struct land_table *land)
{
    assert(land == &file.land && !world_live);
    return world_live = (unavailable != WORLD_TASK);
}

int world_land_stop(void)
{
    assert(world_live);
    world_live = 0;
    return 1;
}

Bool race_camera_create(int track)
{
    assert(track == TRACK_JWL_LONG && !camera_live);
    return camera_live = (unavailable != CAMERA);
}

void garden_select_track(Sint32 track)
{
    assert(track == TRACK_BEG_RUN);
    selected_track = 1;
}

void world_set_near_radius(Float radius)
{
    assert(radius == 10000.0F && world_live);
    radius_installed = 1;
}

Bool garden_flag_test(Uint32 mask)
{
    return (saved_doors & mask) != 0;
}

static void check_lobby(void)
{
    static const struct {
        Uint32 flags;
        const char *name;
    } doors[] = {
        {0, "ChaoStgLobbyLand000.PRS"},
        {GARDEN_KINDER_OPEN, "ChaoStgLobbyLand00k.PRS"},
        {GARDEN_HERO_OPEN, "ChaoStgLobbyLandh0k.PRS"},
        {GARDEN_DARK_OPEN, "ChaoStgLobbyLand0dk.PRS"},
        {GARDEN_HERO_OPEN | GARDEN_DARK_OPEN, "ChaoStgLobbyLandhdk.PRS"},
        {GARDEN_HERO_OPEN | GARDEN_KINDER_OPEN, "ChaoStgLobbyLandh0k.PRS"},
        {GARDEN_DARK_OPEN | GARDEN_KINDER_OPEN, "ChaoStgLobbyLand0dk.PRS"},
        {GARDEN_HERO_OPEN | GARDEN_DARK_OPEN | GARDEN_KINDER_OPEN,
         "ChaoStgLobbyLandhdk.PRS"}};
    unsigned int i;

    expected_capacity = 0x20000;
    for (i = 0; i < sizeof(doors) / sizeof(doors[0]); i++) {
        saved_doors = doors[i].flags;
        expected_land = doors[i].name;
        unavailable = LAND_FILE;
        assert(!lobby_start_land(lobby_layout(), &file.header.land));
        assert(!world_live);
        unavailable = WORLD_TASK;
        assert(!lobby_start_land(lobby_layout(), &file.header.land));
        assert(!world_live);
        unavailable = AVAILABLE;
        assert(lobby_start_land(lobby_layout(), &file.header.land));
        assert(world_live && file.header.land == &file.land);
        world_land_stop();
    }
}

static void check_garden(void)
{
    static const enum unavailable faults[] = {SET_FILE, SET_MANAGER,
                                              WORLD_TASK};
    unsigned int i;

    expected_set = "set_chao_neut.bin";
    expected_track_set = NULL;
    for (i = 0; i < sizeof(faults) / sizeof(faults[0]); i++) {
        unavailable = faults[i];
        assert(garden_start_layout(expected_set, &garden_objects, &file.land) ==
               NULL);
        assert(!layout_owned && !manager_live && !world_live);
    }
    unavailable = AVAILABLE;
    assert(garden_start_layout(expected_set, &garden_objects, &file.land) ==
           &placements);
    assert(layout_owned && manager_live && world_live);
    world_land_stop();
    set_end();
    loader_free(&placements);
}

static void check_race(void)
{
    struct garden_race_course course = {0, RACE_LAND_HERO, TRACK_BEG_RUN,
                                        TRACK_JWL_LONG, RACE_NO_AWARD};
    struct race_controller_state *state;
    enum unavailable fault;

    expected_capacity = 0x60000;
    expected_land = "ChaoStgRaceLandH.PRS";
    expected_set = "ChaoSetRaceLandHero.bin";
    expected_track_set = "ChaoSetRaceBegRun.bin";
    for (fault = LAND_FILE; fault <= CAMERA; fault++) {
        unavailable = fault;
        assert(race_start_course(&course, &file.header) == NULL);
        assert(!layout_owned && !manager_live && !controller_owned);
        assert(!world_live && !camera_live && !selected_track &&
               !radius_installed);
        assert(race_layout == NULL);
    }
    unavailable = AVAILABLE;
    state = race_start_course(&course, &file.header);
    assert(state == &controller);
    assert(layout_owned && manager_live && controller_owned && world_live);
    assert(camera_live && selected_track && radius_installed);
    assert(race_layout == &placements);
}

int main(void)
{
    check_lobby();
    check_garden();
    check_race();
    return 0;
}
