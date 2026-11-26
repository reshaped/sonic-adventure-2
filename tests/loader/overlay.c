/* Exercise the resident's admission boundary with complete, literal PRS
 * images, and the buffered PRS read, in place and through the heap. A stage
 * that can't load stops the game, and the screen names its file and how the
 * load failed. An attract demo's file loads into its whole room. */
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "engine/dc/crash_report.h"
#include "system/loader.h"
#include "system/game_state.h"

static void invalidate_instructions(void);
static Uint8 scratch[4096];
/* the stage slot, then a demo file's room, then the bytes past it */
static struct {
    union {
        struct program_overlay head;
        Uint8 bytes[4096];
    } slot;
    Uint8 demo[512];
    Uint8 past[16];
} memory;
#undef WORK_AREA
#undef WORK_AREA_END
#undef STAGE_SLOT
#undef DEMO_AREA
#undef DEMO_AREA_BYTES
#undef DEMO_FILE_BYTES
#undef syCacheICI
#define WORK_AREA scratch
#define WORK_AREA_END (scratch + sizeof(scratch))
#define STAGE_SLOT memory.slot.bytes
#define DEMO_AREA memory.demo
/* a mini event's file must end 64 bytes short of a demo file's room, as
 * MESSAGE_AREA starts 64 KB short of CAMSET_AREA */
#define DEMO_AREA_BYTES (sizeof(memory.demo) - 64)
#define DEMO_FILE_BYTES sizeof(memory.demo)
#define syCacheICI() invalidate_instructions()
#include "system/loader.c"

struct game_state game_state;

static struct GDS_FS_HANDLE handle;
static Uint8 packed[4096];
static Sint32 packed_size;
/* demo13.prs, apart from the file every other name opens, and whether the
 * open handle is its */
static Uint8 demo_packed[1024];
static Sint32 demo_size;
static Bool demo_open;
static Sint32 status;
static Sint32 sectors_read;
static Bool missing;
static Bool heap_full;
static Uint8 *heap_block;
static Uint32 heap_size;
static int reads;
static int opens;
static int closes;
static int write_backs;
static int invalidations;
static int stale_calls;
/* what gdFsGetStat gives the handle, whatever gdFsStopRd did */
static Sint32 drive_state = GDD_STAT_COMPLETE;
static jmp_buf stopped;
/* whether a stop may return to stopped */
static Bool stopping;

static void old_callback(void)
{
    stale_calls++;
}

int stage_current_id(void)
{
    return STAGE_CITY_ESCAPE;
}

GDFS gdFsOpen(const char *name, GDFS_DIRREC directory)
{
    assert(name != NULL && directory == NULL);
    opens++;
    demo_open = strcmp(name, "demo13.prs") == 0;
    return missing ? NULL : &handle;
}

Bool gdFsGetFileSize(GDFS file, Sint32 *size)
{
    assert(file == &handle);
    *size = demo_open ? demo_size : packed_size;
    return TRUE;
}

Sint32 gdFsRead(GDFS file, Sint32 sectors, void *dest)
{
    const Uint8 *from = demo_open ? demo_packed : packed;
    Sint32 size = demo_open ? demo_size : packed_size;

    assert(file == &handle && (size_t)sectors * GDD_FS_SCTSIZE >= (size_t)size);
    reads++;
    memset(dest, 0x43, sectors * GDD_FS_SCTSIZE);
    memcpy(dest, from, size);
    return status;
}

Sint32 gdFsTell(GDFS file)
{
    assert(file == &handle);
    return sectors_read;
}

void gdFsClose(GDFS file)
{
    assert(file == &handle);
    closes++;
}

Sint32 gdFsStopRd(GDFS file)
{
    assert(file == &handle);
    return GDD_ERR_OK;
}

Sint32 gdFsGetStat(GDFS file)
{
    assert(file == &handle);
    return drive_state;
}

Bool gdFsIsTrayOpen(void)
{
    return FALSE;
}

/* engine/dc/crash.c's port_stop with no machine to read: it draws the stop's
 * lines into crash_screen, with no registers, mode, stage, slot or stack, and
 * returns to expect_stop */
void port_stop(const char *what, const char *name, Uint32 number)
{
    struct crash_stop stop = {0};

    assert(stopping);
    stopping = FALSE;
    stop.what = what;
    stop.number = number;
    stop.name = name;
    stop.callers = CRASH_NO_STACK;
    crash_show(&stop);
    longjmp(stopped, 1);
}

void *syMalloc(Uint32 size)
{
    assert(heap_block == NULL);
    heap_block = heap_full ? NULL : malloc(size + 1);
    if (heap_block != NULL) {
        heap_size = size;
        heap_block[size] = 0xa7;
    }
    return heap_block;
}

void syFree(void *block)
{
    assert(block != NULL && block == heap_block);
    assert(heap_block[heap_size] == 0xa7);
    free(block);
    heap_block = NULL;
}

