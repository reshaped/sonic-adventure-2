/* the current stage and its players' characters. stage_step runs a stage
 * from its title card to its end */
#include <mathf.h>
#include <ninja.h>
#include <sg_bup.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "camera/motion.h"
#include "camera/stack.h"
#include "camera/viewport.h"
#include "collision/collision.h"
#include "collision/polygon.h"
#include "collision/world.h"
#include "draw/fade.h"
#include "draw/light.h"
#include "draw/text_sprite.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "hud/item_stock.h"
#include "hud/texture_lists.h"
#include "hud/title_card.h"
#include "main.h"
#include "object/animal.h"
#include "object/fall.h"
#include "object/set.h"
#include "player/action.h"
#include "player/eggman.h"
#include "player/knuckles.h"
#include "player/player.h"
#include "player/sonic.h"
#include "player/tails.h"
#include "player/walker.h"
#include "save/card.h"
#include "save/save.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/hunt.h"
#include "stage/stage.h"
#include "stage/winner_camera.h"
#include "system/angle.h"
#include "system/flags.h"
#include "system/frame.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/sequence.h"
#include "system/task.h"
#include "system/text.h"
#include "system/texture.h"
#include "system/timer.h"
#include "vmu/lcd.h"
#include "vmu/vibration.h"

#ifdef NONMATCHING
/* present_step, which this side's loops call after each step */
#include "engine/dc/port.h"
#endif

/* the sound of a clear that leads to STAGE_CHAO */
#define SOUND_TO_CHAO 0x8012

/* the stage's texts in the text file: the pause menu's rows, and the
 * stages' names. a stage's name is at TEXT_STAGE_NAMES plus the stage */
#define TEXT_CONTINUE 2
#define TEXT_RESTART 3
#define TEXT_QUIT 4
#define TEXT_STAGE_NAMES 53

/* the blocks STAGE_CHAO needs free on the save's card */
#define CHAO_BLOCKS 58
/* the bytes of a formatted message, its NUL included */
#define MESSAGE_TEXT_SIZE 512

/* a demo's PRESS START: its texture's global index, and the bit of
 * game_state.total_steps that shows it, 64 steps in every 128 */
#define PRESS_START_TEXTURE 100000310
#ifdef NONMATCHING
/* this side shows PRESS START only once its texture has loaded */
#define PRESS_START_READY() texture_slot_loaded(&press_start_texlist, 0)
#else
#define PRESS_START_READY() TRUE
#endif
#define PRESS_START_BLINK 0x40

/* the console's textures that make the pause menu's frame, whose title is
 * that of the player who paused */
#define FRAME_END 18
#define FRAME_CORNER 19
#define FRAME_BAR 20
#define FRAME_SIDE 21
#define FRAME_MIDDLE 22
#define FRAME_TITLE_PLAYER_0 23
#define FRAME_TITLE_PLAYER_1 24
/* the frame's measures in pixels. its top is a band from FRAME_EDGE above
 * the top corners to FRAME_EDGE below them. its sides and its bottom are
 * FRAME_EDGE thick */
#define FRAME_EDGE 8.0F
/* the width of each top corner's quad */
#define FRAME_CORNER_WIDTH 16.0F
/* where the title starts and ends across the top */
#define FRAME_TITLE_LEFT 256.0F
#define FRAME_TITLE_RIGHT 384.0F
#define FRAME_COLOR 0xffffffff

/* the depth of the pause menu frame */
#define FRAME_DEPTH 0.9087F
/* the height the pause menu gives a row */
#define ROW_HEIGHT 32.0F
/* how far a row's or the stage name's shadow lies right of and below its
 * text */
#define SHADOW_OFFSET 2.0F

/* the screen's center. a message and the pause menu's rows center on it */
#define SCREEN_CENTER_X 320
#define SCREEN_CENTER_Y 240
/* a message's colors: its text's, its shadow's and its band's */
#define MESSAGE_COLOR 0xffe0e0e0
#define MESSAGE_SHADOW_COLOR 0xff503010
#define MESSAGE_BAND_COLOR 0xc0b46c28
/* message_draw's depths, the band farthest */
#define BAND_DEPTH 1.0009F
#define MESSAGE_SHADOW_DEPTH 1.001F
#define MESSAGE_DEPTH 1.0011F
/* center message on the screen, with its band's top at its text's, and give it
 * the message's colors */
#define MESSAGE_PLACE()                                                        \
    do {                                                                       \
        if (message) {                                                         \
            message->x = SCREEN_CENTER_X - (message->width >> 1);              \
            message->y = SCREEN_CENTER_Y - (message->height >> 1);             \
            message->band_y = message->y;                                      \
            message->color.color = MESSAGE_COLOR;                              \
            message->shadow_color.color = MESSAGE_SHADOW_COLOR;                \
            message->band_color.color = MESSAGE_BAND_COLOR;                    \
        }                                                                      \
    } while (0)

/* the stage's name: its center's height, and the colors and depths of its text
 * and its shadow */
#define NAME_CENTER_Y 224.0F
#define NAME_COLOR 0xffc0c0c0
#define NAME_SHADOW_COLOR 0xff303030
#define NAME_DEPTH 1.0002F
#define NAME_SHADOW_DEPTH 1.0001F

/* the frames STAGE_CARD_NOTICE lasts, and those the banner of a two-player
 * game shows for */
#define NOTICE_FRAMES 300
#define BANNER_FRAMES 120
/* the frames a stage's name shows for, and those STAGE_RESET waits */
#define NAME_FRAMES 10
#define RESET_FRAMES 10

/* the fog's near and far depths from a stage's load, as njGenerateFogTable3
 * takes them */
#define FOG_NEAR (-7000.0F)
#define FOG_FAR (-8000.0F)

/* the steps a frame takes at most. under DISPLAY_PAL, one frame in every
 * PAL_FRAMES takes one more */
#define FRAME_STEPS 4
#define PAL_FRAMES 5
/* a frame's vertical blanks and steps in a one-player game, then in a
 * two-player game. stage_run gives a two-player game PAIR_STEPS at least */
#define SOLO_BLANKS 1
#define SOLO_STEPS 1
#define PAIR_BLANKS 2
#define PAIR_STEPS 2
/* the cameras of a split screen without four_cameras */
#define SPLIT_CAMERAS 2
/* a color channel at its most. the fades to white take it for each */
#define FULL 0xff
/* the task level of the overlay's own task */
#define OVERLAY_TASK_LEVEL 2
/* the vertical blanks in one of system/timer.h's units, a thousandth of a
 * blank. stage_run adds DRAW_MARGIN units to draw_time */
#define BLANKS_PER_TIME 0.001F
#define DRAW_MARGIN 120.0F

/* the pause menu's choices, in the order of its rows. a menu without
 * PAUSE_RESTART has PAUSE_ROWS_NO_RESTART rows */
enum pause_choice { PAUSE_CONTINUE, PAUSE_RESTART, PAUSE_QUIT, PAUSE_ROWS };
#define PAUSE_ROWS_NO_RESTART (PAUSE_ROWS - 1)

/* the buttons that hide the pause menu while a player holds them together */
#define PAUSE_HIDE_BUTTONS (PDD_DGT_TX | PDD_DGT_TY)
/* the most frames the pause menu ignores a direction held since it
 * opened */
#define PAUSE_HOLD_FRAMES 240

/* empty_slot_steps before any slot is found empty */
#define NO_STEPS 0xffffffff

/* the flags a posed player's record loses */
#define PLAYER_POSE_LOSES                                                      \
    (PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER | PLAYER_INVINCIBLE)

/* how far below a placed player its record's ground.below_y starts */
#define BELOW_START 10.0F

/* start_place's places, by index: a one-player game's, then player 0's and
 * player 1's of two */
enum { PLACE_ONE_PLAYER, PLACE_PLAYER_0, PLACE_PLAYER_1, PLACES };
/* pair_place's places, by index. an intro's are for player 0 and player 1, and
 * a mission win's for MISSION_RINGS and MISSION_CHAO */
enum { PAIR_PLAYER_0, PAIR_PLAYER_1, PAIR_PLACES };
enum { PAIR_RINGS_WINNER, PAIR_CHAO_WINNER };

/* a character's places in one stage, each a turn about y and a position. a
 * table of them, by stage, ends at STAGE_NONE */
struct start_place {
    Sint16 stage;
    Sint16 ang_y[PLACES];
    NJS_POINT3 pos[PLACES];
};

/* a character's two places in one stage, each as start_place's. an intro places
 * player 0 and player 1 before a two-player stage starts. a mission win places
 * the winner of MISSION_RINGS or of MISSION_CHAO. a table of them ends at
 * STAGE_NONE */
struct pair_place {
    Sint16 stage;
    Sint16 ang_y[PAIR_PLACES];
    NJS_POINT3 pos[PAIR_PLACES];
};

/* a mission win's turn for a mission with no place of its own. its winner
 * should then take the stage's place for a win. place_winner_by_mission's test
 * for it never holds */
#define NO_PLACE (-1)

/* a player's restart point, and the stage as it stood when
 * restart_point_keep kept it */
struct restart_point {
    Uint16 stage; /* the stage the point was kept in */
    Sint16 order; /* the order restart_point_keep gave, or -1 for none */
    /* the stage's clock */
    Sint8 minute;
    Sint8 second;
    Sint8 frame;
    NJS_POINT3 pos;
    struct angles ang;
    Angle gravity_ang_x;
    Angle gravity_ang_z;
    Sint32 up_axis; /* a COLLISION_UP_ axis */
};

static void warn_no_pad(void);
static void card_notice_start(void);
static void card_notice_no_room(void);
static void card_error_start(void);
static void card_error_show(void);
static void message_end(void);
static void leave_play(void);
static void play_rewind(void);
static void stage_go_back_now(void);
static void stage_go_next(void);
#ifdef NONMATCHING
/* void, the plain form the hack in its last case names */
static void stage_leave_cleared(void);
#else
static int stage_leave_cleared(void);
#endif
static Sint16 end_kind(void);
static void forget_end(void);
static void stage_reset_play(void);
static void play_reset_state(void);
static void stage_reset(void);
static void stage_load(void);
static void play_start(void);
static void stage_run_solo(void);
static int stage_run(void);
static void stage_run_held(void);
static Sint16 stage_run_warning(void);
static Sint16 stage_run_notice(void);
static Sint16 stage_run_paused(void);
static int stage_run_fading(void);
static void play_stop(void);
static void stage_unload(void);
static void stage_create_players(void);
static void stage_create_task(void);
static void stage_restart_players(void);
static void restart_point_apply(int player, NJS_POINT3 *pos,
                                struct angles *ang);
static void pause_draw(void);
static void stage_name_show(void);
static void banner_draw(void);
static void message_draw(struct text_sprite *sprite);

/* where each character starts a stage */
static struct start_place sonic_starts[16] = {
#include "stage/stage/sonic_starts.inc"
};

static struct start_place shadow_starts[13] = {
#include "stage/stage/shadow_starts.inc"
};

static struct start_place knuckles_starts[11] = {
#include "stage/stage/knuckles_starts.inc"
};

static struct start_place rouge_starts[12] = {
#include "stage/stage/rouge_starts.inc"
};

static struct start_place tails_starts[2] = {
#include "stage/stage/tails_starts.inc"
};

static struct start_place eggman_mech_starts[15] = {
#include "stage/stage/eggman_mech_starts.inc"
};

static struct start_place tails_mech_starts[13] = {
#include "stage/stage/tails_mech_starts.inc"
};

static struct start_place super_sonic_starts[3] = {
#include "stage/stage/super_sonic_starts.inc"
};

static struct start_place super_shadow_starts[3] = {
#include "stage/stage/super_shadow_starts.inc"
};

/* where each character poses as a stage's winner, laid out as its starts */
static struct start_place sonic_wins[17] = {
#include "stage/stage/sonic_wins.inc"
};

static struct start_place shadow_wins[15] = {
#include "stage/stage/shadow_wins.inc"
};

static struct start_place knuckles_wins[11] = {
#include "stage/stage/knuckles_wins.inc"
};

static struct start_place rouge_wins[13] = {
#include "stage/stage/rouge_wins.inc"
};

static struct start_place eggman_mech_wins[15] = {
#include "stage/stage/eggman_mech_wins.inc"
};

static struct start_place tails_mech_wins[13] = {
#include "stage/stage/tails_mech_wins.inc"
};

static struct start_place super_sonic_wins[3] = {
#include "stage/stage/super_sonic_wins.inc"
};

static struct start_place super_shadow_wins[3] = {
#include "stage/stage/super_shadow_wins.inc"
};

/* where each character stands before a two-player stage starts, as player
 * 0 and as player 1 */
static struct pair_place sonic_intros[5] = {
#include "stage/stage/sonic_intros.inc"
};

static struct pair_place shadow_intros[5] = {
#include "stage/stage/shadow_intros.inc"
};

static struct pair_place knuckles_intros[4] = {
#include "stage/stage/knuckles_intros.inc"
};

static struct pair_place rouge_intros[4] = {
#include "stage/stage/rouge_intros.inc"
};

static struct pair_place eggman_mech_intros[4] = {
#include "stage/stage/eggman_mech_intros.inc"
};

static struct pair_place tails_mech_intros[4] = {
#include "stage/stage/tails_mech_intros.inc"
};

/* where each character poses as the winner of MISSION_RINGS and of
 * MISSION_CHAO */
static struct pair_place sonic_mission_wins[9] = {
#include "stage/stage/sonic_mission_wins.inc"
};

static struct pair_place shadow_mission_wins[6] = {
#include "stage/stage/shadow_mission_wins.inc"
};

static struct pair_place knuckles_mission_wins[7] = {
#include "stage/stage/knuckles_mission_wins.inc"
};

static struct pair_place rouge_mission_wins[6] = {
#include "stage/stage/rouge_mission_wins.inc"
};

static struct pair_place eggman_mech_mission_wins[7] = {
#include "stage/stage/eggman_mech_mission_wins.inc"
};

static struct pair_place tails_mech_mission_wins[6] = {
#include "stage/stage/tails_mech_mission_wins.inc"
};

/* 1 while stage_go_back_now or stage_place_player_again runs, and 0
 * otherwise. while it is 1, the clock, the score and the rings stay as they
 * are */
static Sint8 keep_counts = 0;
static Sint16 stage_end_kind = STAGE_END_NONE;
/* the message a mode shows, or null */
static struct text_sprite *message = NULL;
/* the pause menu's rows, each null where its text couldn't be made */
static struct text_sprite *pause_continue_text = NULL;
static struct text_sprite *pause_restart_text = NULL;
static struct text_sprite *pause_quit_text = NULL;
/* 0 while the pause menu takes directions. a direction held as the menu opens
 * makes it count frames from 1. it counts while the direction stays held, up to
 * PAUSE_HOLD_FRAMES */
static Sint16 pause_hold = 0;
static Sint16 notice_timer = 0; /* the frames STAGE_CARD_NOTICE has left */
/* a sprite of the stage's name, for STAGE_TITLE_CARD to draw in place of a
 * title card. no code makes it, so it stays null */
static struct text_sprite *stage_name_text = NULL;
static Sint16 banner_timer = 0; /* the frames the banner has left */
/* counts DISPLAY_PAL's frames from 0 to PAL_FRAMES - 1 */
static Sint8 pal_frames = 0;

/* the PRESS START a demo draws */
#include "stage/stage/text.inc"

static NJS_TEXNAME press_start_texname[1] = {
#include "stage/stage/press_start_texname.inc"
};
static NJS_TEXLIST press_start_texlist = {
#include "stage/stage/press_start_texlist.inc"
};
static NJS_TEXTURE_VTX press_start_quad[4] = {
#include "stage/stage/press_start_quad.inc"
};

