/* the iron balls, IRONBALL2 in a stage's object list. a bar turns about its
 * hub, with an iron ball at each end. the balls roll as the bar turns */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/iron_ball.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "shared/iron_ball/text.inc"

/* the hub stands HUB_HEIGHT above pos. the bar's model is one arm, BAR_LENGTH
 * along x from the hub. the draw scales it by BAR_SCALE(work) in x for one arm,
 * and by -BAR_SCALE(work) for the other. a ball lies at each arm's end */
#define BAR_LENGTH 20.0F
#define BAR_SCALE(work) ((work)->scl.x + 1.0F)
#define HUB_HEIGHT 10.0F
#define DEGREES_PER_STEP(work) ((work)->scl.y + 1.0F)
/* a ball's distance along its circle about the hub in a step. it is the
 * radius, BAR_SCALE(work) * BAR_LENGTH, times the step's turn in radians,
 * with pi as ROUGH_PI */
#define ROUGH_PI 3.14F
#define HALF_TURN_DEGREES 180.0F
#define BALL_SPEED(work)                                                       \
    (BAR_SCALE(work) * DEGREES_PER_STEP(work) *                                \
     (BAR_LENGTH * ROUGH_PI / HALF_TURN_DEGREES))
/* the angle the balls turn about the bar in a step. it is the bar's turn
 * times the radius over ROLL_RADIUS. each ball thus rolls as a ball of radius
 * ROLL_RADIUS would */
#define ROLL_RADIUS 11.0F
#define BALL_ROLL(work)                                                        \
    (BAR_SCALE(work) * DEGREES_PER_STEP(work) *                                \
     (ANGLE_PER_DEGREE * (BAR_LENGTH / ROLL_RADIUS)))
/* a floating bar moves FLOAT_STEP a step, between FLOAT_RANGE(work) below
 * start_y and FLOAT_RANGE(work) above it */
#define FLOAT_STEP 0.1F
#define FLOAT_RANGE(work) ((work)->scl.z)

/* work->timer counts down to the bar's next look for a floor, which it takes
 * at FLOOR_LOOK. a look that finds none sets the timer to FLOOR_RETRY. one
 * that finds a floor sets it to 0, and the bar looks no more */
#define FLOOR_LOOK 1
#define FLOOR_RETRY 40
/* work->tick counts the steps since the balls' last dust. the balls raise
 * dust on the step that finds the tick past DUST_TICK_LIMIT, so once in 8
 * steps */
#define DUST_TICK_LIMIT 6
/* a shadow lies SHADOW_LIFT above the point it is drawn at */
#define SHADOW_LIFT 0.1F
/* the bar plays ROLL_SOUND each step it turns, for ROLL_SOUND_DURATION + 1
 * updates. each play renews that time, so the sound runs while the bar turns */
#define ROLL_SOUND 0x1019
#define ROLL_PRIORITY 1
#define ROLL_VOLUME 0
#define ROLL_SOUND_DURATION 30

/* a floating bar's modes, in its work's mode. it rises in MODE_NEW and
 * MODE_RISE, and falls in MODE_FALL. a bar that doesn't float keeps
 * MODE_NEW */
enum iron_ball_mode { MODE_NEW, MODE_RISE, MODE_FALL };

struct iron_ball {
    Angle roll; /* how far the balls have turned about the bar */
    Float start_y; /* pos.y as iron_ball_init found it */
};

#define IRON_BALL(task) ((struct iron_ball *)(task)->motion.p)

enum iron_ball_model { BALL_BUILD, HUB_BUILD, BAR_BUILD, IRON_BALL_BUILDS };
enum iron_ball_shape {
    HUB_SHAPE,
    BALL_SHAPE_POS_X,
    BALL_SHAPE_NEG_X,
    BAR_SHAPE,
    IRON_BALL_SHAPES
};

static void iron_ball_destroy(struct task *task);
static void iron_ball_step(struct task *task);
static void iron_ball_draw(struct task *task);
static void iron_ball_draw_built(struct task *task);

