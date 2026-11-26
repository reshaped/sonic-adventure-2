/* the score, each player's rings and lives, and the stage's clock, with the
 * hud that draws them. the bonuses that show at the left of the screen. the
 * stage's time limit. the screens that end a stage: the results, a
 * two-player game's winner and the bosses' totals */
#ifndef HUD_HUD_H
#define HUD_HUD_H
#include <ninja.h>
#include <sg_xpt.h>

/* the part of a texture that a HUD_NUMBER_HOOK cell draws, as
 * stage_hooks.number_cell gives it. u0 and v0 give the part's top left
 * corner, and u1 and v1 its bottom right */
struct hud_number_cell {
    Float u0;
    Float v0;
    Float u1;
    Float v1;
    NJS_TEXLIST *texlist;
};

/* a row of decimal digits for hud_number_draw, which draws value with
 * console_texlist's textures no_00 to no_09. a digit is 32 pixels square at
 * a scale of 1 */
struct hud_number {
    Uint8 flags;
    Uint8 place;
    Sint16 unk_02;
    Uint32 max;
    Uint32 value;
    NJS_POINT3 pos; /* in pixels. place says where it falls on the row */
    Float scale;
    Uint32 color; /* 0xAARRGGBB */
};

/* hud_number.flags' bits. without HUD_NUMBER_SCALED, the row draws at a
 * scale of 1 */
#define HUD_NUMBER_SCALED 0x01
/* value is held to max, and the row has as many digits as max. without this
 * bit, the row has as many digits as value */
#define HUD_NUMBER_CLAMPED 0x08
/* the row is two cells in place of value's digits. each cell draws the part
 * of a texture that its own call of stage_hooks.number_cell gives */
#define HUD_NUMBER_HOOK 0x10
#define HUD_NUMBER_COLONS 0x20 /* each digit draws as the texture no_colon */
/* the zeros left of value's highest digit draw too. without this bit, they
 * are left blank */
#define HUD_NUMBER_ZEROS 0x40
/* the row draws at a priority of 1.0001. without this bit, it draws at
 * 0.90909 less its HUD_NUMBER_DEPTH */
#define HUD_NUMBER_FRONT 0x80

/* hud_number.place's bits. HUD_NUMBER_DEPTH holds a depth from 0 to 7, each
 * step 0.00001 of priority further back */
#define HUD_NUMBER_DEPTH 0x07
/* HUD_NUMBER_TOP puts pos.y at the row's top edge, and HUD_NUMBER_BOTTOM at
 * its bottom edge. with neither, pos.y is at its middle. HUD_NUMBER_TOP wins
 * where both are set */
#define HUD_NUMBER_TOP 0x10
#define HUD_NUMBER_BOTTOM 0x20
/* HUD_NUMBER_LEFT puts pos.x at the row's left edge, and HUD_NUMBER_RIGHT two
 * digits' widths left of its right edge. with neither, pos.x is half a digit
 * left of its middle. HUD_NUMBER_LEFT wins where both are set */
#define HUD_NUMBER_RIGHT 0x40
#define HUD_NUMBER_LEFT 0x80

/* the lives play_reset and lives_reset give each player */
#define LIVES_START 4

/* a window framed in corners and edges, which opens to a rectangle. the
 * results' score windows and the mission message take one. its task's extra
 * points to it. corners and targets run left top, left bottom, right top and
 * right bottom, in pixels */
struct frame_window {
    Sint8 mode; /* an enum window_mode */
    Sint32 count; /* the steps WINDOW_OPEN has taken */
    NJS_POINT3 corners[4];
    NJS_POINT3 targets[4]; /* where the corners stop */
};

