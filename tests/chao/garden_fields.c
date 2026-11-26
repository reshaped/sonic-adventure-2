/* Run the sphere constructors, their task rings and the placed fields with
 * the game's allocator callers, collision records and contact scans. */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <shinobi.h>
#include "camera/camera.h"
#include "chao/garden.h"
#include "chao/garden_field.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "collision/collision.h"
#include "draw/lines.h"
#include "object/set.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/timer.h"
#include "system/util.h"

struct chao_tuning chao_tuning;
NJS_CNK_OBJECT debug_sphere_object;
static int allocations, fail_at, blocks, destination;
static NJS_POINT3 point = {2.5F, 3.0F, -6.0F};

void *syMalloc(Uint32 size)
{
    void *p;
    if (++allocations == fail_at) {
        return NULL;
    }
    p = malloc(size);
    assert(p);
    memset(p, 0xa5, size);
    blocks++;
    return p;
}
void *syCalloc(Uint32 count, Uint32 size)
{
    void *p = syMalloc(count * size);
    if (p) {
        memset(p, 0, count * size);
    }
    return p;
}
void syFree(void *p)
{
    assert(blocks > 0 && p);
    blocks--;
    free(p);
}
void njMemCopy(void *to, const void *from, Sint32 size)
{
    memcpy(to, from, size);
}
Float njScalor(NJS_VECTOR *v)
{
    return (Float)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}
Float njSqrt(Float v)
{
    return (Float)sqrt(v);
}
void sound_detach_task(const struct task *task)
{
}
void profile_step_start(void (*step)(struct task *), const char *name)
{
}
void profile_step_end(void (*step)(struct task *))
{
}
Bool placement_try_leave(struct task *task)
{
    return FALSE;
}
void chao_change_area(int area)
{
    destination = area;
}
void draw_control_save(void)
{
}
void draw_control_restore(void)
{
}
void draw_control_set(Uint32 bits)
{
}
void draw_control_clear(Uint32 bits)
{
}
void constant_material_set(Float a, Float r, Float g, Float b)
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
void njTranslateEx(NJS_VECTOR *p)
{
}
void njScale(NJS_MATRIX *m, Float x, Float y, Float z)
{
}
void njCnkSimpleDrawObject(NJS_CNK_OBJECT *object)
{
}

static void verify_sphere(struct task *field, int kind, int list)
{
    struct collision_work *coll = field->work->collision;
    struct collision_shape *shape = collision_get_shape(field, 0);
    assert(field->step && field->draw && field->destructor);
    assert(coll->owner == field && coll->count == 1 && coll->kind == list);
    assert(shape->form == SHAPE_SPHERE && (Uint8)shape->id == kind);
    assert(shape->radius == 3.5F && field->work->scl.x == 3.5F);
    assert(!memcmp(&field->work->pos, &point, sizeof(point)));
}

static void standalone(void)
{
    struct task *(*create[])(int, NJS_POINT3 *, Float, Angle, int) = {
        garden_oriented_object_field, garden_oriented_enemy_field};
    struct task *field;
    int i, n;
    for (i = 0; i < 2; i++) {
        for (n = 1; n <= 4; n++) {
            fail_at = allocations + n;
            assert(!create[i](170, &point, 3.5F, 0x1234, 2));
            assert(allocations == fail_at && !task_levels[2] && !blocks);
            fail_at = 0;
            field = create[i](170, &point, 3.5F, 0x1234, 2);
            assert(field == task_levels[2] && blocks == 4);
            verify_sphere(field, 170, i ? COLLISION_ENEMY : COLLISION_OBJECT);
            assert(field->work->ang.x == 0x1234);
            collision_clear();
            task_step_level(2);
            assert(field->work->timer == 1);
            assert((i ? collision_enemy_count : collision_object_count) == 1);
            collision_clear();
            task_step_level(2);
            assert(field->work->timer == 0);
            collision_clear();
            task_step_level(2);
            assert(!task_levels[2] && !blocks && !collision_enemy_count &&
                   !collision_object_count);
        }
    }
    field = garden_object_field(161, &point, 3.5F);
    assert(field && !field->work->submode);
    task_destroy(field);
    field = garden_enemy_field(200, &point, 3.5F);
    assert(field && !field->work->submode);
    task_destroy(field);
}

