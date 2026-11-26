/* the scenery of Knuckles's route, its poison colliders and its two beam
 * fences. the background's step sets the fog and the specks for the view's side
 * of the water line. a fence opens once its switch turns on, and camera 0 shows
 * it opening */
#include <ninja.h>
#include <sg_xpt.h>
#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "shared/common_object.h"
#include "shared/fog.h"
#include "shared/lagoon_specks.h"
#include "shared/switch.h"
#include "stg38/background.h"
#include "shared/core_scenery_art.h"
#include "stg38/core_knuckles.h"
#include "stg38/water_scenery.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"

/* the task level of the background's task, and of the other tasks here */
#define BACKGROUND_LEVEL 1
#define TASK_LEVEL 2
/* the water line's y. scenery_step counts the view as under it only while the
 * current camera stands toward -z from UNDERWATER_MAX_Z */
#define WATER_HEIGHT 620.0F
#define UNDERWATER_MAX_Z 360.0F

/* a fence's submodes */
enum {
    /* the fence collides and draws until its switch turns on */
    FENCE_CLOSED,
    /* camera 0, in CAMERA_MODE_EASYSET, looks from the fence's view at its
     * target for FENCE_STEPS. over the first FADE_STEPS, the fence fades out
     * on odd steps and draws nothing on even ones. the first fence turns
     * player 0's pad off as the submode starts. each fence turns the pad on
     * as the submode ends */
    FENCE_OPENING,
    /* the fence draws nothing. the second fence takes camera 0's top mode off
     * as the submode starts. the first fence takes it off at its first step
     * in the submode */
    FENCE_OPEN,
    /* the first fence's last submode, once camera 0's top mode is off */
    FENCE_DONE
};
#define FENCE_STEPS 120
#define FADE_STEPS 60
/* the steps a fence has spent FENCE_OPENING */
#define OPENING_STEPS(task) ((task)->unk_3c.l)
/* a fading fence's alpha, which falls from OPAQUE as FENCE_OPENING starts to
 * 0 at FADE_STEPS */
#define FADE_ALPHA(task)                                                       \
    ((1.0F - (OPENING_STEPS(task) * (1.0F / FADE_STEPS))) * OPAQUE)
#define OPAQUE 255.0F
/* a fading fence's tint: its alpha in every byte, a gray that darkens as the
 * fence clears */
#define FADE_TINT(alpha)                                                       \
    (((Uint32)(alpha) << 24) | ((Uint32)(alpha) << 16) |                       \
     ((Uint32)(alpha) << 8) | (alpha))
#define FIRST_FENCE_SWITCH 1
#define SECOND_FENCE_SWITCH 3
/* the center of the first fence's box. the second fence is the first moved
 * by SECOND_FENCE_OFFSET_Y and SECOND_FENCE_OFFSET_Z, its model and its box
 * alike */
#define FIRST_FENCE_Y 310.0F
#define FIRST_FENCE_Z (-255.0F)
#define SECOND_FENCE_OFFSET_Y 70.0F
#define SECOND_FENCE_OFFSET_Z (-650.0F)
/* where the poison's colliders stand, and the shapes each collision here
 * holds */
#define LOWER_POISON_X 0.0F
#define LOWER_POISON_Y 133.0F
#define LOWER_POISON_Z (-550.0F)
#define UPPER_POISON_X 0.0F
#define UPPER_POISON_Y 455.0F
#define UPPER_POISON_Z (-1400.0F)
#define COLLIDER_SHAPES 1

static Float camera_reach = 3.25F;
/* clang-format off */
static struct collision_shape poison_shape = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_PLAYERS, SHAPE_UNK_2000,
    {0, 0, 0}, 125.0F, 5.0F, 125.0F, 0, {0, 0, 0}};
static struct collision_shape first_fence_shape = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
    SHAPE_UNK_2000, {0, FIRST_FENCE_Y, FIRST_FENCE_Z}, 25.0F, 25.0F, 2.5F, 0,
    {0, 0, 0}};
