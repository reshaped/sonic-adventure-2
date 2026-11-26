/* Tails's route through Cannon's Core, stg36: the stage's init, exit and
 * step, its background and its floor. the land blocks' task gives camera 0
 * the land block where player 0 stands. the background draws the rotors in
 * every block. it draws each of its other pieces only while camera 0's
 * land_block_mask holds one of that piece's blocks */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "draw/texture_anim.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/black_smoke.h"
#include "shared/bomb_hat.h"
#include "shared/common_object.h"
#include "shared/core_conduit.h"
#include "shared/core_flags.h"
#include "shared/core_machine_lights.h"
#include "shared/core_mech_route.h"
#include "shared/core_sound.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/stage_util.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg36/core_tails.h"
#include "stg36/machinery_textures.h"
#include "stg36/object_archives.h"
#include "shared/core_shutter.h"
#include "stg36/route.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

/* @hack type: camera 0's land_block_mask and game_state.steps reached
 * through dat_8c0db20c, game_state under a second name, where game_state's
 * own fields would do */
#define LAND_MASK STAGE_LAND_MASK(0)
#define STEPS STAGE_FRAME_COUNT
/* the route's last land block, where the floor lies */
#define LAST_BLOCK 9
/* the task levels of the land blocks' task, the background's and the
 * floor's */
#define CLIP_LEVEL 0
#define BACKGROUND_LEVEL 1
#define FLOOR_LEVEL 2
/* set_read_stage reads the stage's two SET files into one block. the block
 * holds at least SET_ENTRIES 32-byte slots, the head's among them */
#define SET_ENTRIES 2048
/* the voice the route starts with, and its priority */
#define START_VOICE 1757
#define VOICE_PRIORITY 0
/* the background shows FIRST_MESSAGE as camera 0's land block changes from
 * BEFORE_FIRST_MESSAGE_BLOCK to FIRST_MESSAGE_BLOCK. it shows LAST_MESSAGE
 * as the block changes from BEFORE_LAST_BLOCK to LAST_BLOCK. each shows only
 * while its core flag is clear, and showing it sets the flag */
#define FIRST_MESSAGE 21
#define FIRST_MESSAGE_FLAG 0
#define BEFORE_FIRST_MESSAGE_BLOCK 2
#define FIRST_MESSAGE_BLOCK 3
#define LAST_MESSAGE 22
#define LAST_MESSAGE_FLAG 1
#define BEFORE_LAST_BLOCK 8
/* after its first step, the stage's task plays a sound at each step while
 * game_state.time_stopped is 0. the sound is DEEP_SOUND while camera 0's land
 * block is DEEP_BLOCK or BELOW_DEEP_BLOCK, the block below it. in any other
 * block it is HUM_SOUND. the task plays INTRO_SOUND too at the first
 * intro_steps such steps */
#define DEEP_SOUND 20
#define HUM_SOUND 21
#define INTRO_SOUND 22
#define DEEP_BLOCK 7
#define BELOW_DEEP_BLOCK 11
/* INTRO_SOUND's key. it isn't the task, so that the volume calls for the
 * task's sounds leave INTRO_SOUND alone */
#define INTRO_KEY(task) ((char *)(task) + 1)
/* the priority of each play, and the time it gives its sound: SOUND_STEPS + 1
 * updates. while game_state.time_stopped isn't 0, a sound ends that long
 * after its last play */
#define SOUND_PRIORITY 1
#define SOUND_STEPS 10
/* the blocks where the background samples the floor below player 0 and draws
 * its glow */
#define GLOW_BLOCKS (LAND_BLOCK(2) | LAND_BLOCK(4) | LAND_BLOCK(6))

/* camera 0's land_block_mask before the land blocks' task's latest step */
static Sint32 previous_mask = 0;
/* the block of the stage's SET files that core_tails_init read, until
 * core_tails_exit frees it */
static struct set_head *route_set = NULL;
/* INTRO_SOUND's volume, and the number of steps that play it */
static Sint32 intro_volume = 25;
static Sint32 intro_steps = 150;
/* the number of steps that have played INTRO_SOUND since core_tails_step */
static Sint32 intro_played = 0;
/* HUM_SOUND's volume swings hum_amplitude either side of hum_center, over
 * hum_period steps. DEEP_SOUND's is deep_volume */
static Sint32 hum_center = 10;
static Sint32 hum_amplitude = 20;
static Sint32 hum_period = 480;
static Sint32 deep_volume = 25;
static void route_step(struct task *task);
static void create_floor(void);
static void floor_step(struct task *task);
static void floor_destroy(struct task *task);
static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);
static void land_clip_step(struct task *task);

