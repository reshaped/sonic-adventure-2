/* the world ranking's work, and its task, which does nothing. the work has
 * no textures */
#include <sg_xpt.h>

#include "advertise/menus.h"
#include "advertise/world_ranking.h"
#include "system/task.h"

#define TASK_LEVEL 2

/* nothing calls it. it isn't static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* destroy the task, where there is one */
void world_ranking_end(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* world_ranking_create copies the original initial value here. */
static Sint32 reset_word;

static Sint32 initial_reset_word = 0;
/* the task world_ranking_create made, or null */
static struct task *ranking_task = NULL;

static void world_ranking_step(struct task *task)
{
}

static void world_ranking_draw(struct task *task)
{
}

static void world_ranking_destroy(struct task *task)
{
    world_ranking_release();
}

void world_ranking_create(void)
{
    struct task *task;
    Uint8 *byte;
    int left;

    task = task_create(TASK_WORK, TASK_LEVEL, world_ranking_step,
                       "worldRankingExec");
    task->draw = world_ranking_draw;
    task->destructor = world_ranking_destroy;
    ranking_task = task;
    byte = (Uint8 *)&world_ranking_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(world_ranking_work); left != 0; left--) {
        *byte++ = 0;
    }
    reset_word = initial_reset_word;
}

void world_ranking_end(void)
{
    if (ranking_task) {
        task_destroy(ranking_task);
        ranking_task = NULL;
    }
}

void world_ranking_load(void)
{
    /* @hack redundant: the test of loaded before it is set, where the store
     * alone would do */
    if (!world_ranking_work.loaded) {
        world_ranking_work.loaded = TRUE;
    }
}

void world_ranking_release(void)
{
    /* @hack redundant: the test of loaded before it is cleared, where the
     * store alone would do */
    if (world_ranking_work.loaded) {
        world_ranking_work.loaded = FALSE;
    }
}