static struct collision_shape second_fence_shape = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
    SHAPE_UNK_2000,
    {0, FIRST_FENCE_Y + SECOND_FENCE_OFFSET_Y,
     FIRST_FENCE_Z + SECOND_FENCE_OFFSET_Z}, 25.0F, 25.0F, 2.5F, 0, {0, 0, 0}};
/* clang-format on */

Sint32 core_knuckles_water_side;
/* the current camera's pos and target as the first fence's switch turns on.
 * camera 0's CAMERA_MODE_EASYSET takes them for its last step */
static NJS_POINT3 saved_position;
static NJS_POINT3 saved_target;

static void scenery_step(struct task *task);
static void scenery_draw(struct task *task);
static Bool view_dips_below(Float height);
static void lower_poison_start(void);
static void upper_poison_start(void);
static void first_fence_step(struct task *task);
static void first_fence_draw(struct task *task);
static void second_fence_step(struct task *task);
static void second_fence_draw(struct task *task);
static void fence_draw_tinted(NJS_CNK_OBJECT *root, Uint32 color);

void core_knuckles_background_init(void)
{
    struct task *task =
        task_create(TASK_WORK, BACKGROUND_LEVEL, scenery_step, "BgExec");
    if (task) {
        task->draw = scenery_draw;
    }
}

void core_knuckles_hazards_start(void)
{
    lower_poison_start();
    upper_poison_start();
}

static void scenery_step(struct task *task)
{
    if ((core_knuckles_water_side == WATER_SIDE_ABOVE ||
         core_knuckles_water_side == WATER_SIDE_UNSET) &&
        camera_views[camera_current_index]->pos.z < UNDERWATER_MAX_Z &&
        (camera_views[camera_current_index]->pos.y <= WATER_HEIGHT ||
         view_dips_below(WATER_HEIGHT))) {
        fog_set(&core_knuckles_fog);
        if (core_knuckles_specks) {
            core_knuckles_specks->work->mode = MODE_LAGOON_SPECKS_SHOWN;
        }
        core_knuckles_water_side = WATER_SIDE_UNDER;
    } else if ((core_knuckles_water_side == WATER_SIDE_UNDER ||
                core_knuckles_water_side == WATER_SIDE_UNSET) &&
               camera_views[camera_current_index]->pos.y > WATER_HEIGHT &&
               !view_dips_below(WATER_HEIGHT)) {
        fog_set(&core_knuckles_upper_fog);
        if (core_knuckles_specks) {
            core_knuckles_specks->work->mode = MODE_LAGOON_SPECKS_HIDDEN;
        }
        core_knuckles_water_side = WATER_SIDE_ABOVE;
    }
}

#define CORE_KNUCKLES_WATER
#include "shared/core_water_geometry.h"

/* the steps each cell of a 4-by-4 sheet shows for, on the columns and on
 * the other sheets */
#define COLUMN_CELL_STEPS 1
#define CELL_STEPS 2
/* the columns rise and fall COLUMN_SPEED a step, and start over every
 * COLUMN_STEPS steps */
#define COLUMN_STEPS 180
#define COLUMN_SPEED 3.527778F
/* the textures of surface_textures, which scenery_draw shows in turn, one a
 * step */
#define SURFACE_TEXTURES 14
#define SHAFT_ROTOR_SLOWDOWN 4
#define UPPER_ROTOR_SLOWDOWN 3
#define UPPER_ROTOR_Y 630.0F
/* the land blocks in which the columns and shaft_walls draw, and those in
 * which inner_wall_shaft and middle_wall_shaft draw */
#define COLUMN_BLOCKS (LAND_BLOCKS(1, 2) | LAND_BLOCK(11) | LAND_BLOCKS(21, 22))
#define WALL_SHAFT_BLOCKS (LAND_BLOCK(1) | LAND_BLOCK(11) | LAND_BLOCK(21))

