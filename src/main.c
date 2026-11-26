/* the main loop: the game's start, its modes, and its exit to the browser
 * or the boot ROM */
#include <kamui.h>
#include <ninja.h>
#include <nt_utl.h>
#include <sg_sd.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sofdec.h>
#include <stdlib.h>

#ifdef NONMATCHING
/* SH4_CACHE_LINE_BYTES, to which this side's matrix_stack aligns */
#include "engine/dc/kernel_layout.h"
#endif

#include "draw/fade.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "draw/screen.h"
#include "draw/text_sprite.h"
#include "hud/hud.h"
#include "main.h"
#include "player/player.h"
#include "save/card.h"
#include "save/save.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/areas.h"
#include "system/curve.h"
#include "system/flags.h"
#include "system/frame.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/sbinit.h"
#include "system/sequence.h"
#include "system/snapshot.h"
#include "system/text.h"
#include "system/texture.h"
#include "system/timer.h"
#include "vmu/screens.h"

#define MATRIX_DEPTH 128
/* the sizes in bytes of the buffers system_boot_init allocates. njInit3D
 * takes the texture buffer too, as vertices of VERTEX_SIZE bytes */
#define TEXTURE_BUFFER_SIZE 0x40000
#define VERTEX_SIZE 64
#define VERTEX_COUNT (TEXTURE_BUFFER_SIZE / VERTEX_SIZE)
#define SHAPE_BUFFER_SIZE 0x6000
/* a frame of system_boot_init's display lasts FRAME_BLANKS vertical blanks
 * and takes FRAME_STEPS steps */
#define FRAME_BLANKS 1
#define FRAME_STEPS 1
/* network_info_poll asks in the game's first NETWORK_INFO_STEPS steps, and
 * only while bit NETWORK_INFO_BIT of the count is set */
#define NETWORK_INFO_STEPS 180
#define NETWORK_INFO_BIT 4
/* GAME_MODE_MENU counts its skips of the menus in game_state.menu_passes. it
 * shows the logos on one skip in PASSES_PER_LOGOS, and plays sequence 15 on the
 * others */
#define PASSES_PER_LOGOS 3
/* the ntUtl flag holds the display main keeps as it leaves for the browser,
 * plus FLAG_BIAS. a flag of 0 keeps no display */
#define FLAG_BIAS 1
/* the angle the screen's width spans */
#define VIEW_ANGLE NJM_DEG_ANG(70)
/* the depths the depth queue fades between */
#define DEPTH_QUEUE_NEAR (-1800.0F)
#define DEPTH_QUEUE_FAR (-2000.0F)
#define CLIP_NEAR (-3.0F)
#define CLIP_FAR (-3000.0F)
/* the fog's near and far depths, as njGenerateFogTable3 takes them */
#define FOG_NEAR (-1000.0F)
#define FOG_FAR (-1500.0F)
#define CHEAP_SHADOW_INTENSITY 80
/* the size by which KM_CULLSMALL culls a polygon, in the hardware's culling
 * register */
#define CULLING_SIZE 0.05F
/* the texture memory's size, in bytes */
#define TEXTURE_MEMORY_SIZE 0x114000
/* end the program in STAGE_SLOT, and set Ninja up again for the next mode */
#define END_PROGRAM()                                                          \
    (overlay_run_exit(), game_release_textures(), display_reset_full())

/* njInitMatrix uses this array in place. The linker must align its first
 * slot for MOVCA.L, and each 64-byte slot preserves that alignment. */
#pragma section 32
#ifdef NONMATCHING
/* aligndata32 is SHC's pragma, so this side aligns the stack by attribute.
 * the typedef below fails the compile, as an array of -1 chars, unless
 * NJS_MATRIX is 64 bytes aligned to 8 and the stack is 8192 bytes aligned to
 * a cache line */
static NJS_MATRIX matrix_stack[MATRIX_DEPTH]
    __attribute__((aligned(SH4_CACHE_LINE_BYTES)));
typedef char
    matrix_stack_storage[(__alignof__(NJS_MATRIX) >= 8 &&
                          sizeof(NJS_MATRIX) == 64 &&
                          __alignof__(matrix_stack) >= SH4_CACHE_LINE_BYTES &&
                          sizeof(matrix_stack) == 8192)
                             ? 1
                             : -1];
#else
#pragma aligndata32(matrix_stack)
static NJS_MATRIX matrix_stack[MATRIX_DEPTH];
#endif
#pragma section