void core_tails_init(void)
{
    struct task *background;

    njSetBackColor(0, 0, 0);
    task_create(TASK_WORK, CLIP_LEVEL, land_clip_step, "BgClip");
    world_land_start(&core_tails_land);
    texture_load_shared(core_tails_object_pvms, core_tails_object_groups);
    texture_await_shared(core_tails_object_pvms, core_tails_object_groups);
    texture_load_shared(core_tails_bg_pvms, core_tails_bg_groups);
    texture_await_shared(core_tails_bg_pvms, core_tails_bg_groups);
    route_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&core_tails_objects, route_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &core_mech_lights[0]);
    light_set(1, &core_mech_lights[1]);
    light_set(LIGHT_LAND, &core_mech_lights[0]);
    light_set(3, &core_mech_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg36_light.bin");
    core_shutter_build((Uint8 *)world_build_land(
        &core_tails_land, (Uint32)STAGE_MODEL_BUILD_AREA));
    music_fade_in(0);
    music_play_or_defer("c_core_1.adx");
    path_create_manager(core_mech_paths);
    background =
        task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (background) {
        background->draw = background_draw;
        background->destructor = background_destroy;
    }
    kill_areas_start(core_tails_kill_areas);
    if (!player_hooks.drop_rings) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (!player_hooks.add_item_icon) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    if (!player_hooks.draw_blast_ring) {
        player_hooks.draw_blast_ring = bomb_hat_draw_ring;
    }
    broken_down_smoke_create(0, NULL);
    fog_start_file("stg36_fog.bin", &core_mech_fog);
    njFogDisable();
    messenger_messages_load("mh0036");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    sound_load_stage("se_ac_cc_t.mlt", &core_sound_list);
    game_state.block_grid_loaded = TRUE;
}

void core_tails_exit(void)
{
    hint_box_hints_free();
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
    texture_release_shared(core_tails_object_pvms, core_tails_object_groups);
    texture_release_shared(core_tails_bg_pvms, core_tails_bg_groups);
    set_end();
    if (route_set) {
        loader_free(route_set);
        route_set = NULL;
    }
    world_land_stop();
}

void core_tails_step(struct task *task)
{
    create_floor();
    task->step = route_step;
    core_flags_clear();
    voice_play(START_VOICE, VOICE_PRIORITY);
    intro_played = 0;
}

static void route_step(struct task *task)
{
    Float wave;
    int volume;

    if (game_state.time_stopped == 0) {
        if (game_state.land_block_mask[0] == LAND_BLOCK(DEEP_BLOCK) ||
            game_state.land_block_mask[0] == LAND_BLOCK(BELOW_DEEP_BLOCK)) {
            sound_play_for(DEEP_SOUND, task, SOUND_PRIORITY, 0, SOUND_STEPS);
            sound_set_volume(task, SOUND_ANY_ID, deep_volume);
        } else {
            wave = njSin(game_state.steps * ((Float)FULL_TURN / hum_period));
            volume = (wave * hum_amplitude) + hum_center;
            sound_play_for(HUM_SOUND, task, SOUND_PRIORITY, 0, SOUND_STEPS);
            sound_set_volume(task, SOUND_ANY_ID, volume);
        }
        if (intro_played < intro_steps) {
            ++intro_played;
            sound_play_for(INTRO_SOUND, INTRO_KEY(task), SOUND_PRIORITY, 0,
                           SOUND_STEPS);
            sound_set_volume(INTRO_KEY(task), SOUND_ANY_ID, intro_volume);
        }
    }
}

#include "shared/core_floor_glow.h"

static void background_step(struct task *task)
{
    if (LAND_MASK & GLOW_BLOCKS) {
        sample_floor_glow(task);
    }
    if (!core_flag_is_set(FIRST_MESSAGE_FLAG) &&
        (previous_mask & LAND_BLOCK(BEFORE_FIRST_MESSAGE_BLOCK)) &&
        (LAND_MASK & LAND_BLOCK(FIRST_MESSAGE_BLOCK))) {
        messenger_create(&task->work->pos, 0, 0, FIRST_MESSAGE,
                         MESSENGER_TALK_WINDOW, NULL);
        core_flag_set(FIRST_MESSAGE_FLAG);
    }
    if (!core_flag_is_set(LAST_MESSAGE_FLAG) &&
        (previous_mask & LAND_BLOCK(BEFORE_LAST_BLOCK)) &&
        (LAND_MASK & LAND_BLOCK(LAST_BLOCK))) {
        messenger_create(&task->work->pos, 0, 0, LAST_MESSAGE,
                         MESSENGER_TALK_WINDOW, NULL);
        core_flag_set(LAST_MESSAGE_FLAG);
    }
}

