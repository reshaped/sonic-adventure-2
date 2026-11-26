#include <ninja.h>
#include "chao/garden.h"
#include "chaostglobby/gate.h"
#include "chaostglobby/gate_art.h"
#include "collision/collision.h"
#include "system/task.h"
#include "system/util.h"

/* work->timer holds the wheels' turn about y, which wraps at a whole turn.
 * it falls by WHEEL_TURN a step. each wheel adds its object's own ang[1], and
 * wheel b adds WHEEL_B_OFFSET too */
#define WHEEL_TURN 0x100
#define WHEEL_B_OFFSET 0x2000

#define THRESHOLD_Z 1.0F

static void hero_gate_step(struct task *task)
{
    struct task_work *work = task->work;
    work->timer -= WHEEL_TURN;
    if (collision_next_player_by_shape(task, GATE_DOORWAY_SHAPE)) {
        chao_change_area(CHAO_AREA_HERO);
    }
    collision_register(task);
}

/* draw wheel's model at its position, tilted about x by its ang[0].
 * rotation then turns it about y. the draw leaves its ang[2] out */
#define WHEEL_DRAW(wheel, rotation)                                            \
    do {                                                                       \
        njPushMatrixEx();                                                      \
        njTranslateEx((NJS_VECTOR *)(wheel).pos);                              \
        njRotateY(NULL, (rotation));                                           \
        njRotateX(NULL, (wheel).ang[0]);                                       \
        njCnkSimpleDrawModel((wheel).model);                                   \
        njPopMatrixEx();                                                       \
    } while (0)

static void hero_gate_draw(struct task *task)
{
    struct task_work *work = task->work;
    GATE_DRAW_BEGIN(work);
    njCnkSimpleDrawObject(&lobby_hero_arch);
    njCnkSimpleDrawObject(&lobby_hero_left);
    njCnkSimpleDrawObject(&lobby_hero_right);
    draw_control_save();
    draw_control_clear(NJD_CONTROL_3D_CONSTANT_ATTR |
                       NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL);
    draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
    draw_attr_save();
    draw_attr_set(0, NJD_FST_ENV);
    draw_attr_clear(0, NJD_FST_FL | NJD_FST_DB | NJD_FST_UA | NJD_FST_IS);
    WHEEL_DRAW(lobby_hero_wheel_a, lobby_hero_wheel_a.ang[1] + work->timer);
    WHEEL_DRAW(lobby_hero_wheel_b,
               lobby_hero_wheel_b.ang[1] + work->timer + WHEEL_B_OFFSET);
    draw_attr_restore();
    draw_control_restore();
    njTranslate(NULL, 0.0F, 0.0F, THRESHOLD_Z);
    njCnkSimpleDrawObject(&lobby_hero_threshold);
    njPopMatrixEx();
    draw_control_clear(GATE_SHADOW);
}

static void hero_gate_destroy(struct task *task)
{
}

struct task *lobby_hero_gate_create(NJS_POINT3 *pos, Angle yaw)
{
    struct task *task =
        task_create(TASK_WORK, 2, hero_gate_step, "ALO_LobbyGateHeroExecutor");
    if (task) {
        struct task_work *work = task->work;
        GATE_INSTALL(task, work, pos, yaw, lobby_neutral_shapes,
                     lobby_gate_shape_count, hero_gate_draw, hero_gate_destroy);
    }
    return task;
}

/* nothing calls it. it isn't static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void fun_0c9c1066(void)
{
}
