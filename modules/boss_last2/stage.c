#include <ninja.h>
#include <sg_xpt.h>

#include "boss_last2/stage.h"
#include "boss_last2/backdrop.h"
#include "boss_last2/object_archives.h"
#include "boss_last2/sphere.h"
#include "boss_last2/arena.h"
#include "boss_last2/super.h"
#include "boss_last2/body.h"
#include "boss_last2/stars.h"
#include "camera/camera.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"

#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/last_boss_sphere.h"

#include "shared/messenger.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/areas.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/game_state.h"
#include "system/cache.h"
#include "system/texture.h"

#define STAR_COUNT 600
/* the light both players take */
#define PLAYER_LIGHT 0
/* the volume music_fade_in fades the music in to, 0 dB, its full volume */
#define MUSIC_VOLUME 0
#define BACK_COLOR 0xff000000
#define SKY_TILT NJM_DEG_ANG(30)
/* the tint the planet takes as the fade ends, from COLOR_NONE as it starts */
#define PLANET_TINT 0xffff8080
/* the backdrop's fade, in the stage task's unk_3c. it runs from 0 at
 * fade_start to 1 at fade_end, in game_state.steps */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define FADE(task) (*(Float *)&(task)->unk_3c)

static struct task *starfield = NULL;
static Sint32 fade_start = 7200;
static Sint32 fade_end = 18000;
static Sint32 unused_78ec = 1;
/* Earth turns earth_turn about y each step */
static Angle earth_turn = 3;
static Sint32 unused_78f4[2] = {2, 4};
static Float unused_height = -1500.0F;
static Float planet_scale = 1.6F;
static Float sky_height = 0.0F;
static Float sky_scale = 1.0F;
static Float earth_scale = 2.0F;
/* the planet's place from the camera as the fade ends, and as it starts */
static NJS_POINT3 planet_end = {0.0F, 4000.0F, 0.0F};
static NJS_POINT3 planet_start = {-4250.0F, -1500.0F, 0.0F};
/* the color tint_screen adds to the screen. it blends from tint_start as the
 * fade starts to tint_end as it ends. tint_start, black, adds nothing, and
 * tint_end adds red at half strength */
static Uint32 tint_start = 0xff000000;
static Uint32 tint_end = 0xff800000;
/* the screen, 640 by 480, as a strip of TINT_POINTS vertices */
#define TINT_POINTS 4
static NJS_POLYGON_VTX tint_quad[TINT_POINTS] = {
#include "boss_last2/stage/tint_quad.inc"
};
/* the matrices background_draw pushes for the camera's place and for
 * Earth, and pops together */
#define BACKDROP_PUSHES 2
static void background_step(struct task *task);
static void background_draw(struct task *task);

void last2_stage_init(void)
{
    njSetBackColor(BACK_COLOR, BACK_COLOR, BACK_COLOR);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(1.0F, 1.0F, 1.0F);
    light_set(PLAYER_LIGHT, &finalhazard_lights[0]);
    light_set(1, &finalhazard_lights[1]);
    light_set(LIGHT_LAND, &finalhazard_lights[1]);
    light_set(FINALHAZARD_LIGHT, &finalhazard_lights[2]);
    light_set(LAST_BOSS_EFFECT_LIGHT, &finalhazard_lights[2]);
    light_set(6, &finalhazard_lights[3]);
    light_select(PLAYER_LIGHT, 0);
    light_select(PLAYER_LIGHT, 1);
    light_select_camera(LIGHT_LAND);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg56_light.bin");

    if (!player_hooks.drop_rings) {
        player_hooks.drop_rings = ring_scatter;
    }
    texture_load_shared(finalhazard_common_pvms, finalhazard_common_groups);
#ifdef NONMATCHING
    /* this side loads the sky only where finalhazard_backdrop_available finds
     * it named, which the disc-free build's zeros don't */
    if (finalhazard_backdrop_available()) {
        texture_load_shared(finalhazard_sky_pvms, finalhazard_sky_groups);
    }
#else
    texture_load_shared(finalhazard_sky_pvms, finalhazard_sky_groups);
#endif
    music_fade_in(MUSIC_VOLUME);
    music_play_or_defer("boss_07.adx");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    if (player_tasks[0]) {
        task_destroy(player_tasks[0]);
    }
    super_sonic_create(0);
    stage_place_player(0);
    if (player_tasks[1]) {
        task_destroy(player_tasks[1]);
    }
    super_shadow_create(1);
    stage_place_player(1);
    finalhazard_textures_load();
    messenger_messages_load("mh0056");
    finalhazard_build_shots((Uint32)STAGE_MODEL_BUILD_AREA);
    sound_load_stage_and_voices("se_ac_lb2.mlt", &finalhazard_sounds,
                                FINALHAZARD_PLAYERS);
    if (!starfield) {
        finalhazard_starfield_create(0, STAR_COUNT, &starfield);
    }
}

