#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/chunk.h"
#include "effect/particle.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dynamite.h"
#include "shared/shutter_motion.h"
#include "shared/switch.h"
#include "sound/sound.h"
#include "stg22/shutter.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "stg22/shutter/text.inc"

/* the set editor's label for scl.x */
static const char open_range_caption[20] = {
#include "stg22/shutter/open_range_caption.inc"
};

static NJS_TEXNAME gadget_shutter_front_textures[11] = {
#include "stg22/shutter/gadget_shutter_front_textures.inc"
};

static NJS_TEXLIST gadget_shutter_front_texlist = {
#include "stg22/shutter/gadget_shutter_front_texlist.inc"
};

static Sint16 gadget_shutter_lamp_polygons[25] = {
#include "stg22/shutter/gadget_shutter_lamp_polygons.inc"
};

static Sint32 gadget_shutter_lamp_vertices[19] = {
#include "stg22/shutter/gadget_shutter_lamp_vertices.inc"
};

static NJS_CNK_MODEL gadget_shutter_lamp_mesh = {
#include "stg22/shutter/gadget_shutter_lamp_mesh.inc"
};

static NJS_CNK_OBJECT gadget_shutter_lamp[1] = {
#include "stg22/shutter/gadget_shutter_lamp.inc"
};

static Sint16 gadget_shutter_right_polygons[295] = {
#include "stg22/shutter/gadget_shutter_right_polygons.inc"
};

static Sint32 gadget_shutter_right_vertices[263] = {
#include "stg22/shutter/gadget_shutter_right_vertices.inc"
};

static NJS_CNK_MODEL gadget_shutter_right_mesh = {
#include "stg22/shutter/gadget_shutter_right_mesh.inc"
};

static NJS_CNK_OBJECT gadget_shutter_right[1] = {
#include "stg22/shutter/gadget_shutter_right.inc"
};

static Sint16 gadget_shutter_left_polygons[161] = {
#include "stg22/shutter/gadget_shutter_left_polygons.inc"
};

static Sint32 gadget_shutter_left_vertices[155] = {
#include "stg22/shutter/gadget_shutter_left_vertices.inc"
};

static NJS_CNK_MODEL gadget_shutter_left_mesh = {
#include "stg22/shutter/gadget_shutter_left_mesh.inc"
};

static NJS_CNK_OBJECT gadget_shutter_left[1] = {
#include "stg22/shutter/gadget_shutter_left.inc"
};

static Sint16 gadget_shutter_stop_lamp_polygons[15] = {
#include "stg22/shutter/gadget_shutter_stop_lamp_polygons.inc"
};

static Sint32 gadget_shutter_stop_lamp_vertices[19] = {
#include "stg22/shutter/gadget_shutter_stop_lamp_vertices.inc"
};

static NJS_CNK_MODEL gadget_shutter_stop_lamp_mesh = {
#include "stg22/shutter/gadget_shutter_stop_lamp_mesh.inc"
};

static NJS_CNK_OBJECT gadget_shutter_stop_lamp[1] = {
#include "stg22/shutter/gadget_shutter_stop_lamp.inc"
};

static Sint16 gadget_shutter_top_polygons[359] = {
#include "stg22/shutter/gadget_shutter_top_polygons.inc"
};

static Sint32 gadget_shutter_top_vertices[319] = {
#include "stg22/shutter/gadget_shutter_top_vertices.inc"
};

static NJS_CNK_MODEL gadget_shutter_top_mesh = {
#include "stg22/shutter/gadget_shutter_top_mesh.inc"
};

static NJS_CNK_OBJECT gadget_shutter_top[1] = {
#include "stg22/shutter/gadget_shutter_top.inc"
};

static Sint16 gadget_shutter_latch_polygons[269] = {
#include "stg22/shutter/gadget_shutter_latch_polygons.inc"
};

static Sint32 gadget_shutter_latch_vertices[155] = {
#include "stg22/shutter/gadget_shutter_latch_vertices.inc"
};

