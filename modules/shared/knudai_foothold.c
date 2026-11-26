/* the foothold a flute stand calls up, KDASIBA in a stage's object list */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/world.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/knudai.h"
#include "shared/knudai_foothold.h"
#include "shared/ring_sparkle.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the foothold's modes, in its work's submode */
enum knudai_foothold_mode {
    MODE_WAIT, /* until its stand is played */
    MODE_DELAY, /* until its work's timer runs out */
    MODE_APPEAR,
    MODE_SHOWN
};

#define KNUDAI_ID(work, mask) (((work)->ang.x & (mask)) % KNUDAI_IDS)
/* the delay takes DELAY_PER_DISTANCE steps for each unit of distance from the
 * foothold's pos to the origin. knudai_get_pos gives the origin for a played
 * stand. the delay lasts at most DELAY_MAX steps */
#define DELAY_PER_DISTANCE 0.8F
#define DELAY_MAX 160.0F
/* over its last SPARKLE_STEPS steps, the delay gives off a sparkle a step with
 * a chance of SPARKLE_CHANCE */
#define SPARKLE_STEPS 30
#define SPARKLE_CHANCE 0.9F
/* a sparkle starts up to SPARKLE_HEIGHT above pos. it starts up to
 * SPARKLE_REACH from pos, at a random turn about y */
#define SPARKLE_REACH 35.0F
#define SPARKLE_HEIGHT 5.0F
/* a sparkle's speed along x and z is random, across a range SPARKLE_DRIFT
 * wide about 0 */
#define SPARKLE_DRIFT 0.2F
#define SPARKLE_SCALE 1.5F
/* rand() * RAND_TURN is a random Angle less than a full turn */
#define RAND_TURN 2.0F
#define APPEAR_SOUND 0x100b
#define SOUND_PRIORITY 0
#define SOUND_VOLUME 0
#define FADE_STEP 0.0125F /* the alpha each step of the fade adds, up to 1 */
/* each foothold's bob starts a random count of steps ahead, below
 * BOB_PHASES. its work's tick keeps the count */
#define BOB_PHASES 30.0F
/* the foothold's size class in the collision world is measured by its
 * collision model's radius plus WORLD_MARGIN */
#define WORLD_MARGIN 5.0F
/* the world collects the foothold's copy only while a player lies near:
 * within the copy's radius plus REACH_MARGIN */
#define REACH_MARGIN 5.0F
#define LAND_DRAW_BITS (NJD_CONTROL_3D_TRANS_MODIFIER | NJD_CONTROL_3D_SHADOW)

struct knudai_foothold {
    /* the copy of knudai_foothold_collision the foothold puts in the world */
    NJS_OBJECT object;
    Float alpha;
    Float bob; /* how far the bob lifts the foothold above its pos */
};

/* The hidden hunting-stage platform and the surface it adds to the collision
 * world. */
#include "draw/chunk.h"

#include "shared/knudai_foothold/text.inc"

static NJS_TEXNAME knudai_foothold_textures[3] = {
#include "shared/knudai_foothold/knudai_foothold_textures.inc"
};

static NJS_TEXLIST knudai_foothold_texlist = {
#include "shared/knudai_foothold/knudai_foothold_texlist.inc"
};

static Sint16 knudai_foothold_polygons[153] = {
#include "shared/knudai_foothold/knudai_foothold_polygons.inc"
};

static Sint32 knudai_foothold_vertices[99] = {
#include "shared/knudai_foothold/knudai_foothold_vertices.inc"
};

static NJS_CNK_MODEL knudai_foothold_model = {
#include "shared/knudai_foothold/knudai_foothold_model.inc"
};

/* The original member keeps this root alongside its drawable model. */
static NJS_CNK_OBJECT knudai_foothold_object = {
#include "shared/knudai_foothold/knudai_foothold_object.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL foothold_floor_materials[1] = {
#include "shared/knudai_foothold/foothold_floor_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 foothold_floor_indices_0[49] = {
#include "shared/knudai_foothold/foothold_floor_indices_0.inc"
};

static NJS_MESHSET foothold_floor_meshsets[1] = {
#include "shared/knudai_foothold/foothold_floor_meshsets.inc"
};

static NJS_POINT3 foothold_floor_points[16] = {
#include "shared/knudai_foothold/foothold_floor_points.inc"
};

static NJS_VECTOR foothold_floor_normals[16] = {
#include "shared/knudai_foothold/foothold_floor_normals.inc"
};

static NJS_MODEL foothold_floor_model = {
#include "shared/knudai_foothold/foothold_floor_model.inc"
};

static NJS_OBJECT knudai_foothold_collision = {
#include "shared/knudai_foothold/knudai_foothold_collision.inc"
};

static void knudai_foothold_destroy(struct task *task);
static void knudai_foothold_step(struct task *task);
static void knudai_foothold_draw(struct task *task);
static void knudai_foothold_draw_fading(struct task *task);

#define FOOTHOLD(task) ((struct knudai_foothold *)(task)->motion.p)

NJS_TEXLIST *knudai_foothold_texlists[3] = {
#include "shared/knudai_foothold/knudai_foothold_texlists.inc"
};
/* how far the bob reaches above and below pos, and the Angle the bob's
 * phase turns a step */
static Float knudai_foothold_bob_height = 2.0F;
static Angle knudai_foothold_bob_turn = 0x300;

