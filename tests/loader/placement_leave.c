/* Ask a placed task to leave from within its range of the player, from
 * beyond it, and from a place that holds a NaN. */
#include <assert.h>
#include <math.h>

#include "object/set.c"

struct game_state game_state;
struct task_work *player_work[PLAYER_SLOTS];
struct task_work *camera_task_work;

void task_destroy(struct task *task)
{
}

/* whether task, at pos, leaves from a player at the origin. the range is 20,
 * squared */
static Bool leaves(Float x, Float y, Float z)
{
    static struct task_work player;
    struct task_work work = {0};
    struct task task = {0};

    player_work[0] = &player;
    task.work = &work;
    work.pos.x = x;
    work.pos.y = y;
    work.pos.z = z;
    if (placement_try_leave_by(&task, 400.0F)) {
        assert(task.step == task_destroy);
        return TRUE;
    }
    assert(task.step == NULL);
    return FALSE;
}

int main(void)
{
    Float origin[3] = {0.0F, 0.0F, 0.0F};

    assert(!leaves(12.0F, 12.0F, 10.0F));
    assert(leaves(21.0F, 0.0F, 0.0F));
    assert(leaves(0.0F, 21.0F, 0.0F));
    assert(leaves(12.0F, 12.0F, 11.0F));
    /* an enemy pulled in at its home by a range that isn't above 0 takes
     * NaNs in pos.x and pos.z, as enemy.h says. the image's tests pass a
     * NaN, so the enemy stays, whatever its y */
    assert(!leaves(NAN, 5.0F, NAN));
    assert(!leaves(NAN, 500.0F, NAN));
    assert(!leaves(NAN, 0.0F, 0.0F));
    assert(!leaves(0.0F, NAN, 0.0F));
    assert(!leaves(0.0F, 0.0F, NAN));
    /* the pass of two players measures by the same tests */
    assert(point_near_either(origin, NAN, 5.0F, NAN, 400.0F, NULL));
    assert(point_near_either(origin, 12.0F, 12.0F, 10.0F, 400.0F, NULL));
    assert(!point_near_either(origin, 12.0F, 12.0F, 11.0F, 400.0F, NULL));
    return 0;
}
