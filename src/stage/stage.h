/* the stages: their numbers, the current one and its players' characters.
 * stage_step runs a stage from its title card to its end */
#ifndef STAGE_STAGE_H
#define STAGE_STAGE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct hud_number_cell;
struct icon_rect;
struct task;
struct trail;
struct trail_style;

/* the seed rand takes at the game's start, at each demo's, and in each
 * display_reset_full */
#define RAND_SEED 0xdead0cab

/* the number past STAGE_SAND_OCEAN_2P's, whose STG45.PRS is the disc's last
 * stg<stage>.prs. loader_read_stage_overlay reads that name for every stage
 * below STAGE_BOSS_BIGFOOT. some tests stop below STAGE_STG_END, and others
 * take it in */
#define STAGE_STG_END 46

/* the one-player stages, each with the music its overlay plays */
#define STAGE_GREEN_FOREST 3 /* g_fores.adx */
#define STAGE_WHITE_JUNGLE 4 /* w_jungl.adx */
#define STAGE_PUMPKIN_HILL 5 /* p_hill.adx */
#define STAGE_SKY_RAIL 6 /* s_rail.adx */
#define STAGE_AQUATIC_MINE 7 /* A_MINE.ADX */
#define STAGE_SECURITY_HALL 8 /* s_hall.adx */
#define STAGE_PRISON_LANE 9 /* p_lane.adx */
#define STAGE_METAL_HARBOR 10 /* m_harb1.adx */
#define STAGE_IRON_GATE 11 /* i_gate.adx */
#define STAGE_WEAPONS_BED 12 /* W_BED.ADX */
#define STAGE_CITY_ESCAPE 13 /* c_escap1.adx */
#define STAGE_RADICAL_HIGHWAY 14 /* r_hwy.adx */
#define STAGE_WILD_CANYON 16 /* w_canyon.adx */
#define STAGE_MISSION_STREET 17 /* m_street.adx */
#define STAGE_DRY_LAGOON 18 /* d_lagoon.adx */
#define STAGE_SAND_OCEAN 21 /* S_OCEAN.ADX */
#define STAGE_CRAZY_GADGET 22 /* c_gadget.adx */
#define STAGE_HIDDEN_BASE 23 /* h_base.adx */
#define STAGE_ETERNAL_ENGINE 24 /* e_engine.adx */
#define STAGE_DEATH_CHAMBER 25 /* d_chambr.adx */
#define STAGE_EGG_QUARTERS 26 /* e_quart.adx */
#define STAGE_LOST_COLONY 27 /* l_colony.adx */
#define STAGE_PYRAMID_CAVE 28 /* p_cave.adx */
#define STAGE_FINAL_RUSH 30 /* f_rush.adx */
#define STAGE_GREEN_HILL 31 /* g_hill.adx */
#define STAGE_METEOR_HERD 32 /* m_herd.adx */
#define STAGE_FINAL_CHASE 40 /* f_chase.adx */
#define STAGE_COSMIC_WALL 43 /* c_wall.adx */
#define STAGE_MAD_SPACE 44 /* m_space.adx */

/* the two-player stages. each plays the music of the stage it is named for.
 * the two-player game's stage select lists them */
#define STAGE_WEAPONS_BED_2P 15
#define STAGE_MISSION_STREET_2P 39
#define STAGE_WILD_CANYON_2P 41
#define STAGE_SAND_OCEAN_2P 45
/* the stages the title's test menu names BASIC TEST, KNUCKLES TEST and
 * SONIC TEST */
#define STAGE_BASIC_TEST 0
#define STAGE_KNUCKLES_TEST 1
#define STAGE_SONIC_TEST 2

/* the relay: five stages played in turn, in the order below, the last of them
 * leading to STAGE_BOSS_LAST2. main.c gives player 0 the character each stage
 * is named for. the clock, the score and player 0's rings carry over from each
 * stage to the next.
 *
 * a retry and the pause menu's restart start the relay over. so does a miss in
 * MISSION_TIMED past the first stage. stage_go_back in the last stage goes back
 * to the first, with the counts as they stand */
