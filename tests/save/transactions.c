#include <assert.h>
#include <stdlib.h>
#include <sg_bup.h>
#include <shinobi.h>
#include <sg_syrtc.h>
#include <string.h>
#include "draw/text_sprite.h"
#include "save/save.h"
#include "save/card.h"
#include "system/flags.h"
#include "system/game_state.h"
#include "system/task.h"

/* the port's two calls from make dc's side of save.c, which keep its 16:9 and
 * 60 fps in the save. both stay off here */
void port_store(Uint8 *flags);
void port_load(Uint8 flags);

void port_store(Uint8 *flags)
{
    *flags &= (Uint8)~0x30;
}

void port_load(Uint8 flags)
{
    (void)flags;
}


/* Build an ordinary encrypted main record and pack it with the SDK. */
static Uint32 image_storage[8192];
static Uint8 *input_file = (Uint8 *)image_storage;
static Uint8 *input_end;
/* Substitute aligned RAM for the console's DEMO_AREA in the host fixture. */
void *download_workspace[DOWNLOAD_SIZE / sizeof(void *)];
static int scan_files;
static void make_save(void)
{
    struct save_data plain;
    BUS_BACKUPFILEHEADER header;
    Uint32 *word;
    Uint32 key = 0x5a455441;
    Uint16 palette[16] = {0};
    Uint8 icon[512] = {0};
    Uint32 i;
    memset(&plain, 0, sizeof(plain));
    plain.date = 0x01020304;
    for (i = sizeof(plain.checksum); i < sizeof(plain); i++) {
        plain.checksum += ((Uint8 *)&plain)[i];
    }
    for (i = 0; i < sizeof(plain) / sizeof(Uint32); i++) {
        word = (Uint32 *)((Uint8 *)&plain + i * sizeof(Uint32));
        *word = (*word + 0x4e414e41) ^ key;
        key += 0x4e414e41;
    }
    memset(&header, 0, sizeof(header));
    header.icon_palette = palette;
    header.icon_data = icon;
    header.icon_num = 1;
    header.save_size = sizeof(plain);
    header.save_data = &plain;
    input_end = input_file + 512 * buMakeBackupFileImage(input_file, &header);
}
struct game_state game_state;
Uint8 game_flags[GAME_FLAGS], game_bits[GAME_BITS];
static unsigned allocations, releases, failures, alloc_attempt;
static int io_error, queue_error, has_file = 1, queued, stall;
static int forced, frames;
static void *buf, *workspace;
static int checksum_fault, crc_fault;
static struct task ui;
static int sprite_failure, sprites;
static BU_COMPLETECALLBACK done;

void *syMalloc(Uint32 count)
{
    void *p;
    if (failures && ++alloc_attempt == failures) {
        return NULL;
    }
    p = malloc(count);
    assert(p != NULL);
    allocations++;
    return p;
}

