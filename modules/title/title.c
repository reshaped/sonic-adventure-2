/* TITLE.PRS's init, exit and step, which its head names. the program shows
 * the title screen and its test menu. under EDITION_TAIKEN it shows the
 * trial's title and the screens before it instead. a name with Taiken in it
 * is the trial's */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "draw/fade.h"
#include "draw/screen.h"
#include "hud/hud.h"
#include "hud/item_stock.h"
#include "main.h"
#include "player/player.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/flags.h"
#include "system/frame.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/sequence.h"
#include "system/task.h"
#include "system/text.h"
#include "system/texture.h"
#include "system/util.h"
#include "title/title.h"

/* the values of title_phase, three to a screen. a _START phase starts the
 * screen. the phase after it steps the screen's tasks each frame. once
 * title_result leaves OVERLAY_RUNNING, that phase releases the screen's
 * textures and destroys every task. a _DONE phase picks the next screen's
 * _START. PHASE_TITLE_DONE picks none, and title_step stays there */
enum phase {
    PHASE_SEGA_START,
    PHASE_SEGA,
    PHASE_SEGA_DONE,
    PHASE_SONIC_TEAM_START,
    PHASE_SONIC_TEAM,
    PHASE_SONIC_TEAM_DONE,
    PHASE_SOFDEC_START,
    PHASE_SOFDEC,
    PHASE_SOFDEC_DONE,
    PHASE_ALERT_START,
    PHASE_ALERT,
    PHASE_ALERT_DONE,
    PHASE_COMING_SOON_START,
    PHASE_COMING_SOON,
    PHASE_COMING_SOON_DONE,
    PHASE_ANNIVERSARY_START,
    PHASE_ANNIVERSARY,
    PHASE_ANNIVERSARY_DONE,
    PHASE_TITLE_START,
    PHASE_TITLE,
    PHASE_TITLE_DONE
};

/* the modes of the logos', the alert's and the coming soon screen's tasks */
enum { MODE_LOGO_IN, MODE_LOGO_HOLD, MODE_LOGO_OUT, MODE_LOGO_DONE };

/* the modes of title_screen_step's task, which runs the title screen and the
 * trial's title */
enum {
    MODE_TITLE_IN,
    MODE_TITLE,
    MODE_TEST_MENU,
    MODE_PLAYER_1,
    MODE_PLAYER_2,
    MODE_STAGE_MENU,
    MODE_TAIKEN_ZOOM,
    MODE_TAIKEN_FLASH,
    MODE_TAIKEN_TITLE,
    MODE_TAIKEN_START,
    MODE_TAIKEN_OUT,
    MODE_TITLE_OUT,
    MODE_TITLE_END
};

/* the test menu's entries, by the title screen task's submode in
 * MODE_TEST_MENU */
enum {
    MENU_GAME_1P,
    MENU_GAME_2P_3_POINTS,
    MENU_GAME_2P_1_POINT,
    MENU_TEST_EVENT,
    MENU_DEMO_1P,
    MENU_DEMO_2P,
    MENU_DEMO_PREVIOUS,
    MENU_PLAY_HERO,
    MENU_PLAY_DARK,
    MENU_PLAY_LAST,
    MENU_ENTRIES
};

/* where the test menu prints, by NJM_LOCATION's column and row */
#define MENU_LABEL_AT NJM_LOCATION(14, 26)
#define MENU_ENTRY_AT NJM_LOCATION(19, 26)
#define MENU_CHARACTER_AT NJM_LOCATION(17, 26)
#define MENU_STAGE_LABEL_AT NJM_LOCATION(7, 26)
#define MENU_STAGE_AT NJM_LOCATION(13, 26)
#define MENU_EQUIPMENT_AT NJM_LOCATION(2, 4)
#define MENU_MISSION_AT NJM_LOCATION(2, 5)
#define MENU_MISSION_NAME_AT NJM_LOCATION(32, 5)

#define FRAME_BLANKS 1
#define FRAME_STEPS 1
#define TASK_LEVEL 0

/* the buttons that end the fade in of a logo or the alert at once */
#define SKIP_BUTTONS (PDD_DGT_TX | PDD_DGT_ST | PDD_DGT_TA | PDD_DGT_TB)
/* the steps a logo, the alert and the coming soon screen hold before they
 * fade out */
#define LOGO_HOLD_STEPS 120
#define ALERT_HOLD_STEPS 180
#define COMING_SOON_HOLD_STEPS 466

/* a step of the fade in or out, which is TRUE once the fade is over */
/* @hack spelling: the step's result compared with TRUE, where the result
 * alone would do */
#define FADE_IN() (fade_in_step() == TRUE)
#define FADE_OUT() (fade_out_step() == TRUE)
/* the fade's color with an alpha of 0, which draws nothing */
#define FADE_CLEAR 0

/* white, as packed ARGB, opaque or at an alpha from 0 to ALPHA_MAX */
#define WHITE 0xffffffff
#define WHITE_AT(alpha) (((Uint32)(alpha) << 24) | 0xffffff)
#define ALPHA_MAX 255.0F

/* Quad vertices run top left, bottom left, top right and bottom right. */
#define QUAD_CORNERS 4
/* The picture quads tile 640 by 512 from
 * the screen's top left. the quads run a column at a time, each column from
 * its top. the columns at x 0 and 256 hold two quads 256 by 256 each. the
 * column at x 512 holds four quads 128 by 128 */
#define PICTURE_QUADS 8

/* the global indices of the textures the screens draw. a picture's index is
 * its first quad's, and each quad after it takes the next index. the alert's
 * Japanese and English textures interleave, so its quads step by
 * ALERT_TEXTURE_STEP */
#define SEGA_TEXTURE 10000000
#define SEGA_US_TEXTURE 10000126
#define SONIC_TEAM_TEXTURE 10000008
#define SOFDEC_TEXTURE 10000016
#define ALERT_TEXTURE 100000300
#define ALERT_TEXTURE_ENGLISH 100000301
#define ALERT_TEXTURE_STEP 2
#define TAIKEN_TITLE_TEXTURE 10000024
#define TAIKEN_TITLE_TEXTURE_ENGLISH 10000110
#define TAIKEN_GLOW_TEXTURE 10000100
#define TAIKEN_GLOW_TEXTURE_ENGLISH 10000118
#define TITLE_TEXTURE 2
#define PRESS_START_TEXTURE 100000310
/* the index in taiken_title_texlist of the trial's zooming logo, the
 * texture named add_logo */
#define TAIKEN_LOGO_INDEX 16

/* the alphas, from 0 to 1, of what the coming soon screen draws. ALPHA_A and
 * ALPHA_B are each a picture's, and ALPHA_C and ALPHA_D each a quad's. each
 * takes its letter from its textures' names, comminga to commingd */
#define ALPHA_A(work) ((work)->scl.x)
#define ALPHA_B(work) ((work)->scl.y)
#define ALPHA_C(work) ((work)->scl.z)
#define ALPHA_D(work) ((work)->pos.z)
/* the coming soon screen's schedule, by its task's timer. an alpha is 0
 * until the timer reaches its _RISE, and then rises toward 1. from its _FALL
 * it falls toward 0, and from an _END it is 0 again. the pictures' alphas
 * move by PICTURE_FADE_STEP a step, and ALPHA_C's and ALPHA_D's by their own
 * _STEPs */
#define ALPHA_A_RISE 60
#define ALPHA_A_FALL 120
#define ALPHA_A_END 180
#define ALPHA_B_RISE 120
#define ALPHA_B_FALL 180
#define ALPHA_B_END 240
#define PICTURE_FADE_STEP 0.025F
#define ALPHA_C_RISE 180
#define ALPHA_C_FALL 202
#define ALPHA_C_END 252
#define ALPHA_C_RISE_STEP 0.05F
#define ALPHA_C_FALL_STEP 0.02F
#define ALPHA_D_RISE 192
#define ALPHA_D_FALL 440
#define ALPHA_D_RISE_STEP 0.1F
#define ALPHA_D_FALL_STEP 0.04F
/* the coming soon screen's music resumes once its timer passes MUSIC_WAIT.
 * MUSIC_RESUMED is 1 from then on */
