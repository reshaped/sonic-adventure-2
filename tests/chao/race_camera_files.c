/* A track change must retain the live camera selection until both new
 * key lists are ready. Use the real editor state with a private work area. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "chaostgrace/camera.h"
#include "chaostgrace/camera_key.h"
#include "chaostgrace/edit_menu.h"
#include "system/areas.h"

static union {
    struct race_camera_script script;
    Uint8 bytes[0x200000];
} scratch;
#undef WORK_AREA
#undef WORK_AREA_END
#define WORK_AREA scratch.bytes
#define WORK_AREA_END (scratch.bytes + sizeof(scratch.bytes))
#include "chaostgrace/script.c"

static struct race_camera_script file, previous;
static const char *expected_name;
static int missing, allocations, fail_at, live_cells;

Sint32 loader_read_file_checked(const char *name, void *dest, Uint32 capacity)
{
    size_t length = 15360; /* Stored size of the ten Japanese track files. */

    assert(strcmp(name, expected_name) == 0);
    assert(dest == WORK_AREA && capacity >= length);
    if (missing) {
        return -1;
    }
    memcpy(dest, &file, length);
    return length;
}

void *syCalloc(Uint32 count, Uint32 size)
{
    void *cell;
    allocations++;
    if (allocations == fail_at) {
        return NULL;
    }
    cell = calloc(count, size);
    assert(cell != NULL);
    live_cells++;
    return cell;
}

void syFree(void *cell)
{
    assert(cell != NULL && live_cells > 0);
    live_cells--;
    free(cell);
}

static void set_keys(int track)
{
    int i;
    memset(&file, 0, sizeof(file));
    for (i = 0; i < 2; i++) {
        file.path.keys[i].camera.active = 1;
        file.path.keys[i].progress = (Float)(track + i);
        file.position[i].camera.active = 1;
        file.position[i].camera.target.x = (Float)(track + i);
    }
}

static void check_keys(int track)
{
    struct race_path_cell *path = race_path_head;
    struct race_position_cell *position = race_position_head;

    assert(live_cells == 4);
    assert(race_path_count == 2 && race_position_count == 2);
    assert(path && path->prev == NULL && path->next);
    assert(position && position->prev == NULL && position->next);
    assert(path->next->prev == path && path->next->next == NULL);
    assert(position->next->prev == position && position->next->next == NULL);
    assert(path->key.progress == track + 1 &&
           path->next->key.progress == track);
    assert(position->key.camera.target.x == track + 1);
    assert(position->next->key.camera.target.x == track);
}

int main(void)
{
    int i;

    expected_name = "ChaoRaceCamBegSwm.bin";
    set_keys(0);
    memset(WORK_AREA, 0x5a, sizeof(scratch.bytes));
    assert(race_load_camera_keys(0));
    check_keys(0);
    assert(memcmp(&race_camera_script, &file, sizeof(file)) == 0);
    race_edit_path = race_path_head->next;
    race_edit_position = race_position_head;
    previous = race_camera_script;

    expected_name = "ChaoRaceCamJwlLng.bin";
    set_keys(9);
    missing = 1;
    assert(!race_load_camera_keys(9));
    assert(race_edit_path == race_path_head->next);
    assert(race_edit_position == race_position_head);
    assert(memcmp(&previous, &race_camera_script, sizeof(previous)) == 0);
    check_keys(0);
    missing = 0;

    for (i = 1; i <= 4; i++) {
        allocations = 0;
        fail_at = i;
        assert(!race_load_camera_keys(9));
        assert(allocations == i);
        assert(race_edit_path == race_path_head->next);
        assert(race_edit_position == race_position_head);
        assert(memcmp(&previous, &race_camera_script, sizeof(previous)) == 0);
        check_keys(0);
    }
    allocations = fail_at = 0;
    assert(race_load_camera_keys(9));
    check_keys(9);
    assert(race_edit_path == NULL && race_edit_position == NULL);
    assert(memcmp(&race_camera_script, &file, sizeof(file)) == 0);
    race_clear_camera_keys();
    assert(race_path_count == 0 && race_position_count == 0);
    assert(race_path_head == NULL && race_position_head == NULL);
    assert(live_cells == 0);
    return 0;
}
