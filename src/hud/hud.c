/* the score, each player's rings and lives, and the stage's clock, with the
 * hud that draws them. the bonuses that show at the left of the screen. the
 * stage's time limit. the screens that end a stage: the results, a
 * two-player game's winner and the bosses' totals */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "draw/fade.h"
#include "draw/screen.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "hud/texture_lists.h"
#include "hud/hud/text.inc"
#include "player/player.h"
#include "save/save.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/hunt.h"
#include "stage/stage.h"
#include "system/flags.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/sequence.h"
#include "system/snapshot.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "system/vector.h"
#include "vmu/vibration.h"

#ifdef NONMATCHING
/* port_wide_anchor, the pivot this side's one-player counts squeeze to */
#include "engine/dc/port.h"
#endif

#define SCORE_MAX 99999999
#define RINGS_MAX 999
#define LIVES_MAX 99
/* a life for each multiple of RINGS_PER_LIFE the rings climb to */
#define RINGS_PER_LIFE 100
/* the rings player 0 has on STAGE_BOSS_LAST2, whatever was kept */
#define BOSS_LAST2_RINGS 50
/* the rings past a multiple of LEVEL_2_RINGS that earn each ring level in a
 * two-player game */
#define LEVEL_0_RINGS 20
#define LEVEL_1_RINGS 40
#define LEVEL_2_RINGS 60
/* what a new ring level sets its player's ring_level_timer to */
#define RING_LEVEL_FRAMES 60
/* the voices of a new ring level, by a player record's kind */
#define VOICE_LEVEL_SONIC 1519
#define VOICE_LEVEL_SHADOW 1518
#define VOICE_LEVEL_EGGMAN_MECH 1632
#define VOICE_LEVEL_TAILS_MECH 1633
#define VOICE_LEVEL_KNUCKLES 1244
#define VOICE_LEVEL_ROUGE 1245
#define VOICE_LEVEL_AMY_JAPANESE 2671
#define VOICE_LEVEL_AMY_ENGLISH 2677
/* lives_show_step's and rings_hide_step's tasks set their fields for
 * MOMENTARY_LAST + 1 steps. each ends at the next */
#define MOMENTARY_LAST 180

/* the stage's clock: seconds of a minute. a clock that reaches CLOCK_MINUTES
 * stops at CLOCK_LAST_MINUTE, CLOCK_LAST_SECOND and CLOCK_LAST_FRAME. a time
 * in hundredths stops at CLOCK_LAST_HUNDREDTH */
#define SECONDS_PER_MINUTE 60
#define HUNDREDTHS_PER_SECOND 100
#define CLOCK_MINUTES 100
#define CLOCK_LAST_MINUTE 99
#define CLOCK_LAST_SECOND 59
#define CLOCK_LAST_FRAME 59
#define CLOCK_LAST_HUNDREDTH 99

/* the task levels of the score windows, and of the results, the winner and
 * the bosses' totals. the bonuses, the chain and the time limit run at
 * HUD_LEVEL */
#define WINDOW_LEVEL 2
#define RESULT_LEVEL 0
#define HUD_LEVEL 3

#define SOUND_LIFE 0x1008
#define SOUND_TALLY 0x8004 /* the bonus counting into the total */
#define SOUND_SHOW 0x801c /* a line of the results showing */
#define SOUND_RANK 0x801d

/* a bonus shows its label until its timer passes TECHNIQUE_LABEL_STEPS, and
 * then its points. it ends once the timer passes TECHNIQUE_STEPS, or once the
 * stage is cleared. the timer counts the bonus's steps */
#define TECHNIQUE_LABEL_STEPS 60
#define TECHNIQUE_STEPS 120
/* a bonus's left edge stands TECHNIQUE_LEFT pixels in from the screen's.
 * each unit of its slide moves it TECHNIQUE_SLIDE_WIDTH pixels further right.
 * the slide starts at TECHNIQUE_SLIDE_START and falls by TECHNIQUE_SLIDE_STEP
 * a draw, down to 0 */
#define TECHNIQUE_LEFT 16.0F
#define TECHNIQUE_SLIDE_WIDTH 16.0F
#define TECHNIQUE_SLIDE_START 40
#define TECHNIQUE_SLIDE_STEP 2
/* the labels a bonus can take. label 0 is for the fewest points, and the top
 * label, TECHNIQUE_LABELS - 1, for the most */
#define TECHNIQUE_LABELS 11
/* the top of a bonus's first row, and each row's height, in pixels. the
 * points stand TECHNIQUE_POINTS_DROP pixels lower in their row than a label */
#define TECHNIQUE_TOP 112
#define TECHNIQUE_ROW 24
#define TECHNIQUE_POINTS_DROP 2.0F

/* the priority the hud's sprites draw at */
#define SPRITE_PRIORITY (-2.0F)
/* the clock's y, below the score or where the score is missing */
#define CLOCK_Y 52.0F
#define CLOCK_Y_SCORELESS 36.0F
/* the life icon and the lamps center on BOTTOM_ROW_Y. each player's icon and
 * lives stand in its half of the screen, PLAYER_HALF_WIDTH wide. the icon
 * centers LIFE_ICON_X in from the half's left edge. the lives stand LIVES_X
 * in, or LIVES_X_TWO_PARTS beside an icon of two parts */
#define BOTTOM_ROW_Y 424.0F
#define PLAYER_HALF_WIDTH (SCREEN_WIDTH / 2.0F)
#define LIFE_ICON_X 32.0F
#define LIVES_X 64.0F
#define LIVES_X_TWO_PARTS 72.0F
/* a number's colors as ARGB: white, and the red of a count at 0 */
#define NUMBER_WHITE 0xffffffff
#define NUMBER_RED 0xffff0000
/* the flags of most of the hud's numbers. NUMBER_ZEROED shows each of max's
 * digits, zeros and all. NUMBER_COLON shows the colons between a time's
 * parts. the _FRONT flags draw either in front of the result sheet */
#define NUMBER_ZEROED                                                          \
    (HUD_NUMBER_SCALED | HUD_NUMBER_CLAMPED | HUD_NUMBER_ZEROS)
#define NUMBER_COLON (HUD_NUMBER_SCALED | HUD_NUMBER_COLONS)
#define NUMBER_ZEROED_FRONT (NUMBER_ZEROED | HUD_NUMBER_FRONT)
#define NUMBER_COLON_FRONT (NUMBER_COLON | HUD_NUMBER_FRONT)

/* the global indices of console_texlist's textures that the hud draws. a
 * digit is TEXTURE_DIGIT plus the digit, and the colon draws as digit
 * DIGIT_COLON */
#define TEXTURE_DIGIT 5000
#define DIGIT_COLON 10
#define TEXTURE_RING 5011
/* the global index of the texture a HUD_NUMBER_HOOK cell draws, in the list
 * stage_hooks.number_cell gives */
#define TEXTURE_HOOK 10000000
/* the global indices of a score window's corner, edge and middle, in
 * result_texlist */
#define TEXTURE_WINDOW_CORNER 100000001
#define TEXTURE_WINDOW_EDGE 100010000
#define TEXTURE_WINDOW_MIDDLE 100000003
/* the global index of the result sheet, in result_texlist */
#define TEXTURE_RESULT 11000010

/* a score window's priority, just behind the result sheet's 1.0001.
 * WINDOW_PRIORITY_BEHIND puts a window behind the hud's numbers, which draw
 * from 0.90909 back */
#define WINDOW_PRIORITY 1.00008F
#define WINDOW_PRIORITY_BEHIND 0.9F

/* a frame of the clock in hundredths of a second, as a time shows it */
#define HUNDREDTHS_PER_FRAME 1.6659999F

/* the buttons that hurry the results on, any of them pressed */
#define RESULT_HURRY_BUTTONS                                                   \
    (PDD_DGT_TA | PDD_DGT_TB | PDD_DGT_ST | PDD_DGT_TX | PDD_DGT_TY)

/* a clear's rank, from RANK_E up to RANK_A, which game_state.rank keeps.
 * RANK_RISES is the steps up from RANK_E, each for a rank score or rank time
 * a clear reaches */
#define RANK_E 0
#define RANK_A 4
#define RANKS (RANK_A + 1)
#define RANK_RISES (RANK_A - RANK_E)
/* the rank scores' unit, in points */
#define RANK_SCORE_UNIT 100
/* a clear's time bonus: TIME_BONUS, less TIME_BONUS_LOSS for each second of
 * the clock past TIME_BONUS_FREE. a bonus that would fall below 0 is 0 */
#define TIME_BONUS 10000
#define TIME_BONUS_FREE 60
#define TIME_BONUS_LOSS 20
/* the points of the time bonus that the tally moves into the total a step */
#define TALLY_STEP 40

/* the fade's alphas as the results start: white, and then half black */
#define FADE_WHITE 0xff
#define FADE_DIM 0x80
/* how many steps each mode of the results lasts. the winner poses
 * LONG_POSE_STEPS, as does the winner of a result without a total. a line's
 * number zooms in over LINE_STEPS, with SOUND_SHOW at LINE_SOUND_AT, as the
 * rank does over RANK_ZOOM_STEPS. the rank's voice plays RANK_VOICE_AT
 * steps into RANK_SHOWN_STEPS */
#define POSE_STEPS 180
#define LONG_POSE_STEPS 360
#define TITLE_STEPS 30
#define LINE_STEPS 20
#define LINE_SOUND_AT 6
#define ALL_RINGS_STEPS 10
#define BONUS_LABELS_STEPS 30
#define TALLY_WAIT 20 /* the steps before the tally starts */
#define COUNTED_STEPS 120
#define RANK_ZOOM_STEPS 30
#define RANK_SHOWN_STEPS 230
#define RANK_VOICE_AT 35
/* once a mode has run HURRY_AFTER steps, RESULT_HURRY_BUTTONS hurry it. they
 * end the tally at once, and add HURRY_SKIP to SCORE_RESULT_ALL_RINGS' count */
#define HURRY_AFTER 10
#define HURRY_SKIP 60
/* the power and the freq vibration_start takes as a line of the results
 * shows */
#define RUMBLE_POWER 3
#define RUMBLE_FREQ 7

/* the steps the winner screen waits for the players' choice before it
 * leaves. it waits WINNER_AGAIN_STEPS before another match */
#define WINNER_CHOOSE_STEPS 600
#define WINNER_AGAIN_STEPS 60

/* the limit, in minutes, of a MISSION_TIMED stage that time_limits lacks */
#define LIMIT_MINUTES 10
/* the limits, in minutes, that MISSION_FIRST gives three stages */
#define GREEN_FOREST_MINUTES 8
#define WHITE_JUNGLE_MINUTES 10
#define SECURITY_HALL_MINUTES 5

/* the bosses' times: BOSS_TIMES of them, in BOSS_TOTALS runs of BOSS_RUN,
 * BOSS_RUN and BOSS_LAST_RUN times */
#define BOSS_TIMES 18
#define BOSS_TOTALS 3
#define BOSS_RUN 8
#define BOSS_LAST_RUN 2
/* the z of the bosses' totals: their labels' priority */
#define BOSS_Z 2.0F
/* how many steps each mode of the bosses' totals lasts. a line shows each
 * BOSS_LINE_STEPS, from BOSS_WINDOWS_STEPS after the windows open */
#define BOSS_LOAD_STEPS 120
#define BOSS_WINDOWS_STEPS 60
#define BOSS_LINE_STEPS 30
#define BOSS_PAUSE_STEPS 60
#define BOSS_SHOWN_STEPS 1800
/* frames of boss_labels. BOSS_LABEL_TOTAL_1 labels the first run's sum, and
 * the next two the others. BOSS_LABEL_TOTAL labels the sum of all three */
#define BOSS_LABEL_TOTAL_1 17
#define BOSS_LABEL_TOTAL 20

/* the extra of a bonus's task. label picks the bonus's label and its points'
 * color. label 0 shows no label. once the label has shown, points_shown is set
 * and the points show in its place. the top label shows label 0's sprite in
 * place of its points */
struct technique_score {
    Sint8 label;
    Sint8 points_shown;
    Sint16 score; /* the points the task's end adds */
    Sint16 timer;
    Sint16 slide; /* how far the label still slides in */
};

/* a results screen's record, which its task's extra points to */
struct result {
    Sint8 mode;
    Sint8 player; /* the slot of the player who cleared the stage */
    Sint8 rank; /* from RANK_E to RANK_A */
    /* 1 where player 0's rings reach the stage's ring_total, which makes the
     * rank RANK_A */
    Sint8 all_rings;
    Sint32 count; /* the steps the mode has run */
    Uint32 time_bonus;
    Uint32 score; /* the total, which the time bonus counts into */
};

/* the modes of the results that total the score. the draw shows each part of
 * the sheet from its mode on. a number, and the rank, zoom in during their
 * own mode */
enum score_result_mode {
    SCORE_RESULT_FADE = 0, /* the screen fades to white */
    SCORE_RESULT_POSE = 1, /* the winner poses as the white clears */
    /* the screen dims to half black, and the rank is worked out */
    SCORE_RESULT_DIM = 2,
    SCORE_RESULT_TITLE = 3,
    SCORE_RESULT_LABELS = 4,
    SCORE_RESULT_SCORE = 5,
    SCORE_RESULT_TIME = 6,
    SCORE_RESULT_RINGS = 7,
    SCORE_RESULT_ALL_RINGS = 8,
    SCORE_RESULT_BONUS_LABELS = 9,
    SCORE_RESULT_BONUS = 10,
    SCORE_RESULT_TOTAL = 11,
    /* the bonus counts into the total, and the stage is recorded and saved */
    SCORE_RESULT_COUNT = 12,
    SCORE_RESULT_COUNTED = 13,
    SCORE_RESULT_RANK = 14,
    SCORE_RESULT_RANK_SHOWN = 15,
    /* wait for the memory cards, then end play. the results without a total
     * end from this mode too */
    SCORE_RESULT_END = 16,
    SCORE_RESULT_ENDED = 17
};

/* the modes of the results that total the time, as enum score_result_mode's */
enum time_result_mode {
    TIME_RESULT_FADE = 0,
    TIME_RESULT_POSE = 1,
    TIME_RESULT_DIM = 2,
    TIME_RESULT_TITLE = 3,
    TIME_RESULT_LABEL = 4,
    TIME_RESULT_TIME = 5,
    TIME_RESULT_RANK = 6,
    /* the rank's voice plays, and the stage is recorded and saved */
    TIME_RESULT_RANK_SHOWN = 7,
    TIME_RESULT_END = 8,
    TIME_RESULT_ENDED = 9
};

/* the record of a two-player game's winner screen, which its task's extra
 * points to */
struct winner {
    Sint8 mode; /* an enum winner_mode */
    Sint8 player;
    /* WINNER_LEAVE or WINNER_AGAIN, the choice the pads move between */
    Sint8 choice;
    Sint32 count;
};

enum winner_mode {
    WINNER_FADE = 0,
    WINNER_POSE = 1,
    /* where the match is over, or game_state.two_players is
     * TWO_PLAYERS_1_POINT, the players have WINNER_CHOOSE_STEPS to choose */
    WINNER_CHOOSE = 2,
    WINNER_END = 3,
    WINNER_PLAY_AGAIN = 4
};

#define WINNER_LEAVE 0
#define WINNER_AGAIN 1

/* the time limit, in the extra of the task named
 * CountDownTimerForGameOver */
struct countdown {
    Sint8 minute;
    Sint8 second;
};

/* a boss's time, or a sum of times, and the frame of boss_labels it shows
 * by */
struct boss_time {
    Sint32 label;
    Sint32 minute;
    Sint32 second;
    Sint32 hundredths;
};

/* the bosses' times, as boss_time_record entered them. totals holds each
 * run's sum, and total the sum of all three */
struct boss_record {
    Sint32 unk_00;
    /* how many of its course's lines the task named execTotalBossScore
     * shows */
    Sint32 shown;
    Sint32 total_shown; /* below 0 until the task shows total */
    Sint32 timer; /* the steps left in the task's mode */
    Sint32 count; /* where boss_time_record enters the next time */
    struct boss_time times[BOSS_TIMES];
    struct boss_time totals[BOSS_TOTALS];
    struct boss_time total;
};

/* the modes of the task named execTotalBossScore, in its work's mode */
enum boss_total_mode {
    BOSS_TOTAL_LOAD = 0,
    BOSS_TOTAL_WAIT = 1,
    BOSS_TOTAL_WINDOWS = 2, /* the course's two windows open */
    BOSS_TOTAL_LINES = 3, /* the course's lines show, one at a time */
    BOSS_TOTAL_PAUSE = 4,
    /* boss_record.total shows until the timer runs out or player 0 presses a
     * choose button */
    BOSS_TOTAL_SHOWN = 5,
    /* boss_total_showing is FALSE, so the results without a total can end */
    BOSS_TOTAL_DONE = 6
};

/* the rectangle a score window opens to, by its edges */
struct window_rect {
    Float right;
    Float left;
    Float bottom;
    Float top;
};

/* a course's view: its two windows, and how many lines it shows above the
 * total */
struct boss_layout {
    struct window_rect windows[2];
    Sint32 times;
};

/* a stage and its rank scores, in RANK_SCORE_UNIT points. a clear's score
 * and time bonus raise its rank a step for each rank score they reach */
struct rank_scores {
    Sint16 stage;
    Sint16 scores[RANK_RISES];
};
#define RANK_SCORES_HALFWORDS (sizeof(struct rank_scores) / sizeof(Sint16))

/* a stage and its rank times. a clear's time raises its rank a step for
 * each rank time it is within */
struct rank_times {
    Sint8 stage;
    struct stage_time times[RANK_RISES];
};

/* nothing calls these, and no header declares them. they are external only
 * because SHC leaves out a static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the steps of two tasks that nothing starts. each counts its steps in the
 * Sint32 at its task's extra. lives_show_step sets game_state.lives_shown,
 * and rings_hide_step clears rings_shown */
void lives_show_step(struct task *task);
void rings_hide_step(struct task *task);
/* add to the clock, and to snapshot_current's copy. a part below 0 takes
 * time away, and nothing stops the minutes below 0. a sum from 100 minutes
 * on becomes 99:59 and 59 frames */
void stage_clock_add(Sint8 minute, Sint8 second, Sint8 frame);
/* NOLINTEND(misc-use-internal-linkage) */

static void technique_score_step(struct task *task);
static void technique_score_draw(struct task *task);
static void technique_score_destroy(struct task *task);
static void enemy_chain_step(struct task *task);
static void enemy_chain_destroy(struct task *task);
static void hud_draw_score(int scoreless);
static void hud_draw_clock(int scoreless);
static void time_draw(NJS_POINT3 *pos, int minute, int second, int hundredth);
static void hud_draw_rings(int player, Uint32 pulse, int row);
static void hud_draw_lives(int player);
static void hud_draw_round_wins(int player);
static void hud_draw_chao_key(void);
static void score_window_step(struct task *task);
static void score_window_draw(struct task *task);
static void score_window_destroy(struct task *task);
static struct task *score_window_open(Float right, Float left, Float bottom,
                                      Float top);
static void score_rank_set(struct result *result);
static void score_result_step(struct task *task);
static void score_result_draw(struct task *task);
static void score_result_destroy(struct task *task);
static void time_rank_set(struct result *result);
static void cart_rank_set(struct result *result);
static void time_result_step(struct task *task);
static void time_result_draw(struct task *task);
static void time_result_destroy(struct task *task);
static void plain_result_step(struct task *task);
static void plain_result_draw(struct task *task);
static void plain_result_destroy(struct task *task);
static void winner_step(struct task *task);
static void winner_draw(struct task *task);
static void winner_destroy(struct task *task);
static void countdown_draw(struct task *task);
static void countdown_step(struct task *task);
static struct task *countdown_start(Sint8 minute, Sint8 second);
static void boss_label_draw(int label, Float scale, NJS_POINT3 *pos, int attr);
static void boss_times_sum(struct boss_time *sum, int first, int count);
static void boss_totals_sum(void);
static void boss_total_step(struct task *task);
static void boss_total_draw(struct task *task);
static void boss_total_destroy(struct task *task);
static int playing_dark(void);

/* the stages' rank scores in MISSION_FIRST, MISSION_TIMED and MISSION_HD.
 * end holds STAGE_NONE, which stops a walk of the stages. every table holds
 * RANKED_STAGES stages but rank_times_rings, which holds the relay's four
 * other stages too */
