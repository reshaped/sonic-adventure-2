/* A failed optical read must leave the stage's current lighting intact. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "system/loader.h"

static Uint8 sector[GDD_FS_SCTSIZE];
#undef WORK_AREA
#define WORK_AREA sector
#include "system/loader.c"
#include "shared/object_light.c"

struct game_state game_state;
struct light lights[LIGHTS];
static struct light file_lights[LIGHTS];
static struct GDS_FS_HANDLE handle;
static Sint32 file_size;
static Bool missing;
static Bool size_known;
static Sint32 read_status;
static Sint32 tell_sector;

GDFS gdFsOpen(const char *name, GDFS_DIRREC directory)
{
    assert(strcmp(name, "stg11_light.bin") == 0 && directory == NULL);
    return missing ? NULL : &handle;
}

Bool gdFsGetFileSize(GDFS file, Sint32 *size)
{
    assert(file == &handle);
    *size = file_size;
    return size_known;
}

Sint32 gdFsRead(GDFS file, Sint32 sectors, void *dest)
{
    assert(file == &handle && sectors == 1 && dest == sector);
    memset(dest, 0x6d, GDD_FS_SCTSIZE);
    memcpy(dest, file_lights, sizeof(file_lights));
    return read_status;
}

Sint32 gdFsTell(GDFS file)
{
    assert(file == &handle);
    return tell_sector;
}

void gdFsClose(GDFS file)
{
    assert(file == &handle);
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

static void expect_unchanged(void)
{
    struct light previous[LIGHTS];

    memcpy(previous, lights, sizeof(previous));
    assert(!lights_load("stg11_light.bin"));
    assert(memcmp(lights, previous, sizeof(lights)) == 0);
}

int main(void)
{
    int index;
    static const Sint32 invalid_sizes[] = {
        -1,
        0,
        sizeof(struct light),
        sizeof(lights) - 1,
        sizeof(lights) + 1,
        GDD_FS_SCTSIZE + 1,
    };

    assert(sizeof(lights) == 384);
    for (index = 0; index < LIGHTS; ++index) {
        file_lights[index].ambient = (Float)(index + 1);
        lights[index].intensity = 0.5F;
    }
    file_size = sizeof(lights);
    read_status = GDD_ERR_OK;
    tell_sector = 1;
    missing = TRUE;
    expect_unchanged();
    missing = FALSE;
    expect_unchanged();
    size_known = TRUE;
    assert(lights_load("stg11_light.bin"));
    assert(memcmp(lights, file_lights, sizeof(lights)) == 0);

    file_lights[0].ambient = 0.25F;
    for (index = 0; index < (int)(sizeof(invalid_sizes) / sizeof(Sint32));
         ++index) {
        file_size = invalid_sizes[index];
        expect_unchanged();
    }
    file_size = sizeof(lights);
    read_status = GDD_ERR_READ;
    expect_unchanged();
    read_status = GDD_ERR_OK;
    tell_sector = 0;
    expect_unchanged();
    tell_sector = 1;
    assert(lights_load("stg11_light.bin"));
    assert(memcmp(lights, file_lights, sizeof(lights)) == 0);
    return 0;
}
