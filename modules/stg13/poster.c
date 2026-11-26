/* the posters, POSTER, POSTERM and POSTER3 in City Escape's object list: flat
 * textured quads */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "player/player.h"
#include "stg13/poster.h"
#include "system/task.h"
#include "system/util.h"
#ifdef NONMATCHING
/* texture_slot_loaded, which this side's draws test */
#include "system/texture.h"
#endif

/* scl.z, modulo POSTER_TEXTURES, picks a poster's texture of poster_texlist.
 * the init keeps it in the work's submode */
#define POSTER_TEXTURES 10
#define TEXTURE(work) ((work)->submode)
#define TRANSLUCENT_TEXTURE 8
/* the gray a POSTERM tints its quad */
#define SHADE(task) ((task)->unk_3c.l)
/* a color whose red, green and blue are each level, with no alpha */
#define GRAY(level) (((level) << 16) | ((level) << 8) | (level))
#define OPAQUE 0xff000000
#define WHITE 0xffffffff
#define QUAD_VERTICES 4

static void poster_destroy(struct task *task);
static void poster_draw(struct task *task);
static void poster_m_draw(struct task *task);
static void poster_step(struct task *task);
static void poster_m_step(struct task *task);
static void poster_3_draw(struct task *task);
static void poster_3_step(struct task *task);

#include "stg13/poster/text.inc"

static NJS_TEXNAME poster_texnames[POSTER_TEXTURES] = {
#include "stg13/poster/poster_texnames.inc"
};
static NJS_TEXLIST poster_texlist = {
#include "stg13/poster/poster_texlist.inc"
};
static NJS_TEXNAME poster_3_texnames[1] = {
#include "stg13/poster/poster_3_texnames.inc"
};
static NJS_TEXLIST poster_3_texlist = {
#include "stg13/poster/poster_3_texlist.inc"
};
NJS_TEXLIST *poster_texlists[3] = {
#include "stg13/poster/poster_texlists.inc"
};

void poster_init(struct task *task)
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

void poster_m_init(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 gray;

    if (placement_try_leave(task)) {
        return;
    }
    task->step = poster_m_step;
    task->draw = poster_m_draw;
    task->destructor = poster_destroy;
    TEXTURE(work) = (Uint32)work->scl.z % POSTER_TEXTURES;
    gray = (Uint8)work->ang.z;
    SHADE(task) = GRAY(gray) | OPAQUE;
}

static void poster_destroy(struct task *task)
{
}

static void poster_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;
    NJS_TEXTURE_VTX quad[QUAD_VERTICES];
    NJS_TEXTURE_VTX *vertex = quad;

#ifdef NONMATCHING
    /* this side draws nothing where the texture that njSetTextureNum takes
     * below lies past the list or has no address */
    if (!texture_slot_loaded(&poster_texlist, TEXTURE(work))) {
        return;
    }
#endif
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
    njSetTexture(&poster_texlist);
    njSetTextureNum(TEXTURE(work));
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: the comparison's result turned into TRUE or FALSE,
     * where the comparison passed in place would do */
    njDrawTexture3DEx(quad, QUAD_VERTICES,
                      TEXTURE(work) == TRANSLUCENT_TEXTURE ? TRUE : FALSE);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static void poster_m_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;
    NJS_TEXTURE_VTX quad[QUAD_VERTICES];
    NJS_TEXTURE_VTX *vertex = quad;

#ifdef NONMATCHING
    /* this side draws nothing where the texture that njSetTextureNum takes
     * below lies past the list or has no address */
    if (!texture_slot_loaded(&poster_texlist, TEXTURE(work))) {
        return;
    }
#endif
    vertex->x = -work->scl.x;
    vertex->y = -work->scl.y;
    vertex->z = 0.0F;
    vertex->col = SHADE(task);
    vertex->u = 1.0F;
    vertex->v = 1.0F;
    vertex++;
    vertex->x = -work->scl.x;
    vertex->y = work->scl.y;
    vertex->z = 0.0F;
    vertex->col = SHADE(task);
    vertex->u = 1.0F;
    vertex->v = 0.0F;
    vertex++;
    vertex->x = work->scl.x;
    vertex->y = -work->scl.y;
    vertex->z = 0.0F;
    vertex->col = SHADE(task);
    vertex->u = 0.0F;
    vertex->v = 1.0F;
    vertex++;
    vertex->x = work->scl.x;
    vertex->y = work->scl.y;
    vertex->z = 0.0F;
    vertex->col = SHADE(task);
    vertex->u = 0.0F;
    vertex->v = 0.0F;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(&poster_texlist);
    njSetTextureNum(TEXTURE(work));
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
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
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants, and for the registers SHC
     * gives the step. the image holds none of its code, so its statements are
     * stand-ins */
    if (0) {
        struct task_work *player = player_work[0];

        (void)"<- ANGLE Y";
        (void)"<- SCL W";
        (void)"<- SCL H";
        (void)"<- TEX(0-9)";
        njSetTextureNum(TEXTURE(work));
        work->ang.z = 0;
        player->ang.x = 0;
        player->ang.y = 0;
        player->ang.z = 0;
    }
}

static void poster_m_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for its strings and the
     * registers SHC gives the step, as poster_step's is */
    if (0) {
        struct task_work *player = player_work[0];

        (void)"<- ANGLE Y";
        (void)"<- SCL W";
        (void)"<- SCL H";
        (void)"<- TEX(0-9)";
        (void)"<- COL(0-ff)";
        njSetTextureNum(TEXTURE(work));
        placement_kill(task);
        work->ang.x = 0;
        work->ang.z = 0;
        player->ang.x = 0;
        player->ang.y = 0;
        player->ang.z = 0;
        player->mode = 0;
    }
}

void poster_3_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->step = poster_3_step;
    task->draw = poster_3_draw;
    task->destructor = poster_destroy;
}

static void poster_3_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;
    NJS_TEXTURE_VTX quad[QUAD_VERTICES];
    NJS_TEXTURE_VTX *vertex = quad;

#ifdef NONMATCHING
    /* this side draws nothing where the texture that njSetTextureNum takes
     * below lies past the list or has no address */
    if (!texture_slot_loaded(&poster_3_texlist, 0)) {
        return;
    }
#endif
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
    njSetTexture(&poster_3_texlist);
    njSetTextureNum(0);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njDrawTexture3DEx(quad, QUAD_VERTICES, TRUE);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static void poster_3_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for its strings and the
     * registers SHC gives the step, as poster_step's is */
    if (0) {
        struct task_work *player = player_work[0];

        (void)"<- ANGLE Y";
        (void)"<- TEX(0-8)";
        (void)"<- ANGLE X";
        (void)"<- ANGLE Z";
        njSetTextureNum(0);
        player->ang.x = 0;
        player->ang.y = 0;
        player->ang.z = 0;
    }
}