#define RANKED_STAGES 30
#define RING_RANKED_STAGES (RANKED_STAGES + 4)
/* clang-format off */
static struct {
    struct rank_scores stages[RANKED_STAGES];
    Sint16 end;
} rank_scores_first = {
    {
        {STAGE_CITY_ESCAPE,     { 90, 110, 140, 180}},
        {STAGE_WILD_CANYON,     { 80, 100, 140, 160}},
        {STAGE_PRISON_LANE,     {130, 170, 230, 260}},
        {STAGE_METAL_HARBOR,    {140, 160, 180, 200}},
        {STAGE_GREEN_FOREST,    { 90, 120, 130, 150}},
        {STAGE_PUMPKIN_HILL,    { 60,  80, 100, 120}},
        {STAGE_MISSION_STREET,  {130, 170, 220, 250}},
        {STAGE_AQUATIC_MINE,    { 90, 100, 110, 140}},
        {STAGE_HIDDEN_BASE,     {100, 110, 120, 140}},
        {STAGE_PYRAMID_CAVE,    {100, 110, 130, 150}},
        {STAGE_DEATH_CHAMBER,   { 60,  80, 100, 120}},
        {STAGE_ETERNAL_ENGINE,  {200, 250, 300, 350}},
        {STAGE_METEOR_HERD,     { 80, 110, 120, 130}},
        {STAGE_CRAZY_GADGET,    {100, 120, 150, 170}},
        {STAGE_FINAL_RUSH,      { 90, 100, 110, 130}},
        {STAGE_IRON_GATE,       {140, 160, 180, 200}},
        {STAGE_DRY_LAGOON,      { 80, 100, 120, 150}},
        {STAGE_SAND_OCEAN,      {180, 200, 230, 250}},
        {STAGE_RADICAL_HIGHWAY, { 60,  80, 110, 140}},
        {STAGE_EGG_QUARTERS,    { 60,  80, 100, 120}},
        {STAGE_LOST_COLONY,     {220, 260, 300, 340}},
        {STAGE_WEAPONS_BED,     {200, 250, 280, 300}},
        {STAGE_SECURITY_HALL,   { 60,  80, 100, 120}},
        {STAGE_WHITE_JUNGLE,    { 80, 100, 120, 140}},
        {STAGE_SKY_RAIL,        { 90, 110, 130, 140}},
        {STAGE_MAD_SPACE,       { 80, 100, 120, 140}},
        {STAGE_COSMIC_WALL,     {150, 300, 450, 530}},
        {STAGE_FINAL_CHASE,     { 80, 100, 110, 120}},
        {STAGE_RELAY_LAST,      {200, 250, 270, 290}},
        {STAGE_GREEN_HILL,      { 80, 100, 120, 130}},
    },
    STAGE_NONE,
};
static struct {
    struct rank_scores stages[RANKED_STAGES];
    Sint16 end;
} rank_scores_timed = {
    {
        {STAGE_CITY_ESCAPE,     {120, 140, 160, 180}},
        {STAGE_WILD_CANYON,     { 80, 100, 140, 160}},
        {STAGE_PRISON_LANE,     {130, 170, 230, 260}},
        {STAGE_METAL_HARBOR,    {120, 140, 160, 180}},
        {STAGE_GREEN_FOREST,    { 70,  90, 100, 120}},
        {STAGE_PUMPKIN_HILL,    { 70,  90, 110, 130}},
        {STAGE_MISSION_STREET,  {150, 180, 200, 220}},
        {STAGE_AQUATIC_MINE,    { 80, 100, 120, 140}},
        {STAGE_HIDDEN_BASE,     { 80, 100, 110, 120}},
        {STAGE_PYRAMID_CAVE,    { 80,  90, 110, 130}},
        {STAGE_DEATH_CHAMBER,   { 60,  80, 100, 120}},
        {STAGE_ETERNAL_ENGINE,  {200, 250, 280, 300}},
        {STAGE_METEOR_HERD,     { 90, 110, 120, 130}},
        {STAGE_CRAZY_GADGET,    {120, 140, 150, 160}},
        {STAGE_FINAL_RUSH,      { 80, 100, 120, 130}},
        {STAGE_IRON_GATE,       {140, 160, 180, 200}},
        {STAGE_DRY_LAGOON,      { 80, 100, 120, 140}},
        {STAGE_SAND_OCEAN,      {180, 200, 230, 250}},
        {STAGE_RADICAL_HIGHWAY, { 80, 100, 120, 140}},
        {STAGE_EGG_QUARTERS,    { 80, 100, 120, 130}},
        {STAGE_LOST_COLONY,     {220, 260, 300, 340}},
        {STAGE_WEAPONS_BED,     {200, 250, 280, 300}},
        {STAGE_SECURITY_HALL,   { 80, 100, 120, 130}},
        {STAGE_WHITE_JUNGLE,    {100, 120, 130, 140}},
        {STAGE_SKY_RAIL,        { 80, 100, 120, 140}},
        {STAGE_MAD_SPACE,       { 80, 100, 120, 140}},
        {STAGE_COSMIC_WALL,     {300, 350, 400, 450}},
        {STAGE_FINAL_CHASE,     { 80,  90, 100, 110}},
        {STAGE_RELAY_LAST,      {200, 250, 270, 290}},
        {STAGE_GREEN_HILL,      { 20,  50, 100, 150}},
    },
    STAGE_NONE,
};
static struct {
    struct rank_scores stages[RANKED_STAGES];
    Sint16 end;
} rank_scores_hd = {
    {
        {STAGE_CITY_ESCAPE,     {100, 120, 150, 190}},
        {STAGE_WILD_CANYON,     { 80, 100, 150, 170}},
        {STAGE_PRISON_LANE,     {130, 170, 230, 260}},
        {STAGE_METAL_HARBOR,    {120, 140, 160, 180}},
        {STAGE_GREEN_FOREST,    { 80,  90, 100, 120}},
        {STAGE_PUMPKIN_HILL,    {100, 120, 140, 160}},
        {STAGE_MISSION_STREET,  {180, 220, 260, 300}},
        {STAGE_AQUATIC_MINE,    { 90, 100, 130, 150}},
        {STAGE_HIDDEN_BASE,     { 80,  90, 110, 130}},
        {STAGE_PYRAMID_CAVE,    { 90, 110, 130, 140}},
        {STAGE_DEATH_CHAMBER,   { 80, 100, 110, 120}},
        {STAGE_ETERNAL_ENGINE,  {200, 250, 300, 350}},
        {STAGE_METEOR_HERD,     { 70, 100, 110, 130}},
        {STAGE_CRAZY_GADGET,    { 90, 110, 130, 150}},
        {STAGE_FINAL_RUSH,      { 90, 100, 120, 130}},
        {STAGE_IRON_GATE,       {120, 150, 170, 190}},
        {STAGE_DRY_LAGOON,      {100, 120, 150, 170}},
        {STAGE_SAND_OCEAN,      {130, 180, 200, 230}},
        {STAGE_RADICAL_HIGHWAY, {100, 120, 140, 160}},
        {STAGE_EGG_QUARTERS,    { 80, 100, 120, 130}},
        {STAGE_LOST_COLONY,     {300, 350, 400, 440}},
        {STAGE_WEAPONS_BED,     {150, 240, 280, 300}},
        {STAGE_SECURITY_HALL,   {100, 130, 150, 160}},
        {STAGE_WHITE_JUNGLE,    { 80, 100, 120, 130}},
        {STAGE_SKY_RAIL,        { 70,  80,  90, 100}},
        {STAGE_MAD_SPACE,       { 60,  80, 100, 120}},
        {STAGE_COSMIC_WALL,     {300, 400, 450, 500}},
        {STAGE_FINAL_CHASE,     { 70,  80,  90, 100}},
        {STAGE_RELAY_LAST,      {200, 250, 270, 290}},
        {STAGE_GREEN_HILL,      { 20,  50, 100, 150}},
    },
    STAGE_NONE,
};
/* clang-format on */
/* the stages' rank times in MISSION_RINGS and MISSION_CHAO, which end as
 * the rank scores do */
/* clang-format off */
static struct {
    struct rank_times stages[RING_RANKED_STAGES];
    Sint8 end;
} rank_times_rings = {
    {
        {STAGE_CITY_ESCAPE,     {{2, 30}, {2,  0}, {1, 40}, {1, 20}}},
        {STAGE_WILD_CANYON,     {{4, 30}, {3,  0}, {2,  0}, {1, 30}}},
        {STAGE_PRISON_LANE,     {{4,  0}, {3, 30}, {3,  0}, {2, 30}}},
        {STAGE_METAL_HARBOR,    {{1, 50}, {1, 30}, {1, 20}, {1, 10}}},
        {STAGE_GREEN_FOREST,    {{4,  0}, {3, 30}, {3,  0}, {2, 30}}},
        {STAGE_PUMPKIN_HILL,    {{5,  0}, {4,  0}, {3, 30}, {3,  0}}},
        {STAGE_MISSION_STREET,  {{5,  0}, {4, 30}, {4,  0}, {3, 30}}},
        {STAGE_AQUATIC_MINE,    {{4,  0}, {3,  0}, {2, 30}, {1, 50}}},
        {STAGE_HIDDEN_BASE,     {{4, 30}, {4,  0}, {3, 30}, {3, 15}}},
        {STAGE_PYRAMID_CAVE,    {{3,  0}, {2,  0}, {1, 45}, {1, 30}}},
        {STAGE_DEATH_CHAMBER,   {{4, 30}, {3, 30}, {3,  0}, {2, 30}}},
        {STAGE_ETERNAL_ENGINE,  {{6,  0}, {5,  0}, {4,  0}, {3, 30}}},
        {STAGE_METEOR_HERD,     {{5,  0}, {4,  0}, {3,  0}, {2,  0}}},
        {STAGE_CRAZY_GADGET,    {{4, 30}, {4,  0}, {3, 30}, {3,  0}}},
        {STAGE_FINAL_RUSH,      {{3,  0}, {2, 30}, {1, 45}, {1, 30}}},
        {STAGE_IRON_GATE,       {{2, 30}, {2,  0}, {1, 45}, {1, 35}}},
        {STAGE_DRY_LAGOON,      {{4, 30}, {3, 30}, {2, 30}, {2,  0}}},
        {STAGE_SAND_OCEAN,      {{5,  0}, {4, 30}, {4,  0}, {3, 30}}},
        {STAGE_RADICAL_HIGHWAY, {{2, 30}, {2,  0}, {1, 40}, {1, 20}}},
        {STAGE_EGG_QUARTERS,    {{4,  0}, {3,  0}, {2, 30}, {2,  0}}},
        {STAGE_LOST_COLONY,     {{4,  0}, {3,  0}, {2, 30}, {2,  0}}},
        {STAGE_WEAPONS_BED,     {{3, 30}, {2, 45}, {2, 15}, {2,  0}}},
        {STAGE_SECURITY_HALL,   {{5, 30}, {5,  0}, {4, 30}, {4,  0}}},
        {STAGE_WHITE_JUNGLE,    {{3,  0}, {2, 30}, {2,  0}, {1, 30}}},
        {STAGE_SKY_RAIL,        {{2, 30}, {2, 15}, {1, 45}, {1, 15}}},
        {STAGE_MAD_SPACE,       {{5,  0}, {4, 30}, {3,  0}, {2, 30}}},
        {STAGE_COSMIC_WALL,     {{3,  0}, {2,  0}, {1, 45}, {1, 30}}},
        {STAGE_FINAL_CHASE,     {{2, 30}, {2,  0}, {1, 45}, {1, 30}}},
        {STAGE_RELAY_TAILS,     {{5, 30}, {4, 30}, {4,  0}, {3, 30}}},
        {STAGE_RELAY_EGGMAN,    {{5, 30}, {4, 30}, {4,  0}, {3, 30}}},
        {STAGE_RELAY_KNUCKLES,  {{5, 30}, {4, 30}, {4,  0}, {3, 30}}},
        {STAGE_RELAY_ROUGE,     {{5, 30}, {4, 30}, {4,  0}, {3, 30}}},
        {STAGE_RELAY_SONIC,     {{5, 30}, {4, 30}, {4,  0}, {3, 30}}},
        {STAGE_GREEN_HILL,      {{5,  0}, {4,  0}, {3,  0}, {2,  0}}},
    },
    STAGE_NONE,
};
static struct {
    struct rank_times stages[RANKED_STAGES];
    Sint8 end;
} rank_times_chao = {
    {
        {STAGE_CITY_ESCAPE,     {{4,  0}, {3, 10}, {2, 50}, {2, 30}}},
        {STAGE_WILD_CANYON,     {{2, 30}, {2,  0}, {1,  0}, {0, 40}}},
        {STAGE_PRISON_LANE,     {{4,  0}, {3,  0}, {2, 30}, {2,  0}}},
        {STAGE_METAL_HARBOR,    {{1, 20}, {1, 10}, {1,  0}, {0, 50}}},
        {STAGE_GREEN_FOREST,    {{3,  0}, {2, 30}, {2,  0}, {1, 30}}},
        {STAGE_PUMPKIN_HILL,    {{3,  0}, {2, 30}, {1, 30}, {1,  0}}},
        {STAGE_MISSION_STREET,  {{5,  0}, {4,  0}, {3, 40}, {3, 20}}},
        {STAGE_AQUATIC_MINE,    {{4,  0}, {2, 30}, {1, 30}, {1,  0}}},
        {STAGE_HIDDEN_BASE,     {{4, 30}, {4,  0}, {3, 15}, {2, 50}}},
        {STAGE_PYRAMID_CAVE,    {{4, 30}, {4, 15}, {4,  0}, {3, 45}}},
        {STAGE_DEATH_CHAMBER,   {{4,  0}, {3,  0}, {2, 30}, {2,  0}}},
        {STAGE_ETERNAL_ENGINE,  {{4,  0}, {3, 30}, {3,  0}, {2, 45}}},
        {STAGE_METEOR_HERD,     {{5,  0}, {3, 30}, {2, 30}, {1, 30}}},
        {STAGE_CRAZY_GADGET,    {{7,  0}, {6,  0}, {5, 30}, {5,  0}}},
        {STAGE_FINAL_RUSH,      {{5, 30}, {4, 45}, {4, 30}, {4, 15}}},
        {STAGE_IRON_GATE,       {{3,  0}, {2, 30}, {2, 15}, {2,  0}}},
        {STAGE_DRY_LAGOON,      {{2, 30}, {1, 30}, {0, 50}, {0, 40}}},
        {STAGE_SAND_OCEAN,      {{4, 30}, {4,  0}, {3, 30}, {3,  0}}},
        {STAGE_RADICAL_HIGHWAY, {{3, 30}, {3,  0}, {2, 45}, {2, 30}}},
        {STAGE_EGG_QUARTERS,    {{3,  0}, {2,  0}, {1, 30}, {1,  0}}},
        {STAGE_LOST_COLONY,     {{3, 30}, {3,  0}, {2, 45}, {2, 30}}},
        {STAGE_WEAPONS_BED,     {{3, 30}, {3,  0}, {2, 30}, {2, 15}}},
        {STAGE_SECURITY_HALL,   {{3,  0}, {2,  0}, {1,  0}, {0, 30}}},
        {STAGE_WHITE_JUNGLE,    {{5,  0}, {4,  0}, {3, 40}, {3, 20}}},
        {STAGE_SKY_RAIL,        {{2,  0}, {1, 45}, {1, 30}, {1, 10}}},
        {STAGE_MAD_SPACE,       {{5,  0}, {4,  0}, {2,  0}, {1, 30}}},
        {STAGE_COSMIC_WALL,     {{2, 30}, {2,  0}, {1, 45}, {1, 30}}},
        {STAGE_FINAL_CHASE,     {{6, 30}, {6,  0}, {5, 30}, {5, 15}}},
        {STAGE_RELAY_LAST,      {{9,  0}, {8, 30}, {8,  0}, {7, 30}}},
        {STAGE_GREEN_HILL,      {{5,  0}, {4,  0}, {3,  0}, {2,  0}}},
    },
    STAGE_NONE,
};
struct time_limit_table time_limits = {
    {
        {STAGE_CITY_ESCAPE,     {3,  0}},
        {STAGE_WILD_CANYON,     {2,  0}},
        {STAGE_PRISON_LANE,     {3,  0}},
        {STAGE_METAL_HARBOR,    {2,  0}},
        {STAGE_GREEN_FOREST,    {4,  0}},
        {STAGE_PUMPKIN_HILL,    {3,  0}},
        {STAGE_MISSION_STREET,  {3, 30}},
        {STAGE_AQUATIC_MINE,    {3, 30}},
        {STAGE_HIDDEN_BASE,     {3, 30}},
        {STAGE_PYRAMID_CAVE,    {4,  0}},
        {STAGE_DEATH_CHAMBER,   {5,  0}},
        {STAGE_ETERNAL_ENGINE,  {5,  0}},
        {STAGE_METEOR_HERD,     {4, 30}},
        {STAGE_CRAZY_GADGET,    {5,  0}},
        {STAGE_FINAL_RUSH,      {5,  0}},
        {STAGE_IRON_GATE,       {4,  0}},
        {STAGE_DRY_LAGOON,      {3, 30}},
        {STAGE_SAND_OCEAN,      {4,  0}},
        {STAGE_RADICAL_HIGHWAY, {3,  0}},
        {STAGE_EGG_QUARTERS,    {3,  0}},
        {STAGE_LOST_COLONY,     {3, 30}},
        {STAGE_WEAPONS_BED,     {2, 45}},
        {STAGE_SECURITY_HALL,   {3, 30}},
        {STAGE_WHITE_JUNGLE,    {3,  0}},
        {STAGE_SKY_RAIL,        {2,  0}},
        {STAGE_MAD_SPACE,       {4, 30}},
        {STAGE_COSMIC_WALL,     {8,  0}},
        {STAGE_FINAL_CHASE,     {5, 30}},
        {STAGE_RELAY_FIRST,     {7,  0}},
        {STAGE_GREEN_HILL,      {5,  0}},
    },
    STAGE_NONE,
};
/* clang-format on */
struct stage_time cart_time_limits[CART_LIMITS] = {{5, 0}, {5, 0}};
/* STAGE_CART_1's rank times, by mission: the other drivers', then Rouge's */
/* clang-format off */
static struct rank_times rank_times_cart[MISSIONS] = {
    {STAGE_CART_1, {{3, 30}, {3, 15}, {2, 50}, {2, 45}}},
    {STAGE_CART_1, {{3,  0}, {2, 30}, {2,  0}, {1, 30}}},
    {STAGE_CART_1, {{3, 30}, {3, 20}, {3, 10}, {3,  0}}},
    {STAGE_CART_1, {{3, 30}, {3, 20}, {3, 10}, {3,  0}}},
    {STAGE_CART_1, {{3, 20}, {3, 10}, {3,  0}, {2, 50}}},
};
static struct rank_times rank_times_cart_rouge[MISSIONS] = {
    {STAGE_CART_1, {{3, 50}, {3, 40}, {3, 30}, {3, 20}}},
    {STAGE_CART_1, {{3, 30}, {3,  0}, {2, 30}, {2,  0}}},
    {STAGE_CART_1, {{4,  0}, {3, 50}, {3, 40}, {3, 30}}},
    {STAGE_CART_1, {{4,  0}, {3, 55}, {3, 50}, {3, 45}}},
    {STAGE_CART_1, {{3, 50}, {3, 40}, {3, 30}, {3, 20}}},
};
/* clang-format on */
/* each look's life icon, by a player record's kind */
static NJS_TEXLIST *life_icons[17] = {
#include "hud/hud/life_icons.inc"
};

/* TRUE while the bosses' totals show. results without a total wait for it
 * to be FALSE before they end */
static Bool boss_total_showing = FALSE;
/* how many bonuses show, and the row the next bonus drawn takes */
static Sint8 technique_count = 0;
static Sint8 technique_row = 0;
/* the color of a bonus's points, by its label. the last label shows its
 * sprite in place of points, so no draw reads its color */
static Uint32 technique_colors[TECHNIQUE_LABELS] = {
    0x00000000, 0xff00b4ff, 0xff01ff62, 0xff00fff2, 0xffd7ff00, 0xffffce00,
    0xffffa000, 0xffff6f00, 0xffff4800, 0xffff0093, 0xffffffff,
};
/* clang-format off */
static struct hud_number technique_number = {HUD_NUMBER_SCALED, HUD_NUMBER_LEFT, 0, 0, 0, {0.0F, TECHNIQUE_TOP + TECHNIQUE_POINTS_DROP, 0.0F}, 0.75F, NUMBER_WHITE};
/* clang-format on */
/* the bonuses' labels, in console_texlist's texture h_teckpoint_all */
static NJS_TEXANIM technique_labels[TECHNIQUE_LABELS] = {
#include "hud/hud/technique_labels.inc"
};
/* @hack type: one sprite declared as an array of one, where the sprite
 * alone would do */
