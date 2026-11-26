/* the Egg Golem stage of stage.h: its init and exit, its task's steps and the
 * background's draw. boss_golem/stage.c is BOSS_GOLEM.PRS's twin, a source of
 * its own */
#include <ninja.h>
#include <sg_xpt.h>

#ifdef NONMATCHING
/* golem_effect_materials, which this side's stage tests before it loads the
 * golem's textures */
#include "shared/golem_material/names.h"
#endif
#include "boss_golem_e/footholds.h"
#include "boss_golem_e/golem.h"
#include "boss_golem_e/stage.h"
#include "boss_golem_e/weak_points.h"
#include "collision/world.h"
#include "draw/color.h"
#include "draw/light.h"
#include "draw/texture_anim.h"
#include "effect/particle.h"
#include "enemy/enemy.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/black_smoke.h"
#include "shared/blast.h"
#include "shared/bomb_hat.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/golem_archives.h"
#include "shared/golem_land.h"
#include "shared/golem_reel.h"
#include "shared/golem_sounds.h"
#include "shared/golem_title.h"
#include "shared/golem_util.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/ring.h"
#include "shared/ugokidai.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

static Float reel_radius;
static Float reel_y;
static Float reel_down_length;
static Angle reel_returns;
static NJS_TEXLIST pulse_texlist;
static NJS_CNK_MODEL pulse_model;
static NJS_TEXLIST sand_texlist;
static struct texture_anim sand_scroll;
static NJS_CNK_OBJECT sand_object[1];

#define SET_MIN_ENTRIES 512
/* the light both players take */
#define PLAYER_LIGHT 0
#define BACKGROUND_LEVEL 1
#define REEL_LEVEL 2
#define REELS 6

static struct task *golem_e_reels[REELS];
static Float golem_e_arena_radius;

/* arena_hold_player holds player 0 within golem_e_arena_radius of the origin,
 * along x and z. the radius is ARENA_RADIUS until player 0 drops below
 * LOW_LINE. it is then OUTER_RADIUS until player 0 is back at LOW_LINE or
 * above, within ARENA_RADIUS of the origin */
#define ARENA_RADIUS 233.33301F
#define OUTER_RADIUS 300.0F
#define LOW_LINE 120.0F

/* the pulse model's alpha rises to 255 over the first PULSE_RISE_STEPS of each
 * PULSE_STEPS. it falls back to 0 over the rest */
#define PULSE_STEPS 60
#define PULSE_RISE_STEPS 5
#define PULSE_LAST (PULSE_STEPS - 1)
/* the sand scrolls back a unit of texture_anim_apply's time each
 * 1 << SAND_TIME_SHIFT steps */
#define SAND_TIME_SHIFT 1
#define WHITE 0x00ffffff

/* @hack spelling: the data defined after the code, with tentative
 * definitions here, where its definitions here would do */
static struct texture_pvm background_pvms[2];
static NJS_TEXLIST **background_texture_groups[2];
static struct kill_area golem_kill_areas[2];
static struct light golem_lights[2];
static NJS_POINT3 golem_home;
static struct set_head *golem_set;
static Angle reel_turn;
static Angle reel_throw_tilt;
static Float reel_throw_speed;

static void arena_step(struct task *task);
static void background_step(struct task *task);
static void background_draw(struct task *task);
static void reels_create(void);