#define MUSIC_WAIT 180
#define MUSIC_RESUMED(work) ((work)->tick)

/* the steps into PRESS START's blink, which lasts BLINK_STEPS and repeats.
 * PRESS START draws while PRESS_START_SHOWN is TRUE, in each blink's second
 * half */
#define BLINK_TIMER(work) ((work)->tick)
#define PRESS_START_SHOWN(work) ((work)->flags)
#define BLINK_STEPS 120
/* 1.0F while the full equipment is on, and 0.0F otherwise */
#define EQUIPPED(work) ((work)->scl.y)
/* the full equipment: the game flag of every upgrade but UPGRADE_UNK_15,
 * each followed by tail, with join between */
/* clang-format off */
#define FULL_EQUIPMENT(tail, join)                                             \
    game_flags[UPGRADE_SONIC_SHOES] tail join                                  \
    game_flags[UPGRADE_SONIC_SPARKLE] tail join                                \
    game_flags[UPGRADE_SONIC_MAGICWRIST] tail join                             \
    game_flags[UPGRADE_SONIC_FLAMERING] tail join                              \
    game_flags[UPGRADE_SONIC_RUBBERUNIT] tail join                             \
    game_flags[UPGRADE_SONIC_MELODY] tail join                                 \
    game_flags[UPGRADE_SHADOW_SHOES] tail join                                 \
    game_flags[UPGRADE_SHADOW_SPARKLE] tail join                               \
    game_flags[UPGRADE_SHADOW_FLAMERING] tail join                             \
    game_flags[UPGRADE_SHADOW_MELODY] tail join                                \
    game_flags[UPGRADE_TWALKER_BOOSTER] tail join                              \
    game_flags[UPGRADE_TWALKER_HYPERCANNON] tail join                          \
    game_flags[UPGRADE_TWALKER_LAZERBALSTER] tail join                         \
    game_flags[UPGRADE_TWALKER_MELODY] tail join                               \
    game_flags[UPGRADE_EWALKER_JETENGINE] tail join                            \
    game_flags[UPGRADE_EWALKER_BAZOOKA] tail join                              \
    game_flags[UPGRADE_EWALKER_POWERGUN] tail join                             \
    game_flags[UPGRADE_EWALKER_EXTRASHIELD] tail join                          \
    game_flags[UPGRADE_EWALKER_MELODY] tail join                               \
    game_flags[UPGRADE_KNUCKLES_CLAWS] tail join                               \
    game_flags[UPGRADE_KNUCKLES_SUNGLASS] tail join                            \
    game_flags[UPGRADE_KNUCKLES_GROVES] tail join                              \
    game_flags[UPGRADE_KNUCKLES_AIR] tail join                                 \
    game_flags[UPGRADE_KNUCKLES_MELODY] tail join                              \
    game_flags[UPGRADE_ROUGE_NAILS] tail join                                  \
    game_flags[UPGRADE_ROUGE_SCOPE] tail join                                  \
    game_flags[UPGRADE_ROUGE_BOOTS] tail join                                  \
    game_flags[UPGRADE_ROUGE_MELODY] tail join                                 \
    game_flags[UPGRADE_UNK_29] tail
/* clang-format on */
/* the scale of the trial's zooming logo */
#define ZOOM(work) ((work)->scl.x)
/* the Ninja angle, 0x10000 a turn, whose sine pulses the trial title's glow.
 * each draw adds PULSE_STEP */
#define PULSE(work) ((work)->ang.x)
#define PULSE_STEP 320
/* the glow's alpha swings by GLOW_SWING about GLOW_MIDDLE, out of ALPHA_MAX */
#define GLOW_MIDDLE 128.0F
#define GLOW_SWING 127.0F
/* the depths of the trial's title and of its glow, which draws over it */
#define TAIKEN_TITLE_Z 0.5F
#define TAIKEN_GLOW_Z 0.51F

/* whether the stage splits the screen: in a two-player game, unless both
 * players ride walkers */
#define SPLITS_SCREEN()                                                        \
    (game_state.two_players != 0 && (ON_FOOT(0) || ON_FOOT(1)))
#define ON_FOOT(slot)                                                          \
    (stage_character(slot) != PLAYER_EGGMAN_MECH &&                            \
     stage_character(slot) != PLAYER_TAILS_MECH)

/* the attract demos the title screen plays in turn, title_demos' length */
#define TITLE_DEMOS 6
/* the title screen starts an attract demo once its timer passes DEMO_WAIT,
 * unless game_state.next_demo is NO_NEXT_DEMO. the trial's title waits for
 * TAIKEN_DEMO_WAIT */
#define DEMO_WAIT 900
#define TAIKEN_DEMO_WAIT 880
/* the game_state.next_demo of the trial's second and last demo, the event of
 * SEQUENCE_TAIKEN_EVENT. its first, 0, plays title_demos[0] */
#define TAIKEN_EVENT_DEMO 1

/* the trial's logo waits at the scale TAIKEN_ZOOM_START, under the black
 * fade, until the task's timer passes TAIKEN_ZOOM_WAIT. it then shrinks
 * toward 1 by TAIKEN_ZOOM_STEP a step, while the screen fades in. its alpha
 * is TAIKEN_ZOOM_START less its scale, so it appears as it shrinks. once the
 * fade in ends, the screen turns white and fades in again on the trial's
 * title */
#define TAIKEN_ZOOM_WAIT 100
#define TAIKEN_ZOOM_START 2.0F
#define TAIKEN_ZOOM_STEP 0.015F
/* where the zoom centers the trial's logo on the screen */
#define TAIKEN_LOGO_X (SCREEN_WIDTH / 2)
#define TAIKEN_LOGO_Y 187.0F

/* an attract demo: its stage, game_state.two_players and each player's
 * character */
struct title_demo {
    Sint8 stage;
    Sint8 two_players;
    Sint8 characters[2];
};

#include "title/title/text.inc"

/* Each list owns writable texture slots. The archive loader reads the slot
 * count before filling them, including when no archive is available. */
static NJS_TEXNAME logo_texnames[16] = {
#include "title/title/logo_texnames.inc"
};
static NJS_TEXLIST logo_texlist = {logo_texnames, ARRAY_COUNT(logo_texnames)};

static NJS_TEXNAME sofdec_texnames[8] = {
#include "title/title/sofdec_texnames.inc"
};
static NJS_TEXLIST sofdec_texlist = {sofdec_texnames,
                                     ARRAY_COUNT(sofdec_texnames)};

static NJS_TEXNAME taiken_title_texnames[17] = {
#include "title/title/taiken_title_texnames.inc"
};
static NJS_TEXLIST taiken_title_texlist = {taiken_title_texnames,
                                           ARRAY_COUNT(taiken_title_texnames)};

static NJS_TEXNAME alert_texnames[8] = {
#include "title/title/alert_texnames.inc"
};
static NJS_TEXLIST alert_texlist = {alert_texnames,
                                    ARRAY_COUNT(alert_texnames)};

static NJS_TEXNAME coming_soon_texnames[18] = {
#include "title/title/coming_soon_texnames.inc"
};
static NJS_TEXLIST coming_soon_texlist = {coming_soon_texnames,
                                          ARRAY_COUNT(coming_soon_texnames)};

static NJS_TEXNAME anniversary_texnames[1] = {
#include "title/title/anniversary_texnames.inc"
};
static NJS_TEXLIST anniversary_texlist = {anniversary_texnames,
                                          ARRAY_COUNT(anniversary_texnames)};

static NJS_TEXTURE_VTX title_quad[QUAD_CORNERS] = {
#include "title/title/title_quad.inc"
};
static NJS_TEXTURE_VTX press_start_quad[QUAD_CORNERS] = {
#include "title/title/press_start_quad.inc"
};

