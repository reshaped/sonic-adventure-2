#ifdef NONMATCHING
/* memcpy, with which this side's gate copies its shapes */
#include <string.h>
#endif
#include <ninja.h>
#include "chao/garden.h"
#include "chaostglobby/gate.h"
#include "chaostglobby/gate_art.h"
#include "collision/collision.h"
#include "system/task.h"
#include "system/util.h"

#define THRESHOLD_Y (-0.5F)
#define THRESHOLD_Z 9.0F

static void kinder_gate_step(struct task *task)
{
    /* @hack spelling: gate, a copy of task for both calls, where task would
     * do */
    struct task *gate = task;
    if (collision_next_player_by_shape(gate, GATE_DOORWAY_SHAPE)) {
        chao_change_area(CHAO_AREA_KINDERGARTEN);
    }
    collision_register(gate);
}

static void kinder_gate_draw(struct task *task)
{
    struct task_work *work = task->work;
    GATE_DRAW_BEGIN(work);
    njCnkSimpleDrawObject(&lobby_kinder_arch);
    njCnkSimpleDrawObject(&lobby_kinder_left);
    njCnkSimpleDrawObject(&lobby_kinder_right);
    njTranslate(NULL, 0.0F, THRESHOLD_Y, THRESHOLD_Z);
    njCnkSimpleDrawObject(&lobby_threshold);
    njPopMatrixEx();
    draw_control_clear(GATE_SHADOW);
}

static void kinder_gate_destroy(struct task *task)
{
}

struct task *lobby_kinder_gate_create(NJS_POINT3 *pos, Angle yaw)
{
    struct task *task = task_create(TASK_WORK, 2, kinder_gate_step,
                                    "ALO_LobbyGateKinderExecutor");
    if (task) {
        struct task_work *work = task->work;
        /* @bug memory: the gate's collision record takes two shapes past the
         * end of lobby_kinder_shapes, which holds 6. it takes as many as
         * lobby_gate_shape_count says (0x0c9c11d4), which is 8 (0x0c9f29a8).
         * the two are the first bytes of lobby_neutral_arch's polygon list,
         * which starts right past the array (0x0c9e9934) */
#ifdef NONMATCHING
        /* this side keeps the tag's result. GCC doesn't place
         * lobby_neutral_arch's polygons after lobby_kinder_shapes, so the copy
         * puts them there */
        struct collision_shape shapes[GATE_SHAPES];

        memcpy(shapes, lobby_kinder_shapes, sizeof(lobby_kinder_shapes));
        memcpy(&shapes[LOBBY_KINDER_SHAPES], lobby_neutral_arch.model->plist,
               sizeof(shapes) - sizeof(lobby_kinder_shapes));
        GATE_INSTALL(task, work, pos, yaw, shapes, lobby_gate_shape_count,
                     kinder_gate_draw, kinder_gate_destroy);
#else
        GATE_INSTALL(task, work, pos, yaw, lobby_kinder_shapes,
                     lobby_gate_shape_count, kinder_gate_draw,
                     kinder_gate_destroy);
#endif
    }
    return task;
}

/* nothing calls it. it isn't static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void fun_0c9c1202(void)
{
}
