/* Capture the exact data and bank selected at the sound library boundary. */
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shinobi.h>
#include "system/areas.h"
#include "system/loader.h"

static Uint32 scratch[2048];
static int cache_flushes;
#undef WORK_AREA
#undef WORK_AREA_END
#define WORK_AREA ((Uint8 *)scratch)
#define WORK_AREA_END (WORK_AREA + sizeof(scratch))
#undef syCacheICI
#define syCacheICI() ((void)++cache_flushes)
#include "system/loader.c"
#include "sound/sound.c"

struct game_state game_state;
static struct GDS_FS_HANDLE file_handle;
static FILE *disc_file;
static const char *unavailable;
static const char *opened;
static Sint32 sectors_read;
static Bool read_error;
static Bool transfer_error;
static int allocations;
static int library;
static int block;
static Sint32 bytes;
static int drivers, units, banks;
static int ports_open;
static jmp_buf stopped;
/* whether a stop may return to stopped */
static Bool stopping;
/* what the last stop gave port_stop */
static const char *stop_what;
static const char *stop_name;
static Uint32 stop_number;

GDFS gdFsOpen(const char *name, GDFS_DIRREC directory)
{
    assert(disc_file == NULL && directory == NULL);
    if (unavailable != NULL && strcmp(name, unavailable) == 0) {
        return NULL;
    }
    disc_file = fopen(name, "rb");
    if (disc_file == NULL) {
        return NULL;
    }
    opened = name;
    sectors_read = 0;
    return &file_handle;
}

Bool gdFsGetFileSize(GDFS handle, Sint32 *size)
{
    assert(handle == &file_handle);
    fseek(disc_file, 0, SEEK_END);
    *size = (Sint32)ftell(disc_file);
    rewind(disc_file);
    return TRUE;
}

Sint32 gdFsRead(GDFS handle, Sint32 sectors, void *dest)
{
    size_t width = (size_t)sectors * GDD_FS_SCTSIZE;
    assert(handle == &file_handle && width <= sizeof(scratch));
    if (read_error && strcmp(opened, "se_ch_ss.prs") == 0) {
        return GDD_ERR_READ;
    }
    memset(dest, 0, width);
    assert(fread(dest, 1, width, disc_file) > 0);
    sectors_read = sectors;
    return GDD_ERR_OK;
}

Sint32 gdFsTell(GDFS handle)
{
    assert(handle == &file_handle);
    return sectors_read;
}

void gdFsClose(GDFS handle)
{
    assert(handle == &file_handle && disc_file != NULL);
    fclose(disc_file);
    disc_file = NULL;
}

/* every read that fails ends at gdFsStopRd */
Sint32 gdFsStopRd(GDFS handle)
{
    assert(handle == &file_handle);
    return GDD_ERR_OK;
}

Sint32 gdFsGetStat(GDFS handle)
{
    assert(handle == &file_handle);
    return GDD_STAT_COMPLETE;
}

Bool gdFsIsTrayOpen(void)
{
    return FALSE;
}

/* keep what the stop shows, and return to expect_stop */
void port_stop(const char *what, const char *name, Uint32 number)
{
    assert(stopping);
    stopping = FALSE;
    stop_what = what;
    stop_name = name;
    stop_number = number;
    longjmp(stopped, 1);
}

void *syMalloc(Uint32 size)
{
    return malloc(size);
}
void *syCalloc(Uint32 count, Uint32 size)
{
    assert(count == 1 && size == sizeof(struct sound_work));
    allocations++;
    return calloc(count, size);
}

void syFree(void *data)
{
    if (data == sound_work) {
        allocations--;
    }
    free(data);
}