static NJS_TEXNAME title_texnames[1] = {
#include "title/title/title_texnames.inc"
};
static NJS_TEXLIST title_texlist = {title_texnames,
                                    ARRAY_COUNT(title_texnames)};

static NJS_TEXNAME press_start_texnames[1] = {
#include "title/title/press_start_texnames.inc"
};
static NJS_TEXLIST press_start_texlist = {press_start_texnames,
                                          ARRAY_COUNT(press_start_texnames)};

/* the test menu's characters, and the names it shows them by */
static Sint32 menu_characters[] = {
    PLAYER_SONIC, PLAYER_SHADOW, PLAYER_KNUCKLES,   PLAYER_ROUGE,
    PLAYER_TAILS, PLAYER_EGGMAN, PLAYER_TAILS_MECH, PLAYER_EGGMAN_MECH,
};

static char *menu_character_names[] = {
    "SONIC", "SHADOW", "KNUCKLES", "ROUGE",
    "TAILS", "EGGMAN", "T WALKER", "E WALKER",
};
#define MENU_CHARACTERS ARRAY_COUNT(menu_characters)

/* the test menu's stages, and the names it shows them by */
static Sint32 menu_stages[] = {
    STAGE_BASIC_TEST,        STAGE_SONIC_TEST,     STAGE_KNUCKLES_TEST,
    STAGE_GREEN_FOREST,      STAGE_WHITE_JUNGLE,   STAGE_PUMPKIN_HILL,
    STAGE_SKY_RAIL,          STAGE_AQUATIC_MINE,   STAGE_SECURITY_HALL,
    STAGE_PRISON_LANE,       STAGE_METAL_HARBOR,   STAGE_IRON_GATE,
    STAGE_WEAPONS_BED,       STAGE_CITY_ESCAPE,    STAGE_RADICAL_HIGHWAY,
    STAGE_WEAPONS_BED_2P,    STAGE_WILD_CANYON,    STAGE_MISSION_STREET,
    STAGE_DRY_LAGOON,        STAGE_VERSUS_SONIC_1, STAGE_VERSUS_MECH_1,
    STAGE_SAND_OCEAN,        STAGE_CRAZY_GADGET,   STAGE_HIDDEN_BASE,
    STAGE_ETERNAL_ENGINE,    STAGE_DEATH_CHAMBER,  STAGE_EGG_QUARTERS,
    STAGE_LOST_COLONY,       STAGE_PYRAMID_CAVE,   STAGE_VERSUS_MECH_2,
    STAGE_FINAL_RUSH,        STAGE_GREEN_HILL,     STAGE_METEOR_HERD,
    STAGE_VERSUS_KNUCKLES,   STAGE_RELAY_SONIC,    STAGE_RELAY_EGGMAN,
    STAGE_RELAY_TAILS,       STAGE_RELAY_ROUGE,    STAGE_RELAY_KNUCKLES,
    STAGE_MISSION_STREET_2P, STAGE_FINAL_CHASE,    STAGE_WILD_CANYON_2P,
    STAGE_VERSUS_SONIC_2,    STAGE_COSMIC_WALL,    STAGE_MAD_SPACE,
    STAGE_SAND_OCEAN_2P,     STAGE_CART_1,         STAGE_CART_2,
    STAGE_BOSS_BIGFOOT,      STAGE_BOSS_HOTSHOT,   STAGE_BOSS_FDOG,
    STAGE_BOSS_BIGBOGY,      STAGE_BOSS_GOLEM,     STAGE_BOSS_GOLEM_E,
    STAGE_BOSS_LAST1,        STAGE_BOSS_LAST2,     STAGE_CHAO,
};

static char *menu_stage_names[] = {
    "BASIC TEST",
    "SONIC TEST",
    "KNUCKLES TEST",
    "GREENFOREST(SONIC & 2P)",
    "WHITEJUNGLE(SHADOW)",
    "PUMPKINHILL(KNUCKLES & 2P)",
    "SKYRAIL(SHADOW & 2P)",
    "AQUATICMINE(KNUCKLES)",
    "SECURITYHALL(ROUGE)",
    "PRISONLANE(T WALKER)",
    "METALHARBOR(SONIC)",
    "IRONGATE(E WALKER)",
    "WEAPONSBED(E WALKER)",
    "CITYESCAPE(SONIC)",
    "RADICALHIGHWAY(SHADOW & 2P)",
    "WEAPONSBED(WALKER 2P)",
    "WILD CANYON(KNUCKLES 1P)",
    "MISSIONSTREET(T WALKER)",
    "DRY LAGOON(ROUGE)",
    "SONIC VS SHADOW",
    "EGGMAN VS TAILS",
    "SANDOCEAN(E WALKER)",
    "CRAZY GADGET(SONIC)",
    "HIDDEN BASE(T WALKER)",
    "ETERNAL ENGINE(T WALKER)",
    "DEATH CHAMBER(KNUCKLES)",
    "EGG QUARTER(ROUGE)",
    "LOST COLONY(E WALKER)",
    "PYRAMID CAVE(SONIC)",
    "EGGMAN VS TAILS 2",
    "FINAL RUSH(SONIC)",
    "?????",
    "METEOR HERD(KNUCKLES)",
    "KNUCKLES VS ROUGE",
    "CANNON'S CORE(SONIC)",
    "CANNON'S CORE(E WALKER)",
    "CANNON'S CORE(T WALKER)",
    "CANNON'S CORE(ROUGE)",
    "CANNON'S CORE(KNUCKLES)",
    "MISSION STREET(WALKER 2P)",
    "FINAL CHASE(SHADOW)",
    "WILD CANYON(KNUCKLES 2P)",
    "SONIC VS SHADOW 2",
    "COSMIC WALL(E WALKER)",
    "MAD SPACE(ROUGE)",
    "SAND OCEAN(WALKER 2P)",
    "CART(TAILS or ROUGE)",
    "CART MINI",
    "BOSS BIGFOOT(SONIC)",
    "BOSS HOTSHOT(SHADOW)",
    "BOSS FLYING DOG(ROUGE)",
    "BOSS GHOST(KNUCKLES)",
    "BOSS GOLEM(SONIC)",
    "BOSS GOLEM E(E WALKER)",
    "LASTBOSS1(SHADOW)",
    "LASTBOSS2(SUPERSONIC & SUPERSHADOW)",
    "CHAO",
};
#define MENU_STAGES ARRAY_COUNT(menu_stages)

/* the attract demos, by game_state.next_demo. the trial's title plays only
 * the first */
static struct title_demo title_demos[TITLE_DEMOS] = {
    {STAGE_CITY_ESCAPE, 0, {PLAYER_SONIC, PLAYER_SONIC}},
    {STAGE_IRON_GATE, 0, {PLAYER_EGGMAN_MECH, PLAYER_SONIC}},
    {STAGE_DRY_LAGOON, 0, {PLAYER_ROUGE, PLAYER_SONIC}},
    {STAGE_PRISON_LANE, 0, {PLAYER_TAILS_MECH, PLAYER_SONIC}},
    {STAGE_PUMPKIN_HILL, 0, {PLAYER_KNUCKLES, PLAYER_SONIC}},
    {STAGE_SKY_RAIL, 0, {PLAYER_SHADOW, PLAYER_SONIC}},
};

static Sint8 pal_frames = 0;
/* what title_step returns. a screen's task sets OVERLAY_DONE once its fade
 * out ends, and title_step sets OVERLAY_RESET on a reset. title_step sets
 * OVERLAY_RUNNING again as it goes on to the next screen */
static enum overlay_result title_result = OVERLAY_RUNNING;
static enum phase title_phase = PHASE_SEGA_START;

static NJS_TEXTURE_VTX sega_quads[PICTURE_QUADS * QUAD_CORNERS] = {
#include "title/title/sega_quads.inc"
};
static NJS_TEXTURE_VTX sonic_team_quads[PICTURE_QUADS * QUAD_CORNERS] = {
#include "title/title/sonic_team_quads.inc"
};
static NJS_TEXTURE_VTX sofdec_quads[PICTURE_QUADS * QUAD_CORNERS] = {
#include "title/title/sofdec_quads.inc"
};
static NJS_TEXTURE_VTX alert_quads[PICTURE_QUADS * QUAD_CORNERS] = {
#include "title/title/alert_quads.inc"
};

