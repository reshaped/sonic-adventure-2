/* the foothold, ASHIBA in Mad Space's object list: a floor no player can
 * climb, whose lamp pulses.
 * shared/platform_collision.h gives it its init, step and destructor */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "effect/particle.h"
#include "shared/common_object.h"
#include "stg44/foothold.h"
#include "stg44/scenery_shapes.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#define FENCES 0
#define NEAR_RANGE 80.0F
#define PLATFORM_FLAGS (WORLD_SOLID | WORLD_NO_CLIMB)
/* the lamp's color runs from bright_color to dim_color and back with the
 * sine of game_state.steps * PULSE_RATE */
#define PULSE_RATE 0x300
#define PLATFORM_INIT mad_space_foothold_init
#define PLATFORM_COLLISION mad_space_foothold_collision

NJS_TEXLIST *mad_space_foothold_textures[] = {
    &mad_space_foothold_texlist,
    &mad_space_foothold_alt_texlist,
    NULL,
};
static Uint32 bright_color = 0xffffffff;
static Uint32 dim_color = 0xff101010;

#include "shared/platform_collision.h"

static void frstage_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *lamp;
    Uint32 color;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
        (void)"<- MODEL(0-1)";
    }
    if (work->scl.x >= 1.0F) {
        njSetTexture(&mad_space_foothold_alt_texlist);
        object = &mad_space_foothold_alt_object;
    } else {
        njSetTexture(&mad_space_foothold_texlist);
        object = &mad_space_foothold_object;
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(object->model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    lamp = object->child;
    color = color_blend(bright_color, dim_color,
                        (njSin(game_state.steps * PULSE_RATE) + 1.0F) * 0.5F);
    model_draw_tinted(lamp->model, color);
    njPopMatrixEx();
}
