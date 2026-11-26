/* STAGE_VERSUS_MECH_2, the second fight of Tails's and Eggman's walkers: its
 * init and exit, and its tasks' steps. the backdrop's task, BgExec, draws the
 * earth and two spheres of space about the current camera */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/bosspoint.h"
#include "camera/camera.h"
#include "camera/stack.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "player/walker.h"
#include "shared/black_smoke.h"
#include "shared/blast.h"
#include "shared/bomb_hat.h"
#include "shared/common_object.h"
#include "shared/fog.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/ring.h"
#include "sound/adx.h"
#include "stage/stage.h"
#include "stg29/rival_walker.h"
#include "stg29/versus_mech_2.h"
#include "stg29/object_archives.h"
#include "system/areas.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"

#include "stg29/versus_mech_2/text.inc"

/* the fog that versus_mech_2_init reads from stg29_fog.bin */
static struct fog_setting versus_mech_2_fog;

struct backdrop_names {
    char earth512_2[12];
    char spacenbg128[12];
    char spacenbg64[12];
    char y_co8_11[9];
};
static const struct backdrop_names backdrop_names;
#include "stg29/backdrop.h"

#define BACKDROP_LEVEL 1
/* the least the set file is read as, in entries */
#define SET_ENTRIES 512

/* each sphere of space turns about its model's center. it turns about x by
 * SPACE_TILT plus its spin_x times game_state.steps. it then turns about its
 * new y axis by its spin_y times the steps */
#define SPACE_TILT 0x3000

enum versus_mech_2_mode { MODE_START_CAMERA, MODE_FOLLOW_RIVAL };

static void backdrop_step(struct task *task);
static void backdrop_destroy(struct task *task);
static void backdrop_draw(struct task *task);

/* the backdrop's lists, which take their textures from backdrop_pvm_texlist
 * by name */
static NJS_TEXLIST *backdrop_texlists[4] = {
#include "stg29/versus_mech_2/backdrop_texlists.inc"
};
static NJS_TEXLIST **backdrop_texture_groups[2] = {
#include "stg29/versus_mech_2/backdrop_texture_groups.inc"
};
static struct texture_pvm backdrop_pvms[2] = {
#include "stg29/versus_mech_2/backdrop_pvms.inc"
};
/* the stage has no kill areas */
static struct kill_area versus_mech_2_kill_areas[] = {{0, NULL}};
static struct light versus_mech_2_lights[2] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F},
};
/* the set file versus_mech_2_init read, until versus_mech_2_exit frees it */
static struct set_head *versus_mech_2_set = NULL;
/* the spheres' spins, in Angle units for each of game_state.steps */
static Sint32 space_outer_spin_x = 1;
static Sint32 space_outer_spin_y = 2;
static Sint32 space_inner_spin_x = 2;
static Sint32 space_inner_spin_y = 4;
/* the earth draws earth_lift higher than earth_object places it, and
 * earth_scale times as wide in x and z */
static Float earth_lift = 3700.0F;
static Float earth_scale = 1.6F;

