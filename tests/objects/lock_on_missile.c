/* Run the Iron Gate launcher through child allocation, flight and landing. */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <shinobi.h>

#include "collision/collision.h"
#include "effect/burst.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/missile.h"
#include "shared/rocket_thrust.h"
#include "shared/switch.h"
#include "sound/sound.h"
#include "stg11/lock_on_missile.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

struct game_state game_state;
struct missile_target missile_targets[MISSILE_TARGETS];
NJS_TEXLIST lock_on_missile_texlist;
NJS_TEXLIST burst_texlist;
NJS_TEXLIST rocket_thrust_texlist;

static struct task collider, weak_point;
static struct task_work collider_work;
static struct collision_work collision;
static struct collision_shape shapes[2];
static Bool fail_child, fail_collision;
static int live_blocks, covers, landings, depth;

void *syCalloc(Uint32 count, Uint32 size)
{
    void *block = calloc(count, size);
    assert(block != NULL);
    live_blocks++;
    return block;
}

void syFree(void *block)
{
    assert(block != NULL && live_blocks > 0);
    live_blocks--;
    free(block);
}

struct task *task_create_child(Uint16 flags, void (*step)(struct task *),
                               struct task *parent)
{
    assert(flags == TASK_WORK && parent->child == NULL);
    if (fail_child) {
        return NULL;
    }
    memset(&collider, 0, sizeof(collider));
    memset(&collider_work, 0, sizeof(collider_work));
    parent->child = &collider;
    collider.parent = parent;
    collider.step = step;
    collider.work = &collider_work;
    return &collider;
}

void task_destroy_children(struct task *parent)
{
    assert(parent->child == &collider);
    collider.destructor(&collider);
    parent->child = NULL;
}

Bool collision_init(struct task *task, const struct collision_shape *source,
                    int count, Uint8 kind)
{
    assert(count == 1 || count == 2);
    if (fail_collision) {
        return FALSE;
    }
    memcpy(shapes, source, count * sizeof(shapes[0]));
    collision.shapes = shapes;
    collision.count = count;
    task->work->collision = &collision;
    return TRUE;
}

Bool collision_register(struct task *task)
{
    assert(task->work->collision != NULL);
    return TRUE;
}

struct task *weak_point_create(struct task *parent, int index, NJS_POINT3 *pos)
{
    assert(parent == &collider && parent->child == NULL);
    parent->child = &weak_point;
    return &weak_point;
}

void debris_launch_flags(NJS_POINT3 *pos, struct angles *angles, Float floor,
                         NJS_VECTOR *speed, NJS_CNK_MODEL *model,
                         NJS_TEXLIST *textures, Uint32 flags)
{
    assert(model && textures == &lock_on_missile_texlist);
    covers++;
}

struct particle *burst_create_wobbling(NJS_POINT3 *pos, NJS_VECTOR *speed,
                                       Float scale)
{
    assert(scale == 15.0F && pos->x == 12.0F && pos->y == 18.0F);
    assert(speed->x > 0.35F && speed->x < 0.37F);
    assert(speed->y == 0.0F && speed->z == 0.0F);
    landings++;
    return NULL;
}

Bool njPushMatrix(NJS_MATRIX *matrix)
{
    depth++;
    return TRUE;
}
Bool njPushMatrixEx(void)
{
    return njPushMatrix(NULL);
}
Bool njPopMatrix(Uint32 count)
{
    assert(depth >= (int)count);
    depth -= (int)count;
    return TRUE;
}
Bool njPopMatrixEx(void)
{
    return njPopMatrix(1);
}
Float njScalor(NJS_VECTOR *v)
{
    return (Float)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}