static void background_destroy(struct task *task)
{
    if (CORE_GLOW_FLOOR(task)) {
        syFree(CORE_GLOW_FLOOR(task));
        SET_GLOW_FLOOR(task, 0);
    }
}

#include "shared/core_conduit_draw.h"

/* each model drawn below holds its texture's index in the halfword at
 * TID_HALFWORD of its polygon list. the index takes the halfword's low 13
 * bits. SET_TEXTURE writes texture there. it keeps the filter mode's bits,
 * NJD_FFM_MASK, and clears NJD_FSS */
#define TID_HALFWORD 7
/* @hack spelling: the halfword's address and new value worked out in steps,
 * where one assignment to plist[TID_HALFWORD] would do */
#define SET_TEXTURE(model, texture)                                            \
    {                                                                          \
        Sint16 *halfword = (model)->plist;                                     \
        Sint16 value;                                                          \
                                                                               \
        halfword = halfword + TID_HALFWORD;                                    \
        value = *halfword;                                                     \
        value = value & NJD_FFM_MASK;                                          \
        value = value | (texture);                                             \
        *halfword = value;                                                     \
    }
/* SHAPE_FRAME(motion) is the frame of motion to draw now. the motion plays a
 * frame every SHAPE_STEPS steps, and starts again after its last */
#define SHAPE_STEPS 2
#define SHAPE_FRAME(motion)                                                    \
    ((Float)(STEPS % ((motion)->nbFrame * SHAPE_STEPS)) * (1.0F / SHAPE_STEPS))
/* the joints' constant material, JOINT_MATERIAL in alpha and in each color */
#define JOINT_MATERIAL 0.2F

static void draw_scrolling_panel(NJS_CNK_OBJECT *panel,
                                 struct texture_anim *anim, int texture,
                                 Sint32 blocks)
{
    if (LAND_MASK & blocks) {
        SET_TEXTURE(panel->model, texture);
        njPushMatrixEx();
        njTranslate(NULL, panel->pos[0], panel->pos[1], panel->pos[2]);
        njRotateXYZ(NULL, panel->ang[0], panel->ang[1], panel->ang[2]);
        texture_anim_apply(panel->model, anim, STEPS);
        njCnkEasyDrawModel(panel->model);
        njPopMatrixEx();
    }
}

static void draw_joint(NJS_CNK_OBJECT *joint, NJS_MOTION *motion,
                       NJS_MOTION *shape, int texture, Sint32 blocks)
{
    if (LAND_MASK & blocks) {
        CONDUIT_ADD_BEGIN();
        constant_material_set(JOINT_MATERIAL, JOINT_MATERIAL, JOINT_MATERIAL,
                              JOINT_MATERIAL);
        SET_TEXTURE(joint->model, texture);
        njPushMatrixEx();
        njTranslate(NULL, joint->pos[0], joint->pos[1], joint->pos[2]);
        njRotateXYZ(NULL, joint->ang[0], joint->ang[1], joint->ang[2]);
        shape_motion_draw_tinted(joint, motion, shape, SHAPE_FRAME(motion),
                                 COLOR_HALF);
        njPopMatrixEx();
        CONDUIT_ADD_END();
    }
}

/* draw object's shape motion while camera 0's land_block_mask holds one of
 * blocks. no code calls it, but the image holds it. it is external because
 * SHC drops a static that nothing calls */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void core_tails_draw_shape(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                           NJS_MOTION *shape, Sint32 blocks)
{
    if (LAND_MASK & blocks) {
        njPushMatrixEx();
        njTranslate(NULL, object->pos[0], object->pos[1], object->pos[2]);
        njRotateXYZ(NULL, object->ang[0], object->ang[1], object->ang[2]);
        njCnkEasyDrawShapeMotion(object, motion, shape, SHAPE_FRAME(motion));
        njPopMatrixEx();
    }
}

#define DRAW_TINTED_SHAPE(tint)                                                \
    CONDUIT_ADD_BEGIN();                                                       \
    njPushMatrixEx();                                                          \
    njTranslate(NULL, object->pos[0], object->pos[1], object->pos[2]);         \
    njRotateXYZ(NULL, object->ang[0], object->ang[1], object->ang[2]);         \
    shape_motion_draw_tinted(object, motion, shape, frame, tint);              \
    njPopMatrixEx();                                                           \
    CONDUIT_ADD_END()