#define STAGE_RELAY_TAILS 36
#define STAGE_RELAY_EGGMAN 35
#define STAGE_RELAY_ROUGE 37
#define STAGE_RELAY_KNUCKLES 38
#define STAGE_RELAY_SONIC 34
#define STAGE_RELAY_FIRST STAGE_RELAY_TAILS
#define STAGE_RELAY_LAST STAGE_RELAY_SONIC
/* whether stage is one of the relay's, and whether it is one before the
 * last. both rest on the relay's numbers, which run from STAGE_RELAY_SONIC,
 * the last, to STAGE_RELAY_KNUCKLES */
#define STAGE_RELAY(stage)                                                     \
    ((stage) >= STAGE_RELAY_SONIC && (stage) <= STAGE_RELAY_KNUCKLES)
#define STAGE_RELAY_BEFORE_LAST(stage)                                         \
    ((stage) >= STAGE_RELAY_EGGMAN && (stage) <= STAGE_RELAY_KNUCKLES)

/* the stages whose overlays loader_read_stage_overlay reads by names of
 * their own, as boss_bigfoot.PRS */
#define STAGE_BOSS_BIGFOOT 50
#define STAGE_BOSS_HOTSHOT 51
#define STAGE_BOSS_FDOG 52
#define STAGE_BOSS_BIGBOGY 53
#define STAGE_BOSS_GOLEM 54
#define STAGE_BOSS_LAST1 55
#define STAGE_BOSS_LAST2 56
#define STAGE_BOSS_GOLEM_E 57
#define STAGE_CART_1 70
#define STAGE_CART_2 71
#define STAGE_CHAO 90
/* no stage, one past STAGE_CHAO. it ends a table by stage */
#define STAGE_NONE 91

/* the stages where two characters fight. each overlay holds both
 * fighters' title textures */
#define STAGE_VERSUS_SONIC_1 19 /* TITLETEX_SONIC and TITLETEX_SHADOW */
#define STAGE_VERSUS_MECH_1 20 /* TITLETEX_TAILS and TITLETEX_EGGMAN */
#define STAGE_VERSUS_MECH_2 29 /* TITLETEX_TAILS and TITLETEX_EGGMAN */
#define STAGE_VERSUS_KNUCKLES 33 /* TITLETEX_KNUCKLES and TITLETEX_ROUGE */
#define STAGE_VERSUS_SONIC_2 42 /* TITLETEX_SONIC and TITLETEX_SHADOW */
/* whether stage is one of them. the macro reads stage once for each of its
 * five tests */
#define STAGE_VERSUS(stage)                                                    \
    ((stage) == STAGE_VERSUS_SONIC_1 || (stage) == STAGE_VERSUS_SONIC_2 ||     \
     (stage) == STAGE_VERSUS_MECH_1 || (stage) == STAGE_VERSUS_MECH_2 ||       \
     (stage) == STAGE_VERSUS_KNUCKLES)

/* what stage_step does each frame, from the stage's title card to its end.
 * a mode named _FADE fades the screen out, and then moves on. play runs on
 * under each fade but STAGE_RESTART_FADE's and STAGE_RETRY_FADE's, which
 * only draw. no mode has the number 4 or 6 */
