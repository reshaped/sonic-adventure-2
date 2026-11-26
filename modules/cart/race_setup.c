/* the race's set-up: the course's load and release, and the racers on their
 * grid. it starts the race's tasks and the boost windows */
#include <ninja.h>
#include <sg_xpt.h>
#include <string.h>

#include "camera/camera.h"
#include "cart/background_art.h"
#include "cart/boost_window.h"
#include "cart/course.h"
#include "cart/objects.h"
#include "cart/pads.h"
#include "cart/race.h"
#include "cart/stage.h"
#include "cart/vehicle.h"
#include "draw/text_sprite.h"
#include "hud/action_window.h"
#include "hud/hud.h"
#include "player/player.h"
#include "sound/adx.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/texture.h"

/* the voices each story race plays as its course loads */
#define TAILS_START_VOICE 0x700
#define ROUGE_START_VOICE 0x707
/* the grid has two columns, GRID_SPREAD apart from GRID_X. its first kart
 * stands at GRID_Z, and each next one GRID_STEP behind. a two-player race's
 * first two karts share the front row */
#define GRID_X 30.0F
#define GRID_SPREAD 80.0F
#define GRID_Y 6.0F
#define GRID_Z (-100.0F)
#define GRID_STEP 40
/* the boost window's text, and its glyphs' pitch along x */
#define BOOST_TEXT "\aBOOST"
#define BOOST_PITCH 3
/* the text's scale and colors, which each window's first step sets again */
#define BOOST_SCALE 0.7F
#define BOOST_COLOR 0xff000000
#define BOOST_SHADOW 0x00404000

/* each character's kart, by its player kind */
static Sint32 character_models[] = {CART_MODEL_SONIC,    CART_MODEL_SHADOW,
                                    CART_MODEL_TAILS,    CART_MODEL_EGGMAN,
                                    CART_MODEL_KNUCKLES, CART_MODEL_ROUGE,
                                    CART_MODEL_TAILS,    CART_MODEL_EGGMAN};

void cart_load_course(void)
{
    struct cart_course *course;
    switch (cart_setup.course_mode) {
    case CART_TAILS:
        life_icon_load(PLAYER_TAILS_MECH);
        cart_select_layout(cart_setup.course_mode);
        cart_load_chase_textures();
        break;
    case CART_ROUGE:
        life_icon_load(PLAYER_ROUGE);
        cart_select_layout(cart_setup.course_mode);
        cart_load_chase_textures();
        break;
    case CART_BEGINNER:
    case CART_STANDARD:
    case CART_EXPERT:
    case CART_DOWNLOAD:
        cart_select_layout(cart_setup.course_mode);
        break;
    }
    cart_set_tuning();
#ifdef NONMATCHING
    /* this side's cart_init has set up the model slots and the download */
#else
    cart_init_models();
    if (cart_setup.course_mode == CART_DOWNLOAD) {
        cart_load_download_model();
    }
#endif
    cart_load_model_textures();
    cart_build_course(cart_setup.pieces, cart_setup.piece_count);
    cart_set_pad_delay(0);
    njLoadTexturePvmFile("bgCart.pvm", &cart_floor_textures);
    texture_load_pvm("bgEnkei", &cart_sky_textures);
    cart_load_result_textures();
    music_fade_in(0);
    if (cart_setup.course_mode == CART_DOWNLOAD) {
        course = cart_download_course();
        music_play_name(course->music);
    } else {
        music_play_name("kart.adx");
    }
    switch (cart_setup.course_mode) {
    case CART_TAILS:
        voice_play_priority_2(TAILS_START_VOICE);
        break;
    case CART_ROUGE:
        voice_play_priority_2(ROUGE_START_VOICE);
        break;
    }
}
void cart_unload_course(int mode)
{
    switch (mode) {
    case CART_PLAY_RACE:
        break;
    case CART_PLAY_STORY:
        switch (cart_setup.course_mode) {
        case CART_TAILS:
            life_icon_release(PLAYER_TAILS_MECH);
            break;
        case CART_ROUGE:
            life_icon_release(PLAYER_ROUGE);
            break;
        }
        cart_release_chase_textures();
    }
    camera_stop();
    cart_release_models();
    cart_release_model_textures();
    njReleaseTexture(&cart_floor_textures);
    njReleaseTexture(&cart_sky_textures);
    cart_release_result_textures();
}