static void draw_pipe(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                      NJS_MOTION *shape, Float frame, Sint32 blocks)
{
    if (LAND_MASK & blocks) {
        DRAW_TINTED_SHAPE(tails_pipe_tint);
    }
}

/* @hack type: color an int, where Uint32, as shape_motion_draw_tinted takes
 * it, would do */
static void draw_shape_color(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                             NJS_MOTION *shape, Float frame, int color,
                             Sint32 blocks)
{
    if (LAND_MASK & blocks) {
        DRAW_TINTED_SHAPE(color);
    }
}
#undef DRAW_TINTED_SHAPE

/* with CORE_SECOND_LAMP_GROUP defined, core_strip_lights.h defines
 * draw_entrance_lights beside draw_strip_lights, to draw
 * tails_entrance_lamps */
#define CORE_SECOND_LAMP_GROUP
#include "shared/core_strip_lights.h"

#include "shared/core_energy_streams.h"

#include "stg36/panels.h"

/* the panels show PANEL_FRAMES textures of tails_panel_texlist forward and
 * back, PANEL_CYCLE to a cycle, each for PANEL_STEPS steps */
#define PANEL_FRAMES 5
#define PANEL_CYCLE ((PANEL_FRAMES - 1) * 2)
#define PANEL_STEPS 2
/* the pipes lie in PIPE_ROWS rows, PIPE_ROW_GAP apart in y. PLACE_PIPE takes
 * the y between the rows, their joint's height. each row draws its pipes
 * PIPE_PASSES times. the draws' motions run PIPE_PHASE of a cycle apart, and
 * play PIPE_RATE frames a step */
#define PIPE_ROWS 2
#define PIPE_PASSES 2
#define PIPE_PHASE (1.0F / (PIPE_ROWS * PIPE_PASSES))
#define PIPE_ROW_GAP 62.0F
#define PIPE_RATE 0.375F
/* the bounding radius, r, that the pipe's model takes before each draw */
#define PIPE_MODEL_RADIUS 300.0F
/* the glow is a disc of GLOW_SECTORS sectors. shared/core_floor_draw.inc
 * gives each sector SECTOR_VERTICES: the center and two rim vertices. it ends
 * with the center again, GLOW_VERTICES in all, and draws them under
 * GLOW_CONTROL. the rim lies RIM_X_SCALE as far out along x as along z */
#define GLOW_SECTORS 32
#define SECTOR_VERTICES 3
#define GLOW_VERTICES ((GLOW_SECTORS * SECTOR_VERTICES) + 1)
#define GLOW_CONTROL (NJD_CONTROL_3D_SHADOW | NJD_CONTROL_3D_TRANS_MODIFIER)
#define RIM_X_SCALE 0.9F
/* the background draws two rotors at each rotor placement, the second
 * ROTOR_DROP below the first. the second's motion runs 1 / ROTOR_LEAD of its
 * length ahead of the first's */
#define ROTOR_DROP 28.0F
#define ROTOR_LEAD 4
/* the curved conduits' arcs, in degrees, and their streams. the second's arc
 * is negative, which sends its streams the other way */
#define FIRST_CONDUIT_ARC 90
#define FIRST_CONDUIT_STREAMS 3
#define SECOND_CONDUIT_ARC (-180)
#define SECOND_CONDUIT_STREAMS 4

/* @hack spelling: the joints' texture index returned by a function and held
 * in a local, where 0 at each draw_joint would do */
static int joint_texture_index(void)
{
    return 0;
}

static Float glow_uv(Float offset)
{
    return core_glow_uv_scale * offset;
}

#define DRAW_PANEL(n, blocks)                                                  \
    draw_scrolling_panel(&tails_panel_##n, &tails_panel_uv_##n, panel_texture, \
                         blocks)

/* @hack spelling: panels 1 to 11 in an inline function, where DRAW_PANEL
 * lines at the call would do */
#pragma inline(draw_first_panels)
static void draw_first_panels(int panel_texture)
{
    DRAW_PANEL(1, LAND_BLOCK(1));
    DRAW_PANEL(2, LAND_BLOCK(1));
    DRAW_PANEL(3, LAND_BLOCK(1));
    DRAW_PANEL(4, LAND_BLOCKS(1, 2));
    DRAW_PANEL(5, LAND_BLOCKS(1, 2));
    DRAW_PANEL(6, LAND_BLOCKS(4, 5));
    DRAW_PANEL(7, LAND_BLOCK(5));
    DRAW_PANEL(8, LAND_BLOCK(5) | LAND_BLOCK(10));
    DRAW_PANEL(9, LAND_BLOCKS(5, 6) | LAND_BLOCK(10));
    DRAW_PANEL(10, LAND_BLOCKS(5, 6) | LAND_BLOCK(10));
    DRAW_PANEL(11, LAND_BLOCKS(5, 6) | LAND_BLOCK(10));
}

