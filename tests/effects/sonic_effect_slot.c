/* Sonic's effects once his slot is empty: the jitter ball draws nothing, and
 * a dash ends at its first step. player_unregister empties the work, the
 * record and the task together, so all three are null here. */
#include <assert.h>
#include <math.h>

#include "effect/sonic_effect.c"

struct model_entry model_table[MODEL_SLOTS];
struct task *player_tasks[PLAYER_SLOTS];
struct task_work *player_work[PLAYER_SLOTS];
struct player *player_records[PLAYER_SLOTS];
static int kills;
static int textures;

void task_kill(struct task *task)
{
    kills++;
}

Sint32 njSetTexture(NJS_TEXLIST *texlist)
{
    textures++;
    return 0;
}

void njFogDisable(void)
{
}

void njFogEnable(void)
{
}

void njCnkEasyDrawObject(NJS_CNK_OBJECT *object)
{
}

Bool njPushMatrixEx(void)
{
    return TRUE;
}

Bool njPopMatrixEx(void)
{
    return TRUE;
}

void njUnitMatrix(NJS_MATRIX *m)
{
}

void njTranslateEx(NJS_VECTOR *v)
{
}

void njRotateX(NJS_MATRIX *m, Angle ang)
{
}

void njRotateY(NJS_MATRIX *m, Angle ang)
{
}

void njRotateZ(NJS_MATRIX *m, Angle ang)
{
}

void njScale(NJS_MATRIX *m, Float sx, Float sy, Float sz)
{
}

void njScaleEx(NJS_VECTOR *v)
{
}

void njCalcPoint(NJS_MATRIX *m, NJS_POINT3 *ps, NJS_POINT3 *pd)
{
    *pd = *ps;
}

void njAddVector(NJS_VECTOR *vd, NJS_VECTOR *vs)
{
    vd->x += vs->x;
    vd->y += vs->y;
    vd->z += vs->z;
}

void njSubVector(NJS_VECTOR *vd, NJS_VECTOR *vs)
{
    vd->x -= vs->x;
    vd->y -= vs->y;
    vd->z -= vs->z;
}

Float njScalor(NJS_VECTOR *v)
{
    return sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

static void check_jitter_draw(void)
{
    struct model_effect ball = {0};
    struct task task = {0};

    task.extra.p = &ball;
    ball_jitter_draw(&task);
    assert(textures == 0);
}

static void check_dash_step(void)
{
    struct model_effect dash = {0};
    struct task task = {0};

    task.extra.p = &dash;
    task.draw = dash_draw;
    kills = 0;
    dash_step(&task);
    assert(kills == 1 && task.draw == NULL);
}

int main(void)
{
    check_jitter_draw();
    check_dash_step();
    return 0;
}