static NJS_SPRITE technique_sprite[1] = {
#include "hud/hud/technique_sprite.inc"
};
/* the task named CounterOfDestructedEnemy, or null */
static struct task *enemy_chain = NULL;

/* the hud's numbers. score_zeros draws the dimmer zeros left of the score */
/* clang-format off */
static struct hud_number score_zeros      = {NUMBER_ZEROED,     HUD_NUMBER_LEFT,  0, 0,                    0, {16.0F, 36.0F, 0.0F},    0.5F, 0x80303030};
static struct hud_number score_number     = {HUD_NUMBER_SCALED, HUD_NUMBER_RIGHT, 0, 0,                    0, {112.0F, 36.0F, 0.0F},   0.5F, NUMBER_WHITE};
static struct hud_number clock_minutes    = {NUMBER_ZEROED,     0,                0, CLOCK_LAST_MINUTE,    0, {24.0F, CLOCK_Y, 0.0F},  0.5F, NUMBER_WHITE};
static struct hud_number clock_colon_1    = {NUMBER_COLON,      0,                0, 0,                    0, {48.0F, CLOCK_Y, 0.0F},  0.5F, NUMBER_WHITE};
static struct hud_number clock_seconds    = {NUMBER_ZEROED,     0,                0, CLOCK_LAST_SECOND,    0, {72.0F, CLOCK_Y, 0.0F},  0.5F, NUMBER_WHITE};
static struct hud_number clock_colon_2    = {NUMBER_COLON,      0,                0, 0,                    0, {96.0F, CLOCK_Y, 0.0F},  0.5F, NUMBER_WHITE};
static struct hud_number clock_hundredths = {NUMBER_ZEROED,     0,                0, CLOCK_LAST_HUNDREDTH, 0, {120.0F, CLOCK_Y, 0.0F}, 0.5F, NUMBER_WHITE};
static struct hud_number rings_number     = {NUMBER_ZEROED,     0,                0, RINGS_MAX,            0, {64.0F, 88.0F, 0.0F},    0.5F, NUMBER_WHITE};
static struct hud_number lives_number     = {NUMBER_ZEROED,     0,                0, LIVES_MAX,            0, {LIVES_X, 431.0F, 0.0F}, 0.5F, NUMBER_WHITE};
/* clang-format on */
/* the scale of a time the bosses' totals show */
static Float boss_time_scale = 0.7F;
/* the ring beside the rings: hud_draw's, and hud_draw_rings_at's */
static NJS_TEXTURE_VTX ring_quad[4] = {
#include "hud/hud/ring_quad.inc"
};
static NJS_TEXTURE_VTX ring_quad_at[4] = {
#include "hud/hud/ring_quad_at.inc"
};
/* a life icon's frames: hud_draw_lives's, and hud_draw_lives_of's. the
 * second frame draws an icon's second part: Eggman's for both, and Big's for
 * hud_draw_lives alone */
static NJS_TEXANIM life_frames[2] = {
#include "hud/hud/life_frames.inc"
};
static NJS_TEXANIM life_frames_of[2] = {
#include "hud/hud/life_frames_of.inc"
};
/* a round's lamp, lit and unlit, and each lamp's x by player */
static NJS_TEXANIM lamp_frames[2] = {
#include "hud/hud/lamp_frames.inc"
};
static Float lamp_x[2][2] = {{32.0F, 70.0F}, {608.0F, 570.0F}};
static NJS_TEXANIM chao_key_frame[1] = {
#include "hud/hud/chao_key_frame.inc"
};
/* the quad hud_number_draw draws each digit through */
static NJS_TEXTURE_VTX digit_quad[4] = {
#include "hud/hud/digit_quad.inc"
};
/* the rank's voices, by look and then by rank from RANK_E. the VOICED_LOOKS
 * looks are Sonic, Tails's mech, Knuckles, Eggman's mech, Shadow and Rouge.
 * another look has no voice */
#define VOICED_LOOKS 6
/* clang-format off */
static Sint32 score_rank_voices[VOICED_LOOKS][RANKS] = {
    {1603, 1601, 1599, 1597, 1595},
    {1713, 1711, 1709, 1707, 1705},
    {1340, 1338, 1336, 1334, 1332},
    {1710, 1708, 1706, 1704, 1702},
    {1592, 1590, 1588, 1586, 1584},
    {1341, 1339, 1337, 1335, 1333},
};
/* clang-format on */
/* a part of the results zooms in through zoom_scales and zoom_colors, a
 * step of its mode each. it then stays at the last, ZOOMED_SCALE and
 * ZOOMED_COLOR, where its numbers rest. a rank stays at a scale of 1 */
#define ZOOM_STEPS 6
#define ZOOMED_SCALE 0.725F
#define ZOOMED_COLOR 0xffffffff
/* the numbers of the results that total the score */
/* clang-format off */
static struct hud_number result_score      = {HUD_NUMBER_SCALED | HUD_NUMBER_FRONT,                      HUD_NUMBER_RIGHT, 0, 0,                    0, {464.0F, 124.0F, 0.0F}, ZOOMED_SCALE, ZOOMED_COLOR};
static struct hud_number result_minutes    = {NUMBER_ZEROED_FRONT,                                       0,                0, CLOCK_LAST_MINUTE,    0, {348.0F, 160.0F, 0.0F}, ZOOMED_SCALE, ZOOMED_COLOR};
static struct hud_number result_colon_1    = {NUMBER_COLON_FRONT,                                        0,                0, 0,                    0, {380.0F, 160.0F, 0.0F}, ZOOMED_SCALE, ZOOMED_COLOR};
static struct hud_number result_seconds    = {NUMBER_ZEROED_FRONT,                                       0,                0, CLOCK_LAST_SECOND,    0, {410.0F, 160.0F, 0.0F}, ZOOMED_SCALE, ZOOMED_COLOR};
static struct hud_number result_colon_2    = {NUMBER_COLON_FRONT,                                        0,                0, 0,                    0, {444.0F, 160.0F, 0.0F}, ZOOMED_SCALE, ZOOMED_COLOR};
static struct hud_number result_hundredths = {NUMBER_ZEROED_FRONT,                                       0,                0, CLOCK_LAST_HUNDREDTH, 0, {476.0F, 160.0F, 0.0F}, ZOOMED_SCALE, ZOOMED_COLOR};
static struct hud_number result_rings      = {HUD_NUMBER_SCALED | HUD_NUMBER_CLAMPED | HUD_NUMBER_FRONT, HUD_NUMBER_RIGHT, 0, RINGS_MAX,            0, {368.0F, 196.0F, 0.0F}, ZOOMED_SCALE, ZOOMED_COLOR};
static struct hud_number result_ring_total = {HUD_NUMBER_SCALED | HUD_NUMBER_CLAMPED | HUD_NUMBER_FRONT, HUD_NUMBER_RIGHT, 0, RINGS_MAX,            0, {464.0F, 196.0F, 0.0F}, ZOOMED_SCALE, ZOOMED_COLOR};
/* @hack type: the time bonus's number declared as an array of one, where
 * the number alone would do */
static struct hud_number result_bonus[1]   = {{HUD_NUMBER_SCALED | HUD_NUMBER_FRONT,                     HUD_NUMBER_RIGHT, 0, 0,                    0, {464.0F, 272.0F, 0.0F}, ZOOMED_SCALE, ZOOMED_COLOR}};
static struct hud_number result_total      = {HUD_NUMBER_SCALED | HUD_NUMBER_FRONT,                      HUD_NUMBER_RIGHT, 0, 0,                    0, {464.0F, 308.0F, 0.0F}, ZOOMED_SCALE, ZOOMED_COLOR};
static Float zoom_scales[ZOOM_STEPS]  = {2.0F,       1.475F,     1.175F,     0.925F,     0.8F,       ZOOMED_SCALE};
static Uint32 zoom_colors[ZOOM_STEPS] = {0x10ffffff, 0x30ffffff, 0x60ffffff, 0xc0ffffff, 0xe8ffffff, ZOOMED_COLOR};
/* clang-format on */
/* the result sheet's quads, by what each shows. a mission's title has three
 * parts. RESULT_MISSION_1, RESULT_MISSION_2 and RESULT_MISSION_3 each start a
 * part's quads, one for each mission. RESULT_RANK_A starts the
 * ranks, from A down to E. RESULT_ALL_RINGS shows where the rings reach
 * ring_total, and RESULT_RINGS_OF stands between the two counts */
#define RESULT_SCORE_LABEL 0
#define RESULT_TIME_LABEL 1
#define RESULT_RING_LABEL 2 /* for 1 ring or none */
#define RESULT_RINGS_LABEL 3
#define RESULT_BONUS_LABEL 4 /* the first of two quads */
#define RESULT_TOTAL_LABEL 6 /* the first of two quads */
#define RESULT_MISSION_1 8
#define RESULT_MISSION_2 13
#define RESULT_MISSION_3 18
#define RESULT_ALL_RINGS 23
#define RESULT_RINGS_OF 24
#define RESULT_RANK_A 25
/* the title STAGE_GREEN_HILL shows in place of its mission's */
#define RESULT_TITLE_GREEN_HILL 30
#define RESULT_QUADS 31
#define QUAD_VERTICES 4
/* @hack type: the quads as one array of their vertices, four a quad, where
 * an array of quads of four vertices would do */
static NJS_TEXTURE_VTX result_quads[RESULT_QUADS * QUAD_VERTICES] = {
#include "hud/hud/result_quads.inc"
};
#define RESULT_QUAD(n) (&result_quads[(n) * QUAD_VERTICES])
/* the score result's rank. score_rank_corners hold its corners about its
 * middle, and score_rank_place puts its middle on the screen. it zooms in
 * through score_rank_turns and score_rank_scales, a step of its mode each */
/* clang-format off */
static NJS_POINT3 score_rank_corners[QUAD_VERTICES] = {
    {-39.0F, -31.5F, 0.0F},
    {-39.0F,  31.5F, 0.0F},
    { 39.0F, -31.5F, 0.0F},
    { 39.0F,  31.5F, 0.0F},
};
/* clang-format on */
static NJS_VECTOR score_rank_place = {320.0F, 384.0F, 1.0002F};
static Angle score_rank_turns[ZOOM_STEPS] = {6144, 3072, 1536, 512, 256, 0};
/* clang-format off */
static Float score_rank_scales[ZOOM_STEPS] = {2.0F, 1.6F, 1.4F, 1.2F, 1.1F, 1.0F};
/* clang-format on */
/* the voices of the time result's rank, as score_rank_voices */
/* clang-format off */
static Sint32 time_rank_voices[VOICED_LOOKS][RANKS] = {
    {1603, 1601, 1599, 1597, 1595},
    {1713, 1711, 1709, 1707, 1705},
    {1340, 1338, 1336, 1334, 1332},
    {1710, 1708, 1706, 1704, 1702},
    {1592, 1590, 1588, 1586, 1584},
    {1341, 1339, 1337, 1335, 1333},
};
/* clang-format on */
/* result_score's twin for the results that total the time, which draw no
 * score */
/* clang-format off */
static struct hud_number time_score      = {HUD_NUMBER_SCALED | HUD_NUMBER_FRONT, HUD_NUMBER_RIGHT, 0, 0,                    0, {464.0F, 124.0F, 0.0F}, ZOOMED_SCALE, ZOOMED_COLOR};
/* the time result's clock */
static struct hud_number time_minutes    = {NUMBER_ZEROED_FRONT,                  0,                0, CLOCK_LAST_MINUTE,    0, {348.0F, 160.0F, 0.0F}, ZOOMED_SCALE, ZOOMED_COLOR};
static struct hud_number time_colon_1    = {NUMBER_COLON_FRONT,                   0,                0, 0,                    0, {380.0F, 160.0F, 0.0F}, ZOOMED_SCALE, ZOOMED_COLOR};
static struct hud_number time_seconds    = {NUMBER_ZEROED_FRONT,                  0,                0, CLOCK_LAST_SECOND,    0, {410.0F, 160.0F, 0.0F}, ZOOMED_SCALE, ZOOMED_COLOR};
static struct hud_number time_colon_2    = {NUMBER_COLON_FRONT,                   0,                0, 0,                    0, {444.0F, 160.0F, 0.0F}, ZOOMED_SCALE, ZOOMED_COLOR};
static struct hud_number time_hundredths = {NUMBER_ZEROED_FRONT,                  0,                0, CLOCK_LAST_HUNDREDTH, 0, {476.0F, 160.0F, 0.0F}, ZOOMED_SCALE, ZOOMED_COLOR};
/* clang-format on */
/* the time result's label, a copy of result_quads' RESULT_TIME_LABEL */
static NJS_TEXTURE_VTX time_label_quad[4] = {
#include "hud/hud/time_label_quad.inc"
};
/* the time result's rank, as the score result's */
/* clang-format off */
static NJS_POINT3 time_rank_corners[QUAD_VERTICES] = {
    {-39.0F, -31.5F, 0.0F},
    {-39.0F,  31.5F, 0.0F},
    { 39.0F, -31.5F, 0.0F},
    { 39.0F,  31.5F, 0.0F},
};
/* clang-format on */
static NJS_VECTOR time_rank_place = {320.0F, 384.0F, 1.0002F};
static Angle time_rank_turns[ZOOM_STEPS] = {6144, 3072, 1536, 512, 256, 0};
/* clang-format off */
static Float time_rank_scales[ZOOM_STEPS] = {2.0F, 1.6F, 1.4F, 1.2F, 1.1F, 1.0F};
/* clang-format on */
/* the winner screen's quads, in game2p_texlist's one texture.
 * WINNER_AGAIN_LIT and WINNER_LEAVE_LIT show the choice the pads are on, and
 * WINNER_AGAIN_DIM and WINNER_LEAVE_DIM the other */
#define WINNER_BANNER 0
#define WINNER_CONTINUE 1
#define WINNER_AGAIN_LIT 2
#define WINNER_AGAIN_DIM 3
#define WINNER_LEAVE_LIT 4
#define WINNER_LEAVE_DIM 5
#define WINNER_BETWEEN 6
#define WINNER_QUADS 7
/* @hack type: the quads as one array of their vertices, as result_quads */
static NJS_TEXTURE_VTX winner_quads[WINNER_QUADS * QUAD_VERTICES] = {
#include "hud/hud/winner_quads.inc"
};
#define WINNER_QUAD(n) (&winner_quads[(n) * QUAD_VERTICES])
/* the time left to the time limit */
/* clang-format off */
static struct hud_number countdown_minutes    = {NUMBER_ZEROED_FRONT, 0, 0, CLOCK_LAST_MINUTE,    0, {428.0F, 48.0F, 0.0F}, 0.91F, NUMBER_WHITE};
static struct hud_number countdown_colon_1    = {NUMBER_COLON_FRONT,  0, 0, 0,                    0, {460.0F, 48.0F, 0.0F}, 0.91F, NUMBER_WHITE};
static struct hud_number countdown_seconds    = {NUMBER_ZEROED_FRONT, 0, 0, CLOCK_LAST_SECOND,    0, {500.0F, 48.0F, 0.0F}, 0.91F, NUMBER_WHITE};
static struct hud_number countdown_colon_2    = {NUMBER_COLON_FRONT,  0, 0, 0,                    0, {536.0F, 48.0F, 0.0F}, 0.91F, NUMBER_WHITE};
static struct hud_number countdown_hundredths = {NUMBER_ZEROED_FRONT, 0, 0, CLOCK_LAST_HUNDREDTH, 0, {576.0F, 48.0F, 0.0F}, 0.91F, NUMBER_WHITE};
/* clang-format on */
/* the labels of the bosses' totals, in the texture of bossResult */
static NJS_TEXNAME boss_result_texname[1] = {
#include "hud/hud/boss_result_texname.inc"
};
static NJS_TEXLIST boss_result_texlist = {
#include "hud/hud/boss_result_texlist.inc"
};
/* each course's view, from BOSS_COURSE_HERO to BOSS_COURSE_ALL */
/* clang-format off */
static struct boss_layout boss_layouts[BOSS_TOTALS] = {
    {{{540.0F, 100.0F, 320.0F, 128.0F}, {540.0F, 100.0F, 390.0F, 340.0F}}, BOSS_RUN},
    {{{540.0F, 100.0F, 320.0F, 128.0F}, {540.0F, 100.0F, 390.0F, 340.0F}}, BOSS_RUN},
    {{{540.0F, 100.0F, 220.0F, 140.0F}, {540.0F, 100.0F, 290.0F, 240.0F}}, BOSS_TOTALS},
};
/* clang-format on */
#define BOSS_LABELS 21
static NJS_TEXANIM boss_labels[BOSS_LABELS] = {
#include "hud/hud/boss_labels.inc"
};
/* a line of a course's view is boss_line_height high. the sum of all three
 * runs takes a gap above it, which differs for BOSS_COURSE_ALL */
static Float boss_line_height = 20.0F;
static Float boss_total_gap = 50.0F;
static Float boss_total_gap_all = 46.0F;
/* where each course's first line starts */
static Float boss_left = 136.0F;
static Float boss_top_first = 146.0F;
static Float boss_top_second = 146.0F;
static Float boss_top_all = 150.0F;
/* where a time starts from its label */
static NJS_POINT3 boss_time_offset = {250.0F, 10.0F, 0.0F};

static struct boss_record boss_record;

void life_icon_load(int kind)
{
    njLoadTexture(life_icons[kind]);
}

void life_icon_release(int kind)
{
    njReleaseTexture(life_icons[kind]);
}

void play_reset(void)
{
    kept_rings_clear();
    rings_restore();
    kept_score_clear();
    score_restore();
    kept_clock_clear();
    stage_clock_restore();
    kept_ring_total_clear();
    ring_total_restore();
    lives_reset();
}

void kept_score_clear(void)
{
    game_state.kept_score = 0;
    /* @hack redundant: a return at the end */
    return; /* NOLINT(readability-redundant-control-flow) */
}

void kept_rings_clear(void)
{
    game_state.kept_rings = 0;
}

void kept_clock_clear(void)
{
    game_state.kept_frame = 0;
    game_state.kept_second = 0;
    game_state.kept_minute = 0;
    snapshot_current->kept_frame = 0;
    snapshot_current->kept_second = 0;
    snapshot_current->kept_minute = 0;
}

void kept_ring_total_clear(void)
{
    game_state.kept_ring_total = 0;
}

void stage_clock_keep(void)
{
    game_state.kept_frame = game_state.frame;
    game_state.kept_second = game_state.second;
    game_state.kept_minute = game_state.minute;
    snapshot_current->kept_frame = game_state.frame;
    snapshot_current->kept_second = game_state.second;
    snapshot_current->kept_minute = game_state.minute;
}

void rings_keep(void)
{
    game_state.kept_rings = game_state.rings[0];
}

void score_keep(void)
{
    game_state.kept_score = game_state.score;
}

void ring_total_keep(void)
{
    game_state.kept_ring_total = game_state.ring_total;
}

void score_restore(void)
{
    game_state.score = game_state.kept_score;
    snapshot_current->score = game_state.kept_score;
    /* @hack redundant: a return at the end */
    return; /* NOLINT(readability-redundant-control-flow) */
}

void rings_restore(void)
{
    if (stage_current_id() == STAGE_BOSS_LAST2) {
        game_state.rings[0] = BOSS_LAST2_RINGS;
        snapshot_current->rings[0] = BOSS_LAST2_RINGS;
    } else {
        game_state.rings[0] = game_state.kept_rings;
        snapshot_current->rings[0] = game_state.kept_rings;
    }
    game_state.rings[1] = 0;
    snapshot_current->rings[1] = 0;
}

void lives_reset(void)
{
    game_state.lives[0] = LIVES_START;
    game_state.lives[1] = LIVES_START;
    snapshot_current->lives[0] = LIVES_START;
    snapshot_current->lives[1] = LIVES_START;
}

void stage_clock_restore(void)
{
    game_state.frame = game_state.kept_frame;
    game_state.second = game_state.kept_second;
    game_state.minute = game_state.kept_minute;
    game_state.steps = 0;
    snapshot_current->frame = game_state.kept_frame;
    snapshot_current->second = game_state.kept_second;
    snapshot_current->minute = game_state.kept_minute;
}

