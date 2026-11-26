/* The resident language pair through the sector reader and PRS decoder. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <shinobi.h>

#include "system/areas.h"
#include "system/text.h"

static Uint8 scratch[GDD_FS_SCTSIZE * 2];
#undef WORK_AREA
#undef WORK_AREA_END
#define WORK_AREA scratch
#define WORK_AREA_END (scratch + sizeof(scratch))
#include "system/loader.c"

struct game_state game_state;
static struct GDS_FS_HANDLE handle;
static FILE *disc_file;
static const char *missing_name;
static int sectors_read;
static int live_buffers;
static Uint32 unavailable_size;

GDFS gdFsOpen(const char *name, GDFS_DIRREC directory)
{
    assert(directory == NULL && disc_file == NULL);
    if (missing_name != NULL && strcmp(name, missing_name) == 0) {
        return NULL;
    }
    disc_file = fopen(name, "rb");
    return disc_file != NULL ? &handle : NULL;
}

Bool gdFsGetFileSize(GDFS file, Sint32 *size)
{
    assert(file == &handle);
    fseek(disc_file, 0, SEEK_END);
    *size = (Sint32)ftell(disc_file);
    rewind(disc_file);
    return TRUE;
}

Sint32 gdFsRead(GDFS file, Sint32 sectors, void *dest)
{
    assert(file == &handle && sectors > 0);
    memset(dest, 0, sectors * GDD_FS_SCTSIZE);
    if (fread(dest, 1, sectors * GDD_FS_SCTSIZE, disc_file) == 0 ||
        ferror(disc_file)) {
        return GDD_ERR_READ;
    }
    sectors_read = sectors;
    return GDD_ERR_OK;
}

Sint32 gdFsTell(GDFS file)
{
    assert(file == &handle);
    return sectors_read;
}

void gdFsClose(GDFS file)
{
    assert(file == &handle);
    fclose(disc_file);
    disc_file = NULL;
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

void *syCalloc(Uint32 count, Uint32 size)
{
    void *buffer;
    assert(count == 1 && (size == TEXT_SIZE || size == MCWARN_SIZE));
    if (size == unavailable_size) {
        return NULL;
    }
    buffer = calloc(count, size);
    if (buffer != NULL) {
        live_buffers++;
    }
    return buffer;
}

void syFree(void *buffer)
{
    assert(buffer != NULL && live_buffers > 0);
    free(buffer);
    live_buffers--;
}

int main(void)
{
    Sint32 *first;

    game_state.text_language = TEXT_ENGLISH;
    unavailable_size = MCWARN_SIZE;
    text_alloc();
    assert(!text_read() && !text_file && !mcwarn_file && live_buffers == 1);
    unavailable_size = 0;
    missing_name = "text_e.prs";
    assert(!text_read() && !text_file && !mcwarn_file);
    missing_name = NULL;
    assert(text_read());
    first = text_file;
    assert(live_buffers == 2 && strcmp(TEXT(2), "action e") == 0);
    assert(strcmp(MCWARN(7), "notice e") == 0);
    text_alloc();
    assert(text_file == first && live_buffers == 2);

    game_state.text_language = TEXT_JAPANESE;
    missing_name = "mcwarn_j.prs";
    assert(!text_read() && !text_file && !mcwarn_file);
    missing_name = NULL;
    assert(text_read() && text_file == first && live_buffers == 2);
    assert(strcmp(TEXT(143), "action j") == 0);
    assert(strcmp(MCWARN(3), "notice j") == 0);

    text_free();
    assert(!text_file && !mcwarn_file && live_buffers == 0);
    text_free();
    text_alloc();
    assert(text_read() && strcmp(TEXT(2), "action j") == 0);
    text_free();
    assert(live_buffers == 0);
    return 0;
}
