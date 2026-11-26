/* the moving scenery of Sonic's route, and its colliders */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/color.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "stg34/scenery.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"
#include "shared/core_scenery_art.h"
#include "stg34/scenery_art.h"

/* @hack type: camera 0's land_block_mask read through a cast of the array's
 * address, where land_block_mask[0] would do */
#define BLOCK_MASK (*(Sint32 *)&game_state.land_block_mask)
/* the blocks in which a collider registers its shape */
#define COLLIDER_BLOCKS(task) ((task)->unk_3c.l)
/* the task level of the colliders' tasks */
#define COLLIDER_LEVEL 2
/* how many shapes the collisions of a fan, the gate and the entry's trigger
 * hold. the gate's holds only the first of gate_shapes, the wall */
#define FAN_SHAPES 1
#define GATE_SHAPES 1
#define ENTRY_SHAPES 1
#define FRAMES(table) ((int)ARRAY_COUNT(table))
/* game_state.steps at half rate. a frame it picks shows for two steps */
#define HALF_STEPS (game_state.steps >> 1)
/* the halfword of a water model's polygon list that holds its texture's
 * index. the index takes the halfword's low 13 bits. each draw that sets it
 * keeps the filter mode's bits, NJD_FFM_MASK, and clears NJD_FSS */
#define WATER_TID 7
/* the water shows each of water_surface_textures' WATER_TEXTURES for a step.
 * the waterfalls show each of waterfall_textures' WATERFALL_TEXTURES for two
 * steps */
#define WATER_TEXTURES 16
#define WATERFALL_TEXTURES 8

/* the cylinders' lamps go through a cycle of LAMPS_CYCLE steps. they are out
 * until LAMPS_ON, and fade in until LAMPS_LIT. they are lit until LAMPS_DIM,
 * and fade out until LAMPS_OFF. they are out until FLICKER_START, and flicker
 * until FLICKER_END. they are out for the rest of the cycle. a fading lamp's
 * alpha moves ALPHA_PER_STEP a step */
#define LAMPS_CYCLE 420
#define LAMPS_ON 60.0F
#define LAMPS_LIT 75.0F
#define LAMPS_DIM 225.0F
#define LAMPS_OFF 240.0F
#define FLICKER_START 330.0F
#define FLICKER_END 390.0F
#define ALPHA_PER_STEP 17.0F
/* a fading lamp's tint: white, with its alpha in the top byte */
#define WHITE 0xffffff
/* @hack type: the alpha converted through Sint32, where a conversion to
 * Uint32 would do */
#define FADE_TINT(alpha) (((Uint32)(Sint32)(alpha) << ALPHA_SHIFT) | WHITE)
/* a flickering lamp shows in a step while its group's core_flicker is below
 * LIT_BELOW. flicker_lamps flicker in the second half of every FLICKER_PERIOD
 * steps, and are out in the first */
#define LIT_BELOW 0.5F
#define FLICKER_PERIOD 80
/* the steps after which the cylinders' motion and the shaft machinery's start
 * over */
#define HALF_SPEED 0.5F
#define CYLINDER_CYCLE 840
#define ASSEMBLY_CYCLE 798
/* a fan turns a degree a step, a whole turn every FAN_PERIOD steps.
 * reverse_fan turns the other way */
#define FAN_PERIOD 360
/* the bobbing water rises and falls once every BOB_PERIOD steps */
#define BOB_PERIOD 120
#define BOB_STEP_ANGLE (FULL_TURN / BOB_PERIOD)
/* where the scenery's single models stand: the two cylinders, the rotor and
 * the shaft's machinery, shaft_assembly */
#define FIRST_CYLINDER_X 0.0F
#define FIRST_CYLINDER_Y (-480.0F)
#define FIRST_CYLINDER_Z (-1580.0F)
#define SECOND_CYLINDER_X 240.0F
#define SECOND_CYLINDER_Y (-480.0F)
#define SECOND_CYLINDER_Z (-1780.0F)
#define ROTOR_X 0.0F
#define ROTOR_Y (-445.0F)
#define ROTOR_Z (-1780.0F)
#define ASSEMBLY_X (-510.0F)
#define ASSEMBLY_Y (-480.0F)
#define ASSEMBLY_Z (-1780.0F)
/* where the gate's collider and the entry's trigger stand. GATE_BAR_Y is the
 * center.y of the gate's bar, below its pos */
