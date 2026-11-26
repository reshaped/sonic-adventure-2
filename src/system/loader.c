/* reads from the disc, and the head of the overlay in the stage slot. the
 * reads take whole files, PRS files, a stage's overlay and its camera files */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>
#include <string.h>

#ifdef NONMATCHING
/* offsetof, size_t and struct camlin_node, by which this side's camera reader
 * checks the files, and port_stop, by which loader_stop stops the game */
#include <stddef.h>

#include "camera/camlin.h"
#include "engine/dc/crash.h"
#endif
#include "camera/camset.h"
#include "player/player.h"
#include "stage/stage.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/prs.h"
#include "system/sbinit.h"

/* the part of a camera file that a demo's parts start at */
#define DEMO_PART 2

/* the longs in the five camera areas, and in the two camlin areas */
#define CAMERA_AREAS_LONGS ((Sint32 *)CAMLIN_AREA_END - (Sint32 *)CAMSET_AREA)
#define CAMLIN_AREAS_LONGS ((Sint32 *)CAMLIN_AREA_END - (Sint32 *)CAMLIN_AREA_0)

/* the letter in a camset file's name for each number stage_character returns */
static const char *loader_camset_letters[] = {
    "S", "T", "M", "E", "K", "B", "A", "L", "_", "Z", "Z",
};

/* where in WORK_AREA a file of size bytes is read so that its last sector
 * ends at WORK_AREA_END */
static Uint8 *work_area_tail(Sint32 size)
{
    return WORK_AREA_END - (SECTORS(size) << SCT_SHIFT);
}

/* read the file name into dest in whole sectors, and store its size in bytes
 * in *size unless size is null. it returns -1 when the open fails, with
 * nothing read or stored, and 0 otherwise: the size query and the read go
 * unchecked */
static int loader_read(const char *name, void *dest, Sint32 *size)
{
    GDFS gdfs;
    Sint32 file_size;
    /* @hack spelling: sectors, a local computed before the size is stored,
     * where SECTORS(file_size) in gdFsRead's call would do */
    Sint32 sectors;

    gdfs = gdFsOpen(name, NULL);
    if (gdfs == NULL) {
        return -1;
    }
    gdFsGetFileSize(gdfs, &file_size);
    sectors = SECTORS(file_size);
    if (size != NULL) {
        *size = file_size;
    }
    gdFsRead(gdfs, sectors, dest);
    gdFsClose(gdfs);
    return 0;
}

#ifdef NONMATCHING
/* The files on disc contain complete fixed-size banks. Shorter tables can
 * hold whole records, followed by the zero-filled slots in the camera area. */
static Bool camera_table_fits(Uint32 size, Uint32 capacity, size_t record_size)
{
    return size == capacity || (size < capacity && size % record_size == 0);
}

static Bool install_camera_parts(const struct camera_file *file, Sint32 size,
                                 Bool demo)
{
    const Uint32 header_size = offsetof(struct camera_file, parts);
    const Uint32 zone_capacity = CAMSET_AREA_0 - CAMSET_AREA;
    const Uint32 line_capacity = CAMLIN_AREA_1 - CAMLIN_AREA_0;
    const Uint8 *parts;
    Uint32 remaining;
    Uint32 length;
    int index;

    /* The count includes the header. A full archive holds two pairs of
     * tables; demos use the second pair when its camset is present. */
    if (size < (Sint32)header_size || file->count < 1 ||
        file->count > CAMERA_PARTS + 1) {
        return FALSE;
    }
    remaining = (Uint32)size - header_size;
    for (index = 0; index < CAMERA_PARTS; index++) {
        length = (Uint32)file->sizes[index];
        if (length > remaining || (index >= file->count - 1 && length != 0)) {
            return FALSE;
        }
        if (index % 2 == 0) {
            if (!camera_table_fits(length, zone_capacity,
                                   sizeof(struct camera_zone))) {
                return FALSE;
            }
        } else if (!camera_table_fits(length, line_capacity,
                                      sizeof(struct camlin_node))) {
            return FALSE;
        }
        remaining -= length;
    }
    if (remaining != 0) {
        return FALSE;
    }

    parts = (const Uint8 *)file + header_size;
    index = 0;
    if (demo && file->count > 3) {
        parts += file->sizes[0] + file->sizes[1];
        index = DEMO_PART;
    }
    length = (Uint32)file->sizes[index];
    memcpy(CAMSET_AREA, parts, length);
    parts += length;
    length = (Uint32)file->sizes[index + 1];
    memcpy(CAMLIN_AREA_0, parts, length);
    memcpy(CAMLIN_AREA_1, parts, length);
    return TRUE;
}

