/* the save and the download files. the save keeps the game's progress,
 * records and awards in a main file on a memory card */
#include <ninja.h>
#include <sg_bup.h>
#include <sg_syrtc.h>
#include <sg_xpt.h>
#include <shinobi.h>
#ifdef NONMATCHING
/* uintptr_t, by which this side's download checks compare a text's place */
#include <stdint.h>
#endif
#include <stdio.h>
#include <string.h>

#include "hud/hud.h"
#include "player/player.h"
#include "save/card.h"
#include "save/save.h"
#include "sound/adx.h"
#include "stage/stage.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/flags.h"
#include "system/sequence.h"
#include "vmu/vibration.h"

#ifdef NONMATCHING
/* port_store and port_load, which keep this side's toggles in the save */
#include "engine/dc/port.h"
#endif

/* the flags from FLAG_MUSIC_HEARD on that AWARD_LAST_STORY sets to 0xff.
 * the heard bits of the MUSIC_NAMES music fill the first 18, and the last
 * 14 hold none. the award then clears the bit of MUSIC_G_HILL, g_hill.adx,
 * so that music alone stays unheard */
#define HEARD_SET_FLAGS 32
#define FLAG_HEARD_SET_LAST (FLAG_MUSIC_HEARD + HEARD_SET_FLAGS - 1)
#define MUSIC_G_HILL 106

/* each character's group of stages, whose missions save_award judges
 * together. a group holds where test(stage) holds for each of its stages.
 * Tails' and Rouge's take STAGE_CART_1 too, through cart(character), which
 * tests the character's record in cart_stage */
#define SONIC_STAGES(test)                                                     \
    (test(STAGE_CITY_ESCAPE) && test(STAGE_METAL_HARBOR) &&                    \
     test(STAGE_GREEN_FOREST) && test(STAGE_PYRAMID_CAVE) &&                   \
     test(STAGE_CRAZY_GADGET) && test(STAGE_FINAL_RUSH))
#define TAILS_STAGES(test, cart)                                               \
    (test(STAGE_PRISON_LANE) && test(STAGE_MISSION_STREET) &&                  \
     cart(PLAYER_TAILS_MECH) && test(STAGE_HIDDEN_BASE) &&                     \
     test(STAGE_ETERNAL_ENGINE))
#define KNUCKLES_STAGES(test)                                                  \
    (test(STAGE_WILD_CANYON) && test(STAGE_PUMPKIN_HILL) &&                    \
     test(STAGE_AQUATIC_MINE) && test(STAGE_DEATH_CHAMBER) &&                  \
     test(STAGE_METEOR_HERD))
#define SHADOW_STAGES(test)                                                    \
    (test(STAGE_RADICAL_HIGHWAY) && test(STAGE_WHITE_JUNGLE) &&                \
     test(STAGE_SKY_RAIL) && test(STAGE_FINAL_CHASE))
#define EGGMAN_STAGES(test)                                                    \
    (test(STAGE_IRON_GATE) && test(STAGE_SAND_OCEAN) &&                        \
     test(STAGE_LOST_COLONY) && test(STAGE_WEAPONS_BED) &&                     \
     test(STAGE_COSMIC_WALL))
#define ROUGE_STAGES(test, cart)                                               \
    (test(STAGE_DRY_LAGOON) && test(STAGE_EGG_QUARTERS) &&                     \
     test(STAGE_SECURITY_HALL) && cart(PLAYER_ROUGE) && test(STAGE_MAD_SPACE))
/* the bit of game_bits that gives each story's award */
#define HERO_STORY_BIT 28
#define DARK_STORY_BIT 131
#define LAST_STORY_BIT 210

/* the stage whose first mission save_award gives a rank of 1 with
 * AWARD_ALL_EMBLEMS. save_record_stage never raises its ranks */
#define STAGE_ALL_EMBLEMS STAGE_GREEN_HILL

/* the stage whose records save_record_stage keeps for STAGE_VERSUS_KNUCKLES
 * and each of the relay's stages */
#define STAGE_SHARED_RECORDS STAGE_RELAY_LAST

#define HUNDREDTHS_PER_SECOND 100
#define HUNDREDTHS_PER_MINUTE 6000
/* the hundredths of a second in a frame, a 60th. 100 / 60 is 1.6667, but the
 * game takes 1.6659999F, the float just under 1.666F */
#define HUNDREDTHS_PER_FRAME 1.6659999F
/* turn frame 60ths of a second into hundredths, rounded up */
/* @hack type: the conversion to Uint32, where int would do */
#define HUNDREDTHS(frame) ((Uint32)njRoundUp((frame) * HUNDREDTHS_PER_FRAME))
/* a time as the decimal number mmsshh, which orders as the times do */
#define TIME_PARTS_VALUE(minute, second, hundredths)                           \
    (((minute) * 10000) + ((second) * 100) + (hundredths))
#define TIME_VALUE(time)                                                       \
    TIME_PARTS_VALUE((time).minute, (time).second, (time).hundredths)

/* the minutes of a new save's best times, by place among the SAVE_BEST,
 * from 0 for the best. the boss totals take NEW_BOSS_MINUTES, and every
 * other record NEW_MINUTES. each time of download_best_times takes
 * NEW_MINUTES(0) */
#define NEW_MINUTES(place) (30 + (10 * (place)))
#define NEW_BOSS_MINUTES(place) (60 + (10 * (place)))

/* the main file's copy flag, as card_save_file takes it */
#define MAIN_COPY_FLAG 0

/* the main file's cipher. each word adds CIPHER_STEP, then XORs a key,
 * which starts at CIPHER_KEY and adds CIPHER_STEP for each word */
#define CIPHER_KEY 0x5a455441
#define CIPHER_STEP 0x4e414e41

/* the main file's icon is ICON_SIZE pixels square, each a 4-bit index into
 * its palette of ICON_COLORS */
#define ICON_SIZE 32
#define ICON_COLORS 16
static char save_name[SAVE_NAME_LENGTH + 1] = "SONIC2___S01";
static char download_name[SAVE_NAME_LENGTH + 1] = "SONIC2___D01";
struct save_place save_place = {-1, save_name};

/* the icon's colors, ARGB4444 */
static Uint16 save_icon_palette[ICON_COLORS] = {
#include "save/save/save_icon_palette.inc"
};

/* a byte holds two pixels, the left one's index in its high nibble */
static Uint8 save_icon[ICON_SIZE * ICON_SIZE / 2] = {
#include "save/save/save_icon.inc"
};

/* the header save_write writes the main file from. its two comments fill
 * vms_comment and btr_comment, with no terminator. buMakeBackupFileImage
 * copies only their first 16 and 32 characters, so neither needs one */
static BUS_BACKUPFILEHEADER save_header = {
    /* NOLINTBEGIN(clang-diagnostic-unterminated-string-initialization) */
    "MAIN FILE         ",
    "SONIC ADVENTURE 2 / Main File     ",
    /* NOLINTEND(clang-diagnostic-unterminated-string-initialization) */
    "",
    save_icon_palette,
    save_icon,
    1,
    0,
    NULL,
    BUD_VISUALTYPE_NONE,
    0,
    &save_data,
    sizeof(struct save_data),
};

static struct save_place download_place = {-1, download_name};

/* the stages whose missions' ranks save_award counts as emblems: each
 * group's in turn, but STAGE_CART_1, then STAGE_SHARED_RECORDS */