Float njSqrt(Float value)
{
    return (Float)sqrt(value);
}
Float njSin(Angle angle)
{
    return 0.0F;
}
void njCalcPoint(NJS_MATRIX *m, NJS_POINT3 *point, NJS_POINT3 *dest)
{
    *dest = *point;
}
void njCalcVector(NJS_MATRIX *m, NJS_VECTOR *vector, NJS_VECTOR *dest)
{
    *dest = *vector;
}
void njUnitMatrix(NJS_MATRIX *m)
{
}
void njTranslateV(NJS_MATRIX *m, NJS_VECTOR *v)
{
}
void njTranslateEx(NJS_VECTOR *v)
{
}
void njRotateX(NJS_MATRIX *m, Angle angle)
{
}
void njRotateY(NJS_MATRIX *m, Angle angle)
{
}
void njRotateZ(NJS_MATRIX *m, Angle angle)
{
}
Sint32 njSetTexture(NJS_TEXLIST *textures)
{
    return 0;
}
void njCnkEasyDrawModel(NJS_CNK_MODEL *model)
{
}
void model_draw_tinted(NJS_CNK_MODEL *model, Uint32 color)
{
}
void draw_control_set(Uint32 bits)
{
}
void draw_control_clear(Uint32 bits)
{
}
Uint32 blink_light_gray(int period, int rest, int fade)
{
    return 0xffffffffUL;
}
Sint8 placement_get_state(struct task *task)
{
    return 0;
}
void placement_set_state(struct task *task, Sint8 state)
{
}
Bool placement_try_leave(struct task *task)
{
    return FALSE;
}
int switch_is_on(Uint32 id)
{
    return 0;
}
Sint8 sound_play_at(int id, const void *key, Sint8 priority, Sint16 volume,
                    const NJS_POINT3 *pos)
{
    return 0;
}
Sint8 sound_play_at_for(int id, const void *key, Sint8 priority, Sint16 volume,
                        Sint16 duration, const NJS_POINT3 *pos)
{
    return 0;
}
struct particle *rocket_thrust_create(NJS_POINT3 *pos, NJS_VECTOR *speed,
                                      Float size)
{
    return NULL;
}
struct particle *rocket_thrust_settle_create(NJS_POINT3 *pos, NJS_VECTOR *speed,
                                             Float size, Float floor)
{
    return NULL;
}

static void run_flight(Bool destroy_on_landing)
{
    struct task launcher = {0};
    struct task_work work = {0};
    NJS_POINT3 *hit = NULL;
    NJS_POINT3 foreign;
    NJS_POINT3 *other = &foreign;
    void (*start_collision)(struct task *);
    int ticks, i, previous_landings = landings, previous_covers = covers;

    for (i = 0; i < MISSILE_TARGETS; i++) {
        missile_targets[i].id = -1;
        missile_targets[i].hit = NULL;
    }
    missile_targets[0].id = 3;
    missile_targets[0].hit = &hit;
    missile_targets[1].id = 9;
    missile_targets[1].hit = &other;
    launcher.work = &work;
    launcher.step = lock_on_missile_init;
    work.ang.z = 3;
    work.scl.x = 12.0F;
    work.scl.y = 18.0F;

    fail_child = TRUE;
    launcher.step(&launcher);
    assert(launcher.step == lock_on_missile_init && !launcher.motion.p);
    assert(!launcher.draw && !launcher.destructor && !live_blocks);
    fail_child = FALSE;
    launcher.step(&launcher);
    assert(launcher.child == &collider && launcher.motion.p &&
           live_blocks == 1);

    start_collision = collider.step;
    fail_collision = TRUE;
    collider.step(&collider);
    assert(collider.step == start_collision && !collider_work.collision);
    fail_collision = FALSE;
    collider.step(&collider);
    assert(collider.step != start_collision && collider_work.collision);

    for (ticks = 0; !hit && ticks < 400; ticks++) {
        if (collider.child == &weak_point) {
            collider_work.submode = 1;
        }
        launcher.step(&launcher);
        collider.step(&collider);
        assert(depth == 0);
    }
    assert(hit && hit->x == work.scl.x && hit->y == work.scl.y);
    assert(landings == previous_landings + 1 && covers == previous_covers + 1);
    if (!destroy_on_landing) {
        launcher.step(&launcher);
        assert(hit == NULL);
    }
    launcher.destructor(&launcher);
    assert(hit == NULL && other == &foreign);
    assert(!launcher.motion.p && !launcher.child && !live_blocks);
}

int main(void)
{
    run_flight(FALSE);
    run_flight(TRUE);
    return 0;
}