static void read_camera_table(const char *name, Uint8 *dest, Uint32 capacity,
                              size_t record_size)
{
    Sint32 size;

    /* A short GD transfer can write a partial sector before it fails. */
    size = loader_read_file_checked(name, WORK_AREA, capacity);
    if (size >= 0 && camera_table_fits((Uint32)size, capacity, record_size)) {
        memcpy(dest, WORK_AREA, (size_t)size);
    }
}

static void read_camera_tables(int stage, const char *suffix)
{
    char name[32];
    Uint8 *zones;
    Uint8 *lines;
    int player;
    int character;

    sprintf(name, "camset%04d%s.bin", stage, suffix);
    read_camera_table(name, CAMSET_AREA, CAMSET_AREA_0 - CAMSET_AREA,
                      sizeof(struct camera_zone));
    for (player = 0; player < 2; player++) {
        if (player == 1 && player_character(1) == -1) {
            break;
        }
        zones = player == 0 ? CAMSET_AREA_0 : CAMSET_AREA_1;
        lines = player == 0 ? CAMLIN_AREA_0 : CAMLIN_AREA_1;
        character = stage_character(player);
        if ((unsigned int)character <
            sizeof(loader_camset_letters) / sizeof(loader_camset_letters[0])) {
            sprintf(name, "camset%04d%s%s.bin", stage,
                    loader_camset_letters[character], suffix);
            read_camera_table(name, zones, CAMSET_AREA_1 - CAMSET_AREA_0,
                              sizeof(struct camera_zone));
        }
        sprintf(name, "camlin%04d%s.bin", stage, suffix);
        read_camera_table(name, lines, CAMLIN_AREA_1 - CAMLIN_AREA_0,
                          sizeof(struct camlin_node));
    }
}
#endif

