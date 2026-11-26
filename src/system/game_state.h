/* the game's state: one record that sources across the game read and write */
#ifndef SYSTEM_GAME_STATE_H
#define SYSTEM_GAME_STATE_H
#include <ninja.h>
#include <sg_xpt.h>

/* the values of game_state.demo that select a demo's files. under DEMO_ON,
 * loader_read_stage_overlay reads the stage's demo file. under
 * DEMO_ON_NO_FILE it reads no demo file, and sets demo to DEMO_ON */
#define DEMO_ON 1
#define DEMO_ON_NO_FILE 3
/* under DEMO_ON, pad_scan gives players 0 and 1 the demo's frame for the
 * step. under DEMO_RECORD it writes the frame from what they hold */
#define DEMO_RECORD 2

/* the values of game_state.display, by the display system_boot_init starts */
#define DISPLAY_VGA 0
#define DISPLAY_NTSC 1
#define DISPLAY_PAL 2

/* the values of game_state.voice_language */
#define VOICE_JAPANESE 0
#define VOICE_ENGLISH 1

/* the values of game_state.paused but 0. PAUSED_BY_PLAYER_0 and
 * PAUSED_BY_PLAYER_1 name the player who holds the pause menu.
 * PAUSED_BY_GAME holds the game for a message of the stage's */
#define PAUSED_BY_PLAYER_0 1
#define PAUSED_BY_PLAYER_1 2
#define PAUSED_BY_GAME 3

/* the bits of game_state.hidden_cameras */
#define HIDDEN_CAMERA_0 1
#define HIDDEN_CAMERA_1 2

/* the bit of game_state.time_stopped for the player in slot */
#define TIME_STOPPED_BIT(slot) (1 << (slot))

/* the players a game has at most, 0 and 1. each array by player holds an
 * entry for each */
#define GAME_PLAYERS 2
/* the game's steps in a second */
#define STEPS_PER_SECOND 60
/* the levels a player's rings earn in a two-player game. level 0 comes at 20
 * rings past a multiple of 60, 1 at 40 and 2 at 60 */
#define RING_LEVELS 3
/* the ring levels a Sonic or a Shadow spends, by the action each pays for */
#define RING_LEVEL_SPEED_UP 0
#define RING_LEVEL_WIND_CUTTER 1
#define RING_LEVEL_TIME_STOP 2
/* the ring levels a hunter spends, by the attack each pays for */
#define RING_LEVEL_EARTHQUAKE 0
#define RING_LEVEL_EARTHFLAME 1
#define RING_LEVEL_POWER_FLASH 2
/* the ring levels a walker spends, by what each pays for: its first special
 * move, its second and its laser */
#define RING_LEVEL_SPECIAL_1 0
#define RING_LEVEL_SPECIAL_2 1
#define RING_LEVEL_LASER 2

/* the values of game_state.two_players but 0, as the title's test menu names
 * them. 2P 3POINTS MATCH is won by ROUNDS_TO_WIN rounds, and 2P 1POINT MATCH by
 * one */
#define TWO_PLAYERS_3_POINTS 1
#define TWO_PLAYERS_1_POINT 2
#define ROUNDS_TO_WIN 2
/* game_state.round_winner before a round is won */
#define NO_ROUND_WINNER (-1)

/* the values of game_state.chao_race_menu, the menu that opened the Chao
 * world for its races. the versus menu's Chao race sets
 * CHAO_RACE_MENU_VERSUS, and the emblem results' Chao line
 * CHAO_RACE_MENU_EMBLEMS */
#define CHAO_RACE_MENU_NONE 0
#define CHAO_RACE_MENU_VERSUS 1
#define CHAO_RACE_MENU_EMBLEMS 2

/* the values of game_state.mission, which counts a stage's missions from 0.
 * a player clears MISSION_RINGS by holding RINGS_GOAL rings, and
 * MISSION_TIMED has a time limit. under MISSION_HD, set_read_stage takes a
 * stage's _hd_ set files for a one-player game. the names are an action
 * stage's missions. a stage of another kind, as the cart's, gives the values
 * meanings of its own. its source states them where it reads them */