/* the most rounds of a two-player match, and the stages each pair of
 * characters plays them on */
#define MATCH_STAGES 3

/* what stage_step does each frame: an enum stage_mode */
static Sint16 stage_mode;
static Sint16 stage_retries;
static Sint16 stage_current;
static Sint16 stage_previous;
static Sint16 stage_loaded; /* the stage stage_load last loaded */
static Sint16 stage_next;
/* the characters stage_character returns: slot 0's, and every other
 * slot's */
static Sint32 first_character;
static Sint32 other_character;
/* the stage each round of a two-player match played, or -1. stage_start
 * sets every entry to -1 before each round, so stage_leave_cleared finds
 * only the round just won */
static Sint16 match_played[MATCH_STAGES];
static Sint8 reset_timer; /* the frames STAGE_RESET has left */
static Sint8 pause_choice; /* an enum pause_choice */
static Sint8 pause_rows; /* PAUSE_ROWS or PAUSE_ROWS_NO_RESTART */
/* -1 while PAUSE_HIDE_BUTTONS hide the pause menu, and 0 otherwise */
static Sint8 pause_hidden;
static Uint8 dat_8c24ee80[16];
static struct restart_point restart_points[GAME_PLAYERS];

void stage_boot(void)
{
    stage_set_character(0, PLAYER_SONIC);
    srand(RAND_SEED); /* NOLINT(bugprone-random-generator-seed) */
}

Bool stage_start(void)
{
    struct text_sprite *(*create)(const char *text, int language);
    int stage;

    stage_mode = STAGE_ENTER;
    stage_end_kind = STAGE_END_NONE;
    keep_counts = 0;
    pad_clear_reset();
    if (game_state.relay == 0) {
        stage_mode = STAGE_TITLE_CARD;
        kept_clock_clear();
        kept_rings_clear();
        kept_score_clear();
        kept_ring_total_clear();
        kept_object_bits_clear();
        kept_animals_clear();
    }
    match_played[0] = -1;
    match_played[1] = -1;
    match_played[2] = -1;
    world_reset_gravity();
    pause_choice = PAUSE_CONTINUE;
    pause_hidden = 0;
    if (game_state.two_players) {
        game_state.mission = MISSION_FIRST;
        if ((stage_character(0) == PLAYER_EGGMAN_MECH ||
             stage_character(0) == PLAYER_TAILS_MECH) &&
            (stage_character(1) == PLAYER_EGGMAN_MECH ||
             stage_character(1) == PLAYER_TAILS_MECH)) {
            if (stage_current != STAGE_CART_2) {
                game_state.split_screen = 0;
            }
        } else {
            game_state.split_screen = 1;
        }
    } else {
        stage = stage_current;
        if ((stage > STAGE_STG_END && stage != STAGE_CART_1) ||
            STAGE_VERSUS(stage)) {
            game_state.mission = MISSION_FIRST;
        }
        game_state.split_screen = 0;
    }
    task_init();
#ifdef NONMATCHING
    /* this side starts no stage without its language files, as stage.h says */
    if (!text_read()) {
        return FALSE;
    }
#else
    text_read();
#endif
    /* @hack spelling: create, a local for text_sprite_create, where each call
     * would name the function */
    create = text_sprite_create;
    message = create(MCWARN(MCWARN_SOCKET), game_state.text_language);
    MESSAGE_PLACE();
    pause_continue_text = create(TEXT(TEXT_CONTINUE), game_state.text_language);
    pause_restart_text = create(TEXT(TEXT_RESTART), game_state.text_language);
    pause_quit_text = create(TEXT(TEXT_QUIT), game_state.text_language);
    texture_load_pvm("PRESSSTART", &press_start_texlist);
    if (game_state.demo) {
        srand(RAND_SEED); /* NOLINT(bugprone-random-generator-seed) */
    }
    /* @hack spelling: two switches, the second on stage_current read
     * again, where an if on stage would do for each */
    stage = stage_current;
    switch (stage) {
    case STAGE_CHAO:
        game_state.split_screen = 1;
        game_state.four_cameras = 1;
        camera_start(CAMERAS);
        loader_read_camera_files();
        game_state.single_view = 1;
        break;
    default:
        if (game_state.split_screen) {
            if (game_state.four_cameras) {
                camera_start(CAMERAS);
            } else {
                camera_start(SPLIT_CAMERAS);
            }
        } else {
            camera_start(1);
        }
        break;
    }
    switch (stage_current) {
    case STAGE_CART_1:
    case STAGE_CART_2:
        camera_hold(stage_current);
        break;
    default:
        players_init();
        stage_create_players();
        break;
    }
    set_init();
    world_alloc();
    enemy_groups_clear();
    sound_task_create(NULL);
    sound_load_end();
    sound_resume_all();
    item_stock_start();
    card_init();
    lcd_start();
    return TRUE;
}

Bool stage_stop(void)
{
    task_destroy_all();
    players_exit();
    sound_task_kill();
    item_stock_stop();
    camera_stop();
    world_free();
    set_exit();
    njReleaseTexture(&press_start_texlist);
    text_sprite_destroy(message);
    message = NULL;
    if (pause_continue_text) {
        text_sprite_destroy(pause_continue_text);
        pause_continue_text = NULL;
    }
    if (pause_restart_text) {
        text_sprite_destroy(pause_restart_text);
        pause_restart_text = NULL;
    }
    if (pause_quit_text) {
        text_sprite_destroy(pause_quit_text);
        pause_quit_text = NULL;
    }
    card_exit();
    lcd_stop();
    game_state.winner_posed = 0;
    game_state.cleared = 0;
    return TRUE;
}

/* a pad is gone: pause the sound and the vibration, and show MCWARN_NO_PAD
 * until STAGE_NO_PAD finds the pads back */
static void warn_no_pad(void)
{
    sound_play(SOUND_PAUSE_ID, NULL, 0, 0);
    pause_choice = PAUSE_CONTINUE;
    stage_mode = STAGE_NO_PAD;
    sound_pause_all();
    vibration_stop();
    if (game_state.paused == 0) {
        game_state.paused = PAUSED_BY_GAME;
    }
    if (message) {
        text_sprite_destroy(message);
        message = NULL;
    }
    game_state.card_removed = 0;
    message =
        text_sprite_create(MCWARN(MCWARN_NO_PAD), game_state.text_language);
    MESSAGE_PLACE();
}

/* enter STAGE_CARD_NOTICE, showing the message game_state.card_state picks.
 * where play runs, it pauses the sound and the vibration first */
static void card_notice_start(void)
{
    stage_mode = STAGE_CARD_NOTICE;
    pause_choice = PAUSE_CONTINUE;
    if (game_state.paused == 0) {
        sound_play(SOUND_PAUSE_ID, NULL, 0, 0);
        sound_pause_all();
        vibration_stop();
        game_state.paused = PAUSED_BY_GAME;
    }
    notice_timer = NOTICE_FRAMES;
    if (message) {
        text_sprite_destroy(message);
        message = NULL;
    }
    if (game_state.card_state == CARD_SAVE_FAILED) {
        message = text_sprite_create(MCWARN(MCWARN_SAVE_FAILED),
                                     game_state.text_language);
    } else if (game_state.card_state == CARD_LOAD_FAILED) {
        message = text_sprite_create(MCWARN(MCWARN_LOAD_FAILED),
                                     game_state.text_language);
    } else {
        message = text_sprite_create(MCWARN(MCWARN_NOT_OURS),
                                     game_state.text_language);
    }
    MESSAGE_PLACE();
}

/* enter STAGE_CARD_NOTICE, showing that the card lacks CHAO_BLOCKS free
 * blocks */
static void card_notice_no_room(void)
{
    char text[MESSAGE_TEXT_SIZE];
    char *format;

    sound_play(SOUND_PAUSE_ID, NULL, 0, 0);
    pause_choice = PAUSE_CONTINUE;
    stage_mode = STAGE_CARD_NOTICE;
    sound_pause_all();
    vibration_stop();
    game_state.paused = PAUSED_BY_GAME;
    notice_timer = NOTICE_FRAMES;
    if (message) {
        text_sprite_destroy(message);
        message = NULL;
    }
    format = MCWARN(MCWARN_BLOCKS);
    /* @bug memory: draws through a null message in every language but
     * TEXT_JAPANESE. BF at 0x8c024518 passes over the message's making,
     * which leaves it null, as the test at 0x8c024538 expects. each frame
     * of STAGE_CARD_NOTICE then has message_draw read through it (MOV.W at
     * 0x8c028dec) */
    if (game_state.text_language == TEXT_JAPANESE) {
        sprintf(text, format, CHAO_BLOCKS);
        message = text_sprite_create(text, game_state.text_language);
    }
    MESSAGE_PLACE();
}

/* enter STAGE_CARD_ERROR, pausing the sound and the vibration. the message
 * is the save's socket by MCWARN_SOCKET, or MCWARN_NOT_OURS under
 * SAVE_NOT_OURS */
static void card_error_start(void)
{
    static char *port_letters[PAD_PORTS] = {"A", "B", "C", "D"};
    char text[MESSAGE_TEXT_SIZE];
    char *format;

    sound_play(SOUND_PAUSE_ID, NULL, 0, 0);
    pause_choice = PAUSE_CONTINUE;
    stage_mode = STAGE_CARD_ERROR;
    sound_pause_all();
    vibration_stop();
    if (game_state.paused == 0) {
        game_state.paused = PAUSED_BY_GAME;
    }
    notice_timer = NOTICE_FRAMES;
    if (message) {
        text_sprite_destroy(message);
        message = NULL;
    }
    if (game_state.card_warning == SAVE_NOT_OURS) {
        message = text_sprite_create(MCWARN(MCWARN_NOT_OURS),
                                     game_state.text_language);
    } else {
        /* @hack spelling: format set before the test, and the drive read
         * again for each argument, where a local for the drive would do */
        format = MCWARN(MCWARN_SOCKET);
        if (game_state.text_language == TEXT_JAPANESE) {
            sprintf(text, format,
                    port_letters[SAVE_DRIVE_PORT(save_place.drive)],
                    SAVE_DRIVE_SOCKET(save_place.drive));
        } else {
            sprintf(text, format, SAVE_DRIVE_SOCKET(save_place.drive),
                    port_letters[SAVE_DRIVE_PORT(save_place.drive)]);
        }
        message = text_sprite_create(text, game_state.text_language);
    }
    MESSAGE_PLACE();
}

/* show the message card_error_start shows, in STAGE_CARD_NOT_OURS under
 * SAVE_NOT_OURS and in STAGE_CARD_GONE otherwise. it pauses nothing */
static void card_error_show(void)
{
    static char *port_letters[PAD_PORTS] = {"A", "B", "C", "D"};
    char text[MESSAGE_TEXT_SIZE];
    char *format;

    notice_timer = NOTICE_FRAMES;
    if (message) {
        text_sprite_destroy(message);
        message = NULL;
    }
    if (game_state.card_warning == SAVE_NOT_OURS) {
        stage_mode = STAGE_CARD_NOT_OURS;
        message = text_sprite_create(MCWARN(MCWARN_NOT_OURS),
                                     game_state.text_language);
    } else {
        stage_mode = STAGE_CARD_GONE;
        /* @hack spelling: format set before the test, and the drive read
         * again for each argument, as in card_error_start */
        format = MCWARN(MCWARN_SOCKET);
        if (game_state.text_language == TEXT_JAPANESE) {
            sprintf(text, format,
                    port_letters[SAVE_DRIVE_PORT(save_place.drive)],
                    SAVE_DRIVE_SOCKET(save_place.drive));
        } else {
            sprintf(text, format, SAVE_DRIVE_SOCKET(save_place.drive),
                    port_letters[SAVE_DRIVE_PORT(save_place.drive)]);
        }
        message = text_sprite_create(text, game_state.text_language);
    }
    MESSAGE_PLACE();
}

/* a message is over. the stage goes back to the pause menu where a player
 * holds it, and otherwise to play. the return to play resumes the sound if
 * pause_choice is PAUSE_CONTINUE. message becomes MCWARN_SOCKET again, as
 * stage_start made it */
static void message_end(void)
{
    if (message) {
        text_sprite_destroy(message);
        message = NULL;
    }
    message =
        text_sprite_create(MCWARN(MCWARN_SOCKET), game_state.text_language);
    MESSAGE_PLACE();
    sound_play(SOUND_PAUSE_ID, NULL, 0, 0);
    if (game_state.paused == PAUSED_BY_PLAYER_0 ||
        game_state.paused == PAUSED_BY_PLAYER_1) {
        stage_mode = STAGE_PAUSE;
    } else {
        stage_mode = STAGE_PLAY;
        game_state.paused = 0;
        if (pause_choice != PAUSE_CONTINUE) {
            fun_8c059fb2();
        } else {
            sound_resume_all();
        }
    }
}

