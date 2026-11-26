/* the save and the download files. the save keeps the game's progress,
 * records and awards in a main file on a memory card */
#ifndef SAVE_SAVE_H
#define SAVE_SAVE_H
#include <sg_xpt.h>

#include "player/player.h"
#include "system/flags.h"
#include "system/game_state.h"

/* each stage below SAVE_STAGES keeps records for each of its MISSIONS
 * missions. each mission keeps its SAVE_BEST best rings, scores and times,
 * best first */
#define SAVE_STAGES 46
#define SAVE_BEST 3

/* a mission's rank in save_data, from SAVE_RANK_NONE to SAVE_RANK_TOP.
 * save_record_stage raises it to the best game_state.rank + 1 */
#define SAVE_RANK_NONE 0
#define SAVE_RANK_TOP 5

/* the bytes of save_data.awards, and of save_data.chao_emblems */
#define SAVE_AWARDS 12
#define SAVE_CHAO_EMBLEMS 10
/* each award's byte in save_data.awards. save_award gives a character's
 * top award for SAVE_RANK_TOP in every mission of the character's stages.
 * it gives each story's byte for a bit of game_bits. hud/hud.h's
 * boss_total_start gives the three from SAVE_AWARD_FIRST_BOSS, one for each
 * of system/sequence.h's SEQUENCE_HERO_BOSSES, SEQUENCE_DARK_BOSSES and
 * SEQUENCE_ALL_BOSSES */
#define SAVE_AWARD_SONIC_TOP 0
#define SAVE_AWARD_SHADOW_TOP 1
#define SAVE_AWARD_TAILS_TOP 2
#define SAVE_AWARD_EGGMAN_TOP 3
#define SAVE_AWARD_KNUCKLES_TOP 4
#define SAVE_AWARD_ROUGE_TOP 5
#define SAVE_AWARD_HERO_STORY 6
#define SAVE_AWARD_DARK_STORY 7
#define SAVE_AWARD_LAST_STORY 8
#define SAVE_AWARD_FIRST_BOSS 9
#define SAVE_BOSS_AWARDS 3

/* save_award's groups of stages, one to a character. FLAG_RANKED,
 * AWARD_RANKED and AWARD_TOP take a group's number */
#define GROUP_SONIC 1
#define GROUP_TAILS 2
#define GROUP_KNUCKLES 3
#define GROUP_SHADOW 4
#define GROUP_EGGMAN 5
#define GROUP_ROUGE 6

/* each award has a number, which save_award leaves in game_state.unlock_id. a
 * flag of game_flags or a byte of save_data.awards records an award given, so
 * each is given once.
 *
 * a story's award comes with a bit of game_bits, and the story's byte of
 * awards records it.
 *
 * AWARD_CART_ONE comes with a rank in the first mission of either of
 * cart_stage's records. AWARD_CART_BOTH comes with a rank in both.
 * FLAG_CART_ONE and FLAG_CART_BOTH record them. the groups' awards and flags
 * follow these, by the group's number.
 *
 * a group earns its AWARD_RANKED with a rank in every mission of its stages.
 * its FLAG_RANKED records that. it earns its AWARD_TOP with SAVE_RANK_TOP in
 * each of them, and its character's byte of awards records that.
 *
 * AWARD_ALL_EMBLEMS comes once save_award counts SAVE_EMBLEMS emblems, and
 * FLAG_ALL_EMBLEMS records it */
#define AWARD_HERO_STORY 1
#define AWARD_DARK_STORY 2
#define AWARD_LAST_STORY 3
#define AWARD_CART_ONE 4
#define FLAG_CART_ONE 90
#define AWARD_CART_BOTH 5
#define FLAG_CART_BOTH 91
#define AWARD_RANKED(group) (AWARD_CART_BOTH + (group))
#define FLAG_RANKED(group) (FLAG_CART_BOTH + (group))
#define AWARD_TOP(group) (AWARD_RANKED(GROUP_ROUGE) + (group))
#define AWARD_ALL_EMBLEMS 18
#define FLAG_ALL_EMBLEMS 98