static NJS_CNK_MODEL gadget_shutter_latch_mesh = {
#include "stg22/shutter/gadget_shutter_latch_mesh.inc"
};

static NJS_CNK_OBJECT gadget_shutter_latch[1] = {
#include "stg22/shutter/gadget_shutter_latch.inc"
};

static Sint16 gadget_shutter_go_lamp_polygons[27] = {
#include "stg22/shutter/gadget_shutter_go_lamp_polygons.inc"
};

static Sint32 gadget_shutter_go_lamp_vertices[51] = {
#include "stg22/shutter/gadget_shutter_go_lamp_vertices.inc"
};

static NJS_CNK_MODEL gadget_shutter_go_lamp_mesh = {
#include "stg22/shutter/gadget_shutter_go_lamp_mesh.inc"
};

static NJS_CNK_OBJECT gadget_shutter_go_lamp[1] = {
#include "stg22/shutter/gadget_shutter_go_lamp.inc"
};

static NJS_CNK_OBJECT front_panel_root = {
#include "stg22/shutter/front_panel_root.inc"
};
static NJS_TEXNAME gadget_shutter_back_textures[6] = {
#include "stg22/shutter/gadget_shutter_back_textures.inc"
};

static NJS_TEXLIST gadget_shutter_back_texlist = {
#include "stg22/shutter/gadget_shutter_back_texlist.inc"
};

static Sint16 gadget_shutter_back_stop_lamp_polygons[15] = {
#include "stg22/shutter/gadget_shutter_back_stop_lamp_polygons.inc"
};

static Sint32 gadget_shutter_back_stop_lamp_vertices[19] = {
#include "stg22/shutter/gadget_shutter_back_stop_lamp_vertices.inc"
};

static NJS_CNK_MODEL gadget_shutter_back_stop_lamp_mesh = {
#include "stg22/shutter/gadget_shutter_back_stop_lamp_mesh.inc"
};

static NJS_CNK_OBJECT gadget_shutter_back_stop_lamp[1] = {
#include "stg22/shutter/gadget_shutter_back_stop_lamp.inc"
};

static Sint16 gadget_shutter_back_lamp_polygons[25] = {
#include "stg22/shutter/gadget_shutter_back_lamp_polygons.inc"
};

static Sint32 gadget_shutter_back_lamp_vertices[19] = {
#include "stg22/shutter/gadget_shutter_back_lamp_vertices.inc"
};

static NJS_CNK_MODEL gadget_shutter_back_lamp_mesh = {
#include "stg22/shutter/gadget_shutter_back_lamp_mesh.inc"
};

static NJS_CNK_OBJECT gadget_shutter_back_lamp[1] = {
#include "stg22/shutter/gadget_shutter_back_lamp.inc"
};

static Sint16 gadget_shutter_back_polygons[475] = {
#include "stg22/shutter/gadget_shutter_back_polygons.inc"
};

static Sint32 gadget_shutter_back_vertices[347] = {
#include "stg22/shutter/gadget_shutter_back_vertices.inc"
};

static NJS_CNK_MODEL gadget_shutter_back_mesh = {
#include "stg22/shutter/gadget_shutter_back_mesh.inc"
};

static NJS_CNK_OBJECT gadget_shutter_back[1] = {
#include "stg22/shutter/gadget_shutter_back.inc"
};
NJS_TEXLIST *gadget_shutter_texlists[3] = {
#include "stg22/shutter/gadget_shutter_texlists.inc"
};
/* the set editor's names for ang.z's triggers */
static char *trigger_names[4] = {
#include "stg22/shutter/trigger_names.inc"
};

static Float slide_speed = 0.015F;
static Float lift_speed = 0.05F;
/* the shutter's shapes while it is closed, before ang.y turns them. the floor
 * lies under the panels, and stays as the panels open */