/* four sizes of the screen, w and h in pixels */
struct screen_size {
    Float w, h;
    Float unk_08;
    Sint32 unk_0c;
};
static struct screen_size screen_sizes[4] = {
    {600.0F, 440.0F, 0.4999F, -1},
    {600.0F, 480.0F, 0.4999F, -1},
    {640.0F, 440.0F, 0.4999F, -1},
    {640.0F, 480.0F, 0.4999F, -1},
};

/* the buffers and the texture list system_boot_init allocates from the Shinobi
 * heap, null until then. the image holds them in its data, not its bss, as it
 * holds game_leaving. so each has an initializer */
static void *texture_buffer = NULL;
NJS_TEXMEMLIST *texmemlist = NULL;
static void *shape_buffer = NULL;

struct game_state game_state = {
    0, /* demo */
    0, /* next_demo */
    0, /* debug */
    0, /* debug_display */
    {0}, /* unk_04 */
    0, /* unk_09 */
    0, /* unk_0a */
    1, /* unk_0b */
    1, /* hud_on */
    1, /* lives_shown */
    1, /* rings_shown */
    0, /* display */
    0, /* mono */
    0, /* text_language */
    0, /* voice_language */
    0, /* tray_opened */
    CARD_IDLE, /* card_state */
    0, /* card_removed */
    0, /* pause_disabled */
    0, /* paused */
    0, /* unk_18 */
    0, /* sound_paused */
    0, /* clock_stopped */
    0, /* minute */
    0, /* second */
    0, /* frame */
    0, /* two_players */
    0, /* split_screen */
    0, /* four_cameras */
    0, /* single_view */
    MISSION_FIRST, /* mission */
    1, /* music_on */
    1, /* voices_on */
    0, /* unk_25 */
    {0, 0, 1}, /* unk_26 */
    0, /* split_layout */
    0, /* unk_2a */
    0, /* camera_motion_playing */
    {{0, 0, 0}, {0, 0, 0}}, /* ring_levels */
    {0, 0}, /* ring_level_timer */
    {0, 0}, /* round_wins */
    {0, 0}, /* match_wins */
    0, /* time_stopped */
    0, /* sg_players */
    0, /* mini_event_running */
    1, /* idle_dim */
    0, /* block_grid_loaded */
    1, /* builds_drawn */
    0, /* hidden_cameras */
    1, /* pads_on */
    {1, 1}, /* player_pads_on */
    0, /* rank */
    0, /* cleared */
    0, /* winner_posed */
    0, /* chao_key */
    7, /* edition */
    0, /* trial_screen */
    CARD_WARNING_ALERT, /* card_warning */
    0, /* us_sega_logo */
    0, /* solo_player */
    1, /* vibration */
    0, /* active_player */
    NO_ROUND_WINNER, /* round_winner */
    0, /* relay */
    0, /* kept_minute */
    0, /* kept_second */
    0, /* kept_frame */
    1, /* message_window_on */
    1, /* unk_53 */
    {0, 0}, /* costume */
    {0, 0}, /* alt_character */
    0, /* chao_race_menu */
    0, /* leave_to_browser */
    1, /* skip_menus */
    0, /* unk_5b */
    0, /* display_pending */
    0, /* save_drive */
    0, /* theme */
    0, /* network_info_found */
    {0, 0}, /* lives */
    {0, 0}, /* rings */
    0, /* kept_rings */
    0, /* ring_total */
    0, /* kept_ring_total */
    0, /* emblems */
    0, /* total_steps */
    0, /* steps */
    GAME_MODE_BOOT, /* mode */
    0, /* next_mode */
    0, /* resume_mode */
    {0}, /* unk_84 */
    0, /* score */
    0, /* kept_score */
    0, /* idle_calls */
    {-1, -1}, /* land_block_mask */
    {0, 0}, /* land_blocks */
    0, /* kart_course */
    0, /* kart_character_karts */
    0, /* other_print */
    0, /* unlock_id */
    0, /* unlock_pending */
    0, /* menu_passes */
    0, /* save_date */
    0.0F, /* frame_time */
    0.0F, /* draw_time */
    0.0F, /* render_time */
    1.0F, /* unk_cc */
    0, /* cable */
};

static void (*game_frame_wait)(void) = frame_wait;
static Bool game_leaving = FALSE;

NJS_CAMERA camera;
NJS_SCREEN screen;
NJS_FOG_TABLE fog_table;

