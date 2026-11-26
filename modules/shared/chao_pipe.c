/* the Chao pipe, CHAOPIPE in a stage's object list. a near player can take
 * the pipe's action. the pipe then wobbles and lets out a small animal */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/animal.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/chao_pipe.h"
#include "shared/stage_texture_groups.h"
#include "sound/sound.h"
#include "system/task.h"

#define NEAR_RANGE 13.0F
#define MECH_NEAR_RANGE 25.0F

/* the work's timer counts down from WOBBLE_STEPS once the pipe starts to
 * wobble. the animal comes out on the step that finds the timer at
 * RELEASE_STEP */
#define WOBBLE_STEPS 300
#define RELEASE_STEP 270

/* the pipe keeps its squash and the squash's speed in its task's motion and
 * force, as Floats' bits. task_destroy frees a non-null motion or force, so
 * the destructor nulls both first */
/* @hack type: motion read as a Float through its address, where
 * (task)->motion.f would do */
#define SQUASH(task) (*(Float *)&(task)->motion)
/* @hack type: force read as a Float through its address, where (task)->force.f
 * would do */
#define SQUASH_SPEED(task) (*(Float *)&(task)->force)
/* the squash is the share of its width the pipe loses in x and z. the pipe
 * gains STRETCH of that share in y. the squash swings as a damped spring.
 * each step, its speed keeps DAMPING of itself and loses STIFFNESS of the
 * squash. a wobble starts it at a speed of WOBBLE_KICK */
#define WOBBLE_KICK 0.08F
#define DAMPING 0.95F
#define STIFFNESS 0.15F
#define STRETCH 0.9F

#define STATE_RELEASED 1
#define RELEASE_SOUND 0x100a

#define CHAO_PIPE_SHAPES 1

#include "shared/chao_pipe/text.inc"

static NJS_TEXNAME chao_pipe_textures[2] = {
#include "shared/chao_pipe/chao_pipe_textures.inc"
};

static NJS_TEXLIST chao_pipe_texlist = {
#include "shared/chao_pipe/chao_pipe_texlist.inc"
};

static Sint16 chao_pipe_polygons[553] = {
#include "shared/chao_pipe/chao_pipe_polygons.inc"
};

static Sint32 chao_pipe_vertices[387] = {
#include "shared/chao_pipe/chao_pipe_vertices.inc"
};

static NJS_CNK_MODEL chao_pipe_model = {
#include "shared/chao_pipe/chao_pipe_model.inc"
};

static NJS_CNK_OBJECT chao_pipe_object = {
#include "shared/chao_pipe/chao_pipe_object.inc"
};

static NJS_POINT3 chao_pipe_release_point = {
#include "shared/chao_pipe/chao_pipe_release_point.inc"
};

NJS_TEXLIST *chao_pipe_texlists[2] = {
#include "shared/chao_pipe/chao_pipe_texlists.inc"
};

static void chao_pipe_destroy(struct task *task);
static void chao_pipe_step(struct task *task);
static void chao_pipe_draw(struct task *task);

/* the pipe's shape, which its record copies */
static struct collision_shape chao_pipe_shapes[CHAO_PIPE_SHAPES] = {
#include "shared/chao_pipe/chao_pipe_shapes.inc"
};

void chao_pipe_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = chao_pipe_draw;
    task->step = chao_pipe_step;
    task->destructor = chao_pipe_destroy;
    work->submode = 0;
    collision_init(task, chao_pipe_shapes, CHAO_PIPE_SHAPES, COLLISION_OBJECT);
    SQUASH(task) = 0.0F;
    SQUASH_SPEED(task) = 0.0F;
}

static void chao_pipe_destroy(struct task *task)
{
    task->motion.p = NULL;
    task->force.p = NULL;
}

static void chao_pipe_step(struct task *task)
{
    struct task_work *work = task->work;
    int player;
    Float range;
    NJS_POINT3 release;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for its strings and its
     * locals. SHC still places its four strings in this source's constants,
     * where the image has them. the function's entry still saves registers
     * for its two locals. the image shows nothing else of the block, so its
     * statements are stand-ins */
    if (0) {
        struct player *record = player_records[0];
        struct task_work *record_work = player_work[0];

        placement_set_unk_20(task);
        record->action = record_work->mode;
        record->kind = record_work->submode;
        (void)CHAO_PIPE_EDIT_ANGLE_X;
        (void)CHAO_PIPE_EDIT_ANGLE_Y;
        (void)CHAO_PIPE_EDIT_ANGLE_Z;
        (void)CHAO_PIPE_EDIT_ANIMAL_LEVEL;
        /* @hack redundant: an empty else */
    } else {
    }
    if (task->placement != NULL && !placement_unk_20(task)) {
        if (player_character(0) == PLAYER_EGGMAN_MECH ||
            player_character(0) == PLAYER_TAILS_MECH) {
            range = MECH_NEAR_RANGE;
        } else {
            range = NEAR_RANGE;
        }
        player = player_near(&work->pos, range);
        if (work->timer == 0 && player != 0 &&
            player_work[player - 1]->mode == PLAYER_MODE_UNK_2A) {
            work->timer = WOBBLE_STEPS;
            SQUASH(task) = 0.0F;
            SQUASH_SPEED(task) = WOBBLE_KICK;
        } else if (work->timer != 0) {
            if (work->timer == RELEASE_STEP) {
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslateV(NULL, &work->pos);
                njRotateZ(NULL, work->ang.z);
                njRotateX(NULL, work->ang.x);
                njRotateY(NULL, work->ang.y);
                njCalcPoint(NULL, &chao_pipe_release_point, &release);
                njPopMatrix(1);
                animal_release(release.x, release.y, release.z,
                               (Uint32)work->scl.x % ANIMAL_RELEASE_KINDS);
                placement_set_state(task, STATE_RELEASED);
                sound_play(RELEASE_SOUND, NULL, 0, 0);
                placement_set_unk_20(task);
            }
            work->timer--;
        } else if (player != 0) {
            player_records[player - 1]->action = PLAYER_MODE_UNK_2A;
        }
    }
    collision_register(task);
    SQUASH(task) += SQUASH_SPEED(task);
    SQUASH_SPEED(task) =
        (SQUASH_SPEED(task) * DAMPING) - (SQUASH(task) * STIFFNESS);
}

static void chao_pipe_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&chao_pipe_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njScale(NULL, 1.0F - SQUASH(task), 1.0F + (SQUASH(task) * STRETCH),
            1.0F - SQUASH(task));
    njCnkEasyDrawModel(&chao_pipe_model);
    njPopMatrix(1);
}
