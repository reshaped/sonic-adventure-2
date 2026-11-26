/* the board trigger, BCOL in Metal Harbor's object list. a player takes a
 * board there, or leaves one */
#include <ninja.h>

#include "object/set.h"
#include "player/player.h"
#include "stg10/board_trigger.h"
#include "stg10/harbor_missile.h"
#include "system/task.h"

/* the trigger's modes, in its work's mode */
enum board_trigger_mode { MODE_ARMED, MODE_FIRED };

/* the bit of ang.x that has the trigger board its player. without it, the
 * trigger releases the player */
#define BOARD_ON 0x1
/* how far past scl.x the trigger reaches */
#define REACH 10.0F

static void board_trigger_destroy(struct task *task)
{
}

static void board_trigger_step(struct task *task)
{
    struct task_work *work = task->work;
    int player;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for its SET editor
     * captions, which SHC still places in this source's constants */
    if (0) {
        (void)"<- mode[IN]";
        (void)"<- mode[OUT]";
        (void)"<- SCL(+10)";
    }
    /* @hack redundant: a loop that never runs, which stores MODE_FIRED */
    while (0) {
        work->mode = MODE_FIRED;
    }
    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if (work->mode == MODE_ARMED &&
        (player = player_near(&work->pos, work->scl.x + REACH))) {
        if (work->ang.x & BOARD_ON) {
            work->mode = MODE_FIRED;
            player_request(player - 1, PLAYER_REQUEST_BOARD, 0);
            if (task->placement != NULL) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
        } else {
            work->mode = MODE_FIRED;
            player_request(player - 1, PLAYER_REQUEST_RELEASE, 0);
        }
        if (harbor_missile_task != NULL &&
            harbor_missile_task->work->mode != HARBOR_MISSILE_MODE_END) {
            harbor_missile_task->work->mode = HARBOR_MISSILE_MODE_END;
            harbor_missile_task->work->timer = 0;
        }
    }
}

void harbor_board_trigger_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->step = board_trigger_step;
    task->destructor = board_trigger_destroy;
}