/* a window's modes. no mode has the number 2 or 3 */
enum window_mode {
    /* each step moves the corners' x toward the targets' by
     * WINDOW_WIDEN_STEP. once all four arrive, WINDOW_HEIGHTEN moves their y
     * by WINDOW_HEIGHTEN_STEP */
    WINDOW_WIDEN = 0,
    WINDOW_HEIGHTEN = 1,
    /* each step moves every corner 1 / (WINDOW_OPEN_STEPS - count) of its
     * way to its target. the last step lands it there */
    WINDOW_OPEN = 4,
    WINDOW_OPENED = 5
};
#define WINDOW_OPEN_STEPS 10
#define WINDOW_WIDEN_STEP 10.0F
#define WINDOW_HEIGHTEN_STEP 8.0F
/* the width of a window's corners and edges, in pixels */
#define WINDOW_BORDER 8.0F

/* a time on the stage's clock, in whole seconds */
struct stage_time {
    Sint8 minute;
    Sint8 second;
};

/* a stage and its time limit in MISSION_TIMED */
struct time_limit {
    Sint8 stage;
    struct stage_time limit;
};

/* the stages' time limits in MISSION_TIMED, which time_limit_start and the
 * mission message read. end holds STAGE_NONE, which stops a walk of the
 * stages. STAGE_CART_1's limits are cart_time_limits */
#define TIME_LIMIT_STAGES 30
struct time_limit_table {
    struct time_limit stages[TIME_LIMIT_STAGES];
    Sint8 end;
};
extern struct time_limit_table time_limits;
/* STAGE_CART_1's time limits in MISSION_TIMED, by driver: Tails, in his mech
 * or not, and Rouge */
#define CART_LIMIT_TAILS 0
#define CART_LIMIT_ROUGE 1
#define CART_LIMITS 2
extern struct stage_time cart_time_limits[CART_LIMITS];

/* load or release the texture list of kind's life icon, kind being a player
 * record's kind */
void life_icon_load(int kind);
void life_icon_release(int kind);

/* the calls below keep snapshot_current in step with game_state. each that
 * writes a field snapshot_current copies writes the copy too, so
 * snapshots_start must have run, unchecked. lives_add, ring_add, ring_count
 * and lives_count read player as player & 1 */

/* start play over. each player's lives go to LIVES_START, and rings[0] goes
 * to 50 on STAGE_BOSS_LAST2. every other count goes to 0: the clock, the
 * score, the kept counts, game_state.steps, ring_total and the rings */
void play_reset(void);

/* the kept counts are game_state's kept_ fields. they hold the clock, the
 * score, player 0's rings and ring_total as the stage's play began. a clear
 * call sets its kept count to 0, and a keep call copies its count into it */
void kept_score_clear(void);
void kept_rings_clear(void);
void kept_clock_clear(void);
void kept_ring_total_clear(void);
void stage_clock_keep(void);
void rings_keep(void);
void score_keep(void);
void ring_total_keep(void);
/* a restore call sets its count from its kept count. stage_clock_restore
 * also sets game_state.steps to 0. rings_restore sets rings[0] to 50 on
 * STAGE_BOSS_LAST2 instead, and sets rings[1] to 0 */
void score_restore(void);
void rings_restore(void);
void stage_clock_restore(void);
void ring_total_restore(void);
/* set each player's lives to LIVES_START */
void lives_reset(void);

/* add count to player's lives, with the sound of a life where count is
 * above 0. a sum above 99 becomes 99, and so does a sum below 0 where count
 * is above 0. a count below 0 can leave the lives below 0 */
void lives_add(int player, Sint16 count);
/* add count to player's rings, which stay from 0 to 999. a count below 0
 * takes rings away. player gains a life by lives_add for each multiple of
 * 100 the rings climb to or past.
 *
 * a two-player game has three ring levels. they lie 20, 40 and 60 rings above
 * the last multiple of 60 the old count reached. where the rings climb to or
 * past a level, the call sets player's byte of ring_levels for the highest they
 * reach. it clears the other two. where player's slot holds a record, a new
 * level also sets player's ring_level_timer to 60. it then plays the
 * character's voice, where the character has one */