enum stage_mode {
    /* counts the play in save_data and starts the title card. a demo and a
     * two-player game go to STAGE_ENTER instead */
    STAGE_TITLE_CARD = 0,
    /* runs the title card until the card records STAGE_END_CARD_SHOWN */
    STAGE_TITLE_CARD_SHOW = 1,
    /* loads the stage and starts play, under the rest of the title card */
    STAGE_ENTER_CARD = 2,
    STAGE_ENTER = 3, /* loads the stage and starts play */
    STAGE_AGAIN = 5, /* starts play again without a load */
    STAGE_FADE_IN = 7, /* plays under the fade in, then goes to STAGE_PLAY */
    /* to STAGE_LEAVE. under a Chao key the fade is to white, and otherwise to
     * black. in a relay stage before the last, it is white only once a winner
     * has posed */
    STAGE_CLEAR_FADE = 8,
    /* to STAGE_AGAIN at the restart points, counting a miss */
    STAGE_MISS_FADE = 9,
    STAGE_GAME_OVER_FADE = 10, /* to STAGE_GAME_OVER. no code sets it */
    /* to the stage stage_set_next named, by STAGE_LEAVE. where that stage is
     * the current one, it goes to play from the stage's start, without a load.
     * the fade is to white under a Chao key, but in a relay stage before the
     * last */
    STAGE_GO_TO_FADE = 11,
    /* to STAGE_AGAIN at the restart points, counting no miss */
    STAGE_RESTART_FADE = 12,
    /* to play from the stage's start, without a load, counting a retry.
     * player 0's record takes PLAYER_MISSED as the fade ends */
    STAGE_RETRY_FADE = 13,
    STAGE_QUIT_FADE = 14, /* to STAGE_QUIT */
    /* a fade to white, then play from the stage's start without a load. the
     * clock, the score and the rings stay as they stand */
    STAGE_BACK_FADE = 15,
    STAGE_PLAY = 16,
    STAGE_PAUSE = 17, /* the pause menu */
    STAGE_NO_PAD = 18, /* a pad is gone, and the stage waits for it */
    /* unloads the stage, makes the next stage current and returns
     * OVERLAY_DONE */
    STAGE_LEAVE = 19,
    STAGE_GAME_OVER = 20, /* unloads the stage and returns OVERLAY_BACK */
    STAGE_QUIT = 21, /* unloads the stage and returns OVERLAY_BACK */
    STAGE_CARD_NOTICE = 22, /* shows a message about the save for a time */
    STAGE_SAVING = 23, /* waits for the save's write to end */
    /* the save can't be written. the stage waits for a card that takes it,
     * or for a press to play on without it */
    STAGE_CARD_ERROR = 24,
    /* the save's socket holds a card with another save, or with none. the
     * stage waits as in STAGE_CARD_ERROR */
    STAGE_CARD_NOT_OURS = 25,
    STAGE_CARD_GONE = 26, /* the same, for a socket with no card ready */
    /* the pads asked for a reset. the stage waits a few frames, then returns
     * OVERLAY_RESET */
    STAGE_RESET = 27
};

/* how play ends: what stage_end records for stage_step to act on in
 * STAGE_PLAY */
enum stage_end {
    STAGE_END_NONE = 0,
    STAGE_END_CLEAR = 1,
    STAGE_END_MISS = 2, /* a player has missed: back to the restart point */
    STAGE_END_GO_TO = 3, /* to the stage stage_set_next named */
    STAGE_END_RESTART = 4, /* back to the restart point, counting no miss */
    STAGE_END_RETRY = 5, /* the stage from its start */
    /* the title card has shown the stage's name, and play can start under
     * the rest of it. stage_step reads it only in STAGE_TITLE_CARD_SHOW */
    STAGE_END_CARD_SHOWN = 6,
    STAGE_END_BACK = 7, /* a back ring's: see stage_go_back */
    STAGE_END_QUIT = 8
};

/* make Sonic slot 0's character and seed rand, at the game's start */
void stage_boot(void);
/* set the current stage up for stage_step to run. where game_state.relay is
 * 0, it starts from the title card and clears the counts the relay carries
 * over. otherwise it starts at STAGE_ENTER. a two-player game plays
 * MISSION_FIRST. so does a one-player game of a STAGE_VERSUS stage, or of a
 * stage past STAGE_STG_END other than STAGE_CART_1. the port returns FALSE
 * before creating stage resources if the language files are unavailable */