void loader_read_camera_files(void)
{
#ifdef NONMATCHING
    /* this side clears the camera areas, and installs the packed file only
     * where install_camera_parts finds it whole, each part fitting its area.
     * otherwise it reads the stage's separate tables */
    char name[32];
    int stage = stage_current_id();
    Bool demo =
        game_state.demo == DEMO_ON || game_state.demo == DEMO_ON_NO_FILE;
    Sint32 size;

    memset(CAMSET_AREA, 0, CAMLIN_AREA_END - CAMSET_AREA);
    sprintf(name, "stg%02dcam.prs", stage);
    size = loader_read_prs_buffer(name, CAMERA_FILE,
                                  WORK_AREA_END - WORK_AREA_FILE);
    if (!install_camera_parts(CAMERA_FILE, size, demo)) {
        read_camera_tables(stage, demo ? "D" : "");
    }
#else
    char name[32];
    int stage;
    /* @hack spelling: demo, a local for game_state's demo, where the field
     * read in the test would do, as the test for the suffix reads it */
    int demo;
    /* @hack spelling: letter, a local for stage_character's result, where each
     * call as the index would do */
    int letter;
    const char *suffix;
    struct camera_file *file;
    Sint32 *size;
    /* @hack spelling: parts, a local for the parts' start apart from part,
     * where part alone, set to file->parts and then stepped past the first
     * two, would do */
    Uint8 *parts;
    Uint8 *part;
    Sint32 length;
    Sint32 *area;
    int longs;

    /* @hack spelling: each of the two areas cleared through a pointer under a
     * count that runs down to 0, where an index that counts up to the area's
     * longs would do */
    for (area = (Sint32 *)CAMSET_AREA, longs = CAMERA_AREAS_LONGS; longs != 0;
         longs--) {
        *area++ = 0;
    }
    for (area = (Sint32 *)CAMLIN_AREA_0, longs = CAMLIN_AREAS_LONGS; longs != 0;
         longs--) {
        *area++ = 0;
    }
    stage = stage_current_id();
    sprintf(name, "stg%02dcam.prs", stage);
    if (loader_read_prs(name, CAMERA_FILE) != -1) {
        file = CAMERA_FILE;
        parts = file->parts;
        part = parts;
        demo = game_state.demo;
        if ((demo == DEMO_ON || demo == DEMO_ON_NO_FILE) && file->count > 3) {
            part = parts + (file->sizes[0] + file->sizes[1]);
            size = &file->sizes[DEMO_PART];
            file->count -= DEMO_PART;
        } else {
            size = &file->sizes[0];
        }
        /* @hack spelling: this test's assignment and the next's, not
         * statements of their own */
        if ((length = *size++) != 0 && file->count > 0) {
            memcpy(CAMSET_AREA, part, length);
            part += length;
            file->count--;
        }
        if ((length = *size) != 0 && file->count > 0) {
            memcpy(CAMLIN_AREA_0, part, length);
            memcpy(CAMLIN_AREA_1, part, length);
            file->count--;
        }
    } else {
        suffix =
            (game_state.demo == DEMO_ON || game_state.demo == DEMO_ON_NO_FILE)
                ? "D"
                : "";
        sprintf(name, "camset%04d%s.bin", stage, suffix);
        loader_read_file(name, CAMSET_AREA);
        letter = stage_character(0);
        sprintf(name, "camset%04d%s%s.bin", stage,
                loader_camset_letters[letter], suffix);
        loader_read_file(name, CAMSET_AREA_0);
        if (player_character(1) != -1) {
            letter = stage_character(1);
            sprintf(name, "camset%04d%s%s.bin", stage,
                    loader_camset_letters[letter], suffix);
            loader_read_file(name, CAMSET_AREA_1);
        }
        /* @hack redundant: the call of stage_character, whose result nothing
         * reads */
        stage_character(0);
        sprintf(name, "camlin%04d%s.bin", stage, suffix);
        loader_read_file(name, CAMLIN_AREA_0);
        if (player_character(1) != -1) {
            /* @hack redundant: the call of stage_character, whose result
             * nothing reads */
            stage_character(1);
            sprintf(name, "camlin%04d%s.bin", stage, suffix);
            loader_read_file(name, CAMLIN_AREA_1);
        }
    }
#endif
    camset_free_marked();
}

void loader_poll_drive(void)
{
    if (game_state.tray_opened && game_state.card_state != CARD_SAVING) {
        sbExitSystem();
        syBtExit();
    } else if (gdFsIsTrayOpen()) {
        game_state.tray_opened = 1;
    }
}

void loader_note_error(void *obj, Sint32 err)
{
    if (err == GDD_ERR_TRAYOPEND || err == GDD_ERR_UNITATTENT) {
        game_state.tray_opened = 1;
    }
}

#ifdef NONMATCHING
/* how the last checked load failed, for loader_stop: the words port_stop
 * shows, and the GD-ROM's error for a read, else 0. the words are one of:
 *
 * - OPEN: the file didn't open.
 * - SIZE: gdFsGetFileSize gave no size.
 * - ROOM: its whole sectors don't fit where it goes.
 * - READ: gdFsRead failed, or read fewer sectors than it was asked for.
 * - READ RUNS ON: the failed read's request still runs after gdFsStopRd.
 * - PRS: its PRS stream doesn't decode within its room, or to the size the
 *   load needs.
 * - HEAP: its stream, decoded in place, would write over input not yet read,
 *   and the heap had no room for a copy.
 * - HEAD: it isn't the overlay asked for. its head is short, holds another
 *   tag or kind, or names a function outside the overlay.
 * - NO STEP: the overlay's init left it no step inside it.
 * - NO OVERLAY: the current stage has none */
static const char *failure;
static Sint32 failure_error;

static void record_failure(const char *what, Sint32 error)
{
    failure = what;
    failure_error = error;
}