void ring_add(int player, Sint16 count);
/* add points to the score, or twice the points while player 0's record has
 * PLAYER_INVINCIBLE. a sum above 99999999 becomes 99999999. the sum is
 * unsigned, so a sum below 0 becomes 99999999 too */
void score_add(int points);
/* show score as a bonus at the left of the screen. a label that grades score
 * shows first, and then the points. a score under 100 takes no label. the
 * points join the score as the bonus's task ends, undoubled, to no more than
 * 99999999. a two-player game shows and adds none, nor does a call where
 * task_create fails. score must fit a Sint16, unchecked */
void technique_score_start(int score);
/* add score as score_add does, and count an enemy toward a chain. a chain is
 * the enemies player 0 defeats between leaving the ground and landing. the
 * landing ends the chain, which shows as a bonus through
 * technique_score_start. a chain of two gives 100 points, and one of eleven
 * or more gives 2000. a chain's task destroyed outright, not killed by its
 * own step, shows no bonus.
 *
 * Sonic and Shadow count on the ground too, in a mode from
 * PLAYER_MODE_SONIC_LIGHT_ATTACK to PLAYER_MODE_SONIC_BOUNCE. such a mode
 * doesn't end a chain. nothing counts in a two-player game, for a mech, or
 * while enemy_hit_hook is set. a chain ends at its next step once the hook is
 * set. player 0's work must exist, unchecked */
void score_add_enemy(int score);
Sint16 ring_count(int player);
/* ring_count, declared to return an int. it is a second name at
 * ring_count's address, for callers that read the result as an int */
#ifdef NONMATCHING
/* ring_count's result as an int: this side's objects don't define
 * ring_count_int, which only symbols.txt places */
#define ring_count_int(player) ((int)ring_count(player))
#else
int ring_count_int(int player);
#endif
Sint16 lives_count(int player);

/* the stage's clock is game_state's minute, second and frame, a frame being
 * a 60th of a second. stage_clock_stop stands the clock still by setting
 * game_state.clock_stopped to 1, and stage_clock_resume sets it back to 0 */
void stage_clock_resume(void);
void stage_clock_stop(void);
/* count a step of the stage in game_state.steps. the call then runs the
 * clock a frame on, to no more than 99:59 and 59 frames. it counts each
 * ring_level_timer above 0 down by 1. while game_state.time_stopped isn't 0,
 * the call does nothing. while clock_stopped or mini_event_running is 1, it
 * counts the step alone */
void stage_clock_step(void);
/* copy the clock to each of the three pointers that isn't null */
void stage_clock_get(Sint8 *minute, Sint8 *second, Sint8 *frame);
/* set the clock. a part below 0 stays as it is */
void stage_clock_set(Sint8 minute, Sint8 second, Sint8 frame);

/* draw the hud. a one-player game shows the score, the clock, and player 0's
 * rings and lives. it shows the Chao key once it is taken. a two-player game
 * shows both players' rings, and their lamps, lit for the rounds each has won.
 * the rings show only while game_state.rings_shown is 1, and the lives only
 * while lives_shown is 1. nothing shows while hud_on is 0. a stage above
 * STAGE_STG_END, or one of STAGE_VERSUS, shows no score or Chao key. its clock
 * and rings stand higher. player 0's record must exist while its lives show,
 * unchecked */
void hud_draw(void);
/* draw player's rings and the ring beside them. they stand y pixels below
 * where hud_draw puts them on a stage with no score. they stand at the left
 * of the screen, whatever player is. a count of 0 draws in 0xffff0000 | pulse,
 * red whitened by pulse's green and blue */
void hud_draw_rings_at(int player, Uint32 pulse, Float y);
/* draw kind's life icon and player's lives where hud_draw draws player's.
 * unlike hud_draw, the call draws Big's icon in one part. the lives then stand
 * where they do beside any other icon of one part */
void hud_draw_lives_of(int player, int kind);
/* draw number as struct hud_number says. the call leaves the texture filter
 * at NJD_TEXTUREFILTER_BILINEAR, and njSetTexture's list at
 * console_texlist, or at the list stage_hooks.number_cell gave */