int stage_step(void)
{
    int result;

    switch (stage_mode) {
    case STAGE_TITLE_CARD: {
        int stage;

        stage = stage_current_id();
        if (stage < SAVE_STAGES) {
            save_data.stages[stage].plays[game_state.mission]++;
        } else {
            /* @hack spelling: a switch of one case, where an if would do */
            switch (stage) {
            case STAGE_CART_1:
                if (stage_character(0) != PLAYER_ROUGE) {
                    save_data.cart_stage[SAVE_CART_OTHERS]
                        .plays[game_state.mission]++;
                } else {
                    save_data.cart_stage[SAVE_CART_ROUGE]
                        .plays[game_state.mission]++;
                }
                break;
            }
        }
        pad_disable();
        /* @hack spelling: two tests that each set the mode and leave, where
         * demo || two_players would do */
        if (game_state.demo != 0) {
            stage_mode = STAGE_ENTER;
            break;
        } else if (game_state.two_players) {
            stage_mode = STAGE_ENTER;
            break;
        }
        if (stage_name_text) {
            stage_name_show();
            /* stage_name_text stays null, so this never runs. it counts the
             * frames in glyph_color, which holds a pixel */
            if (stage_name_text->glyph_color++ > NAME_FRAMES) {
                stage_mode = STAGE_ENTER_CARD;
            }
        } else if (title_card_start()) {
            stage_mode = STAGE_TITLE_CARD_SHOW;
            fade_set_black();
            fade_draw();
        } else {
            stage_mode = STAGE_ENTER_CARD;
        }
        game_state.mini_event_running = 0;
    } break;
    case STAGE_TITLE_CARD_SHOW:
        task_step_all();
        fade_draw();
        task_draw_all();
        if (end_kind() == STAGE_END_CARD_SHOWN) {
            forget_end();
            stage_mode = STAGE_ENTER_CARD;
        }
        break;
    case STAGE_ENTER_CARD:
        stage_load();
        play_start();
        fade_set_black();
        task_step_all();
        fade_draw();
        task_draw_all();
        stage_mode = STAGE_FADE_IN;
        if (stage_name_text) {
            text_sprite_destroy(stage_name_text);
            stage_name_text = NULL;
        }
        break;
    case STAGE_ENTER:
        pad_disable();
        fade_set_black();
        stage_load();
        play_start();
        stage_mode = STAGE_FADE_IN;
        break;
    case STAGE_AGAIN:
        pad_disable();
        fade_set_black();
        play_start();
        stage_mode = STAGE_FADE_IN;
        break;
    case STAGE_FADE_IN:
        if (sequence_mini_event_due()) {
#ifdef NONMATCHING
            /* this side leaves the flag to mini_event_start, which sets it
             * where it starts the event and refuses an event that didn't
             * load */
            sequence_play_mini_event();
#else
            sequence_play_mini_event();
            game_state.mini_event_running = 1;
#endif
        }
        result = fade_in_step();
        stage_run();
        if (result) {
            stage_mode = STAGE_PLAY;
            stage_clock_resume();
            pad_enable();
            pause_enable();
            fade_set_color(0);
        }
        break;
    case STAGE_PLAY:
        result = stage_run();
        if (end_kind() == STAGE_END_QUIT) {
            stage_mode = STAGE_QUIT_FADE;
            leave_play();
        } else if (end_kind() == STAGE_END_CLEAR) {
            stage_leave_cleared();
            leave_play();
            if (game_state.chao_key == 0 &&
                !STAGE_RELAY_BEFORE_LAST(stage_current) &&
                game_flags[FLAG_SEQUENCE] != SEQUENCE_NONE) {
                sequence_advance();
            }
        } else if (end_kind() == STAGE_END_MISS) {
            if (lives_count(0) <= 0) {
                stage_mode = STAGE_QUIT_FADE;
                stage_set_next(0);
            } else {
                switch (stage_current_id()) {
                case STAGE_RELAY_EGGMAN:
                case STAGE_RELAY_ROUGE:
                case STAGE_RELAY_KNUCKLES:
                case STAGE_RELAY_SONIC:
                    if (game_state.mission == MISSION_TIMED) {
                        stage_set_next(STAGE_RELAY_FIRST);
                        game_state.relay = 0;
                        game_state.chao_key = 0;
                        stage_mode = STAGE_GO_TO_FADE;
                    } else {
                        stage_mode = STAGE_MISS_FADE;
                    }
                    break;
                default:
                    stage_mode = STAGE_MISS_FADE;
                    break;
                }
            }
            leave_play();
        } else if (end_kind() == STAGE_END_GO_TO) {
            stage_mode = STAGE_GO_TO_FADE;
            leave_play();
        } else if (end_kind() == STAGE_END_BACK) {
            if (stage_current_id() == STAGE_RELAY_LAST) {
                stage_set_next(STAGE_RELAY_FIRST);
                game_state.relay = 1;
                stage_mode = STAGE_GO_TO_FADE;
                leave_play();
            } else {
                stage_mode = STAGE_BACK_FADE;
                leave_play();
            }
        } else if (end_kind() == STAGE_END_RESTART) {
            stage_mode = STAGE_RESTART_FADE;
            leave_play();
        } else if (end_kind() == STAGE_END_RETRY) {
            switch (stage_current_id()) {
            case STAGE_RELAY_EGGMAN:
            case STAGE_RELAY_ROUGE:
            case STAGE_RELAY_KNUCKLES:
            case STAGE_RELAY_SONIC:
                stage_set_next(STAGE_RELAY_FIRST);
                game_state.relay = 0;
                game_state.chao_key = 0;
                stage_mode = STAGE_GO_TO_FADE;
                break;
                /* @hack redundant: a store after the break, which never
                 * runs */
                stage_mode = STAGE_RETRY_FADE;
            default:
                stage_mode = STAGE_RETRY_FADE;
                break;
            }
            leave_play();
            sound_stop_all_and_unpause();
        } else if (result) {
            /* player result - 1 pressed Start, which opens the pause menu */
            sound_play(SOUND_PAUSE_ID, NULL, 0, 0);
            pause_choice = PAUSE_CONTINUE;
            stage_mode = STAGE_PAUSE;
            sound_pause_all();
            vibration_stop();
            if (result == 1) {
                game_state.paused = PAUSED_BY_PLAYER_0;
            } else {
                game_state.paused = result = PAUSED_BY_PLAYER_1;
            }
            /* the menu offers PAUSE_RESTART only while every player has a
             * life left, and never in STAGE_CHAO */
            if (stage_current_id() == STAGE_CHAO) {
                pause_rows = PAUSE_ROWS_NO_RESTART;
            } else if ((game_state.two_players != 0 &&
                        game_state.lives[0] > 0 && game_state.lives[1] > 0) ||
                       (game_state.two_players == 0 &&
                        game_state.lives[0] > 0)) {
                pause_rows = PAUSE_ROWS;
            } else {
                pause_rows = PAUSE_ROWS_NO_RESTART;
            }
            if (pad_held[0] & PAD_DIRECTIONS) {
                pause_hold = 1;
            } else {
                pause_hold = 0;
            }
        } else if (game_state.demo == 0) {
            if (!pad_player_is_connected(0) ||
                (!pad_player_is_connected(1) && game_state.two_players != 0)) {
                warn_no_pad();
                break;
            }
            if (game_state.card_state != CARD_IDLE &&
                (game_state.card_removed ||
                 game_state.card_warning >= CARD_WARNING_ERROR)) {
                card_error_start();
            } else if (game_state.card_removed != 0 ||
                       game_state.card_warning >= CARD_WARNING_ERROR) {
                card_error_show();
                sound_play(SOUND_PAUSE_ID, NULL, 0, 0);
                pause_choice = PAUSE_CONTINUE;
                sound_pause_all();
                vibration_stop();
                if (game_state.paused == 0) {
                    game_state.paused = PAUSED_BY_GAME;
                }
            } else if (game_state.card_state < CARD_IDLE) {
                if (stage_current_id() == STAGE_CHAO) {
                    if (game_state.card_state == CARD_LOAD_FAILED) {
                        if (buGetDiskFree(save_place.drive,
                                          BUD_FILETYPE_NORMAL) < CHAO_BLOCKS) {
                            card_notice_no_room();
                        } else {
                            game_state.card_state = CARD_IDLE;
                            game_state.card_warning = SAVE_OK;
                        }
                    } else {
                        card_notice_start();
                    }
                } else {
                    card_notice_start();
                }
            }
        }
        break;
    case STAGE_PAUSE:
        result = stage_run_paused();
        if (!pad_player_is_connected(0) ||
            (!pad_player_is_connected(1) && game_state.two_players)) {
            warn_no_pad();
            break;
            /* @hack redundant: a store after the break, which never runs */
            game_state.card_removed = 0;
        } else if (game_state.card_removed != 0 ||
                   game_state.card_warning >= CARD_WARNING_ERROR) {
            card_error_show();
        } else if ((game_state.paused == PAUSED_BY_PLAYER_0 &&
                    (player_pads[0]->press & PDD_DGT_ST)) ||
                   (game_state.paused == PAUSED_BY_PLAYER_1 &&
                    (player_pads[1]->press & PDD_DGT_ST) &&
                    game_state.two_players)) {
            sound_play(SOUND_PAUSE_ID, NULL, 0, 0);
            stage_mode = STAGE_PLAY;
            game_state.paused = 0;
            sound_resume_all();
        } else if ((game_state.paused == PAUSED_BY_PLAYER_0 &&
                    (player_pads[0]->press & pad_choose_buttons)) ||
                   /* @hack spelling: a unary plus on pad_choose_buttons, which
                    * changes nothing */
                   (game_state.paused == PAUSED_BY_PLAYER_1 &&
                    (player_pads[1]->press & +pad_choose_buttons) &&
                    game_state.two_players)) {
            sound_play(SOUND_PAUSE_ID, NULL, 0, 0);
            if (pause_choice == PAUSE_CONTINUE) {
                stage_mode = STAGE_PLAY;
                game_state.paused = 0;
            } else if (pause_choice == PAUSE_RESTART) {
                switch (stage_current_id()) {
                case STAGE_RELAY_EGGMAN:
                case STAGE_RELAY_ROUGE:
                case STAGE_RELAY_KNUCKLES:
                case STAGE_RELAY_SONIC:
                    stage_set_next(STAGE_RELAY_FIRST);
                    game_state.relay = 0;
                    game_state.chao_key = 0;
                    stage_mode = STAGE_GO_TO_FADE;
                    leave_play();
                    break;
                    /* @hack redundant: a store after the break, which never
                     * runs */
                    stage_mode = STAGE_RETRY_FADE;
                default:
                    stage_mode = STAGE_RETRY_FADE;
                    leave_play();
                    break;
                }
            } else {
                play_stop();
                stage_set_next(0);
                stage_mode = STAGE_QUIT;
            }
            if (pause_choice != PAUSE_CONTINUE) {
                fun_8c059fb2();
                sound_resume();
                music_stop();
            } else {
                sound_resume_all();
            }
        }
        break;
    case STAGE_NO_PAD:
        result = stage_run_warning();
        if (pad_player_is_connected(0) &&
            (pad_player_is_connected(1) || !game_state.two_players)) {
            struct card_status status;

            if (save_place.drive == SAVE_DRIVE_REFUSED) {
                game_state.card_warning = SAVE_OK;
                game_state.card_removed = 0;
                message_end();
            } else if (card_get_status(save_place.drive, &status) == 0) {
                if (status.mounted == TRUE) {
                    if ((game_state.card_warning = save_check(0)) == SAVE_OK) {
                        game_state.card_warning = SAVE_OK;
                        game_state.card_removed = 0;
                        message_end();
                    } else {
                        game_state.card_removed = 0;
                        if (game_state.card_warning != SAVE_BAD_CHECKSUM) {
                            game_state.card_warning = SAVE_NOT_OURS;
                            card_error_show();
                        } else {
                            game_state.card_state = CARD_LOAD_FAILED;
                            card_notice_start();
                        }
                    }
                } else {
                    game_state.card_removed = 1;
                    game_state.card_warning = SAVE_NOT_READY;
                    card_error_show();
                }
            }
        }
        break;
    case STAGE_CARD_NOT_OURS:
        result = stage_run_warning();
        if (player_pads[0]->press & (pad_choose_buttons | pad_cancel_buttons)) {
            game_state.card_removed = 0;
            game_state.card_warning = SAVE_OK;
            message_end();
            save_set_drive(SAVE_DRIVE_REFUSED);
        } else if (pad_player_is_connected(0) &&
                   (pad_player_is_connected(1) || !game_state.two_players)) {
            struct card_status status;

            if (card_get_status(save_place.drive, &status) == 0) {
                if (status.mounted == TRUE) {
                    if (save_check_sockets()) {
                        if ((game_state.card_warning = save_check(0)) ==
                            SAVE_OK) {
                            game_state.card_warning = SAVE_OK;
                            game_state.card_removed = 0;
                            message_end();
                        } else {
                            game_state.card_removed = 0;
                            if (game_state.card_warning != SAVE_BAD_CHECKSUM) {
                                game_state.card_warning = SAVE_NOT_OURS;
                                card_error_show();
                            } else {
                                game_state.card_state = CARD_LOAD_FAILED;
                                card_notice_start();
                            }
                        }
                    }
                } else {
                    game_state.card_removed = 1;
                    game_state.card_warning = SAVE_NOT_READY;
                    card_error_show();
                }
            } else {
                game_state.card_removed = 1;
                game_state.card_warning = SAVE_NOT_READY;
                card_error_show();
            }
        } else {
            warn_no_pad();
        }
        break;
    case STAGE_CARD_GONE:
        result = stage_run_warning();
        if (player_pads[0]->press & (pad_choose_buttons | pad_cancel_buttons)) {
            message_end();
            game_state.card_removed = 0;
            game_state.card_warning = SAVE_OK;
            save_set_drive(SAVE_DRIVE_REFUSED);
        } else if (pad_player_is_connected(0) &&
                   (pad_player_is_connected(1) ||
                    game_state.two_players == 0)) {
            struct card_status status;

            if (card_get_status(save_place.drive, &status) == 0) {
                if (status.mounted == TRUE) {
                    if ((game_state.card_warning = save_check(0)) == SAVE_OK) {
                        message_end();
                        game_state.card_warning = SAVE_OK;
                        game_state.card_removed = 0;
                    } else {
                        game_state.card_removed = 0;
                        if (game_state.card_warning != SAVE_BAD_CHECKSUM) {
                            game_state.card_warning = SAVE_NOT_OURS;
                            card_error_show();
                        } else {
                            game_state.card_state = CARD_LOAD_FAILED;
                            card_notice_start();
                        }
                    }
                } else {
                    game_state.card_removed = 1;
                    game_state.card_warning = SAVE_NOT_READY;
                    card_error_show();
                }
            } else {
                game_state.card_removed = 1;
                game_state.card_warning = SAVE_NOT_READY;
                card_error_show();
            }
        } else {
            warn_no_pad();
        }
        break;
    case STAGE_CARD_ERROR:
        result = stage_run_warning();
        if (pad_player_is_connected(0) &&
            (pad_player_is_connected(1) || game_state.two_players == 0)) {
            int found;
            struct card_status status;

            found = 0;
            if (save_place.drive == SAVE_DRIVE_REFUSED) {
                game_state.card_warning = SAVE_OK;
                game_state.card_removed = 0;
                message_end();
            } else if (save_check_sockets()) {
                if (card_get_status(save_place.drive, &status) == 0 &&
                    status.mounted == TRUE) {
                    found = 1;
                }
            }
            if (!game_state.card_removed && found == 1) {
                if (game_state.card_warning >= CARD_WARNING_ERROR) {
                    game_state.card_warning = SAVE_OK;
                    if (stage_current == STAGE_CHAO) {
                        /* @hack redundant: a goto to the hook's test, past
                         * a store of result that never runs, where neither
                         * would do */
                        goto hook;
                        result = 0;
                    hook:
                        /* @hack type: stage_hooks.save called as save_hook,
                         * where the member would do */
                        if (save_hook) {
                            save_hook();
                        }
                    } else if ((game_state.card_warning = save_check(0)) ==
                               SAVE_OK) {
                        save_write();
                        /* @hack redundant: an empty else */
                    } else {
                    }
                    stage_mode = STAGE_SAVING;
                } else {
                    message_end();
                }
                break;
            }
        }
        if (player_pads[0]->press & (pad_choose_buttons | pad_cancel_buttons)) {
            message_end();
            game_state.card_removed = 0;
            game_state.card_warning = SAVE_OK;
            save_set_drive(SAVE_DRIVE_REFUSED);
        }
        break;
    case STAGE_SAVING:
        result = stage_run_notice();
        if (game_state.card_removed ||
            game_state.card_warning >= CARD_WARNING_ERROR) {
            card_error_start();
        } else if (game_state.card_state < CARD_IDLE) {
            card_notice_start();
        } else if (!game_state.card_state) {
            message_end();
        }
        break;
    case STAGE_CARD_NOTICE:
        result = stage_run_notice();
        if (notice_timer > 0) {
            notice_timer--;
        }
        if (notice_timer <= 0) {
            if (game_state.card_removed) {
                game_state.card_removed = 0;
            } else {
                game_state.card_state = CARD_IDLE;
            }
            game_state.card_warning = SAVE_OK;
            message_end();
        }
        break;
    case STAGE_CLEAR_FADE:
        if (game_state.chao_key != 0 &&
            (game_state.winner_posed == 1 ||
             !STAGE_RELAY_BEFORE_LAST(stage_current))) {
            result = fade_to_color_step(FULL, FULL, FULL, FULL);
        } else {
            result = fade_out_step();
        }
        if (result) {
            play_stop();
            forget_end();
            stage_mode = STAGE_LEAVE;
        } else {
            stage_run();
        }
        break;
    case STAGE_MISS_FADE:
        if ((result = fade_out_step()) != 0) {
            stage_mode = STAGE_AGAIN;
            stage_retries++;
            play_rewind();
            fade_set_black();
        } else {
            stage_run();
        }
        break;
    case STAGE_RETRY_FADE:
        if ((result = fade_out_step()) != 0) {
            stage_mode = STAGE_FADE_IN;
            stage_retries++;
            if (player_tasks[0]) {
                player_records[0]->flags |= PLAYER_MISSED;
            }
            play_rewind();
            world_reset_gravity();
            stage_reset();
            play_start();
            fade_set_black();
        } else {
            stage_run_fading();
        }
        break;
    case STAGE_GAME_OVER_FADE:
        if ((result = fade_out_step()) != 0) {
            play_stop();
            forget_end();
            stage_mode = STAGE_GAME_OVER;
        } else {
            stage_run();
        }
        break;
    case STAGE_RESTART_FADE:
        if ((result = fade_out_step()) != 0) {
            stage_mode = STAGE_AGAIN;
            play_rewind();
        } else {
            stage_run_fading();
        }
        break;
    case STAGE_GO_TO_FADE:
        if (game_state.chao_key != 0 &&
            !STAGE_RELAY_BEFORE_LAST(stage_current)) {
            result = fade_to_color_step(FULL, FULL, FULL, FULL);
        } else {
            result = fade_out_step();
        }
        if (result) {
            stage_go_next();
        } else {
            stage_run();
        }
        break;
    case STAGE_BACK_FADE:
        if ((result = fade_to_color_step(FULL, FULL, FULL, FULL)) != 0) {
            stage_go_back_now();
        } else {
            stage_run();
        }
        break;
    case STAGE_QUIT_FADE:
        if ((result = fade_out_step()) != 0) {
            play_stop();
            forget_end();
            stage_mode = STAGE_QUIT;
        } else {
            stage_run();
        }
        break;
    case STAGE_LEAVE: {
        int relay;

        relay = game_state.relay;
        stage_unload();
        stage_go_to_next();
        if (game_state.winner_posed != 1) {
            switch (stage_current_id()) {
            case STAGE_RELAY_FIRST:
                if (relay == 1) {
                    stage_clock_keep();
                    rings_keep();
                    score_keep();
                    ring_total_keep();
                } else {
                    kept_clock_clear();
                    kept_rings_clear();
                    kept_score_clear();
                    kept_ring_total_clear();
                    kept_object_bits_clear();
                    kept_animals_clear();
                }
                game_state.relay = 1;
                stage_mode = STAGE_ENTER;
                return OVERLAY_DONE;
            case STAGE_RELAY_EGGMAN:
            case STAGE_RELAY_KNUCKLES:
            case STAGE_RELAY_ROUGE:
            case STAGE_RELAY_SONIC:
                game_state.relay = 1;
                stage_clock_keep();
                rings_keep();
                score_keep();
                ring_total_keep();
                stage_mode = STAGE_ENTER;
                return OVERLAY_DONE;
            }
        }
        stage_mode = STAGE_TITLE_CARD;
        return OVERLAY_DONE;
    }
    case STAGE_GAME_OVER:
        stage_unload();
        stage_mode = STAGE_TITLE_CARD;
        return OVERLAY_BACK;
    case STAGE_QUIT:
        stage_unload();
        stage_mode = STAGE_TITLE_CARD;
        return OVERLAY_BACK;
    case STAGE_RESET:
        if (reset_timer-- > 0) {
            return OVERLAY_RUNNING;
        }
        stage_mode = STAGE_TITLE_CARD;
        return OVERLAY_RESET;
        /* @hack redundant: an empty block after the return, which never
         * runs */
        if (0) {
        }
    default:
        stage_mode = STAGE_LEAVE;
        break;
    }
    /* the pads asked for a reset, which STAGE_RESET waits out. a mode that
     * runs play stops it, and every mode the switch lists unloads the
     * stage */
    if (pad_reset_pressed(0)) {
        switch (stage_mode) {
        case STAGE_AGAIN:
        case 6: /* no mode's number */
        case STAGE_FADE_IN:
        case STAGE_CLEAR_FADE:
        case STAGE_MISS_FADE:
        case STAGE_GAME_OVER_FADE:
        case STAGE_GO_TO_FADE:
        case STAGE_RESTART_FADE:
        case STAGE_RETRY_FADE:
        case STAGE_QUIT_FADE:
        case STAGE_BACK_FADE:
        case STAGE_PLAY:
        case STAGE_PAUSE:
        case STAGE_NO_PAD:
        case STAGE_CARD_NOTICE:
        case STAGE_SAVING:
        case STAGE_CARD_ERROR:
        case STAGE_CARD_NOT_OURS:
        case STAGE_CARD_GONE:
            play_stop();
            /* fall through */
        case STAGE_LEAVE:
        case STAGE_GAME_OVER:
        case STAGE_QUIT:
            stage_unload();
            /* @hack redundant: the last case's break */
            break;
        }
        stage_mode = STAGE_RESET;
        forget_end();
        stage_set_next(0);
        game_state.relay = 0;
        game_state.chao_key = 0;
        game_state.hidden_cameras = 0;
        if (stage_name_text) {
            text_sprite_destroy(stage_name_text);
            stage_name_text = NULL;
        }
        reset_timer = RESET_FRAMES;
    }
    return OVERLAY_RUNNING;
}

