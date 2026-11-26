#include "chaostgkinder/rooms.h"
#include "chaostgkinder/services.h"
#include "system/task.h"

#define BANNER_COLOR 0xff3252c4

static void health_center_destroy(struct task *task)
{
    task_destroy_children(task);
}

static void health_center_draw(struct task *task)
{
    kinder_banner_draw();
}

void kinder_health_center_init(struct task *task)
{
    if (!task->destructor) {
        task->destructor = health_center_destroy;
        task->draw = health_center_draw;
        kinder_banner_select(KINDER_BANNER_HEALTH_CENTER, BANNER_COLOR);
        kinder_checkup_create(task);
    }
    if (kinder_checkup_state() >= KINDER_CHECKUP_ABSENT) {
        kinder_request_room(KINDER_CORRIDOR, CORRIDOR_FROM_HEALTH_CENTER);
    }
}