#define GATE_X (-510.0F)
#define GATE_Y (-500.0F)
#define GATE_Z (-2502.5F)
#define GATE_BAR_Y (-21.0F)
#define ENTRY_X 0.0F
#define ENTRY_Y (-480.0F)
#define ENTRY_Z (-1125.0F)

/* clang-format off */
static struct collision_shape pillar_shape[] = {PILLAR_SHAPE(27.5F, 700.0F)};
/* the bar's center.y, which core_barred_gate_step sets */
static Float gate_bar_y = GATE_BAR_Y;
static struct collision_shape fan_shape[] = {
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {0.0F, 0.0F, 0.0F}, 120.0F, 20.0F, 0.0F, 0,
     {0, 0, QUARTER_TURN}}};
/* the gate's wall, and a bar along its foot. core_gate_start's collider takes
 * the wall alone */
static struct collision_shape gate_shapes[] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3), 0,
     {0.0F, 0.0F, 0.0F}, 100.0F, 20.0F, 2.5F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     0, {0.0F, GATE_BAR_Y, 0.0F}, 2.5F, 100.0F, 0.0F, 0, {0, 0, QUARTER_TURN}}};
static struct collision_shape entry_shape[] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {0.0F, 0.0F, 0.0F}, 20.0F, 20.0F, 20.0F, 0, {0, 0, 0}}};
/* clang-format on */

static void collider_step(struct task *task);
static void entry_trigger_step(struct task *task);

static void draw_at_children(NJS_CNK_MODEL *model, NJS_CNK_OBJECT *placements,
                             Sint32 blocks)
{
    NJS_CNK_OBJECT *child;
    if (BLOCK_MASK & blocks) {
        for (child = placements->child; child; child = child->sibling) {
            njPushMatrixEx();
            njTranslate(NULL, child->pos[0], child->pos[1], child->pos[2]);
            njRotateXYZ(NULL, child->ang[0], child->ang[1], child->ang[2]);
            njCnkEasyDrawModel(model);
            njPopMatrixEx();
        }
    }
}

static void cylinder_lamps_draw(Float frame)
{
    NJS_CNK_OBJECT *child = cylinder_lamps->child;
    int group = 0;
    Uint32 color;
    if ((frame >= 0.0F && frame < LAMPS_ON) ||
        (frame >= LAMPS_OFF && frame < FLICKER_START) ||
        (frame >= FLICKER_END && frame < LAMPS_CYCLE)) {
        return;
    }
    njSetTexture(&cylinder_lamp_textures);
    for (; child; child = child->sibling, ++group) {
        njPushMatrixEx();
        njTranslate(NULL, child->pos[0], child->pos[1], child->pos[2]);
        njRotateXYZ(NULL, child->ang[0], child->ang[1], child->ang[2]);
        if (frame >= LAMPS_ON && frame < LAMPS_LIT) {
            color = FADE_TINT((frame - LAMPS_ON) * ALPHA_PER_STEP);
            model_draw_tinted(child->model, color);
        } else if (frame >= LAMPS_DIM && frame < LAMPS_OFF) {
            color = FADE_TINT((LAMPS_OFF - frame) * ALPHA_PER_STEP);
            model_draw_tinted(child->model, color);
        } else if (frame >= FLICKER_START && frame < FLICKER_END) {
            if (core_flicker[group % CORE_FLICKER_GROUPS] < LIT_BELOW) {
                njCnkEasyDrawModel(child->model);
            }
        } else {
            njCnkEasyDrawModel(child->model);
        }
        njPopMatrixEx();
    }
}

static void flicker_lamps_draw(void)
{
    NJS_CNK_OBJECT *child = flicker_lamps->child;
    int group = 0;
    if (game_state.steps % FLICKER_PERIOD >= FLICKER_PERIOD / 2) {
        njSetTexture(&flicker_lamp_textures);
        for (; child; child = child->sibling, ++group) {
            njPushMatrixEx();
            njTranslate(NULL, child->pos[0], child->pos[1], child->pos[2]);
            if (core_flicker[group % CORE_FLICKER_GROUPS] < LIT_BELOW) {
                njCnkEasyDrawModel(child->model);
            }
            njPopMatrixEx();
        }
    }
}