void last2_stage_exit(void)
{
    if (starfield) {
        task_destroy(starfield);
        starfield = NULL;
    }
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    particle_core_stop();
#ifdef NONMATCHING
    /* this side releases the sky only where last2_stage_init loaded it, by the
     * same test */
    if (finalhazard_backdrop_available()) {
        texture_release_shared(finalhazard_sky_pvms, finalhazard_sky_groups);
    }
#else
    texture_release_shared(finalhazard_sky_pvms, finalhazard_sky_groups);
#endif
    texture_release_shared(finalhazard_common_pvms, finalhazard_common_groups);
    if (finalhazard_set) {
        loader_free(finalhazard_set);
        finalhazard_set = NULL;
    }
    messenger_messages_free();
    world_land_stop();
    finalhazard_textures_release();
    if (player_tasks[1]) {
        task_destroy(player_tasks[1]);
    }
    if (player_tasks[0]) {
        task_destroy(player_tasks[0]);
    }
}

void last2_stage_step(struct task *task)
{
    /* @hack redundant: work, a read of the task's work that nothing uses */
    struct task_work *work = task->work;
    (void)work;
#ifdef NONMATCHING
    /* this side quits the stage where either player or finalhazard_players is
     * missing, which the code below reads */
    if (!player_work[0] || !player_work[1] || !finalhazard_players) {
        stage_end(STAGE_END_QUIT);
        task_kill(task);
        return;
    }
#endif
    if (finalhazard_enabled) {
        finalhazard_create();
    }
    task->step = background_step;
    task->draw = background_draw;
}

static void background_step(struct task *task)
{
    Float fade;

    if (game_state.steps > (Uint32)fade_start) {
        if (game_state.steps < (Uint32)fade_end) {
            fade = (Float)(game_state.steps - fade_start) /
                   (Float)(fade_end - fade_start);
        } else {
            fade = 1.0F;
        }
    } else {
        fade = 0.0F;
    }
    if (fade > 1.0F) {
        fade = 1.0F;
    }
    if (fade < 0.0F) {
        fade = 0.0F;
    }
    FADE(task) = fade;
}

static void tint_screen(NJS_COLOR *color)
{
    if (color->argb.a == 0) {
        return;
    }
    tint_quad[0].col = tint_quad[1].col = tint_quad[2].col = tint_quad[3].col =
        color->color;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njDrawPolygon(tint_quad, TINT_POINTS, TRUE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void background_draw(struct task *task)
{
    /* @hack redundant: work, a read of the task's work that nothing uses */
    struct task_work *work = task->work;
    NJS_VECTOR pos;
    NJS_COLOR tint;
    Float fade;
    Float rest;
    (void)work;
    njFogDisable();
    njSetTexture(&finalhazard_sky_texlist);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, sky_height, 0.0F);
    njScale(NULL, sky_scale, 1.0F, sky_scale);
    njRotateZ(NULL, SKY_TILT);
    njCnkEasyDrawObject(&finalhazard_sky);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslateEx(&camera_views[camera_current_index]->pos);
    njSetTexture(&finalhazard_planet_texlist);
    njPushMatrixEx();
    fade = FADE(task);
    rest = 1.0F - fade;
    pos.x = (planet_end.x * fade) + (planet_start.x * rest);
    pos.y = (planet_end.y * fade) + (planet_start.y * rest);
    pos.z = (planet_end.z * fade) + (planet_start.z * rest);
    njTranslateEx(&pos);
    tint.color = color_blend(tint_start, tint_end, fade);
    tint_screen(&tint);
    njScale(NULL, planet_scale, 1.0F, planet_scale);
    njTranslateEx((NJS_VECTOR *)finalhazard_planet->pos);
    model_draw_tinted(finalhazard_planet->model,
                      color_blend(COLOR_NONE, PLANET_TINT, FADE(task)));
    njPopMatrixEx();
    njPushMatrixEx();
    njSetTexture(&finalhazard_earth_texlist);
    njTranslateEx((NJS_VECTOR *)finalhazard_earth.pos);
    njTranslate(NULL, finalhazard_earth.model->center.x,
                finalhazard_earth.model->center.y,
                finalhazard_earth.model->center.z);
    njRotateY(NULL, earth_turn * (Angle)game_state.steps);
    njScale(NULL, earth_scale, earth_scale, earth_scale);
    njTranslate(NULL, -finalhazard_earth.model->center.x,
                -finalhazard_earth.model->center.y,
                -finalhazard_earth.model->center.z);
    njCnkEasyDrawModel(finalhazard_earth.model);
    matrix_load_parent();
    njPopMatrix(BACKDROP_PUSHES);
    njFogEnable();
}