void loader_stop(const char *name)
{
    /* the GD-ROM gives loader_note_error its errors only once game_init has
     * installed it, after the sound driver's read */
    loader_note_error(NULL, failure_error);
    if (gdFsIsTrayOpen()) {
        game_state.tray_opened = 1;
    }
    if (game_state.tray_opened) {
        /* the vertical blank's loader_poll_drive leaves for the boot ROM */
        for (;;) {
        }
    }
    port_stop(failure, name, (Uint32)failure_error);
}

/* A stage slot is executable only after the complete image has been admitted.
 * Absolute callbacks are already bound by the native or GNU image linker. */
static Uint32 installed_overlay_size;

static Bool overlay_callback(const void *slot, Uint32 size, Uint32 header_size,
                             uintptr_t callback)
{
    uintptr_t base = (uintptr_t)slot;

    /* The garden writes its area through P1 and calls the P0 entry. */
    if (callback < 0x20000000U && base >= 0x80000000U && base < 0xa0000000U) {
        callback |= (uintptr_t)0x80000000U;
    }
    return !(callback & 1U) && callback >= base &&
           callback - base >= header_size && callback - base <= size - 2U;
}

static void publish_code(void *slot, Uint32 size)
{
    syCacheOCWB(slot, size);
    syCacheICI();
}

static Bool read_overlay(const char *name, const char *tag, Uint32 capacity,
                         Bool program)
{
    const struct overlay_head *head = OVERLAY_HEAD;
    Uint32 header_size = sizeof(struct program_overlay);
    uintptr_t step;
    Sint32 size;

    installed_overlay_size = 0;
    size = loader_read_prs_checked(name, STAGE_SLOT, capacity);
    if (size < 0) {
        return FALSE;
    }
    if (size < (Sint32)header_size || memcmp(head->name, tag, 8) != 0 ||
        (head->unk_08 != 0) != program) {
        record_failure("HEAD", 0);
        return FALSE;
    }
    step = program ? (uintptr_t)PROGRAM_OVERLAY->step
                   : (uintptr_t)STAGE_OVERLAY->step;
    if ((head->init != NULL &&
         !overlay_callback(head, size, header_size, (uintptr_t)head->init)) ||
        (head->exit != NULL &&
         !overlay_callback(head, size, header_size, (uintptr_t)head->exit))) {
        record_failure("HEAD", 0);
        return FALSE;
    }
    /* ADVERTISE installs the selected title/menu step during its init. */
    if (!overlay_callback(head, size, header_size, step) &&
        !(step == 0 && head->init != NULL && memcmp(tag, "ADVERTIZ", 8) == 0)) {
        record_failure("HEAD", 0);
        return FALSE;
    }
    publish_code(STAGE_SLOT, size);
    installed_overlay_size = size;
    return TRUE;
}

void loader_start_stage(void)
{
    overlay_run_init();
    if (installed_overlay_size == 0 ||
        !overlay_callback(STAGE_SLOT, installed_overlay_size,
                          sizeof(struct stage_overlay),
                          (uintptr_t)STAGE_OVERLAY->step)) {
        record_failure("NO STEP", 0);
        loader_stop(NULL);
    }
}

Bool loader_start_program(const char *name, const char *tag)
{
    if (!read_overlay(name, tag, (uintptr_t)DEMO_AREA - (uintptr_t)STAGE_SLOT,
                      TRUE)) {
        return FALSE;
    }
    overlay_run_init();
    if (!overlay_callback(STAGE_SLOT, installed_overlay_size,
                          sizeof(struct program_overlay),
                          (uintptr_t)PROGRAM_OVERLAY->step)) {
        overlay_run_exit();
        record_failure("NO STEP", 0);
        return FALSE;
    }
    return TRUE;
}

Bool loader_install_chao_area(const char *name, const char *tag, void *slot,
                              Uint32 capacity)
{
    struct chao_area_head *head = slot;
    Sint32 size;

    size = loader_read_prs_checked(name, slot, capacity);
    if (size < (Sint32)sizeof(*head) || memcmp(head->name, tag, 8) != 0 ||
        !overlay_callback(slot, size, sizeof(*head), (uintptr_t)head->step)) {
        return FALSE;
    }
    publish_code(slot, size);
    return TRUE;
}