static void scenery_draw(struct task *task)
{
    NJS_MOTION *motion;
    NJS_CNK_OBJECT *object;
    Float frame;
    /* @hack spelling: the columns' speed held in a local, where COLUMN_SPEED
     * would do */
    Float column_speed;
    int texture;
    int i;

    njSetTexture(&column_textures);
    njPushMatrixEx();
    column_speed = COLUMN_SPEED;
    njTranslate(NULL, 0, (game_state.steps % COLUMN_STEPS) * column_speed, 0);
    core_draw_sheet_children(&rising_columns, COLUMN_CELL_STEPS, COLUMN_BLOCKS);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslate(NULL, 0, -((game_state.steps % COLUMN_STEPS) * column_speed),
                0);
    core_draw_sheet_children(&falling_columns, COLUMN_CELL_STEPS,
                             COLUMN_BLOCKS);
    njPopMatrixEx();
    njSetTexture(&shaft_textures);
    core_draw_sheet_children(&shaft_walls, CELL_STEPS, COLUMN_BLOCKS);
    core_draw_sheet_children(&shaft_entry_walls, CELL_STEPS,
                             LAND_BLOCK(1) | LAND_BLOCKS(21, 22));
    if (core_knuckles_water_side == WATER_SIDE_ABOVE) {
        texture = game_state.steps % SURFACE_TEXTURES;
        njSetTexture(&surface_textures);
        core_draw_texture_children(&surface_tiles, texture, LAND_BLOCK(22));
    }
    if (game_state.land_block_mask[0] &
        (LAND_BLOCKS(1, 2) | LAND_BLOCKS(21, 22))) {
        motion = &shaft_rotor_motion;
        frame = MOTION_FRAME(motion, SHAFT_ROTOR_SLOWDOWN);
        njSetTexture(&rotor_textures);
        njPushMatrixEx();
        object = &shaft_rotor;
        njCnkEasyDrawMotion(object, motion, frame);
        njRotateY(NULL, QUARTER_TURN);
        njCnkEasyDrawMotion(object, motion, frame);
        njRotateY(NULL, QUARTER_TURN);
        njCnkEasyDrawMotion(object, motion, frame);
        njRotateY(NULL, QUARTER_TURN);
        njCnkEasyDrawMotion(object, motion, frame);
        njPopMatrixEx();
    }
    njSetTexture(&poison_wall_textures);
    core_draw_sheet_children(&lower_poison_walls, CELL_STEPS,
                             LAND_BLOCKS(2, 3));
    core_draw_sheet_children(&upper_poison_walls, CELL_STEPS, LAND_BLOCK(12));
    texture = game_state.steps % GLOW_TEXTURES;
    njSetTexture(&core_block_glow_texlist);
    core_draw_texture_children(&lower_poison, texture, LAND_BLOCKS(2, 3));
    core_draw_texture_children(&upper_poison, texture, LAND_BLOCK(12));
    draw_control_set(DRAW_CONTROL_SHADOW);
    i = game_state.steps % WALL_FRAMES;
    njSetTexture(&inner_wall_textures);
    core_draw_at_children(inner_wall_frames[i], &inner_wall_entry,
                          LAND_BLOCKS(1, 2));
    core_draw_at_children(inner_wall_frames[i], &inner_wall_shaft,
                          WALL_SHAFT_BLOCKS);
    core_draw_at_children(inner_wall_frames[i], &inner_wall_channel,
                          LAND_BLOCKS(11, 12));
    core_draw_at_children(inner_wall_frames[i], &inner_wall_exit,
                          LAND_BLOCK(12));
    /* @hack redundant: i taken again from game_state.steps, where the inner
     * walls' i would do */
    i = game_state.steps % WALL_FRAMES;
    njSetTexture(&middle_wall_textures);
    core_draw_at_children(middle_wall_frames[i], &middle_wall_entry,
                          LAND_BLOCKS(1, 2) | LAND_BLOCK(21));
    core_draw_at_children(middle_wall_frames[i], &middle_wall_shaft,
                          WALL_SHAFT_BLOCKS);
    core_draw_at_children(middle_wall_frames[i], &middle_wall_lower,
                          LAND_BLOCKS(2, 3));
    core_draw_at_children(middle_wall_frames[i], &middle_wall_channel,
                          LAND_BLOCKS(11, 12));
    core_draw_at_children(middle_wall_frames[i], &middle_wall_exit,
                          LAND_BLOCK(12));
    /* @hack redundant: i taken again from game_state.steps, where the middle
     * walls' i would do */
    i = game_state.steps % WALL_FRAMES;
    njSetTexture(&outer_wall_textures);
    core_draw_at_children(outer_wall_frames[i], &outer_wall_entry,
                          LAND_BLOCKS(1, 2));
    core_draw_at_children(outer_wall_frames[i], &outer_wall_shaft,
                          LAND_BLOCK(1) | LAND_BLOCK(11));
    core_draw_at_children(outer_wall_frames[i], &outer_wall_lower,
                          LAND_BLOCKS(2, 3));
    core_draw_at_children(outer_wall_frames[i], &outer_wall_channel,
                          LAND_BLOCKS(11, 12));
    core_draw_at_children(outer_wall_frames[i], &outer_wall_exit,
                          LAND_BLOCK(12));
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njSetTexture(&channel_textures);
    core_draw_sheet_children(&lower_channel, CELL_STEPS, LAND_BLOCKS(2, 3));
    core_draw_sheet_children(&channel_turn, CELL_STEPS, LAND_BLOCKS(11, 12));
    core_draw_sheet_children(&upper_channel, CELL_STEPS, LAND_BLOCK(12));
    njSetTexture(&upper_shaft_textures);
    core_draw_sheet_children(&upper_shaft, CELL_STEPS, LAND_BLOCKS(21, 22));
    if (game_state.land_block_mask[0] & LAND_BLOCKS(21, 22)) {
        motion = &upper_rotor_motion;
        frame = MOTION_FRAME(motion, UPPER_ROTOR_SLOWDOWN);
        /* @hack redundant: a test of njPopMatrixEx's address and motion's
         * fields that never runs */
        if (0) {
            if (&njPopMatrixEx && motion->mdata && motion->type &&
                motion->nbFrame) {
            }
        }
        njPushMatrixEx();
        njSetTexture(&upper_rotor_textures);
        njTranslate(NULL, 0, UPPER_ROTOR_Y, 0);
        njCnkEasyDrawMotion(&upper_rotor, motion, frame);
        njRotateY(NULL, HALF_TURN);
        njCnkEasyDrawMotion(&upper_rotor, motion, frame);
        njPopMatrixEx();
    }
}