static void leave_play(void)
{
    stage_end(STAGE_END_NONE);
    fade_reset_idle();
    stage_clock_stop();
    pad_disable();
    pause_disable();
}

/* stop play and put the players back where they restart from */
static void play_rewind(void)
{
    play_stop();
    stage_end(STAGE_END_NONE);
    stage_restart_players();
    camera_restart();
}

/* STAGE_BACK_FADE's end: play the stage from its start, without a load.
 * the clock, the score and the rings stay as they stand */
static void stage_go_back_now(void)
{
    keep_counts = 1;
    play_stop();
    forget_end();
    stage_restart_players();
    camera_restart();
    stage_mode = STAGE_FADE_IN;
    world_reset_gravity();
    stage_reset();
    play_start();
    music_play();
    keep_counts = 0;
}

/* STAGE_GO_TO_FADE's end. where the next stage is the current one, start it
 * again from its start, without a load. otherwise stop play for
 * STAGE_LEAVE */
static void stage_go_next(void)
{
    if (stage_next == stage_current) {
        play_stop();
        forget_end();
        stage_restart_players();
        camera_restart();
        stage_mode = STAGE_FADE_IN;
        world_reset_gravity();
        stage_reset();
        play_start();
        fade_set_black();
        music_play();
    } else {
        play_stop();
        stage_end(STAGE_END_NONE);
        stage_mode = STAGE_LEAVE;
    }
}

/* the stage is cleared: name the stage that follows, and set the mode that
 * leads there.
 *
 * a two-player match ends once a player has won ROUNDS_TO_WIN rounds, or at
 * once under TWO_PLAYERS_1_POINT. until then, a match of mechs plays its stage
 * again. any other match plays another of its three stages. after the first
 * round, rand picks one of the two unplayed. after the second, the match takes
 * the first stage the second round didn't play. the bug tagged below lets that
 * be the first round's.
 *
 * in a one-player game, a Chao key leads to STAGE_CHAO. in a relay stage before
 * the last, it does so only once the winner has posed. otherwise each relay
 * stage names the next, and any other stage names none */
#ifdef NONMATCHING
/* void, the plain form the hack in its last case names */
static void stage_leave_cleared(void)
#else
static int stage_leave_cleared(void)
#endif
{
    /* a match's three stages, by its characters: Sonic's and Shadow's,
     * Knuckles's and Rouge's, and the mechs'. no code reads the mechs' */
    static Sint32 runner_stages[MATCH_STAGES] = {
        STAGE_GREEN_FOREST, STAGE_RADICAL_HIGHWAY, STAGE_SKY_RAIL};
    static Sint32 hunter_stages[MATCH_STAGES] = {
        STAGE_PUMPKIN_HILL, STAGE_WILD_CANYON_2P, STAGE_METEOR_HERD};
    static Sint32 mech_stages[MATCH_STAGES] = {
        STAGE_WEAPONS_BED_2P, STAGE_MISSION_STREET_2P, STAGE_SAND_OCEAN_2P};
    Sint32 *stages;
    int value;
    Sint16 choices[MATCH_STAGES - 1];
    int count;
    int index;
    int next;
    int i;

    if (game_state.two_players) {
        game_state.chao_key = 0;
        if (game_state.two_players == TWO_PLAYERS_1_POINT ||
            game_state.round_wins[0] >= ROUNDS_TO_WIN ||
            game_state.round_wins[1] >= ROUNDS_TO_WIN) {
            stage_mode = STAGE_QUIT_FADE;
        } else {
            stage_mode = STAGE_GO_TO_FADE;
            /* @hack spelling: value, a local for the character and then
             * for the stage, where each call's result would do in place */
            value = stage_character(0);
            switch (value) {
            case PLAYER_SONIC:
            case PLAYER_SHADOW:
                stages = runner_stages;
                break;
            case PLAYER_KNUCKLES:
            case PLAYER_ROUGE:
                stages = hunter_stages;
                break;
            case PLAYER_EGGMAN_MECH:
            case PLAYER_TAILS_MECH:
            default:
                value = stage_current_id();
                stage_set_next(value);
                return; /* NOLINT(clang-diagnostic-return-mismatch) */
            }
            /* @hack spelling: index summed in three statements, where
             * round_wins[0] + round_wins[1] - 1 would do */
            next = 0;
            index = 0;
            index += game_state.round_wins[0];
            index += game_state.round_wins[1];
            index--;
            match_played[index] = stage_current_id();
            switch (index) {
            case 0:
                /* the current stage must be one of the match's stages.
                 * otherwise all three go into choices, which holds two,
                 * unchecked */
                count = 0;
                for (i = 0; i < MATCH_STAGES; i++) {
                    if (stages[i] == match_played[0]) {
                        continue;
                    }
                    choices[count] = stages[i];
                    count++;
                }
                next = floorf((Float)count * rand() * (1.0F / 32768.0F));
                if (next >= count) {
                    next--;
                }
                if (next < 0) {
                    next = stage_current_id();
                } else {
                    next = choices[next];
                }
                break;
            case 1:
                /* @bug logic: passes over the second round's stage only,
                 * so the third round can play the first round's stage
                 * again. match_played[0] is -1 by now (CMP/EQ at
                 * 0x8c025fe2): stage_start cleared all three before the
                 * second round (MOV.W at 0x8c023f26) */
                for (i = 0; i < MATCH_STAGES; i++) {
                    if (stages[i] != match_played[0] &&
                        stages[i] != match_played[1]) {
                        next = stages[i];
                        break;
                    }
                }
                if (next <= 0) {
                    next = stage_current_id();
                }
                break;
            default:
                next = stage_current_id();
                /* @hack redundant: the last case's break */
                break;
            }
            stage_set_next(next);
        }
    } else if (game_state.chao_key != 0 &&
               (!STAGE_RELAY_BEFORE_LAST(stage_current) ||
                game_state.winner_posed == 1)) {
        stage_mode = STAGE_GO_TO_FADE;
        stage_next = STAGE_CHAO;
        /* @hack redundant: a block that never runs */
        if (0) {
            stage_mode = STAGE_QUIT_FADE;
            game_state.chao_key = STAGE_QUIT_FADE;
        }
        sound_play(SOUND_TO_CHAO, NULL, 0, 0);
    } else {
        stage_mode = STAGE_CLEAR_FADE;
        switch (stage_current_id()) {
        case STAGE_RELAY_TAILS:
            stage_set_next(STAGE_RELAY_EGGMAN);
            break;
        case STAGE_RELAY_EGGMAN:
            stage_set_next(STAGE_RELAY_ROUGE);
            break;
        case STAGE_RELAY_ROUGE:
            stage_set_next(STAGE_RELAY_KNUCKLES);
            break;
        case STAGE_RELAY_KNUCKLES:
            stage_set_next(STAGE_RELAY_SONIC);
            break;
        case STAGE_RELAY_SONIC:
            stage_set_next(STAGE_BOSS_LAST2);
            break;
            /* @hack redundant: a return of 0 that never runs, after the
             * last case's break, in a function declared int for it, where
             * void and no return would do. no path returns a value, and the
             * caller reads none */
#ifdef NONMATCHING
#else
            return 0;
#endif
        }
    }
} /* NOLINT(clang-diagnostic-return-type) */

/* NOLINTBEGIN(readability-redundant-control-flow) */
void stage_end(int end)
{
    stage_end_kind = end;
    /* @hack redundant: a return where the function ends */
    return;
}

static Sint16 end_kind(void)
{
    return stage_end_kind;
}

static void forget_end(void)
{
    stage_end(STAGE_END_NONE);
    /* @hack redundant: a return, and a test after it that never runs.
     * with them, SHC calls the function where it would expand it */
    return;
    if (end_kind()) {
        stage_end(STAGE_END_NONE);
    }
}
/* NOLINTEND(readability-redundant-control-flow) */

/* put the stage back as its start finds it. the clock, the rings and the score
 * go back to their copies. the Chao key goes, but in a relay stage after the
 * first. under keep_counts, those counts and the Chao key stay, and each
 * placement keeps its PLACEMENT_UNK_20 */
static void stage_reset_play(void)
{
    int i;
    int stage;
    Sint8 *flag;
    struct hunt *hunt;

    if (keep_counts == 0) {
        stage_clock_restore();
        rings_restore();
        score_restore();
    }
    stage_clock_stop();
    if (keep_counts == 0) {
        set_reset();
    } else {
        set_reset_but_unk_20();
    }
    light_init();
    enemy_groups_clear();
    object_states_clear();
    restart_point_clear(0);
    restart_point_clear(1);
    world_reset_gravity();
    /* @hack spelling: the levels cleared through a pointer under a count
     * that runs down to 0, where an index that counts up to their size would
     * do */
    flag = game_state.ring_levels[0];
    for (i = sizeof(game_state.ring_levels); i != 0; i--) {
        *flag++ = 0;
    }
    if (keep_counts == 0) {
        stage = stage_current;
        if (stage == STAGE_RELAY_FIRST || !STAGE_RELAY(stage)) {
            game_state.chao_key = 0;
        }
    }
    game_state.debug_display = 0;
    game_state.rings_shown = 1;
    game_state.active_player = 0;
    game_state.single_view = 0;
    if (keep_counts == 0) {
        hunt = hunt_current;
        if (hunt) {
            hunt->finish = 1;
        }
    }
}

static void play_reset_state(void)
{
    struct hunt *hunt;

    game_state.pause_disabled = 1;
    game_state.paused = 0;
    game_state.cleared = 0;
    game_state.winner_posed = 0;
    game_state.hidden_cameras = 0;
    game_state.solo_player = 0;
    /* @hack spelling: a call in each arm, where one call of
     * two_players ? PAD_BOTH_PLAYERS : 0 would do */
    if (game_state.two_players) {
        pad_enable_player(PAD_BOTH_PLAYERS);
    } else {
        pad_enable_player(0);
    }
    pal_frames = 0;
    enemy_groups_clear();
    object_states_clear();
    set_tally();
    hunt = hunt_current;
    if (hunt) {
        if (game_state.mission != MISSION_TIMED) {
            hunt->restart = 1;
        } else {
            hunt->finish = 1;
        }
    }
}

static void stage_reset(void)
{
    stage_reset_play();
    game_state.pause_disabled = 1;
    stage_restart_players();
    camera_restart();
    animals_start();
}

/* load the current stage: its fog, its sounds and its overlay, whose init
 * runs. then start it over. Tails and Eggman play STAGE_CHAO on foot, and
 * play STAGE_GREEN_FOREST to STAGE_STG_END and STAGE_CART_1 in their mechs.
 * slot 0's player is made again as the other of the two where the stage
 * wants it */