Bool stage_start(void);
/* destroy every task and free what stage_start made. game_state.cleared and
 * game_state.winner_posed go back to 0. it returns TRUE */
Bool stage_stop(void);
/* run the stage for one frame, as its mode says. it returns an enum
 * overlay_result of main.h. OVERLAY_DONE means the stage has left for the
 * next, and OVERLAY_BACK follows a quit or a game over. OVERLAY_RESET means
 * the pads asked for a reset, and OVERLAY_RUNNING that the stage goes on */
int stage_step(void);

/* record end, an enum stage_end, for stage_step to act on */
void stage_end(int end);

/* the character stage_create_players makes for slot. slot 0 has one, and
 * every other slot shares a second. both are kept apart from the players'
 * records */
void stage_set_character(int slot, int character);
int stage_character(int slot);

/* the misses and retries stage_step has counted since the stage loaded */
int stage_get_retries(void);
/* make stage the current stage, keeping the stage it replaces as the
 * previous one. it loads nothing */
void stage_set_current(int stage);
/* name the stage that follows the current one. stage_go_to_next makes that
 * stage current, keeping the current one as the previous one, and returns
 * it. neither loads anything */
void stage_set_next(int stage);
Sint16 stage_go_to_next(void);
/* end play with STAGE_END_GO_TO, toward stage */
void stage_fade_to(int stage);
/* end play with STAGE_END_RESTART */
void stage_restart(void);
/* the current stage, by the number its files are named with */
int stage_current_id(void);
/* the stage before the current one, and the one stage_set_next named */
int stage_previous_id(void);
int stage_next_id(void);

/* let Start open the pause menu, or keep it from opening. they clear and
 * set game_state.pause_disabled */
void pause_enable(void);
void pause_disable(void);

/* player's start in the current stage, by its character: its place in *pos and
 * its turn in ang->y. it returns FALSE for an empty slot. otherwise it returns
 * TRUE, whether or not the character's table lists the stage.
 *
 * the call first sets each of *pos and *ang that isn't null to zeros. where the
 * table lists the stage, it then fills both, so neither may be null. the table
 * holds a start for a one-player game and one for each player of two. a
 * STAGE_VERSUS stage takes a two-player start in a one-player game too */
Bool stage_find_start(int player, NJS_POINT3 *pos, struct angles *ang);
/* put the player of slot player at its restart point, or else at its start. a
 * restart point gives back the clock, the gravity's tilt and the up axis it
 * kept. it also takes the player's rings. a start in a one-player game gives
 * back the clock, the score and the rings their copies hold. either way the
 * player's past, light and speeds start over. an empty slot changes nothing */
void stage_place_player(int player);
/* stage_place_player, keeping the clock, the score and the rings as they
 * are */
void stage_place_player_again(int player);
/* put the player of slot player where its character stands before a
 * two-player stage starts. where its table lists no place in the stage, the
 * player goes to the origin. an empty slot changes nothing */
void stage_place_player_intro(int player);

/* stage_step's mode, an enum stage_mode */
int stage_get_mode(void);

/* forget player's restart point, whose order becomes -1. while
 * stage_place_player_again or the restart stage_go_back leads to runs, the
 * point keeps the clock as it stands */
void restart_point_clear(int player);
/* make pos and ang player's restart point, of order order. the point also
 * keeps the stage's clock, the gravity's tilt and collision_get_up_axis().
 * the call keeps nothing where player's point already has a higher order.
 * in a two-player game whose slot 0 holds Sonic or Shadow, the other player
 * takes the same point. it does so whatever its own point's order */
void restart_point_keep(int player, NJS_POINT3 *pos, struct angles *ang,
                        int order);

