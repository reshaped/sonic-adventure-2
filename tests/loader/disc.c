/* Exercise whole-sector GD-ROM reads with the same scratch space on every call.
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "system/areas.h"
#include "system/loader.h"

static Uint8 scratch[GDD_FS_SCTSIZE * 2];
#undef WORK_AREA
#undef WORK_AREA_END
#define WORK_AREA scratch
#define WORK_AREA_END (scratch + sizeof(scratch))
#include "system/loader.c"

struct game_state game_state;
static struct GDS_FS_HANDLE handle;
static Sint32 file_bytes;
static Sint32 file_sectors;
static Sint32 read_status;
static int missing;
static int size_failed;
static int heap_full;
static int reads;
static int opens;
static int closes;
static int allocations;

GDFS gdFsOpen(const char *name, GDFS_DIRREC directory)
{
    assert(strcmp(name, "stage.bin") == 0 && directory == NULL);
    opens++;
    return missing ? NULL : &handle;
}

Bool gdFsGetFileSize(GDFS file, Sint32 *size)
{
    assert(file == &handle);
    if (size_failed) {
        return FALSE;
    }
    *size = file_bytes;
    return TRUE;
}

Sint32 gdFsRead(GDFS file, Sint32 sectors, void *dest)
{
    assert(file == &handle && sectors > 0);
    assert(sectors ==
           file_bytes / GDD_FS_SCTSIZE + (file_bytes % GDD_FS_SCTSIZE != 0));
    assert((Uint32)sectors * GDD_FS_SCTSIZE <= sizeof(scratch));
    reads++;
    memset(dest, 0x5a, sectors * GDD_FS_SCTSIZE);
    return read_status;
}

Sint32 gdFsTell(GDFS file)
{
    assert(file == &handle);
    return file_sectors;
}

void gdFsClose(GDFS file)
{
    assert(file == &handle);
    closes++;
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
    allocations++;
    assert(size == (Uint32)file_bytes);
    return heap_full ? NULL : malloc(size);
}

int main(void)
{
    Uint8 *copy;

    missing = 1;
    assert(loader_read_alloc("stage.bin") == NULL && closes == 0);
    missing = 0;
    size_failed = 1;
    assert(loader_read_alloc("stage.bin") == NULL);
    size_failed = 0;
    file_bytes = -1;
    assert(loader_read_alloc("stage.bin") == NULL);
    file_bytes = 0x7fffffffL;
    assert(loader_read_alloc("stage.bin") == NULL);

    file_bytes = GDD_FS_SCTSIZE + 1;
    file_sectors = 2;
    assert(loader_read_file_checked("stage.bin", scratch, file_bytes) == -1);
    assert(loader_read_file_checked("stage.bin", NULL, sizeof(scratch)) == -1);
    assert(reads == 0);
    read_status = GDD_ERR_READ;
    assert(loader_read_alloc("stage.bin") == NULL);
    read_status = GDD_ERR_OK;
    file_sectors = 1;
    assert(loader_read_alloc("stage.bin") == NULL);
    assert(allocations == 0);

    file_sectors = 2;
    heap_full = 1;
    assert(loader_read_alloc("stage.bin") == NULL);
    heap_full = 0;
    copy = loader_read_alloc("stage.bin");
    assert(copy != NULL && memcmp(copy, scratch, file_bytes) == 0);
    free(copy);
    file_bytes = 0;
    assert(loader_read_file_checked("stage.bin", NULL, 0) == 0);
    assert(reads == 4 && closes == opens - 1);
    return 0;
}