#define MISSION_FIRST 0
#define MISSION_RINGS 1
#define MISSION_CHAO 2
#define MISSION_TIMED 3
#define MISSION_HD 4
#define MISSIONS 5 /* the missions a stage can have */
#define RINGS_GOAL 100

/* the values of game_state.card_state: the command the memory cards are busy
 * with, or the one that last failed. each failed value is its busy value's
 * negative. save/card.h says what its callbacks and commands set. the stage
 * writes the state too */
#define CARD_IDLE 0
#define CARD_SAVING 1
#define CARD_LOADING 2
#define CARD_DELETING 3
#define CARD_SAVE_FAILED (-1)
#define CARD_LOAD_FAILED (-2)
#define CARD_DELETE_FAILED (-3)

/* game_state.card_warning as the game starts. under EDITION_TAIKEN, the
 * title opens on the alert while card_warning holds it */
#define CARD_WARNING_ALERT 1
/* the values of game_state.card_warning that GAME_MODE_BOOT sets, where
 * card_check returns CARD_CHECK_NO_CARD or CARD_CHECK_NO_ROOM. MCWARN.PRS
 * shows a warning for each */
#define CARD_WARNING_NO_CARD 2
#define CARD_WARNING_NO_ROOM 3
/* the stage takes a card_warning of CARD_WARNING_ERROR or more for an error of
 * save_check's. those errors start at SAVE_NOT_READY, and the boot's warnings
 * lie below CARD_WARNING_ERROR */
#define CARD_WARNING_ERROR 4

/* the values of game_state.other_print, by what OTHERPRINT.PRS shows: the
 * logos, or the picture whose textures are named tento_ending */
#define OTHER_PRINT_LOGOS 1
#define OTHER_PRINT_ENDING 2

/* the values of game_state.edition that the programs test, each named for what
 * it selects. main.c starts it at 7, which none tests.
 *
 * under EDITION_TAIKEN, the title shows its trial's screens, which its tasks
 * name Taiken. under the other three, ADVERTISE.PRS runs a stand-in title, its
 * fake title, in place of the menus. under EDITION_TENTO, that title shows a
 * page of its own in place of its modes. the logos of OTHER_PRINT_LOGOS then
 * open on the picture whose textures are named tento_alert. game_mode_divert
 * sends a two-player game to OTHER_PRINT_ENDING, which shows no alert */
#define EDITION_TAIKEN 1
#define EDITION_FAKE_TITLE_2 2
#define EDITION_FAKE_TITLE_5 5
#define EDITION_TENTO 6
/* the values of game_state.trial_screen that the trial's title tests, each
 * named for the screen it opens on */
#define TRIAL_SCREEN_TITLE 1
#define TRIAL_SCREEN_COMING_SOON 2
/* game_state.next_demo while the title plays no attract demo */
#define NO_NEXT_DEMO (-1)

/* themes of game_state.theme under which the versus select of ADVERTISE.PRS
 * gives both players COSTUME_H or COSTUME_X of player/player.h */
#define THEME_COSTUME_H 4
#define THEME_COSTUME_X 12

/* the bit of land block n in game_state.land_block_mask and land_blocks, and
 * the bits of blocks first to last */
#define LAND_BLOCK(n) (1 << (n))
#define LAND_BLOCKS(first, last) ((2 << (last)) - (1 << (first)))

/* the state the whole game shares. src/main.c defines it, with the values
 * its fields start at */
