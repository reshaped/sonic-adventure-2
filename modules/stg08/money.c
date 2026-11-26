/* MONEY in Security Hall's object list: a box that throws up banknotes. the
 * box lies about the placement's pos, turned by ang.y, and its half sizes
 * are scl's. each step throws a note from a random place in the box, with a
 * chance of throw_chance */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "object/set.h"
#include "shared/paper.h"
#include "sound/sound.h"
#include "stg08/fixture_art.h"
#include "stg08/paper.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/task.h"
#include "system/util.h"

/* the emitter's sound, an id in the stage's sound list. a play lasts
 * MONEY_SOUND_STEPS + 1 updates, as sound_play_for says */
#define MONEY_SOUND 6
#define MONEY_SOUND_PRIORITY 1
#define MONEY_SOUND_STEPS 10
/* a note's floor lies FLOOR_DROP below the place it starts */
#define FLOOR_DROP 10.0F

NJS_TEXLIST *security_money_textures[] = {
    &security_money_texlist,
    NULL,
};

/* a note spins about each axis at up to spin_range * ANGLE_PER_DEGREE / 2
 * either way. paper_spin counts that spin in degrees a step. a note rises at
 * rise_speed, and up to rise_spread faster */
static Float spin_range = 0.2F;
static Float rise_speed = 2.5F;
static Float rise_spread = 0.8F;
static NJS_DIRECT_HEAD *compiled = NULL;
static Float throw_chance = 0.3F;

static void money_destroy(struct task *task);
static void money_step(struct task *task);
static void money_draw(struct task *task);

/* object_build of shared/common_object.h, but with the note's texture list
 * current and fog on */
static NJS_DIRECT_HEAD *build_note(Uint8 **buffer, NJS_CNK_OBJECT *object)
{
    NJS_CNK_OBJECT node;
    NJS_DIRECT_HEAD *built;
    Uint8 *end;

    node = *object;
    node.evalflags = NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL |
                     NJD_EVAL_BREAK;
    node.child = NULL;
    node.sibling = NULL;
    built = NULL;
    njSetTexture(&security_money_texlist);
    end = *buffer + njCnkDirectObjectCompileSize(&node);
    njFogEnable();
    if (end < DEMO_AREA) {
        njCnkDirectObjectCompile(&node, *buffer);
        built = (NJS_DIRECT_HEAD *)*buffer;
        *buffer = end;
    }
    return built;
}

Uint8 *security_money_build(Uint8 *buffer)
{
    compiled = build_note(&buffer, security_money_object);
    return buffer;
}

void security_money_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = money_draw;
        task->step = money_step;
        task->destructor = money_destroy;
    }
}

static void money_destroy(struct task *task)
{
}

static void money_step(struct task *task)
{
    /* @hack redundant: work a register variable, where a plain one would do */
    register struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct paper_spin spin;
    NJS_VECTOR speed;
    Float spread;
    Float random_scale;
    Float angle_scale;

    if (!placement_try_leave(task)) {
        /* @hack spelling: 2.0F and RAND_SCALE held in locals, where the
         * constants would do */
        spread = 2.0F;
        random_scale = RAND_SCALE;
        if (rand() * random_scale < throw_chance) {
            pos.x = work->scl.x * ((rand() * random_scale) - 0.5F) * spread;
            pos.y = work->scl.y * ((rand() * random_scale) - 0.5F) * spread;
            pos.z = work->scl.z * ((rand() * random_scale) - 0.5F) * spread;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateV(NULL, &work->pos);
            njRotateY(NULL, work->ang.y);
            njCalcPoint(NULL, &pos, &pos);
            njPopMatrix(1);
            /* @hack spelling: ANGLE_PER_DEGREE held in a local, where the
             * constant would do */
            angle_scale = ANGLE_PER_DEGREE;
            spin.x =
                ((rand() * random_scale) - 0.5F) * spin_range * angle_scale;
            spin.y =
                ((rand() * random_scale) - 0.5F) * spin_range * angle_scale;
            spin.z =
                ((rand() * random_scale) - 0.5F) * spin_range * angle_scale;
            /* @hack redundant: the factor 1.0F on the speeds along x and z */
            speed.x = ((rand() * random_scale) - 0.5F) * 1.0F;
            speed.y = rise_speed + (rand() * rise_spread * random_scale);
            speed.z = ((rand() * random_scale) - 0.5F) * 1.0F;
            security_paper_launch(
                &pos, &work->ang, pos.y - FLOOR_DROP, &speed, &spin,
                security_money_object->model, compiled, &security_money_texlist,
                PAPER_THROUGH_FLOOR | PAPER_FADE_COLORS | PAPER_GUSTS);
        }
        sound_play_for(MONEY_SOUND, work, MONEY_SOUND_PRIORITY, 0,
                       MONEY_SOUND_STEPS);
    }
}

static void money_draw(struct task *task)
{
    /* @hack redundant: statements kept for their strings, the set editor's
     * labels for ang.y, scl.x and scl.z, which SHC places in this source's
     * constants */
    (void)"<- ANGLE Y";
    (void)"<- SCL X";
    (void)"<- SCL Z";
}
