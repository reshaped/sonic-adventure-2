#include <ninja.h>
#include "chao/garden.h"
#include "chaostglobby/gate.h"
#include "chaostglobby/gate_art.h"
#include "collision/collision.h"
#include "system/task.h"
#include "system/util.h"

#define THRESHOLD_Y (-0.5F)
#define THRESHOLD_Z 3.0F

struct collision_shape lobby_neutral_shapes[GATE_SHAPES] = {
#include "chaostglobby/neutral/lobby_neutral_shapes.inc"
};
/* the count of shapes the dark, hero and kindergarten gates give their
 * collision records */
int lobby_gate_shape_count = GATE_SHAPES;

static void neutral_gate_step(struct task *task)
{
    /* @hack spelling: gate, a copy of task for both calls, where task would
     * do */
    struct task *gate = task;
    if (collision_next_player_by_shape(gate, GATE_DOORWAY_SHAPE)) {
        chao_change_area(CHAO_AREA_NEUTRAL);
    }
    collision_register(gate);
}

static void neutral_gate_draw(struct task *task)
{
    struct task_work *work = task->work;
    GATE_DRAW_BEGIN(work);
    njCnkSimpleDrawObject(&lobby_neutral_arch);
    njTranslate(NULL, 0.0F, THRESHOLD_Y, THRESHOLD_Z);
    njCnkSimpleDrawObject(&lobby_threshold);
    njPopMatrixEx();
    draw_control_clear(GATE_SHADOW);
}

static void neutral_gate_destroy(struct task *task)
{
}

struct task *lobby_neutral_gate_create(NJS_POINT3 *pos, Angle yaw)
{
    struct task *task = task_create(TASK_WORK, 2, neutral_gate_step,
                                    "ALO_LobbyGateNeutExecutor");
    if (task) {
        struct task_work *work = task->work;
        GATE_INSTALL(task, work, pos, yaw, lobby_neutral_shapes, GATE_SHAPES,
                     neutral_gate_draw, neutral_gate_destroy);
    }
    return task;
}

/* nothing calls it. it isn't static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void fun_0c9c1376(void)
{
}