/* view_dips_below's view reaches VIEW_HALF_ANGLE to each side of its middle.
 * its screen reaches SCREEN_HALF_WIDTH to each side and SCREEN_HALF_HEIGHT up
 * and down. the point view_dips_below tests lies VIEW_DEPTH along the
 * camera's z. that point lies nearer the camera than CLEAR_DEPTHS times
 * |VIEW_DEPTH|. so it stays above any height the camera stands more than that
 * above */
#define SCREEN_HALF_WIDTH 320.0F
#define SCREEN_HALF_HEIGHT 240.0F
#define VIEW_HALF_ANGLE ((Angle)(35.0F * ANGLE_PER_DEGREE))
#define VIEW_DEPTH (-3.0F)
#define CLEAR_DEPTHS 3.0F

/* whether the middle of the bottom edge of the current camera's view lies
 * below height. the point turns with the camera's ang.x alone. the call
 * returns FALSE where the camera lies below height */
static Bool view_dips_below(Float height)
{
    struct camera_place *camera = camera_views[camera_current_index];
    NJS_POINT3 edge;
    NJS_POINT3 point;
    Float distance;
    Float half_height;

    if (height > camera->pos.y) {
        return FALSE;
    }
    /* @hack spelling: njAbs(VIEW_DEPTH), where -VIEW_DEPTH would do */
    if (camera->pos.y > height + (CLEAR_DEPTHS * njAbs(VIEW_DEPTH))) {
        return FALSE;
    }
    distance = SCREEN_HALF_WIDTH / njTan(VIEW_HALF_ANGLE);
    /* @hack spelling: njAbs(VIEW_DEPTH), where -VIEW_DEPTH would do */
    half_height = njAbs(VIEW_DEPTH) * SCREEN_HALF_HEIGHT / distance;
    edge.x = 0.0F;
    edge.y = -half_height;
    edge.z = VIEW_DEPTH;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&camera->pos);
    /* @hack spelling: ang.x read through camera_views[camera_current_index],
     * where camera would do */
    njRotateX(NULL, camera_views[camera_current_index]->ang.x);
    njCalcPoint(NULL, &edge, &point);
    njPopMatrixEx();
    return height > point.y;
}