static void fans_draw(void)
{
    NJS_CNK_OBJECT *child = core_rotor_array->child;
    Angle angle;
    if (BLOCK_MASK & LAND_BLOCKS(3, 5)) {
        njSetTexture(&fan_textures);
        for (; child; child = child->sibling) {
            angle = (Angle)((Float)(game_state.steps % FAN_PERIOD) *
                            ANGLE_PER_DEGREE);
            if (child->model == reverse_fan) {
                angle = -angle;
            }
            njPushMatrixEx();
            njTranslate(NULL, child->pos[0], child->pos[1], child->pos[2]);
            njRotateX(NULL, angle);
            njCnkEasyDrawModel(child->model);
            njPopMatrixEx();
        }
    }
}

static void water_surfaces_draw(NJS_CNK_OBJECT *placements, int texture,
                                Sint32 blocks)
{
    NJS_CNK_OBJECT *child;
    Sint16 word;
    if (BLOCK_MASK & blocks) {
        for (child = placements->child; child; child = child->sibling) {
            /* @hack spelling: word, a local for the halfword's new value, where
             * one assignment to the halfword would do */
            word = (child->model->plist[WATER_TID] & NJD_FFM_MASK) | texture;
            child->model->plist[WATER_TID] = word;
            njPushMatrixEx();
            njTranslate(NULL, child->pos[0], child->pos[1], child->pos[2]);
            njRotateXYZ(NULL, child->ang[0], child->ang[1], child->ang[2]);
            njCnkEasyDrawModel(child->model);
            njPopMatrixEx();
        }
    }
}

static void waterfall_draw(NJS_CNK_MODEL *model, NJS_CNK_OBJECT *placements,
                           int texture, Sint32 blocks)
{
    NJS_CNK_OBJECT *child;
    Sint16 word;
    if (BLOCK_MASK & blocks) {
        for (child = placements->child; child; child = child->sibling) {
            /* @hack spelling: word, a local for the halfword's new value, built
             * in two statements, where one assignment to the halfword would
             * do */
            word = model->plist[WATER_TID] & NJD_FFM_MASK;
            word = word | texture;
            model->plist[WATER_TID] = word;
            njPushMatrixEx();
            njTranslate(NULL, child->pos[0], child->pos[1], child->pos[2]);
            njRotateXYZ(NULL, child->ang[0], child->ang[1], child->ang[2]);
            njCnkEasyDrawModel(model);
            njPopMatrixEx();
        }
    }
}

