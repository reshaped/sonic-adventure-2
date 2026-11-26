#include "shared/deck_radar_s/text.inc"
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "hud/hud.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/deck_signal.h"
#include "sound/sound.h"
#include "system/task.h"

/* the set editor's labels for ang.x and ang.y, which no code reads. ang.x's
 * two name its crash modes: whether the radar's pieces land at pos.y or fall
 * past it */
static const char editor_pass_through[24] = {
#include "shared/deck_radar_s/editor_pass_through.inc"
};
static const char editor_reflect[24] = {
#include "shared/deck_radar_s/editor_reflect.inc"
};
static const char editor_rotation_y[12] = {
#include "shared/deck_radar_s/editor_rotation_y.inc"
};

#define RADAR_SOUND 7
#define RADAR_SOUND_STEPS 10
/* BROKEN is the placement's state for a broken radar. it is also the work's
 * submode bit that a hit on the weak point sets */
#define BROKEN 1
/* the weak point stands TARGET_HEIGHT above pos */
#define TARGET_HEIGHT 30.0F
/* the work's tick counts the steps from the break, up to TICK_LIMIT. the
 * pieces fly at a count of 1, and the dish still draws below DISH_DRAW_STEPS.
 * a radar that loads broken starts at RELOAD_TICK, past both */
#define TICK_LIMIT 100
#define DISH_DRAW_STEPS 5
#define RELOAD_TICK 10
#define PIECES 3
/* the pieces fly at PIECE_SPEED and up to PIECE_SPREAD more, and rise at
 * PIECE_RISE */
#define PIECE_SPEED 0.8F
#define PIECE_SPREAD 1.0F
#define PIECE_RISE 0.8F
/* the radar's shapes, and the one a break turns off */
#define RADAR_SHAPES 2
#define INTACT_SHAPE 1
/* CRASH_THROUGH is ang.x's bit for the crash mode whose pieces' floor is
 * THROUGH_FLOOR, far below the deck */
#define CRASH_THROUGH 1
#define THROUGH_FLOOR (-100000.0F)
#define BREAK_POINTS 20

static NJS_TEXNAME deck_radar_s_textures[10] = {
#include "shared/deck_radar_s/deck_radar_s_textures.inc"
};

static NJS_TEXLIST deck_radar_s_texlist = {
#include "shared/deck_radar_s/deck_radar_s_texlist.inc"
};

static Sint16 deck_radar_s_dish_polygons[477] = {
#include "shared/deck_radar_s/deck_radar_s_dish_polygons.inc"
};

static Sint32 deck_radar_s_dish_vertices[359] = {
#include "shared/deck_radar_s/deck_radar_s_dish_vertices.inc"
};

static NJS_CNK_MODEL deck_radar_s_dish_model = {
#include "shared/deck_radar_s/deck_radar_s_dish_model.inc"
};

static NJS_CNK_OBJECT deck_radar_s_dish = {
#include "shared/deck_radar_s/deck_radar_s_dish.inc"
};

static Sint16 deck_radar_s_polygons[379] = {
#include "shared/deck_radar_s/deck_radar_s_polygons.inc"
};

static Sint32 deck_radar_s_vertices[307] = {
#include "shared/deck_radar_s/deck_radar_s_vertices.inc"
};

static NJS_CNK_MODEL deck_radar_s_model = {
#include "shared/deck_radar_s/deck_radar_s_model.inc"
};

static NJS_CNK_OBJECT deck_radar_s_object = {
#include "shared/deck_radar_s/deck_radar_s_object.inc"
};

static NJS_TEXNAME deck_radar_s_broken_textures[6] = {
#include "shared/deck_radar_s/deck_radar_s_broken_textures.inc"
};

static NJS_TEXLIST deck_radar_s_broken_texlist = {
#include "shared/deck_radar_s/deck_radar_s_broken_texlist.inc"
};

static Sint16 deck_radar_s_broken_polygons[265] = {
#include "shared/deck_radar_s/deck_radar_s_broken_polygons.inc"
};

static Sint32 deck_radar_s_broken_vertices[207] = {
#include "shared/deck_radar_s/deck_radar_s_broken_vertices.inc"
};

static NJS_CNK_MODEL deck_radar_s_broken = {
#include "shared/deck_radar_s/deck_radar_s_broken.inc"
};

static NJS_CNK_OBJECT deck_radar_s_broken_piece_object = {
#include "shared/deck_radar_s/deck_radar_s_broken_piece_object.inc"
};

static NJS_TEXNAME radar_post_textures[4] = {
#include "shared/deck_radar_s/radar_post_textures.inc"
};

static NJS_TEXLIST radar_post_texlist = {
#include "shared/deck_radar_s/radar_post_texlist.inc"
};

static Sint16 radar_post_polygons[297] = {
#include "shared/deck_radar_s/radar_post_polygons.inc"
};

static Sint32 radar_post_vertices[199] = {
#include "shared/deck_radar_s/radar_post_vertices.inc"
};

static NJS_CNK_MODEL radar_post = {
#include "shared/deck_radar_s/radar_post.inc"
};

static NJS_CNK_OBJECT radar_post_piece_object = {
#include "shared/deck_radar_s/radar_post_piece_object.inc"
};

