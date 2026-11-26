/* STAGE_VERSUS_KNUCKLES, the fight of Knuckles and Rouge: its init, exit and
 * step, and its background. the background is the task BgExec, which draws
 * space, two turning shells and a meteor. it then draws uv_land, a land table
 * of two pieces, and the danger model */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "draw/light.h"
#include "draw/texture_anim.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/knuckles.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/flash.h"
#include "shared/fog.h"
#include "shared/item_icon.h"
#include "shared/land_draw.h"
#include "shared/messenger.h"
#include "shared/ring.h"
#include "shared/vertex_colors.h"
#include "sound/adx.h"
#include "stage/stage.h"
#include "stg33/knuckles_rouge.h"
#include "stg33/object_archives.h"
#include "stg33/uv_land.h"
#include "stg33/rival_hunter.h"
#include "system/areas.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"

#include "stg33/knuckles_rouge/text.inc"

static struct fog_setting knuckles_rouge_fog;

struct backdrop_names {
    char et64_3[8];
    char meteo128_3[12];
    char space128_17[12];
    char space128_2[12];
    char space128_3[12];
    char space128_4[12];
    char space128_7[12];
    char space128_9[12];
    char space256_1[12];
    char space256_7[12];
    char space32_4[12];
    char space32_7[12];
    char space512_1[12];
    char space512_2[12];
    char space64_18[12];
    char space64_19[12];
    char space64_20[12];
    char space64_9[12];
    char spacenbg128[12];
    char spacenbg64[12];
    char tx_danger001[16];
    char tx_danger002[16];
    char tx_danger003[16];
    char tx_danger004[16];
    char tx_danger005[16];
    char tx_danger006[16];
    char tx_danger007[16];
    char tx_danger008[16];
    char tx_danger009[16];
    char tx_danger010[16];
    char tx_danger011[16];
    char tx_danger012[16];
    char tx_danger013[13];
};
static const struct backdrop_names backdrop_names;
#include "stg33/backdrop.h"

/* the set file's block has room for at least SET_ENTRIES entries */
#define SET_ENTRIES 512
#define BACKGROUND_LEVEL 1
/* the light both players take */
#define PLAYER_LIGHT 0
/* the light the camera hands Ninja, which light_select_camera sets */
#define CAMERA_LIGHT 0

/* the danger model shows its DANGER_FRAMES textures in turn, one a step, as
 * BgExec's work's timer counts them. plist[DANGER_TID_AT], the second
 * halfword of the model's NJD_CT_TID chunk, holds the texture's index. BgExec's
 * step keeps only the halfword's filter mode, NJD_FFM_MASK, above the index */
#define DANGER_FRAMES 13
#define DANGER_TID_AT 5
/* the angle about x of the shells, space_bg_object and space_bg_2_object,
 * at game_state.steps 0 */
#define SHELL_LEAN 0x3000
/* the matrices background_draw pops at once. it pushes one for the
 * background's height, and one for the shells and the meteor */
#define BACKGROUND_PUSHES 2

/* a piece of uv_land, as BgExec's step animates it. under SCENERY_SCROLL the
 * step scrolls the coordinates of anim's first key by u_speed and v_speed.
 * under SCENERY_PULSE it pulses the color of each of the piece's vertices
 * between pulse_from and pulse_to */
struct scenery {
    Sint32 mode; /* SCENERY_SCROLL or SCENERY_PULSE */
    NJS_CNK_OBJECT *object;
    struct texture_anim *anim;
    Sint32 v_speed;
    Sint32 u_speed;
};
#define SCENERY_SCROLL 0
#define SCENERY_PULSE 1
#define SCENERIES 2

static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);

/* the background's texture lists, which texture_load_shared fills from
 * bgtex33 */
static NJS_TEXLIST *background_group[6] = {
#include "stg33/knuckles_rouge/background_group.inc"
};
static NJS_TEXLIST **background_texture_groups[2] = {
#include "stg33/knuckles_rouge/background_texture_groups.inc"
};
static struct texture_pvm background_pvms[2] = {
#include "stg33/knuckles_rouge/background_pvms.inc"
};
static Sint32 dat_8c537b3c[2] = {0, 0};
static struct light stage_lights[2] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F},
};
/* the set file's block, or null */
static struct set_head *set_file = NULL;
/* the scrolls of uv_land's lower and upper pieces. both sceneries
 * pulse, so neither scroll plays */