static Sint32 emblem_stages[SAVE_EMBLEM_STAGES] = {
    /* clang-format off */
    STAGE_CITY_ESCAPE, STAGE_METAL_HARBOR, STAGE_GREEN_FOREST,
    STAGE_PYRAMID_CAVE, STAGE_CRAZY_GADGET, STAGE_FINAL_RUSH,
    STAGE_PRISON_LANE, STAGE_MISSION_STREET, STAGE_HIDDEN_BASE,
    STAGE_ETERNAL_ENGINE,
    STAGE_WILD_CANYON, STAGE_PUMPKIN_HILL, STAGE_AQUATIC_MINE,
    STAGE_DEATH_CHAMBER, STAGE_METEOR_HERD,
    STAGE_RADICAL_HIGHWAY, STAGE_WHITE_JUNGLE, STAGE_SKY_RAIL,
    STAGE_FINAL_CHASE,
    STAGE_IRON_GATE, STAGE_SAND_OCEAN, STAGE_LOST_COLONY, STAGE_WEAPONS_BED,
    STAGE_COSMIC_WALL,
    STAGE_DRY_LAGOON, STAGE_EGG_QUARTERS, STAGE_SECURITY_HALL,
    STAGE_MAD_SPACE,
    STAGE_SHARED_RECORDS,
    /* clang-format on */
};
/* @hack spelling: heard_bits, a pointer to MUSIC_HEARD_BITS that save_award
 * reads, where MUSIC_HEARD_BITS written in its place would do */
static Uint8 *heard_bits = MUSIC_HEARD_BITS;
/* the music a new save marks heard, by number */
static int new_save_music[] = {
    55, 85, 86, 87, 92, 62, 91, 129, 30, 29, 128, 28, 31, 131,
};

struct save_data save_data;
struct download_entry download_chosen;
/* how many entries save_downloads_find has filled */
static int entries_filled;
/* by slot, 1 once an entry holds that slot's file */
static char slots_taken[DOWNLOAD_SLOTS];
/* save_keep's copy of save_data */
static struct save_data kept_save;
/* the device type save_check_sockets last saw in each drive's socket */
static Uint32 socket_types[CARD_DRIVES];

static void save_record_set_score(struct save_record *record, Sint32 score);
static void downloads_find_on(struct download_entry *entries, void *buffer,
                              Uint32 kinds);
static int stage_all_top(int stage);
static int cart_all_top(int character);
static int stage_all_ranked(int stage);
static int cart_all_ranked(int character);
static int save_is_another(void *data);
static void save_encrypt(void *data, Int size);
static void save_decrypt(void *data, Int size);

/* nothing calls these, and no header declares them */
/* @hack redundant: external linkage, which no call needs, where static would
 * do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the current mission's best rings, score and time on the current stage.
 * its first record holds them. on STAGE_CART_1 the record is cart_stage's
 * for stage_character(0). STAGE_VERSUS_KNUCKLES and the relay's stages read
 * their own records, which nothing writes: save_record_stage records them in
 * STAGE_SHARED_RECORDS's.
 *
 * on a stage without records, save_best_rings and save_best_score return -1.
 * save_best_time_parts and save_best_time then store nothing and return 0,
 * and return 1 otherwise */
int save_best_rings(void);
Sint32 save_best_score(void);
int save_best_time_parts(int *minute, int *second, int *hundredths);
int save_best_time(struct save_time *time);
/* copy *from over *to. a null for either stands for save_keep's copy */
void save_copy(struct save_data *from, struct save_data *to);
/* NOLINTEND(misc-use-internal-linkage) */

/* @hack spelling: save_mark_played, a function SHC writes into save_write
 * under #pragma inline, where its body written in save_write would do */
#pragma inline(save_mark_played)
static void save_mark_played(void)
{
    int character;

    if (game_state.chao_key == 0) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((character = player_character(0)) >= 0) {
        save_data.played[character] = 1;
    }
}