static void stage_load(void)
{
    Sint16 *stage;

#ifdef NONMATCHING
    /* this side reads and admits the stage's files before any of its code
     * runs, and stops the game where one won't load */
    loader_prepare_stage();
#endif

    /* @hack redundant: a block that never runs */
    if (0) {
        stage_retries = stage_previous;
        stage_create_players();
        stage_next = 1;
    }
    stage_retries = 0;
    /* @hack spelling: stage set inside its first read, where a statement of
     * its own would do */
    stage_loaded = *(stage = &stage_current);
    njSetFogColor(0);
    njGenerateFogTable3(fog_table, FOG_NEAR, FOG_FAR);
    njSetFogTable(fog_table);
    if (*stage == STAGE_CHAO) {
        if (first_character == PLAYER_EGGMAN_MECH) {
            task_destroy(player_tasks[0]);
            stage_set_character(0, PLAYER_EGGMAN);
            stage_create_players();
        } else if (first_character == PLAYER_TAILS_MECH) {
            task_destroy(player_tasks[0]);
            stage_set_character(0, PLAYER_TAILS);
            stage_create_players();
        }
    } else if ((*stage >= STAGE_GREEN_FOREST && *stage <= STAGE_STG_END) ||
               *stage == STAGE_CART_1) {
        if (first_character == PLAYER_EGGMAN) {
            task_destroy(player_tasks[0]);
            stage_set_character(0, PLAYER_EGGMAN_MECH);
            stage_create_players();
        } else if (first_character == PLAYER_TAILS) {
            task_destroy(player_tasks[0]);
            stage_set_character(0, PLAYER_TAILS_MECH);
            stage_create_players();
        }
    }
    sound_load_player_sounds();
#ifdef NONMATCHING
    /* this side admitted the overlay in loader_prepare_stage, and stops the
     * game where its init leaves it no step */
    loader_start_stage();
#else
    loader_read_stage_overlay();
    overlay_run_init();
#endif
    stage_reset();
}

static void play_start(void)
{
    /* @hack spelling: a call in each arm, where one call with
     * two_players ? PAIR_BLANKS : SOLO_BLANKS and the same for the steps
     * would do */
    if (game_state.two_players) {
        frame_pacing_init(PAIR_BLANKS, PAIR_STEPS);
    } else {
        frame_pacing_init(SOLO_BLANKS, SOLO_STEPS);
    }
    play_reset_state();
    stage_create_task();
    collision_mesh_init();
    collision_clear();
    set_reactivate();
    music_play();
    time_limit_start();
    if (game_state.two_players) {
        player_pose_two_start();
        if (banner_timer > 0) {
            njReleaseTexture(&game2p_texlist);
            banner_timer = 0;
        }
        banner_timer = BANNER_FRAMES;
        texture_load_pvm("GAME2PTEX", &game2p_texlist);
    }
}

#ifdef NONMATCHING
/* TRUE where a frame of play may show a picture after its first step, under
 * the port's 60 fps (engine/dc/port.h): in STAGE_PLAY alone, outside a
 * demo. another mode's fade, and a demo's prompt, draw outside the two
 * draws below, and would show in every other picture */
#define PLAY_SPLITS() (stage_mode == STAGE_PLAY && game_state.demo == 0)

/* draw the picture stage_run_solo draws as its frame ends, for
 * present_step */
static void solo_draw_step(void)
{
    camera_draw_restart();
    task_draw_all();
    if (game_state.split_screen) {
        viewport_set_whole();
    }
    camera_draw_restart();
}

/* draw the picture stage_run draws as its frame ends, for present_step.
 * the banner's count stays the frame's */
static void play_draw_step(void)
{
    int i;

    camera_draw_restart();
    if (game_state.split_screen) {
        for (i = 0; i < camera_active_count; i++) {
            task_draw_all();
            if (game_state.single_view) {
                break;
            }
        }
        viewport_set_whole();
    } else {
        task_draw_all();
    }
    hud_draw();
    if (game_state.two_players != 0 && banner_timer > 0) {
        banner_draw();
    }
    camera_draw_restart();
}
#endif

/* a frame of play while game_state.solo_player names a player. each step
 * runs the tasks of level 0 and that player's task alone. a demo ends as
 * cleared on Start, or once a player has missed */
static void stage_run_solo(void)
{
    struct task *task;
    int quit;
    int slot;

    quit = 0;
    slot = game_state.solo_player - 1;
    /* each frame tests the count the frame before left. so in a two-player
     * game it goes 2, 1, 2, 1, where stage_run holds it at 2 or more */
    if (game_state.two_players != 0 && frame_step_count <= SOLO_STEPS) {
        frame_step_count = PAIR_STEPS;
    } else {
        frame_step_count = SOLO_STEPS;
    }
    frame_step = 0;
    if (game_state.display == DISPLAY_PAL) {
        pal_frames++;
        if (pal_frames == PAL_FRAMES) {
            frame_step_count++;
        }
        if (pal_frames >= PAL_FRAMES) {
            pal_frames = 0;
        }
    }
    while (frame_step < frame_step_count) {
        pad_scan();
        if (stage_mode == STAGE_PLAY) {
            fade_idle_step();
        }
        task = player_tasks[slot];
        task_step_level(0);
        task->step(task);
        collision_mesh_update();
        if (game_state.demo != 0) {
            if ((player_pads[0]->press & PDD_DGT_ST) ||
                (player_pad_records[0].press & PDD_DGT_ST)) {
                quit = 1;
            } else if ((player_records[0] &&
                        (player_records[0]->flags & PLAYER_MISSED)) ||
                       (player_records[1] &&
                        (player_records[1]->flags & PLAYER_MISSED))) {
                quit = 1;
            }
        }
        frame_step++;
#ifdef NONMATCHING
        /* this side shows the step's picture where the port's 60 fps splits
         * the frame (engine/dc/port.h) */
        present_step(frame_step, PLAY_SPLITS() ? solo_draw_step : NULL);
#endif
        if (frame_step < frame_step_count) {
            frame_wait_step(frame_step_count - frame_step);
        }
    }
    timer_mark();
    if (game_state.split_screen) {
        task_draw_all();
        viewport_set_whole();
    } else {
        task_draw_all();
    }
    if (game_state.demo) {
        if ((game_state.total_steps & PRESS_START_BLINK) &&
            game_state.unk_0b == 1 && PRESS_START_READY()) {
            njSetTexture(&press_start_texlist);
            njDrawTexture(press_start_quad, 4, PRESS_START_TEXTURE, 1);
        }
        if (quit) {
            stage_end(STAGE_END_CLEAR);
        }
    }
    game_state.draw_time = timer_lap();
}

/* a frame of play. its steps come from the times game_state keeps. they number
 * at most FRAME_STEPS, and at least PAIR_STEPS in a two-player game.
 *
 * it returns 1 or 2 where player 0 or 1 pressed Start to pause, and 0
 * otherwise. Start counts only while the pause is on, no mini event runs and
 * the stage isn't cleared. a demo returns 0, and ends as stage_run_solo's does.
 *
 * a one-player game of MISSION_RINGS clears the stage once player 0 holds
 * RINGS_GOAL rings. it does so at the frame's end, on STAGE_CART_1 or a stage
 * up to STAGE_STG_END */
static int stage_run(void)
{
    int result;
    int quit;
    int i;
    Float spent;
    Float rate;
    Float least;
    Float steps;

    result = 0;
    quit = 0;
    if (game_state.solo_player) {
        stage_run_solo();
        return 0;
    }
    spent = (game_state.draw_time + DRAW_MARGIN) * BLANKS_PER_TIME;
    rate =
        ((game_state.frame_time * BLANKS_PER_TIME) - spent) / frame_step_count;
    least = game_state.render_time * BLANKS_PER_TIME;
    if (rate >= 1.0F) {
        steps = FRAME_STEPS;
    } else {
        steps = spent / (1.0F - rate);
    }
    if (steps > least) {
        frame_step_count = njRoundUp(steps);
    } else {
        frame_step_count = njRoundUp(least);
    }
    if (frame_step_count >= FRAME_STEPS + 1) {
        frame_step_count = FRAME_STEPS;
    }
    if (game_state.two_players != 0 && frame_step_count <= SOLO_STEPS) {
        frame_step_count = PAIR_STEPS;
    }
    frame_step = 0;
    if (game_state.display == DISPLAY_PAL) {
        pal_frames++;
        if (pal_frames == PAL_FRAMES) {
            frame_step_count++;
        }
        if (pal_frames >= PAL_FRAMES) {
            pal_frames = 0;
        }
    }
    while (frame_step < frame_step_count) {
        pad_scan();
        if (stage_mode == STAGE_PLAY) {
            fade_idle_step();
        }
        task_step_all();
        collision_collide_all();
        collision_mesh_update();
        stage_clock_step();
        if (((player_pads[0]->press & PDD_DGT_ST) ||
             ((player_pads[1]->press & PDD_DGT_ST) &&
              game_state.two_players != 0)) &&
            game_state.pause_disabled == 0 &&
            game_state.mini_event_running == 0 && game_state.cleared == 0) {
            if (player_pads[0]->press & PDD_DGT_ST) {
                result = 1;
            } else if (player_pads[1]->press & PDD_DGT_ST) {
                result = 2;
            }
        }
        if (game_state.demo != 0) {
            if ((player_pads[0]->press & PDD_DGT_ST) ||
                (player_pad_records[0].press & PDD_DGT_ST)) {
                quit = 1;
                if (player_pads[0]->press & PDD_DGT_ST) {
                    game_state.skip_menus = 0;
                }
            } else if ((player_records[0] &&
                        (player_records[0]->flags & PLAYER_MISSED)) ||
                       (player_records[1] &&
                        (player_records[1]->flags & PLAYER_MISSED))) {
                quit = 1;
            }
        }
        frame_step++;
#ifdef NONMATCHING
        /* this side shows the step's picture, as stage_run_solo's does */
        present_step(frame_step, PLAY_SPLITS() ? play_draw_step : NULL);
#endif
        if (frame_step < frame_step_count) {
            frame_wait_step(frame_step_count - frame_step);
        }
    }
    timer_mark();
    if (game_state.split_screen) {
        for (i = 0; i < camera_active_count; i++) {
            task_draw_all();
            if (game_state.single_view) {
                break;
            }
        }
        viewport_set_whole();
    } else {
        task_draw_all();
    }
    hud_draw();
    if (game_state.two_players != 0 && banner_timer > 0) {
        banner_draw();
        banner_timer--;
    }
    if (game_state.demo) {
        if ((game_state.total_steps & PRESS_START_BLINK) &&
            game_state.unk_0b == 1 && PRESS_START_READY()) {
            njSetTexture(&press_start_texlist);
            njDrawTexture(press_start_quad, 4, PRESS_START_TEXTURE, 1);
        }
        if (quit) {
            stage_end(STAGE_END_CLEAR);
            item_stock_clear_all();
        }
        result = 0;
    }
    game_state.draw_time = timer_lap();
    if (game_state.mission == MISSION_RINGS && game_state.two_players == 0 &&
        (stage_current <= STAGE_STG_END || stage_current == STAGE_CART_1) &&
        ring_count(0) >= RINGS_GOAL && game_state.cleared == 0) {
        stage_clear(0);
    }
    save_count_time();
    return result;
}

/* a frame of a stage that stands still: no task steps, and the tasks and
 * the hud draw as stage_run draws them. the pause menu's frames also run
 * the idle dim */
static void stage_run_held(void)
{
    int i;

    pad_scan();
    if (stage_mode == STAGE_PAUSE) {
        fade_idle_step();
    }
    frame_step = 0;
    if (game_state.split_screen) {
        for (i = 0; i < camera_active_count; i++) {
            task_draw_all();
            if (game_state.single_view) {
                break;
            }
        }
        viewport_set_whole();
    } else {
        task_draw_all();
    }
    hud_draw();
}

/* a frame under the message: stage_run_held, then the message.
 * stage_run_warning serves the modes that wait for a pad or a card.
 * stage_run_notice, the same code again, serves STAGE_SAVING and
 * STAGE_CARD_NOTICE */
static Sint16 stage_run_warning(void)
{
    stage_run_held();
    message_draw(message);
    return 0;
}

static Sint16 stage_run_notice(void)
{
    stage_run_held();
    message_draw(message);
    return 0;
}

/* a frame of the pause menu: stage_run_held, then the menu's directions and
 * its draw. the pad of the player who paused moves the choice up and down,
 * round the ends. a menu of two rows passes over PAUSE_RESTART */
static Sint16 stage_run_paused(void)
{
    stage_run_held();
    if ((player_pads[0]->on & PAUSE_HIDE_BUTTONS) == PAUSE_HIDE_BUTTONS) {
        pause_hidden = -1;
    } else {
        pause_hidden = 0;
    }
    if (!(pad_held[0] & PAD_DIRECTIONS)) {
        pause_hold = 0;
    }
    if (pause_hold >= 1) {
        pause_hold++;
        if (pause_hold > PAUSE_HOLD_FRAMES) {
            pause_hold = 0;
        }
    }
    if (pause_hold == 0) {
        if ((game_state.paused == PAUSED_BY_PLAYER_0 &&
             (pad_press_repeat[0] & PDD_DGT_KU)) ||
            (game_state.paused == PAUSED_BY_PLAYER_1 &&
             (pad_press_repeat[1] & PDD_DGT_KU) &&
             game_state.two_players != 0)) {
            pause_choice--;
            if (pause_rows == PAUSE_ROWS_NO_RESTART &&
                pause_choice == PAUSE_RESTART) {
                pause_choice--;
            }
            sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
        } else if ((game_state.paused == PAUSED_BY_PLAYER_0 &&
                    (pad_press_repeat[0] & PDD_DGT_KD)) ||
                   (game_state.paused == PAUSED_BY_PLAYER_1 &&
                    (pad_press_repeat[1] & PDD_DGT_KD) &&
                    game_state.two_players != 0)) {
            pause_choice++;
            if (pause_rows == PAUSE_ROWS_NO_RESTART &&
                pause_choice == PAUSE_RESTART) {
                pause_choice++;
            }
            sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
        }
    }
    if (pause_choice >= PAUSE_QUIT + 1) {
        pause_choice = PAUSE_CONTINUE;
    } else if (pause_choice < PAUSE_CONTINUE) {
        pause_choice = PAUSE_QUIT;
    }
    if (pause_hidden >= 0) {
        pause_draw();
    }
    return 0;
}

/* a frame of a fade under which no task steps */
static int stage_run_fading(void)
{
    int i;

    pad_scan();
    if (game_state.split_screen) {
        for (i = 0; i < camera_active_count; i++) {
            task_draw_all();
            if (game_state.single_view) {
                break;
            }
        }
        viewport_set_whole();
    } else {
        task_draw_all();
    }
    hud_draw();
    return 0;
}

static void play_stop(void)
{
    collision_collide_all();
    collision_mesh_update();
    fun_8c059fb2();
    task_destroy_levels_2_to_5();
    collision_collide_all();
    collision_mesh_update();
    vibration_stop();
    sound_stop_all_and_unpause();
    voices_stop();
    music_stop();
    if (banner_timer > 0) {
        njReleaseTexture(&game2p_texlist);
        banner_timer = 0;
    }
}

static void stage_unload(void)
{
    overlay_run_exit();
    animals_free();
    game_state.relay = 0;
    stage_hooks.open_message = NULL;
    stage_hooks.trail_alloc = NULL;
    stage_hooks.trail_draw = NULL;
    stage_hooks.trail_free = NULL;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
}

/* a character without a create call here makes no player, and
 * stage_place_player then finds its slot empty */
