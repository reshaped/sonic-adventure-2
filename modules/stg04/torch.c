/* the torch, TAIMATU in White Jungle's object list: a post with a flame at its
 * top. both push players, and the flame hurts them. the flame lights each
 * player near it */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/object_light.h"
#include "sound/sound.h"
#include "stg04/torch.h"
#include "stg04/torch/text.inc"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* the flame's sound in the stage's sound list, and the priority it plays at */
#define FLAME_SOUND 4
#define FLAME_PRIORITY 1
/* the flame lights a player within LIGHT_RANGE of a point LIGHT_HEIGHT above
 * the torch's pos. it tries the first LIT_PLAYERS players */
#define LIGHT_HEIGHT 30.0F
#define LIGHT_RANGE 50.0F
#define LIT_PLAYERS 2
/* the flame's quad, 2 * FLAME_HALF_SIZE on a side, draws FLAME_HEIGHT above
 * the torch's pos. its texture cycles through FLAME_FRAMES frames, one for each
 * count of game_state.steps */
#define FLAME_HEIGHT 56.8F
#define FLAME_HALF_SIZE 14.0F
#define FLAME_FRAMES 16
#define FLAME_COLOR 0xffffffff
#define TORCH_SHAPES 2

/* The torch post. Its separate flame sprite sits above the post. */
static NJS_TEXNAME model_textures[1] = {
#include "stg04/torch/model_textures.inc"
};

static NJS_TEXLIST torch_texlist = {
#include "stg04/torch/torch_texlist.inc"
};

static Sint16 post_polygons[515] = {
#include "stg04/torch/post_polygons.inc"
};

static Sint32 post_vertices[319] = {
#include "stg04/torch/post_vertices.inc"
};

static NJS_CNK_MODEL post_mesh = {
#include "stg04/torch/post_mesh.inc"
};

static NJS_CNK_OBJECT torch_object = {
#include "stg04/torch/torch_object.inc"
};

static void torch_destroy(struct task *task);
static void torch_step(struct task *task);
static void torch_draw(struct task *task);

/* the flame's light, red. object_light_add_near gives each copy a dir of its
 * own, toward the player it lights */
/* @hack type: the light an array of one, where a single record would do */
static struct light torch_light[1] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.0F, 1.0F, 0.0F, 0.0F},
};
static NJS_TEXNAME flame_texnames[FLAME_FRAMES] = {
#include "stg04/torch/flame_texnames.inc"
};
static NJS_TEXLIST flame_texlist = {
#include "stg04/torch/flame_texlist.inc"
};
NJS_TEXLIST *torch_texlists[3] = {
#include "stg04/torch/torch_texlists.inc"
};
/* the post, a cylinder from the torch's pos up to 40 above it, and the
 * flame */
/* clang-format off */
static struct collision_shape torch_shapes[TORCH_SHAPES] = {
    {0, SHAPE_CYLINDER2,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 20.0F, 0.0F}, 4.0F, 20.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(2),
     SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS | SHAPE_IGNORE_PICKUPS |
         SHAPE_IGNORE_UNK_9,
     {0.0F, 50.0F, 0.0F}, 6.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

void torch_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = torch_draw;
        task->destructor = torch_destroy;
        task->step = torch_step;
        collision_init(task, torch_shapes, TORCH_SHAPES, COLLISION_OBJECT);
        sound_play_held_at(FLAME_SOUND, work, FLAME_PRIORITY, 0, &work->pos);
    }
}

static void torch_destroy(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: key, a copy of work, where work itself would do */
    void *key = work;

    sound_stop_key(key);
}

static void torch_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct light *light;
    int i;

    if (!placement_try_leave(task)) {
        pos = work->pos;
        pos.y += LIGHT_HEIGHT;
        for (i = 0; i < LIT_PLAYERS; i++) {
            light = object_light_add_near(torch_light, i, &pos, LIGHT_RANGE);
            if (light) {
                object_light_jitter(light);
            }
        }
        collision_register(task);
        work->timer++;
    }
}

static void torch_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    Sint32 frame;
    /* @hack spelling: edge_angle, a local for QUARTER_TURN set with frame,
     * where QUARTER_TURN passed in place would do */
    Angle edge_angle;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    njSetTexture(&torch_texlist);
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawObject(&torch_object);
    njPopMatrix(1);
    pos = work->pos;
    pos.y += FLAME_HEIGHT;
    frame = game_state.steps % FLAME_FRAMES;
    edge_angle = QUARTER_TURN;
    njSetTexture(&flame_texlist);
    njFogDisable();
    /* particle_draw_quad leaves the blend as it finds it. so these calls set
     * the flame's blend, and PARTICLE_BLEND_ADD changes nothing */
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    particle_draw_quad(&pos, frame, edge_angle, 0, FLAME_HALF_SIZE,
                       FLAME_HALF_SIZE, FLAME_COLOR, PARTICLE_BLEND_ADD, 0.0F,
                       0.0F);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}