void ring_total_restore(void)
{
    game_state.ring_total = game_state.kept_ring_total;
    snapshot_current->ring_total = game_state.kept_ring_total;
}

void lives_show_step(struct task *task)
{
    if ((*(Sint32 *)task->extra.p)++ > MOMENTARY_LAST) {
        task_kill(task);
    } else {
        game_state.lives_shown = 1;
    }
}

/* @hack redundant: lives_show_start, which nothing calls, kept for the task
 * name the image holds at the head of the object's constants. SHC leaves out
 * its code, so the image shows neither the step it starts nor its level */
static void
lives_show_start(void) /* NOLINT(clang-diagnostic-unused-function) */
{
    task_create(TASK_EXTRA, 3, lives_show_step, "MomentaryDisplayPlayerNumber");
}

void lives_add(int player, Sint16 count)
{
    player &= 1;
    if (count > 0) {
        sound_play(SOUND_LIFE, NULL, 0, 0);
    }
    game_state.lives[player] += count;
    if (game_state.lives[player] > LIVES_MAX ||
        (game_state.lives[player] < 0 && count > 0)) {
        game_state.lives[player] = LIVES_MAX;
    }
    snapshot_current->lives[player] = game_state.lives[player];
}

void rings_hide_step(struct task *task)
{
    if ((*(Sint32 *)task->extra.p)++ > MOMENTARY_LAST) {
        task_kill(task);
    } else {
        game_state.rings_shown = 0;
    }
}

/* @hack redundant: rings_hide_start, kept as lives_show_start is */
static void
rings_hide_start(void) /* NOLINT(clang-diagnostic-unused-function) */
{
    task_create(TASK_EXTRA, 3, rings_hide_step, "MomentaryDisplayRingNumber");
}

void ring_add(int player, Sint16 count)
{
    Sint16 old;
    int before;
    int after;
    Sint16 last;
    Sint16 reach;
    int voice;

    player &= 1;
    old = game_state.rings[player];
    game_state.rings[player] += count;
    if (game_state.rings[player] > RINGS_MAX) {
        game_state.rings[player] = RINGS_MAX;
    } else if (game_state.rings[player] < 0) {
        game_state.rings[player] = 0;
    }
    before = old / RINGS_PER_LIFE;
    after = game_state.rings[player] / RINGS_PER_LIFE;
    /* @hack spelling: before < after, where after > before would do */
    if (before < after) {
        lives_add(player, after - before);
    }
    snapshot_current->rings[player] = game_state.rings[player];
    /* @hack spelling: the rest nested in a test of two_players, where a
     * return for 0 would do */
    if (game_state.two_players != 0) {
        voice = -1;
        /* @hack spelling: reach set before last, where the other order
         * would do */
        reach = game_state.rings[player] - old;
        last = old % LEVEL_2_RINGS;
        reach += last;
        if (last < LEVEL_0_RINGS && reach >= LEVEL_0_RINGS) {
            game_state.ring_levels[player][0] = 1;
            game_state.ring_levels[player][1] = 0;
            game_state.ring_levels[player][2] = 0;
            voice = 0;
        }
        if (last < LEVEL_1_RINGS && reach >= LEVEL_1_RINGS) {
            game_state.ring_levels[player][0] = 0;
            game_state.ring_levels[player][1] = 1;
            game_state.ring_levels[player][2] = 0;
            voice = 0;
        }
        if (last < LEVEL_2_RINGS && reach >= LEVEL_2_RINGS) {
            game_state.ring_levels[player][0] = 0;
            game_state.ring_levels[player][1] = 0;
            game_state.ring_levels[player][2] = 1;
            voice = 0;
        }
        if (voice != 0 || player_records[player] == NULL) {
            return;
        }
        switch (player_records[player]->kind) {
        case PLAYER_SONIC:
            voice = VOICE_LEVEL_SONIC;
            break;
        case PLAYER_SHADOW:
            voice = VOICE_LEVEL_SHADOW;
            break;
        case PLAYER_EGGMAN_MECH:
            voice = VOICE_LEVEL_EGGMAN_MECH;
            break;
        case PLAYER_TAILS_MECH:
            voice = VOICE_LEVEL_TAILS_MECH;
            break;
        case PLAYER_KNUCKLES:
            voice = VOICE_LEVEL_KNUCKLES;
            break;
        case PLAYER_ROUGE:
            voice = VOICE_LEVEL_ROUGE;
            break;
        case PLAYER_KIND_AMY:
            if (game_state.voice_language == VOICE_JAPANESE) {
                voice = VOICE_LEVEL_AMY_JAPANESE;
            } else {
                voice = VOICE_LEVEL_AMY_ENGLISH;
            }
            break;
        }
        if (voice > 0) {
            voice_play_priority_2(voice);
        }
        game_state.ring_level_timer[player] = RING_LEVEL_FRAMES;
    }
}

void score_add(int points)
{
    Uint32 total;

    if (player_records[0] != NULL &&
        (player_records[0]->flags & PLAYER_INVINCIBLE)) {
        /* @hack spelling: points shifted, where points * 2 would do */
        points <<= 1;
    }
    total = points + game_state.score;
    if (total > SCORE_MAX) {
        total = SCORE_MAX;
    }
    game_state.score = total;
    snapshot_current->score = total;
}

static void technique_score_step(struct task *task)
{
    struct technique_score *bonus = task->extra.p;

    if (game_state.cleared) {
        task_kill(task);
    } else if (bonus->timer++ > TECHNIQUE_STEPS) {
        task_kill(task);
    } else if (bonus->timer > TECHNIQUE_LABEL_STEPS) {
        bonus->points_shown = 1;
    }
}

static void technique_score_draw(struct task *task)
{
    struct technique_score *bonus = task->extra.p;
    int slide;
    Float x;

    if (technique_row >= technique_count) {
        technique_row = 0;
    }
    if (game_state.unk_0b == 0) {
        return;
    }
    if (bonus->slide > 0) {
        bonus->slide -= TECHNIQUE_SLIDE_STEP;
    }
    slide = bonus->slide;
    /* @hack spelling: x set in the test, ahead of its comma, where a
     * statement of its own would do */
    if (x = (TECHNIQUE_SLIDE_WIDTH * slide) + TECHNIQUE_LEFT,
        bonus->points_shown == 0) {
        if (bonus->label != 0) {
            njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR,
                                NJD_COLOR_BLENDING_INVSRCALPHA);
            technique_sprite->p.x = x;
            if (slide == 0) {
                technique_sprite->p.y =
                    (technique_row * TECHNIQUE_ROW) + TECHNIQUE_TOP;
            } else {
                technique_sprite->p.y = TECHNIQUE_TOP;
            }
            njDrawSprite2D(technique_sprite, bonus->label, SPRITE_PRIORITY,
                           NJD_SPRITE_ALPHA);
            njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
        }
    } else {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        if (bonus->label >= TECHNIQUE_LABELS - 1) {
            njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
            technique_sprite->p.x = x;
            technique_sprite->p.y =
                (technique_row * TECHNIQUE_ROW) + TECHNIQUE_TOP;
            njDrawSprite2D(technique_sprite, 0, SPRITE_PRIORITY,
                           NJD_SPRITE_ALPHA);
            njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
        } else {
            technique_number.color = technique_colors[bonus->label];
            technique_number.value = bonus->score;
            technique_number.pos.x = x;
            technique_number.pos.y = (technique_row * TECHNIQUE_ROW) +
                                     (TECHNIQUE_TOP + TECHNIQUE_POINTS_DROP);
            hud_number_draw(&technique_number);
        }
    }
    technique_row++;
}

static void technique_score_destroy(struct task *task)
{
    struct technique_score *bonus = task->extra.p;
    /* @hack spelling: pending, a pointer to the points, and zero, a local
     * for the 0 stored through it, where bonus->score read and cleared in
     * place would do */
    Sint16 *pending = &bonus->score;
    Sint16 zero = 0;
    int points;
    Uint32 total;

    points = *pending;
    *pending = zero;
    if (points != 0) {
        total = points + game_state.score;
        if (total > SCORE_MAX) {
            total = SCORE_MAX;
        }
        game_state.score = total;
        snapshot_current->score = total;
    }
    technique_count--;
}

void technique_score_start(int score)
{
    struct task *task;
    struct technique_score *bonus;
    int label;

    if (game_state.two_players != 0) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_EXTRA, HUD_LEVEL, technique_score_step,
                            "DispTechniqueScore")) == NULL) {
        return;
    }
    bonus = task->extra.p;
    task->destructor = technique_score_destroy;
    task->draw = technique_score_draw;
    bonus->score = score;
    if (score >= 2000) {
        label = 10;
    } else if (score >= 1500) {
        label = 9;
    } else if (score >= 1000) {
        label = 8;
    } else if (score >= 800) {
        label = 7;
    } else if (score >= 600) {
        label = 6;
    } else if (score >= 500) {
        label = 5;
    } else if (score >= 400) {
        label = 4;
    } else if (score >= 300) {
        label = 3;
    } else if (score >= 200) {
        label = 2;
    } else if (score >= 100) {
        label = 1;
    } else {
        label = 0;
    }
    bonus->label = label;
    bonus->slide = TECHNIQUE_SLIDE_START;
    technique_count++;
}

void score_add_enemy(int score)
{
    int character;
    struct task_work *work;
    int mode;

    character = stage_character(0);
    score_add(score);
    /* @hack spelling: work set inside mode's assignment, where a statement
     * of its own would do */
    mode = (work = player_work[0])->mode;
    if (game_state.two_players != 0) {
        return;
    }
    if (enemy_hit_hook != NULL) {
        return;
    }
    if (character == PLAYER_EGGMAN_MECH || character == PLAYER_TAILS_MECH) {
        return;
    }
    if (work->flags & PLAYER_WORK_GROUNDED) {
        if (character != PLAYER_SONIC && character != PLAYER_SHADOW) {
            return;
        } else if (mode < PLAYER_MODE_SONIC_LIGHT_ATTACK ||
                   mode > PLAYER_MODE_SONIC_BOUNCE) {
            return;
        }
    }
    if (enemy_chain == NULL) {
        if ((enemy_chain = task_create(TASK_EXTRA, HUD_LEVEL, enemy_chain_step,
                                       "CounterOfDestructedEnemy")) == NULL) {
            return;
        }
        *(Sint32 *)enemy_chain->extra.p = 0;
        enemy_chain->destructor = enemy_chain_destroy;
        /* @hack redundant: a return before an else, where the else alone
         * would do */
        return;
    } else {
        (*(Sint32 *)enemy_chain->extra.p)++;
    }
}

static void enemy_chain_step(struct task *task)
{
    int character;
    struct task_work *work;
    int mode;

    character = stage_character(0);
    /* @hack spelling: work set inside mode's assignment, as in
     * score_add_enemy */
    mode = (work = player_work[0])->mode;
    if (enemy_hit_hook == NULL) {
        if (!(work->flags & PLAYER_WORK_GROUNDED)) {
            return;
        }
        if ((character == PLAYER_SONIC || character == PLAYER_SHADOW) &&
            mode >= PLAYER_MODE_SONIC_LIGHT_ATTACK &&
            mode <= PLAYER_MODE_SONIC_BOUNCE) {
            return;
        }
    }
    task_kill(task);
    if (enemy_chain == task) {
        enemy_chain = NULL;
    }
}

/* the chain's bonus, by the enemies it counted after its first. it shows
 * only where the chain's own step ended it */
static void enemy_chain_destroy(struct task *task)
{
    int score;

    if (enemy_chain == task) {
        enemy_chain = NULL;
    }
    switch (*(Sint32 *)task->extra.p) {
    case 0:
        return;
    case 1:
        score = 100;
        break;
    case 2:
        score = 200;
        break;
    case 3:
        score = 300;
        break;
    case 4:
        score = 400;
        break;
    case 5:
        score = 500;
        break;
    case 6:
        score = 600;
        break;
    case 7:
        score = 800;
        break;
    case 8:
        score = 1000;
        break;
    case 9:
        score = 1500;
        break;
    case 10:
    default:
        score = 2000;
        break;
    }
    if (task->step == task_destroy) {
        technique_score_start(score);
    }
    *(Sint32 *)task->extra.p = 0;
}

Sint16 ring_count(int player)
{
    return game_state.rings[player & 1];
}

Sint16 lives_count(int player)
{
    return game_state.lives[player & 1];
}

void stage_clock_resume(void)
{
    game_state.clock_stopped = 0;
}

void stage_clock_stop(void)
{
    game_state.clock_stopped = 1;
}

void stage_clock_step(void)
{
    if (game_state.time_stopped != 0) {
        return;
    }
    game_state.steps++;
    if (game_state.clock_stopped == 1) {
        return;
    }
    if (game_state.mini_event_running == 1) {
        return;
    }
    if (++game_state.frame >= STEPS_PER_SECOND) {
        game_state.frame = 0;
        if (++game_state.second >= SECONDS_PER_MINUTE) {
            game_state.second = 0;
            if (++game_state.minute >= CLOCK_MINUTES) {
                game_state.minute = CLOCK_LAST_MINUTE;
                game_state.second = CLOCK_LAST_SECOND;
                game_state.frame = CLOCK_LAST_FRAME;
            }
        }
    }
    snapshot_current->frame = game_state.frame;
    snapshot_current->second = game_state.second;
    snapshot_current->minute = game_state.minute;
    if (game_state.ring_level_timer[0] > 0) {
        game_state.ring_level_timer[0]--;
    }
    if (game_state.ring_level_timer[1] > 0) {
        game_state.ring_level_timer[1]--;
    }
}

void stage_clock_get(Sint8 *minute, Sint8 *second, Sint8 *frame)
{
    if (minute) {
        *minute = game_state.minute;
    }
    if (second) {
        *second = game_state.second;
    }
    if (frame) {
        *frame = game_state.frame;
    }
}

void stage_clock_set(Sint8 minute, Sint8 second, Sint8 frame)
{
    if (minute >= 0) {
        game_state.minute = minute;
    }
    if (second >= 0) {
        game_state.second = second;
    }
    if (frame >= 0) {
        game_state.frame = frame;
    }
    snapshot_current->frame = game_state.frame;
    snapshot_current->second = game_state.second;
    snapshot_current->minute = game_state.minute;
}

void stage_clock_add(Sint8 minute, Sint8 second, Sint8 frame)
{
    int sum;
    int carry;

    sum = frame + game_state.frame;
    carry = sum / STEPS_PER_SECOND;
    if (sum >= STEPS_PER_SECOND) {
        game_state.frame = sum % STEPS_PER_SECOND;
        sum = carry;
    } else if (sum < 0) {
        /* @bug arithmetic: a sum below 0 that is a multiple of 60 sets the
         * frame to 60, where 0 is meant, and borrows a second too many. a
         * sum of -60 gives 60 frames and a borrow of 2. ADD #60 at
         * 0x8c052d52 and ADD #-1 at 0x8c052d58 compute them. the seconds do
         * the same at 0x8c052d9c */
        game_state.frame = (sum % STEPS_PER_SECOND) + STEPS_PER_SECOND;
        sum = carry - 1;
    } else {
        game_state.frame = sum;
        sum = 0;
    }
    sum += game_state.second + second;
    carry = sum / SECONDS_PER_MINUTE;
    if (sum >= SECONDS_PER_MINUTE) {
        game_state.second = sum % SECONDS_PER_MINUTE;
        sum = carry;
    } else if (sum < 0) {
        game_state.second = (sum % SECONDS_PER_MINUTE) + SECONDS_PER_MINUTE;
        sum = carry - 1;
    } else {
        game_state.second = sum;
        sum = 0;
    }
    sum += game_state.minute + minute;
    if (sum >= CLOCK_MINUTES) {
        game_state.minute = CLOCK_LAST_MINUTE;
        game_state.second = CLOCK_LAST_SECOND;
        game_state.frame = CLOCK_LAST_FRAME;
    } else {
        game_state.minute = sum;
    }
    snapshot_current->frame = game_state.frame;
    snapshot_current->second = game_state.second;
    snapshot_current->minute = game_state.minute;
}

/* the score, and the zeros left of it in a dimmer number */
static void hud_draw_score(int scoreless)
{
    Uint32 score;
    Uint32 zeros;

    if (scoreless == 1) {
        return;
    }
    njSetTexture(&console_texlist);
    score_number.value = game_state.score;
    hud_number_draw(&score_number);
    score = game_state.score;
    if (score < 10) {
        zeros = 9999999;
    } else if (score < 100) {
        zeros = 999999;
    } else if (score < 1000) {
        zeros = 99999;
    } else if (score < 10000) {
        zeros = 9999;
    } else if (score < 100000) {
        zeros = 999;
    } else if (score < 1000000) {
        zeros = 99;
    } else if (score < 10000000) {
        zeros = 9;
    } else {
        zeros = 0;
    }
    score_zeros.max = zeros;
    if (zeros != 0) {
        hud_number_draw(&score_zeros);
    }
}

static void hud_draw_clock(int scoreless)
{
    /* @hack spelling: the five numbers through pointers, set in this order,
     * where the numbers themselves would do */
    struct hud_number *colon_1 = &clock_colon_1;
    struct hud_number *colon_2 = &clock_colon_2;
    struct hud_number *seconds = &clock_seconds;
    struct hud_number *minutes = &clock_minutes;
    struct hud_number *hundredths = &clock_hundredths;
    Float y;

    if (scoreless == 0) {
        y = CLOCK_Y;
    } else {
        y = CLOCK_Y_SCORELESS;
    }
    minutes->pos.y = y;
    colon_1->pos.y = y;
    seconds->pos.y = y;
    colon_2->pos.y = y;
    hundredths->pos.y = y;
    njSetTexture(&console_texlist);
    minutes->value = game_state.minute;
    seconds->value = game_state.second;
    hundredths->value = njRoundUp(game_state.frame * HUNDREDTHS_PER_FRAME);
    hud_number_draw(&clock_minutes);
    hud_number_draw(&clock_colon_1);
    hud_number_draw(&clock_seconds);
    hud_number_draw(&clock_colon_2);
    hud_number_draw(&clock_hundredths);
}

/* a time in the clock's numbers, at boss_time_scale, from pos. its parts take
 * BOSS_Z, though hud_number_draw reads no number's z */
static void time_draw(NJS_POINT3 *pos, int minute, int second, int hundredth)
{
    struct hud_number minutes;
    struct hud_number colon_1;
    struct hud_number seconds;
    struct hud_number colon_2;
    struct hud_number hundredths;

    minutes = clock_minutes;
    colon_1 = clock_colon_1;
    seconds = clock_seconds;
    colon_2 = clock_colon_2;
    hundredths = clock_hundredths;
    minutes.scale = boss_time_scale;
    colon_1.scale = boss_time_scale;
    seconds.scale = boss_time_scale;
    colon_2.scale = boss_time_scale;
    hundredths.scale = boss_time_scale;
    minutes.pos.z = BOSS_Z;
    colon_1.pos.z = BOSS_Z;
    seconds.pos.z = BOSS_Z;
    colon_2.pos.z = BOSS_Z;
    hundredths.pos.z = BOSS_Z;
    minutes.pos.x = pos->x;
    colon_1.pos.x = pos->x + 24.0F;
    seconds.pos.x = pos->x + 48.0F;
    colon_2.pos.x = pos->x + 72.0F;
    hundredths.pos.x = pos->x + 96.0F;
    minutes.pos.y = pos->y;
    colon_1.pos.y = pos->y;
    seconds.pos.y = pos->y;
    colon_2.pos.y = pos->y;
    hundredths.pos.y = pos->y;
    njSetTexture(&console_texlist);
    minutes.value = minute;
    seconds.value = second;
    hundredths.value = hundredth;
    hud_number_draw(&minutes);
    hud_number_draw(&colon_1);
    hud_number_draw(&seconds);
    hud_number_draw(&colon_2);
    hud_number_draw(&hundredths);
}

/* the rows hud_draw_rings can draw in: below the score, where the score is
 * missing, or at the top. RING_ROW_UNDER_SCORE and RING_ROW_SCORELESS are 0
 * and 1, so hud_draw's scoreless picks between them */
#define RING_ROW_UNDER_SCORE 0
#define RING_ROW_SCORELESS 1
#define RING_ROW_TOP 2

/* player's rings and the ring beside them, in row. a count of 0 takes
 * NUMBER_RED | pulse for its color. while game_state.active_player isn't 0, the
 * count is player 1's */