static void stage_create_players(void)
{
    int i;

    for (i = 0; i < GAME_PLAYERS; i++) {
        if (i == 0) {
            if (first_character == PLAYER_SONIC) {
                sonic_create(i);
            }
            if (first_character == PLAYER_SHADOW) {
                shadow_create(i);
            }
            if (first_character == PLAYER_KNUCKLES) {
                knuckles_create(i);
            }
            if (first_character == PLAYER_ROUGE) {
                rouge_create(i);
            }
            if (first_character == PLAYER_TAILS) {
                tails_create(i);
            }
            if (first_character == PLAYER_EGGMAN) {
                eggman_create(i);
            }
            if (first_character == PLAYER_EGGMAN_MECH) {
                walker_create_eggman(i);
            }
            if (first_character == PLAYER_TAILS_MECH) {
                walker_create_tails(i);
            }
            stage_place_player(i);
        } else if (i == 1 && game_state.two_players) {
            if (other_character == PLAYER_SONIC) {
                sonic_create(i);
            }
            if (other_character == PLAYER_SHADOW) {
                shadow_create(i);
            }
            if (other_character == PLAYER_KNUCKLES) {
                knuckles_create(i);
            }
            if (other_character == PLAYER_ROUGE) {
                rouge_create(i);
            }
            if (other_character == PLAYER_TAILS) {
                tails_create(i);
            }
            if (other_character == PLAYER_EGGMAN) {
                eggman_create(i);
            }
            if (other_character == PLAYER_EGGMAN_MECH) {
                walker_create_eggman(i);
            }
            if (other_character == PLAYER_TAILS_MECH) {
                walker_create_tails(i);
            }
            stage_place_player(i);
        }
    }
}

/* create the task that runs the step of the stage's overlay. SHC expands
 * the function into play_start, its one caller, and writes no copy of its
 * own. the task's name follows play_start's "GAME2PTEX" among the image's
 * strings, so the function stands after play_start */
static void stage_create_task(void)
{
    task_create(TASK_WORK, OVERLAY_TASK_LEVEL, STAGE_OVERLAY->step,
                "pfo->exec.subprgmanager");
}

/* reset and place again the players of slots 0 and 1. outside keep_counts,
 * the rings and the score first go back to their copies. an empty slot
 * zeroes game_state.steps, and in STAGE_CART_1 takes a life from that
 * slot's player. only the first empty slot of a step does so */
static void stage_restart_players(void)
{
    /* game_state.total_steps at the last call that found a slot empty, or
     * NO_STEPS before any did */
    static Uint32 empty_slot_steps = NO_STEPS;
    int i;

    if (keep_counts == 0) {
        rings_restore();
        score_restore();
    }
    for (i = 0; i < GAME_PLAYERS; i++) {
        if (player_tasks[i]) {
            player_reset(i);
            stage_place_player(i);
        } else {
            if (empty_slot_steps != game_state.total_steps) {
                if (stage_current_id() == STAGE_CART_1) {
                    lives_add(i, -1);
                }
                game_state.steps = 0;
            }
            empty_slot_steps = game_state.total_steps;
        }
    }
}

void stage_set_character(int slot, int character)
{
    if (slot == 0) {
        first_character = character;
    } else {
        other_character = character;
    }
}

int stage_character(int slot)
{
    if (slot == 0) {
        return first_character;
    } else {
        return other_character;
    }
}

int stage_get_retries(void)
{
    return stage_retries;
}

void stage_set_current(int stage)
{
    stage_previous = stage_current;
    stage_current = stage;
}

/* NOLINTBEGIN(readability-redundant-control-flow) */
void stage_set_next(int stage)
{
    stage_next = stage;
    /* @hack redundant: a return where the function ends */
    return;
}
/* NOLINTEND(readability-redundant-control-flow) */

void stage_fade_to(int stage)
{
    stage_end(STAGE_END_GO_TO);
    stage_set_next(stage);
}

void stage_restart(void)
{
    stage_end(STAGE_END_RESTART);
}

Sint16 stage_go_to_next(void)
{
    stage_previous = stage_current;
    return stage_current = stage_next;
}

int stage_current_id(void)
{
    return stage_current;
}

int stage_previous_id(void)
{
    return stage_previous;
}

int stage_next_id(void)
{
    return stage_next;
}

void pause_enable(void)
{
    game_state.pause_disabled = 0;
}

void pause_disable(void)
{
    game_state.pause_disabled = 1;
}

Bool stage_find_start(int player, NJS_POINT3 *pos, struct angles *ang)
{
    struct task *task;
    struct start_place *start;
    int i;

    task = player_tasks[player];
    if (pos) {
        pos->x = pos->y = pos->z = 0.0F;
    }
    /* @hack spelling: three statements, where one chain of assignments
     * would do, as pos has */
    if (ang) {
        ang->z = 0;
        ang->y = 0;
        ang->x = 0;
    }
    if (!task) {
        return FALSE;
    }
    switch (player_character(player)) {
    case PLAYER_SONIC:
        start = sonic_starts;
        break;
    case PLAYER_SHADOW:
        start = shadow_starts;
        break;
    case PLAYER_KNUCKLES:
        start = knuckles_starts;
        break;
    case PLAYER_ROUGE:
        start = rouge_starts;
        break;
    case PLAYER_TAILS:
        start = tails_starts;
        break;
    case PLAYER_EGGMAN_MECH:
        start = eggman_mech_starts;
        break;
    case PLAYER_TAILS_MECH:
        start = tails_mech_starts;
        break;
    case PLAYER_SUPER_SONIC:
        start = super_sonic_starts;
        break;
    case PLAYER_SUPER_SHADOW:
        start = super_shadow_starts;
        break;
    default:
        start = NULL;
        break;
    }
    if (game_state.two_players == 0 && !STAGE_VERSUS(stage_current)) {
        i = PLACE_ONE_PLAYER;
    } else if (player == 0) {
        i = PLACE_PLAYER_0;
    } else {
        i = PLACE_PLAYER_1;
    }
    for (;;) {
        if (!start || start->stage == STAGE_NONE) {
            break;
        } else if (start->stage == stage_current) {
            ang->y = start->ang_y[i];
            *pos = start->pos[i];
            break;
        }
        start++;
    }
    return TRUE;
}

void stage_place_player(int player)
{
    struct task *task;
    struct task_work *work;
    NJS_POINT3 pos;
    struct angles ang;

    task = player_tasks[player];
    if (task) {
        work = task->work;
        if (restart_point_refresh(player) != -1) {
            restart_point_apply(player, &work->pos, &work->ang);
        } else {
            if (!stage_find_start(player, &pos, &ang)) {
                return;
            }
            if (game_state.two_players == 0) {
                if (keep_counts != 1) {
                    stage_clock_restore();
                    score_restore();
                    rings_restore();
                }
            }
            work->ang = ang;
            work->pos = pos;
        }
        player_motion[player]->aim_y = work->ang.y;
        player_records[player]->ground.below_y = work->pos.y - BELOW_START;
        player_set_past(player, 0, &work->pos, NULL);
        light_select(light_camera, player);
        player_stop(player_motion[player], player_records[player]);
    }
}

void stage_place_player_again(int player)
{
    keep_counts = 1;
    stage_place_player(player);
    keep_counts = 0;
}

void stage_place_player_intro(int player)
{
    struct task *task;
    struct task_work *work;
    struct pair_place *place;
    int i;

    task = player_tasks[player];
    if (!task) {
        return;
    }
    work = task->work;
    switch (player_character(player)) {
    case PLAYER_SONIC:
        place = sonic_intros;
        break;
    case PLAYER_SHADOW:
        place = shadow_intros;
        break;
    case PLAYER_KNUCKLES:
        place = knuckles_intros;
        break;
    case PLAYER_ROUGE:
        place = rouge_intros;
        break;
    case PLAYER_EGGMAN_MECH:
        place = eggman_mech_intros;
        break;
    case PLAYER_TAILS_MECH:
        place = tails_mech_intros;
        break;
    default:
        place = NULL;
        break;
    }
    if (player == 0) {
        i = PAIR_PLAYER_0;
    } else {
        i = PAIR_PLAYER_1;
    }
    for (;;) {
        if (!place || place->stage == STAGE_NONE) {
            work->ang.y = 0;
            work->pos.x = work->pos.y = work->pos.z = 0.0F;
            break;
        } else if (place->stage == stage_current) {
            work->ang.y = place->ang_y[i];
            work->pos = place->pos[i];
            player_motion[player]->aim_y = work->ang.y;
            player_records[player]->ground.below_y = work->pos.y - BELOW_START;
            break;
        }
        place++;
    }
    player_set_past(player, 0, &work->pos, NULL);
    light_select(light_camera, player);
    player_stop(player_motion[player], player_records[player]);
}

/* put player at its character's place for the winner of MISSION_RINGS or
 * of MISSION_CHAO. only a one-player game has such places, on STAGE_CART_1
 * and on a stage up to STAGE_STG_END but STAGE_VERSUS's. it returns FALSE,
 * placing nobody, where the mission, the character or the stage has no such
 * place. player's slot must hold a player */
static Bool place_winner_by_mission(int player)
{
    struct task *task;
    struct task_work *work;
    struct pair_place *place;
    int i;

    task = player_tasks[player];
    if (game_state.two_players != 0 || STAGE_VERSUS(stage_current) ||
        (stage_current > STAGE_STG_END && stage_current != STAGE_CART_1)) {
        return FALSE;
    }
    if (game_state.mission == MISSION_RINGS) {
        i = PAIR_RINGS_WINNER;
    } else if (game_state.mission == MISSION_CHAO) {
        i = PAIR_CHAO_WINNER;
    } else {
        return FALSE;
    }
    work = task->work;
    switch (player_character(player)) {
    case PLAYER_SONIC:
        place = sonic_mission_wins;
        break;
    case PLAYER_SHADOW:
        place = shadow_mission_wins;
        break;
    case PLAYER_KNUCKLES:
        place = knuckles_mission_wins;
        break;
    case PLAYER_ROUGE:
        place = rouge_mission_wins;
        break;
    case PLAYER_EGGMAN_MECH:
        place = eggman_mech_mission_wins;
        break;
    case PLAYER_TAILS_MECH:
        place = tails_mech_mission_wins;
        break;
    case PLAYER_SUPER_SONIC:
    case PLAYER_SUPER_SHADOW:
    default:
        return FALSE;
    }
    for (;;) {
        if (!place || place->stage == STAGE_NONE) {
            return FALSE;
        } else if (place->stage == stage_current) {
            /* @bug logic: the test for NO_PLACE never holds. MOV.W at
             * 0x8c0276d4 sign-extends the turn, and CMP/EQ at 0x8c0276d6
             * compares it with H'0000FFFF. so in the relay's first four
             * stages, MISSION_CHAO's winner poses at its row's position,
             * turned by -1, and not at the stage's place for a win */
            /* NOLINTBEGIN(clang-diagnostic-tautological-constant-out-of-range-compare)
             */
            if (place->ang_y[i] == (Uint16)NO_PLACE) {
                return FALSE;
            }
            /* NOLINTEND(clang-diagnostic-tautological-constant-out-of-range-compare)
             */
            work->ang.x = work->ang.z = 0;
            work->ang.y = place->ang_y[i];
            player_motion[player]->aim_y = work->ang.y;
            work->pos = place->pos[i];
            player_records[player]->ground.below_y = work->pos.y - BELOW_START;
            player_records[player]->look_mode = PLAYER_LOOK_NONE;
            break;
        }
        place++;
    }
    player_set_past(player, 0, &work->pos, NULL);
    if (stage_current != STAGE_LOST_COLONY) {
        light_select(light_camera, player);
    } else {
        light_select(LIGHT_VOLUME, player);
    }
    return TRUE;
}

/* put player at its character's place for a win: the mission's, or else the
 * stage's, or else the origin. an empty slot changes nothing */
static void place_winner(int player)
{
    struct task *task;
    struct task_work *work;
    struct start_place *place;
    int i;

    task = player_tasks[player];
    if (!task) {
        return;
    }
    if (place_winner_by_mission(player) == TRUE) {
        return;
    }
    work = task->work;
    switch (player_character(player)) {
    case PLAYER_SONIC:
        place = sonic_wins;
        break;
    case PLAYER_SHADOW:
        place = shadow_wins;
        break;
    case PLAYER_KNUCKLES:
        place = knuckles_wins;
        break;
    case PLAYER_ROUGE:
        place = rouge_wins;
        break;
    case PLAYER_EGGMAN_MECH:
        place = eggman_mech_wins;
        break;
    case PLAYER_TAILS_MECH:
        place = tails_mech_wins;
        break;
    case PLAYER_SUPER_SONIC:
        place = super_sonic_wins;
        break;
    case PLAYER_SUPER_SHADOW:
        place = super_shadow_wins;
        break;
    default:
        place = NULL;
        break;
    }
    if (game_state.two_players == 0 && !STAGE_VERSUS(stage_current)) {
        i = PLACE_ONE_PLAYER;
    } else if (player == 0) {
        i = PLACE_PLAYER_0;
    } else {
        i = PLACE_PLAYER_1;
    }
    for (;;) {
        if (!place || place->stage == STAGE_NONE) {
            work->ang.x = work->ang.y = work->ang.z = 0;
            work->pos.x = work->pos.y = work->pos.z = 0.0F;
            player_motion[player]->aim_y = work->ang.y;
            break;
        } else if (place->stage == stage_current) {
            work->ang.x = work->ang.z = 0;
            work->ang.y = place->ang_y[i];
            player_motion[player]->aim_y = work->ang.y;
            work->pos = place->pos[i];
            player_records[player]->ground.below_y = work->pos.y - BELOW_START;
            player_records[player]->look_mode = PLAYER_LOOK_NONE;
            break;
        }
        place++;
    }
    player_set_past(player, 0, &work->pos, NULL);
    if (stage_current != STAGE_RADICAL_HIGHWAY &&
        stage_current != STAGE_LOST_COLONY) {
        light_select(light_camera, player);
    } else {
        light_select(LIGHT_VOLUME, player);
    }
}

/* put loser beside winner, at an offset in the winner's axes and a turn
 * from the winner's. the winner's character picks both. it hides the rings.
 * an empty slot of loser changes nothing, and winner's must hold a
 * player */
static void place_beside_winner(int winner, int loser)
{
    struct task *task;
    struct task_work *work;
    NJS_POINT3 offset;
    Angle turn;
    Angle heading;

    task = player_tasks[loser];
    if (!task) {
        return;
    }
    work = task->work;
    switch (player_character(winner)) {
    case PLAYER_SONIC:
        offset.x = -13.0F;
        offset.z = 12.0F;
        turn = NJM_DEG_ANG(3);
        break;
    case PLAYER_SHADOW:
        offset.x = -8.0F;
        offset.z = -11.0F;
        turn = NJM_DEG_ANG(-42);
        break;
    case PLAYER_KNUCKLES:
        offset.x = -10.0F;
        offset.z = -18.0F;
        turn = NJM_DEG_ANG(120);
        break;
    case PLAYER_ROUGE:
        offset.x = -8.0F;
        offset.z = -13.0F;
        turn = NJM_DEG_ANG(-24);
        break;
    case PLAYER_EGGMAN_MECH:
        offset.x = -14.0F;
        offset.z = -30.0F;
        turn = NJM_DEG_ANG(-20);
        break;
    case PLAYER_TAILS_MECH:
        offset.x = -33.0F;
        offset.z = 0.0F;
        turn = NJM_DEG_ANG(80);
        break;
    default:
        turn = 0;
        offset.x = 0.0F;
        offset.z = 0.0F;
        break;
    }
    work->ang.x = work->ang.z = 0;
    work->ang.y = player_work[winner]->ang.y - turn;
    player_motion[loser]->aim_y = work->ang.y;
    work->pos = player_work[winner]->pos;
    heading = player_work[winner]->ang.y;
    work->pos.x += (offset.x * njSin(heading)) + (offset.z * njCos(heading));
    work->pos.z += (-offset.x * njCos(heading)) + (offset.z * njSin(heading));
    player_records[loser]->ground.below_y = work->pos.y - BELOW_START;
    player_records[loser]->look_mode = PLAYER_LOOK_NONE;
    player_set_past(loser, 0, &work->pos, NULL);
    if (stage_current != STAGE_RADICAL_HIGHWAY) {
        light_select(light_camera, loser);
    } else {
        light_select(LIGHT_VOLUME, loser);
    }
    game_state.rings_shown = 0;
}