void game_set_frame_wait(void (*wait)(void))
{
    if (wait != NULL) {
        game_frame_wait = wait;
    } else {
        game_frame_wait = frame_wait;
    }
}

/* nothing in C calls main: the startup code holds its address. each of
 * main's ends is an endless loop, so SHC saves no registers in it */
void main(void) /* NOLINT(misc-use-internal-linkage) */
{
    game_init();
    while (!game_leaving) {
        timer_frame_start();
        if (game_mode_step() == -1) {
            game_leaving = TRUE;
        }
        timer_mark();
        /* @hack redundant: a debug print that never runs */
        if (0) {
            Uint32 time = (Uint32)timer_lap();

            njPrintD(NJM_LOCATION(21, 5), time, 8);
        }
        sound_update();
        timer_lap();
        /* the current snapshot takes each write to game_state.total_steps
         * too, or snapshots_swap fails */
        game_state.total_steps += frame_step_count;
        snapshot_current->total_steps += frame_step_count;
        if (snapshots_swap() == -1) {
            save_data.status |= SAVE_SWAP_FAILED;
        }
        timer_frame_end();
        game_frame_wait();
    }
    while (game_state.card_state == CARD_SAVING) {
    }
    if (game_state.leave_to_browser == 1) {
        ntUtlSetJumpNumber(0);
        gdFsChangeDir("..");
        /* system_boot_init starts the game's next run in this display */
        if (game_state.cable == SYE_CBL_PAL) {
            ntUtlSetFlag(game_state.display + FLAG_BIAS);
        }
        ntLDBrowser();
        for (;;) {
        }
    }
    game_shutdown();
    syBtExit();
    for (;;) {
    }
}

void game_init(void)
{
    system_boot_init();
    card_boot();
    frame_wait();
    frame_wait();
    game_state.mode = GAME_MODE_BOOT;
    game_state.tray_opened = 0;
    gdFsEntryErrFuncAll(loader_note_error, NULL);
    text_fonts_load();
}

#ifdef NONMATCHING
/* load the program overlay name, whose head holds tag, and run its init. the
 * game can't go on without the program: one that won't start stops it, as
 * loader_stop says. MCWARN.PRS, which the game goes on without, starts by
 * loader_start_program instead */
static void start_program(const char *name, const char *tag)
{
    if (!loader_start_program(name, tag)) {
        loader_stop(name);
    }
}
#endif