/* STAGE_CART_1's records in cart_stage: for every character but Rouge, and
 * for Rouge */
#define SAVE_CART_OTHERS 0
#define SAVE_CART_ROUGE 1
#define SAVE_CART_STAGES 2

/* the kart courses' records, and the boss totals' */
#define SAVE_KART_RECORDS 3
#ifdef NONMATCHING
/* whether course names a kart record. the unsigned compare refuses a negative
 * course too */
#define SAVE_KART_COURSE_VALID(course) ((Uint32)(course) < SAVE_KART_RECORDS)
#endif
#define SAVE_BOSS_TOTALS 3

/* the stages whose missions' ranks save_award counts as emblems: each of its
 * groups' stages but STAGE_CART_1, and STAGE_RELAY_LAST */
#define SAVE_EMBLEM_STAGES 29
/* the most emblems save_award counts: each mission with a rank, and each
 * emblem byte that isn't 0 */
#define SAVE_EMBLEMS                                                           \
    ((SAVE_EMBLEM_STAGES * MISSIONS) + (SAVE_CART_STAGES * MISSIONS) +         \
     SAVE_AWARDS + SAVE_CHAO_EMBLEMS + SAVE_KART_RECORDS)

/* the story sequences, SEQUENCE_HERO to SEQUENCE_LAST of system/sequence.h,
 * whose progress the save keeps */
#define SAVE_STORIES 3
/* the bytes that hold game_bits in the save */
#define SAVE_BIT_BYTES 86

/* the download files' slots, 0 to 99 */
#define DOWNLOAD_SLOTS 100

/* the main file's size in blocks of 512 bytes. its 128-byte header, its
 * 512-byte icon and save_data fill 18 */
#define SAVE_BLOCKS 18

struct save_time {
    Sint8 minute;
    Sint8 second;
    Sint8 hundredths;
};

/* a mission's record of rings, score and time. the score's low
 * SAVE_SCORE_LOW_BITS bits are score_low, and its next 8 bits score_high */
#define SAVE_SCORE_LOW_BITS 16
#ifdef NONMATCHING
/* the highest score a record's score_low and score_high hold */
#define SAVE_SCORE_MAX 0xffffffL
#endif
struct save_record {
    Sint16 rings;
    Uint16 score_low;
    struct save_time time;
    Uint8 score_high;
};

/* a stage's ranks and records, by mission. rings, scores and times rank
 * apart, so the best of each can come from a different play */
struct save_stage {
    Sint8 ranks[MISSIONS];
    /* the times the stage's step has started each mission */
    Uint16 plays[MISSIONS];
    struct save_record records[MISSIONS][SAVE_BEST];
};

/* a kart time, and the character who set it */
struct save_kart_time {
    struct save_time time;
    Sint8 character;
};

/* a kart course's best times and its emblem. CART's code records the times
 * with save_record_kart_time. save_award counts the emblem unless it is 0 */
struct save_kart_record {
    struct save_kart_time best[SAVE_BEST];
    Uint8 emblem;
};

/* the bits of save_data's status. ADVERTISE.PRS's options set
 * SAVE_THEMES_REFUSED once a theme file lacks its key, and then read no theme
 * file. pad_scan sets SAVE_CARD_MARK. main sets SAVE_SWAP_FAILED where
 * snapshots_swap fails */
#define SAVE_THEMES_REFUSED 1
#define SAVE_CARD_MARK 2
#define SAVE_SWAP_FAILED 4

/* the save, which the main file holds encrypted. checksum is the sum of the
 * bytes after it */