struct game_state {
    /* 0, or a DEMO_ value while a demo plays or records. no vibration starts
     * while it isn't 0, and placement_try_leave measures from the camera
     * only while it is 0 */
    Sint8 demo;
    /* the attract demo the title plays next: an index of its table, or
     * NO_NEXT_DEMO for none. under EDITION_TAIKEN it alternates 0 and 1. 0
     * plays the table's first demo, and 1 the event of SEQUENCE_TAIKEN_EVENT.
     * the trial's title opens on the logos while it is 0, unless card_warning
     * or trial_screen picks another screen */
    Sint8 next_demo;
    /* while it isn't 0, the Biolizard's test menu runs in player 0's pause.
     * Start then skips an event even before it has been seen. in
     * MISSION_FIRST, STAGE_GREEN_FOREST, STAGE_WHITE_JUNGLE and
     * STAGE_SECURITY_HALL have a time limit only while it is 0 */
    Sint8 debug;
    /* while it is 1, the collision meshes print their pools' use and their
     * warnings, and polygon_sweep draws each triangle it touches. the
     * stage's reset clears it */
    Sint8 debug_display;
    Uint8 unk_04[5];
    /* while it isn't 0, an SG ring draws on every screen. Lost Colony and
     * Final Rush keep their fog clear under it, as they do under unk_25. many
     * enemies, bosses and shots hold still, as they do under time_stopped */
    Sint8 unk_09;
    Uint8 unk_0a;
    /* while it is 0, these draw nothing: the action window, the technique
     * bonuses, the time limit's countdown, the item stocks, the mission
     * message, the title card, the boss title, the hunt's radar and its icons,
     * and a walker's gauge and lock-on markers. Egg Quarters' guards stand
     * down, and an item icon row takes no icon. hud_draw tests hud_on instead,
     * and an event's scene clears both while it plays. while it is 1, a demo
     * draws its PRESS START, and a walker's draw calls walker_gauge_draw */
    Sint8 unk_0b;
    Sint8 hud_on; /* hud_draw draws nothing while it is 0 */
    Sint8 lives_shown; /* hud_draw draws the lives only while it is 1 */
    /* hud_draw draws the rings only while it is 1. the stage's reset sets
     * it to 1, and stage_pose_winner sets it to 0 in a two-player game */
    Sint8 rings_shown;
    /* a DISPLAY_ value. a fade steps by its faster step only under
     * DISPLAY_PAL */
    Sint8 display;
    /* 1 while the sound is mono. sound_apply_mono then sets the sound
     * driver to mono, and sound_pan_by_pos gives a pan of 0 */
    Sint8 mono;
    /* an enum text_language of system/text.h, which selects text_read's
     * files */
    Sint8 text_language;
    Sint8 voice_language; /* selects the files sound_load_voices loads */
    /* 1 once loader_note_error or loader_poll_drive has found the GD-ROM's
     * tray opened */
    Sint8 tray_opened;
    /* a CARD_ value. the cards' screens show nothing while it isn't
     * CARD_IDLE: screens_fill does nothing, and nor does lcd_start's task.
     * while it is CARD_SAVING, no vibration starts, and loader_poll_drive
     * doesn't leave the game.
     *
     * the stage's STAGE_SAVING waits while it is above CARD_IDLE. below it,
     * the stage's step leaves play for STAGE_CARD_NOTICE, whose end sets it
     * back to CARD_IDLE */
    /* @hack type: card_state as a plain Sint8, where a volatile one would
     * do: card.c's callbacks set it in the handler of Kaede, the peripheral
     * driver, while main's exit waits on it */
#ifdef NONMATCHING
    /* GCC reads card_state once for main's wait unless it is volatile. SHC's
     * code reads it at each pass */
    volatile Sint8 card_state;
#else
    Sint8 card_state;
#endif
    /* 1 once the card leaves the drive save_set_drive gave, while card_state
     * is CARD_IDLE. a card's mount in that drive sets it to 0. while it isn't
     * 0, the stage's step leaves play for a message about the save. the
     * message's end clears it */
    Sint8 card_removed;
    /* while it isn't 0, Start opens no pause menu. pause_enable and
     * pause_disable clear and set it */
    Sint8 pause_disabled;
    /* 0 in play, or a PAUSED_BY_ value. the pause menu reads the pad of
     * player paused - 1. the pause hints read the same pad, and step by its
     * L and R */
    Sint8 paused;
    Uint8 unk_18;
    /* 1 while the sound is paused. only the sounds of sound/sound.h's paused
     * ids, SOUND_CURSOR_ID to SOUND_PAUSE_ID, then play. sound_update passes
     * over the slots of the others */
    Sint8 sound_paused;
    /* 1 from stage_clock_stop until stage_clock_resume. the clock stands
     * while it is 1 */
    Sint8 clock_stopped;
    /* the stage's clock, in minutes, seconds and frames, STEPS_PER_SECOND to
     * a second. stage_clock_step runs it */
    Sint8 minute;
    Sint8 second;
    Sint8 frame;
    /* not 0 in a two-player game, which loads the two-player voices and
     * set files. while it is 0, vibration_start and vibration_start_fading
     * refuse every player but 0, except on STAGE_CHAO */
    Sint8 two_players;
    /* while it isn't 0, the stage has a camera for each screen, and draws its
     * tasks once for each. while it is 1, a player's action window draws
     * only on that player's screen */
    Sint8 split_screen;
    /* while it isn't 0, a stage under split_screen has four cameras, where
     * it otherwise has two. stage_start sets it for STAGE_CHAO */
    Sint8 four_cameras;
    /* while it isn't 0, viewport_set gives each camera the whole screen, and
     * a stage under split_screen draws its tasks for the first camera only */
    Sint8 single_view;
    Sint8 mission; /* a MISSION_ value */
    Sint8 music_on; /* music plays only while it isn't 0 */
    Sint8 voices_on; /* voices play only while it isn't 0 */
    /* while this or unk_09 is set, Lost Colony and Final Rush keep their fog
     * clear */
    Sint8 unk_25;
    Uint8 unk_26[3];
    /* which of 8 pairs of areas of the screen viewport_set gives two
     * cameras */
    Sint8 split_layout;
    Uint8 unk_2a;
    /* 1 from camera_motion_start until the motion ends */
    Sint8 camera_motion_playing;
    /* by player, in a two-player game: the three levels the player's rings
     * earn, each 1 while earned. ring_add sets one and clears the other two, as
     * hud/hud.h says. a versus stage sets them without rings. a player's action
     * clears the level it spends. a hit that counts clears the player's three,
     * and the stage's reset clears them all */
    Sint8 ring_levels[GAME_PLAYERS][RING_LEVELS];
    /* by player: 60 at each new ring level, which stage_clock_step counts
     * down to 0. ring_add and the versus stages set it */
    Sint8 ring_level_timer[GAME_PLAYERS];
    /* by player: the rounds of a two-player match the player has won.
     * stage_clear counts one, and GAME_MODE_STAGE and GAME_MODE_DEMO clear
     * both. the first to win ROUNDS_TO_WIN rounds wins the match */
    Sint8 round_wins[GAME_PLAYERS];
    /* by player: the matches won. stage_clear counts one for the player
     * whose round ends a match, and each round under TWO_PLAYERS_1_POINT */
    Sint8 match_wins[GAME_PLAYERS];
    /* the players a versus time stop holds, each by its TIME_STOPPED_BIT.
     * when Sonic or Shadow stops time, the other player's bit is set. Sonic
     * and Shadow pick no mode and run no moves while their own slot's bit is
     * set. while it isn't 0, the enemies and effects that read it pause */
    Sint8 time_stopped;
    /* a bit for each player, bit 0 for player 0. only a player whose bit is
     * set takes an SG ring. while unk_09 is 0, an SG ring draws only on the
     * screens of such players */
    Sint8 sg_players;
    /* mini_event_start sets it to 1, and mini_event_destroy sets it to 0.
     * mini_event_serve_requests loads and starts an event only while it is
     * 0. other code writes it too */
    Sint8 mini_event_running;
    Sint8 idle_dim; /* fade_idle_step does nothing while it is 0 */
    /* 1 once world_load_block_grid loads a grid. world_free_block_grid
     * clears it, and so does each emptying of the world. while it is 0,
     * land_blocks takes every block of land_block_mask */
    Sint8 block_grid_loaded;
    /* while it isn't 0, the Chaos pieces and the ring chain draw from their
     * builds. while it is 1, the land draws each built entry from its build
     * too. main.c starts it at 1 */
    Sint8 builds_drawn;
    /* the cameras, as HIDDEN_CAMERA_ bits, that draw neither the land nor
     * task levels 2 to 5. the ring chain draws for neither camera while any
     * bit is set. the stage's reset clears it */
    Sint8 hidden_cameras;
    /* 1 while the pads drive the game, and 0 while they don't: pad_enable
     * and pad_disable set it */
    Sint8 pads_on;
    /* for players 0 and 1: 1 while the player's own pad drives the player,
     * and 0 while it doesn't. pad_enable_player and pad_disable_player set
     * them. system/pad.h says what follows each */
    Sint8 player_pads_on[GAME_PLAYERS];
    Sint8 rank; /* the stage's rank, from 0 */
    /* 1 from stage_clear until play next starts, or the stage stops. while
     * it is 1, enemies find no player, and Start opens no pause menu */
    Sint8 cleared;
    /* 1 from stage_pose_winner until play next starts, or the stage stops.
     * while it is 0, save_read takes the save's emblems into emblems */
    Sint8 winner_posed;
    /* 1 once a Chao key's flight takes the key. a clear then leads to
     * STAGE_CHAO through a fade to white, and save_write marks the first
     * player's character as played. the stage's reset clears it, but in the
     * relay's stages that follow another */
    Sint8 chao_key;
    Sint8 edition; /* an EDITION_ value, or another that no program tests */
    /* game_mode_step sets it to TRIAL_SCREEN_TITLE where a reset or a stage's
     * OVERLAY_BACK goes back to GAME_MODE_TO_MENU, and to 0 on the way into
     * the menus */
    Sint8 trial_screen;
    /* GAME_MODE_BOOT sets it to CARD_WARNING_NO_CARD or CARD_WARNING_NO_ROOM
     * on the way to the card warning, whose end clears it. in a stage it
     * holds the result of save.h's save_check that the stage's step last took
     * or set. an error there leaves play for a message about the save */
    Sint8 card_warning;
    /* while it isn't 0, the title and the logos of OTHER_PRINT_LOGOS show
     * SEGA's logo in its US form */
    Uint8 us_sega_logo;
    /* 0 in play. otherwise 1 more than a player's slot: each of the stage's
     * steps then runs only the tasks of level 0 and that player's task */
    Sint8 solo_player;
    Sint8 vibration; /* no vibration starts while it is 0 */
    /* in the last boss's fight, the slot of the player in play. while it
     * isn't 0, the HUD counts player 1's rings. the stage's reset clears it */
    Uint8 active_player;
    /* the character of the player who won a two-player game's last round,
     * as stage_clear takes it from the player's record. GAME_MODE_STAGE and
     * GAME_MODE_DEMO set it to -1 */
    Sint8 round_winner;
    /* 1 in a stage of the relay, stage.h's STAGE_RELAY_ stages, that a
     * stage's STAGE_LEAVE went on to. stage_start then starts at STAGE_ENTER,
     * and keeps the counts the relay carries over. GAME_MODE_STAGE clears
     * it */
    Sint8 relay;
    /* the clock as the stage's play began: 0, or as the relay's stage
     * before left it. it is a kept count, and hud/hud.h says what keeps and
     * restores the kept counts */
    Sint8 kept_minute;
    Sint8 kept_second;
    Sint8 kept_frame;
    Sint8 message_window_on; /* the message window draws only while it is 1 */
    Sint8 unk_53; /* Big the Cat appears only while it isn't 0 */
    /* by player, in a two-player game: the COSTUME_ costume of
     * player/player.h the player wears, or 0. the makers of Sonic, Shadow,
     * Knuckles, Rouge and both mechs load its files */
    Sint8 costume[GAME_PLAYERS];
    /* by player, in a two-player game: not 0 where the player takes its
     * character's other look. the same makers read it. the other looks are Amy
     * for Sonic, Metal Sonic for Shadow, Tikal for Knuckles and Chaos 0 for
     * Rouge. the mechs' are player.h's PLAYER_KIND_BWALK for Eggman's and
     * PLAYER_KIND_CWALK for Tails's. screens_fill shows the other look's
     * animation on the player's memory card */
    Sint8 alt_character[GAME_PLAYERS];
    /* a CHAO_RACE_MENU_ value. advertise_init sets CHAO_RACE_MENU_NONE */
    Sint8 chao_race_menu;
    /* 1 once the menus' step returns OVERLAY_BACK. main then leaves the
     * game for the browser */
    Sint8 leave_to_browser;
    /* 1 from the menus' start and from the ending's end. while it is 1 and
     * menu_resume_target is 0, GAME_MODE_MENU passes the menus over */
    Sint8 skip_menus;
    Sint8 unk_5b; /* Big the Cat appears only while it is 0 */
    /* 1 from system_boot_init where a PAL cable finds no display in the
     * ntUtl flag. GAME_MODE_START then goes straight to the menus, and their
     * end clears it and starts them again */
    Sint8 display_pending;
    /* a drive of the memory cards, 0 to 7: two to a port, A's first. the
     * card warning for SAVE_NOT_READY names its port and its socket */
    Sint8 save_drive;
    /* the menus' theme, which the options choose. it picks the menus'
     * color, and which of a voice set's VOICE_SET_VOICES voices
     * voice_set_play plays, as sound/voice_set.h says. under
     * THEME_COSTUME_H and THEME_COSTUME_X the versus select gives both
     * players that costume. save_data keeps it as theme */
    Sint8 theme;
    Sint8 network_info_found; /* 1 once network_info_poll has found it */
    Sint16 lives[GAME_PLAYERS];
    Sint16 rings[GAME_PLAYERS];
    /* player 0's rings as the stage's play began, a kept count */
    Sint16 kept_rings;
    /* the rings the stage holds: set_tally sets it to kept_ring_total plus
     * what the set file's entries tally, and a ring item adds its own, as
     * does a hunter's dig that finds rings. the score result gives an A to
     * player 0's rings where they reach it */
    Sint16 ring_total;
    Sint16 kept_ring_total;
    /* save_data's emblem count, as save_read and save_clear copy it */
    Sint16 emblems;
    /* the steps since the game started: main adds each frame's steps. its
     * low 5 bits place camlin_draw's moving mark, in 32nds of the way from a
     * node to the next */
    Uint32 total_steps;
    /* the game's steps, counted while time_stopped is 0 */
    Uint32 steps;
    /* the game's mode, an enum game_mode of main.h, which game_mode_step
     * runs. screens_fill picks the screens' loops by it */
    Sint32 mode;
    /* the mode after the event, the menus or the ending that runs */
    Sint32 next_mode;
    /* the mode after GAME_MODE_EMBLEM or GAME_MODE_OTHER_PRINT */
    Sint32 resume_mode;
    Uint8 unk_84[4];
    Uint32 score; /* score_add's: 0 to 99999999 */
    /* the score as the stage's play began, a kept count */
    Uint32 kept_score;
    Uint32 idle_calls; /* fade_idle_step's calls in a row without input */
    /* by camera: land_block_mask holds the land blocks the land may draw.
     * land_blocks holds those it last drew from. it is land_block_mask less the
     * blocks the block grid leaves out where the camera stands */
    Sint32 land_block_mask[2];
    Sint32 land_blocks[2];
    /* the index in save_data's kart_records of the record that
     * save_record_kart_time ranks a time in */
    Sint32 kart_course;
    /* 1 where a download kart course is raced in the characters' karts, and
     * 0 where the first racer drives the course's own kart. at 1 the race
     * takes the course's second speeds, and keeps and draws no best time.
     * ADVERTISE.PRS's download menu sets it to 1 for a kart course of kind 3,
     * and to 0 for one of kind 4. its kart race menu sets it to 1 where its
     * course row of download files is chosen */
    Sint32 kart_character_karts;
    /* set on the way to GAME_MODE_OTHER_PRINT: OTHER_PRINT_LOGOS where the
     * menus go to it, and OTHER_PRINT_ENDING where game_mode_divert does */
    Sint32 other_print;
    /* the number of save_award's last award, or 0 for none. save_award sets
     * unlock_pending to 1 with each award, and awards nothing while it isn't
     * 0 */
    Sint32 unlock_id;
    Sint32 unlock_pending;
    /* the times GAME_MODE_MENU has passed the menus over. every third time
     * goes to GAME_MODE_OTHER_PRINT, and the others start
     * SEQUENCE_MENU_EVENT. the ending's end clears the count */
    Sint32 menu_passes;
    /* the date of save_data as save_clear made it or save_read took it in.
     * save_check takes a file whose date differs for another save's file */
    Uint32 save_date;
    /* the last frame's times, in system/timer.h's thousandths of a vertical
     * blank. the stage's step picks a frame's steps by them. frame_time runs
     * from timer_frame_start to timer_frame_end, and render_time from
     * timer_frame_start to the end of the last render. draw_time is what the
     * stage's step last spent drawing, from the tasks' draws on */
    Float frame_time;
    Float draw_time;
    Float render_time;
    Float unk_cc; /* 1 as the program starts */
    Sint32 cable; /* the SYE_CBL syCblCheck returned to system_boot_init */
};
extern struct game_state game_state;
#ifdef NONMATCHING
/* the second names of game_state's fields, each defined as its field */
#define game_state_text_language (game_state.text_language)
#define game_state_two_players (game_state.two_players)
#define game_state_time_stopped (game_state.time_stopped)
#define game_state_mini_event_running (game_state.mini_event_running)
#define game_state_cleared (game_state.cleared)
#define game_state_solo_player (game_state.solo_player)
#define game_state_steps (game_state.steps)
#define game_state_land_block_mask_0 (game_state.land_block_mask[0])
#define game_state_ring_level_timer_1 (game_state.ring_level_timer[1])
#define game_state_mission (*(volatile Sint8 *)&game_state.mission)
#else
/* symbols.txt places each of these second names at the field it names after
 * game_state_. game_state_land_block_mask_0 is land_block_mask's element 0,
 * and game_state_ring_level_timer_1 is ring_level_timer's element 1.
 * game_state_mission is volatile. each is declared only for the hacks tagged
 * at its uses */