Float core_camera_look_height(void)
{
    struct camera_place *camera = camera_views[camera_current_index];
    return camera->pos.y +
           ((camera->target.y - camera->pos.y) / camera->dist * camera_reach);
}

static void poison_step(struct task *task)
{
    collision_register(task);
}

static void lower_poison_start(void)
{
    struct task *task =
        task_create(TASK_WORK, TASK_LEVEL, poison_step, "PoisonColi");
    if (task) {
        struct task_work *work = task->work;
        work->pos.x = LOWER_POISON_X;
        work->pos.y = LOWER_POISON_Y;
        work->pos.z = LOWER_POISON_Z;
        work->ang.x = work->ang.y = work->ang.z = 0;
        collision_init(task, &poison_shape, COLLIDER_SHAPES, COLLISION_OBJECT);
    }
}

static void upper_poison_start(void)
{
    struct task *task =
        task_create(TASK_WORK, TASK_LEVEL, poison_step, "PoisonColi");
    if (task) {
        struct task_work *work = task->work;
        work->pos.x = UPPER_POISON_X;
        work->pos.y = UPPER_POISON_Y;
        work->pos.z = UPPER_POISON_Z;
        work->ang.x = work->ang.y = work->ang.z = 0;
        collision_init(task, &poison_shape, COLLIDER_SHAPES, COLLISION_OBJECT);
    }
}

void core_knuckles_first_fence_start(void)
{
    struct task *task = task_create(TASK_WORK, TASK_LEVEL, first_fence_step,
                                    "BgBeamFence1OpenExec");
    if (task) {
        task->draw = first_fence_draw;
        OPENING_STEPS(task) = 0;
        task->work->submode = FENCE_CLOSED;
        collision_init(task, &first_fence_shape, COLLIDER_SHAPES,
                       COLLISION_OBJECT);
    }
}

static void first_fence_step(struct task *task)
{
    /* @hack spelling: work for the first read of the submode, where task->work
     * would do */
    struct task_work *work = task->work;
    switch (work->submode) {
    case FENCE_CLOSED:
        collision_register(task);
        if (switch_is_on(FIRST_FENCE_SWITCH)) {
            pad_disable_player(0);
            OPENING_STEPS(task) = 0;
            task->work->submode++;
            saved_position = camera_views[camera_current_index]->pos;
            saved_target = camera_views[camera_current_index]->target;
            camera_push_mode(0, CAMERA_MODE_EASYSET);
            camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_NONE);
            {
                struct camera_state *camera = &camera_easyset_states[0];
                /* @hack spelling: the view and the target copied a field at a
                 * time, where struct assignments would do */
                camera->pos.x = first_fence_view.x;
                camera->pos.y = first_fence_view.y;
                camera->pos.z = first_fence_view.z;
                camera->target.x = first_fence_target.x;
                camera->target.y = first_fence_target.y;
                camera->target.z = first_fence_target.z;
                camera->type = CAMERA_POS_TARGET;
            }
        }
        break;
    case FENCE_OPENING:
        if (++OPENING_STEPS(task) >= FENCE_STEPS) {
            pad_enable_player(0);
            {
                struct camera_state *camera = &camera_easyset_states[0];
                camera->pos = saved_position;
                camera->target = saved_target;
            }
            task->work->submode++;
        }
        break;
    case FENCE_OPEN:
        camera_remove_mode(0, camera_top(0));
        task->work->submode++;
        break;
    case FENCE_DONE:
        break;
    }
}