int stage_get_mode(void)
{
    return stage_mode;
}

void restart_point_clear(int player)
{
    Sint8 *byte;
    int i;

    /* @hack spelling: the point cleared through a pointer under a count that
     * runs down to 0, as stage_reset_play clears the ring levels */
    byte = (Sint8 *)&restart_points[player];
    for (i = sizeof(struct restart_point); i != 0; i--) {
        *byte++ = 0;
    }
    restart_points[player].order = -1;
    if (keep_counts == 1) {
        stage_clock_get(&restart_points[player].minute,
                        &restart_points[player].second,
                        &restart_points[player].frame);
    }
}

int restart_point_refresh(int player)
{
    if (stage_current_id() != restart_points[player].stage) {
        restart_point_clear(player);
    }
    return restart_points[player].order;
}

/* apply player's restart point: its place goes to *pos and *ang. its
 * gravity's tilt and up axis go to the world. outside keep_counts, the clock
 * goes back to the point's, and player loses its rings */
static void restart_point_apply(int player, NJS_POINT3 *pos, struct angles *ang)
{
    *pos = restart_points[player].pos;
    *ang = restart_points[player].ang;
    if (keep_counts == 0) {
        stage_clock_set(restart_points[player].minute,
                        restart_points[player].second,
                        restart_points[player].frame);
        ring_add(player, -ring_count(player));
    }
    world_tilt_gravity(restart_points[player].gravity_ang_x,
                       restart_points[player].gravity_ang_z);
    collision_set_up_axis(restart_points[player].up_axis);
}

void restart_point_keep(int player, NJS_POINT3 *pos, struct angles *ang,
                        int order)
{
    /* @hack redundant: a store to order that never runs */
    if (0) {
        order = 0;
    }
    if (restart_point_refresh(player) <= order) {
        stage_clock_get(&restart_points[player].minute,
                        &restart_points[player].second,
                        &restart_points[player].frame);
        restart_points[player].pos = *pos;
        restart_points[player].ang = *ang;
        restart_points[player].order = order;
        restart_points[player].stage = stage_current_id();
        restart_points[player].gravity_ang_x = world_gravity_ang_x;
        restart_points[player].gravity_ang_z = world_gravity_ang_z;
        restart_points[player].up_axis = collision_get_up_axis();
        if (game_state.two_players != 0 && (first_character == PLAYER_SONIC ||
                                            first_character == PLAYER_SHADOW)) {
            player = !player;
            stage_clock_get(&restart_points[player].minute,
                            &restart_points[player].second,
                            &restart_points[player].frame);
            restart_points[player].pos = *pos;
            restart_points[player].ang = *ang;
            restart_points[player].order = order;
            restart_points[player].stage = stage_current_id();
            restart_points[player].gravity_ang_x = world_gravity_ang_x;
            restart_points[player].gravity_ang_z = world_gravity_ang_z;
            restart_points[player].up_axis = collision_get_up_axis();
        }
    }
}

void stage_clear(int player)
{
    int stage;

    /* @hack spelling: a call in each arm, as in play_reset_state */
    if (game_state.two_players) {
        pad_disable_player(PAD_BOTH_PLAYERS);
    } else {
        pad_disable_player(0);
    }
    pause_disable();
    stage_clock_stop();
    if (game_state.two_players) {
        game_state.round_wins[player]++;
        game_state.round_winner = player_records[player]->character;
    }
    if (game_state.two_players == TWO_PLAYERS_1_POINT ||
        game_state.round_wins[0] >= ROUNDS_TO_WIN ||
        game_state.round_wins[1] >= ROUNDS_TO_WIN) {
        game_state.match_wins[player]++;
    }
    game_state.cleared = 1;
    boss_time_record();
    /* @hack spelling: two switches on the stage, the second inside the
     * first's default, where one would do */
    stage = stage_current;
    switch (stage) {
    case STAGE_RELAY_TAILS:
    case STAGE_RELAY_EGGMAN:
    case STAGE_RELAY_ROUGE:
    case STAGE_RELAY_KNUCKLES:
        if (game_state.mission == MISSION_RINGS &&
            ring_count(0) >= RINGS_GOAL) {
            results_start(player);
            camera_push_mode(player, CAMERA_MODE_FIX2);
            camera_set_adjust(player, camera_top(player), CAMERA_ADJUST_NONE);
            return;
        } else {
            stage_end(STAGE_END_CLEAR);
            break;
        }
        /* @hack redundant: a break after an if whose arms both leave */
        break;
    default:
        switch (stage) {
        case STAGE_VERSUS_SONIC_2:
            if (game_flags[FLAG_SEQUENCE] != SEQUENCE_HERO_BOSSES &&
                game_flags[FLAG_SEQUENCE] != SEQUENCE_DARK_BOSSES) {
                break;
            }
            boss_total_start();
            break;
        case STAGE_BOSS_LAST2:
            if (game_flags[FLAG_SEQUENCE] == SEQUENCE_ALL_BOSSES) {
                boss_total_start();
            }
            break;
        }
        results_start(player);
        camera_push_mode(player, CAMERA_MODE_FIX2);
        camera_set_adjust(player, camera_top(player), CAMERA_ADJUST_NONE);
    }
}

void stage_pose_winner(int player)
{
    int character;
    int other;
    int camera;
    int second;
    int voice;
    struct camera_motion motion;

    if (stage_current_id() == STAGE_CART_1) {
        return;
    }
    /* @bug memory: reads player's record untested (MOV.B at 0x8c028018),
     * though the test at 0x8c028080 below expects it may be null */
    character = player_records[player]->character;
    game_state.winner_posed = 1;
    if (player == 0) {
        other = 1;
    } else {
        other = 0;
    }
    place_winner(player);
    if (game_state.two_players != 0) {
        place_beside_winner(player, other);
    } else {
        place_winner(other);
    }
    player_request(player, PLAYER_REQUEST_ANIM, ANIM_WIN);
    if (game_state.two_players) {
        player_request(other, PLAYER_REQUEST_ANIM, ANIM_LOSE);
    } else if (stage_current_id() == STAGE_BOSS_LAST2) {
        player_request(other, PLAYER_REQUEST_ANIM, ANIM_LOSE);
    }
    if (player_records[player]) {
        player_records[player]->flags &= ~PLAYER_POSE_LOSES;
    }
    if (player_records[other]) {
        player_records[other]->flags &= ~PLAYER_POSE_LOSES;
    }
    /* a two-player game shows the pose on camera 0, over the whole screen.
     * STAGE_BOSS_LAST2 shows it on camera 0 too, and any other stage on
     * player's camera */
    if (game_state.two_players != 0) {
        game_state.single_view = 1;
        camera = 0;
    } else if (stage_current_id() == STAGE_BOSS_LAST2) {
        camera = 0;
    } else {
        camera = player;
    }
    /* @hack redundant: a loop of one pass round the camera's motion, which
     * leaves by a break where the character has no motion */
    do {
        motion.origin = &player_work[player]->pos;
        motion.ang = HALF_TURN - player_work[player]->ang.y;
        motion.motion = NULL;
        motion.flags =
            CAMERA_MOTION_HOLD | CAMERA_MOTION_FOLLOWS | CAMERA_MOTION_TURNS;
        motion.done = NULL;
        switch (character) {
        case PLAYER_SONIC:
            if (player_records[player]->kind == PLAYER_KIND_AMY) {
                motion.motion = &amy_winner_camera;
            } else {
                motion.motion = &sonic_winner_camera;
            }
            motion.speed = 0.5F;
            break;
        case PLAYER_SHADOW:
            motion.motion = &shadow_winner_camera;
            motion.speed = 0.5F;
            break;
        case PLAYER_ROUGE:
            motion.motion = &rouge_winner_camera;
            motion.speed = 0.5F;
            break;
        case PLAYER_TAILS_MECH:
            motion.motion = &tails_mech_winner_camera;
            motion.speed = 0.5F;
            break;
        case PLAYER_EGGMAN_MECH:
            motion.motion = &eggman_mech_winner_camera;
            motion.speed = 0.5F;
            break;
        case PLAYER_KNUCKLES:
            if (player_records[player]->kind == PLAYER_KIND_TICAL) {
                motion.motion = &tikal_winner_camera;
            } else {
                motion.motion = &knuckles_winner_camera;
            }
            motion.speed = 0.5F;
            break;
        case PLAYER_SUPER_SONIC:
            motion.motion = &super_sonic_winner_camera;
            motion.speed = 0.3F;
            break;
        case PLAYER_SUPER_SHADOW:
            motion.motion = &super_shadow_winner_camera;
            motion.speed = 0.3F;
            break;
        }
        if (!motion.motion) {
            break;
        }
        if (camera_top(camera) > CAMERA_MOTION_SLOT) {
            camera_remove_mode(camera, camera_top(camera));
        }
        camera_motion_start(camera, &motion);
    } while (0);
    /* the winner's voice, by its kind. a two-player game takes one voice for
     * the winner's first round won, and another for its second. a one-player
     * game takes one by the stage */
    if (game_state.two_players) {
        voice = -1;
        if (game_state.round_wins[player] == 1) {
            second = 0;
        } else {
            second = 1;
        }
        switch (player_records[player]->kind) {
        case PLAYER_SONIC:
            if (second == 0) {
                voice = 1529;
            } else {
                voice = 1531;
            }
            break;
        case PLAYER_SHADOW:
            if (second == 0) {
                voice = 1528;
            } else {
                voice = 1530;
            }
            break;
        case PLAYER_KNUCKLES:
            if (second == 0) {
                voice = 1254;
            } else {
                voice = 1256;
            }
            break;
        case PLAYER_ROUGE:
            if (second == 0) {
                voice = 1255;
            } else {
                voice = 1257;
            }
            break;
        case PLAYER_TAILS_MECH:
            if (second == 0) {
                voice = 1643;
            } else {
                voice = 1645;
            }
            break;
        case PLAYER_EGGMAN_MECH:
            if (second == 0) {
                voice = 1642;
            } else {
                voice = 1644;
            }
            break;
        case PLAYER_KIND_AMY:
            if (second == 0) {
                voice = 2670;
            } else {
                voice = 2673;
            }
            break;
        }
        if (voice != -1) {
            voice_play_priority_2(voice);
        }
    } else {
        voice = -1;
        switch (player_records[player]->kind) {
        case PLAYER_SONIC:
            switch (stage_current) {
            case STAGE_BOSS_BIGFOOT:
                voice = 1605;
                break;
            case STAGE_VERSUS_SONIC_1:
                voice = 1606;
                break;
            case STAGE_BOSS_GOLEM:
                voice = 1607;
                break;
            case STAGE_VERSUS_SONIC_2:
                voice = 1608;
                break;
            default:
                voice = 1593;
                break;
            }
            break;
        case PLAYER_SUPER_SONIC:
            if (stage_current == STAGE_BOSS_LAST2) {
                voice = 1610;
            } else {
                voice = 1593;
            }
            break;
        case PLAYER_SHADOW:
            switch (stage_current) {
            case STAGE_BOSS_HOTSHOT:
                voice = 1594;
                break;
            case STAGE_VERSUS_SONIC_1:
                voice = 1596;
                break;
            case STAGE_VERSUS_SONIC_2:
                voice = 1598;
                break;
            case STAGE_BOSS_LAST1:
                voice = 1600;
                break;
            default:
                voice = 1552;
                break;
            }
            break;
        case PLAYER_SUPER_SHADOW:
            if (stage_current == STAGE_BOSS_LAST2) {
                voice = 1602;
            } else {
                voice = 1552;
            }
            break;
        case PLAYER_KNUCKLES:
            switch (stage_current) {
            case STAGE_BOSS_BIGBOGY:
                voice = 1342;
                break;
            case STAGE_VERSUS_KNUCKLES:
                voice = 1344;
                break;
            default:
                voice = 1330;
                break;
            }
            break;
        case PLAYER_ROUGE:
            switch (stage_current) {
            case STAGE_BOSS_FDOG:
                voice = 1343;
                break;
            case STAGE_VERSUS_KNUCKLES:
                voice = 1345;
                break;
            default:
                voice = 1331;
                break;
            }
            break;
        case PLAYER_TAILS_MECH:
            if (stage_current == STAGE_VERSUS_MECH_1) {
                voice = 1715;
            } else {
                voice = 1703;
            }
            break;
        case PLAYER_EGGMAN_MECH:
            if (stage_current == STAGE_VERSUS_MECH_1) {
                voice = 1642;
            } else {
                voice = 1700;
            }
            break;
        }
        if (voice != -1) {
            voice_play_priority_2(voice);
        }
    }
    /* @hack redundant: a block that never runs */
    if (0) {
        first_character = 20;
        other_character = 20;
        frame_step = 20;
        camera_active_count = 20;
    }
}

void stage_go_back(int player)
{
    pad_disable_player(0);
    pause_disable();
    stage_clock_stop();
    stage_end(STAGE_END_BACK);
    camera_push_mode(player, CAMERA_MODE_FIX2);
    camera_set_adjust(player, camera_top(player), CAMERA_ADJUST_NONE);
}

/* draw the pause menu's frame in eleven quads of the console's textures. rows
 * and the text language pick its corners */