static NJS_POLYGON_VTX coming_soon_backdrop[QUAD_CORNERS] = {
#include "title/title/coming_soon_backdrop.inc"
};

static NJS_TEXTURE_VTX
    coming_soon_quads[((2 * PICTURE_QUADS) + 2) * QUAD_CORNERS] = {
#include "title/title/coming_soon_quads.inc"
};

static NJS_TEXTURE_VTX anniversary_quad[QUAD_CORNERS] = {
#include "title/title/anniversary_quad.inc"
};

static char *mission_names[MISSIONS] = {
    "1ST.", "2ND.", "3RD.", "4TH.", "LAST.",
};

static NJS_TEXTURE_VTX taiken_title_quads[PICTURE_QUADS * QUAD_CORNERS] = {
#include "title/title/taiken_title_quads.inc"
};

static NJS_TEXTURE_VTX taiken_logo_quad[QUAD_CORNERS] = {
#include "title/title/taiken_logo_quad.inc"
};

/* the trial logo's corners about its center, at the scale 1 */
static NJS_POINT3 taiken_logo_corners[QUAD_CORNERS] = {
#include "title/title/taiken_logo_corners.inc"
};

static void sega_start(void);
static void sega_step(struct task *task);
static void sega_draw(struct task *task);
static void sonic_team_start(void);
static void sonic_team_step(struct task *task);
static void sonic_team_draw(struct task *task);
static void sofdec_start(void);
static void sofdec_step(struct task *task);
static void sofdec_draw(struct task *task);
static void alert_start(void);
static void alert_step(struct task *task);
static void alert_draw(struct task *task);
static void coming_soon_start(void);
static void coming_soon_step(struct task *task);
static void coming_soon_draw(struct task *task);
static void anniversary_start(void);
static void anniversary_step(struct task *task);
static void anniversary_draw(struct task *task);
static void title_screen_start(void);
static void title_screen_end(void);
static void title_screen_step(struct task *task);
static void title_screen_draw(struct task *task);
static void title_screen_draw_taiken(struct task *task);

void title_init(void)
{
    title_result = OVERLAY_RUNNING;
    frame_pacing_init(FRAME_BLANKS, FRAME_STEPS);
    task_init();
    sound_load_player_sounds();
    game_state.four_cameras = 0;
    game_state.single_view = 0;
    switch (game_state.edition) {
    /* @hack redundant: a case 0 that does what the default does */
    case 0:
        title_phase = PHASE_TITLE_START;
        break;
    case EDITION_TAIKEN:
        if (game_state.card_warning == CARD_WARNING_ALERT) {
            title_phase = PHASE_ALERT_START;
        } else if (game_state.trial_screen == TRIAL_SCREEN_TITLE) {
            title_phase = PHASE_TITLE_START;
        } else if (game_state.trial_screen == TRIAL_SCREEN_COMING_SOON) {
            title_phase = PHASE_COMING_SOON_START;
        } else if (!(game_state.next_demo & 1)) {
            title_phase = PHASE_SEGA_START;
        } else {
            title_phase = PHASE_TITLE_START;
        }
        game_state.trial_screen = 0;
        break;
    default:
        title_phase = PHASE_TITLE_START;
        break;
    }
    item_stock_clear_all();
}

void title_exit(void)
{
}

/* a run phase of the screen whose textures are texlist's. a reset releases
 * them and destroys every task, and leaves title_result OVERLAY_RESET. once
 * the screen's task sets OVERLAY_DONE, the phase does the same. it then goes
 * on to the next phase, with title_result OVERLAY_RUNNING */
#define RUN_SCREEN(texlist)                                                    \
    RUN_TASKS();                                                               \
    if (pad_reset_pressed(0)) {                                                \
        title_result = OVERLAY_RESET;                                          \
        njReleaseTexture(texlist);                                             \
        task_destroy_all();                                                    \
    } else if (title_result != OVERLAY_RUNNING) {                              \
        njReleaseTexture(texlist);                                             \
        task_destroy_all();                                                    \
        title_phase++;                                                         \
        title_result = OVERLAY_RUNNING;                                        \
    }
/* RUN_SCREEN, but a reset goes on to the trial's title, with title_result
 * OVERLAY_RUNNING */
#define RUN_SCREEN_TO_TITLE(texlist)                                           \
    RUN_TASKS();                                                               \
    if (pad_reset_pressed(0)) {                                                \
        njReleaseTexture(texlist);                                             \
        task_destroy_all();                                                    \
        title_result = OVERLAY_RUNNING;                                        \
        title_phase = PHASE_TITLE_START;                                       \
    } else if (title_result != OVERLAY_RUNNING) {                              \
        njReleaseTexture(texlist);                                             \
        task_destroy_all();                                                    \
        title_phase++;                                                         \
        title_result = OVERLAY_RUNNING;                                        \
    }

int title_step(void)
{
    switch (title_phase) {
    case PHASE_SEGA_START:
        sega_start();
        title_phase++;
        pal_frames = 0;
        break;
    case PHASE_SEGA:
        RUN_SCREEN(&logo_texlist);
        break;
    case PHASE_SEGA_DONE:
        title_phase = PHASE_SONIC_TEAM_START;
        break;
    case PHASE_SONIC_TEAM_START:
        sonic_team_start();
        title_phase++;
        pal_frames = 0;
        break;
    case PHASE_SONIC_TEAM:
        RUN_SCREEN(&logo_texlist);
        break;
    case PHASE_SONIC_TEAM_DONE:
        if (game_state.card_warning == CARD_WARNING_ALERT) {
            title_phase = PHASE_SOFDEC_START;
            game_state.card_warning = 0;
        } else {
            title_phase = PHASE_TITLE_START;
        }
        break;
    case PHASE_SOFDEC_START:
        sofdec_start();
        title_phase++;
        pal_frames = 0;
        break;
    case PHASE_SOFDEC:
        RUN_SCREEN(&sofdec_texlist);
        break;
    case PHASE_SOFDEC_DONE:
        title_phase = PHASE_TITLE_START;
        break;
    case PHASE_ALERT_START:
        alert_start();
        title_phase++;
        pal_frames = 0;
        break;
    case PHASE_ALERT:
        RUN_SCREEN(&alert_texlist);
        break;
    case PHASE_ALERT_DONE:
        title_phase = PHASE_SEGA_START;
        break;
    case PHASE_COMING_SOON_START:
        coming_soon_start();
        title_phase++;
        pal_frames = 0;
        break;
    case PHASE_COMING_SOON:
        RUN_SCREEN_TO_TITLE(&coming_soon_texlist);
        break;
    case PHASE_COMING_SOON_DONE:
        title_phase = PHASE_ANNIVERSARY_START;
        break;
    case PHASE_ANNIVERSARY_START:
        anniversary_start();
        title_phase++;
        pal_frames = 0;
        break;
    case PHASE_ANNIVERSARY:
        RUN_SCREEN_TO_TITLE(&anniversary_texlist);
        break;
    case PHASE_ANNIVERSARY_DONE:
        title_phase = PHASE_SEGA_START;
        break;
    case PHASE_TITLE_START:
        title_screen_start();
        title_phase++;
        pal_frames = 0;
        break;
    case PHASE_TITLE:
        RUN_TASKS();
        /* @bug logic: a reset ends the screen twice. the reset sets
         * title_result to OVERLAY_RESET, so the test of title_result after it
         * holds too. title_screen_end then releases the textures and destroys
         * every task again (calls at 0x8c50053a and 0x8c500546). every other
         * run phase tests title_result in an else */
        if (pad_reset_pressed(0)) {
            title_result = OVERLAY_RESET;
            title_screen_end();
        }
        if (title_result != OVERLAY_RUNNING) {
            title_screen_end();
            title_phase++;
        }
        break;
    /* @hack redundant: a case that does nothing */
    case PHASE_TITLE_DONE:
        break;
    }
    return title_result;
}