SDE_ERR sdLibInit(void *work, Sint32 handles, Sint32 commands)
{
    assert(!library);
    library = 1;
    return SDE_ERR_NOTHING;
}
SDE_ERR sdSysFinish(void)
{
    assert(library && !block);
    library = 0;
    return SDE_ERR_NOTHING;
}
SDE_ERR sdMemBlkSetTransferMode(SDE_MEMBLK_TRANSFER_MODE mode)
{
    assert(library && mode == SDE_MEMBLK_TRANSFER_MODE_DMA);
    return SDE_ERR_NOTHING;
}
SDE_ERR sdMemBlkCreate(SDMEMBLK *handle)
{
    assert(library && !block);
    block = 1;
    *handle = (SDMEMBLK)&block;
    return SDE_ERR_NOTHING;
}
SDE_ERR sdMemBlkSetPrm(SDMEMBLK handle, const void *data, Sint32 size,
                       SD_MEMBLK_CALLBACK_FUNC callback, const void *arg)
{
    assert(handle == (SDMEMBLK)&block && block && data == WORK_AREA);
    assert(size > 0 && callback == SDD_MEMBLK_SYNC_FUNC && arg == NULL);
    bytes = size;
    return SDE_ERR_NOTHING;
}
SDE_ERR sdMemBlkDestroy(SDMEMBLK handle)
{
    assert(handle == (SDMEMBLK)&block && block);
    block = 0;
    return SDE_ERR_NOTHING;
}
SDE_ERR sdDrvInit(SDMEMBLK handle)
{
    assert(block && handle == (SDMEMBLK)&block);
    assert(bytes == 64 && memcmp(WORK_AREA, "SDRV", 4) == 0);
    drivers++;
    return transfer_error ? SDE_ERR_MEMBLK_QUEUE_NO_ENOUGH : SDE_ERR_NOTHING;
}
SDE_ERR sdMultiUnitDownload(SDMEMBLK handle)
{
    assert(block && handle == (SDMEMBLK)&block);
    assert(memcmp(WORK_AREA, "SMLT", 4) == 0);
    units++;
    return transfer_error ? SDE_ERR_MEMBLK_QUEUE_NO_ENOUGH : SDE_ERR_NOTHING;
}
SDE_ERR sdBankDownload(SDMEMBLK handle, SDE_DATA_TYPE type, Sint8 bank)
{
    assert(block && handle == (SDMEMBLK)&block);
    assert(type == SDE_DATA_TYPE_MIDI_PRG_BANK && bank == 3);
    assert(bytes == 32 && memcmp(WORK_AREA, "SMPB", 4) == 0);
    banks++;
    return transfer_error ? SDE_ERR_MEMBLK_QUEUE_NO_ENOUGH : SDE_ERR_NOTHING;
}

int stage_current_id(void)
{
    return STAGE_CITY_ESCAPE;
}
int stage_character(int slot)
{
    return PLAYER_SONIC;
}
int player_character(int slot)
{
    return PLAYER_SONIC;
}
int player_find_rival(int slot)
{
    return 1;
}

/* start the sound system, which must stop the game as what, with number and
 * the driver's name */
static void expect_stop(const char *what, Uint32 number)
{
    stopping = TRUE;
    if (setjmp(stopped) == 0) {
        sound_system_init();
        assert(!"no stop");
    }
    assert(strcmp(stop_what, what) == 0 && stop_number == number);
    assert(strcmp(stop_name, "manatee.drv") == 0);
}

