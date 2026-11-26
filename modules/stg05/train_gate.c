/* the train's gate, GSGATE in Pumpkin Hill's object list. it puffs smoke from
 * its face while a ghost train passes. this source also holds the
 * stage's list of sounds */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "effect/particle.h"
#include "object/set.h"
#include "shared/white_breath.h"
#include "sound/lists.h"
#include "stg05/ghost_train.h"
#include "stg05/train_gate.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the gate's face lies across its local x and y. it is scl.x + FACE_MARGIN
 * wide and scl.y + FACE_MARGIN high. the gate puffs while a train's engine
 * lies within scl.z + FACE_MARGIN of its pos */
#define FACE_MARGIN 30.0F
/* a puff takes color_1 with a chance of FIRST_COLOR_CHANCE. otherwise it takes
 * color_2 with a chance of SECOND_COLOR_CHANCE, and color_3 if not */
#define FIRST_COLOR_CHANCE 0.4F
#define SECOND_COLOR_CHANCE 0.5F

/* the stage's sounds, as sound_load_stage takes them. they open this source's
 * constants, ahead of its strings */
static const struct sound pumpkin_hill_sound_table[6] = {
#include "stg05/train_gate/pumpkin_hill_sound_table.inc"
};
const struct sound_list pumpkin_hill_sounds = {
#include "stg05/train_gate/pumpkin_hill_sounds.inc"
};

NJS_TEXLIST *train_gate_texlists[] = {NULL};
/* a color that no code reads */
static Uint32 dat_8c57de40 = 0xffc090ff;
/* a puff's speed, in the gate's axes. along x and y, it is up to half the
 * spread either way. along z, it is from 0 up to the spread */
static Float spread_x = 0.3F;
static Float spread_y = 1.0F;
static Float spread_z = 1.0F;
/* a puff's size: from size_least up to size_range more */
static Float size_least = 6.0F;
static Float size_range = 3.0F;
/* the puffs' colors: a purple, a pink and a white */
static Uint32 color_1 = 0x908050ff;
static Uint32 color_2 = 0x90ff5080;
static Uint32 color_3 = 0x90ffffff;

static void train_gate_step(struct task *task);
static void train_gate_draw(struct task *task);

static void train_gate_destroy(struct task *task)
{
}

static void train_gate_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *train;
    struct particle *puff;
    NJS_POINT3 engine;
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((train = ghost_train_nearest(&work->pos, &engine)) != NULL &&
        njDistanceP2P(&engine, &work->pos) < work->scl.z + FACE_MARGIN) {
        /* @hack redundant: a block no path reaches, whose call keeps train in
         * the step's frame */
        if (0) {
            task_kill(train);
        }
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        pos.x = ((rand() * RAND_SCALE) - 0.5F) * (work->scl.x + FACE_MARGIN);
        pos.y = ((rand() * RAND_SCALE) - 0.5F) * (work->scl.y + FACE_MARGIN);
        pos.z = 0.0F;
        spd.x = ((rand() * RAND_SCALE) - 0.5F) * spread_x;
        spd.y = ((rand() * RAND_SCALE) - 0.5F) * spread_y;
        spd.z = rand() * spread_z * RAND_SCALE;
        njCalcPoint(NULL, &pos, &pos);
        njCalcVector(NULL, &spd, &spd);
        njPopMatrixEx();
        puff = white_breath_short_create(
            &pos, &spd, (rand() * size_range * RAND_SCALE) + size_least);
        if (puff != NULL) {
            if (rand() * RAND_SCALE < FIRST_COLOR_CHANCE) {
                puff->color = color_1;
            } else if (rand() * RAND_SCALE < SECOND_COLOR_CHANCE) {
                puff->color = color_2;
            } else {
                puff->color = color_3;
            }
        }
    }
}

static void train_gate_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: pos, a local that nothing reads */
    NJS_POINT3 pos;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- SCL X";
        (void)"<- SCL Y";
        (void)"<- COLSCL";
    }
    /* @hack redundant: a block no path reaches, whose calls keep work in the
     * draw */
    if (0) {
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
    }
}

void train_gate_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = train_gate_draw;
        task->step = train_gate_step;
        task->destructor = train_gate_destroy;
    }
}
