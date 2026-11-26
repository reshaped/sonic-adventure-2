/* drill_start where syCalloc has no room for the drill's record. it destroys
 * the drill, so no step runs it without the record */
#include <assert.h>
#include <stdlib.h>

#include "modules/stg43/drill.c"

struct path cosmic_wall_first_path, cosmic_wall_second_path;
struct task *drill_triggers;
NJS_OBJECT platform_collision;

static struct task *destroyed;

/* the heap has no room */
void *syCalloc(Uint32 count, Uint32 size)
{
    (void)count;
    (void)size;
    return NULL;
}

void syFree(void *block)
{
    assert(block == NULL);
}

void task_destroy(struct task *task)
{
    assert(destroyed == NULL);
    destroyed = task;
}

/* nothing past the failed syCalloc runs */
static void unreached(void)
{
    assert(!"drill_start went on without its record");
}

Bool collision_init(struct task *task, const struct collision_shape *shapes,
                    int count, Uint8 kind)
{
    (void)task;
    (void)shapes;
    (void)count;
    (void)kind;
    unreached();
    return FALSE;
}

Bool placement_try_leave(struct task *task)
{
    (void)task;
    unreached();
    return FALSE;
}

void world_add(Sint32 flags, struct task *task, NJS_OBJECT *object)
{
    (void)flags;
    (void)task;
    (void)object;
    unreached();
}

void world_remove(struct task *task, NJS_OBJECT *object)
{
    (void)task;
    (void)object;
    unreached();
}

Bool njPushMatrixEx(void)
{
    unreached();
    return TRUE;
}

Bool njPopMatrixEx(void)
{
    unreached();
    return TRUE;
}

void njUnitMatrix(NJS_MATRIX *m)
{
    (void)m;
    unreached();
}

void njRotateX(NJS_MATRIX *m, Angle ang)
{
    (void)m;
    (void)ang;
    unreached();
}

void njRotateZ(NJS_MATRIX *m, Angle ang)
{
    (void)m;
    (void)ang;
    unreached();
}

void njCalcVector(NJS_MATRIX *m, NJS_VECTOR *vs, NJS_VECTOR *vd)
{
    (void)m;
    (void)vs;
    (void)vd;
    unreached();
}

Float njScalor(NJS_VECTOR *v)
{
    (void)v;
    unreached();
    return 0.0F;
}

Float njDistanceP2P(NJS_POINT3 *p1, NJS_POINT3 *p2)
{
    (void)p1;
    (void)p2;
    unreached();
    return 0.0F;
}

int main(void)
{
    struct task drill = {0};
    struct task_work work = {0};

    drill.work = &work;
    drill.step = drill_step;
    assert(!drill_start(&drill, 0, 0.0F, 0.0F));
    assert(destroyed == &drill);
    return 0;
}