void syCacheOCWB(void *start, Uint32 size)
{
    assert(start == STAGE_SLOT && size > 0);
    assert(write_backs == invalidations);
    write_backs++;
}

static void invalidate_instructions(void)
{
    assert(write_backs == invalidations + 1);
    invalidations++;
}

/* Emit complete groups of eight PRS literals into file, which has room
 * bytes, followed by the end marker. Return the stream's size. */
static Sint32 pack_file(Uint8 *file, size_t room, const void *data,
                        size_t length)
{
    const Uint8 *src = data;
    Uint8 *out = file;

    assert(length % 8 == 0 && length + length / 8 + 3 <= room);
    while (length != 0) {
        *out++ = 0xff;
        memcpy(out, src, 8);
        out += 8;
        src += 8;
        length -= 8;
    }
    *out++ = 2;
    *out++ = 0;
    *out++ = 0;
    return out - file;
}

static void pack(const void *data, size_t length)
{
    packed_size = pack_file(packed, sizeof(packed), data, length);
    sectors_read =
        packed_size / GDD_FS_SCTSIZE + (packed_size % GDD_FS_SCTSIZE != 0);
}

/* run load, which must stop the game, and check the first two lines of the
 * screen its stop drew */
static void expect_stop(void (*load)(void), const char *first,
                        const char *second)
{
    stopping = TRUE;
    if (setjmp(stopped) == 0) {
        load();
        assert(!"no stop");
    }
    assert(strcmp(crash_screen[0], first) == 0);
    assert(strcmp(crash_screen[1], second) == 0);
}

static void expect_inactive(void)
{
    OVERLAY_HEAD->init = old_callback;
    OVERLAY_HEAD->exit = old_callback;
    overlay_run_init();
    overlay_run_exit();
    assert(stale_calls == 0);
}