void hud_number_draw(struct hud_number *number);

/* start the results of the stage player has cleared. the music stops, the
 * fade turns white, and game_state.idle_dim goes to 0. where task_create or
 * syCalloc fails, no results start. each result ends play by stage_end.
 *
 * a two-player game shows the winner. where the match is over, or
 * game_state.two_players is TWO_PLAYERS_1_POINT, the players then choose
 * between leaving and another match.
 *
 * a one-player game's results total the score and a time bonus in
 * MISSION_FIRST, MISSION_TIMED and MISSION_HD. they total the time in
 * MISSION_RINGS and MISSION_CHAO, and in every mission of STAGE_CART_1. a
 * total records the stage by save_record_stage, and saves where save_check
 * allows. it ends play once the memory cards are idle. its task destroyed
 * other than while it waits on the cards sets idle_dim back to 1. player's
 * slot must hold a record as a total's rank shows, unchecked, but on
 * STAGE_CART_1.
 *
 * the STAGE_VERSUS stages, and the stages from STAGE_STG_END on but
 * STAGE_CART_1, total nothing and draw nothing. their results save only in
 * SEQUENCE_HERO, SEQUENCE_DARK and SEQUENCE_LAST. they end play once 360 steps
 * have passed, the bosses' totals no longer show and the memory cards are idle
 */
void results_start(int player);
/* start the stage's time limit, which shows the time left and ends play as
 * the clock reaches it. MISSION_TIMED takes the stage's limit from
 * time_limits, and the relay's stages take STAGE_RELAY_FIRST's. a stage
 * time_limits lacks takes 10:00. MISSION_FIRST gives STAGE_GREEN_FOREST 8:00,
 * STAGE_WHITE_JUNGLE 10:00 and STAGE_SECURITY_HALL 5:00, while
 * game_state.debug is 0. a two-player game, STAGE_CART_1 and every other
 * stage and mission have no limit.
 *
 * a limit reached gives player 0's record PLAYER_MISSED and ends play with
 * STAGE_END_RETRY. where player 0 has no lives left, it ends play with
 * STAGE_END_MISS. a retry during a hunt also finishes hunt_current and clears
 * restart point 0. player 0's record must exist as the limit is reached,
 * unchecked. a stage cleared stops the limit */
void time_limit_start(void);

/* the bosses' times: 18 of them, in three runs of 8, 8 and 2. the boss
 * attack's courses play system/sequence.h's SEQUENCE_HERO_BOSSES,
 * SEQUENCE_DARK_BOSSES and SEQUENCE_ALL_BOSSES, and boss_total_start totals
 * their times. BOSS_COURSE_HERO and BOSS_COURSE_DARK play the first run and
 * the second, and BOSS_COURSE_ALL all three.
 * boss_times_clear empties the times for course. BOSS_COURSE_DARK then
 * enters its first time as the ninth, at the start of the second run.
 *
 * boss_time_record enters the stage's clock as the next time, with a label
 * for the stage's boss. it does nothing once the times are full. a stage
 * without a label of its own leaves the time's label as it was */
#define BOSS_COURSE_HERO 0
#define BOSS_COURSE_DARK 1
#define BOSS_COURSE_ALL 2
void boss_time_record(void);
void boss_times_clear(int course);
/* total the bosses' times of the running sequence, a boss attack course's.
 * the call sums each run, and sums the three runs' sums. it records the sum
 * of the course's own run by save_record_boss_total. BOSS_COURSE_ALL records
 * the sum of all three. the call gives the course's award, from
 * SAVE_AWARD_FIRST_BOSS on, and saves where save_check allows. another
 * sequence records the first run's sum and gives no award. where task_create
 * fails, the call records and gives nothing.
 *
 * the task named execTotalBossScore then shows the course's run, or for
 * BOSS_COURSE_ALL the three runs' sums. the sum of all three shows last */
void boss_total_start(void);

#endif