static void pause_draw_frame(int rows)
{
    /* the frame's corners: top left, bottom left, top right and bottom
     * right, for a menu of two rows and then for one of three. TEXT_FRENCH
     * takes the wider frame */
    static NJS_POINT3 frame[2][4] = {
        {{224.0F, 192.0F, 1.00001F},
         {224.0F, 280.0F, 1.00001F},
         {416.0F, 192.0F, 1.00001F},
         {416.0F, 280.0F, 1.00001F}},
        {{224.0F, 176.0F, 1.00001F},
         {224.0F, 296.0F, 1.00001F},
         {416.0F, 176.0F, 1.00001F},
         {416.0F, 296.0F, 1.00001F}},
    };
    static NJS_POINT3 frame_wide[2][4] = {
        {{212.0F, 192.0F, 1.00001F},
         {212.0F, 280.0F, 1.00001F},
         {428.0F, 192.0F, 1.00001F},
         {428.0F, 280.0F, 1.00001F}},
        {{212.0F, 176.0F, 1.00001F},
         {212.0F, 296.0F, 1.00001F},
         {428.0F, 176.0F, 1.00001F},
         {428.0F, 296.0F, 1.00001F}},
    };
    NJS_TEXTURE_VTX quad[4];
    NJS_POINT3 *corners;
    Int corner;
    Int side;
    Int end;
    Int bar;
    Int title;
    Int middle;

    njSetTexture(&console_texlist);
    njSetTextureNum(FRAME_MIDDLE);
    middle = njGetTextureNumG();
    njSetTextureNum(FRAME_END);
    end = njGetTextureNumG();
    njSetTextureNum(FRAME_CORNER);
    corner = njGetTextureNumG();
    njSetTextureNum(FRAME_SIDE);
    side = njGetTextureNumG();
    njSetTextureNum(FRAME_BAR);
    bar = njGetTextureNumG();
    /* @hack spelling: a call in each arm, where one call with paused ==
     * PAUSED_BY_PLAYER_1 ? FRAME_TITLE_PLAYER_1 : FRAME_TITLE_PLAYER_0 would
     * do */
    if (game_state.paused == PAUSED_BY_PLAYER_1) {
        njSetTextureNum(FRAME_TITLE_PLAYER_1);
    } else {
        njSetTextureNum(FRAME_TITLE_PLAYER_0);
    }
    title = njGetTextureNumG();
    if (game_state.text_language == TEXT_FRENCH) {
        if (rows == PAUSE_ROWS_NO_RESTART) {
            corners = frame_wide[0];
        } else {
            corners = frame_wide[1];
        }
    } else {
        if (rows == PAUSE_ROWS_NO_RESTART) {
            corners = frame[0];
        } else {
            corners = frame[1];
        }
    }
    quad[0].z = quad[1].z = quad[2].z = quad[3].z = FRAME_DEPTH;
    quad[0].col = quad[1].col = quad[2].col = quad[3].col = FRAME_COLOR;

    /* the top left corner, and the left side under it down to the end */
    quad[0].x = corners[0].x;
    quad[0].y = corners[0].y - FRAME_EDGE;
    quad[1].x = corners[0].x;
    quad[1].y = corners[0].y + FRAME_EDGE;
    quad[2].x = corners[0].x + FRAME_CORNER_WIDTH;
    quad[2].y = corners[0].y - FRAME_EDGE;
    quad[3].x = corners[0].x + FRAME_CORNER_WIDTH;
    quad[3].y = corners[0].y + FRAME_EDGE;
    quad[0].u = 0.0F;
    quad[0].v = 0.0F;
    quad[1].u = 0.0F;
    quad[1].v = 1.0F;
    quad[2].u = 1.0F;
    quad[2].v = 0.0F;
    quad[3].u = 1.0F;
    quad[3].v = 1.0F;
    njDrawTexture(quad, 4, corner, 1);

    quad[2].x = corners[0].x + FRAME_EDGE;
    quad[3].x = corners[0].x + FRAME_EDGE;
    quad[0].y = corners[0].y + FRAME_EDGE;
    quad[1].y = corners[1].y - FRAME_EDGE;
    quad[2].y = corners[0].y + FRAME_EDGE;
    quad[3].y = corners[1].y - FRAME_EDGE;
    njDrawTexture(quad, 4, side, 1);

    quad[0].y = corners[1].y - FRAME_EDGE;
    quad[1].y = corners[1].y;
    quad[2].y = corners[1].y - FRAME_EDGE;
    quad[3].y = corners[1].y;
    njDrawTexture(quad, 4, end, 1);

    /* the title across the middle of the top, and the bar to each side */
    quad[0].x = FRAME_TITLE_LEFT;
    quad[0].y = corners[0].y - FRAME_EDGE;
    quad[1].x = FRAME_TITLE_LEFT;
    quad[1].y = corners[0].y + FRAME_EDGE;
    quad[2].x = FRAME_TITLE_RIGHT;
    quad[2].y = corners[2].y - FRAME_EDGE;
    quad[3].x = FRAME_TITLE_RIGHT;
    quad[3].y = corners[2].y + FRAME_EDGE;
    njDrawTexture(quad, 4, title, 1);

    quad[0].x = corners[0].x + FRAME_CORNER_WIDTH;
    quad[1].x = corners[0].x + FRAME_CORNER_WIDTH;
    quad[2].x = FRAME_TITLE_LEFT;
    quad[3].x = FRAME_TITLE_LEFT;
    njDrawTexture(quad, 4, bar, 1);

    quad[0].x = FRAME_TITLE_RIGHT;
    quad[1].x = FRAME_TITLE_RIGHT;
    quad[2].x = corners[2].x - FRAME_CORNER_WIDTH;
    quad[3].x = corners[2].x - FRAME_CORNER_WIDTH;
    njDrawTexture(quad, 4, bar, 1);

    /* the middle, and the bottom edge, which is the side's texture turned */
    quad[0].x = corners[0].x + FRAME_EDGE;
    quad[1].x = corners[0].x + FRAME_EDGE;
    quad[2].x = corners[2].x - FRAME_EDGE;
    quad[3].x = corners[2].x - FRAME_EDGE;
    quad[0].y = corners[0].y + FRAME_EDGE;
    quad[1].y = corners[1].y - FRAME_EDGE;
    quad[2].y = corners[2].y + FRAME_EDGE;
    quad[3].y = corners[3].y - FRAME_EDGE;
    njDrawTexture(quad, 4, middle, 1);

    quad[0].y = corners[1].y;
    quad[2].y = corners[3].y;
    quad[0].u = 0.0F;
    quad[0].v = 0.0F;
    quad[1].u = 1.0F;
    quad[1].v = 0.0F;
    quad[2].u = 0.0F;
    quad[2].v = 1.0F;
    quad[3].u = 1.0F;
    quad[3].v = 1.0F;
    njDrawTexture(quad, 4, side, 1);

    /* the top right corner, the right side and its end */
    quad[0].x = corners[2].x;
    quad[0].y = corners[2].y - FRAME_EDGE;
    quad[1].x = corners[2].x;
    quad[1].y = corners[2].y + FRAME_EDGE;
    quad[2].x = corners[2].x - FRAME_CORNER_WIDTH;
    quad[2].y = corners[2].y - FRAME_EDGE;
    quad[3].x = corners[2].x - FRAME_CORNER_WIDTH;
    quad[3].y = corners[2].y + FRAME_EDGE;
    quad[0].u = 0.0F;
    quad[0].v = 0.0F;
    quad[1].u = 0.0F;
    quad[1].v = 1.0F;
    quad[2].u = 1.0F;
    quad[2].v = 0.0F;
    quad[3].u = 1.0F;
    quad[3].v = 1.0F;
    njDrawTexture(quad, 4, corner, 1);

    quad[2].x = corners[2].x - FRAME_EDGE;
    quad[3].x = corners[2].x - FRAME_EDGE;
    quad[0].y = corners[2].y + FRAME_EDGE;
    quad[1].y = corners[3].y - FRAME_EDGE;
    quad[2].y = corners[2].y + FRAME_EDGE;
    quad[3].y = corners[3].y - FRAME_EDGE;
    njDrawTexture(quad, 4, side, 1);

    quad[0].y = corners[3].y;
    quad[2].y = corners[3].y;
    quad[0].u = 0.0F;
    quad[0].v = 1.0F;
    quad[1].u = 0.0F;
    quad[1].v = 0.0F;
    quad[2].u = 1.0F;
    quad[2].v = 1.0F;
    quad[3].u = 1.0F;
    quad[3].v = 0.0F;
    njDrawTexture(quad, 4, end, 1);
}

/* draw a row of the pause menu: sprite's text, centered, and its shadow. row is
 * the count of rows above it. choice comes in as the row's text, TEXT_CONTINUE
 * to TEXT_QUIT. the switch turns it into the row's pause_choice */
static void pause_draw_row(int choice, int row, struct text_sprite *sprite)
{
    /* a black quad at the row text's depth */
    static NJS_TEXTURE_VTX unused_quad[4] = {
#include "stage/stage/unused_quad.inc"
    };
    /* the row's quad: for a row the menu's choice isn't on, then for the
     * row it is on */
    static NJS_TEXTURE_VTX row_quads[2][4] = {
#include "stage/stage/row_quads.inc"
    };
    static NJS_TEXTURE_VTX row_shadow[4] = {
#include "stage/stage/row_shadow.inc"
    };
    NJS_TEXTURE_VTX *quad;
    Float x;
    Float y;

    /* @bug memory: reads through a null sprite where a row's text couldn't
     * be made (MOV.W at 0x8c0289ee). stage_stop tests each row's sprite for
     * null (0x8c02423c, 0x8c02424c and 0x8c02425c), and pause_draw passes
     * each one untested */
#ifdef NONMATCHING
    /* this side repairs the tag above: a row without its text draws nothing */
    if (sprite == NULL) {
        return;
    }
#endif
    x = -(sprite->width * 0.5F) + SCREEN_CENTER_X;
    y = (row * ROW_HEIGHT) - (pause_rows * (ROW_HEIGHT / 2)) + SCREEN_CENTER_Y;
    switch (choice) {
    case TEXT_CONTINUE:
        choice = PAUSE_CONTINUE;
        break;
    case TEXT_RESTART:
        choice = PAUSE_RESTART;
        break;
    case TEXT_QUIT:
        choice = PAUSE_QUIT;
        break;
    }
    if (choice == pause_choice) {
        quad = row_quads[1];
    } else {
        quad = row_quads[0];
    }
    njSetTexture(&sprite->texlist);
    quad[0].x = quad[1].x = x;
    quad[2].x = quad[3].x = x + sprite->texture_width;
    quad[0].y = quad[2].y = y;
    quad[1].y = quad[3].y = y + sprite->texture_height;
    njDrawTexture(quad, 4, sprite->texture, 1);
    y += SHADOW_OFFSET;
    x = x + SHADOW_OFFSET;
    quad = row_shadow;
    quad[0].x = quad[1].x = x;
    quad[2].x = quad[3].x = x + sprite->texture_width;
    quad[0].y = quad[2].y = y;
    quad[1].y = quad[3].y = y + sprite->texture_height;
    /* @hack redundant: a step of quad that never runs */
    if (0) {
        quad++;
    }
    njDrawTexture(quad, 4, sprite->texture, 1);
}

static void pause_draw(void)
{
    int row;

    row = 0;
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    pause_draw_row(TEXT_CONTINUE, row, pause_continue_text);
    row++;
    if (pause_rows == PAUSE_ROWS) {
        pause_draw_row(TEXT_RESTART, row, pause_restart_text);
        row++;
    }
    pause_draw_row(TEXT_QUIT, row, pause_quit_text);
    pause_draw_frame(pause_rows);
    njFogEnable();
}

/* draw stage_name_text centered on SCREEN_CENTER_X and NAME_CENTER_Y, and
 * then its shadow, SHADOW_OFFSET down and right. the call measures the
 * stage's name first, and uses nothing of the measure */
static void stage_name_draw(void)
{
    static NJS_TEXTURE_VTX name_quad[4] = {
#include "stage/stage/name_quad.inc"
    };
    Uint16 width;
    Uint16 height;
    Float x;
    Float y;
    int i;

    text_sprite_measure(stage_name_text, TEXT(stage_current + TEXT_STAGE_NAMES),
                        game_state.text_language, &width, &height);
    x = -(stage_name_text->texture_width * 0.5F) + SCREEN_CENTER_X;
    y = -(stage_name_text->texture_height * 0.5F) + NAME_CENTER_Y;
    name_quad[0].z = name_quad[1].z = name_quad[2].z = name_quad[3].z =
        NAME_DEPTH;
    name_quad[0].col = name_quad[1].col = name_quad[2].col = name_quad[3].col =
        NAME_COLOR;
    njSetTexture(&stage_name_text->texlist);
    for (i = 0; i < 2; i++) {
        name_quad[0].x = name_quad[1].x = x;
        name_quad[2].x = name_quad[3].x = x + stage_name_text->texture_width;
        name_quad[0].y = name_quad[2].y = y;
        name_quad[1].y = name_quad[3].y = y + stage_name_text->texture_height;
        njDrawTexture(name_quad, 4, stage_name_text->texture, 1);
        x += SHADOW_OFFSET;
        y += SHADOW_OFFSET;
        name_quad[0].z = name_quad[1].z = name_quad[2].z = name_quad[3].z =
            NAME_SHADOW_DEPTH;
        name_quad[0].col = name_quad[1].col = name_quad[2].col =
            name_quad[3].col = NAME_SHADOW_COLOR;
    }
}

static void stage_name_show(void)
{
    njFogDisable();
    stage_name_draw();
    njFogEnable();
}

/* draw the banner of a two-player game from its texture, unfiltered */
static void banner_draw(void)
{
    static NJS_TEXTURE_VTX banner_quad[4] = {
#include "stage/stage/banner_quad.inc"
    };
    Int texture;

    njFogDisable();
    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njSetTexture(&game2p_texlist);
    njSetTextureNum(0);
    texture = njGetTextureNumG();
    njDrawTexture(banner_quad, 4, texture, 1);
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
    njFogEnable();
}

/* draw sprite as struct text_sprite's fields say: its band, then its
 * text's shadow, then its text. the two texts need the sprite's
 * texture */
static void message_draw(struct text_sprite *sprite)
{
    Float x0;
    Float y0;
    Float x1;
    Float y1;
    Int texture;

#ifdef NONMATCHING
    /* this side repairs the tag in card_notice_no_room: it draws nothing for
     * a null sprite */
    if (sprite == NULL) {
        return;
    }
#endif
    njFogDisable();
    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    x0 = sprite->x;
    y0 = sprite->y;
    x1 = x0 + (sprite->texture_width * sprite->scale_x);
    y1 = y0 + (sprite->texture_height * sprite->scale_y);
    if (sprite->band_color.argb.a) {
        NJS_POLYGON_VTX band[4];

        /* @hack redundant: the 0.0F added to band_x, where its conversion
         * alone would do */
        band[0].x = band[1].x = sprite->band_x + 0.0F;
        band[2].x = band[3].x =
            sprite->band_x + (TEXT_BAND_WIDTH * sprite->scale_x);
        band[0].y = band[2].y =
            sprite->band_y - (TEXT_BAND_MARGIN * sprite->scale_y);
        band[1].y = band[3].y =
            sprite->band_y +
            ((sprite->band_height + TEXT_BAND_MARGIN) * sprite->scale_y);
        band[0].z = band[1].z = band[2].z = band[3].z = BAND_DEPTH;
        band[0].col = band[1].col = band[2].col = band[3].col =
            sprite->band_color.color;
        njDrawPolygon(band, 4, 1);
    }
    if (sprite->shadow_color.argb.a && sprite->texlist.textures) {
        NJS_TEXTURE_VTX quad[4];
        /* @hack spelling: shadow, a local for the distance, where the
         * field read for each product would do */
        Float shadow = sprite->shadow_offset;
        Float dx = shadow * sprite->scale_x;
        Float dy = shadow * sprite->scale_y;

        quad[0].x = quad[1].x = x0 + dx;
        quad[2].x = quad[3].x = x1 + dx;
        quad[0].y = quad[2].y = y0 + dy;
        quad[1].y = quad[3].y = y1 + dy;
        quad[0].z = quad[1].z = quad[2].z = quad[3].z = MESSAGE_SHADOW_DEPTH;
        quad[0].u = quad[0].v = quad[1].u = quad[2].v = 0.0F;
        quad[1].v = quad[2].u = quad[3].u = quad[3].v = 1.0F;
        quad[0].col = quad[1].col = quad[2].col = quad[3].col =
            sprite->shadow_color.color;
        njSetTexture(&sprite->texlist);
        njSetTextureNum(0);
        texture = njGetTextureNumG();
        njDrawTexture(quad, 4, texture, 1);
    }
    if (sprite->color.argb.a && sprite->texlist.textures) {
        NJS_TEXTURE_VTX quad[4];

        quad[0].x = quad[1].x = x0;
        quad[2].x = quad[3].x = x1;
        quad[0].y = quad[2].y = y0;
        quad[1].y = quad[3].y = y1;
        quad[0].z = quad[1].z = quad[2].z = quad[3].z = MESSAGE_DEPTH;
        quad[0].u = quad[0].v = quad[1].u = quad[2].v = 0.0F;
        quad[1].v = quad[2].u = quad[3].u = quad[3].v = 1.0F;
        quad[0].col = quad[1].col = quad[2].col = quad[3].col =
            sprite->color.color;
        njSetTexture(&sprite->texlist);
        njSetTextureNum(0);
        texture = njGetTextureNumG();
        njDrawTexture(quad, 4, texture, 1);
    }
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
    njFogEnable();
}