static NJS_TEXNAME radar_dish_textures[3] = {
#include "shared/deck_radar_s/radar_dish_textures.inc"
};

static NJS_TEXLIST radar_dish_texlist = {
#include "shared/deck_radar_s/radar_dish_texlist.inc"
};

static Sint16 radar_dish_polygons[191] = {
#include "shared/deck_radar_s/radar_dish_polygons.inc"
};

static Sint32 radar_dish_vertices[179] = {
#include "shared/deck_radar_s/radar_dish_vertices.inc"
};

static NJS_CNK_MODEL radar_dish = {
#include "shared/deck_radar_s/radar_dish.inc"
};

static NJS_CNK_OBJECT radar_dish_piece_object = {
#include "shared/deck_radar_s/radar_dish_piece_object.inc"
};

static NJS_TEXNAME radar_arm_textures[4] = {
#include "shared/deck_radar_s/radar_arm_textures.inc"
};

static NJS_TEXLIST radar_arm_texlist = {
#include "shared/deck_radar_s/radar_arm_texlist.inc"
};

static Sint16 radar_arm_polygons[149] = {
#include "shared/deck_radar_s/radar_arm_polygons.inc"
};

static Sint32 radar_arm_vertices[115] = {
#include "shared/deck_radar_s/radar_arm_vertices.inc"
};

static NJS_CNK_MODEL radar_arm = {
#include "shared/deck_radar_s/radar_arm.inc"
};

static NJS_CNK_OBJECT radar_arm_piece_object = {
#include "shared/deck_radar_s/radar_arm_piece_object.inc"
};

NJS_TEXLIST *deck_radar_s_texlists[6] = {
#include "shared/deck_radar_s/deck_radar_s_texlists.inc"
};
static struct collision_shape shapes[RADAR_SHAPES] = {
#include "shared/deck_radar_s/shapes.inc"
};
static struct debris_part parts[PIECES] = {
#include "shared/deck_radar_s/parts.inc"
};
static Angle dish_turn = 256;

static void deck_radar_s_destroy(struct task *task);
static void deck_radar_s_step(struct task *task);
static void deck_radar_s_draw(struct task *task);

void deck_radar_s_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 target;

    if (!placement_try_leave(task)) {
        task->draw = deck_radar_s_draw;
        task->step = deck_radar_s_step;
        task->destructor = deck_radar_s_destroy;
        target = work->pos;
        target.y += TARGET_HEIGHT;
        collision_init(task, shapes, RADAR_SHAPES, COLLISION_OBJECT);
        if (task->placement && placement_get_state(task)) {
            work->collision->shapes[INTACT_SHAPE].flags |= SHAPE_OFF;
            work->tick = RELOAD_TICK;
            work->submode |= BROKEN;
        } else {
            weak_point_create(task, 0, &target);
        }
    }
}

static void deck_radar_s_destroy(struct task *task)
{
    struct task_work *work = task->work;
    if (task->placement && work->tick) {
        placement_set_state(task, BROKEN);
    }
}

static void deck_radar_s_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 origin;
    NJS_VECTOR velocity;

    if (!placement_try_leave(task)) {
        /* @hack redundant: a loop no path enters, which would set the tick */
        while (0) {
            work->tick = 1;
        }
        work->timer += dish_turn;
        if (work->tick) {
            if (work->tick == 1) {
                origin = work->pos;
                velocity.x = 0.0F;
                velocity.y = PIECE_RISE;
                velocity.z = 0.0F;
                debris_break_flags(
                    &origin, work->ang.y, parts, PIECES, PIECE_SPEED,
                    PIECE_SPREAD,
                    (work->ang.x & CRASH_THROUGH) ? THROUGH_FLOOR : work->pos.y,
                    &velocity, DEBRIS_FADE_COLORS | DEBRIS_LANDING_BURST);
                score_add(BREAK_POINTS);
            }
            if (work->tick < TICK_LIMIT) {
                ++work->tick;
            }
        } else {
            /* @hack redundant: the test of the tick, which is 0 in this
             * arm */
            if (!work->tick && work->submode) {
                work->collision->shapes[INTACT_SHAPE].flags |= SHAPE_OFF;
                work->tick = 1;
            }
            sound_play_at_for(RADAR_SOUND, work, 1, 0, RADAR_SOUND_STEPS,
                              &work->pos);
        }
        collision_register(task);
    }
}

static void deck_radar_s_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object = &deck_radar_s_object;

    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    if (work->submode) {
        njSetTexture(&deck_radar_s_broken_texlist);
        njCnkEasyDrawModel(&deck_radar_s_broken);
        if (work->tick < DISH_DRAW_STEPS) {
            njSetTexture(&deck_radar_s_texlist);
            object = object->child;
            njTranslateV(NULL, (NJS_VECTOR *)object->pos);
            njRotateY(NULL, work->timer);
            njCnkEasyDrawModel(object->model);
        }
    } else {
        njSetTexture(&deck_radar_s_texlist);
        njCnkEasyDrawModel(object->model);
        object = object->child;
        njTranslateV(NULL, (NJS_VECTOR *)object->pos);
        njRotateY(NULL, work->timer);
        njCnkEasyDrawModel(object->model);
    }
    njPopMatrix(1);
}