/* read and admit a stage's overlay, as read_overlay does. the stage can't go
 * on without it: one that fails to load stops the game, as loader_stop says */
static void read_stage_overlay(const char *name, const char *tag,
                               Uint32 capacity)
{
    if (!read_overlay(name, tag, capacity, FALSE)) {
        loader_stop(name);
    }
}

/* decode the PRS file name into dest, which it must fill: size bytes. a load
 * that fails stops the game, as loader_stop says, and so does a file that
 * decodes to another size, as PRS */
static void read_prs_exact(const char *name, void *dest, Uint32 size)
{
    Sint32 decoded = loader_read_prs_checked(name, dest, size);

    if (decoded < 0) {
        loader_stop(name);
    }
    if ((Uint32)decoded != size) {
        record_failure("PRS", 0);
        loader_stop(name);
    }
}

void loader_prepare_stage(void)
{
    static const struct {
        const char *name;
        const char *tag;
    } bosses[] = {
        {"boss_bigfoot.PRS", "BIGFOOT "}, {"boss_hotshot.PRS", "BOSS0   "},
        {"boss_fdog.PRS", "FLYINGD "},    {"boss_bigbogy.PRS", "BIGBOGY "},
        {"boss_golem.PRS", "GOLEM   "},   {"boss_last1.prs", "LASTBS1 "},
        {"boss_last2.prs", "LASTBS2 "},   {"boss_golem_e.PRS", "GOLEME  "},
    };
    int stage = stage_current_id();
    char name[32];
    char tag[16];
    Uint32 capacity = (uintptr_t)DEMO_AREA - (uintptr_t)STAGE_SLOT;

    installed_overlay_size = 0;
    if (stage >= STAGE_GREEN_FOREST && stage < STAGE_STG_END) {
        if (game_state.demo == DEMO_ON) {
            sprintf(name, "demo%02d.prs", stage);
            read_prs_exact(name, DEMO_AREA, DEMO_FILE_BYTES);
        }
        sprintf(name, "stg%02d.prs", stage);
        /* Prison Lane and Metal Harbor both publish the STG10 header tag. */
        sprintf(tag, "STG%02d   ", stage == STAGE_PRISON_LANE ? 10 : stage);
        read_stage_overlay(name, tag, capacity);
        if (game_state.demo == DEMO_ON_NO_FILE) {
            game_state.demo = DEMO_ON;
        }
    } else if (stage >= STAGE_BOSS_BIGFOOT && stage <= STAGE_BOSS_GOLEM_E) {
        read_stage_overlay(bosses[stage - STAGE_BOSS_BIGFOOT].name,
                           bosses[stage - STAGE_BOSS_BIGFOOT].tag, capacity);
    } else if (stage == STAGE_CART_1 || stage == STAGE_CART_2) {
        read_stage_overlay("Cart.PRS", "CART    ", capacity);
    } else if (stage == STAGE_CHAO) {
        read_prs_exact("Chao_model.PRS", CHAO_MODEL_SLOT, CHAO_MODEL_BYTES);
        read_prs_exact("Chao_motion.PRS", CHAO_MOTION_SLOT, CHAO_MOTION_BYTES);
        read_stage_overlay("Chao.PRS", "ALGARDEN",
                           (uintptr_t)CHAO_MODEL_SLOT - (uintptr_t)STAGE_SLOT);
    } else {
        record_failure("NO OVERLAY", 0);
        loader_stop(NULL);
    }
}
#else
void loader_read_stage_overlay(void)
{
    char name[32];
    Uint16 stage;

    stage = stage_current_id();
    if (stage < STAGE_BOSS_BIGFOOT) {
        sprintf(name, "stg%02d.prs", stage);
        loader_read_prs(name, STAGE_SLOT);
        if (game_state.demo == DEMO_ON) {
            sprintf(name, "demo%02d.prs", stage);
            loader_read_prs(name, DEMO_AREA);
        } else if (game_state.demo == DEMO_ON_NO_FILE) {
            game_state.demo = DEMO_ON;
        }
        return;
    }
    switch (stage) {
    case STAGE_BOSS_BIGFOOT:
        loader_read_prs("boss_bigfoot.PRS", STAGE_SLOT);
        break;
    case STAGE_BOSS_HOTSHOT:
        loader_read_prs("boss_hotshot.PRS", STAGE_SLOT);
        break;
    case STAGE_BOSS_FDOG:
        loader_read_prs("boss_fdog.PRS", STAGE_SLOT);
        break;
    case STAGE_BOSS_BIGBOGY:
        loader_read_prs("boss_bigbogy.PRS", STAGE_SLOT);
        break;
    case STAGE_BOSS_GOLEM:
        loader_read_prs("boss_golem.PRS", STAGE_SLOT);
        break;
    case STAGE_BOSS_GOLEM_E:
        loader_read_prs("boss_golem_e.PRS", STAGE_SLOT);
        break;
    case STAGE_BOSS_LAST1:
        loader_read_prs("boss_last1.prs", STAGE_SLOT);
        break;
    case STAGE_BOSS_LAST2:
        loader_read_prs("boss_last2.prs", STAGE_SLOT);
        break;
    case STAGE_CART_1:
    case STAGE_CART_2:
        loader_read_prs("Cart.PRS", STAGE_SLOT);
        break;
    case STAGE_CHAO:
        loader_read_prs("Chao.PRS", STAGE_SLOT);
        break;
    }
}