static void spawn_racers(Sint32 play_mode)
{
    Sint32 count;
    Sint32 i;
    Sint32 j;
    Sint32 player = 0;
    Sint32 used[CART_RACERS];
    struct cart_start start;
    Float z;
    struct cart_course *course;
    /* @hack spelling: race, a pointer to cart_race, where cart_race itself
     * would do */
    struct cart_race *race;
    memset(cart_racers, 0, sizeof(cart_racers));
    for (i = 0; i < CART_RACERS; i++) {
        used[i] = 0;
    }
    switch (play_mode) {
    case CART_PLAY_RACE:
        count = CART_RACERS;
        cart_story_approach = 0;
        cart_clock_held = 1;
        break;
    default:
        count = 1;
        cart_story_approach = 1;
        cart_clock_held = 0;
        break;
    }
    cart_boost_ready[0] = 0;
    cart_boost_ready[1] = 0;
    for (i = 0; i < count; i++) {
        switch (cart_setup.course_mode) {
        case CART_TAILS:
            start.model = CART_MODEL_TAILS_STORY;
            break;
        case CART_ROUGE:
            start.model = CART_MODEL_ROUGE_STORY;
            break;
        case CART_DOWNLOAD:
            /* where game_state.kart_character_karts is 1, a download course
             * takes the characters' karts. where it is 0, the course takes its
             * own kart for the first racer. it takes it for the others too
             * where it is special */
            switch (game_state.kart_character_karts) {
            case 1:
                goto choose_racer;
                /* @hack redundant: a break after the goto */
                break;
            case 0:
                if (i == 0) {
                    start.model = CART_MODEL_DOWNLOAD;
                } else {
                    course = cart_download_course();
                    if (course->special) {
                        start.model = CART_MODEL_DOWNLOAD;
                    } else {
                        start.model = i + CART_MODEL_SONIC;
                    }
                }
                break;
#ifdef NONMATCHING
            /* this side's default takes the characters' karts. the menus set
             * kart_character_karts to 0 or 1 alone */
            default:
                goto choose_racer;
#endif
            }
            break;
        default:
        choose_racer:
            if (i == 0) {
                start.model = character_models[stage_character(0)];
                used[start.model - CART_MODEL_SONIC] = 1;
            } else if (game_state.two_players && i == 1) {
                start.model = character_models[stage_character(1)];
                used[start.model - CART_MODEL_SONIC] = 1;
            } else {
                for (j = 0; j < CART_RACERS; j++) {
                    if (!used[j]) {
                        start.model = j + CART_MODEL_SONIC;
                        used[j] = 1;
                        break;
                    }
                }
            }
            break;
        }
        start.player = i;
        start.pos.x = GRID_X - ((i % 2) * GRID_SPREAD);
        start.pos.y = GRID_Y;
        z = GRID_Z;
        /* @hack spelling: the test with an empty arm, where the test of
         * !(two_players && i < 2) would do */
        if (game_state.two_players && i < 2) {
        } else {
            z -= i * GRID_STEP;
        }
        start.pos.z = z;
        start.ang.x = 0;
        start.ang.y = 0;
        start.ang.z = 0;
        if (i < camera_active_count) {
            start.player_controlled = 1;
        } else {
            start.player_controlled = 0;
        }
        cart_racers[i] = cart_create_racer(&start);
        if (start.player_controlled) {
            race = &cart_race;
            race->player[player++].task = cart_racers[i];
        }
    }
    cart_reset_history();
}

void cart_start_race(void)
{
    Sint32 i;
    cart_background_start();
    cart_start_course_draw();
    cart_start_course_collision();
    cart_pads_start();
    spawn_racers(cart_setup.play_mode);
    if (cart_setup.course_mode <= CART_ROUGE &&
        game_state.mission != MISSION_TIMED) {
        cart_start_traffic();
    }
    cart_clock_start();
    for (i = 0; i < camera_active_count; i++) {
        cart_camera_start(i);
        cart_window_start(CART_WINDOW_WHOLE + i + (game_state.two_players > 0),
                          &cart_boost_windows[i]);
    }
    cart_start_hud();
    cart_reset_checkpoints();
}

struct action_window cart_boost_windows[CART_PLAYERS];

void cart_boost_windows_start(void)
{
    struct text_sprite *sprite;
    struct action_window *window;
    int i;
    sprite =
        text_sprite_create_styled(BOOST_TEXT, game_state.text_language,
                                  TEXT_WHITE, BOOST_PITCH, TEXT_PITCH_DEFAULT);
    for (i = 0; i < CART_PLAYERS; i++) {
        window = &cart_boost_windows[i];
        window->sprite = sprite;
        text_sprite_measure(window->sprite, BOOST_TEXT,
                            game_state.text_language, &window->text_width,
                            &window->text_height);
        window->sprite->scale_x = BOOST_SCALE;
        window->sprite->scale_y = BOOST_SCALE;
        window->sprite->color.color = BOOST_COLOR;
        window->sprite->shadow_color.color = BOOST_SHADOW;
        window->pressed = 0;
    }
}
void cart_boost_windows_stop(void)
{
    text_sprite_destroy(cart_boost_windows[0].sprite);
}
void cart_boost_window_set(int player, int pressed)
{
    cart_boost_windows[player].pressed = pressed;
}