#define IRON_BALL_TEXTURES 2
static NJS_TEXNAME iron_ball_texnames[IRON_BALL_TEXTURES] = {
#include "shared/iron_ball/iron_ball_texnames.inc"
};
NJS_TEXLIST iron_ball_texlist = {
#include "shared/iron_ball/iron_ball_texlist.inc"
};
static Sint16 iron_ball_polygons[897] = {
#include "shared/iron_ball/iron_ball_polygons.inc"
};
static Sint32 iron_ball_vertices[717] = {
#include "shared/iron_ball/iron_ball_vertices.inc"
};
static NJS_CNK_MODEL iron_ball_model = {
#include "shared/iron_ball/iron_ball_model.inc"
};
static NJS_CNK_OBJECT iron_ball_object = {
#include "shared/iron_ball/iron_ball_object.inc"
};
/* the hub and the bar draw with these */
#define BAR_TEXTURES 2
static NJS_TEXNAME iron_ball_bar_texnames[BAR_TEXTURES] = {
#include "shared/iron_ball/iron_ball_bar_texnames.inc"
};
static NJS_TEXLIST iron_ball_bar_texlist = {
#include "shared/iron_ball/iron_ball_bar_texlist.inc"
};
static Sint16 iron_ball_bar_polygons[27] = {
#include "shared/iron_ball/iron_ball_bar_polygons.inc"
};
static Sint32 iron_ball_bar_vertices[39] = {
#include "shared/iron_ball/iron_ball_bar_vertices.inc"
};
static NJS_CNK_MODEL iron_ball_bar_model = {
#include "shared/iron_ball/iron_ball_bar_model.inc"
};
static NJS_CNK_OBJECT iron_ball_bar_object = {
#include "shared/iron_ball/iron_ball_bar_object.inc"
};
static Sint16 iron_ball_hub_polygons[351] = {
#include "shared/iron_ball/iron_ball_hub_polygons.inc"
};
static Sint32 iron_ball_hub_vertices[165] = {
#include "shared/iron_ball/iron_ball_hub_vertices.inc"
};
static NJS_CNK_MODEL iron_ball_hub_model = {
#include "shared/iron_ball/iron_ball_hub_model.inc"
};
/* the hub's node, whose child is the bar's. the hub's build and draw take its
 * model alone, so nothing here reaches the bar through it */
static NJS_CNK_OBJECT iron_ball_hub_object = {
#include "shared/iron_ball/iron_ball_hub_object.inc"
};

NJS_TEXLIST *iron_ball_texlists[4] = {
#include "shared/iron_ball/iron_ball_texlists.inc"
};
/* TRUE once a single call of iron_ball_build has built all three models */
static Bool iron_ball_built = FALSE;
static struct model_build iron_ball_builds[IRON_BALL_BUILDS] = {
#include "shared/iron_ball/iron_ball_builds.inc"
};
/* the hub's shape, then the ball's at +x, the ball's at -x and the bar's.
 * collision_init gives each record its own copy. iron_ball_init moves the
 * copy's balls to the bar's ends, and stretches its bar between them */
/* clang-format off */
static struct collision_shape iron_ball_shapes[IRON_BALL_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_ALL, SHAPE_HURTABLE,
     {0.0F, HUB_HEIGHT, 0.0F}, 4.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_ALL, SHAPE_HURTABLE,
     {0.0F, HUB_HEIGHT, 0.0F}, 10.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_ALL, SHAPE_HURTABLE,
     {0.0F, HUB_HEIGHT, 0.0F}, 10.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER2, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, HUB_HEIGHT, 0.0F}, 1.0F, 5.0F, 0.0F, 0, {0, 0, QUARTER_TURN}},
};
/* clang-format on */
static Float iron_ball_hub_shadow_scale = 5.0F;
static Float iron_ball_shadow_scale = 10.0F;
static Float iron_ball_dust_scale = 3.0F;
/* a shadow's offset from the hub or the ball above it: HUB_HEIGHT down, at
 * pos's height. the built draw reads its own copy */
static NJS_POINT3 iron_ball_shadow_offset = {0.0F, -HUB_HEIGHT, 0.0F};
static NJS_POINT3 iron_ball_built_shadow_offset = {0.0F, -HUB_HEIGHT, 0.0F};

Uint8 *iron_ball_build(Uint8 *buf)
{
    if (model_builds_compile(&buf, iron_ball_builds, IRON_BALL_BUILDS) ==
        IRON_BALL_BUILDS) {
        iron_ball_built = TRUE;
    }
    return buf;
}

/* draw shadow_object at pos, in the current matrix's space, scaled by scale
 * in x and z */
static void iron_ball_draw_shadow(NJS_POINT3 *pos, Float scale)
{
    njPushMatrixEx();
    njTranslate(NULL, pos->x, pos->y + SHADOW_LIFT, pos->z);
    njScale(NULL, scale, 1.0F, scale);
    njCnkModDrawObject(&shadow_object);
    njPopMatrix(1);
}