static Sint16 lower_scroll_uvs[70] = {
#include "stg33/knuckles_rouge/lower_scroll_uvs.inc"
};
static struct texture_anim lower_scroll = {
#include "stg33/knuckles_rouge/lower_scroll.inc"
};
static Sint16 upper_scroll_uvs[64] = {
#include "stg33/knuckles_rouge/upper_scroll_uvs.inc"
};
static struct texture_anim upper_scroll = {
#include "stg33/knuckles_rouge/upper_scroll.inc"
};
static struct scenery sceneries[SCENERIES] = {
#include "stg33/knuckles_rouge/sceneries.inc"
};
/* a pulsing scenery's color swings between pulse_from and pulse_to as a
 * sine. the sine's angle turns by pulse_speed each step */
static Uint32 pulse_from = 0xff101010;
static Uint32 pulse_to = 0xffffffff;
static Angle pulse_speed = 0x200;
/* a scroll offsets its piece's coordinates by its speeds times scroll_speed
 * times game_state.steps */
static Sint16 scroll_speed = 35;
/* the turns of the shells and of the meteor about x and y, each step */
static Angle space_bg_turn_x = 1;
static Angle space_bg_turn_y = 2;
static Angle space_bg_2_turn_x = 2;
static Angle space_bg_2_turn_y = 4;
static Angle meteor_turn_x = 0;
static Angle meteor_turn_y = 2;
static Float dat_8c537d2c = 1500.0F;
static Float dat_8c537d30 = 1.6F;
/* background_height lifts space_object, the shells and the meteor above
 * the stage's origin. space_rise lifts space_object further. space_scale
 * scales space_object across x and z. meteor_offset moves the meteor along y
 * from its pos */
static Float space_rise = 0.0F;
static Float space_scale = 1.0F;
static Float background_height = 3000.0F;
static Float meteor_offset = -3700.0F;
static Float shell_scale = 1.0F;
static Float meteor_scale = 1.0F;

void knuckles_rouge_init(void)
{
    struct task *task;

    if (stage_character(PLAYER_SLOT) == PLAYER_ROUGE) {
        knuckles_create(RIVAL_SLOT);
    } else if (stage_character(PLAYER_SLOT) == PLAYER_KNUCKLES) {
        rouge_create(RIVAL_SLOT);
    }
    if (player_tasks[RIVAL_SLOT]) {
        stage_place_player(RIVAL_SLOT);
        player_work[RIVAL_SLOT]->id = WORK_ID_RIVAL;
        pad_disable_player(RIVAL_SLOT);
        rival_hunter_create();
    }
    world_land_start(&knuckles_rouge_land);
    texture_load_shared(knuckles_rouge_pvms, knuckles_rouge_texture_groups);
    texture_await_shared(knuckles_rouge_pvms, knuckles_rouge_texture_groups);
    texture_load_shared(background_pvms, background_texture_groups);
    texture_await_shared(background_pvms, background_texture_groups);
    set_file = set_read("set0033_s.bin", "set0033_u.bin", SET_ENTRIES);
    set_start(&knuckles_rouge_objects, set_file);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(PLAYER_LIGHT, &stage_lights[0]);
    light_set(1, &stage_lights[1]);
    light_set(LIGHT_LAND, &stage_lights[0]);
    light_set(3, &stage_lights[0]);
    light_select(PLAYER_LIGHT, PLAYER_SLOT);
    light_select(PLAYER_LIGHT, RIVAL_SLOT);
    light_select_camera(CAMERA_LIGHT);
    light_select_after_draw(LIGHT_LAND);
    world_build_land(&knuckles_rouge_land, (Uint32)STAGE_MODEL_BUILD_AREA);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task) {
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    if (!player_hooks.drop_rings) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (!player_hooks.add_item_icon) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    if (!player_hooks.flash) {
        player_hooks.flash = flash_manager_create_sparks_depth;
    }
    fog_start_file("stg33_fog.bin", &knuckles_rouge_fog);
    njFogDisable();
    messenger_messages_load("mh0033");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    music_fade_in(VOLUME_FULL);
    music_play_or_defer("BOSS_03.ADX");
}

static const struct backdrop_names backdrop_names = {
#include "stg33/knuckles_rouge/backdrop_names.inc"
};

void knuckles_rouge_exit(void)
{
    messenger_messages_free();
    fog_stop();
    njFogDisable();
    if (player_hooks.flash == flash_manager_create_sparks_depth) {
        player_hooks.flash = NULL;
    }
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    particle_core_stop();
    texture_release_shared(knuckles_rouge_pvms, knuckles_rouge_texture_groups);
    texture_release_shared(background_pvms, background_texture_groups);
    set_end();
    if (set_file) {
        loader_free(set_file);
        set_file = NULL;
    }
    world_land_stop();
}