enum shutter_shape { RIGHT_PANEL, LEFT_PANEL, FLOOR, SHAPES };
/* clang-format off */
static struct collision_shape closed_shapes[SHAPES] = {
    {0, SHAPE_AXIS_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNTURNED,
     {20, 30, 0}, 60, 30, 18, 0, {0, 0, 0}},
    {0, SHAPE_AXIS_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {-55, 30, 0}, 25, 30, 18, 0, {0, 0, 0}},
    {0, SHAPE_AXIS_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {-30, -5.5F, 0}, 100, 5, 20, 0, {0, 0, 0}},
};
/* clang-format on */
static Float opening_kick[TRACKS] = {1.0F, 1.5F, 2.0F};
static Float closing_kick[TRACKS] = {-1.0F, -1.5F, -2.0F};

/* the shutter's trigger, by shared/shutter_motion.h's enum shutter_trigger */
/* @hack spelling: ang.z cut to a byte and taken mod 4, where ang.z & 3 would
 * do */
#define TRIGGER(work) ((Uint8)(work)->ang.z % 4)
/* an enemy shutter opens only with a player within scl.x + RANGE_ADDED of its
 * pos */
#define RANGE_ADDED 100.0F
/* a closed shutter whose face turns from the camera draws its back. it does so
 * only where the view z of its pos passes BACK_DEPTH */
#define BACK_DEPTH 100.0F
#define PLAYER_INSIDE(point)                                                   \
    ((point).y < 80.0F && (point).y > -10.0F && njAbs((point).x) < 100.0F &&   \
     (point).z < 150.0F && (point).z > -80.0F)
/* turn from's centers and closed_shapes' sizes by ang.y into the collision
 * record's shapes. a turned size's components are made positive. it takes
 * work, i and size from the caller */
#define TURN_SHAPES(from)                                                      \
    njPushMatrixEx();                                                          \
    njUnitMatrix(NULL);                                                        \
    njRotateY(NULL, work->ang.y);                                              \
    for (i = 0; i < SHAPES; i++) {                                             \
        njCalcPoint(NULL, &(from)[i].center,                                   \
                    &work->collision->shapes[i].center);                       \
    }                                                                          \
    for (i = 0; i < SHAPES; i++) {                                             \
        size.x = closed_shapes[i].radius;                                      \
        size.y = closed_shapes[i].size_y;                                      \
        size.z = closed_shapes[i].size_z;                                      \
        njCalcPoint(NULL, &size, &size);                                       \
        work->collision->shapes[i].radius = njAbs(size.x);                     \
        work->collision->shapes[i].size_y = njAbs(size.y);                     \
        work->collision->shapes[i].size_z = njAbs(size.z);                     \
    }                                                                          \
    njPopMatrixEx()

static void shutter_destroy(struct task *owner);
static void shutter_step(struct task *owner);
static void shutter_draw(struct task *task);

void gadget_shutter_init(struct task *task)
{
    struct task_work *work = task->work;
    int i;
    NJS_VECTOR size;
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct shutter_motion));
    if (!task->motion.p) {
        return;
    }
    task->draw = shutter_draw;
    task->step = shutter_step;
    task->destructor = shutter_destroy;
    /* @hack spelling: the call of collision_init in the test's comma
     * expression, where a statement of its own would do */
    if (collision_init(task, closed_shapes, SHAPES, COLLISION_OBJECT),
        work->collision) {
        TURN_SHAPES(closed_shapes);
    }
    work->mode = CLOSED;
}

static void shutter_destroy(struct task *owner)
{
    /* @hack spelling: task, a copy of owner, where the parameter would do */
    struct task *task = owner;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static Bool shutter_triggered(struct task *task)
{
    struct task_work *work = task->work;
    Bool inside;
    NJS_POINT3 point;
    switch (TRIGGER(work)) {
    case BY_SWITCH:
        return switch_is_on((Uint8)work->ang.x);
    case BY_ENEMIES:
        return enemy_group_empty((Uint8)work->ang.x);
    case BY_DYNAMITE:
        return !dynamite_count((Uint8)work->ang.x);
    case BY_PROXIMITY:
        inside = FALSE;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, -work->ang.y);
        njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
        if (player_work[0]) {
            njCalcPoint(NULL, &player_work[0]->pos, &point);
            if (PLAYER_INSIDE(point)) {
                inside = TRUE;
            }
        }
        if (player_work[1]) {
            njCalcPoint(NULL, &player_work[1]->pos, &point);
            if (PLAYER_INSIDE(point)) {
                inside = TRUE;
            }
        }
        njPopMatrixEx();
        return inside;
    }
    return TRUE;
}