/* create the screen's task with step and drawer, under name, into the
 * caller's task. then set the fade black and enable the pads */
/* @hack spelling: the assignment inside the test, where a statement of its
 * own would do */
#define START_SCREEN(step, drawer, name)                                       \
    if ((task = task_create(TASK_WORK, TASK_LEVEL, step, name)) != NULL) {     \
        task->draw = drawer;                                                   \
    }                                                                          \
    fade_set_black();                                                          \
    pad_enable()

/* the step of task, a logo's or the alert's, whose hold lasts hold steps. its
 * fade in ends at once on SKIP_BUTTONS. it sets title_result to OVERLAY_DONE
 * once its fade out ends */
#define LOGO_STEP(hold)                                                        \
    struct task_work *work = task->work;                                       \
                                                                               \
    switch (work->mode) {                                                      \
    case MODE_LOGO_IN:                                                         \
        if (FADE_IN()) {                                                       \
            work->mode = MODE_LOGO_HOLD;                                       \
        } else if (pad_press_repeat[0] & SKIP_BUTTONS) {                       \
            fade_set_color(FADE_CLEAR);                                        \
            work->mode = MODE_LOGO_HOLD;                                       \
        }                                                                      \
        break;                                                                 \
    case MODE_LOGO_HOLD:                                                       \
        work->timer++;                                                         \
        if (work->timer >= (hold)) {                                           \
            work->mode = MODE_LOGO_OUT;                                        \
            fade_reset_idle();                                                 \
            pad_disable();                                                     \
        }                                                                      \
        break;                                                                 \
    case MODE_LOGO_OUT:                                                        \
        if (FADE_OUT()) {                                                      \
            work->mode = MODE_LOGO_DONE;                                       \
            title_result = OVERLAY_DONE;                                       \
        }                                                                      \
        break;                                                                 \
    }

static void sega_start(void)
{
    struct task *task;

    if (game_state.us_sega_logo == 0) {
        texture_load_pvm("LOGOTEX", &logo_texlist);
    } else {
        texture_load_pvm("LOGOTEXE", &logo_texlist);
    }
    START_SCREEN(sega_step, sega_draw, "DisplaySegalogo");
}

static void sega_step(struct task *task)
{
    LOGO_STEP(LOGO_HOLD_STEPS);
}

static void sega_draw(struct task *task)
{
    NJS_TEXTURE_VTX *quad = sega_quads;
    Int texture;
    int i;

    njSetTexture(&logo_texlist);
    if (game_state.us_sega_logo == 0) {
        texture = SEGA_TEXTURE;
    } else {
        texture = SEGA_US_TEXTURE;
    }
    for (i = 0; i < PICTURE_QUADS; i++) {
        njDrawTexture(quad, QUAD_CORNERS, texture, FALSE);
        quad += QUAD_CORNERS;
        texture++;
    }
}

static void sonic_team_start(void)
{
    struct task *task;

    texture_load_pvm("LOGOTEX", &logo_texlist);
    START_SCREEN(sonic_team_step, sonic_team_draw, "DisplaySonicteamlogo");
}

static void sonic_team_step(struct task *task)
{
    LOGO_STEP(LOGO_HOLD_STEPS);
}

static void sonic_team_draw(struct task *task)
{
    NJS_TEXTURE_VTX *quad = sonic_team_quads;
    Int texture = SONIC_TEAM_TEXTURE;
    int i;

    njSetTexture(&logo_texlist);
    for (i = 0; i < PICTURE_QUADS; i++) {
        njDrawTexture(quad, QUAD_CORNERS, texture, FALSE);
        /* @hack spelling: quad = quad + QUAD_CORNERS, where += would do */
        quad = quad + QUAD_CORNERS;
        texture++;
    }
}

static void sofdec_start(void)
{
    struct task *task;

    texture_load_pvm("LOGO2TEX", &sofdec_texlist);
    START_SCREEN(sofdec_step, sofdec_draw, "DisplayAdxSofdeclogo");
}

static void sofdec_step(struct task *task)
{
    LOGO_STEP(LOGO_HOLD_STEPS);
}

static void sofdec_draw(struct task *task)
{
    NJS_TEXTURE_VTX *quad = sofdec_quads;
    Int texture = SOFDEC_TEXTURE;
    int i;

    njSetTexture(&sofdec_texlist);
    for (i = 0; i < PICTURE_QUADS; i++) {
        njDrawTexture(quad, QUAD_CORNERS, texture, FALSE);
        /* @hack spelling: quad = quad + QUAD_CORNERS, where += would do */
        quad = quad + QUAD_CORNERS;
        texture++;
    }
}

static void alert_start(void)
{
    struct task *task;

    if (game_state.text_language == TEXT_JAPANESE) {
        texture_load_pvm("ALERTPSOTEX", &alert_texlist);
    } else {
        texture_load_pvm("ALERTPSOTEXE", &alert_texlist);
    }
    START_SCREEN(alert_step, alert_draw, "DisplayTaikenAlert");
}

static void alert_step(struct task *task)
{
    LOGO_STEP(ALERT_HOLD_STEPS);
}

static void alert_draw(struct task *task)
{
    NJS_TEXTURE_VTX *quad = alert_quads;
    Int texture;
    int i;

    njSetTexture(&alert_texlist);
    if (game_state.text_language == TEXT_JAPANESE) {
        texture = ALERT_TEXTURE;
    } else {
        texture = ALERT_TEXTURE_ENGLISH;
    }
    for (i = 0; i < PICTURE_QUADS; i++) {
        njDrawTexture(quad, QUAD_CORNERS, texture, FALSE);
        quad += QUAD_CORNERS;
        texture += ALERT_TEXTURE_STEP;
    }
}

static void coming_soon_start(void)
{
    struct task *task;

    texture_load_pvm("COMINGPSOTEX", &coming_soon_texlist);
    START_SCREEN(coming_soon_step, coming_soon_draw, "DisplayTaikenComingSoon");
    sound_task_create(NULL);
    music_fade_in(0);
}

static void coming_soon_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case MODE_LOGO_IN:
        fade_set_color(FADE_CLEAR);
        work->mode = MODE_LOGO_HOLD;
        music_play_name("coming.adx");
        music_pause();
        break;
    case MODE_LOGO_HOLD:
        work->timer++;
        if (work->timer < ALPHA_A_RISE) {
            ALPHA_A(work) = 0.0F;
        } else if (work->timer < ALPHA_A_FALL) {
            ALPHA_A(work) =
                float_approach(ALPHA_A(work), 1.0F, PICTURE_FADE_STEP);
        } else if (work->timer < ALPHA_A_END) {
            ALPHA_A(work) =
                float_approach(ALPHA_A(work), 0.0F, PICTURE_FADE_STEP);
        } else {
            ALPHA_A(work) = 0.0F;
        }
        if (work->timer < ALPHA_B_RISE) {
            ALPHA_B(work) = 0.0F;
        } else if (work->timer < ALPHA_B_FALL) {
            ALPHA_B(work) =
                float_approach(ALPHA_B(work), 1.0F, PICTURE_FADE_STEP);
        } else if (work->timer < ALPHA_B_END) {
            ALPHA_B(work) =
                float_approach(ALPHA_B(work), 0.0F, PICTURE_FADE_STEP);
        } else {
            ALPHA_B(work) = 0.0F;
        }
        if (work->timer > MUSIC_WAIT && MUSIC_RESUMED(work) == 0) {
            music_resume();
            MUSIC_RESUMED(work) = 1;
        }
        if (work->timer < ALPHA_C_RISE) {
            ALPHA_C(work) = 0.0F;
        } else if (work->timer < ALPHA_C_FALL) {
            ALPHA_C(work) =
                float_approach(ALPHA_C(work), 1.0F, ALPHA_C_RISE_STEP);
        } else if (work->timer < ALPHA_C_END) {
            ALPHA_C(work) =
                float_approach(ALPHA_C(work), 0.0F, ALPHA_C_FALL_STEP);
        } else {
            ALPHA_C(work) = 0.0F;
        }
        if (work->timer < ALPHA_D_RISE) {
            ALPHA_D(work) = 0.0F;
        } else if (work->timer < ALPHA_D_FALL) {
            ALPHA_D(work) =
                float_approach(ALPHA_D(work), 1.0F, ALPHA_D_RISE_STEP);
        } else {
            ALPHA_D(work) =
                float_approach(ALPHA_D(work), 0.0F, ALPHA_D_FALL_STEP);
        }
        if (work->timer >= COMING_SOON_HOLD_STEPS) {
            work->mode = MODE_LOGO_OUT;
            fade_reset_idle();
            pad_disable();
        }
        break;
    case MODE_LOGO_OUT:
        if (FADE_OUT()) {
            work->mode = MODE_LOGO_DONE;
            title_result = OVERLAY_DONE;
        }
        break;
    }
}