void syFree(void *p)
{
    assert(!queued || (p != workspace && p != buf));
    releases++;
    free(p);
}
void *syCalloc(Uint32 count, Uint32 size)
{
    void *p = syMalloc(count * size);
    if (p) {
        memset(p, 0, count * size);
    }
    return p;
}
void buSetProgressCallback(BU_PROGRESSCALLBACK f)
{
    (void)f;
}
void buSetCompleteCallback(BU_COMPLETECALLBACK f)
{
    done = f;
}
Sint32 buInit(Sint32 c, Sint32 d, void *w, void (*cb)(void))
{
    cb();
    return 0;
}
Sint32 buExitForce(void)
{
    forced++;
    queued = 0;
    workspace = NULL;
    return 0;
}
Sint32 buExit(void)
{
    if (queued) {
        return BUD_ERR_BUSY;
    }
    workspace = NULL;
    return 0;
}
Sint32 buMountDisk(Sint32 d, void *w, Sint32 size)
{
    workspace = w;
    queued = BUD_OP_MOUNT;
    return 0;
}
Sint32 buUnmount(Sint32 d)
{
    if (queued) {
        return BUD_ERR_BUSY;
    }
    workspace = NULL;
    return 0;
}
Sint32 buIsReady(Sint32 d)
{
    return workspace != NULL;
}
Sint32 buGetFileSize(Sint32 d, const char *n)
{
    if (scan_files &&
        (d != 0 || (strcmp(n, "SONIC2___D00") && strcmp(n, "SONIC2___D01")))) {
        return BUD_ERR_FILE_NOT_FOUND;
    }
    return has_file ? (input_end - input_file) / 512 : BUD_ERR_FILE_NOT_FOUND;
}
Sint32 buLoadFile(Sint32 d, const char *n, void *p, Uint32 blocks)
{
    if (queue_error) {
        return queue_error;
    }
    if (scan_files) {
        io_error = strcmp(n, "SONIC2___D01") == 0 ? BUD_ERR_NO_DISK : 0;
    }
    buf = p;
    queued = BUD_OP_LOADFILE;
    return 0;
}
static void complete(int error)
{
    int op = queued;
    queued = 0;
    if (op == BUD_OP_LOADFILE && error == 0) {
        memcpy(buf, input_file, input_end - input_file);
        if (crc_fault || checksum_fault) {
            /* Damage one main-record word. A zero outer CRC separately
             * exercises the supported unchecked container convention. */
            ((Uint8 *)buf)[640] ^= 1;
            if (checksum_fault) {
                ((Uint8 *)buf)[70] = ((Uint8 *)buf)[71] = 0;
            }
        }
    }
    done(0, op, error, 0);
    buf = NULL;
}
Sint32 buStat(Sint32 d)
{
    if (queued && !stall) {
        complete(io_error);
    }
    return queued ? BUD_STAT_BUSY : 0;
}
Sint32 buGetLastError(Sint32 d)
{
    return io_error;
}
void frame_wait(void)
{
    frames++;
    if (!stall && queued) {
        complete(io_error);
    }
}
Sint32 syRtcInit(void)
{
    return 0;
}
void syRtcFinish(void)
{
}
struct task *task_create(Uint16 f, int l, void (*s)(struct task *),
                         const char *n)
{
    memset(&ui, 0, sizeof(ui));
    ui.step = s;
    return &ui;
}
void task_kill(struct task *t)
{
    if (t->destructor) {
        t->destructor(t);
    }
    if (t->extra.p) {
        syFree(t->extra.p);
    }
    t->extra.p = NULL;
}
struct text_sprite *text_sprite_create_styled(const char *t, int l, int c,
                                              Sint16 x, Sint16 y)
{
    if (++sprites == sprite_failure) {
        return NULL;
    }
    return syCalloc(1, sizeof(struct text_sprite));
}
void text_sprite_measure(struct text_sprite *s, const char *t, int l, Uint16 *w,
                         Uint16 *h)
{
    *w = *h = 4;
}
void text_sprite_destroy(struct text_sprite *s)
{
    syFree(s);
}
Sint32 njSetTexture(NJS_TEXLIST *t)
{
    return 0;
}
void njDrawTexture(NJS_TEXTURE_VTX *p, Int c, Int t, Int f)
{
}
Sint32 buGetDiskInfo(Sint32 drive, BUS_DISKINFO *info)
{
    (void)info;
    return drive == 0 ? 0 : BUD_ERR_NO_DISK;
}

static void check_downloads(void)
{
    struct {
        struct download_file file;
        struct download_text texts[DOWNLOAD_TEXTS];
        char line[8];
    } payload;
    struct download_entry entries[2];
    BUS_BACKUPFILEHEADER header;
    Uint16 palette[16] = {0};
    Uint8 icon[512] = {0};
    static void *other_area[DOWNLOAD_SIZE / sizeof(void *)];
    struct download_file *loaded = (struct download_file *)other_area;
    Uint32 length;
    int i, j;
    void *data;

    memset(&payload, 0, sizeof(payload));
    payload.file.kind = DOWNLOAD_KIND_3;
    payload.file.values[0] = 77;
    strcpy(payload.line, "course");
    payload.file.data = ((Uint8 *)download_workspace) +
                        ((Uint8 *)payload.line - (Uint8 *)&payload);
    for (i = 0; i < DOWNLOAD_TEXTS; i++) {
        payload.file.texts[i] =
            (struct download_text *)(((Uint8 *)download_workspace) +
                                     ((Uint8 *)&payload.texts[i] -
                                      (Uint8 *)&payload));
        for (j = 0; j < DOWNLOAD_LINES; j++) {
            payload.texts[i].lines[j] = payload.file.data;
        }
    }
    memset(&header, 0, sizeof(header));
    header.icon_palette = palette;
    header.icon_data = icon;
    header.icon_num = 1;
    header.save_size = sizeof(payload);
    header.save_data = &payload;
    input_end = input_file + 512 * buMakeBackupFileImage(input_file, &header);
    scan_files = 1;
    memset(entries, 0, sizeof(entries));
    assert(save_downloads_find(entries, DOWNLOAD_FIND_ALL) == 1);
    assert(entries[0].slot == 0 && entries[0].values[0] == 77);
    assert(strcmp(entries[0].texts[4].line4, "course") == 0);
    assert(entries[1].name[0] == 0);
    data = save_download_load_sized(&entries[0], loaded, &length);
    assert(data != loaded->data && length >= 7);
    assert(strcmp(data, "course") == 0);
    strcpy(entries[0].name, "SONIC2___D01");
    assert(!save_download_load_sized(&entries[0], loaded, &length) && !length);
    scan_files = io_error = 0;
    make_save();
}