#endif

#ifdef NONMATCHING
/* end the request a failed read left running on file, so that the read's
 * buffer is the caller's again. where gdFsStopRd can't end it, the drive may
 * still write the buffer: the game stops, as loader_stop says, with name and
 * the read's error. gdFsStopRd's own result doesn't say whether the request
 * ended: a stop that ends it can return an error too */
static void settle(GDFS file, const char *name, Sint32 error)
{
    Sint32 state;

    gdFsStopRd(file);
    state = gdFsGetStat(file);
    if (state == GDD_STAT_READ || state == GDD_STAT_BUSY) {
        record_failure("READ RUNS ON", error);
        loader_stop(name);
    }
}

/* The GD-ROM transfers whole sectors. Choose the address after querying the
 * open handle so a tail read uses the size of the file actually read. */
static Sint32 read_sectors(const char *name, GDFS file, Uint8 *buffer,
                           Uint32 capacity, Uint8 **tail)
{
    Sint32 size;
    Sint32 sectors;
    Sint32 error;

    if (file == NULL) {
        record_failure("OPEN", 0);
        return -1;
    }
    if (!gdFsGetFileSize(file, &size) || size < 0) {
        gdFsClose(file);
        record_failure("SIZE", 0);
        return -1;
    }
    sectors = size / GDD_FS_SCTSIZE + (size % GDD_FS_SCTSIZE != 0);
    if ((Uint32)sectors > capacity / GDD_FS_SCTSIZE ||
        (sectors != 0 && buffer == NULL)) {
        record_failure("ROOM", 0);
        size = -1;
    } else {
        if (tail != NULL) {
            buffer += capacity - (Uint32)sectors * GDD_FS_SCTSIZE;
            *tail = buffer;
        }
        if (sectors != 0) {
            error = gdFsRead(file, sectors, buffer);
            if (error != GDD_ERR_OK) {
                settle(file, name, error);
                record_failure("READ", error);
                size = -1;
            } else if (gdFsTell(file) != sectors) {
                record_failure("READ", 0);
                size = -1;
            }
        }
    }
    gdFsClose(file);
    return size;
}

Sint32 loader_read_file_checked(const char *name, void *dest, Uint32 capacity)
{
    return read_sectors(name, gdFsOpen(name, NULL), dest, capacity, NULL);
}