int save_write(void)
{
    int i;
    Uint8 *bytes;

    switch (game_flags[FLAG_SEQUENCE]) {
    case SEQUENCE_HERO:
        if (game_flags[FLAG_SEQUENCE_STEP] >= save_data.progress[0]) {
            save_data.progress[0] = game_flags[FLAG_SEQUENCE_STEP] + 1;
        }
        break;
    case SEQUENCE_DARK:
        if (game_flags[FLAG_SEQUENCE_STEP] >= save_data.progress[1]) {
            save_data.progress[1] = game_flags[FLAG_SEQUENCE_STEP] + 1;
        }
        break;
    case SEQUENCE_LAST:
        if (game_flags[FLAG_SEQUENCE_STEP] >= save_data.progress[2]) {
            save_data.progress[2] = game_flags[FLAG_SEQUENCE_STEP] + 1;
        }
        break;
    }
    for (i = 0; i < GAME_FLAGS; i++) {
        save_data.flags[i] = game_flags[i];
    }
    memset(save_data.bits, 0, sizeof(save_data.bits));
    for (i = 0; i < GAME_BITS; i++) {
        save_data.bits[i / 8] |= (game_bits[i] > 0) << (i % 8);
    }
    save_data.text_language = game_state.text_language;
    save_data.voice_language = game_state.voice_language;
    save_data.vibration = game_state.vibration;
    save_data.lives[0] = game_state.lives[0];
    save_data.lives[1] = game_state.lives[1];
    save_data.theme = game_state.theme;
#ifdef NONMATCHING
    /* this side keeps the port's 16:9 and 60 fps in two of status's spare
     * bits (engine/dc/port.h) */
    port_store(&save_data.status);
#endif
    syCfgGetIndividualID(save_data.console_id);
    save_award();
    if (save_place.drive >= 0) {
        save_mark_played();
        bytes = (Uint8 *)&save_data;
        save_data.checksum = 0;
        /* @hack type: i compared as a Uint32 with the size, where a signed
         * test would do */
        for (i = sizeof(save_data.checksum);
             (Uint32)i < sizeof(struct save_data); i++) {
            save_data.checksum += bytes[i];
        }
        save_keep();
        save_encrypt(&save_data, sizeof(struct save_data));
        vibration_stop();
        if (card_save_file(save_place.drive, save_place.name, &save_header,
                           MAIN_COPY_FLAG) == -1) {
            save_restore();
            return 0;
        }
        save_restore();
        return 1;
    }
    /* @hack implicit: no return value without a drive, where return 0 would do
     * while no caller reads that result */
#ifdef NONMATCHING
    /* the tag's plain form */
    return 0;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

int save_read(int keep)
{
    int i;
    Uint32 sum;
    Uint8 *bytes;
#ifdef NONMATCHING
    /* this side repairs the tag below: it loads into a copy of its own, and
     * takes the copy for save_data only where the load ends whole and its
     * checksum holds */
    struct save_data *candidate;
    Sint32 status;
    int drive = save_place.drive;

    candidate = syMalloc(sizeof(*candidate));
    if (!candidate) {
        return SAVE_READ_FAILED;
    }
    status =
        card_start_load(drive, save_place.name, candidate, sizeof(*candidate));
    if (status != 0) {
        syFree(candidate);
        return status == BUD_ERR_FILE_NOT_FOUND ? SAVE_READ_NOT_STARTED
                                                : SAVE_READ_FAILED;
    }
    while (buStat(drive) != 0) {
    }
    status = card_load_result(drive);
    if (status != (Sint32)sizeof(*candidate)) {
        syFree(candidate);
        if (status >= 0 || status == BUD_ERR_BUPFILE_ILLEGAL ||
            status == BUD_ERR_BUPFILE_CRC) {
            return SAVE_READ_BAD_FORMAT;
        }
        return SAVE_READ_FAILED;
    }
    save_decrypt(candidate, sizeof(*candidate));
    sum = 0;
    bytes = (Uint8 *)candidate;
    for (i = sizeof(candidate->checksum); (Uint32)i < sizeof(*candidate); i++) {
        sum += bytes[i];
    }
    if (sum != candidate->checksum) {
        syFree(candidate);
        return SAVE_READ_BAD_CHECKSUM;
    }
    save_data = *candidate;
    syFree(candidate);
#else
    if (card_load_file(save_place.drive, save_place.name, &save_data,
                       sizeof(struct save_data)) == -1) {
        return SAVE_READ_NOT_STARTED;
    }
    /* @bug logic: where the load fails, save_read decrypts save_data's old
     * bytes, which garbles them. it waits for the drive (BSR _save_busy at
     * 0x8c0bab98), but never tests the load's result. save_download_load
     * does test it, with buGetLastError (JSR @R3 at 0x8c0bb23c) */
    while (save_busy() != 0) {
    }
    save_decrypt(&save_data, sizeof(struct save_data));
    sum = 0;
    bytes = (Uint8 *)&save_data;
    /* @hack type: i compared as a Uint32 with the size, where a signed test
     * would do */
    for (i = sizeof(save_data.checksum); (Uint32)i < sizeof(struct save_data);
         i++) {
        sum += bytes[i];
    }
    if (sum != save_data.checksum) {
        return SAVE_READ_BAD_CHECKSUM;
    }
#endif
    if (!(keep & SAVE_KEEP_FLAGS)) {
        for (i = 0; i < GAME_FLAGS; i++) {
            game_flags[i] = save_data.flags[i];
        }
    }
    if (!(keep & SAVE_KEEP_BITS)) {
        for (i = 0; i < GAME_BITS; i++) {
            game_bits[i] = (save_data.bits[i / 8] & (1 << (i % 8))) > 0;
        }
    }
    if (!(keep & SAVE_KEEP_STATE)) {
        game_state.text_language = save_data.text_language;
        game_state.voice_language = save_data.voice_language;
        game_state.save_date = save_data.date;
        game_state.vibration = save_data.vibration;
        game_state.lives[0] = save_data.lives[0];
        game_state.lives[1] = save_data.lives[1];
        game_state.theme = save_data.theme;
#ifdef NONMATCHING
        /* this side takes the port's 16:9 and 60 fps from the save */
        port_load(save_data.status);
#endif
    }
    if (game_state.winner_posed == 0) {
        game_state.emblems = save_data.emblems;
    }
    return SAVE_READ_OK;
}

int save_busy(void)
{
    /* @hack spelling: drive, a local for the drive, where save_place.drive
     * read twice would do */
    int drive;

    drive = save_place.drive;
    if (drive < 0) {
        return 0;
    }
    return buStat(drive);
}

void save_set_drive(int drive)
{
    save_place.drive = drive;
}

void save_set_slot(int slot)
{
    sprintf(save_name, "SONIC2___S%02d", slot);
}

int save_delete(void)
{
    return buDeleteFile(save_place.drive, save_place.name);
}

int save_check(int create)
{
    /* @hack spelling: drive, a local for the test and buIsReady, while
     * buGetDiskFree reads save_place.drive again, where drive in all three
     * would do */
    int drive;
    /* @hack spelling: result, a local for save_read's result, which the
     * switch tests, where switch (save_read(...)) would do */
    int result;

    drive = save_place.drive;
    if (drive < 0) {
        return SAVE_OK;
    }
    if (buIsReady(drive) == 0) {
        return SAVE_NOT_READY;
    }
    save_keep();
    result = save_read(SAVE_KEEP_STATE | SAVE_KEEP_FLAGS | SAVE_KEEP_BITS);
    switch (result) {
    case SAVE_READ_NOT_STARTED:
        /* @hack redundant: the else after a return */
        if (create == 0) {
            save_restore();
            return SAVE_NOT_OURS;
        } else if (buGetDiskFree(save_place.drive, BUD_FILETYPE_NORMAL) <
                   SAVE_BLOCKS) {
            save_restore();
            return SAVE_NO_ROOM;
        }
        break;
#ifdef NONMATCHING
    /* the results only this side's save_read gives. an outer image or a length
     * it refuses counts as a bad checksum */
    case SAVE_READ_FAILED:
        save_restore();
        return SAVE_NOT_READY;
    case SAVE_READ_BAD_FORMAT:
#endif
    case SAVE_READ_BAD_CHECKSUM:
        save_restore();
        return SAVE_BAD_CHECKSUM;
    }
    if (save_is_another(&save_data) != 0) {
        save_restore();
        return SAVE_NOT_OURS;
    }
    save_restore();
    return SAVE_OK;
}

static void save_record_set_score(struct save_record *record, Sint32 score)
{
#ifdef NONMATCHING
    /* this side repairs the tag below: score_high takes the score's bits 16 to
     * 23 */
    record->score_low = (Uint16)score;
    record->score_high = (Uint8)((Uint32)score >> SAVE_SCORE_LOW_BITS);
#else
    record->score_low = score % (1 << SAVE_SCORE_LOW_BITS);
    /* @bug logic: score_high takes the wrong value for a score of 0xff0000
     * or more. score_high needs the score mod 0x1000000, its low 24 bits. the
     * code takes it mod 0xff0000 instead (JSR @R3 at 0x8c0baefe calls _modls
     * with H'00FF0000) */
    record->score_high = (score % 0xff0000) >> SAVE_SCORE_LOW_BITS;
#endif
}

Sint32 save_record_score(const struct save_record *record)
{
    /* @hack spelling: the score built in a local over two statements, where
     * return record->score_low + (record->score_high << SAVE_SCORE_LOW_BITS)
     * would do */
    Sint32 score;

    score = record->score_low;
    score += record->score_high << SAVE_SCORE_LOW_BITS;
    return score;
}

#ifdef NONMATCHING
static void rank_stage_score(struct save_record *records, Sint32 score)
{
    int rank, place;

    /* The HUD can exceed the VMU field. Compare the value that will actually
     * be stored, so a larger score cannot displace a better saved value. */
    if (score > SAVE_SCORE_MAX) {
        score = SAVE_SCORE_MAX;
    }
    for (rank = 0; rank < SAVE_BEST; rank++) {
        if (score > save_record_score(&records[rank])) {
            for (place = SAVE_BEST - 1; place > rank; place--) {
                records[place].score_low = records[place - 1].score_low;
                records[place].score_high = records[place - 1].score_high;
            }
            save_record_set_score(&records[rank], score);
            return;
        }
    }
}
#endif

int save_download_read(void *buffer)
{
    if (card_load_file(download_place.drive, download_place.name, buffer,
                       DOWNLOAD_SIZE) == -1) {
        return 0;
    }
    return 1;
}

void save_download_set_drive(int drive)
{
    download_place.drive = drive;
}

/* nonzero while download_place's drive has a command queued. SHC writes
 * the body in place of the one call, in downloads_find_on, so nothing calls
 * the function itself */
/* @hack redundant: external linkage, which no call needs, where static would
 * do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
int save_download_busy(void)
{
    return buStat(download_place.drive);
}
/* NOLINTEND(misc-use-internal-linkage) */

void save_download_set_slot(int slot)
{
    sprintf(download_name, "SONIC2___D%02d", slot);
}

#ifdef NONMATCHING
/* Download files store addresses for the game's work areas. Bound each
 * address against the bytes the card delivered before following it. */
static Uint32 download_available(const void *buffer, Uint32 bytes,
                                 const void *address)
{
    uintptr_t base = (uintptr_t)buffer;
    uintptr_t start = (uintptr_t)address;

    if (start < base || start - base >= bytes) {
        return 0;
    }
    return bytes - (Uint32)(start - base);
}

static Bool download_texts_valid(const struct download_file *file, Sint32 bytes)
{
    int i;
    int j;

    if (bytes < (Sint32)sizeof(*file) || bytes > DOWNLOAD_SIZE) {
        return FALSE;
    }
    for (i = 0; i < DOWNLOAD_TEXTS; i++) {
        const struct download_text *text = file->texts[i];
        if ((uintptr_t)text % sizeof(void *) != 0 ||
            download_available(file, bytes, text) < sizeof(*text)) {
            return FALSE;
        }
        for (j = 0; j < DOWNLOAD_LINES; j++) {
            Uint32 available = download_available(file, bytes, text->lines[j]);
            if (available < DOWNLOAD_LINE_SIZE) {
                Uint32 k;
                for (k = 0; k < available; k++) {
                    if (text->lines[j][k] == 0) {
                        break;
                    }
                }
                if (k == available) {
                    return FALSE;
                }
            }
        }
    }
    return TRUE;
}
#endif

/* copy each download file on download_place's drive whose kind kinds asks
 * for into the next of entries. it loads each through buffer, and passes
 * over a slot that an entry already holds */
static void downloads_find_on(struct download_entry *entries, void *buffer,
                              Uint32 kinds)
{
    /* @hack redundant: info, which no code reads */
    BUS_DISKINFO info;
    /* @hack type: buffer as a void pointer under file, a typed local, where
     * file as the parameter would do */
    struct download_file *file = buffer;
    struct download_entry *entry;
    int i;
    int j;

    for (i = 0; i < DOWNLOAD_SLOTS; i++) {
        save_download_set_slot(i);
        if (save_download_read(buffer) == 0) {
            continue;
        }
        /* @bug logic: where a slot's load fails, downloads_find_on takes the
         * slot's kind, values and texts from what buffer held before. it
         * waits for the drive (JSR @R13 at 0x8c0bb000), but never tests the
         * load's result, as save_read never does */
        while (save_download_busy() != 0) {
            /* @hack redundant: the continue */
            continue; /* NOLINT(readability-redundant-control-flow) */
        }
#ifdef NONMATCHING
        /* this side repairs the tag above: it skips a slot whose load didn't
         * end with valid texts */
        if (!download_texts_valid(file,
                                  card_load_result(download_place.drive))) {
            continue;
        }
#endif
        if (slots_taken[i]) {
            continue;
        }
        if ((kinds & DOWNLOAD_FIND_ALL) ||
            (file->kind == DOWNLOAD_KIND_1 && (kinds & DOWNLOAD_FIND_KIND_1)) ||
            (file->kind == DOWNLOAD_KIND_2 && (kinds & DOWNLOAD_FIND_KIND_2)) ||
            (file->kind == DOWNLOAD_KIND_3 &&
             (kinds & DOWNLOAD_FIND_KINDS_3_4)) ||
            (file->kind == DOWNLOAD_KIND_4 &&
             (kinds & DOWNLOAD_FIND_KINDS_3_4))) {
            slots_taken[i] = 1;
            entry = &entries[entries_filled];
            entry->drive = download_place.drive;
            entry->slot = i;
            strncpy(entry->name, download_name, SAVE_NAME_LENGTH);
#ifdef NONMATCHING
            /* this side ends the name, where the matching side needs the
             * caller's 0 there */
            entry->name[SAVE_NAME_LENGTH] = 0;
#endif
            entry->kind = file->kind;
            for (j = 0; j < DOWNLOAD_VALUES; j++) {
                entry->values[j] = file->values[j];
            }
            for (j = 0; j < DOWNLOAD_TEXTS; j++) {
                strncpy(entry->texts[j].line0, file->texts[j]->lines[0],
                        DOWNLOAD_LINE_SIZE);
                strncpy(entry->texts[j].line1, file->texts[j]->lines[1],
                        DOWNLOAD_LINE_SIZE);
                strncpy(entry->texts[j].line2, file->texts[j]->lines[2],
                        DOWNLOAD_LINE_SIZE);
                strncpy(entry->texts[j].line3, file->texts[j]->lines[3],
                        DOWNLOAD_LINE_SIZE);
                strncpy(entry->texts[j].line4, file->texts[j]->lines[4],
                        DOWNLOAD_LINE_SIZE);
            }
            entries_filled++;
        }
    }
}

int save_downloads_find(struct download_entry *entries, Uint32 kinds)
{
    BUS_DISKINFO info;
    int drive;
    int i;
    char *taken;

    entries_filled = 0;
    /* @hack spelling: the slots cleared through a pointer under a count that
     * runs down to 0, where an index that counts up to DOWNLOAD_SLOTS would
     * do */
    taken = slots_taken;
    for (i = DOWNLOAD_SLOTS; i != 0; i--) {
        *taken++ = 0;
    }
    for (drive = 0; drive < CARD_DRIVES; drive++) {
        if (buGetDiskInfo(drive, &info) != 0) {
            continue;
        }
        save_download_set_drive(drive);
        downloads_find_on(entries, DEMO_AREA, kinds);
    }
    return entries_filled;
}

#ifdef NONMATCHING
void *save_download_load_sized(struct download_entry *entry,
                               struct download_file *file, Uint32 *size)
{
    Sint32 bytes;

    if (!size) {
        return NULL;
    }
    *size = 0;
    if (!entry || !file) {
        return NULL;
    }
    if (card_load_file(entry->drive, entry->name, file, DOWNLOAD_SIZE) == 0) {
        while (buStat(entry->drive) == BUD_STAT_BUSY) {
        }
        bytes = card_load_result(entry->drive);
        if (bytes >= (Sint32)sizeof(*file) && bytes <= DOWNLOAD_SIZE) {
            /* Distributed files encode addresses for DEMO_AREA. The kart
             * loader reads the same file into WORK_AREA. */
            *size = download_available(DEMO_AREA, bytes, file->data);
            if (*size) {
                return (Uint8 *)file + ((Uint32)bytes - *size);
            }
        }
    }
    return NULL;
}
#endif

void *save_download_load(struct download_entry *entry,
                         struct download_file *file)
{
#ifdef NONMATCHING
    /* this side checks the load's size too, by save_download_load_sized */
    Uint32 size;
    return save_download_load_sized(entry, file, &size);
#else
    if (card_load_file(entry->drive, entry->name, file, DOWNLOAD_SIZE) != -1) {
        while (buStat(entry->drive) == BUD_STAT_BUSY) {
        }
        if (buGetLastError(entry->drive) == 0) {
            return file->data;
        }
    }
    return NULL;
#endif
}

int save_best_rings(void)
{
    int stage = stage_current_id();

    /* @hack redundant: the else after a return */
    if (stage < SAVE_STAGES) {
        return save_data.stages[stage].records[game_state.mission][0].rings;
    } else if (stage == STAGE_CART_1) {
        if (stage_character(0) != PLAYER_ROUGE) {
            return save_data.cart_stage[SAVE_CART_OTHERS]
                .records[game_state.mission][0]
                .rings;
        }
        return save_data.cart_stage[SAVE_CART_ROUGE]
            .records[game_state.mission][0]
            .rings;
    }
    return -1;
}

Sint32 save_best_score(void)
{
    int stage = stage_current_id();

    /* @hack redundant: the else after a return */
    if (stage < SAVE_STAGES) {
        return save_record_score(
            &save_data.stages[stage].records[game_state.mission][0]);
    } else if (stage == STAGE_CART_1) {
        if (stage_character(0) != PLAYER_ROUGE) {
            return save_record_score(&save_data.cart_stage[SAVE_CART_OTHERS]
                                          .records[game_state.mission][0]);
        }
        return save_record_score(&save_data.cart_stage[SAVE_CART_ROUGE]
                                      .records[game_state.mission][0]);
    }
    return -1;
}

int save_best_time_parts(int *minute, int *second, int *hundredths)
{
    int stage = stage_current_id();

    /* @hack redundant: the else after a return */
    if (stage < SAVE_STAGES) {
        *minute =
            save_data.stages[stage].records[game_state.mission][0].time.minute;
        *second =
            save_data.stages[stage].records[game_state.mission][0].time.second;
        *hundredths = save_data.stages[stage]
                          .records[game_state.mission][0]
                          .time.hundredths;
        return 1;
    } else if (stage == STAGE_CART_1) {
        /* @hack spelling: the three stores written out for each record,
         * where a record pointer set from cart_stage indexed by
         * stage_character(0) != PLAYER_ROUGE ? SAVE_CART_OTHERS :
         * SAVE_CART_ROUGE, and the stores through it, would do */
        if (stage_character(0) != PLAYER_ROUGE) {
            *minute = save_data.cart_stage[SAVE_CART_OTHERS]
                          .records[game_state.mission][0]
                          .time.minute;
            *second = save_data.cart_stage[SAVE_CART_OTHERS]
                          .records[game_state.mission][0]
                          .time.second;
            *hundredths = save_data.cart_stage[SAVE_CART_OTHERS]
                              .records[game_state.mission][0]
                              .time.hundredths;
        } else {
            *minute = save_data.cart_stage[SAVE_CART_ROUGE]
                          .records[game_state.mission][0]
                          .time.minute;
            *second = save_data.cart_stage[SAVE_CART_ROUGE]
                          .records[game_state.mission][0]
                          .time.second;
            *hundredths = save_data.cart_stage[SAVE_CART_ROUGE]
                              .records[game_state.mission][0]
                              .time.hundredths;
        }
        return 1;
    }
    return 0;
}

int save_best_time(struct save_time *time)
{
    int stage = stage_current_id();

    /* @hack redundant: the else after a return */
    if (stage < SAVE_STAGES) {
        *time = save_data.stages[stage].records[game_state.mission][0].time;
        return 1;
    } else if (stage == STAGE_CART_1) {
        if (stage_character(0) != PLAYER_ROUGE) {
            *time = save_data.cart_stage[SAVE_CART_OTHERS]
                        .records[game_state.mission][0]
                        .time;
        } else {
            *time = save_data.cart_stage[SAVE_CART_ROUGE]
                        .records[game_state.mission][0]
                        .time;
        }
        return 1;
    }
    return 0;
}

void save_record_kart_time(int player, int minute, int second, int hundredths)
{
    struct save_kart_record *record;
    /* @hack spelling: best, the three best times as numbers, worked out
     * before the tests, where each worked out in its test would do */
    int best[SAVE_BEST];
    int value;
    int i;

    /* @bug logic: the test lets through kart_course SAVE_KART_RECORDS, past the
     * last kart record, and every negative kart_course. the test is a signed
     * kart_course > SAVE_KART_RECORDS (CMP/GT R2,R14 at 0x8c0bb698). for
     * kart_course SAVE_KART_RECORDS, the record read is the 12 bytes after
     * kart_records. they hold a pad byte, boss_totals[0]'s ranks and the start
     * of its plays. the time is ranked against those bytes, and can be stored
     * over them. a negative kart_course reaches back before kart_records */
#ifdef NONMATCHING
    /* this side repairs the tag above: it records no time for a course outside
     * kart_records */
    if (!SAVE_KART_COURSE_VALID(game_state.kart_course)) {
        return;
    }
#else
    if (game_state.kart_course > SAVE_KART_RECORDS) {
        return;
    }
#endif
    record = &save_data.kart_records[game_state.kart_course];
    value = (minute * HUNDREDTHS_PER_MINUTE) +
            (second * HUNDREDTHS_PER_SECOND) + hundredths;
    for (i = 0; i < SAVE_BEST; i++) {
        best[i] = (record->best[i].time.minute * HUNDREDTHS_PER_MINUTE) +
                  (record->best[i].time.second * HUNDREDTHS_PER_SECOND) +
                  record->best[i].time.hundredths;
    }
    if (value < best[0]) {
        record->best[2] = record->best[1];
        record->best[1] = record->best[0];
        record->best[0].time.minute = minute;
        record->best[0].time.second = second;
        record->best[0].time.hundredths = hundredths;
        record->best[0].character = stage_character(player);
    } else if (value < best[1]) {
        record->best[2] = record->best[1];
        record->best[1].time.minute = minute;
        record->best[1].time.second = second;
        record->best[1].time.hundredths = hundredths;
        record->best[1].character = stage_character(player);
    } else if (value < best[2]) {
        record->best[2].time.minute = minute;
        record->best[2].time.second = second;
        record->best[2].time.hundredths = hundredths;
        record->best[2].character = stage_character(player);
    }
}

int save_record_stage(Sint32 score)
{
    struct save_stage *stage;
    int id;
    int value;

    id = stage_current_id();
    if (id < SAVE_STAGES) {
        switch (id) {
        case STAGE_VERSUS_KNUCKLES:
        case STAGE_RELAY_SONIC:
        case STAGE_RELAY_EGGMAN:
        case STAGE_RELAY_TAILS:
        case STAGE_RELAY_ROUGE:
        case STAGE_RELAY_KNUCKLES:
            id = STAGE_SHARED_RECORDS;
            break;
        }
        stage = &save_data.stages[id];
    } else if (id == STAGE_CART_1) {
        if (stage_character(0) != PLAYER_ROUGE) {
            stage = &save_data.cart_stage[SAVE_CART_OTHERS];
        } else {
            stage = &save_data.cart_stage[SAVE_CART_ROUGE];
        }
    } else {
        return 0;
    }

#ifdef NONMATCHING
    /* this side repairs the tag below: rank_stage_score compares each record's
     * whole score */
    rank_stage_score(stage->records[game_state.mission], score);
#else
    /* @bug logic: the score is ranked against the second and third records
     * by their score_low alone, the low 16 bits (MOV.W @(10,R3),R0 at
     * 0x8c0bb92a and MOV.W @(18,R3),R0 at 0x8c0bb976). so a record of 65536
     * or more can lose its place to a lower score */
    if (score > save_record_score(&stage->records[game_state.mission][0])) {
        save_record_set_score(
            &stage->records[game_state.mission][2],
            save_record_score(&stage->records[game_state.mission][1]));
        save_record_set_score(
            &stage->records[game_state.mission][1],
            save_record_score(&stage->records[game_state.mission][0]));
        save_record_set_score(&stage->records[game_state.mission][0], score);
    } else if (score > stage->records[game_state.mission][1].score_low) {
        save_record_set_score(
            &stage->records[game_state.mission][2],
            save_record_score(&stage->records[game_state.mission][1]));
        save_record_set_score(&stage->records[game_state.mission][1], score);
    } else if (score > stage->records[game_state.mission][2].score_low) {
        save_record_set_score(&stage->records[game_state.mission][2], score);
    }
#endif

    if (game_state.rings[0] > stage->records[game_state.mission][0].rings) {
        stage->records[game_state.mission][2].rings =
            stage->records[game_state.mission][1].rings;
        stage->records[game_state.mission][1].rings =
            stage->records[game_state.mission][0].rings;
        stage->records[game_state.mission][0].rings = game_state.rings[0];
    } else if (game_state.rings[0] >
               stage->records[game_state.mission][1].rings) {
        stage->records[game_state.mission][2].rings =
            stage->records[game_state.mission][1].rings;
        stage->records[game_state.mission][1].rings = game_state.rings[0];
    } else if (game_state.rings[0] >
               stage->records[game_state.mission][2].rings) {
        stage->records[game_state.mission][2].rings = game_state.rings[0];
    }

    value = TIME_PARTS_VALUE(game_state.minute, game_state.second,
                             HUNDREDTHS(game_state.frame));
    if (value < TIME_VALUE(stage->records[game_state.mission][0].time)) {
        stage->records[game_state.mission][2].time =
            stage->records[game_state.mission][1].time;
        stage->records[game_state.mission][1].time =
            stage->records[game_state.mission][0].time;
        stage->records[game_state.mission][0].time.minute = game_state.minute;
        stage->records[game_state.mission][0].time.second = game_state.second;
        stage->records[game_state.mission][0].time.hundredths =
            HUNDREDTHS(game_state.frame);
    } else if (value < TIME_VALUE(stage->records[game_state.mission][1].time)) {
        stage->records[game_state.mission][2].time =
            stage->records[game_state.mission][1].time;
        stage->records[game_state.mission][1].time.minute = game_state.minute;
        stage->records[game_state.mission][1].time.second = game_state.second;
        stage->records[game_state.mission][1].time.hundredths =
            HUNDREDTHS(game_state.frame);
    } else if (value < TIME_VALUE(stage->records[game_state.mission][2].time)) {
        stage->records[game_state.mission][2].time.minute = game_state.minute;
        stage->records[game_state.mission][2].time.second = game_state.second;
        stage->records[game_state.mission][2].time.hundredths =
            HUNDREDTHS(game_state.frame);
    }

    if (id != STAGE_ALL_EMBLEMS) {
        if (game_state.rank + 1 > stage->ranks[game_state.mission]) {
            stage->ranks[game_state.mission] = game_state.rank + 1;
        }
    }
    save_award();
    save_data.stage = stage_current_id();
    save_data.character = stage_character(0);
    return 1;
}

void save_record_boss_total(int which, int minute, int second, int hundredths)
{
    struct save_stage *stage = &save_data.boss_totals[which];
    int value = TIME_PARTS_VALUE(minute, second, hundredths);

    if (value < TIME_VALUE(stage->records[0][0].time)) {
        stage->records[0][2].time = stage->records[0][1].time;
        stage->records[0][1].time = stage->records[0][0].time;
        stage->records[0][0].time.minute = minute;
        stage->records[0][0].time.second = second;
        stage->records[0][0].time.hundredths = hundredths;
    } else if (value < TIME_VALUE(stage->records[0][1].time)) {
        stage->records[0][2].time = stage->records[0][1].time;
        stage->records[0][1].time.minute = minute;
        stage->records[0][1].time.second = second;
        stage->records[0][1].time.hundredths = hundredths;
    } else if (value < TIME_VALUE(stage->records[0][2].time)) {
        stage->records[0][2].time.minute = minute;
        stage->records[0][2].time.second = second;
        stage->records[0][2].time.hundredths = hundredths;
    }

    stage->ranks[0] = 1;
}

/* 1 where every mission of stage is at SAVE_RANK_TOP, and 0 otherwise.
 * cart_all_top asks it of character's record in cart_stage */
static int stage_all_top(int stage)
{
    int i;

    for (i = 0; i < MISSIONS; i++) {
        if (save_data.stages[stage].ranks[i] != SAVE_RANK_TOP) {
            return 0;
        }
    }
    return 1;
}

static int cart_all_top(int character)
{
    int i;

    /* @hack spelling: a loop for each record, where a pointer to cart_stage
     * indexed by character != PLAYER_ROUGE ? SAVE_CART_OTHERS :
     * SAVE_CART_ROUGE, and one loop through it, would do */
    if (character != PLAYER_ROUGE) {
        for (i = 0; i < MISSIONS; i++) {
            if (save_data.cart_stage[SAVE_CART_OTHERS].ranks[i] !=
                SAVE_RANK_TOP) {
                return 0;
            }
        }
    } else {
        for (i = 0; i < MISSIONS; i++) {
            if (save_data.cart_stage[SAVE_CART_ROUGE].ranks[i] !=
                SAVE_RANK_TOP) {
                return 0;
            }
        }
    }
    return 1;
}

/* 1 where every mission of stage has a rank, and 0 otherwise.
 * cart_all_ranked asks it of character's record in cart_stage */
static int stage_all_ranked(int stage)
{
    int i;

    for (i = 0; i < MISSIONS; i++) {
        if (save_data.stages[stage].ranks[i] == SAVE_RANK_NONE) {
            return 0;
        }
    }
    return 1;
}

static int cart_all_ranked(int character)
{
    int i;

    /* @hack spelling: a loop for each record, where a pointer to cart_stage
     * indexed by character != PLAYER_ROUGE ? SAVE_CART_OTHERS :
     * SAVE_CART_ROUGE, and one loop through it, would do */
    if (character != PLAYER_ROUGE) {
        for (i = 0; i < MISSIONS; i++) {
            if (save_data.cart_stage[SAVE_CART_OTHERS].ranks[i] ==
                SAVE_RANK_NONE) {
                return 0;
            }
        }
    } else {
        for (i = 0; i < MISSIONS; i++) {
            if (save_data.cart_stage[SAVE_CART_ROUGE].ranks[i] ==
                SAVE_RANK_NONE) {
                return 0;
            }
        }
    }
    return 1;
}

void save_award(void)
{
    int i;
    int j;
    /* @hack spelling: stage, a local for emblem_stages[i], where
     * save_data.stages[emblem_stages[i]] in its place would do */
    int stage;
    /* @hack spelling: is_set, a local for flag_get, where flag_get called by
     * name would do */
    Uint8 (*is_set)(int flag);

    if (game_state.unlock_pending != 0) {
        return;
    }
    game_state.unlock_id = 0;
    is_set = flag_get;
    if (SONIC_STAGES(stage_all_ranked) && !is_set(FLAG_RANKED(GROUP_SONIC))) {
        flag_set(FLAG_RANKED(GROUP_SONIC));
        game_state.unlock_pending = 1;
        game_state.unlock_id = AWARD_RANKED(GROUP_SONIC);
    }
    if (SHADOW_STAGES(stage_all_ranked) && !is_set(FLAG_RANKED(GROUP_SHADOW))) {
        flag_set(FLAG_RANKED(GROUP_SHADOW));
        game_state.unlock_pending = 1;
        game_state.unlock_id = AWARD_RANKED(GROUP_SHADOW);
    }
    if (TAILS_STAGES(stage_all_ranked, cart_all_ranked) &&
        !is_set(FLAG_RANKED(GROUP_TAILS))) {
        flag_set(FLAG_RANKED(GROUP_TAILS));
        game_state.unlock_pending = 1;
        game_state.unlock_id = AWARD_RANKED(GROUP_TAILS);
    }
    if (KNUCKLES_STAGES(stage_all_ranked) &&
        !is_set(FLAG_RANKED(GROUP_KNUCKLES))) {
        flag_set(FLAG_RANKED(GROUP_KNUCKLES));
        game_state.unlock_pending = 1;
        game_state.unlock_id = AWARD_RANKED(GROUP_KNUCKLES);
    }
    if (EGGMAN_STAGES(stage_all_ranked) && !is_set(FLAG_RANKED(GROUP_EGGMAN))) {
        flag_set(FLAG_RANKED(GROUP_EGGMAN));
        game_state.unlock_pending = 1;
        game_state.unlock_id = AWARD_RANKED(GROUP_EGGMAN);
    }
    if (ROUGE_STAGES(stage_all_ranked, cart_all_ranked) &&
        !is_set(FLAG_RANKED(GROUP_ROUGE))) {
        flag_set(FLAG_RANKED(GROUP_ROUGE));
        game_state.unlock_pending = 1;
        game_state.unlock_id = AWARD_RANKED(GROUP_ROUGE);
    }
    /* @hack spelling: each cart flag tested in an if of its own, not joined
     * to its ranks' test by && */
    if (save_data.cart_stage[SAVE_CART_OTHERS].ranks[0] ||
        save_data.cart_stage[SAVE_CART_ROUGE].ranks[0]) {
        if (!is_set(FLAG_CART_ONE)) {
            flag_set(FLAG_CART_ONE);
            game_state.unlock_pending = 1;
            game_state.unlock_id = AWARD_CART_ONE;
        }
    }
    if (save_data.cart_stage[SAVE_CART_OTHERS].ranks[0] &&
        save_data.cart_stage[SAVE_CART_ROUGE].ranks[0]) {
        if (!is_set(FLAG_CART_BOTH)) {
            flag_set(FLAG_CART_BOTH);
            game_state.unlock_pending = 1;
            game_state.unlock_id = AWARD_CART_BOTH;
        }
    }

    /* @hack spelling: the awards in a loop over a switch on i, where the
     * cases written out in turn would do */
    /* @hack spelling: each case's two tests in ifs of their own, not joined
     * by && */
    for (i = 0; i < SAVE_AWARDS; i++) {
        switch (i) {
        case SAVE_AWARD_SONIC_TOP:
            if (SONIC_STAGES(stage_all_top)) {
                if (!save_data.awards[i]) {
                    save_data.awards[i] = 1;
                    game_state.unlock_pending = 1;
                    game_state.unlock_id = AWARD_TOP(GROUP_SONIC);
                }
            }
            break;
        case SAVE_AWARD_SHADOW_TOP:
            if (SHADOW_STAGES(stage_all_top)) {
                if (!save_data.awards[i]) {
                    save_data.awards[i] = 1;
                    game_state.unlock_pending = 1;
                    game_state.unlock_id = AWARD_TOP(GROUP_SHADOW);
                }
            }
            break;
        case SAVE_AWARD_TAILS_TOP:
            if (TAILS_STAGES(stage_all_top, cart_all_top)) {
                if (!save_data.awards[i]) {
                    save_data.awards[i] = 1;
                    game_state.unlock_pending = 1;
                    game_state.unlock_id = AWARD_TOP(GROUP_TAILS);
                }
            }
            break;
        case SAVE_AWARD_EGGMAN_TOP:
            if (EGGMAN_STAGES(stage_all_top)) {
                if (!save_data.awards[i]) {
                    save_data.awards[i] = 1;
                    game_state.unlock_pending = 1;
                    game_state.unlock_id = AWARD_TOP(GROUP_EGGMAN);
                }
            }
            break;
        case SAVE_AWARD_KNUCKLES_TOP:
            if (KNUCKLES_STAGES(stage_all_top)) {
                if (!save_data.awards[i]) {
                    save_data.awards[i] = 1;
                    game_state.unlock_pending = 1;
                    game_state.unlock_id = AWARD_TOP(GROUP_KNUCKLES);
                }
            }
            break;
        case SAVE_AWARD_ROUGE_TOP:
            if (ROUGE_STAGES(stage_all_top, cart_all_top)) {
                if (!save_data.awards[i]) {
                    save_data.awards[i] = 1;
                    game_state.unlock_pending = 1;
                    game_state.unlock_id = AWARD_TOP(GROUP_ROUGE);
                }
            }
            break;
        case SAVE_AWARD_HERO_STORY:
            if (game_bits[HERO_STORY_BIT]) {
                if (!save_data.awards[i]) {
                    save_data.awards[i] = 1;
                    game_state.unlock_pending = 1;
                    game_state.unlock_id = AWARD_HERO_STORY;
                }
            }
            break;
        case SAVE_AWARD_DARK_STORY:
            if (game_bits[DARK_STORY_BIT]) {
                if (!save_data.awards[i]) {
                    save_data.awards[i] = 1;
                    game_state.unlock_pending = 1;
                    game_state.unlock_id = AWARD_DARK_STORY;
                }
            }
            break;
        case SAVE_AWARD_LAST_STORY:
            if (game_bits[LAST_STORY_BIT]) {
                if (!save_data.awards[i]) {
                    /* @hack type: the store through a Sint8 pointer, where
                     * save_data.awards[i] = 1 would do */
                    ((Sint8 *)save_data.awards)[i] = 1;
                    game_state.unlock_pending = 1;
                    game_state.unlock_id = AWARD_LAST_STORY;
                    for (j = FLAG_MUSIC_HEARD; j <= FLAG_HEARD_SET_LAST; j++) {
                        game_flags[j] = 0xff;
                    }
                    heard_bits[MUSIC_G_HILL / 8] &= ~(1 << (MUSIC_G_HILL % 8));
                }
            }
            break;
        /* @hack redundant: the boss awards' cases, which give the switch's
         * table its twelve entries */
        case SAVE_AWARD_FIRST_BOSS:
        case SAVE_AWARD_FIRST_BOSS + 1:
        case SAVE_AWARD_FIRST_BOSS + 2:
            break;
        }
    }

    save_data.emblems = 0;
    /* @hack type: i compared as a Uint32 with the table's length, where a
     * signed test would do */
    for (i = 0; (Uint32)i < sizeof(emblem_stages) / sizeof(emblem_stages[0]);
         i++) {
        stage = emblem_stages[i];
        /* @hack spelling: j = j + 1, where j++ would do */
        for (j = 0; j < MISSIONS; j = j + 1) {
            if (save_data.stages[stage].ranks[j]) {
                save_data.emblems++;
            }
        }
    }
    for (j = 0; j < MISSIONS; j++) {
        if (save_data.cart_stage[SAVE_CART_OTHERS].ranks[j]) {
            save_data.emblems++;
        }
        if (save_data.cart_stage[SAVE_CART_ROUGE].ranks[j]) {
            save_data.emblems++;
        }
    }
    for (j = 0; j < SAVE_CHAO_EMBLEMS; j++) {
        if (save_data.chao_emblems[j]) {
            save_data.emblems++;
        }
    }
    for (j = 0; j < SAVE_KART_RECORDS; j++) {
        if (save_data.kart_records[j].emblem) {
            save_data.emblems++;
        }
    }
    for (j = 0; j < SAVE_AWARDS; j++) {
        if (save_data.awards[j]) {
            save_data.emblems++;
        }
    }
    if (save_data.emblems == SAVE_EMBLEMS && !flag_get(FLAG_ALL_EMBLEMS)) {
        flag_set(FLAG_ALL_EMBLEMS);
        game_state.unlock_pending = 1;
        game_state.unlock_id = AWARD_ALL_EMBLEMS;
        save_data.stages[STAGE_ALL_EMBLEMS].ranks[0] = 1;
    }
}

void save_keep(void)
{
    while (save_busy() != 0) {
    }
    kept_save = save_data;
}

void save_restore(void)
{
    save_data = kept_save;
}

void save_copy(struct save_data *from, struct save_data *to)
{
    if (!from) {
        from = &kept_save;
    }
    if (!to) {
        to = &kept_save;
    }
    *to = *from;
}

/* 1 where data's date isn't game_state's save_date, and 0 otherwise */
/* @hack type: data as a void pointer under save, a typed local, where a
 * struct save_data pointer would do */
static int save_is_another(void *data)
{
    struct save_data *save = data;

    /* @hack type: the dates compared as Sint32s, where the Uint32s they are
     * would do */
    if ((Sint32)game_state.save_date != (Sint32)save->date) {
        return 1;
    }
    return 0;
}

/* the port of each drive's socket */
static const Uint32 drive_ports[CARD_DRIVES] = {
    PDD_PORT_A1, PDD_PORT_A2, PDD_PORT_B1, PDD_PORT_B2,
    PDD_PORT_C1, PDD_PORT_C2, PDD_PORT_D1, PDD_PORT_D2,
};

void save_count_time(void)
{
    /* @hack spelling: the two tests nested, not joined by && */
    if (save_data.emblems < SAVE_EMBLEMS) {
        if (save_data.emblem_time < SAVE_TIME_MAX) {
            save_data.emblem_time++;
        }
    }
    if (save_data.play_time < SAVE_TIME_MAX) {
        save_data.play_time++;
    }
}

static void save_encrypt(void *data, Int size)
{
    Uint32 *words = data;
    Uint32 key = CIPHER_KEY;
    Int i;

    for (i = 0; i < size / (Int)sizeof *words; i++) {
        /* @hack spelling: the step and the XOR as two statements, where
         * *words = (*words + CIPHER_STEP) ^ key would do */
        *words += CIPHER_STEP;
        *words ^= key;
        words++;
        key += CIPHER_STEP;
    }
}

static void save_decrypt(void *data, Int size)
{
    Uint32 *words = data;
    Uint32 key = CIPHER_KEY;
    Int i;

    for (i = 0; i < size / (Int)sizeof *words; i++) {
        /* @hack spelling: the XOR and the step as two statements, where
         * *words = (*words ^ key) - CIPHER_STEP would do */
        *words ^= key;
        *words -= CIPHER_STEP;
        words++;
        key += CIPHER_STEP;
    }
}

void save_clear(void)
{
    SYS_RTC_DATE date;
    Uint8 *bytes;
    Uint32 left;
    int i;
    int j;

    bytes = (Uint8 *)&save_data;
    /* @hack type: i compared as a Uint32 with the size, where a signed test
     * would do */
    for (i = 0; (Uint32)i < sizeof(save_data); i++) {
        bytes[i] = 0;
    }
    syRtcGetDate(&date);
    save_data.date =
        (date.day << 24) + (date.hour << 16) + (date.minute << 8) + date.second;
    game_state.save_date = save_data.date;
    play_reset();
    /* @hack spelling: the flags and game_bits cleared through a pointer
     * under a count that runs down to 0, where an index that counts up to
     * each array's size would do */
    bytes = game_flags;
    for (left = sizeof(game_flags); left != 0; left--) {
        *bytes++ = 0;
    }
    bytes = game_bits;
    for (left = sizeof(game_bits); left != 0; left--) {
        *bytes++ = 0;
    }
    syCfgGetIndividualID(save_data.console_id);
    /* @hack redundant: status cleared again, which the first loop zeroed and
     * no call since writes */
    save_data.status = 0;
    /* @hack type: i compared as a Uint32 with the table's length, where a
     * signed test would do */
    for (i = 0; (Uint32)i < sizeof(new_save_music) / sizeof(new_save_music[0]);
         i++) {
        music_mark_heard_number(new_save_music[i]);
    }
    for (i = 0; i < MISSIONS; i++) {
        for (j = 0; j < SAVE_STAGES; j++) {
            save_data.stages[j].records[i][0].time.minute = NEW_MINUTES(0);
            save_data.stages[j].records[i][1].time.minute = NEW_MINUTES(1);
            save_data.stages[j].records[i][2].time.minute = NEW_MINUTES(2);
        }
        for (j = 0; j < SAVE_CART_STAGES; j++) {
            save_data.cart_stage[j].records[i][0].time.minute = NEW_MINUTES(0);
            save_data.cart_stage[j].records[i][1].time.minute = NEW_MINUTES(1);
            save_data.cart_stage[j].records[i][2].time.minute = NEW_MINUTES(2);
        }
    }
    for (j = 0; j < SAVE_BOSS_TOTALS; j++) {
        save_data.boss_totals[j].records[0][0].time.minute =
            NEW_BOSS_MINUTES(0);
        save_data.boss_totals[j].records[0][1].time.minute =
            NEW_BOSS_MINUTES(1);
        save_data.boss_totals[j].records[0][2].time.minute =
            NEW_BOSS_MINUTES(2);
    }
    for (j = 0; j < SAVE_KART_RECORDS; j++) {
        save_data.kart_records[j].best[0].time.minute = NEW_MINUTES(0);
        save_data.kart_records[j].best[1].time.minute = NEW_MINUTES(1);
        save_data.kart_records[j].best[2].time.minute = NEW_MINUTES(2);
    }
    for (j = 0; j < DOWNLOAD_SLOTS; j++) {
        save_data.download_best_times[j].minute = NEW_MINUTES(0);
    }
    game_state.emblems = save_data.emblems;
}

Sint32 save_check_sockets(void)
{
    Sint32 changed = 0;
    Sint32 i;
    /* @hack spelling: info, a local for pdGetPeripheralInfo's result, where
     * the type read from the call's result in place would do */
    const PDS_PERIPHERALINFO *info;
    /* @hack spelling: type, a local for the device's type, where info->type
     * read in place would do */
    Uint32 type;

    for (i = 0; i < CARD_DRIVES; i++) {
        info = pdGetPeripheralInfo(drive_ports[i]);
        type = info->type;
        if (socket_types[i] != type) {
            changed = 1;
            socket_types[i] = type;
        }
    }
    if (changed) {
        for (i = 0; i < CARD_DRIVES; i++) {
            while (buStat(i)) {
            }
        }
    }
    return changed;
}
