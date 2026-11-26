/* Read synthetic stage camera files through the real GD and PRS loader paths.
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "camera/camlin.h"
#include "camera/zones.h"
#include "system/game_state.h"
#include "system/loader.h"

#define GUARD 32
#define CAMSET_BYTES 0x10000
#define PLAYER_BYTES 0x4000
#define CAMERA_BYTES (CAMSET_BYTES + 4 * PLAYER_BYTES)
#define WORK_BYTES 0x200000

static union {
    Uint32 aligned;
    Uint8 bytes[GUARD + CAMERA_BYTES + GUARD];
} memory;
static union {
    Uint32 aligned;
    Uint8 bytes[GUARD + WORK_BYTES + GUARD];
} work;
#undef CAMSET_AREA
#undef CAMSET_AREA_0
#undef CAMSET_AREA_1
#undef CAMLIN_AREA_0
#undef CAMLIN_AREA_1
#undef CAMLIN_AREA_END
#undef WORK_AREA
#undef WORK_AREA_END
#define CAMSET_AREA (memory.bytes + GUARD)
#define CAMSET_AREA_0 (CAMSET_AREA + CAMSET_BYTES)
#define CAMSET_AREA_1 (CAMSET_AREA_0 + PLAYER_BYTES)
#define CAMLIN_AREA_0 (CAMSET_AREA_1 + PLAYER_BYTES)
#define CAMLIN_AREA_1 (CAMLIN_AREA_0 + PLAYER_BYTES)
#define CAMLIN_AREA_END (CAMLIN_AREA_1 + PLAYER_BYTES)
#define WORK_AREA (work.bytes + GUARD)
#define WORK_AREA_END (WORK_AREA + WORK_BYTES)
#include "system/loader.c"

struct game_state game_state;

enum read_result { AVAILABLE, SHORT_TRANSFER, READ_ERROR };

struct disc_file {
    const char *name;
    const void *data;
    Sint32 size;
    enum read_result result;
};

static struct disc_file files[6];
static int file_count, closed, queries, finished, second_player;
static struct disc_file *reading;
static struct GDS_FS_HANDLE handle;
static struct {
    Sint32 count;
    Sint32 sizes[CAMERA_PARTS];
    Sint32 reserved[3];
    struct camera_zone zone;
    struct camlin_node line;
    struct camera_zone demo_zone;
    struct camlin_node demo_line;
} sample;
static Uint8 packed[1024];
static Uint8 expected[CAMERA_BYTES];

int stage_current_id(void)
{
    return 13;
}

int player_character(int slot)
{
    assert(slot == 1);
    return second_player ? 4 : -1;
}

int stage_character(int slot)
{
    assert(slot == 0 || slot == 1);
    return slot == 0 ? 0 : 4;
}

GDFS gdFsOpen(const char *name, GDFS_DIRREC directory)
{
    int i;

    assert(directory == NULL && reading == NULL);
    queries++;
    for (i = 0; i < file_count; i++) {
        if (strcmp(name, files[i].name) == 0) {
            reading = &files[i];
            return &handle;
        }
    }
    return NULL;
}

Bool gdFsGetFileSize(GDFS file, Sint32 *size)
{
    assert(file == &handle && reading != NULL);
    *size = reading->size;
    return TRUE;
}

Sint32 gdFsRead(GDFS file, Sint32 sectors, void *dest)
{
    assert(file == &handle && reading != NULL && sectors == 1);
    /* A GD transfer can write a sector before it reports a short read. */
    memset(dest, 0, GDD_FS_SCTSIZE);
    memcpy(dest, reading->data, reading->size);
    return reading->result == READ_ERROR ? GDD_ERR_READ : GDD_ERR_OK;
}

Sint32 gdFsTell(GDFS file)
{
    assert(file == &handle && reading != NULL);
    return reading->result == SHORT_TRANSFER ? 0 : 1;
}

void gdFsClose(GDFS file)
{
    assert(file == &handle && reading != NULL);
    reading = NULL;
    closed++;
}

/* every read that fails ends at gdFsStopRd, and no load here stops the
 * game */
Sint32 gdFsStopRd(GDFS file)
{
    assert(file == &handle);
    return GDD_ERR_OK;
}

Sint32 gdFsGetStat(GDFS file)
{
    assert(file == &handle);
    return GDD_STAT_COMPLETE;
}

Bool gdFsIsTrayOpen(void)
{
    return FALSE;
}

void port_stop(const char *what, const char *name, Uint32 number)
{
    abort();
}

void *syMalloc(Uint32 size)
{
    return malloc(size);
}

void syFree(void *block)
{
    free(block);
}

void camset_free_marked(void)
{
    assert(memcmp(CAMSET_AREA, expected, sizeof(expected)) == 0);
    finished++;
}

static void add_file(const char *name, const void *data, size_t size,
                     enum read_result result)
{
    struct disc_file *file;

    assert(file_count < 6 && size <= GDD_FS_SCTSIZE);
    file = &files[file_count++];
    file->name = name;
    file->data = data;
    file->size = (Sint32)size;
    file->result = result;
}

static void add_packed(size_t size)
{
    const Uint8 *bytes = (const Uint8 *)&sample;
    Uint8 *out = packed;
    size_t i;

    assert(size % 8 == 0 && size * 9 / 8 + 3 <= sizeof(packed));
    for (i = 0; i < size; i += 8) {
        *out++ = 0xff;
        memcpy(out, bytes + i, 8);
        out += 8;
    }
    *out++ = 2;
    *out++ = 0;
    *out++ = 0;
    add_file("stg13cam.prs", packed, out - packed, AVAILABLE);
}

