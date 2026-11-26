/* the cart stage's entry points: its set-up, its first step, which loads the
 * course, and its teardown */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "cart/boost_window.h"
#include "cart/course.h"
#include "cart/objects.h"
#include "cart/stage.h"
#include "cart/stage_assets.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/hud.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/player.h"
#include "save/save.h"
#include "shared/common_object.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/frame.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

/* each frame lasts FRAME_BLANKS vertical blanks and takes FRAME_STEPS steps */
#define FRAME_BLANKS 1
#define FRAME_STEPS 1
/* the life a story race's player gains as the stage starts */
#define START_LIFE 1
/* the lights cart_init sets. KART_LIGHT, which the players and the camera
 * take, and LIGHT_LAND hold day_light. DIM_LIGHT holds dim_light and
 * SPARE_LIGHT day_light, and no cart code selects either */
#define KART_LIGHT 0
#define DIM_LIGHT 1
#define SPARE_LIGHT 3
/* world_collect_near_players' radius on the course, which world_land_start
 * sets to 400 */
#define NEAR_RADIUS 1000000.0F
/* read a course's set file into cart_set.file */
#ifdef NONMATCHING
/* this side checks the file, and reads no more than cart_set.file holds */
#define READ_CART_SET(name, dest)                                              \
    set_read_into((name), &(dest)->head, sizeof(*(dest)))
#else
#define READ_CART_SET(name, dest) loader_read_file((name), (dest))
#endif

static int stage_mode = 0;
static int previous_mode = 0;
static struct land_table course_land = {0,    -1,   -1,   0,    1,   0,
                                        3000, NULL, NULL, NULL, NULL};
static struct light day_light = {{0, -0.707F, 0.707F}, 1, 0.2F, 1, 1, 1};
static struct light dim_light = {
    {0, -0.707F, 0.707F}, 0.5F, 0.2F, 0.5F, 0.5F, 0.5F};
static Bool entering = 1;
Sint32 cart_traffic_paused = 0;

/* the fields of game_state that cart_init saves and cart_exit restores */
static Sint8 previous_0b, previous_hud;

