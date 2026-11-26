/* the lamp, MILUMP and milump in Aquatic Mine's object list. it glows, and
 * lights each player near it */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/object_light.h"
#include "stg07/lamp.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

#include "stg07/lamp/text.inc"

#include "stg07/scenery/lamp.h"

#define LAMP_SHAPES 1
/* the lamp lights a player within LIGHT_RANGE of its pos */
#define LIGHT_RANGE 30.0F
/* the glow's quad, 2 * GLOW_HALF_SIZE on a side. its texture cycles through
 * GLOW_FRAMES frames, one for each count of game_state.steps */
#define GLOW_FRAMES 16
#define GLOW_HALF_SIZE 6.5F
#define GLOW_COLOR 0xffffffff

/* the lamp's light, blue. object_light_add_near gives each copy a dir of its
 * own, toward the player it lights */
/* @hack type: the light declared as an array of one, where the light alone
 * would do */
static struct light lamp_light[1] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.0F, 0.1F, 0.0F, 1.0F},
};
NJS_TEXLIST *lamp_texlists[3] = {
#include "stg07/lamp/lamp_texlists.inc"
};
/* clang-format off */
static struct collision_shape lamp_shapes[LAMP_SHAPES] = {
    {0, SHAPE_SPHERE,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 0.0F, 0.0F}, 9.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void lamp_destroy(struct task *task);
static void lamp_step(struct task *task);
static void lamp_draw(struct task *task);

void lamp_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        /* @hack spelling: the draw and the destructor set before the step,
         * where struct task's order would set the step first */
        task->draw = lamp_draw;
        task->destructor = lamp_destroy;
        task->step = lamp_step;
        collision_init(task, lamp_shapes, LAMP_SHAPES, COLLISION_OBJECT);
    }
}

static void lamp_destroy(struct task *task)
{
}

static void lamp_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct light *light;
    int i;

    if (!placement_try_leave(task)) {
        /* @hack redundant: a store of GAME_PLAYERS to the work's mode,
         * read again through task, which no path reaches */
        if (0) {
            task->work->mode = GAME_PLAYERS;
        }
        pos = work->pos;
        for (i = 0; i < GAME_PLAYERS; i++) {
            light = object_light_add_near(lamp_light, i, &pos, LIGHT_RANGE);
            if (light != NULL) {
                object_light_jitter(light);
            }
        }
        collision_register(task);
        work->timer++;
    }
}

static void lamp_draw(struct task *task)
{
    struct task_work *work = task->work;
    Sint32 frame;
    Angle edge;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
    }
    njSetTexture(&mine_lamp_texlist);
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(mine_lamp_object->model);
    /* @hack spelling: frame and edge, locals for the glow's texture and its
     * edge's angle, where the values in the call would do */
    frame = game_state.steps & (GLOW_FRAMES - 1);
    edge = QUARTER_TURN;
    njSetTexture(&mine_lamp_glow_texlist);
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    particle_draw_quad((NJS_POINT3 *)mine_lamp_object->child->pos, frame, edge,
                       0, GLOW_HALF_SIZE, GLOW_HALF_SIZE, GLOW_COLOR,
                       PARTICLE_BLEND_ADD, 0.0F, 0.0F);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
    njPopMatrix(1);
}