struct save_data {
    Uint32 checksum;
    /* when save_clear made the save: the day, hour, minute and second, a
     * byte each from the top */
    Uint32 date;
    Sint8 text_language;
    Sint8 voice_language;
    /* for each story in turn, one past the furthest step its sequence has
     * reached */
    Uint8 progress[SAVE_STORIES];
    Uint8 emblems; /* the emblems save_award last counted */
    Sint8 vibration;
    /* stage_character(0) when save_record_stage last recorded a stage */
    Sint8 character;
    Sint8 stage; /* the stage save_record_stage last recorded */
    Uint8 status; /* SAVE_THEMES_REFUSED, SAVE_CARD_MARK and SAVE_SWAP_FAILED */
    Sint8 theme;
    Sint8 console_id[6]; /* syCfgGetIndividualID's ID */
    Uint8 unk_19[3];
    Sint16 lives[2];
    /* the place the emblem results draw under their ranking */
    Sint32 ranking_place;
    /* save_count_time's calls while emblems is below SAVE_EMBLEMS */
    Sint32 emblem_time;
    Sint32 play_time; /* save_count_time's calls */
    Uint8 unk_2c[0x40];
    struct save_stage stages[SAVE_STAGES];
    /* STAGE_CART_1's records. each counts as a stage of save_award's
     * groups */
    struct save_stage cart_stage[SAVE_CART_STAGES];
    struct save_kart_record kart_records[SAVE_KART_RECORDS];
    /* each boss total keeps its best times in its first mission's records.
     * save_record_boss_total sets that mission's rank to 1 */
    struct save_stage boss_totals[SAVE_BOSS_TOTALS];
    Sint8 chao_emblems[SAVE_CHAO_EMBLEMS]; /* each an emblem unless 0 */
    /* by character: 1 once it has entered Chao World. save_write also sets
     * player_character(0)'s while game_state.chao_key isn't 0 */
    Sint8 played[PLAYER_EGGMAN_MECH + 1];
    Uint8 flags[GAME_FLAGS];
    /* game_bits, packed: game_bits[i] is bit i % 8 of bits[i / 8]. the
     * GAME_BITS bits fill bits[0] to bits[76], and the rest hold none */
    Uint8 bits[SAVE_BIT_BYTES];
    /* by award, from SAVE_AWARD_SONIC_TOP: 1 once it is given */
    Uint8 awards[SAVE_AWARDS];
    /* by slot, the best time CART's code keeps for the download file there */
    struct save_time download_best_times[DOWNLOAD_SLOTS];
    Uint8 unk_1f4c[0x200];
};

/* the save in play */
extern struct save_data save_data;

/* nonzero once a character has entered Chao World, or saved with a Chao
 * key, as played records it */
#define SAVE_CHAO_ENTERED                                                      \
    (save_data.played[PLAYER_SONIC] || save_data.played[PLAYER_SHADOW] ||      \
     save_data.played[PLAYER_TAILS] || save_data.played[PLAYER_EGGMAN] ||      \
     save_data.played[PLAYER_KNUCKLES] || save_data.played[PLAYER_ROUGE] ||    \
     save_data.played[PLAYER_TAILS_MECH] ||                                    \
     save_data.played[PLAYER_EGGMAN_MECH])

/* where a file goes: its drive, or SAVE_DRIVE_NONE, and its name. a drive's
 * low bit is its socket, and the bits above are its port. save_place's
 * drive can also be SAVE_DRIVE_REFUSED */
struct save_place {
    int drive;
    char *name;
};
/* the main file's place. save_set_drive sets its drive. the stage's step
 * and the file select set SAVE_DRIVE_REFUSED where a player chooses to play
 * without the save. the stage's step then asks for no card again */
extern struct save_place save_place;
#define SAVE_DRIVE_REFUSED (-2)
#define SAVE_DRIVE_NONE (-1)
/* a drive's port, from 0 for A to 3 for D, and its socket's number, 1 or 2 */
#define SAVE_DRIVE_PORT(drive) (((drive) >> 1) & 3)
#define SAVE_DRIVE_SOCKET(drive) (((drive) & 1) + 1)

