#include "chaostgkinder/rooms.h"
#include "chaostgkinder/services.h"
#include "system/task.h"

static void classroom_destroy(struct task *task)
{
}

static void classroom_draw(struct task *task)
{
    kinder_banner_draw();
}

void kinder_classroom_init(struct task *task)
{
    if (!task->destructor) {
        task->destructor = classroom_destroy;
        task->draw = classroom_draw;
        kinder_banner_select(KINDER_BANNER_CLASSROOM, 0xfff21c03);
        kinder_lesson_create(task);
    }
    if (kinder_lesson_state() >= KINDER_LESSON_ABSENT) {
        kinder_request_room(KINDER_CORRIDOR, CORRIDOR_FROM_CLASSROOM);
    }
}