int game_mode_step(void)
{
    struct game_state *state;
    int result = OVERLAY_RUNNING;
    int cards;

    /* @hack spelling: state set in the switch's expression, where its
     * declaration would set it */
    switch ((state = &game_state)->mode) {
    case GAME_MODE_BOOT:
        stage_boot();
        cards = card_check();
        if (cards != CARD_CHECK_OK) {
            if (cards == CARD_CHECK_NO_CARD) {
                state->card_warning = CARD_WARNING_NO_CARD;
            } else {
                state->card_warning = CARD_WARNING_NO_ROOM;
            }
            state->mode = GAME_MODE_CARD_WARNING;
            break;
        }
        /* fall through */
    case GAME_MODE_START:
        if (state->display_pending) {
            state->mode = GAME_MODE_MENU;
            state->skip_menus = 0;
        } else {
            state->mode = GAME_MODE_TO_MENU;
        }
        result = OVERLAY_RUNNING;
        break;
    case GAME_MODE_TO_MENU:
        state->mode = GAME_MODE_MENU;
        break;
    case GAME_MODE_STAGE:
        frame_render_off = 1;
#ifdef NONMATCHING
        /* this side starts no stage without set 0's textures, and leaves the
         * mode as it was */
        if (!texture_load_set(0)) {
            break;
        }
#else
        texture_load_set(0);
#endif
        state->round_wins[0] = 0;
        state->round_wins[1] = 0;
        state->round_winner = NO_ROUND_WINNER;
        state->relay = 0;
#ifdef NONMATCHING
        /* this side releases set 0 and starts nothing where stage_start fails.
         * stage_start fails without the language files, as stage.h says */
        if (!stage_start()) {
            texture_release_set();
            break;
        }
#else
        stage_start();
#endif
        state->mode = GAME_MODE_STAGE_RUN;
        result = OVERLAY_RUNNING;
        break;
    case GAME_MODE_STAGE_RUN:
        frame_render_off = 0;
        result = stage_step();
        if (result == OVERLAY_RESET) {
            stage_stop();
            state->mode = GAME_MODE_TO_MENU;
            game_release_textures();
            display_reset_full();
            result = OVERLAY_RUNNING;
            state->trial_screen = TRIAL_SCREEN_TITLE;
            state->skip_menus = 0;
            menu_resume_target = 0;
        } else if (result == OVERLAY_BACK) {
            stage_stop();
            state->mode = GAME_MODE_TO_MENU;
            game_release_textures();
            display_reset_full();
            result = OVERLAY_RUNNING;
            if (state->trial_screen != TRIAL_SCREEN_COMING_SOON) {
                state->trial_screen = TRIAL_SCREEN_TITLE;
            }
            game_mode_divert();
            state->skip_menus = 0;
        } else if (result == OVERLAY_DONE) {
            stage_stop();
            if (state->relay) {
                /* stage_step has made the relay's next stage current. player
                 * 0 takes on that stage's character */
                state->mode = GAME_MODE_STAGE_AGAIN;
                switch (stage_current_id()) {
                case STAGE_RELAY_TAILS:
                    stage_set_character(0, PLAYER_TAILS_MECH);
                    break;
                case STAGE_RELAY_EGGMAN:
                    stage_set_character(0, PLAYER_EGGMAN_MECH);
                    break;
                case STAGE_RELAY_ROUGE:
                    stage_set_character(0, PLAYER_ROUGE);
                    break;
                case STAGE_RELAY_KNUCKLES:
                    stage_set_character(0, PLAYER_KNUCKLES);
                    break;
                case STAGE_RELAY_SONIC:
                    stage_set_character(0, PLAYER_SONIC);
                    break;
                }
            } else if (state->chao_key) {
                state->mode = GAME_MODE_STAGE_AGAIN;
            } else if (state->two_players) {
                /* @hack spelling: a test of its own for two_players, where
                 * chao_key || two_players would do */
                state->mode = GAME_MODE_STAGE_AGAIN;
            } else if (game_flags[FLAG_SEQUENCE]) {
                state->mode = GAME_MODE_SEQUENCE;
            } else {
                state->mode = GAME_MODE_TO_MENU;
                state->skip_menus = 0;
            }
            game_release_textures();
            display_reset_full();
            result = OVERLAY_RUNNING;
            game_mode_divert();
        }
        break;
    case GAME_MODE_STAGE_AGAIN:
        frame_render_off = 1;
#ifdef NONMATCHING
        /* this side starts no stage without set 0's textures, and leaves the
         * mode as it was */
        if (!texture_load_set(0)) {
            break;
        }
#else
        texture_load_set(0);
#endif
#ifdef NONMATCHING
        /* this side releases set 0 and starts nothing where stage_start fails.
         * stage_start fails without the language files, as stage.h says */
        if (!stage_start()) {
            texture_release_set();
            break;
        }
#else
        stage_start();
#endif
        state->mode = GAME_MODE_STAGE_RUN;
        result = OVERLAY_RUNNING;
        break;
    case GAME_MODE_EVENT:
        frame_render_off = 1;
#ifdef NONMATCHING
        /* this side admits the program before its init runs, and stops the
         * game where it won't start */
        start_program("EVENT.PRS", "EVENT   ");
#else
        loader_read_prs("EVENT.PRS", STAGE_SLOT);
        overlay_run_init();
#endif
        state->mode = GAME_MODE_EVENT_RUN;
        result = OVERLAY_RUNNING;
        break;
    case GAME_MODE_EVENT_RUN:
        frame_render_off = 0;
        switch (PROGRAM_OVERLAY->step()) {
        case OVERLAY_RESET:
            END_PROGRAM();
            state->mode = GAME_MODE_TO_MENU;
            if (state->demo == DEMO_ON) {
                result = OVERLAY_RESET;
            } else {
                result = OVERLAY_RUNNING;
                state->trial_screen = TRIAL_SCREEN_TITLE;
                state->skip_menus = 0;
                menu_resume_target = 0;
            }
            break;
        case OVERLAY_RUNNING:
            result = OVERLAY_RUNNING;
            break;
        case OVERLAY_BACK:
        case OVERLAY_DONE:
            END_PROGRAM();
            state->mode = state->next_mode;
            result = OVERLAY_RUNNING;
            if (state->edition != EDITION_TENTO &&
                game_flags[FLAG_SEQUENCE] == SEQUENCE_LAST) {
                game_mode_divert();
            }
            state->skip_menus = 0;
            break;
        }
        break;
    case GAME_MODE_MENU:
        if (state->skip_menus && menu_resume_target == 0) {
            if (state->menu_passes % PASSES_PER_LOGOS == 0) {
                state->mode = GAME_MODE_OTHER_PRINT;
                state->next_mode = GAME_MODE_MENU;
                state->resume_mode = GAME_MODE_MENU;
                state->other_print = OTHER_PRINT_LOGOS;
            } else {
                sequence_start(SEQUENCE_MENU_EVENT);
                state->mode = GAME_MODE_SEQUENCE;
                state->skip_menus = 0;
            }
            result = OVERLAY_RUNNING;
            state->menu_passes++;
        } else {
            sequence_start(SEQUENCE_NONE);
            frame_render_off = 1;
#ifdef NONMATCHING
            /* this side admits the program before its init runs, and stops the
             * game where it won't start */
            start_program("ADVERTISE.PRS", "ADVERTIZ");
#else
            loader_read_prs("ADVERTISE.PRS", STAGE_SLOT);
            overlay_run_init();
#endif
            result = OVERLAY_RUNNING;
            state->mode = GAME_MODE_MENU_RUN;
            state->skip_menus = 1;
        }
        state->trial_screen = 0;
        break;
    case GAME_MODE_MENU_RUN:
        frame_render_off = 0;
        switch (PROGRAM_OVERLAY->step()) {
        case OVERLAY_RESET:
            END_PROGRAM();
            state->mode = GAME_MODE_TO_MENU;
            result = OVERLAY_RUNNING;
            state->trial_screen = TRIAL_SCREEN_TITLE;
            menu_resume_target = 0;
            break;
        case OVERLAY_RUNNING:
            result = OVERLAY_RUNNING;
            break;
        case OVERLAY_BACK:
            /* main leaves the game for the browser once this frame ends */
            END_PROGRAM();
            game_leaving = TRUE;
            state->leave_to_browser = 1;
            break;
        case OVERLAY_DONE:
            END_PROGRAM();
            if (state->display_pending) {
                state->display_pending = 0;
                state->mode = GAME_MODE_MENU;
            } else {
                state->mode = state->next_mode;
            }
            result = OVERLAY_RUNNING;
            break;
        }
        break;
    case GAME_MODE_ENDING:
        frame_render_off = 1;
#ifdef NONMATCHING
        /* this side admits the program before its init runs, and stops the
         * game where it won't start */
        start_program("ENDING.PRS", "ENDING  ");
#else
        loader_read_prs("ENDING.PRS", STAGE_SLOT);
        overlay_run_init();
#endif
        state->mode = GAME_MODE_ENDING_RUN;
        result = OVERLAY_RUNNING;
        break;
    case GAME_MODE_ENDING_RUN:
        frame_render_off = 0;
        switch (PROGRAM_OVERLAY->step()) {
        case OVERLAY_RESET:
            END_PROGRAM();
            state->mode = GAME_MODE_TO_MENU;
            result = OVERLAY_RUNNING;
            state->trial_screen = TRIAL_SCREEN_TITLE;
            state->skip_menus = 0;
            menu_resume_target = 0;
            break;
        case OVERLAY_RUNNING:
            result = OVERLAY_RUNNING;
            break;
        case OVERLAY_BACK:
        case OVERLAY_DONE:
            END_PROGRAM();
            state->mode = state->next_mode;
            result = OVERLAY_RUNNING;
            game_mode_divert();
            state->skip_menus = 1;
            menu_resume_target = 0;
            state->menu_passes = 0;
            break;
        }
        break;
    case GAME_MODE_EMBLEM:
        frame_render_off = 1;
#ifdef NONMATCHING
        /* this side admits the program before its init runs, and stops the
         * game where it won't start */
        start_program("EMBLEMGET.PRS", "EMBLEM_G");
#else
        loader_read_prs("EMBLEMGET.PRS", STAGE_SLOT);
        overlay_run_init();
#endif
        state->mode = GAME_MODE_EMBLEM_RUN;
        result = OVERLAY_RUNNING;
        break;
    case GAME_MODE_EMBLEM_RUN:
        frame_render_off = 0;
        switch (PROGRAM_OVERLAY->step()) {
        case OVERLAY_RESET:
            END_PROGRAM();
            state->mode = GAME_MODE_TO_MENU;
            result = OVERLAY_RUNNING;
            state->trial_screen = TRIAL_SCREEN_TITLE;
            state->skip_menus = 0;
            menu_resume_target = 0;
            break;
        case OVERLAY_RUNNING:
            result = OVERLAY_RUNNING;
            break;
        case OVERLAY_BACK:
        case OVERLAY_DONE:
            END_PROGRAM();
            state->mode = state->resume_mode;
            result = OVERLAY_RUNNING;
            break;
        }
        break;
    case GAME_MODE_DEMO:
        frame_render_off = 1;
#ifdef NONMATCHING
        /* this side starts no stage without set 0's textures, and leaves the
         * mode as it was */
        if (!texture_load_set(0)) {
            break;
        }
#else
        texture_load_set(0);
#endif
        state->round_wins[0] = 0;
        state->round_wins[1] = 0;
        state->round_winner = NO_ROUND_WINNER;
        state->mission = MISSION_FIRST;
#ifdef NONMATCHING
        /* this side releases set 0 and starts nothing where stage_start fails.
         * stage_start fails without the language files, as stage.h says */
        if (!stage_start()) {
            texture_release_set();
            break;
        }
#else
        stage_start();
#endif
        state->mode = GAME_MODE_DEMO_RUN;
        result = OVERLAY_RUNNING;
        break;
    case GAME_MODE_DEMO_RUN:
        frame_render_off = 0;
        result = stage_step();
        if (result == OVERLAY_RESET) {
            stage_stop();
            state->mode = GAME_MODE_TO_MENU;
            game_release_textures();
            display_reset_full();
            result = OVERLAY_RESET;
        } else if (result != OVERLAY_RUNNING) {
            stage_stop();
            state->mode = GAME_MODE_TO_MENU;
            game_release_textures();
            display_reset_full();
            result = OVERLAY_RUNNING;
            state->demo = 0;
            state->trial_screen = 0;
        }
        break;
    case GAME_MODE_SEQUENCE:
        sequence_apply_step();
        result = OVERLAY_RUNNING;
        break;
    case GAME_MODE_CARD_WARNING:
        frame_render_off = 1;
#ifdef NONMATCHING
        /* this side takes loader_start_program's checks, as the other programs
         * do */
        if (loader_start_program("MCWARN.PRS", "MCWARN  ")) {
            state->mode = GAME_MODE_CARD_WARNING_RUN;
        } else {
            END_PROGRAM();
            state->mode = GAME_MODE_START;
            state->card_warning = 0;
        }
#else
        if (loader_read_prs("MCWARN.PRS", STAGE_SLOT) < 0) {
            END_PROGRAM();
            state->mode = GAME_MODE_START;
            state->card_warning = 0;
        } else {
            overlay_run_init();
            state->mode = GAME_MODE_CARD_WARNING_RUN;
        }
#endif
        result = OVERLAY_RUNNING;
        break;
    case GAME_MODE_CARD_WARNING_RUN:
        frame_render_off = 0;
        switch (PROGRAM_OVERLAY->step()) {
        case OVERLAY_RESET:
            END_PROGRAM();
            state->card_warning = 0;
            result = OVERLAY_RESET;
            break;
        case OVERLAY_RUNNING:
            result = OVERLAY_RUNNING;
            break;
        case OVERLAY_BACK:
        case OVERLAY_DONE:
            END_PROGRAM();
            state->mode = GAME_MODE_START;
            state->card_warning = 0;
            result = OVERLAY_RUNNING;
            break;
        }
        break;
    case GAME_MODE_OTHER_PRINT:
        frame_render_off = 1;
#ifdef NONMATCHING
        /* this side admits the program before its init runs, and stops the
         * game where it won't start */
        start_program("OTHERPRINT.PRS", "OTHERPRI");
#else
        loader_read_prs("OTHERPRINT.PRS", STAGE_SLOT);
        overlay_run_init();
#endif
        state->mode = GAME_MODE_OTHER_PRINT_RUN;
        result = OVERLAY_RUNNING;
        break;
    case GAME_MODE_OTHER_PRINT_RUN:
        frame_render_off = 0;
        switch (PROGRAM_OVERLAY->step()) {
        case OVERLAY_RESET:
            END_PROGRAM();
            result = OVERLAY_RESET;
            state->mode = GAME_MODE_TO_MENU;
            break;
        case OVERLAY_RUNNING:
            result = OVERLAY_RUNNING;
            break;
        case OVERLAY_BACK:
        case OVERLAY_DONE:
            END_PROGRAM();
            state->mode = state->resume_mode;
            result = OVERLAY_RUNNING;
            break;
        }
        break;
    default:
        result = OVERLAY_RUNNING;
        break;
    }
    fade_draw();
    screens_fill();
    network_info_poll();
    /* result is OVERLAY_RESET only where the reset ends the game */
    if (result == OVERLAY_RESET) {
        return -1;
    }
    return 0;
}