/* @hack spelling: panel 16 in an inline function, where a DRAW_PANEL line at
 * the call would do */
#pragma inline(draw_lower_panel)
static void draw_lower_panel(int panel_texture)
{
    DRAW_PANEL(16, LAND_BLOCK(10));
}

static void background_draw(struct task *task)
{
    NJS_POINT3 pos;
    NJS_TEXTURE_VTX vertices[GLOW_VERTICES];
    NJS_TEXTURE_VTX *vertex;
    struct task_work *work = task->work;
    struct surface_side *floor;
    NJS_CNK_OBJECT *rotor;
    NJS_MOTION *motion;
    Uint32 color;
    Angle angle;
    Angle pitch;
    Angle roll;
    int panel_texture;
    int panel_frame;
    int alpha;
    int row;
    int pass;
    int phase;
    int sector;
    int vertex_index;
    int frames;
    Float frame;
    Float sine;
    Float cosine;
    Float origin_u;
    Float origin_v;
    Float rate;
    Float period;
    int joint_texture;
    /* shared/core_floor_draw.inc writes it for a hack of its own, and
     * nothing reads it */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    int unused_clock;

    panel_frame = (game_state.steps / PANEL_STEPS) % PANEL_CYCLE;
    panel_texture =
        panel_frame < PANEL_FRAMES ? panel_frame : PANEL_CYCLE - panel_frame;
    njSetTexture(&tails_panel_texlist);
    draw_first_panels(panel_texture);
    DRAW_PANEL(12, LAND_BLOCKS(1, 3));
    DRAW_PANEL(13, LAND_BLOCKS(1, 3));
    DRAW_PANEL(14, LAND_BLOCKS(4, 5));
    DRAW_PANEL(15, LAND_BLOCKS(5, 6) | LAND_BLOCK(10));
    draw_lower_panel(panel_texture);
    DRAW_PANEL(17, LAND_BLOCKS(1, 3));
    DRAW_PANEL(18, LAND_BLOCKS(1, 3));
    DRAW_PANEL(19, LAND_BLOCK(6) | LAND_BLOCK(10));
    DRAW_PANEL(20, LAND_BLOCK(6) | LAND_BLOCK(10));
    DRAW_PANEL(21, LAND_BLOCK(8) | LAND_BLOCK(11));
    DRAW_PANEL(22, LAND_BLOCK(8) | LAND_BLOCK(11));
    DRAW_PANEL(23, LAND_BLOCKS(1, 4));
    DRAW_PANEL(24, LAND_BLOCKS(1, 4));
    DRAW_PANEL(25, LAND_BLOCKS(2, 4));
    DRAW_PANEL(26, LAND_BLOCKS(3, 4));
    DRAW_PANEL(27, LAND_BLOCKS(8, 9) | LAND_BLOCK(11));
    DRAW_PANEL(28, LAND_BLOCKS(8, 9) | LAND_BLOCK(11));
    DRAW_PANEL(29, LAND_BLOCKS(1, 4));
    DRAW_PANEL(30, LAND_BLOCKS(2, 4));
    DRAW_PANEL(31, LAND_BLOCKS(8, 9) | LAND_BLOCK(11));
    DRAW_PANEL(32, LAND_BLOCKS(8, 9));
    DRAW_PANEL(33, LAND_BLOCKS(1, 3));
    DRAW_PANEL(34, LAND_BLOCKS(1, 4));
    DRAW_PANEL(35, LAND_BLOCKS(1, 4));
    DRAW_PANEL(36, LAND_BLOCKS(3, 4));
    DRAW_PANEL(37, LAND_BLOCKS(3, 4));
    DRAW_PANEL(38, LAND_BLOCKS(4, 5));
    DRAW_PANEL(39, LAND_BLOCK(6) | LAND_BLOCK(10));
    DRAW_PANEL(40, LAND_BLOCKS(6, 7));
    DRAW_PANEL(41, LAND_BLOCKS(6, 7));
    DRAW_PANEL(42, LAND_BLOCK(8) | LAND_BLOCK(11));
    DRAW_PANEL(43, LAND_BLOCK(8) | LAND_BLOCK(11));
    DRAW_PANEL(44, LAND_BLOCKS(8, 9));
    DRAW_PANEL(45, LAND_BLOCK(5));
#undef DRAW_PANEL
    joint_texture = joint_texture_index();
    /* @hack redundant: a goto to the next line */
    goto joints;
joints:
    njSetTexture(&tails_joint_texlist);
#define PIPE_STRAIGHT
#define PIPE_TURN njRotateY(NULL, QUARTER_TURN);
#define PLACE_JOINT(part, x, y, z, heading, blocks)                            \
    njPushMatrixEx();                                                          \
    njTranslate(NULL, x, y, z);                                                \
    heading draw_joint(&tails_##part, &tails_##part##_motion,                  \
                       &tails_##part##_shape, joint_texture, blocks);          \
    njPopMatrixEx()
    if (game_state.land_block_mask[0] & LAND_BLOCKS(1, 3)) {
        PLACE_JOINT(joint, 0.0F, 0.0F, -250.2F, PIPE_STRAIGHT,
                    LAND_BLOCKS(1, 3));
    }
    if (game_state.land_block_mask[0] & (LAND_BLOCK(6) | LAND_BLOCK(10))) {
        PLACE_JOINT(joint, -720.0F, -608.0F, -1170.2F, PIPE_STRAIGHT,
                    LAND_BLOCK(6) | LAND_BLOCK(10));
    }
    if (game_state.land_block_mask[0] & (LAND_BLOCK(8) | LAND_BLOCK(11))) {
        PLACE_JOINT(joint, -1740.2F, -1308.0F, -1540.0F, PIPE_TURN,
                    LAND_BLOCK(8) | LAND_BLOCK(11));
    }
    if (game_state.land_block_mask[0] & LAND_BLOCKS(8, 9)) {
        PLACE_JOINT(outlet, -1991.76F, -1242.25F, -2084.41F, PIPE_STRAIGHT,
                    LAND_BLOCK(9));
        PLACE_JOINT(elbow, -1991.76F, -1300.25F, -2441.41F, PIPE_STRAIGHT,
                    LAND_BLOCKS(8, 9));
        PLACE_JOINT(end_joint, -1992.0F, -1238.0F, -2645.0F, PIPE_STRAIGHT,
                    LAND_BLOCKS(8, 9));
    }
#undef PLACE_JOINT
#define PLACE_PIPE(x, y, z, heading, blocks)                                   \
    if (game_state.land_block_mask[0] & (blocks)) {                            \
        njPushMatrixEx();                                                      \
        njTranslate(NULL, x,                                                   \
                    (row * PIPE_ROW_GAP) + ((y) - (PIPE_ROW_GAP / 2)), z);     \
        heading draw_pipe(tails_pipe, motion, &tails_pipe_shape, frame,        \
                          blocks);                                             \
        njPopMatrixEx();                                                       \
    }
    njSetTexture(&tails_pipe_texlist);
    for (row = 0, motion = tails_pipe_motion; row < PIPE_ROWS; ++row) {
        /* @hack spelling: phase = phase + PIPE_ROWS, where phase += PIPE_ROWS
         * would do */
        for (pass = 0, phase = row; pass < PIPE_PASSES;
             ++pass, phase = phase + PIPE_ROWS) {
            rate = PIPE_RATE;
            frames = motion->nbFrame;
            /* @hack redundant: period, which nothing reads, set inside the
             * expression that sets frame */
            frame = (int)(game_state.steps +
                          (phase * (period = frames / rate) * PIPE_PHASE)) %
                    (int)(frames / rate) * rate;
            tails_pipe->model->r = PIPE_MODEL_RADIUS;
            PLACE_PIPE(0.0F, 0.0F, -150.0F, PIPE_STRAIGHT, LAND_BLOCKS(1, 3));
            PLACE_PIPE(-720.0F, -608.0F, -1070.0F, PIPE_STRAIGHT,
                       LAND_BLOCK(6) | LAND_BLOCK(10));
            PLACE_PIPE(-1640.0F, -1308.0F, -1540.0F, PIPE_TURN,
                       LAND_BLOCK(8) | LAND_BLOCK(11));
        }
    }
#undef PLACE_PIPE
#undef PIPE_TURN
#undef PIPE_STRAIGHT
    if ((game_state.land_block_mask[0] & GLOW_BLOCKS) &&
        CORE_GLOW_FLOOR(task) && player_work[0]) {
#define CORE_TAILS_FLOOR
#define GLOW_SECTOR_INDEX sector
#define GLOW_ROLL(roll) njRotateZ(NULL, roll)
#define GLOW_TEXLIST tails_glow_texlist
/* @hack spelling: sine = sine * RIM_X_SCALE, where sine *= RIM_X_SCALE
 * would do */
#define GLOW_NARROW_RIM sine = sine * RIM_X_SCALE
#include "shared/core_floor_draw.inc"
#undef GLOW_ROLL
#undef GLOW_TEXLIST
#undef GLOW_NARROW_RIM
#undef GLOW_SECTOR_INDEX
#undef CORE_TAILS_FLOOR
    }
    njPushMatrixEx();
    /* @hack spelling: rotor set before njSetTexture, where the loop's own
     * init would do */
    rotor = tails_rotor_placements->child;
    njSetTexture(&tails_rotor_texlist);
    for (; rotor; rotor = rotor->sibling) {
        njPushMatrixEx();
        njTranslate(NULL, rotor->pos[0], rotor->pos[1], rotor->pos[2]);
        njRotateXYZ(NULL, rotor->ang[0], rotor->ang[1], rotor->ang[2]);
        njCnkEasyDrawMotion(&tails_rotor, tails_rotor_motion,
                            game_state.steps % tails_rotor_motion->nbFrame);
        njPopMatrixEx();
    }
    njTranslate(NULL, 0.0F, -ROTOR_DROP, 0.0F);
    for (rotor = tails_rotor_placements->child; rotor; rotor = rotor->sibling) {
        njPushMatrixEx();
        njTranslate(NULL, rotor->pos[0], rotor->pos[1], rotor->pos[2]);
        njRotateXYZ(NULL, rotor->ang[0], rotor->ang[1], rotor->ang[2]);
        njCnkEasyDrawMotion(
            &tails_rotor, tails_rotor_motion,
            (game_state.steps + (tails_rotor_motion->nbFrame / ROTOR_LEAD)) %
                tails_rotor_motion->nbFrame);
        njPopMatrixEx();
    }
    njPopMatrixEx();
    njSetTexture(&tails_strip_texlist);
#define PLACE_LAMPS(draw, x, y, z, blocks)                                     \
    if (game_state.land_block_mask[0] & (blocks)) {                            \
        njPushMatrixEx();                                                      \
        njTranslate(NULL, x, y, z);                                            \
        draw(task);                                                            \
        njPopMatrixEx();                                                       \
    }
    PLACE_LAMPS(draw_entrance_lights, 0.0F, 803.8F, 0.0F, LAND_BLOCK(1));
    PLACE_LAMPS(draw_entrance_lights, 0.0F, 46.0F, 0.0F, LAND_BLOCKS(1, 2));
    PLACE_LAMPS(draw_strip_lights, -0.024F, 46.0F, -499.639F,
                LAND_BLOCKS(2, 3));
    PLACE_LAMPS(draw_strip_lights, -250.0F, 46.0F, -500.0F, LAND_BLOCKS(3, 4));
    PLACE_LAMPS(draw_strip_lights, -1990.0F, -1262.0F, -1540.0F,
                LAND_BLOCK(8) | LAND_BLOCK(11));
#undef PLACE_LAMPS
    if (game_state.land_block_mask[0] & LAND_BLOCKS(3, 5)) {
        njPushMatrixEx();
        njTranslate(NULL, -450.0F, -30.0F, -770.0F);
        draw_streams(task, FIRST_CONDUIT_ARC, FIRST_CONDUIT_STREAMS,
                     LAND_BLOCKS(3, 5));
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] & (LAND_BLOCK(6) | LAND_BLOCK(10))) {
        njPushMatrixEx();
        njTranslate(NULL, -990.0F, -638.0F, -1270.0F);
        njRotateY(NULL, -QUARTER_TURN);
        draw_streams(task, SECOND_CONDUIT_ARC, SECOND_CONDUIT_STREAMS,
                     LAND_BLOCK(6) | LAND_BLOCK(10));
        njPopMatrixEx();
    }
}