void iron_ball_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct iron_ball));
    if (task->motion.p == NULL) {
        return;
    }
    if (iron_ball_built) {
        task->draw = iron_ball_draw_built;
    } else {
        task->draw = iron_ball_draw;
    }
    task->step = iron_ball_step;
    task->destructor = iron_ball_destroy;
    work->mode = MODE_NEW;
    work->ang.x = 0;
    work->ang.z &= IRON_BALL_NO_DUST | IRON_BALL_FLOAT;
    collision_init(task, iron_ball_shapes, IRON_BALL_SHAPES, COLLISION_OBJECT);
    work->collision->shapes[BALL_SHAPE_POS_X].center.x =
        BAR_SCALE(work) * BAR_LENGTH;
    work->collision->shapes[BALL_SHAPE_NEG_X].center.x =
        -(BAR_SCALE(work) * BAR_LENGTH);
    work->collision->shapes[BAR_SHAPE].size_y = BAR_SCALE(work) * BAR_LENGTH;
    collision_fit_record_radius(work->collision);
    IRON_BALL(task)->start_y = work->pos.y;
    work->timer = FLOOR_LOOK;
    work->ang.y = 0;
    IRON_BALL(task)->roll = 0;
    work->tick = 0;
}

static void iron_ball_destroy(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task read in place
     * would do */
    struct task *owner = task;

    syFree(owner->motion.p);
    owner->motion.p = NULL;
}

static void iron_ball_step(struct task *task)
{
    struct task_work *work = task->work;
    Float floor;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants. the image holds none of the
     * block's code. without its stores and calls, SHC writes the rest of the
     * step differently */
    if (0) {
        work->pos.x = 10.0F;
        work->mode = MODE_FALL;
        work->pos.y = 1.0F;
        (void)"<- EFF OFF";
        (void)"<- EFF ON";
        (void)"FLOAT ON";
        (void)"FLOAT OFF";
        (void)"<- BAR SCL";
        (void)"<- ROTSPD";
        (void)"<- U/D RANGE";
        matrix_load_parent();
        matrix_load_parent();
        matrix_load_parent();
    }
    collision_register(task);
    if (work->timer > 0) {
        if (work->timer == FLOOR_LOOK) {
            struct angles ang;

            floor = surface_probe_floor(work->pos.x, work->pos.y + HUB_HEIGHT,
                                        work->pos.z, &ang);
            if (floor != NO_FLOOR) {
                if (!(work->ang.z & IRON_BALL_FLOAT)) {
                    work->pos.y = floor;
                }
                work->timer = 0;
            } else {
                work->timer = FLOOR_RETRY;
            }
        } else {
            work->timer--;
        }
    } else if (!game_state.time_stopped && !(work->ang.z & IRON_BALL_NO_DUST) &&
               work->tick++ > DUST_TICK_LIMIT) {
        NJS_VECTOR dir;
        NJS_VECTOR spd;
        NJS_POINT3 offset;
        NJS_POINT3 pos;

        work->tick = 0;
        dir.x = 0.0F;
        dir.y = 0.0F;
        /* each puff moves with its ball. dir.z is negative, since the ball at
         * +x moves toward -z as ang.y grows */
        dir.z = -BALL_SPEED(work);
        offset.x = BAR_SCALE(work) * BAR_LENGTH;
        offset.y = 0.0F;
        offset.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &offset, &pos);
        njCalcVector(NULL, &dir, &spd);
        dust_short_create(&pos, &spd, iron_ball_dust_scale);
        njRotateY(NULL, HALF_TURN);
        njCalcPoint(NULL, &offset, &pos);
        njCalcVector(NULL, &dir, &spd);
        dust_create(&pos, &spd, iron_ball_dust_scale);
        njPopMatrix(1);
    }
    if (work->ang.z & IRON_BALL_FLOAT) {
        if (work->mode != MODE_FALL) {
            work->pos.y += FLOAT_STEP;
            if (work->pos.y > FLOAT_RANGE(work) + IRON_BALL(task)->start_y) {
                work->pos.y = FLOAT_RANGE(work) + IRON_BALL(task)->start_y;
                work->mode = MODE_FALL;
            }
        } else {
            work->pos.y -= FLOAT_STEP;
            /* @hack spelling: -FLOAT_RANGE(work) + start_y, where start_y -
             * FLOAT_RANGE(work) would do */
            if (work->pos.y < -FLOAT_RANGE(work) + IRON_BALL(task)->start_y) {
                work->pos.y = -FLOAT_RANGE(work) + IRON_BALL(task)->start_y;
                work->mode = MODE_RISE;
                /* @hack redundant: an empty else */
            } else {
            }
        }
    }
    if (!game_state.time_stopped) {
        work->ang.y += (Angle)(DEGREES_PER_STEP(work) * ANGLE_PER_DEGREE);
        IRON_BALL(task)->roll -= (Angle)BALL_ROLL(work);
        sound_play_at_for(ROLL_SOUND, work, ROLL_PRIORITY, ROLL_VOLUME,
                          ROLL_SOUND_DURATION, &work->pos);
    }
}