void core_scenery_draw(struct task *task)
{
    int frame;
    int sequence_frame;
    int flame_cycle;
    int water_texture;
    Float motion_frame;
    Sint16 word;

    /* the three walls' frame tables are of one length */
    frame = game_state.steps % FRAMES(inner_wall_frames);
    water_texture = game_state.steps % WATER_TEXTURES;
    njSetTexture(&inner_wall_textures);
    draw_control_set(DRAW_CONTROL_SHADOW);
    draw_at_children(inner_wall_frames[frame], &inner_wall_entry,
                     LAND_BLOCK(1));
    draw_at_children(inner_wall_frames[frame], &inner_wall_shaft,
                     LAND_BLOCKS(5, 8));
    njSetTexture(&middle_wall_textures);
    draw_at_children(middle_wall_frames[frame], &middle_wall_entry,
                     LAND_BLOCK(1));
    draw_at_children(middle_wall_frames[frame], &middle_wall_rotor,
                     LAND_BLOCKS(1, 3));
    draw_at_children(middle_wall_frames[frame], &middle_wall_drop,
                     LAND_BLOCKS(6, 9));
    njSetTexture(&outer_wall_textures);
    draw_at_children(outer_wall_frames[frame], &outer_wall_entry,
                     LAND_BLOCKS(1, 3));
    draw_at_children(outer_wall_frames[frame], &outer_wall_drop,
                     LAND_BLOCKS(7, 9));
    draw_control_clear(DRAW_CONTROL_SHADOW);

    if (BLOCK_MASK & LAND_BLOCKS(1, 3)) {
        motion_frame = (Float)(game_state.steps % CYLINDER_CYCLE) * HALF_SPEED;
        njPushMatrixEx();
        njTranslate(NULL, FIRST_CYLINDER_X, FIRST_CYLINDER_Y, FIRST_CYLINDER_Z);
        njSetTexture(&cylinder_textures);
        njCnkEasyDrawMotion(&cylinder_body, &cylinder_motion, motion_frame);
        njPopMatrixEx();
        njPushMatrixEx();
        njTranslate(NULL, SECOND_CYLINDER_X, SECOND_CYLINDER_Y,
                    SECOND_CYLINDER_Z);
        njRotateY(NULL, -QUARTER_TURN);
        njSetTexture(&cylinder_textures);
        njCnkEasyDrawMotion(&cylinder_body, &cylinder_motion, motion_frame);
        njPopMatrixEx();
        cylinder_lamps_draw((Float)(game_state.steps % LAMPS_CYCLE));
    }
    frame = HALF_STEPS % FRAMES(entrance_unit_frames);
    njSetTexture(&entrance_unit_textures);
    draw_at_children(entrance_unit_frames[frame], &entrance_units,
                     LAND_BLOCKS(1, 3));
    /* the left flames burn in every other run of entrance_flame_frames, and
     * the right flames in the runs between */
    sequence_frame = game_state.steps % FRAMES(entrance_flame_frames);
    flame_cycle = game_state.steps % (2 * FRAMES(entrance_flame_frames));
    njSetTexture(&entrance_flame_textures);
    draw_at_children(entrance_flame_frames[sequence_frame], &entrance_flames,
                     LAND_BLOCKS(1, 3));
    /* @hack spelling: a conditional expression of the two calls, where one
     * call whose placements flame_cycle picks would do */
    flame_cycle < FRAMES(entrance_flame_frames)
        ? draw_at_children(entrance_flame_frames[sequence_frame], &left_flames,
                           LAND_BLOCKS(1, 3))
        : draw_at_children(entrance_flame_frames[sequence_frame], &right_flames,
                           LAND_BLOCKS(1, 3));

#ifdef NONMATCHING
#else
    /* @hack redundant: a push, three translations, a draw and two turns, in a
     * block no path reaches */
    if (0) {
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, 0.0F, 0.0F);
        njTranslate(NULL, 0.0F, 0.0F, 0.0F);
        njTranslate(NULL, 0.0F, 0.0F, 0.0F);
        draw_at_children(pillar_frames[sequence_frame], &right_flames,
                         LAND_BLOCKS(1, 3));
        njRotateXYZ(NULL, 0, 0, 0);
        njRotateXYZ(NULL, 0, 0, 0);
    }
