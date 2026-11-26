/* A bolt whose segment passes its length, where the heap gives no light:
 * thunder_step kills no null task, and flashes. */
#include <assert.h>
#include <stdlib.h>

#include "effect/thunder.c"

struct game_state game_state;
struct player_hooks player_hooks;
struct task_work *player_work[PLAYER_SLOTS];
struct player *player_records[PLAYER_SLOTS];
static int kills;

/* the heap gives no task, so thunder_light_task_create returns null */
struct task *task_create(Uint16 flags, int level,
                         void (*step)(struct task *task), const char *name)
{
    return NULL;
}

void task_kill(struct task *task)
{
    assert(task != NULL);
    kills++;
}

void task_destroy(struct task *task)
{
}

void *syMalloc(Uint32 size)
{
    return malloc(size);
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

Float njUnitVector(NJS_VECTOR *v)
{
    return 0.0F;
}

Float njDistanceP2L(NJS_POINT3 *p, NJS_LINE *l, NJS_POINT3 *cp)
{
    *cp = *p;
    return 0.0F;
}

/* every distance is 0, past the bolt's length of -1 */
Float njDistanceP2P(NJS_POINT3 *p1, NJS_POINT3 *p2)
{
    return 0.0F;
}

void line_queue(NJS_POINT3 *points, NJS_COLOR *colors, enum line_kind kind)
{
}

static int flashes;

static void count_flash(Float x, Float y, Float z, Float scale, Uint8 sparks,
                        Float depth_bias)
{
    flashes++;
}

int main(void)
{
    struct thunder_bolt bolt = {0};
    struct task task = {0};

    bolt.steps = 1;
    bolt.length = -1.0F;
    task.extra.p = &bolt;
    player_hooks.flash = count_flash;
    thunder_step(&task);
    assert(kills == 0 && flashes == 1);
    return 0;
}