void cart_set_mode(int mode)
{
    previous_mode = stage_mode;
    stage_mode = mode;
}
void cart_step(struct task *task)
{
    if (entering) {
        cart_load_course();
        entering = 0;
    }
}
void cart_init(void)
{
    /* @hack spelling: setup, a pointer to cart_setup, where cart_setup itself
     * would do */
    struct cart_setup *setup;
    /* @hack spelling: light, a pointer to day_light, where &day_light would
     * do */
    struct light *light;
    Uint32 *word;
    int count;
    /* @hack spelling: buffer, a pointer to cart_set.file, where
     * &cart_set.file would do */
    struct cart_set_file *buffer;

#ifdef NONMATCHING
    /* this side's cart_exit tests entering to learn whether cart_step loaded
     * the course, so each start sets it again */
    entering = 1;
#endif
    frame_pacing_init(FRAME_BLANKS, FRAME_STEPS);
    cart_set_mode(CART_MODE_START);
    if (game_state.voice_language) {
        sound_load_cart("se_kart_e.mlt", &cart_sounds);
    } else {
        sound_load_cart("se_kart.mlt", &cart_sounds);
    }
    setup = &cart_setup;
    if (stage_current_id() == STAGE_CART_1) {
        setup->play_mode = CART_PLAY_STORY;
        if (stage_character(0) == PLAYER_ROUGE) {
            setup->course_mode = CART_ROUGE;
        } else {
            setup->course_mode = CART_TAILS;
        }
        lives_add(0, START_LIFE);
    } else {
        setup->play_mode = CART_PLAY_RACE;
#ifdef NONMATCHING
        /* this side takes course 0 for a kart_course outside the save's records
         * and the download. the tests below don't refuse such a course */
        if (game_state.kart_course < 0 ||
            game_state.kart_course > SAVE_KART_RECORDS) {
            game_state.kart_course = 0;
        }
#endif
        if (game_state.kart_course < SAVE_KART_RECORDS) {
            setup->course_mode = game_state.kart_course + CART_BEGINNER;
        } else {
            setup->course_mode = CART_DOWNLOAD;
#ifdef NONMATCHING
            /* this side loads the download below, once its model slots
             * are set */
#else
            cart_download_load();
#endif
        }
    }
    world_land_start(&course_land);
    loader_read_camera_files();
    light_set(KART_LIGHT, light = &day_light);
    light_set(DIM_LIGHT, &dim_light);
    light_set(LIGHT_LAND, light);
    light_set(SPARE_LIGHT, light);
    light_select(KART_LIGHT, 0);
    light_select(KART_LIGHT, 1);
    light_select_camera(KART_LIGHT);
    light_select_after_draw(LIGHT_LAND);
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = cart_action_icon;
    stage_hooks.number_cell = cart_number_cell;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    particle_core_start();
    texture_load_shared(cart_pvms, cart_texture_groups);
    texture_await_shared(cart_pvms, cart_texture_groups);
    njLoadTexturePvmFile("course.pvm", &cart_object_textures);
    if (setup->play_mode == CART_PLAY_STORY) {
        njLoadTexturePvmFile("cartObjModels.pvm", &cart_story_textures);
    }
#ifdef NONMATCHING
    cart_init_models();
    if (setup->course_mode == CART_DOWNLOAD &&
        (download_chosen.slot < 0 || download_chosen.slot >= DOWNLOAD_SLOTS ||
         !cart_download_load())) {
        /* kart_course falls back to 0, as the course mode falls back to
         * CART_BEGINNER. the results and the HUD read it */
        game_state.kart_course = 0;
        setup->course_mode = CART_BEGINNER;
    }
#endif
    word = cart_set.words;
    count = sizeof(cart_set.words) / sizeof(cart_set.words[0]);
    do {
        *word++ = 0;
    } while (--count);
    buffer = &cart_set.file;
    switch (setup->course_mode) {
    case CART_TAILS:
        if (game_state.mission == MISSION_HD) {
            READ_CART_SET("setCartTails_hd.bin", buffer);
        } else {
            READ_CART_SET("setCartTails.bin", buffer);
        }
        break;
    case CART_ROUGE:
        if (game_state.mission == MISSION_HD) {
            READ_CART_SET("setCartRouge_hd.bin", buffer);
        } else {
            READ_CART_SET("setCartRouge.bin", buffer);
        }
        break;
    case CART_BEGINNER:
        READ_CART_SET("setCartMini1.bin", buffer);
        break;
    case CART_STANDARD:
        READ_CART_SET("setCartMini2.bin", buffer);
        break;
    case CART_EXPERT:
        READ_CART_SET("setCartMini3.bin", buffer);
        break;
    case CART_DOWNLOAD:
#ifdef NONMATCHING
        /* this side bounds the copy by cart_set's size */
        cart_download_copy_set(buffer, sizeof(cart_set));
#else
        cart_download_copy_set(buffer);
#endif
        break;
    }
    set_start(&cart_objects, &cart_set.file.head);
    previous_0b = game_state.unk_0b;
    previous_hud = game_state.hud_on;
    game_state.hud_on = 0;
    world_set_near_radius(NEAR_RADIUS);
    particle_core_start();
    cart_boost_windows_start();
}
void cart_exit(void)
{
    sound_release_fx_level();
    particle_core_stop();
    texture_release_shared(cart_pvms, cart_texture_groups);
    njReleaseTexture(&cart_object_textures);
    if (cart_setup.play_mode == CART_PLAY_STORY) {
        njReleaseTexture(&cart_story_textures);
    }
    /* @bug logic: the course mode passed where cart_unload_course takes the
     * play mode (0x8c500382). Tails's story course, mode 0, so keeps its life
     * icon and chase textures loaded */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it passes cart_unload_course the
     * play mode */
    if (entering) {
        /* cart_step never ran, so only cart_init's models are loaded */
        cart_release_models();
        entering = 0;
    } else {
        cart_unload_course(cart_setup.play_mode);
    }
#else
    cart_unload_course(cart_setup.course_mode);
#endif
    world_land_stop();
    camera_stop();
    game_state.unk_0b = previous_0b;
    game_state.hud_on = previous_hud;
    particle_core_stop();
    sound_task_kill();
    world_reset_near_radius();
    cart_boost_windows_stop();
}