/* draw the caller's quad with its texture, in white at alpha from 0 to 1. an
 * alpha of 0 draws nothing. either way, step the caller's quad and texture on
 * to the next quad's */
#define DRAW_AT_ALPHA(alpha)                                                   \
    if ((alpha) > 0.0F) {                                                      \
        quad[0].col = WHITE_AT((alpha) * ALPHA_MAX);                           \
        quad[1].col = quad[2].col = quad[3].col = quad[0].col;                 \
        njDrawTexture(quad, QUAD_CORNERS, texture, TRUE);                      \
    }                                                                          \
    quad += QUAD_CORNERS;                                                      \
    texture++

static void coming_soon_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_TEXTURE_VTX *quad = coming_soon_quads;
    Int texture;
    int i;

    njSetTexture(&coming_soon_texlist);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njDrawPolygon(coming_soon_backdrop, QUAD_CORNERS, FALSE);
    njSetTextureNum(0);
    texture = njGetTextureNumG();
    for (i = 0; i < PICTURE_QUADS; i++) {
        DRAW_AT_ALPHA(ALPHA_A(work));
    }
    for (i = 0; i < PICTURE_QUADS; i++) {
        DRAW_AT_ALPHA(ALPHA_B(work));
    }
    DRAW_AT_ALPHA(ALPHA_C(work));
    DRAW_AT_ALPHA(ALPHA_D(work));
}

static void anniversary_start(void)
{
    struct task *task;

    texture_load_pvm("ANNIVTEX", &anniversary_texlist);
    START_SCREEN(anniversary_step, anniversary_draw,
                 "Display10thAnniversarylogo");
}

static void anniversary_step(struct task *task)
{
    LOGO_STEP(LOGO_HOLD_STEPS);
}

static void anniversary_draw(struct task *task)
{
    NJS_TEXTURE_VTX *quad = anniversary_quad;
    /* @hack type: texture a Uint32, where the Int that njDrawTexture takes
     * would do */
    Uint32 texture;

    njSetTexture(&anniversary_texlist);
    njSetTextureNum(0);
    texture = njGetTextureNumG();
    njDrawTexture(quad, QUAD_CORNERS, texture, FALSE);
}

static void title_screen_start(void)
{
    struct task *task;

    if (game_state.edition == EDITION_TAIKEN) {
        if (game_state.text_language == TEXT_JAPANESE) {
            texture_load_pvm("LOGOPSOTEX", &taiken_title_texlist);
        } else {
            texture_load_pvm("LOGOPSOTEXE", &taiken_title_texlist);
        }
    } else {
        njLoadTexture(&title_texlist);
    }
    texture_load_pvm("PRESSSTART", &press_start_texlist);
    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((task = task_create(TASK_WORK, TASK_LEVEL, title_screen_step,
                            "DisplayTitle")) != NULL) {
        if (game_state.edition == EDITION_TAIKEN) {
            task->draw = title_screen_draw_taiken;
            task->work->mode = MODE_TAIKEN_ZOOM;
        } else {
            task->draw = title_screen_draw;
        }
        if (FULL_EQUIPMENT(!= 0, &&)) {
            EQUIPPED(task->work) = 1.0F;
        }
    }
    fade_set_black();
    sound_task_create(NULL);
    music_fade_in(0);
    music_play_name("title.adx");
    game_state.demo = 0;
    lives_reset();
}

static void title_screen_end(void)
{
    if (game_state.edition == EDITION_TAIKEN) {
        njReleaseTexture(&taiken_title_texlist);
    } else {
        njReleaseTexture(&title_texlist);
    }
    njReleaseTexture(&press_start_texlist);
    task_destroy_all();
}