static void expect_part(const Uint8 *dest, const void *src, size_t size)
{
    memcpy(expected + (dest - CAMSET_AREA), src, size);
}

static void start(void)
{
    memset(expected, 0, sizeof(expected));
    file_count = queries = closed = finished = 0;
}

static void load(void)
{
    int i;

    assert(reading == NULL);
    loader_read_camera_files();
    assert(finished == 1 && queries >= 1 && reading == NULL);
    assert(memcmp(CAMSET_AREA, expected, sizeof(expected)) == 0);
    for (i = 0; i < GUARD; i++) {
        assert(memory.bytes[i] == 0xa5 && CAMLIN_AREA_END[i] == 0xa5);
        assert(work.bytes[i] == 0x5a && WORK_AREA_END[i] == 0x5a);
    }
}

int main(void)
{
    int i;

    memset(&memory, 0xa5, sizeof(memory));
    memset(&work, 0x5a, sizeof(work));
    sample.count = 5;
    sample.sizes[0] = sample.sizes[2] = sizeof(sample.zone);
    sample.sizes[1] = sample.sizes[3] = sizeof(sample.line);
    sample.zone.shape = ZONE_SPHERE;
    sample.zone.pos.x = 4.0F;
    sample.demo_zone = sample.zone;
    sample.demo_zone.pos.x = 7.0F;
    sample.line.used = 1;
    sample.line.player_pos.x = 3.0F;
    for (i = 0; i < CAMLIN_LINKS; i++) {
        sample.line.links[i] = CAMLIN_NONE;
    }
    sample.line.next = CAMLIN_NONE;
    sample.demo_line = sample.line;
    sample.demo_line.player_pos.x = 9.0F;

    /* Each packed variant supplies shared zones and both players' lines. */
    start();
    add_packed(sizeof(sample));
    expect_part(CAMSET_AREA, &sample.zone, sizeof(sample.zone));
    expect_part(CAMLIN_AREA_0, &sample.line, sizeof(sample.line));
    expect_part(CAMLIN_AREA_1, &sample.line, sizeof(sample.line));
    load();
    assert(closed == 1);
    game_state.demo = DEMO_ON_NO_FILE;
    start();
    add_packed(sizeof(sample));
    expect_part(CAMSET_AREA, &sample.demo_zone, sizeof(sample.demo_zone));
    expect_part(CAMLIN_AREA_0, &sample.demo_line, sizeof(sample.demo_line));
    expect_part(CAMLIN_AREA_1, &sample.demo_line, sizeof(sample.demo_line));
    load();
    assert(closed == 1);

    /* An archive with three parts has demo zones, but no demo lines. */
    start();
    sample.count = 4;
    sample.sizes[3] = 0;
    add_packed(sizeof(sample) - sizeof(sample.demo_line));
    expect_part(CAMSET_AREA, &sample.demo_zone, sizeof(sample.demo_zone));
    load();
    sample.count = 5;
    sample.sizes[3] = sizeof(sample.demo_line);

    /* A missing packed demo selects the individual, character-specific files.
     */
    second_player = 1;
    start();
    add_file("camset0013D.bin", &sample.zone, sizeof(sample.zone), AVAILABLE);
    add_file("camset0013SD.bin", &sample.demo_zone, sizeof(sample.demo_zone),
             AVAILABLE);
    add_file("camset0013KD.bin", &sample.zone, sizeof(sample.zone), AVAILABLE);
    add_file("camlin0013D.bin", &sample.line, sizeof(sample.line), AVAILABLE);
    expect_part(CAMSET_AREA, &sample.zone, sizeof(sample.zone));
    expect_part(CAMSET_AREA_0, &sample.demo_zone, sizeof(sample.demo_zone));
    expect_part(CAMSET_AREA_1, &sample.zone, sizeof(sample.zone));
    expect_part(CAMLIN_AREA_0, &sample.line, sizeof(sample.line));
    expect_part(CAMLIN_AREA_1, &sample.line, sizeof(sample.line));
    load();

    /* A partly transferred raw table stays empty. Other files finish normally.
     */
    game_state.demo = 0;
    second_player = 0;
    start();
    add_file("camset0013.bin", &sample.zone, sizeof(sample.zone),
             SHORT_TRANSFER);
    add_file("camset0013S.bin", &sample.demo_zone, sizeof(sample.demo_zone),
             READ_ERROR);
    add_file("camlin0013.bin", &sample.line, sizeof(sample.line), AVAILABLE);
    expect_part(CAMLIN_AREA_0, &sample.line, sizeof(sample.line));
    load();
    assert(closed == 3);

    /* Reject an incomplete last part before publishing the first pair. */
    start();
    sample.sizes[3] += sizeof(sample.line);
    add_packed(sizeof(sample));
    add_file("camset0013S.bin", &sample.demo_zone, sizeof(sample.demo_zone),
             AVAILABLE);
    expect_part(CAMSET_AREA_0, &sample.demo_zone, sizeof(sample.demo_zone));
    load();
    sample.sizes[3] = sizeof(sample.line);
    start();
    sample.count++;
    add_packed(sizeof(sample));
    load();

    /* A fresh valid file is still accepted after both rejected reads. */
    start();
    sample.count = 5;
    add_packed(sizeof(sample));
    expect_part(CAMSET_AREA, &sample.zone, sizeof(sample.zone));
    expect_part(CAMLIN_AREA_0, &sample.line, sizeof(sample.line));
    expect_part(CAMLIN_AREA_1, &sample.line, sizeof(sample.line));
    load();
    start();
    load();
    assert(closed == 0);
    return 0;
}
