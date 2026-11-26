/* Step a volley shot whose victim's slot is empty. */
#include <assert.h>
#include <stdlib.h>

#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "player/player.h"
#include "player/walker_weapons.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

struct game_state game_state;
struct task *player_tasks[PLAYER_SLOTS];
struct task_work *player_work[PLAYER_SLOTS];

static int kills;

void task_kill(struct task *task)
{
    kills++;
}

/* the shot reaches none of these at an empty slot */
Float njScalor(NJS_VECTOR *v)
{
    abort();
}
void njSubVector(NJS_VECTOR *vd, NJS_VECTOR *vs)
{
    abort();
}
void njAddVector(NJS_VECTOR *vd, NJS_VECTOR *vs)
{
    abort();
}
Float njUnitVector(NJS_VECTOR *v)
{
    abort();
}
Float njInnerProduct(NJS_VECTOR *v1, NJS_VECTOR *v2)
{
    abort();
}
Bool njPushMatrixEx(void)
{
    abort();
}
Bool njPopMatrixEx(void)
{
    abort();
}
void njUnitMatrix(NJS_MATRIX *m)
{
    abort();
}
void njRotate(NJS_MATRIX *m, NJS_VECTOR *v, Angle ang)
{
    abort();
}
void njCalcVector(NJS_MATRIX *m, NJS_VECTOR *vs, NJS_VECTOR *vd)
{
    abort();
}
Float njDistanceP2P(NJS_POINT3 *p1, NJS_POINT3 *p2)
{
    abort();
}
void walker_tracer_create(int player, NJS_POINT3 *from, NJS_POINT3 *to)
{
    abort();
}
void collision_get_shape_center(struct task_work *work,
                                const struct collision_shape *shape,
                                NJS_POINT3 *pos)
{
    abort();
}
Bool placement_try_leave_by(struct task *task, Float range)
{
    abort();
}
Bool collision_register(struct task *task)
{
    abort();
}

static void volley_draw(struct task *task)
{
}

#include "extracted.inc"

int main(void)
{
    struct homing shot = {0, 1};
    struct task task = {0};
    struct task parent = {0};
    struct task_work work = {0};

    task.parent = &parent;
    task.work = &work;
    task.extra.p = &shot;
    task.draw = volley_draw;
    game_state.steps = 2;
    volley_step(&task);
    assert(kills == 1 && task.draw == NULL);
    return 0;
}