void knuckles_rouge_step(struct task *task)
{
}

static void background_step(struct task *task)
{
    struct task_work *work = task->work;
    struct scenery *scenery;
    Uint32 steps;
    Uint32 color;
    int scroll;
    /* @hack type: i unsigned, where an int would do */
    unsigned int i;
    /* @hack type: model a pointer to a const model, where a plain pointer
     * would do */
    const NJS_CNK_MODEL *model;
    Sint16 texture;

    /* @hack spelling: the steps kept inside the scroll's product, where a
     * statement of their own would do */
    scroll = (Uint16)((steps = game_state.steps) * scroll_speed);
    scenery = sceneries;
    color = color_blend(pulse_from, pulse_to,
                        (njSin((Angle)steps * pulse_speed) * 0.5F) + 0.5F);
    for (i = 0; i < SCENERIES; i++, scenery++) {
        if (scenery->mode == SCENERY_SCROLL) {
            texture_anim_scroll(scenery->object->model, scenery->anim,
                                (scenery->u_speed * scroll) & (SCROLL_WRAP - 1),
                                (scenery->v_speed * scroll) &
                                    (SCROLL_WRAP - 1));
        } else {
            vlist_set_color(scenery->object->model->vlist, color);
        }
    }
    work->timer++;
    if (work->timer >= DANGER_FRAMES) {
        work->timer = 0;
    }
    /* @hack spelling: model assigned inside the read of its texture's
     * halfword, where a statement of its own would do */
    texture = (model = &danger_model)->plist[DANGER_TID_AT];
    texture = (texture & NJD_FFM_MASK) | work->timer;
    model->plist[DANGER_TID_AT] = texture;
}

static void background_destroy(struct task *task)
{
}

static void background_draw(struct task *task)
{
    NJS_CNK_OBJECT *object;
    struct game_state *state;

    njPushMatrixEx();
    njTranslate(NULL, 0.0F, background_height, 0.0F);
    njSetTexture(&space_texlist);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, space_rise, 0.0F);
    njScale(NULL, space_scale, 1.0F, space_scale);
    njCnkEasyDrawObject(&space_object);
    njPopMatrixEx();
    njPushMatrixEx();
    object = &space_bg_object;
    njSetTexture(&space_bg_texlist);
    njTranslateEx((NJS_VECTOR *)object->pos);
    njTranslate(NULL, object->model->center.x, object->model->center.y,
                object->model->center.z);
    /* @hack spelling: state, a local for &game_state, where game_state read
     * in place would do */
    state = &game_state;
    njRotateX(NULL, (space_bg_turn_x * state->steps) + SHELL_LEAN);
    njRotateY(NULL, space_bg_turn_y * state->steps);
    njScale(NULL, shell_scale, shell_scale, shell_scale);
    njTranslate(NULL, -object->model->center.x, -object->model->center.y,
                -object->model->center.z);
    njCnkEasyDrawModel(object->model);
    matrix_load_parent();
    object = &space_bg_2_object;
    njSetTexture(&space_bg_2_texlist);
    njTranslateEx((NJS_VECTOR *)object->pos);
    njTranslate(NULL, object->model->center.x, object->model->center.y,
                object->model->center.z);
    njRotateX(NULL, (space_bg_2_turn_x * state->steps) + SHELL_LEAN);
    njRotateY(NULL, space_bg_2_turn_y * state->steps);
    njScale(NULL, shell_scale, shell_scale, shell_scale);
    njTranslate(NULL, -object->model->center.x, -object->model->center.y,
                -object->model->center.z);
    njCnkEasyDrawModel(object->model);
    matrix_load_parent();
    object = &meteor_object;
    njSetTexture(&meteor_texlist);
    njTranslateEx((NJS_VECTOR *)object->pos);
    njTranslate(NULL, object->model->center.x,
                object->model->center.y + meteor_offset,
                object->model->center.z);
    njRotateX(NULL, meteor_turn_x * state->steps);
    njRotateY(NULL, meteor_turn_y * state->steps);
    njScale(NULL, meteor_scale, meteor_scale, meteor_scale);
    njTranslate(NULL, -object->model->center.x, -object->model->center.y,
                -object->model->center.z);
    njCnkEasyDrawModel(object->model);
    matrix_load_parent();
    njPopMatrix(BACKGROUND_PUSHES);
    land_table_draw(&uv_land);
    njSetTexture(&danger_texlist);
    njCnkEasyDrawObject(&danger_object);
}