static void child_fields(void)
{
    struct task *parent = task_create(0, 3, NULL, "racer");
    struct chao_work *work = syCalloc(1, sizeof(*work));
    struct task *first, *replacement;
    int i, before;
    parent->work = &work->base;
    first = garden_create_child_field(parent, 175, &point, 3.5F);
    assert(first && first->parent == parent && work->field == first);
    verify_sphere(first, 175, COLLISION_ENEMY);
    before = blocks;
    for (i = 1; i <= 4; i++) {
        fail_at = allocations + i;
        assert(!garden_create_timed_field(parent, 175, &point, 3.5F, 2));
        assert(allocations == fail_at && blocks == before);
        assert(parent->child == first && first->next == first &&
               work->field == first);
    }
    fail_at = 0;
    garden_field_set_lifetime(first, 1);
    replacement = garden_create_timed_field(parent, 175, &point, 3.5F, 2);
    assert(replacement && replacement != first && work->field == replacement);
    verify_sphere(replacement, 175, COLLISION_ENEMY);
    collision_clear();
    task_step_level(3);
    assert(collision_enemy_count == 2);
    collision_clear();
    task_step_level(3);
    assert(parent->child == replacement && work->field == replacement &&
           collision_enemy_count == 1);
    collision_clear();
    task_step_level(3);
    assert(!parent->child && !work->field && !collision_enemy_count);
    assert(garden_create_child_field(parent, 175, &point, 3.5F));
    task_destroy(parent);
    assert(!task_levels[3] && !blocks);
}

static void placed_fields(void)
{
    static void (*const steps[])(struct task *) = {
        garden_race_field, garden_cube_field, garden_climb_field,
        garden_lobby_trigger, garden_entrance_trigger};
    struct task *field;
    struct task_work *work;
    struct collision_shape *shape;
    struct task player = {0};
    struct task_work pw = {0};
    struct collision_work pc = {0};
    int i, n;
    player.work = &pw;
    pw.collision = &pc;
    pc.kind = COLLISION_PLAYER;
    for (i = 0; i < 5; i++) {
        for (n = 1; n <= 2; n++) {
            field = task_create(TASK_WORK, 2, steps[i], "field");
            work = field->work;
            work->scl = point;
            work->ang.x = 17;
            work->ang.y = 42;
            work->ang.z = 15;
            collision_clear();
            fail_at = allocations + n;
            task_step_level(2);
            assert(allocations == fail_at && blocks == 2 && !work->mode &&
                   !work->collision);
            assert(!collision_enemy_count && !collision_object_count);
            fail_at = 0;
            task_step_level(2);
            assert(work->mode == 1 && blocks == 4);
            shape = collision_get_shape(field, 0);
            assert(shape->radius == 12.5F && shape->size_y == 13.0F &&
                   shape->size_z == 4.0F);
            assert(shape->form == (i == 1 ? SHAPE_BOX2 : SHAPE_BOX));
            if (i == 0) {
                assert((Uint8)shape->id == 179);
            }
            if (i == 1) {
                assert(shape->ang.x == 17 && shape->ang.y == 42 &&
                       shape->ang.z == 15);
            }
            if (i == 2) {
                assert((Uint8)shape->id == 203);
            }
            task_step_level(2);
            assert((i == 0 ? collision_enemy_count : collision_object_count) ==
                   1);
            if (i >= 3) {
                collision_clear();
                work->collision->hits[0].own_shape = 0;
                work->collision->hits[0].hit_shape = 0;
                work->collision->hits[0].task = &player;
                destination = -1;
                task_step_level(2);
                assert(destination ==
                       (i == 3 ? CHAO_AREA_LOBBY : CHAO_AREA_ENTRANCE));
                assert(work->mode == 2 && !collision_object_count);
            }
            task_destroy(field);
            assert(!task_levels[2] && !blocks);
        }
    }
}

int main(void)
{
    task_init();
    standalone();
    child_fields();
    placed_fields();
    return 0;
}
