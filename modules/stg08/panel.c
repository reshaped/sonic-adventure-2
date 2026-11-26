/* PANELAREA in Security Hall's object list: a panel of one of the areas A, B
 * and C. the placement's ang.y turns it, and ang.x picks its areas, as
 * security_hall.h's SAFE_AREA and SAFE_TEXTURE read them */
#include <ninja.h>

#include "object/set.h"
#include "stg08/fixture_art.h"
#include "stg08/prop_textures.h"
#include "stg08/security_hall.h"
#include "stg08/security_props.h"
#include "system/port.h"
#include "system/task.h"

NJS_TEXLIST *security_panel_textures[] = {
    &security_panel_a_texlist,
    &security_panel_b_texlist,
    &security_panel_c_texlist,
    NULL,
};

/* the set editor's labels for ang.x and ang.y */
static const char editor_model[12] = "<- MOD/NUM";
static const char editor_angle_y[] = "<- ANGLE Y";
KEPT static NJS_TEXLIST *textures[SAFE_AREAS] = {
#include "stg08/panel/textures.inc"
};
KEPT static NJS_CNK_OBJECT *objects[SAFE_AREAS] = {
#include "stg08/panel/objects.inc"
};

static void panel_destroy(struct task *task);
static void panel_step(struct task *task);
static void panel_draw(struct task *task);

void security_panel_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = panel_draw;
        task->step = panel_step;
        task->destructor = panel_destroy;
        work->mode = 0;
    }
}

static void panel_destroy(struct task *task)
{
}

static void panel_step(struct task *task)
{
    struct task_work *work = task->work;
    if (!placement_try_leave(task)) {
        /* @hack redundant: a block no path reaches, which clears mode and
         * submode */
        if (0) {
            work->mode = 0;
            work->submode = 0;
        }
    }
}

static void panel_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(textures[SAFE_TEXTURE(work)]);
    njCnkEasyDrawModel(objects[SAFE_AREA(work)]->model);
    njPopMatrixEx();
}
