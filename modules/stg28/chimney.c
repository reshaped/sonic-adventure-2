/* the chimney, CHIMNEY in Pyramid Cave's object list. at random it bursts: a
 * blast flares above it while its model swells and settles */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "object/set.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stg28/cave_util.h"
#include "stg28/chimney.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#include "stg28/chimney/text.inc"
#include "stg28/models/chimney.h"

enum chimney_mode { MODE_IDLE, MODE_BURST };

/* task's unk_3c holds a burst's frame, from 0 to BURST_FRAMES - 1 */
#define BURST_FRAME(task) ((task)->unk_3c.l)
#define BURST_FRAMES 10
#define CHIMNEY_MODELS 6
/* the model the chimney draws between bursts */
#define IDLE_MODEL 0
/* the chance on each step without a burst that one starts */
#define BURST_CHANCE (1.0F / 300)

/* a burst's sounds, ids in pyramid_cave_sound_list, which play SOUND_HEIGHT
 * above the chimney's pos. a play of BURST_SOUND lasts
 * BURST_SOUND_STEPS + 1 updates, as sound_play_at_for says */
#define START_SOUND 12
#define START_SOUND_PRIORITY 0
#define BURST_SOUND 13
#define BURST_SOUND_PRIORITY 1
#define BURST_SOUND_STEPS 15
#define SOUND_HEIGHT 120.0F
/* the blast draws BLAST_HEIGHT above the chimney's pos, times the chimney's
 * scale, which cave_model_draw_scaled leaves on the matrix */
#define BLAST_HEIGHT 110.0F

#define SCALE(work) ((work)->scl.x + 1.0F)

#define CHIMNEY_SHAPES 1

KEPT static NJS_CNK_MODEL *chimney_models[CHIMNEY_MODELS] = {
#include "stg28/chimney/chimney_models.inc"
};
NJS_TEXLIST *cave_chimney_texlists[CHIMNEY_MODELS + 1] = {
#include "stg28/chimney/cave_chimney_texlists.inc"
};
KEPT static NJS_CNK_MODEL *blast_models[BURST_FRAMES] = {
#include "stg28/chimney/blast_models.inc"
};
NJS_TEXLIST *cave_chimney_blast_texlists[BURST_FRAMES + 1] = {
#include "stg28/chimney/cave_chimney_blast_texlists.inc"
};
/* the chimney's model at each frame of a burst */
static Sint32 swell[BURST_FRAMES] = {1, 2, 3, 4, 5, 4, 3, 2, 1, IDLE_MODEL};
/* clang-format off */
static struct collision_shape chimney_shapes[CHIMNEY_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(SHAPE_TOP_LEVEL), SHAPE_UNK_2000 | SHAPE_UNTURNED,
     {0.0F, -190.0F, 0.0F}, 25.0F, 300.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* each sound's volume, and how near player 0 must lie for the step to play
 * it. the step measures the start's sound by burst_range too, so start_range
 * goes unread */
static Sint32 start_volume = 50;
static Float start_range = 250.0F;
static Sint32 burst_volume = 50;
static Float burst_range = 250.0F;

static void chimney_destroy(struct task *task);
static void chimney_step(struct task *task);
static void chimney_draw(struct task *task);

void cave_chimney_init(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y (   )";
        (void)"<- SCALE(1.0+x)";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = chimney_draw;
    task->step = chimney_step;
    task->destructor = chimney_destroy;
    BURST_FRAME(task) = 0;
    task->work->mode = MODE_IDLE;
    collision_init(task, chimney_shapes, CHIMNEY_SHAPES, COLLISION_OBJECT);
}

static void chimney_destroy(struct task *task)
{
}

static void chimney_step(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    /* @hack redundant: the else after the return */
    if (placement_try_leave(task)) {
        return;
    } else {
        collision_register(task);
    }
    if (task->work->mode == MODE_BURST && !(game_state.steps & 1) &&
        ++BURST_FRAME(task) == BURST_FRAMES) {
        task->work->mode = MODE_IDLE;
        BURST_FRAME(task) = 0;
    }
    if (task->work->mode == MODE_BURST) {
        pos = work->pos;
        pos.y += SOUND_HEIGHT;
        if (burst_range > njDistanceP2P(&player_work[0]->pos, &pos)) {
            sound_play_at_for(BURST_SOUND, task, BURST_SOUND_PRIORITY,
                              burst_volume, BURST_SOUND_STEPS, &pos);
        }
    }
    if (task->work->mode == MODE_IDLE && rand() * RAND_SCALE < BURST_CHANCE) {
        pos = work->pos;
        pos.y += SOUND_HEIGHT;
        task->work->mode = MODE_BURST;
        BURST_FRAME(task) = 0;
        if (burst_range > njDistanceP2P(&player_work[0]->pos, &pos)) {
            sound_play_at(START_SOUND, NULL, START_SOUND_PRIORITY, start_volume,
                          &pos);
        }
    }
}

static void chimney_draw(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    if (task->work->mode == MODE_BURST) {
        njSetTexture(cave_chimney_texlists[swell[BURST_FRAME(task)]]);
        cave_model_draw_scaled(chimney_models[swell[BURST_FRAME(task)]],
                               SCALE(task->work));
        njTranslate(NULL, 0.0F, BLAST_HEIGHT, 0.0F);
        /* the blast draws in the camera's own axes, turned a half turn about
         * x. so it faces the camera however the chimney and the camera turn */
        njGetTranslation(NULL, &pos);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&pos);
        njRotateX(NULL, HALF_TURN);
        njFogDisable();
        draw_control_save();
        draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
        njSetCnkBlendMode(ADDITIVE_BLEND);
        njSetTexture(cave_chimney_blast_texlists[BURST_FRAME(task)]);
        cave_model_draw_scaled(blast_models[BURST_FRAME(task)],
                               SCALE(task->work));
        njSetCnkBlendMode(ALPHA_BLEND);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        draw_control_restore();
        njFogEnable();
        njPopMatrixEx();
    } else {
        njSetTexture(cave_chimney_texlists[IDLE_MODEL]);
        cave_model_draw_scaled(chimney_models[IDLE_MODEL], SCALE(task->work));
    }
    njPopMatrixEx();
}