int main(void)
{
    const struct sound_list *sounds;
    const struct sound_list *voices;
    const struct sound_list *fallback_sounds;
    const struct sound_list *fallback_voices;
    const struct sound_list *lists[SOUND_LISTS] = {0};
    /* the game can't go on without its driver */
    unavailable = "manatee.drv";
    expect_stop("OPEN", 0);
    assert(!drivers && !sound_work && !allocations && !library && !ports_open);
    unavailable = NULL;
    transfer_error = TRUE;
    expect_stop("SOUND DRIVER", SDE_ERR_MEMBLK_QUEUE_NO_ENOUGH);
    assert(drivers == 1 && !block && !ports_open);
    /* the stopped game left the library started and the work allocated. a
     * console that boots again starts from neither */
    syFree(sound_work);
    sound_work = NULL;
    library = 0;
    transfer_error = FALSE;
    sound_system_init();
    assert(drivers == 2 && sound_work && allocations == 1);
    assert(ports_open == SDD_MIDI_PORT_NUM && units == 1 && banks == 1);

    assert(sound_load_file("se_ch_ss.mlt") == 0 && bytes == 32);
    assert(sound_load_file("e_test.mlt") == 0 && bytes == 64);
    unavailable = "se_ch_ss.prs";
    assert(sound_load_file("se_ch_ss.mlt") == 0 && bytes == 64);
    unavailable = NULL;
    read_error = TRUE;
    assert(sound_load_file("se_ch_ss.mlt") == -1 && units == 4);
    assert(cache_flushes == 4);
    assert(sound_load_file("missing.mlt") == -1 && units == 4);
    read_error = FALSE;
    assert(sound_load_file("se_ch_ss.mlt") == 0 && units == 5);

    transfer_error = TRUE;
    sound_load_character(PLAYER_SONIC, &sounds, &voices);
    assert(!block && sounds == NULL && voices == NULL);
    unavailable = "b03_sn.mpb";
    transfer_error = FALSE;
    assert(sound_load_bank_untested(3, unavailable) == -1);
    sound_load_character(PLAYER_SONIC, &sounds, &voices);
    assert(sounds != NULL && voices == NULL && banks == 2);
    unavailable = NULL;
    sound_load_character(PLAYER_SONIC, &sounds, &voices);
    assert(sounds != NULL && voices != NULL && banks == 3);
    /* Super Sonic has a one-player voice entry and shares Sonic's fallback
     * effects. The shorter effects table is exhausted in both calls. */
    sound_load_character(PLAYER_SUPER_SONIC, &fallback_sounds,
                         &fallback_voices);
    assert(fallback_sounds == sounds && fallback_voices == voices);
    sound_lists_set_character(PLAYER_SUPER_SONIC, lists);
    assert(lists[SOUND_LIST_CHARACTER] == sounds &&
           lists[SOUND_LIST_VOICES] == voices);
    assert(!block && !disc_file);
    syFree(sound_work);
    sound_work = NULL;
    return 0;
}

/* The real initialization goes on to open eight MIDI ports and reset their
 * levels. These calls follow the driver download and consume no file data. */
SDE_ERR sdMidiOpenPort(SDMIDI *handle)
{
    assert(library && drivers == 2 && !block);
    *handle = (SDMIDI)&ports_open;
    ports_open++;
    return SDE_ERR_NOTHING;
}
SDE_ERR sdMidiGetStat(SDMIDI port, SDS_MIDI_STAT *stat)
{
    memset(stat, 0, sizeof(*stat));
    return SDE_ERR_NOTHING;
}
SDE_ERR sdSysSetSlotMax(Sint32 midi, Sint32 shots, Sint32 streams)
{
    assert(midi == DRIVER_MIDI_SLOTS && shots == DRIVER_SHOT_SLOTS &&
           streams == DRIVER_STREAM_SLOTS);
    return SDE_ERR_NOTHING;
}
SDE_ERR sdSysFlushHostCmd(void)
{
    return SDE_ERR_NOTHING;
}
SDE_ERR sdSysHostCmdBufFlush(SDE_HOST_CMD_FLUSH_MODE mode)
{
    return SDE_ERR_NOTHING;
}
SDE_ERR sdSndSetMasterVol(Sint8 volume)
{
    return SDE_ERR_NOTHING;
}
SDE_ERR sdSndSetPanMode(SDE_PAN_MODE mode)
{
    return SDE_ERR_NOTHING;
}
SDE_ERR sdSndSetFxPrg(Sint8 program, Sint8 output)
{
    return SDE_ERR_NOTHING;
}
SDE_ERR sdMidiResetAllPrm(void)
{
    return SDE_ERR_NOTHING;
}
SDE_ERR sdMidiPlay(SDMIDI port, Sint8 bank, Sint8 number, Sint8 priority)
{
    return SDE_ERR_NOTHING;
}
SDE_ERR sdMidiStop(SDMIDI port)
{
    return SDE_ERR_NOTHING;
}
SDE_ERR sdMidiSetFxLev(SDMIDI port, Sint8 level)
{
    return SDE_ERR_NOTHING;
}
SDE_ERR sdMidiSetMes(SDMIDI port, SDS_MIDI_MES *message, Uint8 status, ...)
{
    return SDE_ERR_NOTHING;
}
SDE_ERR sdMidiSendMes(SDMIDI port, const SDS_MIDI_MES *message, Sint8 priority)
{
    return SDE_ERR_NOTHING;
}
Uint32 syTmrGetCount(void)
{
    return 0;
}
Uint32 syTmrDiffCount(Uint32 first, Uint32 second)
{
    return second - first;
}
Uint32 syTmrCountToMicro(Uint32 count)
{
    return count;
}