static void hud_draw_rings(int player, Uint32 pulse, int row)
{
    /* @hack spelling: a 64.0F kept in x and stored from it in the rows under
     * the score and at the top, where 64.0F at each store would do */
    Float x = 64.0F;

    if (row == RING_ROW_UNDER_SCORE) {
        rings_number.pos.y = 88.0F;
        ring_quad[0].y = ring_quad[2].y = x;
        ring_quad[1].y = ring_quad[3].y = 96.0F;
    } else if (row == RING_ROW_SCORELESS) {
        rings_number.pos.y = 72.0F;
        ring_quad[0].y = ring_quad[2].y = 48.0F;
        ring_quad[1].y = ring_quad[3].y = 80.0F;
    } else {
        rings_number.pos.y = 56.0F;
        ring_quad[0].y = ring_quad[2].y = 32.0F;
        ring_quad[1].y = ring_quad[3].y = x;
    }
    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    if (player == 0) {
        ring_quad[0].x = ring_quad[1].x = 12.0F;
        ring_quad[2].x = ring_quad[3].x = 44.0F;
    } else {
        x = 384.0F;
        ring_quad[0].x = ring_quad[1].x = 332.0F;
        ring_quad[2].x = ring_quad[3].x = 364.0F;
    }
    njSetTexture(&console_texlist);
    njDrawTexture(ring_quad, 4, TEXTURE_RING, 1);
    rings_number.pos.x = x;
    if (game_state.active_player != 0) {
        rings_number.value = game_state.rings[1];
    } else {
        rings_number.value = game_state.rings[player];
    }
    if (rings_number.value == 0) {
        rings_number.color = pulse | NUMBER_RED;
    } else {
        rings_number.color = NUMBER_WHITE;
    }
    hud_number_draw(&rings_number);
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
}

void hud_draw_rings_at(int player, Uint32 pulse, Float y)
{
    Float x;

    rings_number.pos.y = 88.0F;
    ring_quad_at[0].y = ring_quad_at[2].y = y + 48.0F;
    ring_quad_at[1].y = ring_quad_at[3].y = y + 80.0F;
    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    x = 64.0F;
    ring_quad_at[0].x = ring_quad_at[1].x = 12.0F;
    ring_quad_at[2].x = ring_quad_at[3].x = 44.0F;
    njSetTexture(&console_texlist);
    njDrawTexture(ring_quad_at, 4, TEXTURE_RING, 1);
    rings_number.pos.x = x;
    rings_number.value = game_state.rings[player];
    rings_number.pos.y += y - 16.0F;
    if (rings_number.value == 0) {
        rings_number.color = pulse | NUMBER_RED;
    } else {
        rings_number.color = NUMBER_WHITE;
    }
    hud_number_draw(&rings_number);
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
}

/* player's life icon and lives, at the bottom left of player's half of the
 * screen. a kind of -1 draws nothing. player's record must exist,
 * unchecked */
static void hud_draw_lives(int player)
{
    NJS_SPRITE sprite;
    int kind;
    Float x;
    struct hud_number *lives;

    kind = player_records[player]->kind;
    if (kind == -1) {
        return;
    }
    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    sprite.tanim = life_frames;
    sprite.tlist = life_icons[kind];
    /* @hack spelling: x set inside the sprite's x, where a statement of its
     * own would do */
    sprite.p.x = (x = player * PLAYER_HALF_WIDTH) + LIFE_ICON_X;
    sprite.p.y = BOTTOM_ROW_Y;
    sprite.sx = sprite.sy = 1.0F;
    njDrawSprite2D(&sprite, 0, SPRITE_PRIORITY, NJD_SPRITE_ALPHA);
    /* @hack spelling: the number through lives, where lives_number itself
     * would do */
    lives = &lives_number;
    if (kind == PLAYER_EGGMAN || kind == PLAYER_EGGMAN_MECH ||
        kind == PLAYER_KIND_BWALK) {
        njDrawSprite2D(&sprite, 1, SPRITE_PRIORITY, NJD_SPRITE_ALPHA);
        x += LIVES_X_TWO_PARTS;
    } else {
        x += LIVES_X;
    }
    lives->pos.x = x;
    lives->value = game_state.lives[player];
    hud_number_draw(&lives_number);
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
}

void hud_draw_lives_of(int player, int kind)
{
    NJS_SPRITE sprite;
    Float x;
    struct hud_number *lives;

    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    sprite.tanim = life_frames_of;
    sprite.tlist = life_icons[kind];
    /* @hack spelling: x set inside the sprite's x, as in hud_draw_lives */
    sprite.p.x = (x = player * PLAYER_HALF_WIDTH) + LIFE_ICON_X;
    sprite.p.y = BOTTOM_ROW_Y;
    sprite.sx = sprite.sy = 1.0F;
    njDrawSprite2D(&sprite, 0, SPRITE_PRIORITY, NJD_SPRITE_ALPHA);
    /* @hack spelling: the number through lives, as in hud_draw_lives */
    lives = &lives_number;
    if (kind == PLAYER_EGGMAN || kind == PLAYER_EGGMAN_MECH) {
        njDrawSprite2D(&sprite, 1, SPRITE_PRIORITY, NJD_SPRITE_ALPHA);
        x += LIVES_X_TWO_PARTS;
    } else {
        x += LIVES_X;
    }
    lives->pos.x = x;
    lives->value = game_state.lives[player];
    hud_number_draw(&lives_number);
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
}

/* player's lamps beside the rings, ROUNDS_TO_WIN of them, or one under
 * TWO_PLAYERS_1_POINT. a lamp is lit for each round player has won */
static void hud_draw_round_wins(int player)
{
    NJS_SPRITE sprite;
    int lamps;
    int i;

    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    sprite.tlist = &console_texlist;
    if (game_state.two_players == TWO_PLAYERS_1_POINT) {
        lamps = 1;
    } else {
        lamps = ROUNDS_TO_WIN;
    }
    for (i = 0; i < lamps; i++) {
        if (game_state.round_wins[player] > i) {
            /* @hack spelling: the lit frame as lamp_frames, and the unlit as
             * lamp_frames + 1, where &lamp_frames[0] and &lamp_frames[1] would
             * do */
            sprite.tanim = lamp_frames;
        } else {
            sprite.tanim = lamp_frames + 1;
        }
        sprite.p.x = lamp_x[player][i];
        sprite.p.y = BOTTOM_ROW_Y;
        sprite.sx = sprite.sy = 1.0F;
        njDrawSprite2D(&sprite, 0, SPRITE_PRIORITY, NJD_SPRITE_ALPHA);
    }
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
}

static void hud_draw_chao_key(void)
{
    NJS_SPRITE sprite;

    if (game_state.chao_key == 0) {
        return;
    }
    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    sprite.tlist = &console_texlist;
    sprite.tanim = chao_key_frame;
    sprite.p.x = 120.0F;
    sprite.p.y = 80.0F;
    sprite.sx = sprite.sy = 1.0F;
    njDrawSprite2D(&sprite, 0, SPRITE_PRIORITY, NJD_SPRITE_ALPHA);
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
}

void hud_draw(void)
{
    Uint8 phase;
    Uint32 pulse;
    int scoreless;

    phase = game_state.steps;
    if (game_state.hud_on == 0) {
        return;
    }
    njFogDisable();
    /* fold the low byte of game_state.steps into a phase from 0 to 63. the
     * phase rises over each quarter of the byte and falls over the next. a
     * count of 0 rings pulses with it from red to white and back */
    if (phase >= 192) {
        phase = 255 - phase;
    } else if (phase >= 128) {
        phase = phase - 128;
    } else if (phase >= 64) {
        phase = 127 - phase;
    }
    phase <<= 2;
    pulse = ((phase << 8) & 0xff00) | (phase & 0xff);
    scoreless =
        stage_current_id() > STAGE_STG_END || STAGE_VERSUS(stage_current_id());
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    if (game_state.two_players == 0) {
#ifdef NONMATCHING
        /* the port's 16:9 squeezes a one-player game's counts about the
         * screen's left edge, where they stand (engine/dc/port.h) */
        port_wide_anchor = WIDE_LEFT;
#endif
        hud_draw_score(scoreless);
        hud_draw_clock(scoreless);
        if (game_state.rings_shown == 1) {
            hud_draw_rings(0, pulse, scoreless);
        }
        if (game_state.lives_shown == 1) {
            hud_draw_lives(0);
        }
        if (scoreless == 0) {
            hud_draw_chao_key();
        }
#ifdef NONMATCHING
        /* this side puts the pivot back at the screen's middle */
        port_wide_anchor = WIDE_MIDDLE;
#endif
    } else {
        if (game_state.rings_shown == 1) {
            hud_draw_rings(0, pulse, RING_ROW_TOP);
            hud_draw_rings(1, pulse, RING_ROW_TOP);
        }
        hud_draw_round_wins(0);
        hud_draw_round_wins(1);
    }
    njFogEnable();
}

void hud_number_draw(struct hud_number *number)
{
    struct hud_number_cell cell;
    NJS_TEXTURE_VTX quad[4];
    int flags;
    int place;
    Uint32 color;
    Uint32 value;
    int rest;
    int digits;
    int hook;
    int digit;
    int first;
    int i;
    Float scale;
    Float z;
    Float width;
    Float dx;
    Float dy;

    flags = number->flags;
    place = number->place;
    color = number->color;
    value = number->value;
    if (flags & HUD_NUMBER_CLAMPED) {
        value = value > number->max ? number->max : value;
    }
    digits = 1;
    /* the row's digits, counted from max where it clamps */
    rest = (flags & HUD_NUMBER_CLAMPED) ? number->max : number->value;
    while (rest >= 10) {
        /* @hack redundant: a read of rest that never runs */
        if (0) {
            if (rest) {
            }
        }
        /* @hack redundant: the same read again, in a block of its own */
        if (0) {
            if (rest) {
            }
        }
        rest = rest / 10;
        digits++;
    }
    hook = flags & HUD_NUMBER_HOOK;
    if (hook) {
        digits = 2;
    }
    if (flags & HUD_NUMBER_SCALED) {
        scale = number->scale;
    } else {
        scale = 1.0F;
    }
    if (scale > 1.0F) {
        njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
    } else {
        njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    }
    if (flags & HUD_NUMBER_FRONT) {
        z = 1.0001F;
    } else {
        z = 0.90909F - ((place & HUD_NUMBER_DEPTH) * 0.00001F);
    }
    /* @hack spelling: width set in the test, ahead of its comma, where a
     * statement of its own would do */
    if ((width = scale * 32.0F), place & HUD_NUMBER_TOP) {
        dy = 0.0F;
    } else if (place & HUD_NUMBER_BOTTOM) {
        dy = -width;
    } else {
        dy = -(scale * 16.0F);
    }
    if (place & HUD_NUMBER_LEFT) {
        dx = (digits - 1) * scale * 32.0F;
    } else if (place & HUD_NUMBER_RIGHT) {
        dx = width;
    } else {
        dx = (digits - 1) * scale * 16.0F;
    }
    first = 1;
    digit_quad[0].x = digit_quad[1].x = number->pos.x + dx;
    /* @hack spelling: the digit's width as scale * 32.0F, where width would
     * do */
    digit_quad[2].x = digit_quad[3].x = number->pos.x + dx + (scale * 32.0F);
    digit_quad[0].y = digit_quad[2].y = number->pos.y + dy;
    digit_quad[1].y = digit_quad[3].y = number->pos.y + dy + (scale * 32.0F);
    digit_quad[0].z = z;
    digit_quad[1].z = z;
    digit_quad[2].z = z;
    digit_quad[3].z = z;
    digit_quad[0].col = color;
    digit_quad[1].col = color;
    digit_quad[2].col = color;
    digit_quad[3].col = color;
    njSetTexture(&console_texlist);
    while (digits > 0) {
        if (flags & HUD_NUMBER_COLONS) {
            digit = DIGIT_COLON;
        } else {
            digit = value % 10;
            value = value / 10;
        }
        if (first == 1) {
            if (hook) {
                /* @hack type: stage_hooks.number_cell called as
                 * number_cell_hook, where the member would do */
                number_cell_hook[0](&cell);
                njSetTexture(cell.texlist);
                quad[0].u = cell.u0;
                quad[0].v = cell.v0;
                quad[1].u = cell.u0;
                quad[1].v = cell.v1;
                quad[2].u = cell.u1;
                quad[2].v = cell.v0;
                quad[3].u = cell.u1;
                quad[3].v = cell.v1;
                for (i = 0; i < 4; i++) {
                    quad[i].x = digit_quad[i].x;
                    quad[i].y = digit_quad[i].y;
                    quad[i].z = digit_quad[i].z;
                    quad[i].col = digit_quad[i].col;
                }
                njDrawTexture(quad, 4, TEXTURE_HOOK, 1);
            } else {
                njDrawTexture(digit_quad, 4, digit + TEXTURE_DIGIT, 1);
            }
        }
        if ((flags & HUD_NUMBER_ZEROS) || value != 0 || hook) {
            first = 1;
        } else {
            first = 0;
        }
        digits--;
        /* @hack spelling: the first corner moved by width and the others by
         * scale * 32.0F, where width for all four would do */
        digit_quad[0].x -= width;
        digit_quad[1].x -= scale * 32.0F;
        digit_quad[2].x -= scale * 32.0F;
        digit_quad[3].x -= scale * 32.0F;
    }
    /* @hack redundant: a read of digit_quad that never runs */
    if (0) {
        /* NOLINTNEXTLINE(clang-diagnostic-pointer-bool-conversion) */
        if (digit_quad) {
        }
    }
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
}

static void score_window_step(struct task *task)
{
    struct frame_window *window = task->extra.p;
    Bool done;
    int i;

    switch (window->mode) {
    case WINDOW_WIDEN:
        done = TRUE;
        for (i = 0; i < 4; i++) {
            window->corners[i].x = float_approach(
                window->corners[i].x, window->targets[i].x, WINDOW_WIDEN_STEP);
            if (window->corners[i].x != window->targets[i].x) {
                done = FALSE;
            }
        }
        if (done == TRUE) {
            window->mode = WINDOW_HEIGHTEN;
        }
        break;
    case WINDOW_HEIGHTEN:
        done = TRUE;
        for (i = 0; i < 4; i++) {
            window->corners[i].y =
                float_approach(window->corners[i].y, window->targets[i].y,
                               WINDOW_HEIGHTEN_STEP);
            if (window->corners[i].y != window->targets[i].y) {
                done = FALSE;
            }
        }
        if (done == TRUE) {
            window->mode = WINDOW_OPENED;
        }
        break;
    case WINDOW_OPEN: {
        Float share = 1.0F / ((Float)WINDOW_OPEN_STEPS - window->count);

        for (i = 0; i < 4; i++) {
            window->corners[i].x +=
                (window->targets[i].x - window->corners[i].x) * share;
            window->corners[i].y +=
                (window->targets[i].y - window->corners[i].y) * share;
        }
    }
        if (window->count >= WINDOW_OPEN_STEPS - 1) {
            window->mode = WINDOW_OPENED;
        } else {
            window->count++;
        }
        break;
    case WINDOW_OPENED:
        break;
    }
}

/* a score window's corners, edges and middle, at WINDOW_PRIORITY. a window
 * whose task's unk_3c isn't 0 draws at WINDOW_PRIORITY_BEHIND, so that the
 * hud's numbers show over it. only the bosses' totals set unk_3c */
static void score_window_draw(struct task *task)
{
    struct frame_window *window = task->extra.p;
    NJS_TEXTURE_VTX quad[4];

    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njSetTexture(&result_texlist);
    if (task->unk_3c.l != 0) {
        quad[0].z = quad[1].z = quad[2].z = quad[3].z = WINDOW_PRIORITY_BEHIND;
    } else {
        quad[0].z = quad[1].z = quad[2].z = quad[3].z = WINDOW_PRIORITY;
    }
    quad[0].col = quad[1].col = quad[2].col = quad[3].col = 0xffffffff;

    quad[0].x = window->corners[0].x;
    quad[0].y = window->corners[0].y;
    quad[1].x = window->corners[0].x;
    quad[1].y = window->corners[0].y + WINDOW_BORDER;
    quad[2].x = window->corners[0].x + WINDOW_BORDER;
    quad[2].y = window->corners[0].y;
    quad[3].x = window->corners[0].x + WINDOW_BORDER;
    quad[3].y = window->corners[0].y + WINDOW_BORDER;
    quad[0].u = 0.0F;
    quad[0].v = 0.0F;
    quad[1].u = 0.0F;
    quad[1].v = 1.0F;
    quad[2].u = 1.0F;
    quad[2].v = 0.0F;
    quad[3].u = 1.0F;
    quad[3].v = 1.0F;
    njDrawTexture(quad, 4, TEXTURE_WINDOW_CORNER, 1);

    quad[0].y = window->corners[0].y + WINDOW_BORDER;
    quad[1].y = window->corners[1].y - WINDOW_BORDER;
    quad[2].y = window->corners[0].y + WINDOW_BORDER;
    quad[3].y = window->corners[1].y - WINDOW_BORDER;
    njDrawTexture(quad, 4, TEXTURE_WINDOW_EDGE, 1);

    quad[0].y = window->corners[1].y;
    quad[2].y = window->corners[1].y;
    njDrawTexture(quad, 4, TEXTURE_WINDOW_CORNER, 1);

    quad[0].x = window->corners[0].x + WINDOW_BORDER;
    quad[0].y = window->corners[0].y;
    quad[1].x = window->corners[0].x + WINDOW_BORDER;
    quad[1].y = window->corners[0].y + WINDOW_BORDER;
    quad[2].x = window->corners[2].x - WINDOW_BORDER;
    quad[2].y = window->corners[2].y;
    quad[3].x = window->corners[2].x - WINDOW_BORDER;
    quad[3].y = window->corners[2].y + WINDOW_BORDER;
    quad[0].u = 0.0F;
    quad[0].v = 0.0F;
    quad[1].u = 1.0F;
    quad[1].v = 0.0F;
    quad[2].u = 0.0F;
    quad[2].v = 1.0F;
    quad[3].u = 1.0F;
    quad[3].v = 1.0F;
    njDrawTexture(quad, 4, TEXTURE_WINDOW_EDGE, 1);

    quad[0].y = window->corners[0].y + WINDOW_BORDER;
    quad[1].y = window->corners[1].y - WINDOW_BORDER;
    quad[2].y = window->corners[2].y + WINDOW_BORDER;
    quad[3].y = window->corners[3].y - WINDOW_BORDER;
    njDrawTexture(quad, 4, TEXTURE_WINDOW_MIDDLE, 1);

    quad[0].y = window->corners[1].y;
    quad[2].y = window->corners[3].y;
    njDrawTexture(quad, 4, TEXTURE_WINDOW_EDGE, 1);

    quad[0].x = window->corners[2].x;
    quad[0].y = window->corners[2].y;
    quad[1].x = window->corners[2].x;
    quad[1].y = window->corners[2].y + WINDOW_BORDER;
    quad[2].x = window->corners[2].x - WINDOW_BORDER;
    quad[2].y = window->corners[2].y;
    quad[3].x = window->corners[2].x - WINDOW_BORDER;
    quad[3].y = window->corners[2].y + WINDOW_BORDER;
    quad[0].u = 0.0F;
    quad[0].v = 0.0F;
    quad[1].u = 0.0F;
    quad[1].v = 1.0F;
    quad[2].u = 1.0F;
    quad[2].v = 0.0F;
    quad[3].u = 1.0F;
    quad[3].v = 1.0F;
    njDrawTexture(quad, 4, TEXTURE_WINDOW_CORNER, 1);

    quad[0].y = window->corners[2].y + WINDOW_BORDER;
    quad[1].y = window->corners[3].y - WINDOW_BORDER;
    quad[2].y = window->corners[2].y + WINDOW_BORDER;
    quad[3].y = window->corners[3].y - WINDOW_BORDER;
    njDrawTexture(quad, 4, TEXTURE_WINDOW_EDGE, 1);

    quad[0].y = window->corners[3].y;
    quad[2].y = window->corners[3].y;
    njDrawTexture(quad, 4, TEXTURE_WINDOW_CORNER, 1);
    njFogEnable();
}

static void score_window_destroy(struct task *task)
{
}

/* start a window that opens from its middle to the rectangle from left to
 * right and top to bottom. it returns the window's task, or null where
 * task_create or syCalloc fails */
