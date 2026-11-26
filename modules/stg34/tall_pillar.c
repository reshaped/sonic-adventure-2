/* the electric pillar ELEPIL2, whose arc shows a frame of pillar_frames a
 * step */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "object/set.h"
#include "stg34/electric_pillar.h"
#include "stg34/scenery.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the frame of pillar_frames the pillar's arc shows at step 0. each pillar
 * picks its own at random, so that the arcs run apart */
#define ARC_PHASE(task) ((task)->unk_3c.l)

static void pillar_step(struct task *task);
static void pillar_destroy(struct task *task);
static void tall_pillar_draw(struct task *task);

static struct collision_shape pillar_shape;

void tall_pillar_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = tall_pillar_draw;
        task->step = pillar_step;
        task->destructor = pillar_destroy;
#ifdef NONMATCHING
        /* the original's rand gives 15 bits. this takes the low 15 of the
         * host's, so that the same bits pick the same frame */
        ARC_PHASE(task) =
            (int)(((Uint32)rand() & 0x7fffU) * PILLAR_FRAMES >> 15);
#else
        ARC_PHASE(task) = (int)(rand() * (PILLAR_FRAMES * RAND_SCALE));
#endif
        collision_init(task, &pillar_shape, PILLAR_SHAPES, COLLISION_OBJECT);
    }
}

#include "stg34/pillar_impl.h"

static struct collision_shape pillar_shape = PILLAR_SHAPE(25.0F, 700.0F);

NJS_TEXLIST *tall_pillar_texlists[] = {&tall_pillar_texlist, NULL};

static void tall_pillar_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 frame = (ARC_PHASE(task) + game_state.steps) % PILLAR_FRAMES;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njSetTexture(&tall_pillar_texlist);
    njCnkEasyDrawModel(&tall_pillar_model);
    njCnkEasyDrawModel(pillar_frames[frame]);
    njPopMatrixEx();
}
