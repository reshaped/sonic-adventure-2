/* Exercise the live fleet with an allocation failure partway through a spawn.
 */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "cart/objects.h"

/* Keep rendering outside this scheduler and allocation test. */
#define cart_out_of_view(pos) TRUE
#include "cart/traffic.c"
#undef cart_out_of_view

struct cart_setup cart_setup;
struct game_state game_state;
Sint32 cart_traffic_paused;
NJS_TEXLIST cart_story_textures;

/* The lane declarations in traffic_lanes.h supply the real capacities. */
NJS_POINT3 cart_tails_lane_0[], cart_tails_lane_1[];
NJS_POINT3 cart_rouge_lane_0[], cart_rouge_lane_1[];
NJS_POINT3 cart_tails_hd_lane_0[], cart_tails_hd_lane_1[];
NJS_POINT3 cart_rouge_hd_lane_0[], cart_rouge_hd_lane_1[];
NJS_CNK_OBJECT cart_traffic_model_0, cart_traffic_model_1;
NJS_CNK_OBJECT cart_traffic_model_2, cart_traffic_model_3;
NJS_CNK_OBJECT cart_traffic_model_4, cart_traffic_model_5;

/* Task, work, car, collision record and privately copied shape. */
enum { CAR_ALLOCATIONS = 5, FAILED_CAR = 2 };
static int allocations, fail_at, live_blocks;

void *syMalloc(Uint32 size)
{
    void *block;
    if (++allocations == fail_at) {
        return NULL;
    }
    block = malloc(size);
    assert(block != NULL);
    live_blocks++;
    return block;
}

void *syCalloc(Uint32 count, Uint32 size)
{
    void *block = syMalloc(count * size);
    if (block != NULL) {
        memset(block, 0, count * size);
    }
    return block;
}

void syFree(void *block)
{
    assert(block != NULL && live_blocks > 0);
    live_blocks--;
    free(block);
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
void njMemCopy(void *dest, const void *source, Sint32 size)
{
    memcpy(dest, source, (size_t)size);
}
Float njSqrt(Float value)
{
    return (Float)sqrt(value);
}
Float njScalor(NJS_VECTOR *v)
{
    return njSqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}
void vector_sum(const NJS_VECTOR *a, const NJS_VECTOR *b, NJS_VECTOR *out)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
void vector_diff(const NJS_VECTOR *a, const NJS_VECTOR *b, NJS_VECTOR *out)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
Bool cart_is_racer(struct task *task)
{
    return FALSE;
}
void cart_hit_traffic(struct task *task, NJS_POINT3 *pos)
{
    abort();
}
void placement_kill(struct task *task)
{
    abort();
}

static void drive(int mission, int mode, int failure)
{
    struct task *task;
    struct traffic_car *car;
    int count = mission == MISSION_HD ? 60 : 30;
    int lane, i, survivors;

    cart_setup.course_mode = mode;
    game_state.mission = mission;
    allocations = 0;
    fail_at = failure ? FAILED_CAR * CAR_ALLOCATIONS + failure : 0;
    collision_enemy_count = 0;
    cart_start_traffic();
    survivors = count - (failure != 0);
    assert(live_blocks == survivors * CAR_ALLOCATIONS);
    task = task_levels[2];
    for (i = 0; i < count; i++) {
        if (failure && i == FAILED_CAR) {
            continue;
        }
        assert(task != NULL && task->step == traffic_step &&
               task->draw == traffic_draw &&
               task->destructor == traffic_destroy);
        assert(task->work->collision != NULL &&
               task->work->collision->owner == task);
        car = task->motion.p;
        lane = (i % 2) + 2 * mode + (mission == MISSION_HD ? 4 : 0);
        assert(car->start.path == paths[lane] &&
               car->start.count == path_lengths[lane] &&
               car->start.model == i % 6);
        assert(car->point == car->start.first);
        task = task->next;
    }
    assert(task == task_levels[2]);
    task_step_level(2);
    assert(collision_enemy_count == survivors);
    cart_stop_traffic();
    task_step_level(2);
    assert(task_levels[2] == NULL && live_blocks == 0);
}

int main(void)
{
    int failure;
    task_init();
    for (failure = 1; failure <= CAR_ALLOCATIONS; failure++) {
        drive(0, 0, failure);
        drive(0, 0, 0);
    }
    drive(MISSION_HD, 1, 0);
    return 0;
}
