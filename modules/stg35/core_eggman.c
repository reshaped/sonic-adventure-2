/* Eggman's route through Cannon's Core, stg35: the stage's init, exit and
 * step, and its background. the land blocks' task gives camera 0 the land
 * block where player 0 stands. the background draws each piece of the
 * machinery while camera 0's land_block_mask holds one of its blocks. it also
 * draws a glow on the floor below player 0 */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/color.h"
#include "draw/light.h"
#include "draw/texture_anim.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/big_splash.h"
#include "shared/black_smoke.h"
#include "shared/bomb_hat.h"
#include "shared/common_object.h"
#include "shared/core_conduit.h"
#include "shared/core_flags.h"
#include "shared/core_machine_lights.h"
#include "shared/core_mech_route.h"
#include "shared/core_scenery_art.h"
#include "shared/core_sound.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/stage_util.h"
#include "shared/water_ripple.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg35/core_eggman.h"
#include "stg35/machinery_textures.h"
#include "stg35/object_archives.h"
#include "stg35/layout.h"
#include "stg35/machinery.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* @hack type: camera 0's land_block_mask read as game_state_land_block_mask_0,
 * where game_state.land_block_mask[0] would do */
#define BLOCK_MASK game_state_land_block_mask_0
/* @hack type: camera 0's land_block_mask and game_state.steps reached
 * through dat_8c0db20c, where game_state's own fields would do */
#define LAND_MASK STAGE_LAND_MASK(0)
#define STEPS STAGE_FRAME_COUNT
/* the task levels of the land blocks' task, the background's and the
 * barriers' */
#define CLIP_LEVEL 0
#define BACKGROUND_LEVEL 1
#define BARRIER_LEVEL 2
/* set_read_stage reads the stage's two SET files into one block. the block
 * holds at least SET_ENTRIES 32-byte slots, the head's among them */
#define SET_ENTRIES 2048
/* START_MESSAGE shows at the stage task's first step if camera 0's
 * land_block_mask holds START_BLOCK. LAST_MESSAGE shows when the land blocks'
 * task moves the mask from BEFORE_LAST_BLOCK to LAST_BLOCK. it shows only
 * while the core flag LAST_MESSAGE_FLAG is clear, and showing it sets the
 * flag */
#define START_MESSAGE 21
#define START_BLOCK 1
#define LAST_MESSAGE 22
#define LAST_MESSAGE_FLAG 1
#define BEFORE_LAST_BLOCK 18
#define LAST_BLOCK 19
/* after its first step, the stage's task plays HUM_SOUND, whose key is the
 * task. it plays it again each step while game_state.time_stopped is 0. each
 * play gives the sound a time of SOUND_STEPS + 1 updates. once time stops, the
 * sound ends that long after the last play. its volume swings
 * eggman_hum_amplitude either side of eggman_hum_center over eggman_hum_period
 * steps */
#define HUM_SOUND 21
#define SOUND_PRIORITY 1
#define SOUND_STEPS 10
/* the background samples the floor below player 0 while camera 0's
 * land_block_mask holds one of GLOW_SAMPLE_BLOCKS. it draws the glow in
 * GLOW_BLOCKS. in a block it doesn't sample, the glow stays where the last
 * sample left it */
#define GLOW_SAMPLE_BLOCKS (LAND_BLOCK(1) | LAND_BLOCK(8) | LAND_BLOCK(10))
#define GLOW_BLOCKS (LAND_BLOCK(1) | LAND_BLOCKS(8, 10) | LAND_BLOCKS(18, 19))
/* the near and far clip planes init gives Ninja */
#define CLIP_NEAR (-3.0F)
#define CLIP_FAR (-50000.0F)

/* camera 0's land_block_mask before the land blocks' task's latest step */
static Sint32 previous_mask = 0;
/* the block of the stage's SET files that core_eggman_init read, until
 * core_eggman_exit frees it */
static struct set_head *route_set = NULL;
static void hum_step(struct task *stage);
static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);
static void land_clip_step(struct task *task);

void core_eggman_init(void)
{
    struct task *background;

    njSetBackColor(0, 0, 0);
    task_create(TASK_WORK, CLIP_LEVEL, land_clip_step, "BgClip");
    world_land_start(&core_eggman_land);
    texture_load_shared(core_eggman_object_pvms, core_eggman_object_groups);
    texture_await_shared(core_eggman_object_pvms, core_eggman_object_groups);
    texture_load_shared(core_eggman_bg_pvms, core_eggman_bg_groups);
    texture_await_shared(core_eggman_bg_pvms, core_eggman_bg_groups);
    route_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&core_eggman_objects, route_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &core_mech_lights[0]);
    light_set(1, &core_mech_lights[1]);
    light_set(2, &core_mech_lights[0]);
    light_set(3, &core_mech_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg35_light.bin");
    world_build_land(&core_eggman_land, (Uint32)STAGE_MODEL_BUILD_AREA);
    music_fade_in(0);
    music_play_or_defer("c_core_1.adx");
    path_create_manager(core_mech_paths);
    background =
        task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (background) {
        background->draw = background_draw;
        background->destructor = background_destroy;
    }
    kill_areas_start(core_eggman_kill_areas);
    if (!player_hooks.drop_rings) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (!player_hooks.add_item_icon) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    if (!player_hooks.ripple) {
        player_hooks.ripple = ripple_create;
    }
    if (!player_hooks.splash) {
        player_hooks.splash = big_splash_create;
    }
    big_splash_load_texture();
    if (!player_hooks.draw_blast_ring) {
        player_hooks.draw_blast_ring = bomb_hat_draw_ring;
    }
    broken_down_smoke_create(0, NULL);
    fog_start_file("stg35_fog.bin", &core_mech_fog);
    njFogDisable();
    object_light_create();
    messenger_messages_load("mh0035");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    njClipZ(CLIP_NEAR, CLIP_FAR);
    sound_load_stage("se_ac_cc_e.mlt", &core_sound_list);
    game_state.block_grid_loaded = TRUE;
}