static void title_screen_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: choose, a local for the address of pad_choose_buttons,
     * where pad_choose_buttons read in place would do */
    Uint32 *choose = &pad_choose_buttons;
    /* @hack spelling: previous_buttons and next_buttons, locals for the two
     * masks, where the masks written in place would do */
    const Uint32 previous_buttons = PDD_DGT_KU | PDD_DGT_KL;
    const Uint32 next_buttons = PDD_DGT_KD | PDD_DGT_KR;
    /* @hack spelling: one, a local for 1.0F, where 1.0F written in place would
     * do */
    const Float one = 1.0F;

    switch (work->mode) {
    case MODE_TITLE_IN:
        if (FADE_IN()) {
            work->mode = MODE_TITLE;
            pad_enable();
        }
        break;
    case MODE_TITLE:
        BLINK_TIMER(work)++;
        work->timer++;
        if (BLINK_TIMER(work) >= BLINK_STEPS) {
            BLINK_TIMER(work) = 0;
        }
        if (BLINK_TIMER(work) <= BLINK_STEPS / 2) {
            PRESS_START_SHOWN(work) = FALSE;
        } else {
            PRESS_START_SHOWN(work) = TRUE;
        }
        if (pad_press_repeat[0] & PDD_DGT_ST) {
            work->mode = MODE_TEST_MENU;
            PRESS_START_SHOWN(work) = FALSE;
            work->submode = 0;
        }
        if (work->timer > DEMO_WAIT && game_state.next_demo != NO_NEXT_DEMO) {
            work->mode = MODE_TITLE_OUT;
            fade_reset_idle();
            pad_disable();
            stage_set_current(title_demos[game_state.next_demo].stage);
            game_state.two_players =
                title_demos[game_state.next_demo].two_players;
            stage_set_character(
                0, title_demos[game_state.next_demo].characters[0]);
            stage_set_character(
                1, title_demos[game_state.next_demo].characters[1]);
            if (SPLITS_SCREEN()) {
                game_state.split_screen = 1;
            } else {
                game_state.split_screen = 0;
            }
            game_state.next_mode = GAME_MODE_DEMO;
            game_state.demo = DEMO_ON;
            if (++game_state.next_demo >= TITLE_DEMOS) {
                game_state.next_demo = 0;
            }
        }
        break;
    case MODE_TEST_MENU:
        if (pad_press_repeat[0] & next_buttons) {
            work->submode++;
        }
        if (pad_press_repeat[0] & previous_buttons) {
            work->submode--;
        }
        if (work->submode < 0) {
            work->submode = MENU_ENTRIES - 1;
        }
        if (work->submode >= MENU_ENTRIES) {
            work->submode = 0;
        }
        switch (work->submode) {
        case MENU_GAME_1P:
            njPrintC(MENU_LABEL_AT, "GAME:");
            njPrintC(MENU_ENTRY_AT, "1P ONLY");
            if (pad_press_repeat[0] & *choose) {
                work->mode = MODE_PLAYER_1;
                game_state.two_players = 0;
                work->submode = 0;
                game_state.next_mode = GAME_MODE_STAGE;
            }
            break;
        case MENU_GAME_2P_3_POINTS:
            njPrintC(MENU_LABEL_AT, "GAME:");
            njPrintC(MENU_ENTRY_AT, "2P 3POINTS MATCH");
            if (pad_press_repeat[0] & *choose) {
                work->mode = MODE_PLAYER_1;
                game_state.two_players = TWO_PLAYERS_3_POINTS;
                work->submode = 0;
                game_state.next_mode = GAME_MODE_STAGE;
            }
            break;
        case MENU_GAME_2P_1_POINT:
            njPrintC(MENU_LABEL_AT, "GAME:");
            njPrintC(MENU_ENTRY_AT, "2P 1POINT MATCH");
            if (pad_press_repeat[0] & *choose) {
                work->mode = MODE_PLAYER_1;
                game_state.two_players = TWO_PLAYERS_1_POINT;
                work->submode = 0;
                game_state.next_mode = GAME_MODE_STAGE;
            }
            break;
        case MENU_TEST_EVENT:
            njPrintC(MENU_LABEL_AT, "TEST:");
            njPrintC(MENU_ENTRY_AT, "EVENT");
            if (pad_press_repeat[0] & *choose) {
                work->mode = MODE_TITLE_OUT;
                fade_reset_idle();
                pad_disable();
                game_state.next_mode = GAME_MODE_EVENT;
            }
            break;
        case MENU_DEMO_1P:
            njPrintC(MENU_LABEL_AT, "DEMO:");
            njPrintC(MENU_ENTRY_AT, "1P ONLY");
            if (pad_press_repeat[0] & *choose) {
                work->mode = MODE_PLAYER_1;
                game_state.two_players = 0;
                work->submode = 0;
                game_state.next_mode = GAME_MODE_DEMO;
                game_state.demo = DEMO_RECORD;
            }
            break;
        case MENU_DEMO_2P:
            njPrintC(MENU_LABEL_AT, "DEMO:");
            njPrintC(MENU_ENTRY_AT, "2P");
            if (pad_press_repeat[0] & *choose) {
                work->mode = MODE_PLAYER_2;
                game_state.two_players = TWO_PLAYERS_3_POINTS;
                work->submode = 0;
                game_state.next_mode = GAME_MODE_DEMO;
                game_state.demo = DEMO_RECORD;
            }
            break;
        case MENU_DEMO_PREVIOUS:
            njPrintC(MENU_LABEL_AT, "DEMO:");
            njPrintC(MENU_ENTRY_AT, "LOOK PREVIOUS TAKEN ONE");
            if (pad_press_repeat[0] & *choose) {
                work->mode = MODE_TITLE_OUT;
                fade_reset_idle();
                pad_disable();
                game_state.next_mode = GAME_MODE_DEMO;
                game_state.demo = DEMO_ON_NO_FILE;
            }
            break;
        case MENU_PLAY_HERO:
            njPrintC(MENU_LABEL_AT, "PLAY:");
            njPrintC(MENU_ENTRY_AT, "FROM HERO");
            if (pad_press_repeat[0] & *choose) {
                sequence_start(SEQUENCE_HERO);
                work->mode = MODE_TITLE_OUT;
                fade_reset_idle();
                pad_disable();
                game_state.next_mode = GAME_MODE_SEQUENCE;
            }
            break;
        case MENU_PLAY_DARK:
            njPrintC(MENU_LABEL_AT, "PLAY:");
            njPrintC(MENU_ENTRY_AT, "FROM DARK");
            if (pad_press_repeat[0] & *choose) {
                sequence_start(SEQUENCE_DARK);
                work->mode = MODE_TITLE_OUT;
                fade_reset_idle();
                pad_disable();
                game_state.next_mode = GAME_MODE_SEQUENCE;
            }
            break;
        case MENU_PLAY_LAST:
            njPrintC(MENU_LABEL_AT, "PLAY:");
            njPrintC(MENU_ENTRY_AT, "FROM LAST");
            if (pad_press_repeat[0] & *choose) {
                sequence_start(SEQUENCE_LAST);
                work->mode = MODE_TITLE_OUT;
                fade_reset_idle();
                pad_disable();
                game_state.next_mode = GAME_MODE_SEQUENCE;
            }
            break;
        }
        break;
    case MODE_PLAYER_1:
        if (pad_press_repeat[0] & next_buttons) {
            work->submode++;
        }
        if (pad_press_repeat[0] & previous_buttons) {
            work->submode--;
        }
        if (work->submode < 0) {
            work->submode = MENU_CHARACTERS - 1;
        }
        if ((Uint32)work->submode >= MENU_CHARACTERS) {
            work->submode = 0;
        }
        njPrintC(MENU_LABEL_AT, "1P:");
        njPrintC(MENU_CHARACTER_AT, menu_character_names[work->submode]);
        if (pad_press_repeat[0] & *choose) {
            stage_set_character(0, menu_characters[work->submode]);
            if (game_state.two_players) {
                work->mode = MODE_PLAYER_2;
                work->submode++;
            } else {
                work->mode = MODE_STAGE_MENU;
                work->submode = 0;
            }
        } else if (pad_press_repeat[0] & pad_cancel_buttons) {
            work->mode = MODE_TEST_MENU;
            PRESS_START_SHOWN(work) = FALSE;
            work->submode = 0;
        }
        break;
    case MODE_PLAYER_2:
        if (pad_press_repeat[0] & next_buttons) {
            work->submode++;
        }
        if (pad_press_repeat[0] & previous_buttons) {
            work->submode--;
        }
        if (work->submode < 0) {
            work->submode = MENU_CHARACTERS - 1;
        }
        if ((Uint32)work->submode >= MENU_CHARACTERS) {
            work->submode = 0;
        }
        njPrintC(MENU_LABEL_AT, "2P:");
        njPrintC(MENU_CHARACTER_AT, menu_character_names[work->submode]);
        if (pad_press_repeat[0] & *choose) {
            work->mode = MODE_STAGE_MENU;
            stage_set_character(1, menu_characters[work->submode]);
            work->submode = 0;
        } else if (pad_press_repeat[0] & pad_cancel_buttons) {
            work->mode = MODE_PLAYER_1;
            PRESS_START_SHOWN(work) = FALSE;
            work->submode = 0;
        }
        break;
    case MODE_STAGE_MENU:
        if (pad_press_repeat[0] & next_buttons) {
            work->submode++;
        }
        if (pad_press_repeat[0] & previous_buttons) {
            work->submode--;
        }
        if (work->submode < 0) {
            work->submode = MENU_STAGES - 1;
        }
        if ((Uint32)work->submode >= MENU_STAGES) {
            work->submode = 0;
        }
        njPrintC(MENU_STAGE_LABEL_AT, "STAGE:");
        njPrintC(MENU_STAGE_AT, menu_stage_names[work->submode]);
        if (pad_press_repeat[0] & *choose) {
            work->mode = MODE_TITLE_OUT;
            fade_reset_idle();
            pad_disable();
            stage_set_current(menu_stages[work->submode]);
            if (SPLITS_SCREEN()) {
                game_state.split_screen = 1;
            } else {
                game_state.split_screen = 0;
            }
        } else if (pad_press_repeat[0] & pad_cancel_buttons) {
            if (game_state.two_players != 0) {
                work->mode = MODE_PLAYER_2;
            } else {
                work->mode = MODE_PLAYER_1;
            }
            PRESS_START_SHOWN(work) = FALSE;
            work->submode = 0;
        }
        break;
    case MODE_TITLE_OUT:
        if (FADE_OUT()) {
            work->mode = MODE_TITLE_END;
            title_result = OVERLAY_DONE;
        }
        break;
    case MODE_TAIKEN_ZOOM:
        if (work->timer++ > TAIKEN_ZOOM_WAIT) {
            if (FADE_IN()) {
                work->mode = MODE_TAIKEN_FLASH;
                fade_set_color(WHITE);
                work->timer = 0;
            }
            ZOOM(work) = float_approach(ZOOM(work), one, TAIKEN_ZOOM_STEP);
        } else {
            ZOOM(work) = TAIKEN_ZOOM_START;
        }
        break;
    case MODE_TAIKEN_FLASH:
        if (FADE_IN()) {
            work->mode = MODE_TAIKEN_TITLE;
            pad_enable();
            fade_set_color(FADE_CLEAR);
        }
        break;
    case MODE_TAIKEN_TITLE:
        BLINK_TIMER(work)++;
        work->timer++;
        if (BLINK_TIMER(work) >= BLINK_STEPS) {
            BLINK_TIMER(work) = 0;
        }
        if (BLINK_TIMER(work) <= BLINK_STEPS / 2) {
            PRESS_START_SHOWN(work) = FALSE;
        } else {
            PRESS_START_SHOWN(work) = TRUE;
        }
        if (pad_press_repeat[0] & PDD_DGT_ST) {
            work->mode = MODE_TAIKEN_START;
            PRESS_START_SHOWN(work) = FALSE;
            work->submode = 0;
        }
        if (work->timer > TAIKEN_DEMO_WAIT &&
            game_state.next_demo != NO_NEXT_DEMO) {
            work->mode = MODE_TAIKEN_OUT;
            fade_reset_idle();
            pad_disable();
            stage_set_current(title_demos[0].stage);
            game_state.two_players = title_demos[0].two_players;
            stage_set_character(0, title_demos[0].characters[0]);
            stage_set_character(1, title_demos[0].characters[1]);
            game_state.split_screen = 0;
            game_state.next_mode = GAME_MODE_DEMO;
            game_state.demo = DEMO_ON;
            flags_clear_all();
            if (game_state.next_demo == TAIKEN_EVENT_DEMO) {
                sequence_start(SEQUENCE_TAIKEN_EVENT);
                game_state.next_mode = GAME_MODE_SEQUENCE;
            }
            game_state.next_demo++;
            if (game_state.next_demo > TAIKEN_EVENT_DEMO) {
                game_state.next_demo = 0;
            }
        }
        break;
    case MODE_TAIKEN_START:
        sequence_start(SEQUENCE_TAIKEN);
        game_state.next_mode = GAME_MODE_SEQUENCE;
        work->mode = MODE_TAIKEN_OUT;
        fade_reset_idle();
        pad_disable();
        /* @hack redundant: next_mode set again, though nothing has changed
         * it */
        game_state.next_mode = GAME_MODE_SEQUENCE;
        music_fade_out();
        break;
    case MODE_TAIKEN_OUT:
        if (FADE_OUT()) {
            work->mode = MODE_TITLE_END;
            title_result = OVERLAY_DONE;
        }
        break;
    case MODE_TITLE_END:
        task_destroy(task);
        break;
    /* @hack redundant: a default that does nothing */
    default:
        break;
    }
    switch (work->mode) {
    case MODE_TEST_MENU:
    case MODE_PLAYER_1:
    case MODE_PLAYER_2:
    case MODE_STAGE_MENU:
        if ((pad_press_repeat[0] & PDD_DGT_TY) != 0 && EQUIPPED(work) == 0.0F) {
            FULL_EQUIPMENT(= 1, ;);
            EQUIPPED(work) = one;
        } else if ((pad_press_repeat[0] & PDD_DGT_TY) != 0 &&
                   EQUIPPED(work) != 0.0F) {
            FULL_EQUIPMENT(= 0, ;);
            EQUIPPED(work) = 0.0F;
        }
        if (EQUIPPED(work) == one) {
            njPrintC(MENU_EQUIPMENT_AT, "Full equipments!");
        } else {
            njPrintC(MENU_EQUIPMENT_AT, "[Y]Full equipments.");
        }
        njPrintC(MENU_MISSION_AT, "[X]To Change mission.Now it's ");
        if (pad_press_repeat[0] & PDD_DGT_TX) {
            game_state.mission++;
            if (game_state.mission > MISSION_HD) {
                game_state.mission = MISSION_FIRST;
            }
        }
        njPrintC(MENU_MISSION_NAME_AT, mission_names[game_state.mission]);
        break;
    }
}