static struct task *score_window_open(Float right, Float left, Float bottom,
                                      Float top)
{
    struct task *task;
    struct frame_window *window;

    if ((task = task_create(0, WINDOW_LEVEL, score_window_step,
                            "ScoreWindow")) == NULL) {
        return NULL;
    }
    if ((window = syCalloc(1, sizeof(struct frame_window))) == NULL) {
        task_destroy(task);
        return NULL;
    }
    task->extra.p = window;
    window->mode = WINDOW_OPEN;
    window->targets[0].x = left;
    window->targets[0].y = top;
    window->targets[1].x = left;
    window->targets[1].y = bottom;
    window->targets[2].x = right;
    window->targets[2].y = top;
    window->targets[3].x = right;
    window->targets[3].y = bottom;
    window->corners[0].x = window->corners[1].x =
        ((right + left) * 0.5F) - WINDOW_BORDER;
    window->corners[2].x = window->corners[3].x =
        ((right + left) * 0.5F) + WINDOW_BORDER;
    window->corners[0].y = window->corners[2].y =
        ((bottom + top) * 0.5F) - WINDOW_BORDER;
    window->corners[1].y = window->corners[3].y =
        ((bottom + top) * 0.5F) + WINDOW_BORDER;
    task->draw = score_window_draw;
    task->destructor = score_window_destroy;
    return task;
}

/* set result's score, time bonus and rank, and game_state's rank. the rank
 * rises from RANK_E for each stage rank score that the score and the bonus
 * reach. all the rings make it RANK_A */
static void score_rank_set(struct result *result)
{
    Uint32 seconds;
    Uint32 total;
    Sint16 *entry;
    int stage;
    int i;

    result->score = game_state.score;
    seconds = (game_state.minute * SECONDS_PER_MINUTE) + game_state.second;
    if (seconds < TIME_BONUS_FREE) {
        seconds = TIME_BONUS_FREE;
    }
    result->time_bonus =
        TIME_BONUS - ((seconds - TIME_BONUS_FREE) * TIME_BONUS_LOSS);
    if (result->time_bonus > TIME_BONUS) {
        result->time_bonus = 0;
    }
    if (game_state.rings[0] >= game_state.ring_total) {
        result->all_rings = 1;
    } else {
        result->all_rings = 0;
    }
    total = result->time_bonus + result->score;
    if (result->all_rings == 1) {
        result->rank = RANK_A;
    } else {
        stage = stage_current_id();
        result->rank = RANK_E;
        total /= RANK_SCORE_UNIT;
        /* @hack type: the table walked as halfwords, a stage's and then its
         * scores, where a struct rank_scores pointer would do */
        if (game_state.mission == MISSION_HD) {
            entry = &rank_scores_hd.stages[0].stage;
        } else if (game_state.mission == MISSION_TIMED) {
            entry = &rank_scores_timed.stages[0].stage;
        } else {
            entry = &rank_scores_first.stages[0].stage;
        }
        for (; *entry != STAGE_NONE; entry += RANK_SCORES_HALFWORDS) {
            if (stage == *entry) {
                for (i = RANK_RISES; i > 0; i--) {
                    entry++;
                    /* @hack spelling: a continue after the rise, and a
                     * break after it, where a break for a score below the
                     * entry would do */
                    if (total >= (Uint32)*entry) {
                        result->rank++;
                        continue;
                    }
                    break;
                }
                break;
            }
        }
    }
    game_state.rank = result->rank;
    snapshot_current->rank = game_state.rank;
}

#ifdef NONMATCHING
/* A result has its draw callback only if it acquired the sheet. Its step
 * still tallies, saves and leaves the stage after a failed load. */
static void open_result_window(const struct task *task, Float right, Float left,
                               Float bottom, Float top)
{
    if (task->draw != NULL) {
        score_window_open(right, left, bottom, top);
    }
}

static void release_result_sheet(const struct task *task, NJS_TEXLIST *sheet)
{
    if (task->draw != NULL) {
        texture_discard_pvm(sheet);
    }
}
#define OPEN_RESULT_WINDOW open_result_window
#define RELEASE_RESULT_SHEET release_result_sheet
#else
#define OPEN_RESULT_WINDOW(task, right, left, bottom, top)                     \
    score_window_open(right, left, bottom, top)
#define RELEASE_RESULT_SHEET(task, sheet) njReleaseTexture(sheet)
#endif

static void score_result_step(struct task *task)
{
    struct result *result = task->extra.p;
    int voice;

    switch (result->mode) {
    case SCORE_RESULT_FADE:
        if (fade_to_alpha_step(FADE_WHITE) != 0) {
            result->mode = SCORE_RESULT_POSE;
            result->count = 0;
            if (stage_current_id() != STAGE_CART_1) {
                stage_pose_winner(result->player);
            }
        }
        break;
    case SCORE_RESULT_POSE:
        fade_to_alpha_step(0);
        if (result->count++ >= POSE_STEPS) {
            result->mode = SCORE_RESULT_DIM;
            result->count = 0;
            music_resume();
            music_play_next(RESULT_MUSIC);
            fade_set_color(0);
        }
        break;
    case SCORE_RESULT_DIM:
        if (fade_to_alpha_step(FADE_DIM) != 0) {
            score_rank_set(result);
            result->mode = SCORE_RESULT_TITLE;
            OPEN_RESULT_WINDOW(task, 540.0F, 100.0F, 232.0F, 96.0F);
        }
        break;
    case SCORE_RESULT_TITLE:
        if (result->count++ >= TITLE_STEPS) {
            result->mode = SCORE_RESULT_LABELS;
            result->count = 0;
        }
        break;
    case SCORE_RESULT_LABELS:
        result->mode = SCORE_RESULT_SCORE;
        if (game_state.two_players == 0) {
            vibration_start(0, RUMBLE_POWER, RUMBLE_FREQ, VIBRATION_UNTIMED);
        }
        break;
    case SCORE_RESULT_SCORE:
        if (result->count++ >= LINE_STEPS) {
            result->mode = SCORE_RESULT_TIME;
            result->count = 0;
            if (game_state.two_players == 0) {
                vibration_start(0, RUMBLE_POWER, RUMBLE_FREQ,
                                VIBRATION_UNTIMED);
            }
        } else if (result->count == LINE_SOUND_AT) {
            sound_play(SOUND_SHOW, NULL, 0, 0);
        }
        break;
    case SCORE_RESULT_TIME:
        if (result->count++ >= LINE_STEPS) {
            result->mode = SCORE_RESULT_RINGS;
            result->count = 0;
            if (game_state.two_players == 0) {
                vibration_start(0, RUMBLE_POWER, RUMBLE_FREQ,
                                VIBRATION_UNTIMED);
            }
        } else if (result->count == LINE_SOUND_AT) {
            sound_play(SOUND_SHOW, NULL, 0, 0);
        }
        break;
    case SCORE_RESULT_RINGS:
        if (result->count++ >= LINE_STEPS) {
            result->mode = SCORE_RESULT_ALL_RINGS;
            result->count = 0;
            sound_play(SOUND_SHOW, NULL, 0, 0);
            OPEN_RESULT_WINDOW(task, 540.0F, 100.0F, 336.0F, 248.0F);
        } else if (result->count == LINE_SOUND_AT) {
            sound_play(SOUND_SHOW, NULL, 0, 0);
        }
        break;
    case SCORE_RESULT_ALL_RINGS:
        if ((pad_press_repeat[0] & RESULT_HURRY_BUTTONS) &&
            result->count >= HURRY_AFTER) {
            result->count += HURRY_SKIP;
        }
        if (result->count++ >= ALL_RINGS_STEPS) {
            result->mode = SCORE_RESULT_BONUS_LABELS;
            result->count = 0;
        }
        break;
    case SCORE_RESULT_BONUS_LABELS:
        if (result->count++ >= BONUS_LABELS_STEPS) {
            result->mode = SCORE_RESULT_BONUS;
            result->count = 0;
            if (game_state.two_players == 0) {
                vibration_start(0, RUMBLE_POWER, RUMBLE_FREQ,
                                VIBRATION_UNTIMED);
            }
        }
        break;
    case SCORE_RESULT_BONUS:
        if (result->count++ >= LINE_STEPS) {
            result->mode = SCORE_RESULT_TOTAL;
            result->count = 0;
            if (game_state.two_players == 0) {
                vibration_start(0, RUMBLE_POWER, RUMBLE_FREQ,
                                VIBRATION_UNTIMED);
            }
        } else if (result->count == LINE_SOUND_AT) {
            sound_play(SOUND_SHOW, NULL, 0, 0);
        }
        break;
    case SCORE_RESULT_TOTAL:
        if (result->count++ >= LINE_STEPS) {
            result->mode = SCORE_RESULT_COUNT;
            result->count = 0;
        } else if (result->count == LINE_SOUND_AT) {
            sound_play(SOUND_SHOW, NULL, 0, 0);
        }
        break;
    case SCORE_RESULT_COUNT:
        if ((pad_press_repeat[0] & RESULT_HURRY_BUTTONS) &&
            result->count >= HURRY_AFTER) {
            result->score += result->time_bonus;
            result->time_bonus = 0;
        }
        /* @hack type: an unsigned bonus tested <= 0, where == 0 would do */
        if (result->time_bonus <= 0) {
            result->mode = SCORE_RESULT_COUNTED;
            result->count = 0;
            sound_play_for(SOUND_TALLY, result, 1, 0, 120);
            sound_set_volume(result, SOUND_TALLY, 127);
            save_record_stage(result->score);
            if ((game_state.card_warning = save_check(0)) == SAVE_OK) {
                save_write();
            }
        } else if (result->count++ >= TALLY_WAIT) {
            if (result->time_bonus > TALLY_STEP) {
                result->score += TALLY_STEP;
                result->time_bonus -= TALLY_STEP;
            } else {
                result->score += result->time_bonus;
                result->time_bonus = 0;
            }
            sound_start_for(SOUND_TALLY, task, 0, 0, 8);
        }
        break;
    case SCORE_RESULT_COUNTED:
        if (result->count++ >= COUNTED_STEPS) {
            result->mode = SCORE_RESULT_RANK;
            result->count = 0;
        }
        break;
    case SCORE_RESULT_RANK:
        if (result->count++ >= RANK_ZOOM_STEPS) {
            result->mode = SCORE_RESULT_RANK_SHOWN;
            result->count = 0;
        } else if (result->count == LINE_SOUND_AT) {
            sound_play(SOUND_RANK, NULL, 0, 0);
            if (game_state.two_players == 0) {
                vibration_start(0, RUMBLE_POWER, RUMBLE_FREQ,
                                VIBRATION_UNTIMED);
            }
        }
        break;
    case SCORE_RESULT_RANK_SHOWN:
        if (result->count++ >= RANK_SHOWN_STEPS) {
            result->mode = SCORE_RESULT_END;
            result->count = 0;
        } else if (result->count == RANK_VOICE_AT) {
            voice = -1;
            if (stage_current_id() != STAGE_CART_1) {
                switch (player_records[result->player]->kind) {
                case PLAYER_SONIC:
                    voice = score_rank_voices[0][game_state.rank];
                    break;
                case PLAYER_TAILS_MECH:
                    voice = score_rank_voices[1][game_state.rank];
                    break;
                case PLAYER_KNUCKLES:
                    voice = score_rank_voices[2][game_state.rank];
                    break;
                case PLAYER_EGGMAN_MECH:
                    voice = score_rank_voices[3][game_state.rank];
                    break;
                case PLAYER_SHADOW:
                    voice = score_rank_voices[4][game_state.rank];
                    break;
                case PLAYER_ROUGE:
                    voice = score_rank_voices[5][game_state.rank];
                    break;
                }
                if (voice != -1) {
                    voice_play_priority_2(voice);
                }
            }
        }
        break;
    case SCORE_RESULT_END:
        if (game_state.card_state == CARD_IDLE) {
            stage_end(STAGE_END_CLEAR);
            result->mode = SCORE_RESULT_ENDED;
        }
        break;
    case SCORE_RESULT_ENDED:
        /* @hack redundant: a case that does nothing */
        break;
    }
}

/* the score result's sheet, as far as its mode has come. a part zooms in
 * through zoom_scales and zoom_colors in its own mode */
static void score_result_draw(struct task *task)
{
    struct result *result = task->extra.p;
    int step;
    Float scale;
    Uint32 color;
    Angle angle;
    NJS_TEXTURE_VTX *quad;
    NJS_TEXTURE_VTX *corner;
    NJS_POINT3 points[4];
    struct hud_number *number;

    step = result->count;
    if (step >= ZOOM_STEPS) {
        step = ZOOM_STEPS - 1;
    }
    njFogDisable();
    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    if (result->mode >= SCORE_RESULT_TITLE) {
        njSetTexture(&result_texlist);
        if (stage_current_id() == STAGE_GREEN_HILL) {
            njDrawTexture(RESULT_QUAD(RESULT_TITLE_GREEN_HILL), 4,
                          TEXTURE_RESULT, 1);
        } else {
            njDrawTexture(RESULT_QUAD(game_state.mission + RESULT_MISSION_1), 4,
                          TEXTURE_RESULT, 1);
            njDrawTexture(RESULT_QUAD(game_state.mission + RESULT_MISSION_2), 4,
                          TEXTURE_RESULT, 1);
            njDrawTexture(RESULT_QUAD(game_state.mission + RESULT_MISSION_3), 4,
                          TEXTURE_RESULT, 1);
        }
    }
    if (result->mode >= SCORE_RESULT_LABELS) {
        njSetTexture(&result_texlist);
        njDrawTexture(RESULT_QUAD(RESULT_SCORE_LABEL), 4, TEXTURE_RESULT, 1);
        njDrawTexture(RESULT_QUAD(RESULT_TIME_LABEL), 4, TEXTURE_RESULT, 1);
        if (game_state.rings[0] <= 1) {
            njDrawTexture(RESULT_QUAD(RESULT_RING_LABEL), 4, TEXTURE_RESULT, 1);
        } else {
            njDrawTexture(RESULT_QUAD(RESULT_RINGS_LABEL), 4, TEXTURE_RESULT,
                          1);
        }
    }
    if (result->mode >= SCORE_RESULT_SCORE) {
        if (result->mode == SCORE_RESULT_SCORE) {
            scale = zoom_scales[step];
            color = zoom_colors[step];
        } else {
            scale = ZOOMED_SCALE;
            color = ZOOMED_COLOR;
        }
        /* @hack spelling: the score's number through number, where
         * result_score itself would do */
        number = &result_score;
        number->scale = scale;
        number->color = color;
        number->value = game_state.score;
        hud_number_draw(&result_score);
    }
    if (result->mode >= SCORE_RESULT_TIME) {
        if (result->mode == SCORE_RESULT_TIME) {
            scale = zoom_scales[step];
            color = zoom_colors[step];
        } else {
            scale = ZOOMED_SCALE;
            color = ZOOMED_COLOR;
        }
        result_minutes.scale = scale;
        result_colon_1.scale = scale;
        result_seconds.scale = scale;
        result_colon_2.scale = scale;
        result_hundredths.scale = scale;
        result_minutes.color = color;
        result_colon_1.color = color;
        result_seconds.color = color;
        result_colon_2.color = color;
        result_hundredths.color = color;
        result_minutes.value = game_state.minute;
        result_seconds.value = game_state.second;
        result_hundredths.value =
            njRoundUp(game_state.frame * HUNDREDTHS_PER_FRAME);
        hud_number_draw(&result_minutes);
        hud_number_draw(&result_colon_1);
        hud_number_draw(&result_seconds);
        hud_number_draw(&result_colon_2);
        hud_number_draw(&result_hundredths);
    }
    if (result->mode >= SCORE_RESULT_RINGS) {
        if (result->mode == SCORE_RESULT_RINGS) {
            scale = zoom_scales[step];
            color = zoom_colors[step];
        } else {
            scale = ZOOMED_SCALE;
            color = ZOOMED_COLOR;
        }
        result_rings.scale = scale;
        result_rings.color = color;
        result_ring_total.scale = scale;
        result_ring_total.color = color;
        result_rings.value = game_state.rings[0];
        hud_number_draw(&result_rings);
        result_ring_total.value = game_state.ring_total;
        hud_number_draw(&result_ring_total);
        njSetTexture(&result_texlist);
        njDrawTexture(RESULT_QUAD(RESULT_RINGS_OF), 4, TEXTURE_RESULT, 1);
    }
    if (result->mode >= SCORE_RESULT_ALL_RINGS) {
        if (result->all_rings == 1) {
            njSetTexture(&result_texlist);
            njDrawTexture(RESULT_QUAD(RESULT_ALL_RINGS), 4, TEXTURE_RESULT, 1);
        }
    }
    if (result->mode >= SCORE_RESULT_BONUS_LABELS) {
        njSetTexture(&result_texlist);
        njDrawTexture(RESULT_QUAD(RESULT_BONUS_LABEL), 4, TEXTURE_RESULT, 1);
        njDrawTexture(RESULT_QUAD(RESULT_BONUS_LABEL + 1), 4, TEXTURE_RESULT,
                      1);
        njDrawTexture(RESULT_QUAD(RESULT_TOTAL_LABEL), 4, TEXTURE_RESULT, 1);
        njDrawTexture(RESULT_QUAD(RESULT_TOTAL_LABEL + 1), 4, TEXTURE_RESULT,
                      1);
    }
    if (result->mode >= SCORE_RESULT_BONUS) {
        if (result->mode == SCORE_RESULT_BONUS) {
            scale = zoom_scales[step];
            color = zoom_colors[step];
        } else {
            scale = ZOOMED_SCALE;
            color = ZOOMED_COLOR;
        }
        result_bonus[0].scale = scale;
        result_bonus[0].color = color;
        result_bonus[0].value = result->time_bonus;
        hud_number_draw(result_bonus);
    }
    if (result->mode >= SCORE_RESULT_TOTAL) {
        if (result->mode == SCORE_RESULT_TOTAL) {
            scale = zoom_scales[step];
            color = zoom_colors[step];
        } else {
            scale = ZOOMED_SCALE;
            color = ZOOMED_COLOR;
        }
        result_total.scale = scale;
        result_total.color = color;
        result_total.value = result->score;
        hud_number_draw(&result_total);
    }
    if (result->mode >= SCORE_RESULT_RANK) {
        quad = RESULT_QUAD((RESULT_RANK_A + RANK_A) - result->rank);
        njSetTexture(&result_texlist);
        if (result->mode == SCORE_RESULT_RANK) {
            scale = score_rank_scales[step];
            color = zoom_colors[step];
            angle = score_rank_turns[step];
        } else {
            color = ZOOMED_COLOR;
            scale = 1.0F;
            angle = 0;
        }
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&score_rank_place);
        njRotateZ(NULL, angle);
        njScale(NULL, scale, scale, scale);
        njCalcPoints(NULL, score_rank_corners, points, 4);
        njPopMatrixEx();
        quad[0].x = points[0].x;
        quad[0].y = points[0].y;
        quad[0].z = points[0].z;
        quad[1].x = points[1].x;
        quad[1].y = points[1].y;
        quad[1].z = points[1].z;
        quad[2].x = points[2].x;
        quad[2].y = points[2].y;
        quad[2].z = points[2].z;
        /* @hack spelling: the last corner through corner, where quad[3]
         * would do */
        corner = &quad[3];
        corner->x = points[3].x;
        corner->y = points[3].y;
        corner->z = points[3].z;
        quad[0].col = quad[1].col = quad[2].col = corner->col = color;
        njDrawTexture(quad, 4, TEXTURE_RESULT, 1);
    }
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
    njFogEnable();
}

/* release the result sheet. a result destroyed in any mode but
 * SCORE_RESULT_END sets game_state.idle_dim back to 1 */
static void score_result_destroy(struct task *task)
{
    struct result *result = task->extra.p;

    /* @hack spelling: an empty arm for SCORE_RESULT_END, where a test of
     * mode != SCORE_RESULT_END would do */
    if (result->mode == SCORE_RESULT_END) {
    } else {
        game_state.idle_dim = 1;
    }
    RELEASE_RESULT_SHEET(task, &result_texlist);
}

/* set result's rank and game_state's from the clock, rounded up to the
 * second. the rank rises from RANK_E for each of the stage's rank times the
 * clock is within */
static void time_rank_set(struct result *result)
{
    int stage;
    int time;
    Sint8 *entry;
    int limit;
    int i;

    stage = stage_current_id();
    time = (game_state.minute * SECONDS_PER_MINUTE) + game_state.second;
    if (game_state.frame > 0) {
        time++;
    }
    result->rank = RANK_E;
    /* @hack type: the table walked as bytes, a stage's and then each
     * minute and second, where a struct rank_times pointer would do */
    if (game_state.mission == MISSION_RINGS) {
        entry = &rank_times_rings.stages[0].stage;
    } else {
        entry = &rank_times_chao.stages[0].stage;
    }
    for (; *entry != STAGE_NONE; entry += sizeof(struct rank_times)) {
        if (stage == *entry) {
            entry++;
            for (i = RANK_RISES; i > 0; i--) {
                limit = *entry++ * SECONDS_PER_MINUTE;
                limit += *entry++;
                /* @hack spelling: a continue after the rise, as in
                 * score_rank_set */
                if (time <= limit) {
                    result->rank++;
                    continue;
                }
                break;
            }
            break;
        }
    }
    game_state.rank = result->rank;
    snapshot_current->rank = game_state.rank;
}