void core_eggman_exit(void)
{
    hint_box_hints_free();
    messenger_messages_free();
    object_light_destroy();
    fog_stop();
    njFogDisable();
    if (player_hooks.draw_blast_ring == bomb_hat_draw_ring) {
        player_hooks.draw_blast_ring = NULL;
    }
    if (player_hooks.splash == big_splash_create) {
        player_hooks.splash = NULL;
    }
    big_splash_release_texture();
    if (player_hooks.ripple == ripple_create) {
        player_hooks.ripple = NULL;
    }
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    particle_core_stop();
    texture_release_shared(core_eggman_object_pvms, core_eggman_object_groups);
    texture_release_shared(core_eggman_bg_pvms, core_eggman_bg_groups);
    set_end();
    if (route_set) {
        loader_free(route_set);
        route_set = NULL;
    }
    world_land_stop();
}

static void barrier_step(struct task *task)
{
    collision_register(task);
}

#ifdef NONMATCHING
/* whether any barrier's position or heading is other than 0. a layout of all
 * zeros places no barriers. a real layout may still hold a 0 among them */
static Bool route_has_barriers(void)
{
    int i;

    for (i = 0; i < EGG_ROUTE_BARRIERS; i++) {
        if (eggman_barrier_positions[i].x != 0.0F ||
            eggman_barrier_positions[i].y != 0.0F ||
            eggman_barrier_positions[i].z != 0.0F ||
            eggman_barrier_headings[i] != 0) {
            return TRUE;
        }
    }
    return FALSE;
}
#endif

void core_eggman_step(struct task *stage)
{
    /* @hack spelling: stage copied into task, where stage itself would do */
    struct task *task = stage;
    int i;
    struct task *barrier;

#ifdef NONMATCHING
    /* this side places no barriers for a layout of all zeros, as a build
     * without the disc's data has */
    if (route_has_barriers())
#endif
    {
        for (i = 0; i < EGG_ROUTE_BARRIERS; ++i) {
            barrier =
                task_create(TASK_WORK, BARRIER_LEVEL, barrier_step, "BgColi");
#ifdef NONMATCHING
            /* this side skips a barrier where task_create or collision_init
             * fails */
            if (barrier == NULL) {
                continue;
            }
            if (!collision_init(barrier, eggman_barrier_walls,
                                EGG_BARRIER_WALLS, COLLISION_OBJECT)) {
                task_destroy(barrier);
                continue;
            }
#else
            collision_init(barrier, eggman_barrier_walls, EGG_BARRIER_WALLS,
                           COLLISION_OBJECT);
#endif
            barrier->work->pos = eggman_barrier_positions[i];
            barrier->work->ang.y = eggman_barrier_headings[i];
        }
    }
    core_flags_clear();
    if (BLOCK_MASK & LAND_BLOCK(START_BLOCK)) {
        messenger_create(&task->work->pos, 0, 0, START_MESSAGE,
                         MESSENGER_TALK_WINDOW, NULL);
    }
    task->step = hum_step;
}

static void hum_step(struct task *stage)
{
    /* @hack spelling: stage copied into task, where stage itself would do */
    struct task *task = stage;
    Float wave;
    int volume;

    if (game_state.time_stopped == 0) {
        wave = njSin(game_state.steps * ((Float)FULL_TURN / eggman_hum_period));
        volume = (wave * eggman_hum_amplitude) + eggman_hum_center;
        sound_play_for(HUM_SOUND, task, SOUND_PRIORITY, 0, SOUND_STEPS);
        sound_set_volume(task, SOUND_ANY_ID, volume);
    }
}

#include "shared/core_floor_glow.h"
#include "shared/core_strip_lights.h"

static void draw_lamp(NJS_CNK_OBJECT *lamp, Uint32 color)
{
    njPushMatrixEx();
    njTranslate(NULL, lamp->pos[0], lamp->pos[1], lamp->pos[2]);
    njRotateXYZ(NULL, lamp->ang[0], lamp->ang[1], lamp->ang[2]);
    model_draw_tinted_additive(lamp->model, color);
    njPopMatrixEx();
}