/* the bounds on player 0's position that land_clip_step parts the land blocks
 * by. END_Z is where a block ends toward -z, WEST_X its side toward -x and
 * BOTTOM_Y its bottom. blocks 10 and 11 lie below blocks 5 and 7.
 * LAST_BLOCK takes in the route past LAST_BLOCK_START_Z, toward -z */
#define BLOCK_1_END_Z (-100.0F)
#define BLOCK_2_END_Z (-400.0F)
#define BLOCK_3_WEST_X (-450.0F)
#define BLOCK_4_END_Z (-770.0F)
#define BLOCK_5_END_Z (-1020.0F)
#define BLOCK_5_BOTTOM_Y (-487.0F)
#define BLOCK_6_WEST_X (-1090.0F)
#define BLOCK_7_WEST_X (-1890.0F)
#define BLOCK_7_BOTTOM_Y (-1200.0F)
#define LAST_BLOCK_START_Z (-2140.0F)

/* give camera 0 the one land block where player 0 stands. a NaN coordinate
 * passes each !(a < b) test, as in the image. it would fail a >= b */
static void land_clip_step(struct task *task)
{
    struct task_work *player = player_work[0];

    previous_mask = LAND_MASK;
    /* @bug memory: reads player 0's pos through null where player_work[0] is
     * null. background_draw tests that pointer at 0x8c5021b4, and
     * sample_floor_glow at 0x8c500588. this step reads pos.z untested at
     * 0x8c50283a */
#ifdef NONMATCHING
    /* this side repairs the tag above: while slot 0 holds no player, the step
     * leaves the land mask as it was */
    if (player == NULL) {
        return;
    }
#endif
    if (player->pos.z < LAST_BLOCK_START_Z) {
        LAND_MASK = LAND_BLOCK(LAST_BLOCK);
    } else if (!(player->pos.z < BLOCK_1_END_Z)) {
        LAND_MASK = LAND_BLOCK(1);
    } else if (!(player->pos.z < BLOCK_2_END_Z)) {
        LAND_MASK = LAND_BLOCK(2);
    } else if (!(player->pos.x < BLOCK_3_WEST_X)) {
        LAND_MASK = LAND_BLOCK(3);
    } else if (!(player->pos.z < BLOCK_4_END_Z)) {
        LAND_MASK = LAND_BLOCK(4);
    } else if (!(player->pos.z < BLOCK_5_END_Z)) {
        if (!(player->pos.y < BLOCK_5_BOTTOM_Y)) {
            LAND_MASK = LAND_BLOCK(5);
        } else {
            LAND_MASK = LAND_BLOCK(10);
        }
    } else if (!(player->pos.x < BLOCK_6_WEST_X)) {
        LAND_MASK = LAND_BLOCK(6);
    } else if (!(player->pos.x < BLOCK_7_WEST_X)) {
        if (!(player->pos.y < BLOCK_7_BOTTOM_Y)) {
            LAND_MASK = LAND_BLOCK(7);
        } else {
            LAND_MASK = LAND_BLOCK(11);
        }
    } else {
        LAND_MASK = LAND_BLOCK(8);
    }
}