static void first_fence_draw(struct task *task)
{
    struct task *fence;
    NJS_CNK_OBJECT *object;
    /* @hack type: color held as an int, where a Uint32, as fence_draw_tinted
     * takes it, would do */
    int color;
    Uint8 alpha;
    /* @hack spelling: task copied to fence, where task would do */
    fence = task;
    switch (fence->work->submode) {
    case FENCE_CLOSED:
        if (game_state.land_block_mask[0] & (LAND_BLOCK(1) | LAND_BLOCK(21))) {
            njPushMatrixEx();
            njSetTexture(&fence_textures);
            object = &beam_fence;
            if (game_state.steps & 1) {
                njCnkEasyDrawObject(object);
            } else {
                fence_draw_tinted(object, COLOR_HALF);
            }
            njPopMatrixEx();
        }
        break;
    case FENCE_OPENING:
        if (OPENING_STEPS(fence) < FADE_STEPS) {
            if (game_state.steps & 1) {
                njPushMatrixEx();
                njSetTexture(&fence_textures);
                alpha = FADE_ALPHA(fence);
                color = FADE_TINT(alpha);
                fence_draw_tinted(&beam_fence, color);
                njPopMatrixEx();
            }
        }
        break;
    case FENCE_OPEN:
        break;
    }
}

void core_knuckles_second_fence_start(void)
{
    struct task *task = task_create(TASK_WORK, TASK_LEVEL, second_fence_step,
                                    "BgBeamFence2OpenExec");
    if (task) {
        task->draw = second_fence_draw;
        OPENING_STEPS(task) = 0;
        task->work->submode = FENCE_CLOSED;
        collision_init(task, &second_fence_shape, COLLIDER_SHAPES,
                       COLLISION_OBJECT);
    }
}

static void second_fence_step(struct task *task)
{
    /* @hack spelling: work for the first read of the submode, where task->work
     * would do */
    struct task_work *work = task->work;
    switch (work->submode) {
    case FENCE_CLOSED:
        collision_register(task);
        if (switch_is_on(SECOND_FENCE_SWITCH)) {
            OPENING_STEPS(task) = 0;
            task->work->submode++;
            camera_push_mode(0, CAMERA_MODE_EASYSET);
            camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_NONE);
            {
                struct camera_state *camera = &camera_easyset_states[0];
                /* @hack spelling: the view and the target copied a field at a
                 * time, where struct assignments would do */
                camera->pos.x = second_fence_view.x;
                camera->pos.y = second_fence_view.y;
                camera->pos.z = second_fence_view.z;
                camera->target.x = second_fence_target.x;
                camera->target.y = second_fence_target.y;
                camera->target.z = second_fence_target.z;
                camera->type = CAMERA_POS_TARGET;
            }
        }
        break;
    case FENCE_OPENING:
        if (++OPENING_STEPS(task) >= FENCE_STEPS) {
            pad_enable_player(0);
            camera_remove_mode(0, camera_top(0));
            task->work->submode++;
        }
        break;
    case FENCE_OPEN:
        break;
    }
}

static void second_fence_draw(struct task *task)
{
    NJS_CNK_OBJECT *object;
    /* @hack type: color held as an int, where a Uint32, as fence_draw_tinted
     * takes it, would do */
    int color;
    Uint8 alpha;
    switch (task->work->submode) {
    case FENCE_CLOSED:
        njPushMatrixEx();
        njTranslate(NULL, 0, SECOND_FENCE_OFFSET_Y, SECOND_FENCE_OFFSET_Z);
        njSetTexture(&fence_textures);
        object = &beam_fence;
        if (game_state.steps & 1) {
            njCnkEasyDrawObject(object);
        } else {
            fence_draw_tinted(object, COLOR_HALF);
        }
        njPopMatrixEx();
        break;
    case FENCE_OPENING:
        if (OPENING_STEPS(task) < FADE_STEPS) {
            if (game_state.steps & 1) {
                njPushMatrixEx();
                njTranslate(NULL, 0, SECOND_FENCE_OFFSET_Y,
                            SECOND_FENCE_OFFSET_Z);
                njSetTexture(&fence_textures);
                alpha = FADE_ALPHA(task);
                color = FADE_TINT(alpha);
                fence_draw_tinted(&beam_fence, color);
                njPopMatrixEx();
            }
        }
        break;
    case FENCE_OPEN:
        break;
    }
}

static void fence_draw_tinted(NJS_CNK_OBJECT *root, Uint32 color)
{
    NJS_CNK_OBJECT *child;
    for (child = root->child; child; child = child->sibling) {
        model_draw_tinted(child->model, color);
    }
}