Sint32 loader_read_prs_checked(const char *name, void *dest, Uint32 capacity)
{
    Sint32 size;
    Uint32 written;

    size = loader_read_file_checked(name, WORK_AREA, WORK_AREA_END - WORK_AREA);
    if (size < 0) {
        return -1;
    }
    if (dest == NULL || !prs_decompress_checked(WORK_AREA, (Uint32)size, dest,
                                                capacity, NULL, &written)) {
        record_failure("PRS", 0);
        return -1;
    }
    return (Sint32)written;
}

static Sint32 read_prs_buffer(const char *name, GDFS file, void *buffer,
                              Uint32 capacity)
{
    Uint8 *input;
    Uint8 *copy;
    Uint32 offset;
    Uint32 written;
    Uint32 consumed;
    Sint32 size;
    enum prs_in_place_result result;
    Bool ok;

    size = read_sectors(name, file, buffer, capacity, &input);
    if (size < 0) {
        return -1;
    }
    offset = (Uint32)(input - (Uint8 *)buffer);
    result = prs_decompress_in_place(buffer, capacity, offset, (Uint32)size,
                                     &consumed, &written);
    if (result == PRS_INVALID) {
        record_failure("PRS", 0);
        return -1;
    }
    if (result == PRS_DECODED) {
        return (Sint32)written;
    }
    copy = syMalloc(consumed);
    if (copy == NULL) {
        record_failure("HEAP", 0);
        return -1;
    }
    memcpy(copy, input, consumed);
    ok = prs_decompress_checked(copy, consumed, buffer, capacity, NULL,
                                &written);
    syFree(copy);
    if (!ok) {
        record_failure("PRS", 0);
        return -1;
    }
    return (Sint32)written;
}
Sint32 loader_read_prs_buffer(const char *name, void *buffer, Uint32 capacity)
{
    if (buffer == NULL) {
        record_failure("ROOM", 0);
        return -1;
    }
    return read_prs_buffer(name, gdFsOpen(name, NULL), buffer, capacity);
}

static void *copy_work_area(Sint32 size)
{
    void *memory;

    if (size < 0) {
        return NULL;
    }
    memory = syMalloc((Uint32)size);
    if (memory != NULL) {
        memcpy(memory, WORK_AREA, (size_t)size);
    }
    return memory;
}

#endif

