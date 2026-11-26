#include "chaostgkinder/rooms.h"
#include "chaostgkinder/services.h"
#include "system/task.h"

#define BANNER_COLOR 0xff00922d

static void office_destroy(struct task *task)
{
}

static void office_draw(struct task *task)
{
    kinder_banner_draw();
}

void kinder_principal_room_init(struct task *task)
{
    if (!task->destructor) {
        task->destructor = office_destroy;
        task->draw = office_draw;
        kinder_banner_select(KINDER_BANNER_PRINCIPAL, BANNER_COLOR);
        kinder_advice_create(task);
    }
    if (kinder_advice_state() >= KINDER_ADVICE_ABSENT) {
        kinder_request_room(KINDER_CORRIDOR, CORRIDOR_FROM_PRINCIPAL);
    }
}