#endif
    frame = game_state.steps % FRAMES(rotor_flame_frames);
    njSetTexture(&rotor_flame_textures);
    draw_at_children(rotor_flame_frames[frame], &rotor_flames,
                     LAND_BLOCKS(2, 4));
    frame = HALF_STEPS % FRAMES(rotor_wall_frames);
    njSetTexture(&rotor_wall_textures);
    draw_at_children(rotor_wall_frames[frame], &rotor_walls, LAND_BLOCKS(1, 4));
    if (BLOCK_MASK & LAND_BLOCKS(1, 4)) {
        njSetTexture(&rotor_textures);
        njPushMatrixEx();
        njTranslate(NULL, ROTOR_X, ROTOR_Y, ROTOR_Z);
        njRotateZ(NULL, core_rotor_angle);
        njCnkEasyDrawModel(&rotor_body);
        if (core_rotor_turning == ROTOR_TURNING) {
            njCnkEasyDrawModel(&rotor_highlight);
        }
        njPopMatrixEx();
    }
    frame = game_state.steps % FRAMES(shaft_flame_frames);
    njSetTexture(&shaft_flame_textures);
    draw_at_children(shaft_flame_frames[frame], &shaft_flames,
                     LAND_BLOCKS(3, 5));
    frame = HALF_STEPS % FRAMES(shaft_unit_frames);
    njSetTexture(&shaft_unit_textures);
    draw_at_children(shaft_unit_frames[frame], &shaft_units, LAND_BLOCKS(3, 6));
    if (BLOCK_MASK & LAND_BLOCKS(3, 6)) {
        motion_frame = (Float)(game_state.steps % ASSEMBLY_CYCLE) * HALF_SPEED;
        njPushMatrixEx();
        njTranslate(NULL, ASSEMBLY_X, ASSEMBLY_Y, ASSEMBLY_Z);
        njSetTexture(&shaft_motion_textures);
        njCnkEasyDrawMotion(&shaft_assembly, &core_shaft_motion, motion_frame);
        njPopMatrixEx();
    }
    if (BLOCK_MASK & LAND_BLOCKS(3, 4)) {
        flicker_lamps_draw();
    }
    frame = HALF_STEPS % FRAMES(fan_wall_frames);
    njSetTexture(&fan_wall_textures);
    draw_at_children(fan_wall_frames[frame], &fan_walls, LAND_BLOCKS(4, 7));
    if (BLOCK_MASK & LAND_BLOCKS(4, 7)) {
        njSetTexture(&shutter_textures);
        if (game_state.steps & 1) {
            njCnkEasyDrawObject(&shutter_odd);
        } else {
            njCnkEasyDrawObject(&shutter_even);
        }
        if (BLOCK_MASK & LAND_BLOCKS(4, 7)) {
            /* @hack spelling: word, a local for the halfword's new value, read
             * and then set in two statements, where one assignment to the
             * halfword would do */
            word = bobbing_water_model->plist[WATER_TID];
            word = (word & NJD_FFM_MASK) | water_texture;
            bobbing_water_model->plist[WATER_TID] = word;
            njPushMatrixEx();
            njTranslate(NULL, 0.0F,
                        njCos(game_state.steps % BOB_PERIOD * BOB_STEP_ANGLE),
                        0.0F);
            njSetTexture(&water_surface_textures);
            njCnkEasyDrawObject(&bobbing_water);
            njPopMatrixEx();
        }
    }
    frame = game_state.steps % PILLAR_FRAMES;
    draw_at_children(pillar_frames[frame], &core_pillar_upper,
                     LAND_BLOCKS(8, 9));
    draw_at_children(pillar_frames[frame], &core_pillar_lower,
                     LAND_BLOCKS(9, 10));
    fans_draw();
    /* the channels' frame tables are as long as water_unit_frames */
    frame = HALF_STEPS % FRAMES(water_unit_frames);
    njSetTexture(&water_unit_textures);
    draw_at_children(water_unit_frames[frame], &water_units, LAND_BLOCKS(8, 9));
    njSetTexture(&upper_channel_textures);
    draw_at_children(upper_channel_frames[frame], &upper_channel_entry,
                     LAND_BLOCKS(8, 9));
    draw_at_children(upper_channel_frames[frame], &upper_channel_exit,
                     LAND_BLOCKS(9, 10));
    njSetTexture(&middle_channel_textures);
    draw_at_children(middle_channel_frames[frame], &middle_channel_entry,
                     LAND_BLOCKS(8, 9));
    draw_at_children(middle_channel_frames[frame], &middle_channel_exit,
                     LAND_BLOCKS(9, 10));
    njSetTexture(&lower_channel_textures);
    draw_at_children(lower_channel_frames[frame], &lower_channel_entry,
                     LAND_BLOCKS(7, 9));
    draw_at_children(lower_channel_frames[frame], &lower_channel_exit,
                     LAND_BLOCKS(9, 10));
    njSetTexture(&water_surface_textures);
    water_surfaces_draw(&water_turn_entry, water_texture, LAND_BLOCKS(8, 9));
    water_surfaces_draw(&water_turn_exit, water_texture, LAND_BLOCKS(9, 10));
    water_surfaces_draw(&water_upper, water_texture, LAND_BLOCKS(10, 11));
    water_surfaces_draw(&water_middle, water_texture, LAND_BLOCKS(11, 12));
    water_surfaces_draw(&water_lower, water_texture, LAND_BLOCKS(12, 13));
    frame = game_state.steps % FRAMES(falling_water_frames);
    waterfall_draw(falling_water_frames[frame], &falling_water, water_texture,
                   LAND_BLOCKS(7, 10));
    frame = game_state.steps % FRAMES(waterfall_frames);
    sequence_frame = HALF_STEPS % WATERFALL_TEXTURES;
    njSetTexture(&waterfall_textures);
    waterfall_draw(waterfall_frames[frame], &waterfalls, sequence_frame,
                   LAND_BLOCKS(8, 9));
    njSetTexture(&water_surface_textures);
    water_surfaces_draw(&water_end, water_texture, LAND_BLOCK(13));
}

