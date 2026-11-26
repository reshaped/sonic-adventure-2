/* the alpha title: the title's backdrop behind the menus, and the prompt the
 * title screen blinks over it */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/alpha_title.h"
#include "advertise/menu_draw.h"
#include "advertise/menu_util.h"
#include "draw/lines.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

#include "advertise/alpha_title/text.inc"

#define TASK_LEVEL 2
#define FADE_STEP (1.0F / 30)
#define BACKDROP_TEXTURE 0
#define BACKDROP_PRIORITY (-10000.0F)
#define PROMPT_CELL 0

/* nothing calls it. it isn't static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* have the task destroy itself at its next step */
void alpha_title_end(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* alpha_title_ending is TRUE from alpha_title_end until alpha_title_create. */
static Bool alpha_title_ending;
/* @hack type: the fade as a record of one Float, where a Float would do */
struct backdrop_fade {
    Float level;
};
static struct backdrop_fade alpha_title_fade;

static void alpha_title_reset(void);

static struct backdrop_fade fade_start = {0.0F};
static NJS_POINT3 prompt_pos = {
#include "advertise/alpha_title/prompt_pos.inc"
};
static Float dim_levels[2] = {0.3F, 0.1F};
static NJS_TEXNAME texnames[9] = {
#include "advertise/alpha_title/texnames.inc"
};
static NJS_TEXLIST texlist = {
#include "advertise/alpha_title/texlist.inc"
};
/* the prompt, in texture title_parts */
static NJS_TEXANIM cells[1] = {
#include "advertise/alpha_title/cells.inc"
};

static void alpha_title_sprite_draw(Int cell, const NJS_POINT3 *pos, Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = cells;
    sprite.sx = 1.0F;
    sprite.sy = 1.0F;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = cells[cell].attr * ANGLE_PER_DEGREE;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

static void alpha_title_step(struct task *task)
{
    alpha_title_work.fading = FALSE;
    if (!alpha_title_work.shown) {
        if (menu_approach(&alpha_title_fade.level, 0.0F, FADE_STEP)) {
            alpha_title_work.fading = TRUE;
        }
    } else if (!alpha_title_work.bright) {
        if (menu_approach(&alpha_title_fade.level,
                          dim_levels[alpha_title_work.dim], FADE_STEP)) {
            alpha_title_work.fading = TRUE;
        }
    } else {
        if (menu_approach(&alpha_title_fade.level, 1.0F, FADE_STEP)) {
            alpha_title_work.fading = TRUE;
        }
    }
    if (alpha_title_work.fading) {
        alpha_title_work.prompt = FALSE;
    }
    if (alpha_title_ending) {
        task_destroy(task);
    }
}

static void alpha_title_draw(struct task *task)
{
    /* @hack spelling: the fade tested alone, where a compare with 0.0F
     * would do */
    if (alpha_title_fade.level) {
        draw_attr_save();
        constant_material_set(1.0F, alpha_title_fade.level,
                              alpha_title_fade.level, alpha_title_fade.level);
        menu_backdrop_draw(BACKDROP_TEXTURE, &texlist, BACKDROP_PRIORITY, 0);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        /* @hack spelling: the draw as a conditional expression, where an if
         * would do */
        alpha_title_work.prompt
            ? alpha_title_sprite_draw(PROMPT_CELL, &prompt_pos,
                                      NJD_SPRITE_ALPHA)
            : (void)0;
    }
}

static void alpha_title_destroy(struct task *task)
{
    alpha_title_release();
}

void alpha_title_create(void)
{
    struct task *task;
    Uint8 *byte;
    int left;

    task =
        task_create(TASK_WORK, TASK_LEVEL, alpha_title_step, "alphaTitleExec");
    task->draw = alpha_title_draw;
    task->destructor = alpha_title_destroy;
    byte = (Uint8 *)&alpha_title_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(alpha_title_work); left != 0; left--) {
        *byte++ = 0;
    }
    alpha_title_reset();
    alpha_title_ending = FALSE;
}

static void alpha_title_reset(void)
{
    Sint32 loaded;
    Uint8 *byte;
    int left;

    loaded = alpha_title_work.loaded;
    byte = (Uint8 *)&alpha_title_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(alpha_title_work); left != 0; left--) {
        *byte++ = 0;
    }
    alpha_title_fade = fade_start;
    alpha_title_work.loaded = loaded;
}

void alpha_title_end(void)
{
    alpha_title_ending = TRUE;
}

void alpha_title_load(void)
{
    if (!alpha_title_work.loaded) {
        texture_load_pvm(ALPHA_TITLE_ARCHIVE, &texlist);
        alpha_title_work.loaded = TRUE;
    }
}

void alpha_title_release(void)
{
    if (alpha_title_work.loaded) {
        njReleaseTexture(&texlist);
        alpha_title_work.loaded = FALSE;
    }
    /* @hack redundant: a return at the end */
    return; /* NOLINT(readability-redundant-control-flow) */
}