/* player has reached the stage's goal. the pads and the pause go off, the clock
 * stops, and game_state.cleared becomes 1. a two-player game counts the round
 * for player, and the match too where the round ends it. the stage's clock goes
 * to hud/hud.h's boss_time_record. a boss attack's last stage also starts
 * boss_total_start. the results then start, with player's camera in
 * CAMERA_MODE_FIX2. a relay stage before the last ends play with
 * STAGE_END_CLEAR instead. it starts the results all the same under
 * MISSION_RINGS, where player 0 holds RINGS_GOAL rings or more */
void stage_clear(int player);
/* pose player as the stage's winner, at its character's place for the win.
 * it plays the winner's animation, a camera motion about it and its voice.
 * in a two-player game the other player stands beside it, and otherwise at
 * its own place. it sets game_state.winner_posed to 1. in STAGE_CART_1 it
 * does nothing. player's slot must hold a player, whose record the call
 * reads before it tests it */
void stage_pose_winner(int player);
/* clear player's restart point where another stage kept it, as
 * restart_point_clear does. it returns the point's order, or -1 for none */
int restart_point_refresh(int player);
/* player has touched a back ring. player 0's pad and the pause go off, and
 * the clock stops. player's camera takes CAMERA_MODE_FIX2, and play ends
 * with STAGE_END_BACK. stage_step then plays the stage from its start. the
 * clock, the score and the rings stay as they stand. the relay's last
 * stage goes back to the relay's first instead */
void stage_go_back(int player);

/* the calls a stage's overlay hands the game. the overlay's init fills them,
 * and stage_unload clears them */
struct stage_hooks {
    /* message_window_open_unowned in every overlay but the Chao world's. no
     * code calls through it */
    struct task *(*open_message)(int kind, const char *text, int time,
                                 int language);
    /* a Chaos Drive's trail: in every overlay, shared/common_object.h's calls
     * of the same names */
    struct trail *(*trail_alloc)(struct trail_style *style, int count);
    void (*trail_draw)(struct trail_style *style, struct trail *trail,
                       NJS_POINT3 *pos);
    void (*trail_free)(struct trail_style *style, struct trail *trail);
    /* set by the carts' overlay alone: the part of a texture the action
     * window's icon takes */
    void (*action_icon)(struct icon_rect *rect);
    /* set by the carts' overlay alone: each cell of a HUD_NUMBER_HOOK
     * number, for hud_number_draw */
    void (*number_cell)(struct hud_number_cell *cell);
    /* set by the Chao world alone, to chao_current_area. while it isn't
     * null, the action window steps and draws only while it returns 1 to 3 */
    int (*chao_area)(void);
    /* set by the Chao world alone: the save STAGE_CARD_ERROR makes there, in
     * place of save_check and save_write */
    void (*save)(void);
};
extern struct stage_hooks stage_hooks;
#ifdef NONMATCHING
/* the image reads each call through an address of its own. on the port,
 * every read goes through the record the overlay filled */
#define trail_alloc_hook (&stage_hooks.trail_alloc)
#define trail_draw_hook (&stage_hooks.trail_draw)
#define trail_free_hook (&stage_hooks.trail_free)
#define action_icon_hook (&stage_hooks.action_icon)
#define number_cell_hook (&stage_hooks.number_cell)
#define chao_area_hook (&stage_hooks.chao_area)
#define save_hook (stage_hooks.save)
#else
/* the members of stage_hooks under names of their own, which symbols.txt
 * places at the members' addresses. each is declared only for the hack
 * tagged at its call */
/* @hack type: each but save_hook an array, where a pointer would do */
extern struct trail *(*trail_alloc_hook[])(struct trail_style *style,
                                           int count);
extern void (*trail_draw_hook[])(struct trail_style *style, struct trail *trail,
                                 NJS_POINT3 *pos);
extern void (*trail_free_hook[])(struct trail_style *style,
                                 struct trail *trail);
extern void (*action_icon_hook[])(struct icon_rect *rect);
extern void (*number_cell_hook[])(struct hud_number_cell *cell);
extern int (*chao_area_hook[])(void);
extern void (*save_hook)(void);
#endif

#endif