void game_shutdown(void)
{
    text_fonts_free();
    game_release_textures();
    system_shutdown();
    sdSysFinish();
    sbExitSystem();
}

void game_release_textures(void)
{
    texture_release_set();
    njExitTexture();
}

void display_reset_full(void)
{
    NJS_ARGB material;

    njInitMatrix(matrix_stack, MATRIX_DEPTH, 0);
    screen.w = SCREEN_WIDTH;
    screen.h = SCREEN_HEIGHT;
    screen.cx = screen.w * 0.5F;
    screen.cy = screen.h * 0.5F;
    njSetScreen(&screen);
    njSetPerspective(VIEW_ANGLE);
    njInitCamera(&camera);
    njSetCamera(&camera);
    njInitTexture(texmemlist, TEXTURE_COUNT);
    njSetBackColor(0, 0, 0);
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
    njSuperSampleMode(0);
    njTextureClampMode(NJD_TEXTURECLAMP_NOCLAMP);
    njTextureFlipMode(NJD_TEXTUREFLIP_NOFLIP);
    njControl3D(NJD_CONTROL_3D_CONSTANT_ATTR | NJD_CONTROL_3D_ENABLE_ALPHA |
                NJD_CONTROL_3D_OFFSET_MATERIAL | NJD_CONTROL_3D_DEPTH_QUEUE |
                NJD_CONTROL_3D_MODEL_CLIP |
                NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL);
    njSetConstantAttr(0xffffffff, NJD_FILTER_BILINEAR);
    material.a = material.r = material.g = material.b = 0.0F;
    njSetConstantMaterial(&material);
    njSetDepthQueue(DEPTH_QUEUE_NEAR, DEPTH_QUEUE_FAR);
    njClipZ(CLIP_NEAR, CLIP_FAR);
    njSetCheapShadowMode(CHEAP_SHADOW_INTENSITY);
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 0.0F, 1.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.0F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    simple_light_set_direction(0.3F, 0.1F, 1.0F);
    njCnkSetSimpleLightIntensity(1.0F, 0.0F);
    njCnkSetSimpleLightColor(1.0F, 1.0F, 1.0F);
    njSetFogColor(0x00ffffff);
    njGenerateFogTable3(fog_table, FOG_NEAR, FOG_FAR);
    njSetFogTable(fog_table);
    njFogEnable();
    njPolygonCullingMode(KM_CULLSMALL);
    njPolygonCullingSize(CULLING_SIZE);
    srand(RAND_SEED); /* NOLINT(bugprone-random-generator-seed) */
    njPrintSize(PRINT_SIZE);
}