int main(void)
{
    union {
        struct program_overlay program;
        struct chao_area_head area;
        Uint8 bytes[128];
    } image;
    Uint8 buffer[GDD_FS_SCTSIZE];
    Uint8 frames[sizeof(memory.demo) + 8];
    Uint8 *copy;
    unsigned int i;
    static const Uint8 repeat[] = {0x15, 'a', 0xf8, 0xff, 0xff, 0, 0};
    static const Uint8 odd_file[] = {0x17, 1, 2, 3, 0, 0};

    memset(&image, 0, sizeof(image));
    memcpy(image.program.head.name, "EVENT   ", 8);
    image.program.head.unk_08 = 2;
    image.program.step = (int (*)(void))(STAGE_SLOT + 96);
    pack(&image, sizeof(image));
    assert(loader_start_program("event.prs", "EVENT   "));
    assert(PROGRAM_OVERLAY->step == image.program.step && write_backs == 1);

    missing = TRUE;
    assert(!loader_start_program("event.prs", "EVENT   "));
    expect_inactive();
    missing = FALSE;
    status = GDD_ERR_READ;
    assert(!loader_start_program("event.prs", "EVENT   "));
    expect_inactive();
    status = GDD_ERR_OK;
    sectors_read = 0;
    assert(!loader_start_program("event.prs", "EVENT   "));
    expect_inactive();
    sectors_read = 1;
    assert(!loader_start_program("ending.prs", "ENDING  "));
    expect_inactive();
    assert(write_backs == 1);
    assert(loader_start_program("event.prs", "EVENT   "));
    overlay_run_exit();
    expect_inactive();

    memset(&image, 0, sizeof(image));
    memcpy(image.area.name, "ALSTNEUT", 8);
    image.area.step = (void (*)(struct task *))(STAGE_SLOT + 96);
    pack(&image, sizeof(image));
    assert(loader_install_chao_area("neut.prs", "ALSTNEUT", STAGE_SLOT,
                                    sizeof(image)));
    assert(!loader_install_chao_area("neut.prs", "ALSTNEUT", STAGE_SLOT,
                                     sizeof(image) - 1));
    assert(!loader_install_chao_area("dark.prs", "ALSTDARK", STAGE_SLOT,
                                     sizeof(image)));
    assert(write_backs == 3 && invalidations == 3);

    memset(&image, 0, sizeof(image));
    memcpy(image.program.head.name, "ALGARDEN", 8);
    image.program.step = (int (*)(void))(STAGE_SLOT + 96);
    pack(&image, sizeof(image));
    assert(read_overlay("chao.prs", "ALGARDEN", sizeof(memory.slot), FALSE));
    loader_start_stage();
    /* An init that leaves its overlay no step stops the game, and names no
     * file. The screen's SLOT names the overlay on the console. */
    STAGE_OVERLAY->step = NULL;
    expect_stop(loader_start_stage, "NO STEP 00000000",
                "PC 00000000 SR 00000000");

    /* City Escape's overlay loads and starts. Then it fails to open, to read
     * and to end its read, and its head names another stage. */
    memset(&image, 0, sizeof(image));
    memcpy(image.program.head.name, "STG13   ", 8);
    image.program.step = (int (*)(void))(STAGE_SLOT + 96);
    pack(&image, sizeof(image));
    loader_prepare_stage();
    loader_start_stage();
    missing = TRUE;
    expect_stop(loader_prepare_stage, "OPEN 00000000", "STG13.PRS");
    missing = FALSE;
    status = GDD_ERR_READ;
    expect_stop(loader_prepare_stage, "READ ffffffed", "STG13.PRS");
    /* gdFsStopRd leaves the failed read's request running */
    drive_state = GDD_STAT_READ;
    expect_stop(loader_prepare_stage, "READ RUNS ON ffffffed", "STG13.PRS");
    drive_state = GDD_STAT_COMPLETE;
    status = GDD_ERR_OK;
    memcpy(image.program.head.name, "STG10   ", 8);
    pack(&image, sizeof(image));
    expect_stop(loader_prepare_stage, "HEAD 00000000", "STG13.PRS");

    /* An attract demo reads demo13.prs before the overlay. Each of the
     * disc's demo files is a dump of DEMO_AREA and of the mini event's three
     * areas past it, up to CAMSET_AREA. So a file of DEMO_FILE_BYTES loads
     * whole, past where a mini event's file must end. One that decodes to a
     * group more stops the game, and writes nothing. One a group short stops
     * it too: its last frames would be the bytes the RAM held before. */
    assert(DEMO_AREA == STAGE_SLOT + sizeof(memory.slot));
    memcpy(image.program.head.name, "STG13   ", 8);
    pack(&image, sizeof(image));
    for (i = 0; i < sizeof(frames); i++) {
        frames[i] = (Uint8)(i * 7 + 1);
    }
    memset(memory.past, 0x5a, sizeof(memory.past));
    game_state.demo = DEMO_ON;
    demo_size =
        pack_file(demo_packed, sizeof(demo_packed), frames, DEMO_FILE_BYTES);
    loader_prepare_stage();
    loader_start_stage();
    assert(memcmp(memory.demo, frames, sizeof(memory.demo)) == 0);
    memset(memory.demo, 0, sizeof(memory.demo));
    demo_size = pack_file(demo_packed, sizeof(demo_packed), frames,
                          DEMO_FILE_BYTES + 8);
    expect_stop(loader_prepare_stage, "PRS 00000000", "DEMO13.PRS");
    for (i = 0; i < sizeof(memory.demo); i++) {
        assert(memory.demo[i] == 0);
    }
    demo_size = pack_file(demo_packed, sizeof(demo_packed), frames,
                          DEMO_FILE_BYTES - 8);
    expect_stop(loader_prepare_stage, "PRS 00000000", "DEMO13.PRS");
    for (i = 0; i < sizeof(memory.past); i++) {
        assert(memory.past[i] == 0x5a);
    }
    game_state.demo = 0;

    /* A one-sector buffer holds its file at its start. Literals never decode
     * ahead of their input, so neut.prs decodes in place with the heap full. */
    heap_full = TRUE;
    assert(loader_read_prs_buffer("neut.prs", buffer, sizeof(buffer)) ==
           sizeof(image));
    assert(memcmp(buffer, &image, sizeof(image)) == 0);

    /* repeat.prs is 'a', then a copy of 256 bytes from 1 back. Decoded in
     * place, that copy would write over the end marker before it is read. So
     * the read decodes from a duplicate of the stream on the heap, and fails
     * without room for it. */
    memcpy(packed, repeat, sizeof(repeat));
    packed_size = sizeof(repeat);
    assert(loader_read_prs_buffer("repeat.prs", buffer, sizeof(buffer)) == -1);
    assert(memcmp(buffer, repeat, sizeof(repeat)) == 0);
    heap_full = FALSE;
    assert(loader_read_prs_buffer("repeat.prs", buffer, sizeof(buffer)) == 257);
    assert(heap_size == sizeof(repeat) && heap_block == NULL);
    for (i = 0; i < 257; i++) {
        assert(buffer[i] == 'a');
    }
    memcpy(packed, odd_file, sizeof(odd_file));
    packed_size = sizeof(odd_file);
    sectors_read = 1;
    heap_full = TRUE;
    assert(loader_read_prs_alloc("message.prs") == NULL);
    heap_full = FALSE;
    status = GDD_ERR_READ;
    assert(loader_read_prs_alloc("message.prs") == NULL && heap_block == NULL);
    status = GDD_ERR_OK;
    copy = loader_read_prs_alloc("message.prs");
    assert(copy == heap_block && copy != NULL && heap_size == 3);
    assert(memcmp(copy, odd_file + 1, 3) == 0);
    loader_free(copy);
    assert(heap_block == NULL);
    /* every file that opened was read, and closed but for the one whose read
     * ran on. two files were missing */
    assert(reads == closes + 1 && opens == reads + 2);
    return 0;
}
