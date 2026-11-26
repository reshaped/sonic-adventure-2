/* the light effect, LIGHTEFF in a stage's object list: a flat quad, added to
 * what lies behind it. it flickers between three textures */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "object/set.h"
#include "shared/light_effect.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the work holds the step's picks for the draw. submode holds the quad's
 * texture. mode holds whether it flips u, and tick whether it flips v */
#define TEXTURE(work) ((work)->submode)
#define FLIP_U(work) ((work)->mode)
#define FLIP_V(work) ((work)->tick)
#define TEXTURES 3
/* the step picks the texture as a random whole number below ROLLS, modulo
 * TEXTURES. it flips u, and then v, each with a chance of FLIP_CHANCE */
#define ROLLS 100.0F
#define FLIP_CHANCE 0.5F
#define QUAD_VERTICES 4
#define OPAQUE_WHITE 0xffffffff

static void light_effect_destroy(struct task *task);
static void light_effect_draw(struct task *task);
static void light_effect_step(struct task *task);

#include "shared/light_effect/text.inc"

static NJS_TEXNAME light_effect_texnames[TEXTURES] = {
#include "shared/light_effect/light_effect_texnames.inc"
};
static NJS_TEXLIST light_effect_texlist = {light_effect_texnames, TEXTURES};
NJS_TEXLIST *light_effect_texlists[] = {&light_effect_texlist, NULL};

void light_effect_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->step = light_effect_step;
    task->draw = light_effect_draw;
    task->destructor = light_effect_destroy;
    TEXTURE(work) = 0;
}

static void light_effect_destroy(struct task *task)
{
}

static void light_effect_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local nothing reads, which only sizes the
     * frame */
    NJS_POINT3 unused;
    NJS_TEXTURE_VTX quad[QUAD_VERTICES];
    NJS_TEXTURE_VTX *vertex;
    Float left_u;
    Float right_u;
    Float bottom_v;
    Float top_v;

    if (FLIP_U(work)) {
        left_u = 1.0F;
        right_u = 0.0F;
    } else {
        right_u = 1.0F;
        left_u = 0.0F;
    }
    if (FLIP_V(work)) {
        bottom_v = 1.0F;
        top_v = 0.0F;
    } else {
        top_v = 1.0F;
        bottom_v = 0.0F;
    }
    vertex = quad;
    vertex->x = -work->scl.x;
    vertex->y = -work->scl.y;
    vertex->z = 0.0F;
    vertex->col = OPAQUE_WHITE;
    vertex->u = left_u;
    vertex->v = bottom_v;
    vertex++;
    vertex->x = -work->scl.x;
    vertex->y = work->scl.y;
    vertex->z = 0.0F;
    vertex->col = OPAQUE_WHITE;
    vertex->u = left_u;
    vertex->v = top_v;
    vertex++;
    vertex->x = work->scl.x;
    vertex->y = -work->scl.y;
    vertex->z = 0.0F;
    vertex->col = OPAQUE_WHITE;
    vertex->u = right_u;
    vertex->v = bottom_v;
    vertex++;
    vertex->x = work->scl.x;
    vertex->y = work->scl.y;
    vertex->z = 0.0F;
    vertex->col = OPAQUE_WHITE;
    vertex->u = right_u;
    vertex->v = top_v;
    njSetTexture(&light_effect_texlist);
    njSetTextureNum(TEXTURE(work));
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njFogDisable();
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    njDrawTexture3DEx(quad, QUAD_VERTICES, TRUE);
    njPopMatrixEx();
    njFogEnable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void light_effect_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
        (void)"<- SCL W";
        (void)"<- SCL H";
    }
    if (game_state.steps & 1) {
        TEXTURE(work) = (int)((Float)rand() * (ROLLS * RAND_SCALE)) % TEXTURES;
        if ((Float)rand() * RAND_SCALE < FLIP_CHANCE) {
            FLIP_U(work) = TRUE;
        } else {
            FLIP_U(work) = FALSE;
        }
        if ((Float)rand() * RAND_SCALE < FLIP_CHANCE) {
            FLIP_V(work) = TRUE;
        } else {
            FLIP_V(work) = FALSE;
        }
    }
}