static void shutter_step(struct task *owner)
{
    /* @hack spelling: task, a copy of owner, where the parameter would do */
    struct task *task = owner;
    struct task_work *work = task->work;
    int i;
    /* @hack spelling: closing_track, SETTLING's own counter, where i would
     * do */
    int closing_track;
    struct collision_shape shapes[SHAPES];
    NJS_VECTOR size;

    /* @hack spelling: an else after the return, where an if of its own would
     * do */
    if (placement_try_leave(task)) {
        return;
    } else if (shutter_triggered(task)) {
        if (work->mode == CLOSED) {
            if (TRIGGER(work) != BY_ENEMIES ||
                player_near(&work->pos, work->scl.x + RANGE_ADDED)) {
                /* @bug logic: a proximity shutter that has closed unlocks for
                 * one step, not UNLOCK_STEPS + 1. SETTLING leaves the timer
                 * past UNLOCK_STEPS. this store doesn't reset it, so the test
                 * of the timer ends UNLOCKING on its first step. the store and
                 * the test lie at 0x8c50804c and 0x8c5080fa */
                work->mode = UNLOCKING;
                sound_play(START_SOUND, NULL, 0, 0);
            }
        }
    } else if (TRIGGER(work) == BY_PROXIMITY) {
        if (work->mode == OPEN) {
            work->mode = CLOSING;
        }
    } else {
        work->mode = CLOSED;
    }
    /* @hack redundant: a store of OPENING to the mode, in a block no path
     * reaches */
    if (0) {
        work->mode = OPENING;
    }
    /* @hack redundant: a store of 0 to the timer, in a block no path
     * reaches */
    if (0) {
        work->timer = 0;
    }
    switch (work->mode) {
    case CLOSED:
        MOTION(task)->closed_light = 1.0F;
        MOTION(task)->open_light = 0.0F;
        MOTION(task)->opening = 0.0F;
        MOTION(task)->lift = 0.0F;
        break;
    case UNLOCKING:
        MOTION(task)->closed_light = 1.0F;
        MOTION(task)->open_light = 1.0F;
        work->timer++;
        if (work->timer > UNLOCK_STEPS) {
            work->mode = LIFTING;
            work->timer = 0;
        }
        break;
    case LIFTING:
        MOTION(task)->closed_light = 0.0F;
        MOTION(task)->open_light = 1.0F;
        MOTION(task)->lift += lift_speed;
        if (MOTION(task)->lift > 1.0F) {
            MOTION(task)->lift = 1.0F;
            work->mode = OPENING;
        }
        break;
    case OPENING:
        MOTION(task)->closed_light = 0.0F;
        MOTION(task)->open_light = 1.0F;
        MOTION(task)->opening += slide_speed;
        if (MOTION(task)->opening > 1.0F) {
            MOTION(task)->opening = 1.0F;
            work->mode = OPEN;
            sound_play(STOP_SOUND, NULL, 0, 0);
            {
                /* @hack spelling: the opening kicks read through a pointer to
                 * the first, where opening_kick[i] would do */
                const Float *kick = &opening_kick[0];
                for (i = 0; i < TRACKS; i++) {
                    MOTION(task)->velocity[i] = kick[i];
                }
            }
        }
        break;
    case OPEN:
        MOTION(task)->open_light = 1.0F;
        MOTION(task)->closed_light = 0.0F;
        for (i = 0; i < TRACKS; i++) {
            MOTION(task)->offset[i] += MOTION(task)->velocity[i];
            MOTION(task)->velocity[i] =
                SPRING_VELOCITY(MOTION(task), i, SPRING_INCREMENT);
            if (MOTION(task)->offset[i] < 0.0F) {
                MOTION(task)->offset[i] = 0.0F;
                MOTION(task)->velocity[i] =
                    njAbs(MOTION(task)->velocity[i]) * BOUNCE;
            }
        }
        break;
    case CLOSING:
        MOTION(task)->closed_light = 1.0F;
        MOTION(task)->open_light = 1.0F;
        MOTION(task)->lift -= lift_speed;
        if (MOTION(task)->lift <= 0.0F) {
            MOTION(task)->lift = 0.0F;
            MOTION(task)->opening -= slide_speed;
            if (MOTION(task)->opening < 0.0F) {
                MOTION(task)->opening = 0.0F;
                work->mode = SETTLING;
                work->timer = 0;
                for (i = 0; i < TRACKS; i++) {
                    MOTION(task)->velocity[i] = closing_kick[i];
                }
            }
        }
        break;
    case SETTLING: {
        /* @hack spelling: restitution, a local for BOUNCE, where BOUNCE in
         * place would do, as in OPEN */
        const Float restitution = BOUNCE;
        MOTION(task)->open_light = 0.0F;
        MOTION(task)->closed_light = 1.0F;
        for (closing_track = 0; closing_track < TRACKS; closing_track++) {
            MOTION(task)->offset[closing_track] +=
                MOTION(task)->velocity[closing_track];
            MOTION(task)->velocity[closing_track] =
                SPRING_VELOCITY(MOTION(task), closing_track, SPRING_INCREMENT);
            if (MOTION(task)->offset[closing_track] > 0.0F) {
                MOTION(task)->offset[closing_track] = 0.0F;
                MOTION(task)->velocity[closing_track] = -(
                    njAbs(MOTION(task)->velocity[closing_track]) * restitution);
            }
        }
        work->timer++;
        if (work->timer > SETTLE_STEPS) {
            work->mode = CLOSED;
        }
    }
    }
    if (work->collision) {
        shapes[RIGHT_PANEL] = closed_shapes[RIGHT_PANEL];
        shapes[LEFT_PANEL] = closed_shapes[LEFT_PANEL];
        shapes[FLOOR] = closed_shapes[FLOOR];
        shapes[RIGHT_PANEL].center.x = closed_shapes[RIGHT_PANEL].center.x +
                                       (MOTION(task)->opening * RIGHT_TRAVEL);
        shapes[LEFT_PANEL].center.x = closed_shapes[LEFT_PANEL].center.x -
                                      (MOTION(task)->opening * LEFT_TRAVEL);
        TURN_SHAPES(shapes);
        collision_fit_radius(task);
        collision_register(task);
    }
}