/* where the floor lies, in LAST_BLOCK */
#define FLOOR_X (-1991.0F)
#define FLOOR_Y (-1229.0F)
#define FLOOR_Z (-2500.0F)

static void create_floor(void)
{
    NJS_OBJECT *object = world_pool_alloc();
    struct task *task;
    struct task_work *work;

#ifdef NONMATCHING
    /* this side makes no floor where world_pool_alloc lends no object. the
     * matching side writes through the null */
    if (!object) {
        return;
    }
#endif
    task = task_create(TASK_WORK, FLOOR_LEVEL, floor_step, "BgColiExec");
#ifdef NONMATCHING
    /* where task_create fails, this side gives the object back to the pool,
     * which the matching side leaves lent */
    if (!task) {
        world_pool_free(object);
        return;
    }
#endif
    if (task) {
        task->destructor = floor_destroy;
        work = task->work;
        work->pos.x = FLOOR_X;
        work->pos.y = FLOOR_Y;
        work->pos.z = FLOOR_Z;
        *object = tails_floor_template;
        task->extra.p = object;
        object->evalflags &= ~NJD_EVAL_UNIT_POS;
        object->pos[0] = work->pos.x;
        object->pos[1] = work->pos.y;
        object->pos[2] = work->pos.z;
        world_add(WORLD_SOLID, task, object);
#ifdef NONMATCHING
        /* this side kills the floor and gives its object back where world_add
         * didn't add it */
        if (!(work->flags & WORLD_ADDED)) {
            task->extra.p = NULL;
            world_pool_free(object);
            task_kill(task);
        }
#endif
    }
}

static void floor_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

/* world_collect_near_players passes over the floor while its work lacks
 * WORLD_ADDED */
static void floor_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!game_state.time_stopped &&
        (game_state.land_block_mask[0] & LAND_BLOCK(LAST_BLOCK))) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}
