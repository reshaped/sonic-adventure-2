#include <ninja.h>
#include "chao/garden.h"
#include "chaostglobby/gate.h"
#include "chaostglobby/gate_art.h"
#include "collision/collision.h"
#include "draw/lines.h"
#include "system/task.h"
#include "system/util.h"

/* tick holds the flames' frame, and scl.x the part of a frame past it */
#define FLAME_FRAMES 16
#define FLAME_ATTR (NJD_SPRITE_COLOR | NJD_SPRITE_SCALE | NJD_SPRITE_ALPHA)
#define FLAME_SCALE 0.2F

/* where each flame burns, FLAME_X to either side of the gate's middle. the
 * one at FLAME_X runs FLAME_LEAD frames ahead of the other */
#define FLAME_X 15.8F
#define FLAME_Y 22.0F
#define FLAME_Z 2.6F
#define FLAME_LEAD 7

#define THRESHOLD_Z 2.0F

/* the steps after its creation that the gate keeps its shapes out of the
 * checks. timer counts them */
#define START_STEPS 11

#define LOBBY_DARK_SHAPES 9

static struct collision_shape lobby_dark_shapes[LOBBY_DARK_SHAPES] = {
#include "chaostglobby/dark/lobby_dark_shapes.inc"
};

/* frame n shows texture n + 9, except that frame 5 repeats frame 4's texture
 * 13 */
static NJS_TEXANIM lobby_gate_flames[FLAME_FRAMES] = {
#include "chaostglobby/dark/lobby_gate_flames.inc"
};

static void dark_gate_step(struct task *task)
{
    struct task_work *work = task->work;

    work->scl.x += lobby_gate_flame_speed;
    if (work->scl.x > 1.0F) {
        work->tick++;
        if (work->tick > FLAME_FRAMES - 1) {
            work->tick = 0;
        }
        work->scl.x -= 1.0F;
    }
    if (collision_next_player_by_shape(task, GATE_DOORWAY_SHAPE)) {
        chao_change_area(CHAO_AREA_DARK);
    }
    if (work->timer > START_STEPS - 1) {
        collision_register(task);
    } else {
        collision_reset(task);
        work->timer++;
    }
}

/* draw the flame at flame_x: frame tick + first, fading into the frame after
 * it by scl.x */
#define GATE_FLAME(sprite, work, flame_x, first)                               \
    do {                                                                       \
        njPushMatrixEx();                                                      \
        njTranslate(NULL, (flame_x), FLAME_Y, FLAME_Z);                        \
        constant_material_set(1.0F - (work)->scl.x, 1.0F, 1.0F, 1.0F);         \
        njDrawSprite3D(&(sprite), ((work)->tick + (first)) % FLAME_FRAMES,     \
                       FLAME_ATTR);                                            \
        constant_material_set((work)->scl.x, 1.0F, 1.0F, 1.0F);                \
        njDrawSprite3D(&(sprite), ((work)->tick + (first) + 1) % FLAME_FRAMES, \
                       FLAME_ATTR);                                            \
        njPopMatrixEx();                                                       \
    } while (0)

static void dark_gate_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_SPRITE sprite;

    sprite.tlist = &lobby_texlist;
    sprite.tanim = lobby_gate_flames;
    sprite.ang = 0;
    sprite.sx = FLAME_SCALE;
    sprite.sy = FLAME_SCALE;
    sprite.p.x = 0.0F;
    sprite.p.y = 0.0F;
    sprite.p.z = 0.0F;
    GATE_DRAW_BEGIN(work);
    njCnkSimpleDrawObject(&lobby_dark_arch);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    GATE_FLAME(sprite, work, -FLAME_X, 0);
    GATE_FLAME(sprite, work, FLAME_X, FLAME_LEAD);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    njTranslate(NULL, 0.0F, 0.0F, THRESHOLD_Z);
    njCnkSimpleDrawObject(&lobby_dark_threshold);
    njPopMatrixEx();
    draw_control_clear(GATE_SHADOW);
}

static void dark_gate_destroy(struct task *task)
{
}

struct task *lobby_dark_gate_create(NJS_POINT3 *pos, Angle yaw)
{
    struct task *task =
        task_create(TASK_WORK, 2, dark_gate_step, "ALO_LobbyGateDarkExecutor");
    if (task) {
        struct task_work *work = task->work;
        /* @bug logic: the gate's collision record leaves out the ninth and
         * last of lobby_dark_shapes, the capsule at x -19 that mirrors the
         * one at 19. the record takes as many shapes as
         * lobby_gate_shape_count says (0x0c9c0e3e), which is 8 (0x0c9f29a8) */
        GATE_INSTALL(task, work, pos, yaw, lobby_dark_shapes,
                     lobby_gate_shape_count, dark_gate_draw, dark_gate_destroy);
    }
    return task;
}

/* nothing calls it. it isn't static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void fun_0c9c0e6c(void)
{
}