void *loader_read_alloc(const char *name)
{
#ifdef NONMATCHING
    /* this side copies what a checked read gives, at most WORK_AREA's bytes,
     * where the matching side's read is unbounded */
    return copy_work_area(
        loader_read_file_checked(name, WORK_AREA, WORK_AREA_END - WORK_AREA));
#else
    Sint32 size;
    void *memory;

    size = loader_file_size(name);
    if (size == -1) {
        return NULL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((memory = syMalloc(size)) == NULL) {
        return NULL;
    }
    if (loader_read_file(name, WORK_AREA) == -1) {
        syFree(memory);
        return NULL;
    }
    memcpy(memory, WORK_AREA, size);
    return memory;
#endif
}

void loader_free(void *memory)
{
    syFree(memory);
}

void *loader_read_prs_alloc(const char *name)
{
#ifdef NONMATCHING
    /* this side copies what a checked decode gives, within WORK_AREA, where the
     * matching side's is unbounded */
    return copy_work_area(
        loader_read_prs_buffer(name, WORK_AREA, WORK_AREA_END - WORK_AREA));
#else
    Sint32 size;
    Uint8 *file;
    /* @hack spelling: result, a local for loader_read's result, where the
     * call in the test would do */
    int result;
    void *memory;
    Sint32 longs;

    size = loader_file_size(name);
    if (size == -1) {
        return NULL;
    }
    /* @hack spelling: the address written out, not through work_area_tail,
     * as loader_read_prs_or_file takes it */
    file = WORK_AREA_END - (SECTORS(size) << SCT_SHIFT);
    result = loader_read(name, file, NULL);
    /* @hack spelling: the null return in an else, where an early return for
     * result < 0 would do */
    if (result >= 0) {
        size = prs_decompress(file, WORK_AREA);
    } else {
        return NULL;
    }
    /* @hack spelling: the assignment inside the test, as in
     * loader_read_alloc, where a statement of its own would do */
    if ((memory = syMalloc(size)) == NULL) {
        return NULL;
    }
    /* @hack spelling: longs set to 1 or 0 for the odd bytes by an if and an
     * else, then raised by size >> 2, where longs = (size >> 2) + ((size &
     * 3) ? 1 : 0) would do */
    if (size & 3) {
        longs = 1;
    } else {
        longs = 0;
    }
    longs += size >> 2;
    njMemCopy4(memory, WORK_AREA, longs);
    return memory;
#endif
}

int loader_read_file(const char *name, void *dest)
{
    return loader_read(name, dest, NULL);
}

Sint32 loader_read_file_get_size(const char *name, void *dest)
{
    Sint32 size = 0;

    loader_read(name, dest, &size);
    return size;
}

int loader_read_prs(const char *name, void *dest)
{
    int result;

    result = loader_read(name, WORK_AREA, NULL);
    if (result >= 0) {
        prs_decompress(WORK_AREA, dest);
    }
    syCacheICI();
    return result;
}

Sint32 loader_read_prs_get_size(const char *name, void *dest)
{
    Sint32 result;

    result = loader_read(name, WORK_AREA, NULL);
    if (result >= 0) {
        result = prs_decompress(WORK_AREA, dest);
    }
    syCacheICI();
    return result;
}

Sint32 loader_file_size(const char *name)
{
    GDFS gdfs;
    Sint32 size;

    gdfs = gdFsOpen(name, NULL);
    if (gdfs == NULL) {
        return -1;
    }
    if (!gdFsGetFileSize(gdfs, &size)) {
        size = -1;
    }
    gdFsClose(gdfs);
    return size;
}

Sint32 loader_read_prs_or_file(const char *name)
{
    char path[64];
#ifdef NONMATCHING
    /* this side bounds the name to path, reads a name without a dot as it is,
     * and decodes within WORK_AREA. the matching side copies and decodes
     * unchecked */
    const char *dot;
    GDFS packed = NULL;
    Sint32 size;

    if (name == NULL) {
        return -1;
    }
    if (name[0] != 'e') {
        dot = strchr(name, '.');
        if (dot != NULL &&
            (size_t)(dot - name) <= sizeof(path) - sizeof(".prs")) {
            memcpy(path, name, (size_t)(dot - name));
            strcpy(path + (dot - name), ".prs");
            packed = gdFsOpen(path, NULL);
        }
    }
    if (packed != NULL) {
        size =
            read_prs_buffer(path, packed, WORK_AREA, WORK_AREA_END - WORK_AREA);
    } else {
        size = loader_read_file_checked(name, WORK_AREA,
                                        WORK_AREA_END - WORK_AREA);
    }
    if (size >= 0) {
        syCacheICI();
    }
    return size;
#else
    Sint32 result;
    Uint8 *file;

    strcpy(path, name);
    strcpy(strchr(path, '.'), ".prs");
    /* @hack spelling: the PRS form's size assigned inside the test, beside
     * the test of name's first letter, where nested ifs, with the read of
     * name written in each, would do */
    if (strncmp(name, "e", 1) == 0 || (result = loader_file_size(path)) == -1) {
        result = loader_read_file_get_size(name, WORK_AREA);
    } else {
        file = work_area_tail(result);
        result = loader_read(path, file, NULL);
        if (result >= 0) {
            result = prs_decompress(file, WORK_AREA);
        }
    }
    syCacheICI();
    return result;
#endif
}

void overlay_run_init(void)
{
#ifdef NONMATCHING
    /* this side runs no init without an installed overlay */
    if (installed_overlay_size == 0) {
        return;
    }
#endif
    if (OVERLAY_HEAD->init != NULL) {
        OVERLAY_HEAD->init();
        OVERLAY_HEAD->init = NULL;
    }
}

void overlay_run_exit(void)
{
#ifdef NONMATCHING
    /* this side runs an overlay's exit once, and none without an installed
     * overlay */
    if (installed_overlay_size == 0) {
        return;
    }
    installed_overlay_size = 0;
#endif
    if (OVERLAY_HEAD->exit != NULL) {
        OVERLAY_HEAD->exit();
        OVERLAY_HEAD->exit = NULL;
    }
}
