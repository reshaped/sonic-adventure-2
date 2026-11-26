/* FOGSW in Security Hall's object list: a box that changes the fog. the box
 * lies about the placement's pos, turned by ang.y, and its half sizes are
 * scl's. while player 0 lies inside, it sets SECURITY_ALT_FOG in
 * security_background's submode. otherwise it clears it, as its destructor
 * does */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "player/player.h"
#include "stg08/background.h"
#include "stg08/security_props.h"
#include "system/task.h"

/* the set editor's label for ang.y */
static const char editor_angle_y[] = "<- ANGLE Y";

static void fog_switch_destroy(struct task *task);
static void fog_switch_step(struct task *task);

void security_fog_switch_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->step = fog_switch_step;
        task->destructor = fog_switch_destroy;
    }
}

static void fog_switch_destroy(struct task *task)
{
    struct task *background = security_background;

    if (background) {
        background->work->submode &= ~SECURITY_ALT_FOG;
    }
}

/* @hack redundant: pos, a parameter that no line reads. the test reads
 * player 0's pos itself */
static Bool contains_player(struct task_work *work, NJS_POINT3 *pos)
{
    NJS_POINT3 local;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, -work->ang.y);
    njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
    njCalcPoint(NULL, &player_work[0]->pos, &local);
    njPopMatrix(1);
    if (_builtin_fabsf(local.x) < work->scl.x &&
        _builtin_fabsf(local.z) < work->scl.z &&
        _builtin_fabsf(local.y) < work->scl.y) {
        return TRUE;
    }
    return FALSE;
}

static void fog_switch_step(struct task *task)
{
    /* @hack redundant: work a register variable, where a plain one would do */
    register struct task_work *work = task->work;

    /* @hack redundant: unused, a point that no line reads */
    NJS_POINT3 unused;

    if (!placement_try_leave(task)) {
        if (contains_player(work, &player_work[0]->pos)) {
            if (security_background) {
                security_background->work->submode |= SECURITY_ALT_FOG;
            }
        } else {
            fog_switch_destroy(task);
        }
    }
}