static void title_screen_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&title_texlist);
    njDrawTexture(title_quad, QUAD_CORNERS, TITLE_TEXTURE, FALSE);
    if (PRESS_START_SHOWN(work)) {
        njSetTexture(&press_start_texlist);
        njDrawTexture(press_start_quad, QUAD_CORNERS, PRESS_START_TEXTURE,
                      TRUE);
    }
}

static void title_screen_draw_taiken(struct task *task)
{
    struct task_work *work = task->work;
    NJS_TEXTURE_VTX *quad = taiken_title_quads;
    int i;
    Int texture;
    Uint32 color;
    NJS_POINT3 corners[QUAD_CORNERS];

    if (work->mode != MODE_TAIKEN_ZOOM) {
        njSetTexture(&taiken_title_texlist);
        if (game_state.text_language == TEXT_JAPANESE) {
            texture = TAIKEN_TITLE_TEXTURE;
        } else {
            texture = TAIKEN_TITLE_TEXTURE_ENGLISH;
        }
        /* the title and then its glow draw through taiken_title_quads, so each
         * sets every corner's depth and color first */
        for (i = 0; i < PICTURE_QUADS; i++) {
            quad[0].z = TAIKEN_TITLE_Z;
            quad[1].z = TAIKEN_TITLE_Z;
            quad[2].z = TAIKEN_TITLE_Z;
            quad[3].z = TAIKEN_TITLE_Z;
            quad[0].col = WHITE;
            quad[1].col = WHITE;
            quad[2].col = WHITE;
            quad[3].col = WHITE;
            njDrawTexture(quad, QUAD_CORNERS, texture, FALSE);
            quad += QUAD_CORNERS;
            texture++;
        }
        if (game_state.text_language == TEXT_JAPANESE) {
            texture = TAIKEN_GLOW_TEXTURE;
        } else {
            texture = TAIKEN_GLOW_TEXTURE_ENGLISH;
        }
        PULSE(work) += PULSE_STEP;
        color = WHITE_AT((njSin(PULSE(work)) * GLOW_SWING) + GLOW_MIDDLE);
        quad = taiken_title_quads;
        for (i = 0; i < PICTURE_QUADS; i++) {
            quad[0].z = TAIKEN_GLOW_Z;
            quad[1].z = TAIKEN_GLOW_Z;
            quad[2].z = TAIKEN_GLOW_Z;
            quad[3].z = TAIKEN_GLOW_Z;
            quad[0].col = color;
            quad[1].col = color;
            quad[2].col = color;
            quad[3].col = color;
            njDrawTexture(quad, QUAD_CORNERS, texture, TRUE);
            quad += QUAD_CORNERS;
            texture++;
        }
        if (PRESS_START_SHOWN(work)) {
            njSetTexture(&press_start_texlist);
            njDrawTexture(press_start_quad, QUAD_CORNERS, PRESS_START_TEXTURE,
                          TRUE);
        }
    }
    if (work->mode == MODE_TAIKEN_ZOOM) {
        njSetTexture(&taiken_title_texlist);
        njSetTextureNum(TAIKEN_LOGO_INDEX);
        texture = njGetTextureNumG();
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslate(NULL, TAIKEN_LOGO_X, TAIKEN_LOGO_Y, 0.0F);
        njScale(NULL, ZOOM(work), ZOOM(work), 1.0F);
        njCalcPoints(NULL, taiken_logo_corners, corners, QUAD_CORNERS);
        njPopMatrixEx();
        taiken_logo_quad[0].x = corners[0].x;
        taiken_logo_quad[0].y = corners[0].y;
        taiken_logo_quad[1].x = corners[1].x;
        taiken_logo_quad[1].y = corners[1].y;
        taiken_logo_quad[2].x = corners[2].x;
        taiken_logo_quad[2].y = corners[2].y;
        taiken_logo_quad[3].x = corners[3].x;
        taiken_logo_quad[3].y = corners[3].y;
        taiken_logo_quad[0].col = taiken_logo_quad[1].col =
            taiken_logo_quad[2].col = taiken_logo_quad[3].col =
                WHITE_AT((TAIKEN_ZOOM_START - ZOOM(work)) * ALPHA_MAX);
        njDrawTexture(&taiken_logo_quad[0], QUAD_CORNERS, texture, TRUE);
    }
}