/* draw from the models. an iron ball set up before iron_ball_build had built
 * all three draws this way */
static void iron_ball_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y + HUB_HEIGHT, work->pos.z);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&iron_ball_bar_texlist);
    njCnkSimpleDrawModel(&iron_ball_hub_model);
    iron_ball_draw_shadow(&iron_ball_shadow_offset, iron_ball_hub_shadow_scale);
    njPushMatrix(NULL);
    njScale(NULL, BAR_SCALE(work), 1.0F, 1.0F);
    njCnkSimpleDrawModel(&iron_ball_bar_model);
    njPopMatrix(1);
    njPushMatrix(NULL);
    njTranslate(NULL, BAR_SCALE(work) * BAR_LENGTH, 0.0F, 0.0F);
    iron_ball_draw_shadow(&iron_ball_shadow_offset, iron_ball_shadow_scale);
    njRotateX(NULL, IRON_BALL(task)->roll);
    njSetTexture(&iron_ball_texlist);
    njCnkSimpleDrawModel(&iron_ball_model);
    njPopMatrix(1);
    /* the arm at -x draws with iron_ball_texlist current. that list names the
     * same two textures as iron_ball_bar_texlist, so the arm looks the same */
    njPushMatrix(NULL);
    njScale(NULL, -BAR_SCALE(work), 1.0F, 1.0F);
    njCnkSimpleDrawModel(&iron_ball_bar_model);
    njPopMatrix(1);
    njTranslate(NULL, -(BAR_SCALE(work) * BAR_LENGTH), 0.0F, 0.0F);
    iron_ball_draw_shadow(&iron_ball_shadow_offset, iron_ball_shadow_scale);
    njRotateX(NULL, -IRON_BALL(task)->roll);
    njRotateY(NULL, HALF_TURN);
    njSetTexture(&iron_ball_texlist);
    njCnkSimpleDrawModel(&iron_ball_model);
    njPopMatrix(1);
}

/* draw as iron_ball_draw does, from the builds iron_ball_build made */
static void iron_ball_draw_built(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y + HUB_HEIGHT, work->pos.z);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njSetTexture(&iron_ball_bar_texlist);
    njDirectDrawObject(iron_ball_builds[HUB_BUILD].built);
    iron_ball_draw_shadow(&iron_ball_built_shadow_offset,
                          iron_ball_hub_shadow_scale);
    njScale(NULL, BAR_SCALE(work), 1.0F, 1.0F);
    njDirectDrawObject(iron_ball_builds[BAR_BUILD].built);
    matrix_load_parent();
    njTranslate(NULL, BAR_SCALE(work) * BAR_LENGTH, 0.0F, 0.0F);
    iron_ball_draw_shadow(&iron_ball_built_shadow_offset,
                          iron_ball_shadow_scale);
    njRotateX(NULL, IRON_BALL(task)->roll);
    njSetTexture(&iron_ball_texlist);
    njDirectDrawObject(iron_ball_builds[BALL_BUILD].built);
    matrix_load_parent();
    njScale(NULL, -BAR_SCALE(work), 1.0F, 1.0F);
    njDirectDrawObject(iron_ball_builds[BAR_BUILD].built);
    matrix_load_parent();
    njTranslate(NULL, -(BAR_SCALE(work) * BAR_LENGTH), 0.0F, 0.0F);
    iron_ball_draw_shadow(&iron_ball_built_shadow_offset,
                          iron_ball_shadow_scale);
    njRotateX(NULL, -IRON_BALL(task)->roll);
    njRotateY(NULL, HALF_TURN);
    njSetTexture(&iron_ball_texlist);
    njDirectDrawObject(iron_ball_builds[BALL_BUILD].built);
    njPopMatrix(2);
}