/* start saving save_data, encrypted, to the main file on save_place's
 * drive.
 *
 * save_data first takes in game_flags, game_bits and the progress of a
 * running story's sequence. it also takes game_state's languages,
 * vibration, lives and theme, and the console's individual ID. save_award
 * then runs.
 *
 * without a drive, save_write stops there and returns no defined value.
 * with one, it marks player_character(0) played while game_state.chao_key
 * isn't 0. it sets checksum, stops the vibration and starts the save. it
 * returns 1 where the save starts, and 0 where it can't. either way,
 * save_data is unencrypted again, and save_keep's copy holds the same bytes.
 * the save works from an image of its own, so save_data may change once
 * save_write returns */
int save_write(void);
/* load the main file into save_data, wait for the drive, and decrypt
 * save_data in place.
 *
 * it returns SAVE_READ_NOT_STARTED where the load can't start, and leaves
 * save_data as it was. it returns SAVE_READ_BAD_CHECKSUM where the sum of
 * the decrypted bytes isn't checksum, and SAVE_READ_OK otherwise.
 *
 * The port validates a temporary record and preserves save_data and the
 * derived game state on every failure. SAVE_READ_NOT_STARTED identifies a
 * missing file, SAVE_READ_FAILED a device or allocation failure, and
 * SAVE_READ_BAD_FORMAT an invalid outer image or record length.
 *
 * In the original, a load that fails leaves save_data's old bytes, and the
 * decrypt garbles them. so does an image that buAnalyzeBackupFileImage finds
 * illegal. the result is then SAVE_READ_BAD_CHECKSUM, unless the garbled bytes'
 * sum happens to match.
 *
 * after SAVE_READ_OK, the game takes from save_data each part that keep
 * doesn't name. game_state.emblems takes save_data.emblems too while
 * game_state.winner_posed is 0, whatever keep says */
int save_read(int keep);
#define SAVE_READ_NOT_STARTED 0
#define SAVE_READ_OK 1
#define SAVE_READ_BAD_CHECKSUM 2
#ifdef NONMATCHING
/* Only an absent file may be used as a new slot. Failed reads preserve the
 * current save and all derived fields. */
#define SAVE_READ_FAILED 3
#define SAVE_READ_BAD_FORMAT 4
#endif
/* save_read's keep: each bit names a part of the game that a good read
 * leaves as it is. SAVE_KEEP_STATE is game_state's languages, save_date,
 * vibration, lives and theme */
#define SAVE_KEEP_STATE 1
#define SAVE_KEEP_FLAGS 2 /* game_flags */
#define SAVE_KEEP_BITS 4 /* game_bits */
/* nonzero while the main file's drive has a command queued, and 0 without
 * a drive */
int save_busy(void);
/* set save_place's drive: below save/card.h's CARD_DRIVES, SAVE_DRIVE_NONE
 * or SAVE_DRIVE_REFUSED */
void save_set_drive(int drive);
/* name the main file for slot: SONIC2___S and the slot in two digits */
void save_set_slot(int slot);
/* delete the main file, and return buDeleteFile's result */
int save_delete(void);
/* whether save_write can save the main file now. it returns SAVE_OK, or
 * one of the results below, which says why not.
 *
 * without a drive, it returns SAVE_OK. on a drive that isn't ready, it
 * returns SAVE_NOT_READY. otherwise it reads the file with save_read under
 * every SAVE_KEEP bit. it puts save_data back before it returns, and
 * leaves save_keep's copy holding the same bytes.
 *
 * where the read can't start, the file must be made anew. save_check then
 * returns SAVE_NOT_OURS while create is 0, and SAVE_NO_ROOM where the drive
 * has fewer than SAVE_BLOCKS free. where the checksum is wrong, it returns
 * SAVE_BAD_CHECKSUM. it returns SAVE_NOT_OURS too where save_data's date,
 * the file's after a good read, isn't game_state.save_date.
 *
 * a good read can change game_state.emblems, as save_read says, and that
 * change stays */