void versus_mech_2_init(void)
{
    struct task *task;

    if (stage_character(0) == PLAYER_EGGMAN_MECH) {
        walker_create_tails(1);
    } else if (stage_character(0) == PLAYER_TAILS_MECH) {
        walker_create_eggman(1);
    }
    if (player_tasks[1]) {
        stage_place_player(1);
        player_work[1]->id = WORK_ID_RIVAL;
        pad_disable_player(1);
        rival_walker_create();
        broken_down_smoke_create(0, NULL);
        broken_down_smoke_create(1, NULL);
    }
    world_land_start(&versus_mech_2_land);
    texture_load_shared(versus_mech_2_pvms, versus_mech_2_texture_groups);
    texture_await_shared(versus_mech_2_pvms, versus_mech_2_texture_groups);
    texture_load_shared(backdrop_pvms, backdrop_texture_groups);
    texture_await_shared(backdrop_pvms, backdrop_texture_groups);
    versus_mech_2_set = set_read("set0029_s.bin", "set0029_u.bin", SET_ENTRIES);
    set_start(&versus_mech_2_objects, versus_mech_2_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &versus_mech_2_lights[0]);
    light_set(1, &versus_mech_2_lights[1]);
    light_set(2, &versus_mech_2_lights[0]);
    light_set(3, &versus_mech_2_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    world_build_land(&versus_mech_2_land, (Uint32)STAGE_MODEL_BUILD_AREA);
    task = task_create(TASK_WORK, BACKDROP_LEVEL, backdrop_step, "BgExec");
    if (task) {
        task->draw = backdrop_draw;
        task->destructor = backdrop_destroy;
    }
    kill_areas_start(versus_mech_2_kill_areas);
    if (!player_hooks.drop_rings) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (!player_hooks.add_item_icon) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    if (!player_hooks.draw_blast_ring) {
        player_hooks.draw_blast_ring = bomb_hat_draw_ring;
    }
    fog_start_file("stg29_fog.bin", &versus_mech_2_fog);
    njFogDisable();
    messenger_messages_load("mh0029");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    blast_load_texture();
    music_fade_in(0);
    music_play_or_defer("BOSS_03.ADX");
}

static const struct backdrop_names backdrop_names = {
#include "stg29/versus_mech_2/backdrop_names.inc"
};

void versus_mech_2_exit(void)
{
    messenger_messages_free();
    fog_stop();
    njFogDisable();
    if (player_hooks.draw_blast_ring == bomb_hat_draw_ring) {
        player_hooks.draw_blast_ring = NULL;
    }
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    particle_core_stop();
    texture_release_shared(versus_mech_2_pvms, versus_mech_2_texture_groups);
    texture_release_shared(backdrop_pvms, backdrop_texture_groups);
    set_end();
    if (versus_mech_2_set) {
        loader_free(versus_mech_2_set);
        versus_mech_2_set = NULL;
    }
    world_land_stop();
}

void versus_mech_2_step(struct task *task)
{
    struct task_work *work = task->work;
    struct camera_bosspoint camera;

    switch (work->mode) {
    case MODE_START_CAMERA:
        camera.target = &work->pos;
        camera.dist = RIVAL_CAMERA_DIST;
        camera.height = RIVAL_CAMERA_HEIGHT;
        camera.part = RIVAL_CAMERA_PART;
        camera.adjust = CAMERA_ADJUST_RELATIVE3;
        camera.collides = 1;
        camera.trigger_push = 0;
        camera.lift = RIVAL_CAMERA_LIFT;
        camera.level = 0;
        camera_bosspoint_start(0, &camera);
        work->mode = MODE_FOLLOW_RIVAL;
        /* fall through */
    case MODE_FOLLOW_RIVAL:
        if (player_work[1]) {
            work->pos = player_work[1]->pos;
            work->pos.y += RIVAL_LIFT;
        }
        break;
    }
}

static void backdrop_step(struct task *task)
{
}

static void backdrop_destroy(struct task *task)
{
}

static void backdrop_draw(struct task *task)
{
    njPushMatrixEx();
    njTranslateEx(&camera_views[camera_current_index]->pos);
    njSetTexture(&earth_texlist);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, earth_lift, 0.0F);
    njScale(NULL, earth_scale, 1.0F, earth_scale);
    njCnkEasyDrawObject(&earth_object);
    matrix_load_parent();
    njSetTexture(&space_outer_texlist);
    njTranslateEx((NJS_VECTOR *)space_outer_object.pos);
    njTranslate(NULL, space_outer_object.model->center.x,
                space_outer_object.model->center.y,
                space_outer_object.model->center.z);
    njRotateX(NULL, (space_outer_spin_x * game_state.steps) + SPACE_TILT);
    njRotateY(NULL, space_outer_spin_y * game_state.steps);
    njTranslate(NULL, -space_outer_object.model->center.x,
                -space_outer_object.model->center.y,
                -space_outer_object.model->center.z);
    njCnkEasyDrawModel(space_outer_object.model);
    matrix_load_parent();
    njSetTexture(&space_inner_texlist);
    njTranslateEx((NJS_VECTOR *)space_inner_object.pos);
    njTranslate(NULL, space_inner_object.model->center.x,
                space_inner_object.model->center.y,
                space_inner_object.model->center.z);
    njRotateX(NULL, (space_inner_spin_x * game_state.steps) + SPACE_TILT);
    njRotateY(NULL, space_inner_spin_y * game_state.steps);
    njTranslate(NULL, -space_inner_object.model->center.x,
                -space_inner_object.model->center.y,
                -space_inner_object.model->center.z);
    njCnkEasyDrawModel(space_inner_object.model);
    njPopMatrix(2);
}