extern Sint8 game_state_text_language;
extern Sint8 game_state_two_players;
extern Sint8 game_state_time_stopped;
extern Sint8 game_state_mini_event_running;
extern Sint8 game_state_cleared;
extern Sint8 game_state_solo_player;
extern Uint32 game_state_steps;
extern Sint32 game_state_land_block_mask_0;
extern Sint8 game_state_ring_level_timer_1;
extern volatile Sint8 game_state_mission;
#endif
/* game_state under a second name, which symbols.txt places at its address.
 * each named field lies where game_state's field of that name does. the
 * vines' debug previews write point over game_state's first 12 bytes */
struct game_state_point {
    NJS_POINT3 point;
    Uint8 unk_0c[0x2c];
    Sint8 time_stopped;
    Uint8 unk_39[0x3b];
    Uint32 steps;
    Uint8 unk_78[0x1c];
    Sint32 land_block_mask_0;
    Sint32 land_block_mask_1;
    Sint32 land_blocks_0;
    Sint32 land_blocks_1;
};
extern struct game_state_point dat_8c0db20c;

/* the fields that stage objects reach through dat_8c0db20c. camera must be
 * the literal 0 or 1 */
#ifdef NONMATCHING
/* game_state's own fields: this side's objects don't define dat_8c0db20c,
 * which only symbols.txt places */
#define STAGE_TIME_STOPPED (game_state.time_stopped)
#define STAGE_FRAME_COUNT (game_state.steps)
#define STAGE_LAND_MASK(camera) (game_state.land_block_mask[(camera)])
#define STAGE_VISIBLE_LAND(camera) (game_state.land_blocks[(camera)])
#else
#define STAGE_TIME_STOPPED (dat_8c0db20c.time_stopped)
#define STAGE_FRAME_COUNT (dat_8c0db20c.steps)
#define STAGE_LAND_MASK(camera) (dat_8c0db20c.land_block_mask_##camera)
#define STAGE_VISIBLE_LAND(camera) (dat_8c0db20c.land_blocks_##camera)
#endif

#endif