int save_check(int create);
/* save_check's results */
#define SAVE_OK 0
#define SAVE_NOT_READY 5
#define SAVE_NO_ROOM 6
#define SAVE_NOT_OURS 7
#define SAVE_BAD_CHECKSUM 9

/* the record's score, from the two parts it is kept in */
Sint32 save_record_score(const struct save_record *record);

/* rank a time among the best of kart_records[game_state.kart_course], with
 * stage_character(player) as its character. The port skips a course outside
 * [0, SAVE_KART_RECORDS). SHC retains the original incomplete bounds check. */
void save_record_kart_time(int player, int minute, int second, int hundredths);
/* rank the stage just played in the current mission's records. score,
 * game_state.rings[0] and game_state's clock each rank apart. The port ranks
 * all three stored score bytes and caps new scores at 0xffffff. Existing VMU
 * records are read as stored and require no migration.
 *
 * STAGE_VERSUS_KNUCKLES and the relay's stages record in STAGE_RELAY_LAST's
 * records. STAGE_CART_1 records in cart_stage's record for
 * stage_character(0). the mission's rank rises to game_state.rank + 1,
 * except on STAGE_GREEN_HILL.
 *
 * save_award then runs, and save_data keeps the stage and
 * stage_character(0). it returns 1, or 0 with nothing changed for a stage
 * without records */
int save_record_stage(Sint32 score);
/* rank a time among boss_totals[which]'s best, and set the rank of its first
 * mission to 1 */
void save_record_boss_total(int which, int minute, int second, int hundredths);
/* give each award save_data has earned, and count its emblems. while
 * game_state.unlock_pending isn't 0, it does nothing. otherwise it sets
 * unlock_id to 0, and each award sets unlock_id to its number and
 * unlock_pending to 1. where two fall in one call, only the last is named */
void save_award(void);

/* wait while the main file's drive is busy, then keep a copy of save_data.
 * save_restore puts the copy back */
void save_keep(void);
void save_restore(void);

/* count a call in play_time, and in emblem_time while emblems is below
 * SAVE_EMBLEMS. each stops at SAVE_TIME_MAX */
void save_count_time(void);
#define SAVE_TIME_MAX 0x7fffff00

/* make save_data a new save, dated by the clock. it holds zeros but for the
 * date, the console's individual ID and the new best times. game_flags and
 * game_bits clear, but for the heard bits of the music a new save starts
 * with.
 *
 * game_state.save_date takes the date, and game_state.emblems takes 0.
 * play_reset starts game_state's play over. it writes snapshot_current's
 * copies too, so snapshots_start must have run, unchecked */
void save_clear(void);

/* note the device type in each drive's socket. where one has changed since
 * the last call, it waits until every drive is idle and returns 1. it
 * returns 0 otherwise */
Sint32 save_check_sockets(void);

#define DOWNLOAD_TEXTS 5 /* a download file's texts */
#define DOWNLOAD_LINES 5 /* a text's lines */
#define DOWNLOAD_VALUES 8 /* a download file's values */
#define DOWNLOAD_LINE_SIZE 128 /* the bytes of a line that an entry keeps */

/* a download file's text, a pointer to each line */
struct download_text {
    char *lines[DOWNLOAD_LINES];
};

/* a download file: its kind, its texts, its values and its own data. the
 * code follows texts and data as the file holds them, without relocating
 * them */
struct download_file {
    Sint32 kind;
    struct download_text *texts[DOWNLOAD_TEXTS];
    Sint32 values[DOWNLOAD_VALUES];
    void *data;
};

/* the lines of a download file's text, copied */
/* @hack type: five named arrays, where one array of DOWNLOAD_LINES lines
 * would do */
struct download_text_copy {
    char line0[DOWNLOAD_LINE_SIZE];
    char line1[DOWNLOAD_LINE_SIZE];
    char line2[DOWNLOAD_LINE_SIZE];
    char line3[DOWNLOAD_LINE_SIZE];
    char line4[DOWNLOAD_LINE_SIZE];
};