void system_boot_init(void)
{
    Uint32 display;
    Sint32 setting;
    Sint32 language;
    Sint32 voice;

    mwPlyPreInitSofdec();
    njSetTextureMemorySize(TEXTURE_MEMORY_SIZE);
    ntUtlInit();
    game_state.cable = syCblCheck();
    if (game_state.cable == SYE_CBL_VGA) {
        sbInitSystem(NJD_RESOLUTION_VGA, NJD_FRAMEBUFFER_MODE_RGB565,
                     FRAME_BLANKS);
        game_state.display = DISPLAY_VGA;
    } else {
        /* a PAL cable with no display kept starts in DISPLAY_PAL.
         * display_pending then opens the menus on their TV setting */
        display = 0;
        ntUtlGetFlag(&display);
        display -= FLAG_BIAS;
        if (display > DISPLAY_PAL && game_state.cable == SYE_CBL_PAL) {
            game_state.display_pending = 1;
            display = DISPLAY_PAL;
        }
        if (display == DISPLAY_PAL) {
            sbInitSystem(RESOLUTION_PAL, NJD_FRAMEBUFFER_MODE_RGB565,
                         FRAME_BLANKS);
            game_state.display = DISPLAY_PAL;
        } else {
            sbInitSystem(NJD_RESOLUTION_640x480_NTSCNI,
                         NJD_FRAMEBUFFER_MODE_RGB565, FRAME_BLANKS);
            game_state.display = DISPLAY_NTSC;
        }
    }
    if (syCfgInit(CONFIG_WORK) == 0) {
        voice = VOICE_ENGLISH;
#ifdef NONMATCHING
        /* this side repairs the tag below: where syCfgGetLanguage fails, it
         * takes the default case's language */
        language = TEXT_ENGLISH;
#endif
        /* @bug uninitialized: a syCfgGetLanguage that fails leaves language
         * unset (BF at 0x8c02cfa4). MOV.B R4,@(R0,R14) at 0x8c02cfdc then
         * stores what the call left in R4 as the text language */
        /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
        if (syCfgGetLanguage(&setting) == 0) {
            switch (setting) {
            case SYD_CFG_JAPANESE:
                voice = VOICE_JAPANESE;
                language = TEXT_JAPANESE;
                break;
            case SYD_CFG_ENGLISH:
                language = TEXT_ENGLISH;
                break;
            case SYD_CFG_GERMAN:
                language = TEXT_GERMAN;
                break;
            case SYD_CFG_FRENCH:
                language = TEXT_FRENCH;
                break;
            case SYD_CFG_SPANISH:
                language = TEXT_SPANISH;
                break;
            case SYD_CFG_ITALIAN:
                language = TEXT_ENGLISH;
                break;
            default:
                language = TEXT_ENGLISH;
                break;
            }
        }
        /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
        game_state.text_language = language;
        game_state.voice_language = voice;
        if (syCfgGetSoundMode(&setting) == 0) {
            if (setting == SYD_CFG_STEREO) {
                game_state.mono = 0;
            } else {
                game_state.mono = 1;
            }
        }
        syCfgExit();
    }
    njSetBorderColor(0);
    texture_buffer = syMalloc(TEXTURE_BUFFER_SIZE);
    shape_buffer = syMalloc(SHAPE_BUFFER_SIZE);
    frame_pacing_init(FRAME_BLANKS, FRAME_STEPS);
    snapshots_start();
    sound_system_init();
    sound_init();
    text_alloc();
    line_batches_alloc();
    play_reset();
    njInitTextureBuffer(texture_buffer, TEXTURE_BUFFER_SIZE);
    njInitVertexBuffer(OPAQUE_WORDS, OPAQUE_MODIFIER_WORDS, TRANSLUCENT_WORDS,
                       TRANSLUCENT_MODIFIER_WORDS, PUNCH_THROUGH_WORDS);
    njInitPrint(NULL, 0, 0);
    njInit3D(texture_buffer, VERTEX_COUNT);
    njInitShape(shape_buffer);
    texmemlist = syMalloc(TEXTURE_COUNT * sizeof(NJS_TEXMEMLIST));
    display_reset_full();
    player_pads[0] = njGetPeripheral(NJD_PORT_A0);
    player_pads[1] = njGetPeripheral(NJD_PORT_B0);
    player_pads[2] = njGetPeripheral(NJD_PORT_C0);
    player_pads[3] = njGetPeripheral(NJD_PORT_D0);
    curve_weights_init();
}

void system_shutdown(void)
{
    sound_exit();
    text_free();
    line_batches_free();
    syFree(texmemlist);
    texmemlist = NULL;
    ntUtlExit();
}

void game_mode_divert(void)
{
    struct game_state *state = &game_state;

    if (state->edition == EDITION_TENTO) {
        if (state->two_players) {
            state->resume_mode = GAME_MODE_MENU;
            state->mode = GAME_MODE_OTHER_PRINT;
            state->other_print = OTHER_PRINT_ENDING;
            state->skip_menus = 0;
        }
    } else if (game_flags[FLAG_SEQUENCE] != 15) {
        if (save_data.emblems != state->emblems) {
            state->resume_mode = state->mode;
            state->mode = GAME_MODE_EMBLEM;
        }
    }
}

void network_info_poll(void)
{
    struct game_state *state = &game_state;
    Uint32 steps;

    if (state->network_info_found == 0) {
        steps = state->total_steps;
        if (steps < NETWORK_INFO_STEPS && (steps >> NETWORK_INFO_BIT) & 1) {
            if (ntInfIsNetworkInfo() == NTD_OK) {
                state->network_info_found = 1;
            }
        }
    }
}
