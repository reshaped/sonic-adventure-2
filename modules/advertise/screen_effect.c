/* the screen effect: an animated model drawn under a camera motion of its
 * own. it shows and hides at once as its work's shown changes */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/screen_effect.h"
#include "advertise/screen_effect_art.h"
#include "camera/viewport.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "system/task.h"
#include "system/texture.h"

#include "advertise/screen_effect/text.inc"

struct screen_effect_anim screen_effect_anim;
struct screen_effect_work screen_effect_work;

#define TASK_LEVEL 2
/* the level moves from 0 to 1 in one step, so the effect shows and hides
 * at once */
#define FADE_STEP 1.0F

/* nothing calls it. it isn't static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* destroy the task, where there is one */
void screen_effect_end(void);
/* NOLINTEND(misc-use-internal-linkage) */

static struct screen_effect_anim anim_start = {0.0F, 0.0F};
/* the task screen_effect_create made, or null */
static struct task *effect_task = NULL;
static NJS_TEXNAME texnames[1] = {
#include "advertise/screen_effect/texnames.inc"
};
static NJS_TEXLIST texlist = {
#include "advertise/screen_effect/texlist.inc"
};

static void screen_effect_step(struct task *task)
{
    screen_effect_work.fading = FALSE;
    if (screen_effect_work.shown) {
        if (menu_approach(&screen_effect_anim.level, 1.0F, FADE_STEP)) {
            screen_effect_work.fading = TRUE;
        }
    } else {
        if (menu_approach(&screen_effect_anim.level, 0.0F, FADE_STEP)) {
            screen_effect_work.fading = TRUE;
        }
    }
    screen_effect_anim.frame += 1.0F;
    if (screen_effect_anim.frame >= screen_effect_motion[0].nbFrame) {
        screen_effect_anim.frame = 0.0F;
    }
}

static void screen_effect_draw(struct task *task)
{
    NJS_CAMERA camera;

    /* @hack spelling: the level tested alone, where a compare with 0.0F
     * would do */
    if (screen_effect_anim.level) {
        viewport_set_whole();
        njPushMatrixEx();
        menu_fade_begin(screen_effect_anim.level);
        njInitCamera(&camera);
        easy_light_set_direction(0.0F, 0.0F, -1.0F);
        njCnkSetEasyLightIntensity(1.0F, 0.2F);
        njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
        njCameraMotion(&camera, &screen_effect_camera_motion, 0.0F);
        njSetTexture(&texlist);
        njCnkEasyDrawMotion(&screen_effect_object, screen_effect_motion,
                            screen_effect_anim.frame);
        menu_tint_end();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        njPopMatrixEx();
    }
}

static void screen_effect_destroy(struct task *task)
{
    screen_effect_release();
}

void screen_effect_create(void)
{
    struct task *task;
    Uint8 *byte;
    int left;

    task = task_create(TASK_WORK, TASK_LEVEL, screen_effect_step,
                       "screenEffectExec");
    task->draw = screen_effect_draw;
    task->destructor = screen_effect_destroy;
    effect_task = task;
    byte = (Uint8 *)&screen_effect_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(screen_effect_work); left != 0; left--) {
        *byte++ = 0;
    }
    screen_effect_reset();
}

void screen_effect_reset(void)
{
    Sint32 loaded;
    Uint8 *byte;
    int left;

    loaded = screen_effect_work.loaded;
    byte = (Uint8 *)&screen_effect_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(screen_effect_work); left != 0; left--) {
        *byte++ = 0;
    }
    screen_effect_anim = anim_start;
    screen_effect_work.loaded = loaded;
}

void screen_effect_end(void)
{
    if (effect_task) {
        task_destroy(effect_task);
        effect_task = NULL;
    }
}

void screen_effect_load(void)
{
    if (!screen_effect_work.loaded) {
        texture_load_pvm(SCREEN_EFFECT_ARCHIVE, &texlist);
        screen_effect_work.loaded = TRUE;
    }
}

void screen_effect_release(void)
{
    /* @hack spelling: the return where the work isn't loaded, where an if
     * around the release would do */
    if (!screen_effect_work.loaded) {
        return;
    }
    njReleaseTexture(&texlist);
    screen_effect_work.loaded = FALSE;
}
