/* the spider web, SPIDERWEB in Pyramid Cave's object list: a web, turned and
 * scaled as its work says */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "player/player.h"
#include "stg28/cave_util.h"
#include "stg28/spider_web.h"
#include "system/port.h"
#include "system/task.h"

#define SPIDER_WEB_KINDS 1
#define KIND(task) ((Sint32)njAbs((task)->work->scl.z) % SPIDER_WEB_KINDS)

#include "stg28/spider_web/text.inc"
#include "stg28/models/spider_web.h"

static void spider_web_destroy(struct task *task);
static void spider_web_step(struct task *task);
static void spider_web_draw(struct task *task);

KEPT static NJS_CNK_MODEL *spider_web_models[SPIDER_WEB_KINDS] = {
#include "stg28/spider_web/spider_web_models.inc"
};
NJS_TEXLIST *cave_spider_web_texlists[SPIDER_WEB_KINDS + 1] = {
#include "stg28/spider_web/cave_spider_web_texlists.inc"
};
static Sint32 dat_8c5f4338 = 0;
static char *spider_web_kind_names[SPIDER_WEB_KINDS] = {
#include "stg28/spider_web/spider_web_kind_names.inc"
};

void cave_spider_web_init(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE X (   )";
        (void)"<- ANGLE Y (   )";
        (void)"<- ANGLE Z (   )";
        (void)"<- SCALE (1.0+x)";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = spider_web_draw;
    task->step = spider_web_step;
    task->destructor = spider_web_destroy;
}

static void spider_web_destroy(struct task *task)
{
}

static void spider_web_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a return that ends the function anyway */
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches */
    if (0) {
        struct task_work *player = player_work[0];
        struct task_work *other = player_work[1];
        Float scale = work->scl.x;

        placement_try_leave(NULL);
        /* NOLINTBEGIN(misc-redundant-expression) */
        if (player && player && player && player && other && other && scale &&
            scale && work && work) {
        }
        /* NOLINTEND(misc-redundant-expression) */
    }
}

static void spider_web_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njRotateZ(NULL, work->ang.z);
    njSetTexture(cave_spider_web_texlists[KIND(task)]);
    /* @hack spelling: the work read again through task, where work would do */
    cave_model_draw_scaled(spider_web_models[KIND(task)],
                           task->work->scl.x + 1.0F);
    njPopMatrixEx();
}