void golem_e_stage_init(void)
{
    struct task *task;
    Uint8 *buf;

    world_land_start(&golem_land);
#ifdef NONMATCHING
    /* this side loads the golem's textures only where golem_effect_materials
     * holds their names. the disc-free build's zeros hold none. it awaits only
     * a load that started */
    if (golem_effect_materials.bs0_ml1[0] != '\0' &&
        texture_load_shared(golem_texture_pvms, golem_texture_groups)) {
        texture_await_shared(golem_texture_pvms, golem_texture_groups);
    }
#else
    texture_load_shared(golem_texture_pvms, golem_texture_groups);
    texture_await_shared(golem_texture_pvms, golem_texture_groups);
#endif
    texture_load_shared(background_pvms, background_texture_groups);
    texture_await_shared(background_pvms, background_texture_groups);
    golem_set =
        set_read("set_b_Golem_e_s.bin", "set_b_Golem_e_u.bin", SET_MIN_ENTRIES);
    set_start(&golem_e_object_list, golem_set);
    particle_core_start();
    njCnkSetAmbient(1.0F, 1.0F, 1.0F);
    light_set(PLAYER_LIGHT, &golem_lights[0]);
    light_set(1, &golem_lights[1]);
    light_set(LIGHT_LAND, &golem_lights[1]);
    light_set(3, &golem_lights[0]);
    light_select(PLAYER_LIGHT, 0);
    light_select(PLAYER_LIGHT, 1);
    light_select_camera(LIGHT_LAND);
    light_select_after_draw(LIGHT_LAND);
    buf =
        (Uint8 *)world_build_land(&golem_land, (Uint32)STAGE_MODEL_BUILD_AREA);
    buf = object_builds_run(buf);
    buf = golem_e_footholds_build(buf);
    buf = ugokidai_build(buf);
    golem_e_parts_build(buf);
    kill_areas_start(golem_kill_areas);
    module_motions_register();
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (player_hooks.draw_blast_ring == NULL) {
        player_hooks.draw_blast_ring = bomb_hat_draw_ring;
    }
    texture_load_pvm("titletex_golem", &golem_title_texlist);
    music_fade_in(0);
    music_play_or_defer("boss_02b.adx");
    messenger_messages_load("mh0057");
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        task->draw = background_draw;
    }
    enemy_groups_clear();
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    blast_load_texture();
    broken_down_smoke_create(0, NULL);
    smoke_task_create(0, NULL);
    sound_load_stage("se_ac_egl.mlt", &golem_sound_list);
    loader_read_camera_files();
}