/* a lamp's alpha runs from 0 to FULL_ALPHA. its tint is eggman_lamp_red,
 * eggman_lamp_green and eggman_lamp_blue scaled by the alpha */
#define FULL_ALPHA 255.0F

/* lamps[0] to lamps[count - 1] light in turn, each eggman_lamp_delay steps
 * after the one before. each brightens over eggman_lamp_rise steps and fades
 * over eggman_lamp_fall. the first lights again in the step the last goes
 * out */
static void draw_lamp_sequence(NJS_CNK_OBJECT **lamps, int count)
{
    int frame = game_state.steps % ((eggman_lamp_delay * (count - 1)) +
                                    eggman_lamp_rise + eggman_lamp_fall);
    int i;
    int age;
    int alpha;
    Uint32 color;

    for (i = 0; i < count; ++i) {
        if ((eggman_lamp_delay * i) <= frame &&
            frame <
                (eggman_lamp_delay * i) + eggman_lamp_rise + eggman_lamp_fall) {
            age = frame - (eggman_lamp_delay * i);
            if (age < eggman_lamp_rise) {
                alpha = ((Float)age / (eggman_lamp_rise - 1)) * FULL_ALPHA;
            } else {
                alpha =
                    ((Float)(eggman_lamp_rise + eggman_lamp_fall - age - 1) /
                     (eggman_lamp_fall - 1)) *
                    FULL_ALPHA;
            }
            /* @hack type: each channel converted through int, where a
             * conversion to Uint32 would do */
            color = ((Uint32)(int)(alpha * eggman_lamp_red) << RED_SHIFT) |
                    ((Uint32)(int)(alpha * eggman_lamp_green) << GREEN_SHIFT) |
                    (Uint32)(int)(alpha * eggman_lamp_blue) |
                    ((Uint32)alpha << ALPHA_SHIFT);
            draw_lamp(lamps[i], color);
        }
    }
}