/* time_rank_set for STAGE_CART_1, from its table for Rouge or for the
 * others, by mission */
static void cart_rank_set(struct result *result)
{
    int time;
    Sint8 *entry;
    int limit;
    int i;

    time = (game_state.minute * SECONDS_PER_MINUTE) + game_state.second;
    if (game_state.frame > 0) {
        time++;
    }
    result->rank = RANK_E;
    /* @hack type: the table walked as bytes, as in time_rank_set */
    if (stage_character(0) == PLAYER_ROUGE) {
        entry = (Sint8 *)rank_times_cart_rouge;
    } else {
        entry = (Sint8 *)rank_times_cart;
    }
    entry += game_state.mission * (int)sizeof(struct rank_times);
    entry++;
    for (i = RANK_RISES; i > 0; i--) {
        limit = *entry++ * SECONDS_PER_MINUTE;
        limit += *entry++;
        /* @hack spelling: a continue after the rise, as in
         * score_rank_set */
        if (time <= limit) {
            result->rank++;
            continue;
        }
        break;
    }
    game_state.rank = result->rank;
    snapshot_current->rank = game_state.rank;
}

static void time_result_step(struct task *task)
{
    struct result *result = task->extra.p;
    int voice;

    switch (result->mode) {
    case TIME_RESULT_FADE:
        if (fade_to_alpha_step(FADE_WHITE) != 0) {
            result->mode = TIME_RESULT_POSE;
            result->count = 0;
            stage_pose_winner(result->player);
        }
        break;
    case TIME_RESULT_POSE:
        fade_to_alpha_step(0);
        if (result->count++ >= POSE_STEPS) {
            result->mode = TIME_RESULT_DIM;
            result->count = 0;
            music_resume();
            music_play_next(RESULT_MUSIC);
            fade_set_color(0);
        }
        break;
    case TIME_RESULT_DIM:
        if (fade_to_alpha_step(FADE_DIM) != 0) {
            if (stage_current_id() == STAGE_CART_1) {
                cart_rank_set(result);
            } else {
                time_rank_set(result);
            }
            result->mode = TIME_RESULT_TITLE;
            OPEN_RESULT_WINDOW(task, 540.0F, 100.0F, 184.0F, 136.0F);
        }
        break;
    case TIME_RESULT_TITLE:
        if (result->count++ >= TITLE_STEPS) {
            result->mode = TIME_RESULT_LABEL;
            result->count = 0;
        }
        break;
    case TIME_RESULT_LABEL:
        result->mode = TIME_RESULT_TIME;
        if (game_state.two_players == 0) {
            vibration_start(0, RUMBLE_POWER, RUMBLE_FREQ, VIBRATION_UNTIMED);
        }
        break;
    case TIME_RESULT_TIME:
        if (result->count++ >= LINE_STEPS) {
            result->mode = TIME_RESULT_RANK;
            result->count = 0;
        } else if (result->count == LINE_SOUND_AT) {
            sound_play(SOUND_SHOW, NULL, 0, 0);
        }
        break;
    case TIME_RESULT_RANK:
        if (result->count++ >= RANK_ZOOM_STEPS) {
            result->mode = TIME_RESULT_RANK_SHOWN;
            result->count = 0;
        } else if (result->count == LINE_SOUND_AT) {
            sound_play(SOUND_RANK, NULL, 0, 0);
            if (game_state.two_players == 0) {
                vibration_start(0, RUMBLE_POWER, RUMBLE_FREQ,
                                VIBRATION_UNTIMED);
            }
        }
        break;
    case TIME_RESULT_RANK_SHOWN:
        if (result->count++ >= RANK_SHOWN_STEPS) {
            result->mode = TIME_RESULT_END;
            result->count = 0;
            save_record_stage(0);
            if ((game_state.card_warning = save_check(0)) == SAVE_OK) {
                save_write();
            }
        } else if (result->count == RANK_VOICE_AT &&
                   stage_current_id() != STAGE_CART_1) {
            voice = -1;
            switch (player_records[result->player]->kind) {
            case PLAYER_SONIC:
                voice = time_rank_voices[0][game_state.rank];
                break;
            case PLAYER_TAILS_MECH:
                voice = time_rank_voices[1][game_state.rank];
                break;
            case PLAYER_KNUCKLES:
                voice = time_rank_voices[2][game_state.rank];
                break;
            case PLAYER_EGGMAN_MECH:
                voice = time_rank_voices[3][game_state.rank];
                break;
            case PLAYER_SHADOW:
                voice = time_rank_voices[4][game_state.rank];
                break;
            case PLAYER_ROUGE:
                voice = time_rank_voices[5][game_state.rank];
                break;
            }
            if (voice != -1) {
                voice_play_priority_2(voice);
            }
        }
        break;
    case TIME_RESULT_END:
        if (game_state.card_state == CARD_IDLE) {
            stage_end(STAGE_END_CLEAR);
            result->mode = TIME_RESULT_ENDED;
        }
        break;
    case TIME_RESULT_ENDED:
        /* @hack redundant: a case that does nothing */
        break;
    }
}

/* the time result's sheet, as score_result_draw's */
static void time_result_draw(struct task *task)
{
    struct result *result = task->extra.p;
    int step;
    Float scale;
    Uint32 color;
    Angle angle;
    NJS_TEXTURE_VTX *quad;
    NJS_TEXTURE_VTX *corner;
    NJS_POINT3 points[4];

    step = result->count;
    if (step >= ZOOM_STEPS) {
        step = ZOOM_STEPS - 1;
    }
    njFogDisable();
    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    if (result->mode >= TIME_RESULT_TITLE) {
        njSetTexture(&result_texlist);
        njDrawTexture(RESULT_QUAD(game_state.mission + RESULT_MISSION_1), 4,
                      TEXTURE_RESULT, 1);
        njDrawTexture(RESULT_QUAD(game_state.mission + RESULT_MISSION_2), 4,
                      TEXTURE_RESULT, 1);
        njDrawTexture(RESULT_QUAD(game_state.mission + RESULT_MISSION_3), 4,
                      TEXTURE_RESULT, 1);
    }
    if (result->mode >= TIME_RESULT_LABEL) {
        njSetTexture(&result_texlist);
        /* @hack spelling: the label through quad, where time_label_quad
         * itself would do */
        quad = time_label_quad;
        njDrawTexture(quad, 4, TEXTURE_RESULT, 1);
    }
    if (result->mode >= TIME_RESULT_TIME) {
        if (result->mode == TIME_RESULT_TIME) {
            scale = zoom_scales[step];
            color = zoom_colors[step];
        } else {
            color = ZOOMED_COLOR;
            scale = ZOOMED_SCALE;
        }
        time_minutes.scale = scale;
        time_colon_1.scale = scale;
        time_seconds.scale = scale;
        time_colon_2.scale = scale;
        time_hundredths.scale = scale;
        time_minutes.color = color;
        time_colon_1.color = color;
        time_seconds.color = color;
        time_colon_2.color = color;
        time_hundredths.color = color;
        time_minutes.value = game_state.minute;
        time_seconds.value = game_state.second;
        time_hundredths.value =
            njRoundUp(game_state.frame * HUNDREDTHS_PER_FRAME);
        hud_number_draw(&time_minutes);
        hud_number_draw(&time_colon_1);
        hud_number_draw(&time_seconds);
        hud_number_draw(&time_colon_2);
        hud_number_draw(&time_hundredths);
    }
    if (result->mode >= TIME_RESULT_RANK) {
        quad = RESULT_QUAD((RESULT_RANK_A + RANK_A) - result->rank);
        njSetTexture(&result_texlist);
        if (result->mode == TIME_RESULT_RANK) {
            scale = time_rank_scales[step];
            color = zoom_colors[step];
            angle = time_rank_turns[step];
        } else {
            color = ZOOMED_COLOR;
            scale = 1.0F;
            angle = 0;
        }
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&time_rank_place);
        njRotateZ(NULL, angle);
        njScale(NULL, scale, scale, scale);
        njCalcPoints(NULL, time_rank_corners, points, 4);
        njPopMatrixEx();
        /* @hack spelling: the first corner through corner, where quad[0]
         * would do */
        corner = &quad[0];
        corner->x = points[0].x;
        corner->y = points[0].y;
        corner->z = points[0].z;
        quad[1].x = points[1].x;
        quad[1].y = points[1].y;
        quad[1].z = points[1].z;
        quad[2].x = points[2].x;
        quad[2].y = points[2].y;
        quad[2].z = points[2].z;
        quad[3].x = points[3].x;
        quad[3].y = points[3].y;
        quad[3].z = points[3].z;
        corner->col = quad[1].col = quad[2].col = quad[3].col = color;
        njDrawTexture(quad, 4, TEXTURE_RESULT, 1);
    }
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
    njFogEnable();
}

/* as score_result_destroy, for TIME_RESULT_END */
static void time_result_destroy(struct task *task)
{
    struct result *result = task->extra.p;

    /* @hack spelling: an empty arm for TIME_RESULT_END, as in
     * score_result_destroy */
    if (result->mode == TIME_RESULT_END) {
    } else {
        game_state.idle_dim = 1;
    }
    RELEASE_RESULT_SHEET(task, &result_texlist);
}

/* the result without a total: the winner's pose, and a save in the three
 * stories' sequences. it ends play once the bosses' totals no longer show */
static void plain_result_step(struct task *task)
{
    struct result *result = task->extra.p;

    switch (result->mode) {
    case SCORE_RESULT_FADE:
        if (fade_to_alpha_step(FADE_WHITE) != 0) {
            result->mode = SCORE_RESULT_POSE;
            result->count = 0;
            stage_pose_winner(result->player);
            music_resume();
            music_play_next(RESULT_MUSIC);
            switch (game_flags[FLAG_SEQUENCE]) {
            case SEQUENCE_HERO:
            case SEQUENCE_DARK:
            case SEQUENCE_LAST:
                if ((game_state.card_warning = save_check(0)) == SAVE_OK) {
                    save_write();
                }
                break;
            }
        }
        break;
    case SCORE_RESULT_POSE:
        fade_to_alpha_step(0);
        if (result->count++ >= LONG_POSE_STEPS && boss_total_showing == FALSE) {
            result->mode = SCORE_RESULT_END;
            result->count = 0;
            fade_set_color(0);
        }
        break;
    case SCORE_RESULT_END:
        if (game_state.card_state == CARD_IDLE) {
            stage_end(STAGE_END_CLEAR);
            task_destroy(task);
        }
        break;
    }
}

static void plain_result_draw(struct task *task)
{
}

static void plain_result_destroy(struct task *task)
{
}

static void winner_step(struct task *task)
{
    struct winner *winner = task->extra.p;

    switch (winner->mode) {
    case WINNER_FADE:
        if (fade_to_alpha_step(FADE_WHITE) != 0) {
            winner->mode = WINNER_POSE;
            winner->count = 0;
            stage_pose_winner(winner->player);
            music_resume();
            music_play_next(RESULT_MUSIC);
        }
        break;
    case WINNER_POSE:
        fade_to_alpha_step(0);
        /* @hack redundant: a read of winner that never runs */
        if (0) {
            if (winner) {
            }
        }
        if (winner->count++ >= LONG_POSE_STEPS) {
            if (game_state.two_players == TWO_PLAYERS_1_POINT ||
                game_state.round_wins[0] >= ROUNDS_TO_WIN ||
                game_state.round_wins[1] >= ROUNDS_TO_WIN) {
                winner->mode = WINNER_CHOOSE;
            } else {
                winner->mode = WINNER_END;
                winner->choice = WINNER_LEAVE;
            }
            winner->count = 0;
            fade_set_color(0);
        }
        break;
    case WINNER_CHOOSE:
        if (winner->count++ >= WINNER_CHOOSE_STEPS) {
            winner->mode = WINNER_END;
            winner->count = 0;
        } else {
            Uint32 *buttons = &pad_choose_buttons;
            Uint32 *pressed = pad_press_repeat;

            /* @hack type: the second test reads the buttons as Sint32, where
             * Uint32 would do */
            if ((pressed[0] & *buttons) || (pressed[1] & *(Sint32 *)buttons)) {
                if (winner->choice == WINNER_LEAVE) {
                    winner->mode = WINNER_END;
                } else {
                    winner->mode = WINNER_PLAY_AGAIN;
                }
                winner->count = 0;
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                /* @hack redundant: a break out of the case, where the end of
                 * the if would do */
                break;
            } else if ((pressed[0] & (PDD_DGT_KL | PDD_DGT_KR)) ||
                       (pressed[1] & (PDD_DGT_KL | PDD_DGT_KR))) {
                winner->choice ^= 1;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        }
        break;
    case WINNER_END:
        stage_end(STAGE_END_CLEAR);
        task_destroy(task);
        break;
    case WINNER_PLAY_AGAIN:
        if (winner->count++ > WINNER_AGAIN_STEPS) {
            stage_end(STAGE_END_RETRY);
            game_state.round_wins[0] = 0;
            game_state.round_wins[1] = 0;
            game_state.round_winner = -1;
            task_destroy(task);
        }
        break;
    }
}

/* the winner's banner while the winner poses. then the choice, with the one
 * the pads are on lit. once the players choose to play again, that choice
 * flashes until play ends */
static void winner_draw(struct task *task)
{
    struct winner *winner = task->extra.p;
    Int texture;

    njFogDisable();
    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njSetTexture(&game2p_texlist);
    njSetTextureNum(0);
    texture = njGetTextureNumG();
    if (winner->mode == WINNER_POSE) {
        njDrawTexture(WINNER_QUAD(WINNER_BANNER), 4, texture, 1);
    } else if (winner->mode == WINNER_CHOOSE ||
               winner->mode == WINNER_PLAY_AGAIN) {
        njDrawTexture(WINNER_QUAD(WINNER_CONTINUE), 4, texture, 1);
        if (winner->mode == WINNER_PLAY_AGAIN) {
            if (winner->count & 2) {
                njDrawTexture(WINNER_QUAD(WINNER_AGAIN_LIT), 4, texture, 1);
            }
        } else {
            if (winner->choice == WINNER_AGAIN) {
                njDrawTexture(WINNER_QUAD(WINNER_AGAIN_LIT), 4, texture, 1);
                njDrawTexture(WINNER_QUAD(WINNER_LEAVE_DIM), 4, texture, 1);
            } else {
                njDrawTexture(WINNER_QUAD(WINNER_AGAIN_DIM), 4, texture, 1);
                njDrawTexture(WINNER_QUAD(WINNER_LEAVE_LIT), 4, texture, 1);
            }
            njDrawTexture(WINNER_QUAD(WINNER_BETWEEN), 4, texture, 1);
        }
    }
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
    njFogEnable();
}

static void winner_destroy(struct task *task)
{
    RELEASE_RESULT_SHEET(task, &game2p_texlist);
}

void results_start(int player)
{
    struct task *task;
    char *name;
    NJS_TEXLIST *texlist;
    struct result *result;
    /* @hack redundant: a register local, where a plain one would do */
    register int stage_end_from;
    struct task *(*create)(Uint16, int, void (*)(struct task *), const char *);
    /* @hack redundant: a register local, where a plain one would do */
    register int (*current)(void);
#ifdef NONMATCHING
    /* texture_load_pvm returns on this side whether the textures loaded */
    Bool (*load)(const char *, NJS_TEXLIST *);
#else
    void (*load)(const char *, NJS_TEXLIST *);
#endif
    void (*destroy)(struct task *);
    void *(*allocate)(Uint32, Uint32);

    /* @hack spelling: the five callees and the first stage without a total
     * held in locals, where calls by name and STAGE_STG_END would do */
    load = texture_load_pvm;
    destroy = task_destroy;
    allocate = syCalloc;
    create = task_create;
    current = stage_current_id;
    music_stop();
    music_pause();
    fade_set_color(0x00ffffff);
    stage_end_from = STAGE_STG_END;
    game_state.idle_dim = 0;
    /* @hack redundant: a call of allocate that never runs */
    if (0) {
        allocate(1, sizeof(struct result));
    }
    /* @hack spelling: two_players read as a byte of game_state's, where the
     * field would do */
    if (((Sint8 *)&game_state)[0x1e] != 0) {
        /* @hack redundant: winner a register local, where a plain one would
         * do */
        register struct winner *winner;

        if ((task = create(0, RESULT_LEVEL, winner_step,
                           "DispWinnerAndContinue")) == NULL) {
            return;
        }
        /* the winner's record takes the room of a struct result, which is
         * larger */
        if ((winner = allocate(1, sizeof(struct result))) == NULL) {
            destroy(task);
            return;
        }
        task->extra.p = winner;
        task->draw = winner_draw;
        task->destructor = winner_destroy;
        winner->player = player;
        texlist = &game2p_texlist;
        name = WINNER_TEXTURE_ARCHIVE;
        goto load_texture;
    }
    if (current() < stage_end_from) {
        switch (current()) {
        case STAGE_VERSUS_SONIC_1:
        case STAGE_VERSUS_SONIC_2:
        case STAGE_VERSUS_MECH_1:
        case STAGE_VERSUS_MECH_2:
        case STAGE_VERSUS_KNUCKLES:
            goto no_total;
        }
    } else if (current() != STAGE_CART_1) {
    no_total:
        if ((task = create(0, RESULT_LEVEL, plain_result_step,
                           "NotCalcTotalScore")) == NULL) {
            return;
        }
        /* @hack redundant: a call of current that never runs */
        if (0) {
            current();
        }
        if ((result = allocate(1, sizeof(struct result))) == NULL) {
            destroy(task);
            return;
        }
        task->extra.p = result;
        task->draw = plain_result_draw;
        task->destructor = plain_result_destroy;
        result->player = player;
        return;
    }
    if (current() == STAGE_CART_1) {
        if ((task = create(0, RESULT_LEVEL, time_result_step,
                           "CalcTotalTime")) == NULL) {
            return;
        }
        /* @hack redundant: a call of current that never runs */
        if (0) {
            current();
        }
        if ((result = allocate(1, sizeof(struct result))) == NULL) {
            destroy(task);
            return;
        }
        task->extra.p = result;
        task->draw = time_result_draw;
        task->destructor = time_result_destroy;
        result->player = player;
        texlist = &result_texlist;
        name = RESULT_TEXTURE_ARCHIVE;
    load_texture:
#ifdef NONMATCHING
        /* this side draws no result whose textures didn't load */
        if (!load(name, texlist)) {
            task->draw = NULL;
        }
#else
        load(name, texlist);
#endif
        /* @hack redundant: a read of task that never runs */
        if (0) {
            if (task) {
            }
        }
        return;
    }
    switch (game_state.mission) {
    case MISSION_FIRST:
    case MISSION_TIMED:
    case MISSION_HD:
        if ((task = create(0, RESULT_LEVEL, score_result_step,
                           "CalcTotalScore")) == NULL) {
            return;
        }
        /* @hack redundant: a read of current that never runs */
        if (0) {
            if (current) {
            }
        }
        if ((result = allocate(1, sizeof(struct result))) == NULL) {
            destroy(task);
            return;
        }
        task->extra.p = result;
        task->draw = score_result_draw;
        task->destructor = score_result_destroy;
        break;
    case MISSION_RINGS:
    case MISSION_CHAO:
        if ((task = create(0, RESULT_LEVEL, time_result_step,
                           "CalcTotalTime")) == NULL) {
            return;
        }
        /* @hack redundant: a call of current that never runs */
        if (0) {
            current();
        }
        if ((result = allocate(1, sizeof(struct result))) == NULL) {
            destroy(task);
            return;
        }
        task->extra.p = result;
        task->draw = time_result_draw;
        task->destructor = time_result_destroy;
        break;
    default:
        return;
    }
    result->player = player;
#ifdef NONMATCHING
    /* this side draws no result whose textures didn't load */
    if (!load(RESULT_TEXTURE_ARCHIVE, &result_texlist)) {
        task->draw = NULL;
    }
#else
    load(RESULT_TEXTURE_ARCHIVE, &result_texlist);
#endif
}

/* the time left to the limit, from the clock. it is red at 0:00:00, and red
 * with a pulse of green before that. once LAST_SECONDS or fewer are left, the
 * green pulses every 16 steps. in the last minute it pulses every 32, and
 * before that every 128, by hud_draw's phase. it draws under HUD_NUMBER_FRONT
 * while game_state.idle_dim is 0 */
#define LAST_SECONDS 15
static void countdown_draw(struct task *task)
{
    struct countdown *limit;
    struct countdown *parts;
    int minute;
    int second;
    int frame;
    Uint8 phase;
    Uint8 level;
    Uint32 color;
    Uint32 green;
    Uint32 hundredths;

    limit = task->extra.p;
    if (game_state.mini_event_running != 0) {
        return;
    }
    /* @hack spelling: parts, a second pointer to limit's record, where
     * limit itself would do */
    parts = (struct countdown *)&limit->minute;
    minute = parts->minute - game_state.minute;
    second = parts->second - game_state.second;
    /* @hack redundant: a read of limit that never runs */
    if (0) {
        if (limit) {
        }
    }
    frame = STEPS_PER_SECOND - game_state.frame;
    if (frame < STEPS_PER_SECOND && frame > 0) {
        second = second - 1;
    }
    if (second < 0) {
        second += SECONDS_PER_MINUTE;
        minute--;
    }
    if (minute < 0) {
        minute = 0;
        second = 0;
        frame = 0;
    }
    if (game_state.unk_0b == 0) {
        return;
    }
    phase = game_state.steps;
    njFogDisable();
    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njSetTexture(&console_texlist);
    if (minute == 0 && second == 0 && frame == 0) {
        color = NUMBER_RED;
    } else if (minute == 0 && second <= LAST_SECONDS) {
        /* @hack spelling: phase - (phase & 0xf0), where phase & 0x0f would
         * do */
        if (phase & 8) {
            phase = 15 - (phase - (phase & 0xf0));
        } else {
            phase = (phase - (phase & 0xf0));
        }
        phase = phase << 4;
        green = 0xff00 & (phase << 8);
        color = green | NUMBER_RED;
    } else if (minute <= 0) {
        /* @hack spelling: phase - (phase & 0xe0), where phase & 0x1f would
         * do */
        if (phase & 16) {
            phase = 31 - (phase - (phase & 0xe0));
        } else {
            phase = (phase - (phase & 0xe0));
        }
        phase = phase << 3;
        green = 0xff00 & (phase << 8);
        color = green | NUMBER_RED;
    } else {
        if (phase >= 0xc0) {
            phase = 0xff - phase;
        } else if (phase >= 0x80) {
            phase += 0x80;
        } else if (phase >= 0x40) {
            phase = 0x7f - phase;
        }
        level = (phase << 1) + 0x80;
        green = 0xff00 & (level << 8);
        color = green | NUMBER_RED;
    }
    countdown_minutes.color = color;
    countdown_colon_1.color = color;
    countdown_seconds.color = color;
    countdown_colon_2.color = color;
    countdown_hundredths.color = color;
    if (game_state.idle_dim == 0) {
        countdown_minutes.flags = NUMBER_ZEROED_FRONT;
        countdown_colon_1.flags = NUMBER_COLON_FRONT;
        countdown_seconds.flags = NUMBER_ZEROED_FRONT;
        countdown_colon_2.flags = NUMBER_COLON_FRONT;
        countdown_hundredths.flags = NUMBER_ZEROED_FRONT;
    } else {
        countdown_minutes.flags = NUMBER_ZEROED;
        countdown_colon_1.flags = NUMBER_COLON;
        countdown_seconds.flags = NUMBER_ZEROED;
        countdown_colon_2.flags = NUMBER_COLON;
        countdown_hundredths.flags = NUMBER_ZEROED;
    }
    countdown_minutes.value = minute;
    countdown_seconds.value = second;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((hundredths = HUNDREDTHS_PER_SECOND -
                      (Uint32)njRoundUp((STEPS_PER_SECOND - frame) *
                                        HUNDREDTHS_PER_FRAME)) >=
        HUNDREDTHS_PER_SECOND) {
        hundredths = 0;
    }
    countdown_hundredths.value = hundredths;
    hud_number_draw(&countdown_minutes);
    hud_number_draw(&countdown_colon_1);
    hud_number_draw(&countdown_seconds);
    hud_number_draw(&countdown_colon_2);
    hud_number_draw(&countdown_hundredths);
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
    njFogEnable();
}

/* end play once the clock reaches the limit, unless the stage is cleared.
 * the task then idles */
static void countdown_step(struct task *task)
{
    struct countdown *limit = task->extra.p;
    struct countdown *parts;
    Sint8 minute;
    Sint8 second;

    if (game_state.cleared == 1) {
        task->step = task_idle;
        task->draw = NULL;
        return;
    }
    /* @hack spelling: parts, a second pointer to limit's record, where
     * limit itself would do */
    parts = (struct countdown *)&limit->minute;
    minute = parts->minute;
    second = parts->second;
    /* @hack redundant: a read of minute that never runs */
    if (0) {
        if (minute) {
        }
    }
    /* @hack redundant: the same read again, in a block of its own */
    if (0) {
        if (minute) {
        }
    }
    if (minute < game_state.minute ||
        (minute == game_state.minute && second <= game_state.second)) {
        pause_disable();
        if (lives_count(0) > 0) {
            stage_end(STAGE_END_RETRY);
            player_records[0]->flags |= PLAYER_MISSED;
            task->step = task_idle;
            if (hunt_current) {
                hunt_current->finish = 1;
                restart_point_clear(0);
            }
        } else {
            stage_end(STAGE_END_MISS);
            player_records[0]->flags |= PLAYER_MISSED;
            task->step = task_idle;
        }
    }
}

/* start the task named CountDownTimerForGameOver for a limit of minute and
 * second. it returns the task, or null where task_create fails */
static struct task *countdown_start(Sint8 minute, Sint8 second)
{
    struct task *task;

    if ((task = task_create(TASK_EXTRA, HUD_LEVEL, countdown_step,
                            "CountDownTimerForGameOver")) == NULL) {
        return NULL;
    }
    ((struct countdown *)task->extra.p)->minute = minute;
    ((struct countdown *)task->extra.p)->second = second;
    task->draw = countdown_draw;
    return task;
}

void time_limit_start(void)
{
    Sint8 stage;
    Sint8 minute;
    Sint8 second;
    Sint8 *limit;
    struct time_limit *entry;
    Sint8 *time;

    stage = stage_current_id();
    if (game_state.two_players != 0) {
        return;
    }
    if (stage == STAGE_CART_1) {
        return;
    }
    if (game_state.mission == MISSION_TIMED) {
        minute = LIMIT_MINUTES;
        second = 0;
        /* @bug logic: STAGE_CART_1's limit by driver never starts. the call
         * returns for that stage at 0x8c056282, so the test of STAGE_CART_1
         * at 0x8c056296 never holds */
        if (stage == STAGE_CART_1) {
            /* @hack type: the limits walked as bytes, where a struct
             * stage_time pointer would do */
            limit = &cart_time_limits[CART_LIMIT_TAILS].minute;
            switch (stage_character(0)) {
            case PLAYER_TAILS_MECH:
            case PLAYER_TAILS:
                break;
            case PLAYER_ROUGE:
                limit += sizeof(struct stage_time);
                break;
            default:
                return;
            }
            minute = *limit++;
            second = *limit;
            /* @hack spelling: a jump to the start, where the end of the if
             * would do */
            goto start;
        } else if (stage >= STAGE_RELAY_SONIC &&
                   stage <= STAGE_RELAY_KNUCKLES) {
            for (entry = time_limits.stages; entry->stage != STAGE_NONE;
                 entry++) {
                if (entry->stage == STAGE_RELAY_FIRST) {
                    /* @hack spelling: the limit read through a pointer
                     * stepped from its minute, here and below, where
                     * entry->limit's fields would do */
                    time = &entry->limit.minute;
                    minute = *time++;
                    second = *time;
                    break;
                }
            }
        } else {
            for (entry = time_limits.stages; entry->stage != STAGE_NONE;
                 entry++) {
                if (stage == entry->stage) {
                    time = &entry->limit.minute;
                    minute = *time++;
                    second = *time;
                    break;
                }
            }
        }
    } else if (stage == STAGE_GREEN_FOREST && game_state.debug == 0 &&
               game_state.mission == MISSION_FIRST) {
        minute = GREEN_FOREST_MINUTES;
        second = 0;
    } else if (stage == STAGE_WHITE_JUNGLE && game_state.debug == 0 &&
               game_state.mission == MISSION_FIRST) {
        minute = WHITE_JUNGLE_MINUTES;
        second = 0;
    } else if (stage == STAGE_SECURITY_HALL && game_state.debug == 0 &&
               game_state.mission == MISSION_FIRST) {
        minute = SECURITY_HALL_MINUTES;
        second = 0;
    } else {
        return;
    }
start:
    countdown_start(minute, second);
}

/* draw boss_labels' frame label at pos, at scale. pos.z is its priority */
static void boss_label_draw(int label, Float scale, NJS_POINT3 *pos, int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &boss_result_texlist;
    sprite.tanim = boss_labels;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, label, pos->z, attr);
}

