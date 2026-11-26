/* the poster, POSTER in Radical Highway's object list: a flat textured quad.
 * it is stg13/poster.c's POSTER with two textures, turned in all three axes */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "player/player.h"
#include "stg14/poster.h"
#include "system/task.h"
#include "system/util.h"

#include "stg14/poster/text.inc"

/* scl.z, modulo POSTER_TEXTURES, picks a poster's texture of
 * highway_poster_texlist. the init keeps it in the work's submode */
#define POSTER_TEXTURES 2
#define TEXTURE(work) ((work)->submode)
/* the texture the draw takes as translucent. TEXTURE stays below
 * POSTER_TEXTURES, so every poster here draws opaque */
#define TRANSLUCENT_TEXTURE 8
#define WHITE 0xffffffff
#define QUAD_VERTICES 4

/* The two selectable poster textures. */

static NJS_TEXNAME highway_poster_textures[2] = {
#include "stg14/poster/highway_poster_textures.inc"
};

static NJS_TEXLIST highway_poster_texlist = {
#include "stg14/poster/highway_poster_texlist.inc"
};

NJS_TEXLIST *highway_poster_texlists[2] = {
#include "stg14/poster/highway_poster_texlists.inc"
};

static void poster_destroy(struct task *task);
static void poster_draw(struct task *task);
static void poster_step(struct task *task);

void highway_poster_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->step = poster_step;
    task->draw = poster_draw;
    task->destructor = poster_destroy;
    TEXTURE(work) = (Uint32)work->scl.z % POSTER_TEXTURES;
}

static void poster_destroy(struct task *task)
{
}

static void poster_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local nothing reads, which only sizes the
     * frame */
    NJS_POINT3 unused;
    NJS_TEXTURE_VTX quad[QUAD_VERTICES];
    NJS_TEXTURE_VTX *vertex = quad;

    vertex->x = -work->scl.x;
    vertex->y = -work->scl.y;
    vertex->z = 0.0F;
    vertex->col = WHITE;
    vertex->u = 1.0F;
    vertex->v = 1.0F;
    vertex++;
    vertex->x = -work->scl.x;
    vertex->y = work->scl.y;
    vertex->z = 0.0F;
    vertex->col = WHITE;
    vertex->u = 1.0F;
    vertex->v = 0.0F;
    vertex++;
    vertex->x = work->scl.x;
    vertex->y = -work->scl.y;
    vertex->z = 0.0F;
    vertex->col = WHITE;
    vertex->u = 0.0F;
    vertex->v = 1.0F;
    vertex++;
    vertex->x = work->scl.x;
    vertex->y = work->scl.y;
    vertex->z = 0.0F;
    vertex->col = WHITE;
    vertex->u = 0.0F;
    vertex->v = 0.0F;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(&highway_poster_texlist);
    njSetTextureNum(TEXTURE(work));
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    /* @hack spelling: the comparison's result turned into TRUE or FALSE,
     * where the comparison passed in place would do */
    njDrawTexture3DEx(quad, QUAD_VERTICES,
                      TEXTURE(work) == TRANSLUCENT_TEXTURE ? TRUE : FALSE);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static void poster_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for its strings, which SHC
     * still places in this source's constants, and for its statements. the
     * image holds none of its code, so the statements are stand-ins, without
     * which SHC writes other code */
    if (0) {
        struct task_work *player = player_work[0];
        struct task_work *other = player_work[1];

        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
        (void)"<- SCL W";
        (void)"<- SCL H";
        (void)"<- TEX(0-1)";
        njSetTextureNum(TEXTURE(work));
        work->ang.x = 0;
        work->ang.z = 0;
        player->ang.x = 0;
        player->ang.y = 0;
        player->ang.z = 0;
        player->mode = 0;
        player->submode = 0;
        other->ang.x = 0;
        other->ang.y = 0;
        other->ang.z = 0;
    }
}