/* @hack redundant: register on task, where a plain parameter would do */
static void background_step(register struct task *task)
{
    if (BLOCK_MASK & GLOW_SAMPLE_BLOCKS) {
        sample_floor_glow(task);
    }
    if (!core_flag_is_set(LAST_MESSAGE_FLAG) &&
        (previous_mask & LAND_BLOCK(BEFORE_LAST_BLOCK)) &&
        (BLOCK_MASK & LAND_BLOCK(LAST_BLOCK))) {
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

/* draw each child of root, its model showing texture, while camera 0's
 * land_block_mask holds one of blocks. no code calls it. it is external so
 * that SHC keeps it, as the image does */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void core_eggman_draw_textured_children(NJS_CNK_OBJECT *root, int texture,
                                        Sint32 blocks)
{
    NJS_CNK_OBJECT *child;

    if (BLOCK_MASK & blocks) {
        for (child = root->child; child; child = child->sibling) {
            /* @hack spelling: the halfword's address and new value worked out
             * in steps, read and masked in one, where one assignment to
             * plist[TID_HALFWORD] would do */
            Sint16 *halfword = child->model->plist;
            Sint16 value;

            halfword = halfword + TID_HALFWORD;
            value = *halfword & NJD_FFM_MASK;
            value = value | texture;
            *halfword = value;
            njPushMatrixEx();
            njTranslate(NULL, child->pos[0], child->pos[1], child->pos[2]);
            njRotateXYZ(NULL, child->ang[0], child->ang[1], child->ang[2]);
            njCnkEasyDrawModel(child->model);
            njPopMatrixEx();
        }
    }
}

/* a panel's animation advances PANEL_SPEED of texture_anim_apply's time a
 * step */
#define PANEL_SPEED 2

static void draw_scrolling_panel(NJS_CNK_OBJECT *panel,
                                 struct texture_anim *anim, int texture,
                                 Sint32 blocks)
{
    if (BLOCK_MASK & blocks) {
        SET_TEXTURE(panel->model, texture);
        njPushMatrixEx();
        njTranslate(NULL, panel->pos[0], panel->pos[1], panel->pos[2]);
        njRotateXYZ(NULL, panel->ang[0], panel->ang[1], panel->ang[2]);
        texture_anim_apply(panel->model, anim, game_state.steps * PANEL_SPEED);
        njCnkEasyDrawModel(panel->model);
        njPopMatrixEx();
    }
}

static void draw_glass(NJS_CNK_OBJECT *glass, int texture, Sint32 blocks)
{
    if (BLOCK_MASK & blocks) {
        SET_TEXTURE(glass->model, texture);
        njPushMatrixEx();
        njTranslate(NULL, glass->pos[0], glass->pos[1], glass->pos[2]);
        njRotateXYZ(NULL, glass->ang[0], glass->ang[1], glass->ang[2]);
        CONDUIT_ADD_BEGIN();
        model_draw_tinted(glass->model, eggman_glass_tint);
        CONDUIT_ADD_END();
        njPopMatrixEx();
    }
}

/* @hack type: color an int, where Uint32, as shape_motion_draw_tinted takes
 * it, would do */
static void draw_shape_color(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                             NJS_MOTION *shape, Float frame, int color,
                             Sint32 blocks)
{
    if (BLOCK_MASK & blocks) {
        CONDUIT_ADD_BEGIN();
        njPushMatrixEx();
        njTranslate(NULL, object->pos[0], object->pos[1], object->pos[2]);
        njRotateXYZ(NULL, object->ang[0], object->ang[1], object->ang[2]);
        shape_motion_draw_tinted(object, motion, shape, frame, color);
        njPopMatrixEx();
        CONDUIT_ADD_END();
    }
}

/* @hack type: external linkage, where static would do */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void core_eggman_draw_shape(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                            NJS_MOTION *shape, Float frame, Sint32 blocks)
{
    draw_shape_color(object, motion, shape, frame, eggman_motion_tint, blocks);
}

#include "shared/core_energy_streams.h"

#include "stg35/panels.h"

/* STEPS at half rate. a texture it picks shows for two steps */
#define HALF_STEPS (STEPS / 2)
/* the panels show eggman_panel_texlist's PANEL_FRAMES textures forward and
 * back, PANEL_CYCLE frames to a cycle. the glass shows
 * core_block_glow_texlist's GLASS_FRAMES in turn */
#define PANEL_FRAMES 5
#define PANEL_CYCLE ((PANEL_FRAMES - 1) * 2)
#define GLASS_FRAMES 13
/* the pipes lie in PIPE_ROWS rows, PIPE_ROW_GAP apart in y. each row draws
 * its pipes PIPE_PASSES times. phase numbers the draws, rows first. each
 * draw's motion runs PIPE_PHASE of a cycle ahead of the draw before it. the
 * motion plays PIPE_RATE frames a step */
#define PIPE_ROWS 2
#define PIPE_PASSES 2
#define PIPE_PHASE (1.0F / (PIPE_ROWS * PIPE_PASSES))
#define PIPE_RATE 0.375F
#define PIPE_ROW_GAP 62.0F
/* the first three pipes of a draw play their motion PIPE_LEAD frames apart.
 * second and third run PIPE_LEAD and twice PIPE_LEAD frames ahead of first.
 * SECOND_PIPE_LEAD and THIRD_PIPE_LEAD are those leads in steps */
#define PIPE_LEAD 20.0F
#define SECOND_PIPE_LEAD (PIPE_LEAD / PIPE_RATE)
#define THIRD_PIPE_LEAD ((2 * PIPE_LEAD) / PIPE_RATE)
/* the bounding radius, r, that the pipe's model takes before each draw */
#define PIPE_MODEL_RADIUS 300.0F
/* the joints' motion plays a frame every JOINT_STEPS steps */
#define JOINT_STEPS 2
#define JOINT_RATE (1.0F / JOINT_STEPS)
/* where the curved conduits stand. each spans CONDUIT_ARC degrees and carries
 * CONDUIT_STREAMS streams */
#define CONDUIT_ARC 180
#define CONDUIT_STREAMS 4
#define FIRST_CONDUIT_X (-230.0F)
#define FIRST_CONDUIT_Y (-30.0F)
#define FIRST_CONDUIT_Z (-1300.0F)
#define SECOND_CONDUIT_X 720.0F
#define SECOND_CONDUIT_Y 120.0F
#define SECOND_CONDUIT_Z (-1839.5F)
/* the glow is a disc of GLOW_SECTORS sectors. shared/core_floor_draw.inc
 * gives each sector SECTOR_VERTICES: the center and two rim vertices. it ends
 * with the center again, GLOW_VERTICES in all, and draws them under
 * GLOW_CONTROL. the rim lies RIM_X_SCALE as far out along x as along z */
#define GLOW_SECTORS 32
#define SECTOR_VERTICES 3
#define GLOW_VERTICES ((GLOW_SECTORS * SECTOR_VERTICES) + 1)
#define GLOW_CONTROL (NJD_CONTROL_3D_SHADOW | NJD_CONTROL_3D_TRANS_MODIFIER)
#define RIM_X_SCALE 0.9F
/* the points the outer lamps and the inner lamps are placed from */
#define OUTER_LAMPS_X 669.6873F
#define OUTER_LAMPS_Y 40.0F
#define OUTER_LAMPS_Z (-1569.5F)
#define INNER_LAMPS_X 669.8581F
#define INNER_LAMPS_Y 36.75F
#define INNER_LAMPS_Z (-1569.5F)

static Float glow_uv(Float offset)
{
    return core_glow_uv_scale * offset;
}

static void rotate_z(Angle roll)
{
    njRotateZ(NULL, roll);
}

static void background_draw(struct task *task)
{
    struct task_work *work = task->work;
    int panel_texture;
    int glass_texture;
    int panel_frame;
    int row;
    int pass;
    int phase;
    int vertex_index;
    int frames;
    Float time;
    Float first;
    Float second;
    Float third;
    Float joint_frame;
    Float period;
    Float rate;
    Float clock;
    NJS_CNK_OBJECT *rotor;
    /* floor and the locals below it are shared/core_floor_draw.inc's, as are
     * work and vertex_index */
    struct surface_side *floor;
    NJS_POINT3 pos;
    NJS_TEXTURE_VTX vertices[GLOW_VERTICES];
    NJS_TEXTURE_VTX *vertex;
    Float origin_u;
    Float origin_v;
    Float sine;
    Float cosine;
    int alpha;
    Uint32 color;
    Angle angle;
    Angle pitch;
    Angle roll;

    panel_frame = HALF_STEPS % PANEL_CYCLE;
    glass_texture = HALF_STEPS % GLASS_FRAMES;
    panel_texture =
        panel_frame < PANEL_FRAMES ? panel_frame : PANEL_CYCLE - panel_frame;
    njSetTexture(&eggman_panel_texlist);
#define DRAW_PANEL(n, blocks)                                                  \
    draw_scrolling_panel(&eggman_panel_##n, &eggman_panel_uv_##n,              \
                         panel_texture, blocks)
    DRAW_PANEL(1, LAND_BLOCK(3));
    DRAW_PANEL(2, LAND_BLOCKS(2, 4));
    DRAW_PANEL(3, LAND_BLOCK(3));
    DRAW_PANEL(4, LAND_BLOCKS(1, 4));
    DRAW_PANEL(5, LAND_BLOCKS(3, 4));
    DRAW_PANEL(6, LAND_BLOCKS(4, 5));
    DRAW_PANEL(7, LAND_BLOCKS(5, 6));
    DRAW_PANEL(8, LAND_BLOCKS(6, 7));
    DRAW_PANEL(9, LAND_BLOCKS(6, 8));
    DRAW_PANEL(10, LAND_BLOCKS(6, 7));
    DRAW_PANEL(11, LAND_BLOCKS(6, 7));
    DRAW_PANEL(12, LAND_BLOCKS(6, 8));
    DRAW_PANEL(13, LAND_BLOCKS(1, 4));
    DRAW_PANEL(14, LAND_BLOCKS(4, 6));
    DRAW_PANEL(15, LAND_BLOCKS(4, 6));
    DRAW_PANEL(16, LAND_BLOCKS(6, 9));
    DRAW_PANEL(17, LAND_BLOCKS(1, 2));
    DRAW_PANEL(18, LAND_BLOCKS(1, 2));
    DRAW_PANEL(19, LAND_BLOCKS(1, 3));
    DRAW_PANEL(20, LAND_BLOCKS(1, 3));
    DRAW_PANEL(21, LAND_BLOCKS(1, 3));
    DRAW_PANEL(22, LAND_BLOCKS(1, 3));
    DRAW_PANEL(23, LAND_BLOCKS(7, 9));
    DRAW_PANEL(24, LAND_BLOCKS(7, 9));
    DRAW_PANEL(25, LAND_BLOCKS(8, 11));
    DRAW_PANEL(26, LAND_BLOCKS(8, 11));
    DRAW_PANEL(27, LAND_BLOCK(16));
    DRAW_PANEL(28, LAND_BLOCK(16));
    DRAW_PANEL(29, LAND_BLOCKS(17, 19));
    DRAW_PANEL(30, LAND_BLOCKS(17, 19));
    DRAW_PANEL(31, LAND_BLOCK(16));
    DRAW_PANEL(32, LAND_BLOCKS(1, 3));
    DRAW_PANEL(33, LAND_BLOCKS(1, 3));
    DRAW_PANEL(34, LAND_BLOCKS(4, 6));
    DRAW_PANEL(35, LAND_BLOCKS(4, 6));
    DRAW_PANEL(36, LAND_BLOCKS(14, 16));
    DRAW_PANEL(37, LAND_BLOCKS(14, 16));
    DRAW_PANEL(38, LAND_BLOCKS(2, 3));
    DRAW_PANEL(39, LAND_BLOCKS(4, 6));
    DRAW_PANEL(40, LAND_BLOCK(15));
    DRAW_PANEL(41, LAND_BLOCKS(1, 2));
    DRAW_PANEL(42, LAND_BLOCKS(1, 3));
    DRAW_PANEL(43, LAND_BLOCKS(4, 6));
    DRAW_PANEL(44, LAND_BLOCKS(4, 6));
    DRAW_PANEL(45, LAND_BLOCKS(7, 9));
    DRAW_PANEL(46, LAND_BLOCKS(9, 11));
    DRAW_PANEL(47, LAND_BLOCKS(12, 14));
    DRAW_PANEL(48, LAND_BLOCKS(14, 16));
    DRAW_PANEL(49, LAND_BLOCKS(14, 16));
    DRAW_PANEL(50, LAND_BLOCKS(15, 16));
    DRAW_PANEL(51, LAND_BLOCK(16));
    DRAW_PANEL(52, LAND_BLOCKS(18, 19));
    DRAW_PANEL(53, LAND_BLOCKS(17, 18));
    DRAW_PANEL(54, LAND_BLOCK(19));
    DRAW_PANEL(55, LAND_BLOCKS(18, 19));
    DRAW_PANEL(56, LAND_BLOCKS(15, 18));
    DRAW_PANEL(57, LAND_BLOCKS(9, 12));
    DRAW_PANEL(58, LAND_BLOCKS(10, 12));
    DRAW_PANEL(59, LAND_BLOCKS(10, 13));
    DRAW_PANEL(60, LAND_BLOCKS(11, 13));
    DRAW_PANEL(61, LAND_BLOCKS(10, 12));
#undef DRAW_PANEL
    njSetTexture(&core_block_glow_texlist);
    draw_glass(&eggman_glass_1, glass_texture, LAND_BLOCKS(9, 13));
    draw_glass(&eggman_glass_2, glass_texture, LAND_BLOCKS(11, 13));
    draw_glass(&eggman_glass_3, glass_texture, LAND_BLOCKS(17, 18));
    draw_glass(&eggman_glass_4, glass_texture, LAND_BLOCKS(18, 19));
    njSetTexture(&eggman_rotor_texlist);
    for (rotor = eggman_rotor_assembly->child; rotor; rotor = rotor->sibling) {
/* speed is the rotor's turn about y a step, as an Angle */
#define DRAW_ROTOR(speed, blocks)                                              \
    if (LAND_MASK & (blocks)) {                                                \
        njPushMatrixEx();                                                      \
        njTranslate(NULL, rotor->pos[0], rotor->pos[1], rotor->pos[2]);        \
        njRotateY(NULL, rotor->ang[1] + (STEPS * (speed)));                    \
        njCnkEasyDrawModel(rotor->model);                                      \
        njPopMatrixEx();                                                       \
    }
        if (rotor == &eggman_rotor_fast_a || rotor == &eggman_rotor_fast_b) {
            DRAW_ROTOR(128, LAND_BLOCKS(15, 18));
        } else if (rotor == &eggman_rotor_medium_a ||
                   rotor == &eggman_rotor_medium_b) {
            DRAW_ROTOR(96, LAND_BLOCKS(16, 18));
        } else if (rotor == &eggman_rotor_slow_a ||
                   rotor == &eggman_rotor_slow_b) {
            DRAW_ROTOR(64, LAND_BLOCKS(16, 18));
        } else if (rotor == &eggman_rotor_reverse_a ||
                   rotor == &eggman_rotor_reverse_b ||
                   rotor == &eggman_rotor_reverse_c) {
            DRAW_ROTOR(-112, LAND_BLOCKS(16, 18));
        } else if (rotor == &eggman_rotor_return_a ||
                   rotor == &eggman_rotor_return_b) {
            DRAW_ROTOR(-80, LAND_BLOCKS(16, 18));
        }
#undef DRAW_ROTOR
    }
    njSetTexture(&eggman_pipe_texlist);
    for (row = 0; row < PIPE_ROWS; ++row) {
        /* @hack spelling: pass = pass + 1 and phase = phase + PIPE_ROWS,
         * where ++pass and phase += PIPE_ROWS would do */
        for (pass = 0, phase = row; pass < PIPE_PASSES;
             pass = pass + 1, phase = phase + PIPE_ROWS) {
            frames = eggman_pipe_motion->nbFrame;
            rate = PIPE_RATE;
            /* @hack redundant: clock and period, which nothing reads, set
             * inside the expression that sets time and first */
            first =
                (int)(time = (clock = STEPS) +
                             (phase * (period = frames / rate) * PIPE_PHASE)) %
                (int)(frames / rate) * rate;
            second =
                (int)(time + SECOND_PIPE_LEAD) % (int)(frames / rate) * rate;
            third = (int)(time + THIRD_PIPE_LEAD) % (int)(frames / rate) * rate;
            eggman_pipe->model->r = PIPE_MODEL_RADIUS;
#define PIPE_STRAIGHT
#define PIPE_TURN njRotateY(NULL, -QUARTER_TURN);
#define PLACE_SHAPE(part, x, y, z, heading, frame, blocks)                     \
    njPushMatrixEx();                                                          \
    njTranslate(NULL, x, y, z);                                                \
    heading core_eggman_draw_shape(eggman_##part, eggman_##part##_motion,      \
                                   eggman_##part##_shape, frame, blocks);      \
    njPopMatrixEx();
            PLACE_SHAPE(pipe, 0.0F, (row * PIPE_ROW_GAP) - 31.0F, 0.0F,
                        PIPE_STRAIGHT, first, LAND_BLOCKS(1, 2));
            PLACE_SHAPE(pipe, 0.0F, (row * PIPE_ROW_GAP) - 31.0F, -200.0F,
                        PIPE_STRAIGHT, second, LAND_BLOCKS(1, 2));
            PLACE_SHAPE(pipe, 0.0F, (row * PIPE_ROW_GAP) - 31.0F, -400.0F,
                        PIPE_STRAIGHT, third, LAND_BLOCKS(1, 3));
            PLACE_SHAPE(pipe, -500.0F, (row * PIPE_ROW_GAP) - 31.0F, -1100.0F,
                        PIPE_STRAIGHT, first, LAND_BLOCKS(7, 9));
            PLACE_SHAPE(pipe, -230.0F, (row * PIPE_ROW_GAP) - 31.0F, -1570.0F,
                        PIPE_TURN, first, LAND_BLOCKS(9, 11));
            PLACE_SHAPE(pipe, 570.0F, (row * PIPE_ROW_GAP) + 120.0F, -2595.0F,
                        PIPE_STRAIGHT, first, LAND_BLOCKS(17, 19));
            PLACE_SHAPE(pipe, 570.0F, (row * PIPE_ROW_GAP) - 2710.0F, -2845.0F,
                        PIPE_STRAIGHT, first, LAND_BLOCKS(17, 19));
        }
    }
    njSetTexture(&eggman_joint_texlist);
    joint_frame =
        (Float)(STEPS % (eggman_joint_motion->nbFrame * JOINT_STEPS)) *
        JOINT_RATE;
    PLACE_SHAPE(joint, 0.0F, 0.0F, -100.2F, PIPE_STRAIGHT, joint_frame,
                LAND_BLOCKS(1, 2));
    PLACE_SHAPE(joint, 0.0F, 0.0F, -300.2F, PIPE_STRAIGHT, joint_frame,
                LAND_BLOCKS(1, 2));
    PLACE_SHAPE(joint, 0.0F, 0.0F, -500.2F, PIPE_STRAIGHT, joint_frame,
                LAND_BLOCKS(1, 3));
    PLACE_SHAPE(joint, -500.0F, 0.0F, -1200.2F, PIPE_STRAIGHT, joint_frame,
                LAND_BLOCKS(7, 9));
    PLACE_SHAPE(joint, -129.8F, 0.0F, -1569.5F, PIPE_TURN, joint_frame,
                LAND_BLOCKS(9, 11));
    PLACE_SHAPE(joint, 570.0F, 150.0F, -2695.2F, PIPE_STRAIGHT, joint_frame,
                LAND_BLOCKS(17, 19));
    PLACE_SHAPE(joint, 570.0F, -2680.0F, -2945.2F, PIPE_STRAIGHT, joint_frame,
                LAND_BLOCKS(17, 19));
#undef PLACE_SHAPE
#undef PIPE_STRAIGHT
#undef PIPE_TURN
    if (LAND_MASK & LAND_BLOCKS(7, 10)) {
        njPushMatrixEx();
        njTranslate(NULL, FIRST_CONDUIT_X, FIRST_CONDUIT_Y, FIRST_CONDUIT_Z);
        njRotateY(NULL, -QUARTER_TURN);
        draw_streams(task, CONDUIT_ARC, CONDUIT_STREAMS, LAND_BLOCKS(7, 10));
        njPopMatrixEx();
    }
    if (LAND_MASK & LAND_BLOCKS(12, 15)) {
        njPushMatrixEx();
        njTranslate(NULL, SECOND_CONDUIT_X, SECOND_CONDUIT_Y, SECOND_CONDUIT_Z);
        njRotateY(NULL, -QUARTER_TURN);
        draw_streams(task, -CONDUIT_ARC, CONDUIT_STREAMS, LAND_BLOCKS(12, 15));
        njPopMatrixEx();
    }
    if ((LAND_MASK & GLOW_BLOCKS) && CORE_GLOW_FLOOR(task) && player_work[0]) {
#define GLOW_ROLL rotate_z
#define GLOW_TEXLIST eggman_glow_texlist
#define GLOW_NARROW_RIM sine *= RIM_X_SCALE
/* @hack spelling: the glow's sectors counted in row, the pipes' counter,
 * where a local of their own would do */
#define GLOW_SECTOR_INDEX row
#include "shared/core_floor_draw.inc"
#undef GLOW_SECTOR_INDEX
#undef GLOW_NARROW_RIM
#undef GLOW_TEXLIST
#undef GLOW_ROLL
    }
    njSetTexture(&eggman_strip_texlist);
#define PLACE_STRIP(x, y, z, blocks)                                           \
    if (LAND_MASK & (blocks)) {                                                \
        njPushMatrixEx();                                                      \
        njTranslate(NULL, x, y, z);                                            \
        draw_strip_lights(task);                                               \
        njPopMatrixEx();                                                       \
    }
    PLACE_STRIP(-0.004F, 46.0F, -699.999F, LAND_BLOCKS(1, 3));
    PLACE_STRIP(-500.004F, 654.0F, -699.999F, LAND_BLOCKS(4, 6));
    PLACE_STRIP(569.996F, 196.0F, -2109.999F, LAND_BLOCKS(14, 16));
#undef PLACE_STRIP
    if (LAND_MASK & LAND_BLOCKS(10, 12)) {
        njPushMatrixEx();
        njTranslate(NULL, OUTER_LAMPS_X, OUTER_LAMPS_Y, OUTER_LAMPS_Z);
        njSetTexture(&eggman_outer_lamp_texlist);
        draw_lamp_sequence(eggman_outer_lamps_a,
                           ARRAY_COUNT(eggman_outer_lamps_a));
        draw_lamp_sequence(eggman_outer_lamps_b,
                           ARRAY_COUNT(eggman_outer_lamps_b));
        draw_lamp_sequence(eggman_outer_lamps_c,
                           ARRAY_COUNT(eggman_outer_lamps_c));
        draw_lamp_sequence(eggman_outer_lamps_d,
                           ARRAY_COUNT(eggman_outer_lamps_d));
        njPopMatrixEx();
        njPushMatrixEx();
        njTranslate(NULL, INNER_LAMPS_X, INNER_LAMPS_Y, INNER_LAMPS_Z);
        njSetTexture(&eggman_inner_lamp_texlist);
        draw_lamp_sequence(eggman_inner_lamps_a,
                           ARRAY_COUNT(eggman_inner_lamps_a));
        draw_lamp_sequence(eggman_inner_lamps_b,
                           ARRAY_COUNT(eggman_inner_lamps_b));
        njPopMatrixEx();
    }
}

/* the bounds land_clip_step parts the land blocks by. END_Z is where a block
 * ends toward -z, and the blocks beside it end there too. EAST_X and WEST_X
 * are a block's sides toward +x and -x. TOP_Y and BOTTOM_Y are its top and
 * bottom */
#define BLOCK_1_END_Z (-600.0F)
#define BLOCK_2_END_Z (-800.0F)
#define BLOCK_2_WEST_X (-150.0F)
#define BLOCK_3_WEST_X (-350.0F)
#define BLOCK_3_TOP_Y 120.0F
#define BLOCK_6_END_Z (-1100.0F)
#define BLOCK_7_TOP_Y 120.0F
#define BLOCK_8_EAST_X (-230.0F)
#define BLOCK_8_END_Z (-1300.0F)
#define BLOCK_10_END_Z (-1840.0F)
#define BLOCK_10_EAST_X 20.0F
#define BLOCK_11_EAST_X 400.0F
#define BLOCK_12_EAST_X 670.0F
#define BLOCK_14_END_Z (-2260.0F)
#define BLOCK_15_EAST_X 670.0F
#define BLOCK_16_BOTTOM_Y 40.0F
#define BLOCK_17_END_Z (-2650.0F)
#define BLOCK_18_END_Z (-3045.0F)

/* give camera 0 the one land block where player 0 stands */
static void land_clip_step(struct task *task)
{
    struct task_work *player;

    player = player_work[0];

    previous_mask = LAND_MASK;
    /* @bug memory: reads player 0's pos through null where player_work[0] is
     * null. background_draw tests that pointer at 0x8c50258c, and
     * sample_floor_glow at 0x8c500610. this step reads pos.z untested at
     * 0x8c502ae4 */
#ifdef NONMATCHING
    /* this side repairs the tag above: while slot 0 holds no player, the step
     * leaves the land mask as it was */
    if (player == NULL) {
        return;
    }
#endif
    if (player->pos.z > BLOCK_1_END_Z) {
        LAND_MASK = LAND_BLOCK(1);
    } else if (player->pos.z > BLOCK_2_END_Z) {
        if (player->pos.x > BLOCK_2_WEST_X) {
            LAND_MASK = LAND_BLOCK(2);
        } else if (player->pos.x > BLOCK_3_WEST_X) {
            if (player->pos.y < BLOCK_3_TOP_Y) {
                LAND_MASK = LAND_BLOCK(3);
            } else {
                LAND_MASK = LAND_BLOCK(4);
            }
        } else {
            LAND_MASK = LAND_BLOCK(5);
        }
    } else if (player->pos.z > BLOCK_6_END_Z) {
        if (player->pos.y > BLOCK_7_TOP_Y) {
            LAND_MASK = LAND_BLOCK(6);
        } else {
            LAND_MASK = LAND_BLOCK(7);
        }
    } else if (player->pos.x < BLOCK_8_EAST_X) {
        if (player->pos.z > BLOCK_8_END_Z) {
            LAND_MASK = LAND_BLOCK(8);
        } else {
            LAND_MASK = LAND_BLOCK(9);
        }
    } else if (player->pos.z > BLOCK_10_END_Z) {
        if (player->pos.x < BLOCK_10_EAST_X) {
            LAND_MASK = LAND_BLOCK(10);
        } else if (player->pos.x < BLOCK_11_EAST_X) {
            LAND_MASK = LAND_BLOCK(11);
        } else if (player->pos.x < BLOCK_12_EAST_X) {
            LAND_MASK = LAND_BLOCK(12);
        } else {
            LAND_MASK = LAND_BLOCK(13);
        }
    } else if (player->pos.z > BLOCK_14_END_Z) {
        if (player->pos.x > BLOCK_15_EAST_X) {
            LAND_MASK = LAND_BLOCK(14);
        } else {
            LAND_MASK = LAND_BLOCK(15);
        }
    } else if (player->pos.y > BLOCK_16_BOTTOM_Y) {
        LAND_MASK = LAND_BLOCK(16);
    } else if (player->pos.z > BLOCK_17_END_Z) {
        LAND_MASK = LAND_BLOCK(17);
    } else if (player->pos.z > BLOCK_18_END_Z) {
        LAND_MASK = LAND_BLOCK(18);
    } else {
        LAND_MASK = LAND_BLOCK(19);
    }
}