void golem_e_stage_exit(void)
{
    if (player_hooks.draw_blast_ring == bomb_hat_draw_ring) {
        player_hooks.draw_blast_ring = NULL;
    }
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    particle_core_stop();
#ifdef NONMATCHING
    /* this side releases the golem's textures only where golem_e_stage_init
     * loaded them, by the same test */
    if (golem_effect_materials.bs0_ml1[0] != '\0') {
        texture_release_shared(golem_texture_pvms, golem_texture_groups);
    }
#else
    texture_release_shared(golem_texture_pvms, golem_texture_groups);
#endif
    texture_release_shared(background_pvms, background_texture_groups);
    njReleaseTexture(&golem_title_texlist);
    set_end();
    if (golem_set != NULL) {
        loader_free(golem_set);
        golem_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
    messenger_messages_free();
}

#ifdef NONMATCHING
/* create the footholds, or try again at the next step where they can't be */
static void arena_start_footholds(struct task *task)
{
    if (golem_e_footholds_create()) {
        golem_e_footholds_open_gaps();
        golem_e_arena_radius = ARENA_RADIUS;
        task->step = arena_step;
    }
}
#endif

void golem_e_stage_step(struct task *task)
{
    NJS_POINT3 pos = {0.0F, 0.0F, 0.0F};

#ifdef NONMATCHING
    /* this side builds the arena only once golem_e_create succeeds, and tries
     * again the next step where it fails. arena_start_footholds waits for the
     * footholds the same way */
    if (golem_e_create(&pos, 0, &golem_home) != NULL) {
        reels_create();
        task->step = arena_start_footholds;
        arena_start_footholds(task);
    }
#else
    golem_e_create(&pos, 0, &golem_home);
    reels_create();
    golem_e_footholds_create();
    golem_e_footholds_open_gaps();
    golem_e_arena_radius = ARENA_RADIUS;
    task->step = arena_step;
#endif
}

static void arena_hold_player(void)
{
    NJS_VECTOR flat;
    Float distance;

    flat.x = player_work[0]->pos.x;
    flat.y = 0.0F;
    flat.z = player_work[0]->pos.z;
    distance = njScalor(&flat);
    if (golem_e_arena_radius == ARENA_RADIUS) {
        if (player_work[0]->pos.y < LOW_LINE) {
            golem_e_arena_radius = OUTER_RADIUS;
        }
    } else if (player_work[0]->pos.y >= LOW_LINE && distance < ARENA_RADIUS) {
        golem_e_arena_radius = ARENA_RADIUS;
    }
    if (distance > golem_e_arena_radius) {
        player_work[0]->pos.x = flat.x / distance * golem_e_arena_radius;
        player_work[0]->pos.z = flat.z / distance * golem_e_arena_radius;
    }
}

static void arena_step(struct task *task)
{
    arena_hold_player();
}

static void background_step(struct task *task)
{
}

/* unlike boss_golem/stage.c's, this draw doesn't set DRAW_CONTROL_SHADOW around
 * the sand */
static void background_draw(struct task *task)
{
    int frame;
    Uint32 color;

    frame = game_state.steps % PULSE_STEPS;
    if (frame < PULSE_RISE_STEPS) {
        color = ((Uint32)(int)(frame * (255.0F / (PULSE_RISE_STEPS - 1)))
                 << ALPHA_SHIFT) |
                WHITE;
    } else {
        color = ((Uint32)(int)((PULSE_LAST - frame) *
                               (255.0F / (PULSE_LAST - PULSE_RISE_STEPS)))
                 << ALPHA_SHIFT) |
                WHITE;
    }
    njSetTexture(&pulse_texlist);
    model_draw_tinted(&pulse_model, color);
    njSetTexture(&sand_texlist);
    texture_anim_apply(sand_object->model, &sand_scroll,
                       -game_state.steps >> SAND_TIME_SHIFT);
    njCnkEasyDrawModel(sand_object->model);
}

/* create the reels, a sixth of a turn apart, the first a twelfth of a turn
 * round. a reel whose task can't be created is left out */
static void reels_create(void)
{
    struct task *task;
    struct task_work *work;
    int i;

    for (i = 0; i < REELS; i++) {
        /* @hack spelling: the assignments inside the test, where a statement
         * of their own would do */
        /* @hack spelling: the continue, where a test around the rest would
         * do */
        if ((golem_e_reels[i] = task =
                 task_create(TASK_WORK, REEL_LEVEL, golem_reel_init,
                             "ObjectGolemUdreel")) == NULL) {
            continue;
        }
        work = task->work;
        work->ang.x = reel_returns;
        work->ang.y =
            (FULL_TURN / (REELS * 2.0F)) + (i * (FULL_TURN / (Float)REELS));
        work->ang.z = reel_throw_tilt;
        work->scl.x = 0.0F;
        work->scl.y =
            (reel_down_length * (1.0F / GOLEM_REEL_LENGTH_UNIT)) - 1.0F;
        work->scl.z = reel_throw_speed;
        golem_polar_xz(work->ang.y, reel_radius, &work->pos.x, &work->pos.z);
        work->ang.y += reel_turn;
        work->pos.y = reel_y;
    }
}

static Float reel_radius = 250.0F;
static Float reel_y = 255.0F;
/* the wire a reel pays out to the bottom of a ride */
static Float reel_down_length = 180.0F;
/* each reel's ang.x */
static Angle reel_returns = GOLEM_REEL_RETURNS;

#include "boss_golem_e/stage/text.inc"
#include "shared/golem_scenery/arena.h"

static NJS_POINT3 golem_home = {0.0F, 0.0F, 0.0F};
static Float dat_8c54d8f4 = 210.0F;
static struct set_head *golem_set = NULL;
/* a reel's turn beyond its heading about the arena */
static Angle reel_turn = HALF_TURN + QUARTER_TURN;
/* what a reel adds to its throw's tilt from straight up, and to its throw's
 * speed */
static Angle reel_throw_tilt = EIGHTH_TURN;
static Float reel_throw_speed = 1.0F;
