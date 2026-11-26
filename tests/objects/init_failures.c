/* Run each init of an object that the heap or world_pool_alloc can leave
 * without its collision object, body or collision record, with that record
 * withheld. test_objects.py's ObjectInitFailures empties the objects' steps,
 * draws and mode starts, which no init here reaches. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <ninja.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "stg26/quarters_util.h"
#include "stg26/snake_pot.h"
#include "stg28/awning.h"
#include "stg28/block.h"
#include "stg28/cave_pyramid.h"
#include "stg28/cave_util.h"
#include "stg28/eye.h"
#include "stg28/firepot.h"
#include "stg28/key.h"
#include "stg28/key_door.h"
#include "stg28/keyhole.h"
#include "stg28/pyramid_cave.h"
#include "stg28/sandglass_door.h"
#include "stg28/snake_head.h"
#include "stg28/snake_rail.h"
#include "stg28/snake_statue.h"
#include "stg28/wall.h"
#include "stg28/windmill.h"
#include "stg32/meteor_gate.h"
#include "stg32/meteors.h"
#include "stg32/scenery_shapes.h"
#include "stg44/ore.h"
#include "stg44/scenery_shapes.h"
#include "system/math.h"
#include "system/task.h"

NJS_TEXLIST pyramid_cave_fire_texlist;
NJS_TEXLIST large_meteor_texlist, large_meteor_chip_texlist,
    large_meteor_shard_texlist;
NJS_TEXLIST mad_space_ore_large_texlist, mad_space_ore_large_cracked_texlist,
    mad_space_ore_small_texlist, mad_space_ore_small_cracked_texlist,
    mad_space_ore_first_chip_texlist, mad_space_ore_second_chip_texlist,
    mad_space_ore_third_chip_texlist;
struct task *cave_keyholes[KEY_COLORS];
struct task *cave_key_doors[KEY_COLORS];

/* the task the init under test destroyed, the last child it made, and how
 * many blocks the heap has given and not taken back */
static struct task *destroyed;
static struct task *child;
static int live_blocks;

void *syMalloc(Uint32 size)
{
    void *block = malloc(size);

    assert(block != NULL);
    live_blocks++;
    return block;
}

void *syCalloc(Uint32 count, Uint32 size)
{
    void *block = calloc(count, size);

    assert(block != NULL);
    live_blocks++;
    return block;
}

void syFree(void *block)
{
    if (block != NULL) {
        live_blocks--;
        free(block);
    }
}

/* the pool has no object to lend */
NJS_OBJECT *world_pool_alloc(void)
{
    return NULL;
}

/* the heap has no body to give */
void body_init(struct task *task)
{
    task->force.p = NULL;
}

void body_destroy(struct task *task)
{
    syFree(task->force.p);
    task->force.p = NULL;
}

/* the heap has no collision record to give */
Bool collision_init(struct task *task, const struct collision_shape *shapes,
                    int count, Uint8 kind)
{
    (void)shapes;
    (void)count;
    (void)kind;
    task->work->collision = NULL;
    return FALSE;
}

void task_destroy(struct task *task)
{
    assert(destroyed == NULL);
    destroyed = task;
    if (task->destructor != NULL) {
        task->destructor(task);
    }
}

struct task *task_create_child(Uint16 flags, void (*step)(struct task *task),
                               struct task *parent)
{
    child = calloc(1, sizeof(*child));
    assert(child != NULL);
    (void)flags;
    child->step = step;
    child->parent = parent;
    child->work = calloc(1, sizeof(*child->work));
    assert(child->work != NULL);
    return child;
}

Bool placement_try_leave(struct task *task)
{
    (void)task;
    return FALSE;
}

int player_near(NJS_POINT3 *pos, Float range)
{
    (void)pos;
    (void)range;
    return FALSE;
}

/* nothing past an init's guard runs */
static void unreached(void)
{
    assert(!"an init went on past its guard");
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

void world_pool_free(NJS_OBJECT *object)
{
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

void njRotateY(NJS_MATRIX *m, Angle ang)
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

void current_matrix_angles_xyz(Angle *ax, Angle *ay, Angle *az)
{
    (void)ax;
    (void)ay;
    (void)az;
    unreached();
}

void cave_polar_xz(Angle ang, Float radius, Float *x, Float *z)
{
    (void)ang;
    (void)radius;
    *x = 0.0F;
    *z = 0.0F;
}

void cave_point_turn_y(Float x, Float z, Angle ang, NJS_POINT3 *point)
{
    (void)x;
    (void)z;
    (void)ang;
    (void)point;
    unreached();
}

void dust_smoke_rect(NJS_POINT3 *pos, Float half_x, Float half_z,
                     struct angles *ang, Float scale, Float density,
                     Float speed)
{
    (void)pos;
    (void)half_x;
    (void)half_z;
    (void)ang;
    (void)scale;
    (void)density;
    (void)speed;
    unreached();
}

Uint32 quarters_world_size_class(Float radius)
{
    (void)radius;
    unreached();
    return 0;
}

void collision_fit_radius(struct task *task)
{
    (void)task;
    unreached();
}

void placement_set_state(struct task *task, Sint8 state)
{
    (void)task;
    (void)state;
    unreached();
}

Sint8 placement_get_state(struct task *task)
{
    (void)task;
    unreached();
    return 0;
}

static struct task *make_task(void)
{
    struct task *task = calloc(1, sizeof(*task));

    assert(task != NULL);
    task->work = calloc(1, sizeof(*task->work));
    assert(task->work != NULL);
    return task;
}

/* init leaves no block of its own held, and destroys task where
 * destroys is TRUE */
static void run(void (*init)(struct task *task), Bool destroys)
{
    struct task *task = make_task();
    int held = live_blocks;

    destroyed = NULL;
    init(task);
    assert(destroyed == (destroys ? task : NULL));
    assert(live_blocks == held);
}

int main(void)
{
    void (*inner_init)(struct task *task);

    run(cave_awning_init, TRUE);
    run(cave_block_init, TRUE);
    run(cave_eye_init, TRUE);
    run(cave_firepot_init, TRUE);
    run(cave_key_door_init, TRUE);
    assert(cave_key_doors[0] == NULL);
    run(cave_keyhole_init, TRUE);
    assert(cave_keyholes[0] == NULL);
    run(cave_sandglass_door_init, TRUE);
    run(cave_snake_head_init, TRUE);
    run(cave_snake_rail_init, TRUE);
    run(cave_snake_statue_init, TRUE);
    run(cave_wall_init, TRUE);
    run(cave_windmill_init, TRUE);
    run(cave_pyramid_init, TRUE);
    run(quarters_snake_pot_init, TRUE);
    inner_init = child->step;
    run(large_meteor_init, TRUE);
    /* ore_destroy, which would save the damage, doesn't run */
    run(mad_space_ore_init, TRUE);
    run(meteor_fireball_init, TRUE);
    assert(fireball_count == 0);

    /* the pot's inner part, a child, takes its init as its step again */
    child = task_create_child(TASK_WORK, inner_init, make_task());
    destroyed = NULL;
    child->step(child);
    assert(destroyed == NULL);
    assert(child->step == inner_init);
    return 0;
}