static struct save_data before;
static Uint8 flags_before[GAME_FLAGS], bits_before[GAME_BITS];
static int check_reads(void)
{
    int status, i;
    Uint8 dest[16];
    {
        BUS_BACKUPFILEHEADER parsed, saved;
        Uint32 needed;
        assert(buAnalyzeBackupFileImageSized(&parsed, input_file,
                                             input_end - input_file) == 0);
        saved = parsed;
        needed =
            (Uint32)((Uint8 *)parsed.save_data - input_file) + parsed.save_size;
        assert(buAnalyzeBackupFileImageSized(&parsed, input_file, needed - 1) ==
               BUD_ERR_BUPFILE_ILLEGAL);
        assert(!memcmp(&parsed, &saved, sizeof(parsed)));
        assert(buAnalyzeBackupFileImageSized(&parsed, input_file, needed) == 0);
    }
    assert(save_read(0) == SAVE_READ_OK);
    {
        int keep;
        for (keep = 0; keep < 8; keep++) {
            game_state.save_date = 199;
            game_flags[0] = game_bits[0] = 17;
            assert(save_read(keep) == SAVE_READ_OK);
            assert(game_state.save_date ==
                   ((keep & SAVE_KEEP_STATE) ? 199 : save_data.date));
            assert(game_flags[0] == ((keep & SAVE_KEEP_FLAGS) ? 17 : 0));
            assert(game_bits[0] == ((keep & SAVE_KEEP_BITS) ? 17 : 0));
        }
        assert(save_read(0) == SAVE_READ_OK);
    }
    before = save_data;
    memcpy(flags_before, game_flags, sizeof(game_flags));
    memcpy(bits_before, game_bits, sizeof(game_bits));
    for (i = 1; i <= 4; i++) {
        if (i == 1) {
            io_error = BUD_ERR_NO_DISK;
        }
        if (i == 2) {
            queue_error = BUD_ERR_BUSY;
        }
        if (i == 3) {
            has_file = 0;
        }
        if (i == 4) {
            checksum_fault = 1;
        }
        status = save_read(0);
        assert(status == (i == 3   ? SAVE_READ_NOT_STARTED
                          : i == 4 ? SAVE_READ_BAD_CHECKSUM
                                   : SAVE_READ_FAILED));
        assert(!memcmp(&before, &save_data, sizeof(before)));
        assert(!memcmp(game_flags, flags_before, sizeof(game_flags)));
        assert(!memcmp(game_bits, bits_before, sizeof(game_bits)));
        assert(game_state.save_date == before.date);
        io_error = queue_error = checksum_fault = 0;
        has_file = 1;
    }
    crc_fault = 1;
    assert(save_read(0) == SAVE_READ_BAD_FORMAT);
    assert(card_load_result(0) == BUD_ERR_BUPFILE_CRC);
    assert(!memcmp(&before, &save_data, sizeof(before)));
    crc_fault = 0;
    memset(dest, 99, sizeof(dest));
    io_error = BUD_ERR_NO_DISK;
    assert(card_load_file(0, "test", dest, sizeof(dest)) == 0);
    assert(!buStat(0) && card_load_result(0) == BUD_ERR_NO_DISK);
    assert(game_state.card_state == CARD_LOAD_FAILED && dest[0] == 99 &&
           dest[15] == 99);
    io_error = 0;
    return 0;
}
static int exercise(void)
{
    int i;
    struct card_status st;
    Uint8 dest[16];
    save_set_drive(0);
    for (i = 0; i < 4; i++) {
        card_init();
        alloc_attempt = 0;
        failures = i == 0 ? 1 : 0;
        sprite_failure = i;
        sprites = 0;
        ui.step(&ui);
        assert(!ui.draw && !ui.extra.p);
        assert(allocations == releases);
        failures = sprite_failure = 0;
        ui.step(&ui);
        assert(ui.draw && ui.extra.p);
        card_exit();
        assert(allocations == releases);
        card_exit();
    }
    card_init();
    assert(done(0, BUD_OP_CONNECT, 15, 0) == 0);
    complete(BUD_ERR_UNFORMAT);
    assert(card_get_status(0, &st) == 0 && !st.mounted && workspace == NULL);
    assert(done(0, BUD_OP_CONNECT, 15, 0) == 0);
    complete(0);
    assert(card_get_status(0, &st) == 0 && st.mounted && workspace);
    check_downloads();
    if (check_reads()) {
        return 1;
    }
    stall = 1;
    assert(card_load_file(0, "test", dest, sizeof(dest)) == 0);
    done(0, BUD_OP_UNMOUNT, 0, 0);
    assert(workspace && queued);
    stall = 0;
    complete(BUD_ERR_NO_DISK);
    assert(!workspace && game_state.card_state == CARD_LOAD_FAILED);
    assert(done(0, BUD_OP_CONNECT, 15, 0) == 0);
    complete(0);
    assert(workspace);
    assert(card_load_file(0, "test", dest, sizeof(dest)) == 0);
    card_exit();
    assert(allocations == releases && forced == 0 && frames > 0);
    card_init();
    assert(done(0, BUD_OP_CONNECT, 15, 0) == 0);
    complete(0);
    assert(card_load_file(0, "test", dest, sizeof(dest)) == 0);
    stall = 1;
    card_exit();
    assert(forced == 1 && game_state.card_state == CARD_LOAD_FAILED);
    assert(allocations == releases);
    return 0;
}
int main(void)
{
    make_save();
    return exercise();
}
