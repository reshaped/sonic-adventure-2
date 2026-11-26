/* the picture whose textures are named tento_ending, which OTHERPRINT.PRS
 * shows under OTHER_PRINT_ENDING */
#include <ninja.h>
#include <sg_xpt.h>

#include "otherprint/otherprint.h"
#include "otherprint/tento_ending.h"
#include "shared/screen_sprite.h"
#include "system/task.h"
#include "system/texture.h"

#define TASK_LEVEL 2
#define FADE_STEP (1.0F / 30)
#define HOLD_STEPS 300
#define PICTURE_TEXTURE 0
#define PICTURE_PRIORITY (-10.0F)

enum { MODE_IN, MODE_HOLD, MODE_OUT };

struct tento_ending_fade {
    Float level; /* screen_draw_begin's level, from 0 to 1 */
    Sint32 held; /* the steps the hold has lasted */
};
static struct tento_ending_fade tento_ending_fade;
/* step the level by FADE_STEP toward target. where the level is at target
 * already, nothing changes and the result is TRUE */
/* @hack spelling: the step's result compared with FALSE, where its
 * negation would do */
#define FADE_TO(target)                                                        \
    (float_step_toward(&tento_ending_fade.level, (target), FADE_STEP) == FALSE)

#include "otherprint/tento_ending/text.inc"

static NJS_TEXNAME tento_ending_texnames[8] = {
#include "otherprint/tento_ending/tento_ending_texnames.inc"
};
static NJS_TEXLIST tento_ending_texlist = {
#include "otherprint/tento_ending/tento_ending_texlist.inc"
};

static void tento_ending_step(struct task *task);
static void tento_ending_draw(struct task *task);
static void tento_ending_destroy(struct task *task);

void tento_ending_start(void)
{
#ifdef NONMATCHING
    /* this side ends the ending where the texture list is empty or the textures
     * don't load. the disc-free build's zeros leave the list empty. it ends the
     * ending too where task_create fails, which the matching side writes
     * through */
    if (tento_ending_texlist.nbTexture == 0 ||
        !texture_load_pvm("tentoEnding", &tento_ending_texlist)) {
        otherprint_done = TRUE;
        return;
    }
    if (task_create(TASK_WORK, TASK_LEVEL, tento_ending_step,
                    "execTentoEnding") == NULL) {
        texture_discard_pvm(&tento_ending_texlist);
        otherprint_done = TRUE;
        return;
    }
#else
    texture_load_pvm("tentoEnding", &tento_ending_texlist);
    task_create(TASK_WORK, TASK_LEVEL, tento_ending_step, "execTentoEnding");
#endif
    tento_ending_fade.level = 0.0F;
}

static void tento_ending_step(struct task *task)
{
    struct task_work *work = task->work;

    task->destructor = tento_ending_destroy;
    task->draw = tento_ending_draw;
    switch (work->mode) {
    case MODE_IN:
        if (FADE_TO(1.0F)) {
            work->mode = MODE_HOLD;
            tento_ending_fade.held = 0;
        }
        break;
    case MODE_HOLD:
        tento_ending_fade.held++;
        if (tento_ending_fade.held >= HOLD_STEPS) {
            work->mode = MODE_OUT;
        }
        break;
    case MODE_OUT:
        if (FADE_TO(0.0F)) {
            task_destroy(task);
        }
        break;
    }
}

static void tento_ending_draw(struct task *task)
{
    screen_draw_begin(tento_ending_fade.level);
    screen_picture_draw(PICTURE_TEXTURE, &tento_ending_texlist,
                        PICTURE_PRIORITY, NJD_SPRITE_COLOR | NJD_SPRITE_ALPHA);
    screen_draw_end();
}

static void tento_ending_destroy(struct task *task)
{
    njReleaseTexture(&tento_ending_texlist);
    otherprint_done = TRUE;
}
