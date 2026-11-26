/* the retry banner */
#include "chaostgrace/banners.h"
#include "system/task.h"

#define RESULT_FRAMES 300
#define RESULT_Y 220.0F
#define RESULT_ASPECT 0.8F
#define RESULT_HEIGHT(scale) ((scale) * RESULT_ASPECT)
#define RESULT_CELL BANNER_RETRY
#include "chaostgrace/result_impl.h"

void race_retry_banner_create(void)
{
    struct task *task = task_create(TASK_WORK, 2, result_banner_step,
                                    "ALR_ADV_TryAgainManagerExecutor");

    task->draw = result_banner_draw;
    task->destructor = result_banner_destroy;
}