#define DRAW_TINTED(node, dx, color)                                           \
    MOVE_X(node, dx);                                                          \
    model_draw_tinted((node)->model, color);                                   \
    matrix_load_parent()

static void shutter_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 player;
    NJS_POINT3 position;
    NJS_VECTOR normal;
    Uint32 closed_light;
    Uint32 open_light;
    Uint32 main_light;
    Float left;
    Float right;
    Float top;

    njCalcPoint(NULL, &player_work[camera_current_index]->pos, &player);
    njCalcPoint(NULL, &work->pos, &position);
    if (player.z > position.z && !game_state.demo) {
        draw_control_save();
        draw_attr_save();
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        njSetTexture(&gadget_shutter_front_texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njPushMatrixEx();
        left = -(MOTION(task)->opening * LEFT_TRAVEL) +
               MOTION(task)->offset[LEFT_TRACK];
        right = (MOTION(task)->opening * RIGHT_TRAVEL) -
                MOTION(task)->offset[RIGHT_TRACK];
        /* @hack redundant: top worked out in the translucent pass, which draws
         * only the side panels */
        top = (MOTION(task)->opening * TOP_TRAVEL) -
              MOTION(task)->offset[TOP_TRACK];
        DRAW_TINTED(gadget_shutter_left, left, TRANSLUCENT);
        DRAW_TINTED(gadget_shutter_right, right, TRANSLUCENT);
        njPopMatrix(2);
        draw_attr_restore();
        draw_control_restore();
    } else {
        closed_light = color_blend(
            SHUTTER_DARK, SHUTTER_LIT,
            blink_level(STOP_BLINK_PERIOD, STOP_BLINK_REST, STOP_BLINK_FADE) *
                MOTION(task)->closed_light);
        open_light = color_blend(
            SHUTTER_DARK, SHUTTER_LIT,
            blink_level(GO_BLINK_PERIOD, GO_BLINK_REST, GO_BLINK_FADE) *
                MOTION(task)->open_light);
        main_light =
            work->mode == CLOSED
                ? color_blend(SHUTTER_DARK, SHUTTER_LIT,
                              blink_level(MAIN_BLINK_PERIOD, MAIN_BLINK_REST,
                                          MAIN_BLINK_FADE))
                : SHUTTER_LIT;
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njPushMatrixEx();
        left = -(MOTION(task)->opening * LEFT_TRAVEL) +
               MOTION(task)->offset[LEFT_TRACK];
        right = (MOTION(task)->opening * RIGHT_TRAVEL) -
                MOTION(task)->offset[RIGHT_TRACK];
        top = (MOTION(task)->opening * TOP_TRAVEL) -
              MOTION(task)->offset[TOP_TRACK];
        normal.x = 0.0F;
        normal.z = 1.0F;
        normal.y = 0.0F;
        njCalcVector(NULL, &normal, &normal);
        if (work->mode == CLOSED && position.z > BACK_DEPTH &&
            normal.z < 0.0F) {
            njSetTexture(&gadget_shutter_back_texlist);
            njCnkEasyDrawModel(gadget_shutter_back->model);
            njFogDisable();
            if (TRIGGER(work) != BY_PROXIMITY) {
                MOVE_LATCH(gadget_shutter_back_stop_lamp, right,
                           MOTION(task)->lift);
                model_draw_tinted(gadget_shutter_back_stop_lamp->model,
                                  closed_light);
                matrix_load_parent();
            }
            MOVE_X(gadget_shutter_back_lamp, right);
            model_draw_tinted(gadget_shutter_back_lamp->model, main_light);
            njFogEnable();
        } else {
            njSetTexture(&gadget_shutter_front_texlist);
            MOVE_X(gadget_shutter_left, left);
            njCnkEasyDrawModel(gadget_shutter_left->model);
            matrix_load_parent();
            MOVE_LATCH(gadget_shutter_latch, right, MOTION(task)->lift);
            njCnkEasyDrawModel(gadget_shutter_latch->model);
            matrix_load_parent();
            njFogDisable();
            if (TRIGGER(work) != BY_PROXIMITY) {
                MOVE_LATCH(gadget_shutter_stop_lamp, right, MOTION(task)->lift);
                model_draw_tinted(gadget_shutter_stop_lamp->model,
                                  closed_light);
                matrix_load_parent();
            }
            MOVE_LATCH(gadget_shutter_go_lamp, right, MOTION(task)->lift);
            model_draw_tinted(gadget_shutter_go_lamp->model, open_light);
            matrix_load_parent();
            DRAW_TINTED(gadget_shutter_lamp, right, main_light);
            njFogEnable();
            MOVE_X(gadget_shutter_right, right);
            njCnkEasyDrawModel(gadget_shutter_right->model);
            matrix_load_parent();
            njTranslate(NULL, gadget_shutter_top->pos[0],
                        gadget_shutter_top->pos[1] + top,
                        gadget_shutter_top->pos[2]);
            njCnkEasyDrawModel(gadget_shutter_top->model);
            matrix_load_parent();
        }
        njPopMatrix(2);
    }
}
