#include "chaostgkinder/rooms.h"
#include "chaostgkinder/services.h"
#include "system/task.h"

enum account_room_mode { OPEN_MENU, WAIT_FOR_MENU };

void kinder_account_init(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case OPEN_MENU:
        kinder_account_menu_create(task);
        work->mode = WAIT_FOR_MENU;
        break;
    case WAIT_FOR_MENU:
        switch (kinder_account_menu_result()) {
        /* @hack spelling: a case for each result, each with its own call,
         * where one case for both would do */
        case MENU_SAVED:
            kinder_request_room(KINDER_BLACK_MARKET, 0);
            break;
        case MENU_CLOSED:
            kinder_request_room(KINDER_BLACK_MARKET, 0);
            break;
        }
        break;
    }
}
