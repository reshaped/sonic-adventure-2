/* the win banner */
#include "chaostgrace/banners.h"
#include "system/task.h"

#define RESULT_FRAMES 360
#define RESULT_Y 200.0F
#define RESULT_ASPECT 1.0F
/* @hack spelling: RESULT_HEIGHT the scale alone, where the scale times
 * RESULT_ASPECT would do */
#define RESULT_HEIGHT(scale) (scale)
#define RESULT_CELL BANNER_WIN_LEFT
#define RESULT_SECOND_CELL BANNER_WIN_RIGHT
#include "chaostgrace/result_impl.h"

void race_win_banner_create(void)
{
    struct task *task = task_create(TASK_WORK, 2, result_banner_step,
                                    "ALR_ADV_CongraManagerExecutor");

    task->draw = result_banner_draw;
    task->destructor = result_banner_destroy;
}