void knudai_foothold_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float radius;
    NJS_POINT3 pos;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct knudai_foothold));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack redundant: the phase masked to a byte, which its store in the
     * byte tick does anyway */
    work->tick = (int)((Float)rand() * (BOB_PHASES * RAND_SCALE)) & 0xff;
    task->draw = NULL;
    task->step = knudai_foothold_step;
    task->destructor = knudai_foothold_destroy;
    work->submode = MODE_WAIT;
    object = &FOOTHOLD(task)->object;
    *object = knudai_foothold_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_MOVING | WORLD_SOLID;
    radius = object->model->r + WORLD_MARGIN;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    /* @hack redundant: a test of knudai_get_pos, which returns 1 for every id
     * knudai_played returns 1 for. nothing reads the pos it writes */
    if (knudai_played(KNUDAI_ID(work, KNUDAI_ID_MASK)) == 1 &&
        knudai_get_pos(KNUDAI_ID(work, KNUDAI_ID_MASK), &pos)) {
        work->timer = 0;
        task->draw = knudai_foothold_draw_fading;
        work->submode = MODE_APPEAR;
        FOOTHOLD(task)->alpha = 1.0F;
    }
}

static void knudai_foothold_destroy(struct task *task)
{
    if (FOOTHOLD(task)->object.model != NULL) {
        world_remove(task, &FOOTHOLD(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void knudai_foothold_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    Float delay;
    NJS_OBJECT *object;
    struct task_force *force;
    Float height;
    Float rise;
    int i;
    /* @hack spelling: mask, a local for KNUDAI_ID_MASK, where the constant in
     * place would do */
    int mask;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for its two strings,
     * which SHC still places in this source's constants */
    if (0) {
        (void)KNUDAI_FOOTHOLD_DEBUG_ID;
        (void)KNUDAI_FOOTHOLD_DEBUG_YAW;
    }
    mask = KNUDAI_ID_MASK;
    switch (work->submode) {
    case MODE_WAIT:
        if (knudai_played(KNUDAI_ID(work, mask)) != 1) {
            break;
        }
        if (knudai_get_pos(KNUDAI_ID(work, mask), &pos)) {
            delay = njDistanceP2P(&work->pos, &pos) * DELAY_PER_DISTANCE;
            if (delay > DELAY_MAX) {
                delay = DELAY_MAX;
            }
            work->timer = delay;
            work->submode = MODE_DELAY;
        }
        break;
    case MODE_DELAY:
        if (work->timer < SPARKLE_STEPS) {
            if ((Float)rand() * RAND_SCALE < SPARKLE_CHANCE) {
                pos.x = (Float)rand() * (SPARKLE_REACH * RAND_SCALE);
                pos.y = (Float)rand() * (SPARKLE_HEIGHT * RAND_SCALE);
                pos.z = 0.0F;
                njPushMatrix(NULL);
                njUnitMatrix(NULL);
                njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
                njRotateY(NULL, (Angle)((Float)rand() * RAND_TURN));
                njCalcPoint(NULL, &pos, &pos);
                njPopMatrix(1);
                spd.x = (((Float)rand() * RAND_SCALE) - 0.5F) * SPARKLE_DRIFT;
                spd.y = ((Float)rand() * RAND_SCALE) - 0.5F;
                spd.z = (((Float)rand() * RAND_SCALE) - 0.5F) * SPARKLE_DRIFT;
                ring_sparkle_create_long(&pos, &spd, SPARKLE_SCALE);
            }
        }
        if (work->timer-- == 0) {
            task->draw = knudai_foothold_draw_fading;
            work->submode = MODE_APPEAR;
            FOOTHOLD(task)->alpha = 0.0F;
            sound_play(APPEAR_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
        }
        break;
    case MODE_APPEAR:
        FOOTHOLD(task)->alpha += FADE_STEP;
        if (FOOTHOLD(task)->alpha > 1.0F) {
            FOOTHOLD(task)->alpha = 1.0F;
            work->submode = MODE_SHOWN;
            task->draw = knudai_foothold_draw;
        }
        break;
    case MODE_SHOWN:
        break;
    }
    if (work->submode == MODE_APPEAR || work->submode == MODE_SHOWN) {
        FOOTHOLD(task)->bob =
            njSin((game_state.steps + work->tick) * knudai_foothold_bob_turn) *
            knudai_foothold_bob_height;
    }
    if (FOOTHOLD(task)->object.model != NULL) {
        height = work->pos.y + FOOTHOLD(task)->bob;
        object = &FOOTHOLD(task)->object;
        rise = height - object->pos[1];
        for (i = 0, force = task->force.p; i < TASK_FORCES; i++, force++) {
            force->speed.y = rise;
        }
        object->pos[1] = height;
        if (work->submode != MODE_WAIT &&
            player_near((NJS_POINT3 *)object->pos,
                        object->model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void knudai_foothold_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&knudai_foothold_texlist);
    draw_control_set(LAND_DRAW_BITS);
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y + FOOTHOLD(task)->bob,
                work->pos.z);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(&knudai_foothold_model);
    njPopMatrix(1);
    draw_control_clear(LAND_DRAW_BITS);
}

static void knudai_foothold_draw_fading(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(LAND_DRAW_BITS);
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT);
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA | NJD_FST_IS);
    constant_material_set(FOOTHOLD(task)->alpha, 1.0F, 1.0F, 1.0F);
    njSetTexture(&knudai_foothold_texlist);
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y + FOOTHOLD(task)->bob,
                work->pos.z);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(&knudai_foothold_model);
    njPopMatrix(1);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    draw_control_clear(LAND_DRAW_BITS);
}