/* a file's name, without its terminator: SONIC2___, a letter and two
 * digits. a download entry keeps it in DOWNLOAD_NAME_SIZE bytes */
#define SAVE_NAME_LENGTH 12
#define DOWNLOAD_NAME_SIZE 16
/* a download file save_downloads_find found: its drive, slot and name, and a
 * copy of its kind, values and texts */
struct download_entry {
    Sint32 drive;
    Sint32 slot;
    char name[DOWNLOAD_NAME_SIZE];
    Sint32 kind;
    Sint32 values[DOWNLOAD_VALUES];
    struct download_text_copy texts[DOWNLOAD_TEXTS];
};

/* the entry of the download file chosen to race. ADVERTISE.PRS's download and
 * kart race menus copy it here before the race, whose kart_course is
 * SAVE_KART_RECORDS. CART's code loads the file through it, and keeps its best
 * time in download_best_times at its slot */
extern struct download_entry download_chosen;

/* start loading the download file save_download_set_slot named, from the
 * drive save_download_set_drive gave, into buffer. it returns 1 where the
 * load starts, and 0 where it can't. buffer must hold DOWNLOAD_SIZE bytes,
 * and stay until the drive is idle. it then holds up to DOWNLOAD_SIZE bytes
 * of the file's save data. the data reaches it only where the load succeeds
 * and its image is legal. buffer is otherwise left as it was */
int save_download_read(void *buffer);
#define DOWNLOAD_SIZE 0x20000
void save_download_set_drive(int drive);
/* name the download file for slot: SONIC2___D and the slot in two digits */
void save_download_set_slot(int slot);
/* find the download files of the kinds that kinds asks for, on every
 * drive. it copies each file it finds into the next of entries, and returns
 * how many entries it filled. a slot that two drives hold gives the lower
 * drive's file.
 *
 * it loads each file to DEMO_AREA. a file whose load fails or whose image is
 * illegal never reaches DEMO_AREA. The port skips such files, and checks
 * each text table and line against the successfully loaded payload.
 *
 * entries must hold DOWNLOAD_SLOTS. each entry's name[SAVE_NAME_LENGTH] must
 * be 0, since the copy ends no name */
int save_downloads_find(struct download_entry *entries, Uint32 kinds);
/* save_downloads_find's kinds, as bits. DOWNLOAD_FIND_ALL takes a file of
 * any kind. each other bit takes the kinds its name gives */
#define DOWNLOAD_FIND_ALL 0x1
#define DOWNLOAD_FIND_KIND_1 0x2
#define DOWNLOAD_FIND_KIND_2 0x4
#define DOWNLOAD_FIND_KINDS_3_4 0x8
#define DOWNLOAD_FIND_KINDS_1_TO_4                                             \
    (DOWNLOAD_FIND_KIND_1 | DOWNLOAD_FIND_KIND_2 | DOWNLOAD_FIND_KINDS_3_4)
/* the values of a download file's kind. ADVERTISE.PRS's download menu
 * plays only DOWNLOAD_KIND_3 and DOWNLOAD_KIND_4, each a kart course */
#define DOWNLOAD_KIND_1 1
#define DOWNLOAD_KIND_2 2
#define DOWNLOAD_KIND_3 3
#define DOWNLOAD_KIND_4 4
/* load entry's file into file, up to DOWNLOAD_SIZE bytes, and wait for the
 * drive. it returns file->data, the pointer the loaded file holds. it returns
 * null where the load can't start, or where the drive reports that it failed.
 * The port rejects an invalid image or a data pointer outside the physical
 * payload and returns null. */
#ifdef NONMATCHING
/* Return the validated data pointer and the number of remaining bytes in
 * the loaded payload. Failure returns null and sets *size to zero. */
void *save_download_load_sized(struct download_entry *entry,
                               struct download_file *file, Uint32 *size);
#endif
void *save_download_load(struct download_entry *entry,
                         struct download_file *file);

#endif