/* sum count of the times from first into sum, to no more than 99:59 and 99
 * hundredths */
static void boss_times_sum(struct boss_time *sum, int first, int count)
{
    int minute = 0;
    int second = 0;
    int hundredths = 0;
    int i;

    for (i = first; i < first + count; i++) {
        minute += boss_record.times[i].minute;
        second += boss_record.times[i].second;
        hundredths += boss_record.times[i].hundredths;
    }
    second += hundredths / HUNDREDTHS_PER_SECOND;
    hundredths %= HUNDREDTHS_PER_SECOND;
    minute += second / SECONDS_PER_MINUTE;
    second %= SECONDS_PER_MINUTE;
    if (minute > CLOCK_LAST_MINUTE) {
        minute = CLOCK_LAST_MINUTE;
        second = CLOCK_LAST_SECOND;
        hundredths = CLOCK_LAST_HUNDREDTH;
    }
    sum->minute = minute;
    sum->second = second;
    sum->hundredths = hundredths;
}

/* sum the totals into total, as boss_times_sum does */
static void boss_totals_sum(void)
{
    int minute = 0;
    int second = 0;
    int hundredths = 0;
    int i;

    for (i = 0; i < BOSS_TOTALS; i++) {
        minute += boss_record.totals[i].minute;
        second += boss_record.totals[i].second;
        hundredths += boss_record.totals[i].hundredths;
    }
    second += hundredths / HUNDREDTHS_PER_SECOND;
    hundredths %= HUNDREDTHS_PER_SECOND;
    minute += second / SECONDS_PER_MINUTE;
    second %= SECONDS_PER_MINUTE;
    if (minute > CLOCK_LAST_MINUTE) {
        minute = CLOCK_LAST_MINUTE;
        second = CLOCK_LAST_SECOND;
        hundredths = CLOCK_LAST_HUNDREDTH;
    }
    boss_record.total.minute = minute;
    boss_record.total.second = second;
    boss_record.total.hundredths = hundredths;
}

void boss_total_start(void)
{
    struct task *task;
    struct task_work *work;

    boss_record.totals[0].label = BOSS_LABEL_TOTAL_1;
    boss_record.totals[1].label = BOSS_LABEL_TOTAL_1 + 1;
    boss_record.totals[2].label = BOSS_LABEL_TOTAL_1 + 2;
    if ((task = task_create(TASK_WORK, RESULT_LEVEL, boss_total_step,
                            "execTotalBossScore")) != NULL) {
        work = task->work;
        work->mode = BOSS_TOTAL_LOAD;
        /* the work's submode keeps the course the sequence plays, which the
         * task shows */
        switch (game_flags[FLAG_SEQUENCE]) {
        case SEQUENCE_HERO_BOSSES:
            work->submode = BOSS_COURSE_HERO;
            save_data.awards[SAVE_AWARD_FIRST_BOSS + BOSS_COURSE_HERO] = 1;
            break;
        case SEQUENCE_DARK_BOSSES:
            work->submode = BOSS_COURSE_DARK;
            save_data.awards[SAVE_AWARD_FIRST_BOSS + BOSS_COURSE_DARK] = 1;
            break;
        case SEQUENCE_ALL_BOSSES:
            work->submode = BOSS_COURSE_ALL;
            save_data.awards[SAVE_AWARD_FIRST_BOSS + BOSS_COURSE_ALL] = 1;
            break;
        }
        boss_times_sum(&boss_record.totals[0], 0, BOSS_RUN);
        boss_times_sum(&boss_record.totals[1], BOSS_RUN, BOSS_RUN);
        boss_times_sum(&boss_record.totals[2], BOSS_RUN * 2, BOSS_LAST_RUN);
        boss_totals_sum();
        if (work->submode == BOSS_COURSE_ALL) {
            save_record_boss_total(work->submode, boss_record.total.minute,
                                   boss_record.total.second,
                                   boss_record.total.hundredths);
        } else {
            save_record_boss_total(
                work->submode, boss_record.totals[work->submode].minute,
                boss_record.totals[work->submode].second,
                boss_record.totals[work->submode].hundredths);
        }
        if ((game_state.card_warning = save_check(0)) == SAVE_OK) {
            save_write();
        }
    }
    boss_record.timer = 0;
    boss_record.shown = 0;
    boss_record.total_shown = -1;
}

/* show the course's view. its windows open, its lines show one at a time,
 * and then the sum of all three runs */
static void boss_total_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *window;

    switch (work->mode) {
    case BOSS_TOTAL_LOAD:
        music_stop();
        task->draw = boss_total_draw;
        task->destructor = boss_total_destroy;
        texture_load_pvm(BOSS_RESULT_TEXTURE_ARCHIVE, &boss_result_texlist);
        texture_load_pvm(RESULT_TEXTURE_ARCHIVE, &result_texlist);
        boss_record.timer = BOSS_LOAD_STEPS;
        work->mode = BOSS_TOTAL_WAIT;
        boss_total_showing = TRUE;
        /* fall through */
    case BOSS_TOTAL_WAIT:
        boss_record.timer--;
        if (boss_record.timer <= 0) {
            work->mode = BOSS_TOTAL_WINDOWS;
        }
        break;
    case BOSS_TOTAL_WINDOWS:
        window =
            score_window_open(boss_layouts[work->submode].windows[0].right,
                              boss_layouts[work->submode].windows[0].left,
                              boss_layouts[work->submode].windows[0].bottom,
                              boss_layouts[work->submode].windows[0].top);
        window->unk_3c.l = 1;
        window =
            score_window_open(boss_layouts[work->submode].windows[1].right,
                              boss_layouts[work->submode].windows[1].left,
                              boss_layouts[work->submode].windows[1].bottom,
                              boss_layouts[work->submode].windows[1].top);
        window->unk_3c.l = 1;
        work->mode = BOSS_TOTAL_LINES;
        boss_record.shown = 0;
        boss_record.timer = BOSS_WINDOWS_STEPS;
        break;
    case BOSS_TOTAL_LINES:
        boss_record.timer--;
        if (boss_record.timer <= 0) {
            boss_record.shown++;
            sound_play(SOUND_SHOW, NULL, 0, 0);
            if (boss_record.shown >= boss_layouts[work->submode].times) {
                work->mode = BOSS_TOTAL_PAUSE;
                boss_record.timer = BOSS_PAUSE_STEPS;
            } else {
                boss_record.timer = BOSS_LINE_STEPS;
            }
        }
        break;
    case BOSS_TOTAL_PAUSE:
        boss_record.timer--;
        if (boss_record.timer <= 0) {
            boss_record.total_shown = work->submode;
            boss_record.timer = BOSS_SHOWN_STEPS;
            work->mode = BOSS_TOTAL_SHOWN;
            sound_play(SOUND_SHOW, NULL, 0, 0);
        }
        break;
    case BOSS_TOTAL_SHOWN:
        boss_record.timer--;
        if (boss_record.timer > 0 &&
            !(pad_press_repeat[0] & pad_choose_buttons)) {
            break;
        }
        work->mode = BOSS_TOTAL_DONE;
        break;
    case BOSS_TOTAL_DONE:
        boss_total_showing = FALSE;
        break;
    }
}

static void boss_total_draw(struct task *task)
{
    NJS_POINT3 pos;
    NJS_POINT3 time_pos;
    struct task_work *work;
    struct boss_time *time;
    int i;

    if (boss_total_showing == FALSE) {
        return;
    }
    work = task->work;
    pos.x = boss_left;
    pos.z = BOSS_Z;
    switch (work->submode) {
    case BOSS_COURSE_HERO:
        pos.y = boss_top_first;
        time = &boss_record.times[0];
        break;
    case BOSS_COURSE_DARK:
        pos.y = boss_top_second;
        time = &boss_record.times[BOSS_RUN];
        break;
    case BOSS_COURSE_ALL:
        pos.y = boss_top_all;
        time = &boss_record.totals[0];
        break;
    }
    for (i = 0; i < boss_record.shown; i++) {
        boss_label_draw(time->label, 1.0F, &pos, NJD_SPRITE_ALPHA);
        vector_sum(&pos, &boss_time_offset, &time_pos);
        time_draw(&time_pos, time->minute, time->second, time->hundredths);
        pos.y += boss_line_height;
        time++;
    }
    if (work->submode == BOSS_COURSE_ALL) {
        pos.y += boss_total_gap_all;
    } else {
        pos.y += boss_total_gap;
    }
    if (boss_record.total_shown >= 0) {
        boss_label_draw(BOSS_LABEL_TOTAL, 1.0F, &pos, NJD_SPRITE_ALPHA);
        vector_sum(&pos, &boss_time_offset, &time_pos);
        time_draw(&time_pos, boss_record.total.minute, boss_record.total.second,
                  boss_record.total.hundredths);
    }
}

static void boss_total_destroy(struct task *task)
{
    njReleaseTexture(&boss_result_texlist);
    njReleaseTexture(&result_texlist);
    boss_total_showing = FALSE;
}

/* whether player 0 plays one of the dark story's characters */
static int playing_dark(void)
{
    switch (stage_character(0)) {
    case PLAYER_SONIC:
    case PLAYER_TAILS_MECH:
    case PLAYER_TAILS:
    case PLAYER_KNUCKLES:
        return 0;
    default:
        return 1;
    }
}

void boss_times_clear(int course)
{
    Sint32 *word = (Sint32 *)&boss_record;
    int i;

    for (i = sizeof(boss_record) / sizeof(Sint32); i != 0; i--) {
        *word++ = 0;
    }
    boss_total_showing = FALSE;
    if (course == BOSS_COURSE_DARK) {
        boss_record.count = BOSS_RUN;
    }
}

void boss_time_record(void)
{
    struct boss_time *time = &boss_record.times[boss_record.count];

    if (boss_record.count >= BOSS_TIMES) {
        return;
    }
    switch (stage_current_id()) {
    case STAGE_BOSS_BIGFOOT:
        time->label = 0;
        break;
    case STAGE_VERSUS_SONIC_1:
        if (playing_dark()) {
            time->label = 11;
        } else {
            time->label = 2;
        }
        break;
    case STAGE_VERSUS_MECH_1:
        if (playing_dark()) {
            time->label = 9;
        } else {
            time->label = 1;
        }
        break;
    case STAGE_BOSS_BIGBOGY:
        time->label = 3;
        break;
    case STAGE_BOSS_GOLEM:
    case STAGE_BOSS_GOLEM_E:
        time->label = 4;
        break;
    case STAGE_VERSUS_KNUCKLES:
        if (playing_dark()) {
            time->label = 12;
        } else {
            time->label = 5;
        }
        break;
    case STAGE_VERSUS_MECH_2:
        if (playing_dark()) {
            time->label = 13;
        } else {
            time->label = 6;
        }
        break;
    case STAGE_VERSUS_SONIC_2:
        if (playing_dark()) {
            time->label = 14;
        } else {
            time->label = 7;
        }
        break;
    case STAGE_BOSS_HOTSHOT:
        time->label = 8;
        break;
    case STAGE_BOSS_FDOG:
        time->label = 10;
        break;
    case STAGE_BOSS_LAST1:
        time->label = 15;
        break;
    case STAGE_BOSS_LAST2:
        time->label = 16;
        break;
    }
    time->minute = game_state.minute;
    time->second = game_state.second;
    time->hundredths =
        (Uint32)njRoundUp(game_state.frame * HUNDREDTHS_PER_FRAME);
    boss_record.count++;
}