void core_pillars_start(NJS_CNK_OBJECT *placements, Sint32 blocks)
{
    NJS_CNK_OBJECT *child;
    struct task *pillar;
    struct task_work *work;
    for (child = placements->child; child; child = child->sibling) {
        pillar =
            task_create(TASK_WORK, COLLIDER_LEVEL, collider_step, "DenkiColi");
        if (!pillar) {
            return;
        }
        work = pillar->work;
        work->pos.x = child->pos[0];
        work->pos.y = child->pos[1];
        work->pos.z = child->pos[2];
        collision_init(pillar, pillar_shape, PILLAR_SHAPES, COLLISION_OBJECT);
        COLLIDER_BLOCKS(pillar) = blocks;
    }
}

static void collider_step(struct task *task)
{
    if (BLOCK_MASK & COLLIDER_BLOCKS(task)) {
        collision_register(task);
    }
}

void core_barred_gate_step(struct task *task)
{
    struct task_work *work = task->work;
    if (BLOCK_MASK & COLLIDER_BLOCKS(task)) {
        work->collision->shapes[1].center.y = gate_bar_y;
        collision_fit_radius(task);
        collision_register(task);
    }
}

void core_fans_start(NJS_CNK_OBJECT *placements, Sint32 blocks)
{
    NJS_CNK_OBJECT *child;
    struct task *fan;
    struct task_work *work;
    for (child = placements->child; child; child = child->sibling) {
        fan =
            task_create(TASK_WORK, COLLIDER_LEVEL, collider_step, "DenkiColi");
        if (!fan) {
            return;
        }
        work = fan->work;
        work->pos.x = child->pos[0];
        work->pos.y = child->pos[1];
        work->pos.z = child->pos[2];
        collision_init(fan, fan_shape, FAN_SHAPES, COLLISION_OBJECT);
        COLLIDER_BLOCKS(fan) = blocks;
    }
}

void core_gate_start(Sint32 blocks)
{
    struct task *gate;
    struct task_work *work;
    gate = task_create(TASK_WORK, COLLIDER_LEVEL, collider_step, "DenkiColi");
    if (gate) {
        work = gate->work;
        work->pos.x = GATE_X;
        work->pos.y = GATE_Y;
        work->pos.z = GATE_Z;
        collision_init(gate, gate_shapes, GATE_SHAPES, COLLISION_OBJECT);
        COLLIDER_BLOCKS(gate) = blocks;
    }
}

void core_entry_trigger_start(void)
{
    struct task *entry;
    struct task_work *work;
    entry =
        task_create(TASK_WORK, COLLIDER_LEVEL, entry_trigger_step, "InputColi");
    if (entry) {
        work = entry->work;
        work->pos.x = ENTRY_X;
        work->pos.y = ENTRY_Y;
        work->pos.z = ENTRY_Z;
        collision_init(entry, entry_shape, ENTRY_SHAPES, COLLISION_OBJECT);
        pad_disable_player(0);
    }
}

static void entry_trigger_step(struct task *task)
{
    struct task *player;
    if ((task->work->collision->flags & COLLISION_TOUCHED) &&
        (player = collision_next_player(task)) != NULL &&
        player_records[player_slot_of(player)]->character == PLAYER_SONIC) {
        pad_enable_player(0);
        task_kill(task);
    } else {
        collision_register(task);
        pad_disable_player(0);
    }
}
