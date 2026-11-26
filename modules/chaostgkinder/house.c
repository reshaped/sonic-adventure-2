#include "chaostgkinder/labels.h"
#include "chaostgkinder/objects.h"
#include "chaostgkinder/rooms.h"
#include "player/player.h"
#include "system/pad.h"
#include "system/task.h"

#define HOUSE_DOOR 0
#define DOOR_APPROACH_Z 50.0F
#define DOORWAY_Z 0.0F
#define LOBBY_EXIT_Z 200.0F
#define WALK_DELAY 5
/* the player's speed along z: in, toward the corridor, and out, toward the
 * lobby */
#define INWARD_SPEED (-1.0F)
#define OUTWARD_SPEED 1.0F
#define EXIT_LABEL_X 100
#define EXIT_LABEL_Y 100
/* the choice label's caption, past the last of the KINDER_CAPTIONS */
#define CHOICE_CAPTION 19
#define CHOICE_LABEL_X 400
#define CHOICE_LABEL_Y 240

enum house_mode { HOUSE_START, HOUSE_RUN };

/* kinder_house_init never enters HOUSE_WAIT. no phase from it through
 * HOUSE_WAIT_FOR_CHOICE runs, and nor do HOUSE_RETURN_TO_LOBBY and
 * HOUSE_REENTER_CORRIDOR, which only HOUSE_WAIT_FOR_CHOICE enters */
enum house_phase {
    HOUSE_WALK_IN = 0,
    HOUSE_WAIT = 1,
    HOUSE_WAIT_FOR_TEXT = 3,
    HOUSE_SHOW_TEXT = 5,
    HOUSE_WAIT_FOR_CHOICE = 6,
    HOUSE_ENTER_CORRIDOR = 8,
    HOUSE_RETURN_TO_LOBBY = 9,
    HOUSE_REENTER_CORRIDOR = 10,
    HOUSE_DONE = 12
};

enum house_choice { CHOICE_ENTER, CHOICE_LEAVE, CHOICE_REENTER };

static struct task *exit_label = NULL;
static struct task *choice_label = NULL;

static void house_destroy(struct task *task)
{
}

static void house_draw(struct task *task)
{
}

/* @hack spelling: a macro, where an inline function would do */
#define START_WALKING(player, work)                                            \
    do {                                                                       \
        if ((work)->tick <= WALK_DELAY) {                                      \
            if ((work)->tick == WALK_DELAY) {                                  \
                if ((player)->flags & PLAYER_WORK_HOLDING) {                   \
                    (player)->mode = PLAYER_MODE_UNK_19;                       \
                } else {                                                       \
                    (player)->mode = PLAYER_MODE_RUN;                          \
                }                                                              \
            }                                                                  \
            (work)->tick++;                                                    \
        }                                                                      \
    } while (0)

void kinder_house_init(struct task *task)
{
    /* @hack redundant: first, which only hands player_work[0] to player */
    struct task_work *first = player_work[0];
    struct task_work *player = first;
    struct task_work *work = task->work;

    /* @hack spelling: a switch of one case, where an if would do */
    switch (work->mode) {
    case HOUSE_START:
        task->draw = house_draw;
        task->destructor = house_destroy;
        pad_disable_player(0);
        work->mode++;
        break;
    }
    switch (kinder_current_entrance()) {
    /* @hack redundant: case HOUSE_FROM_LOBBY, which default already takes */
    case HOUSE_FROM_LOBBY:
    default:
        switch (work->submode) {
        case HOUSE_WALK_IN:
            START_WALKING(player, work);
            player_set_speed(0, 0.0F, 0.0F, INWARD_SPEED);
            if (player_work[0]->pos.z < DOOR_APPROACH_Z) {
                /* @hack spelling: the local door, where passing
                 * kinder_door(HOUSE_DOOR) to kinder_door_open would do */
                struct task *door = kinder_door(HOUSE_DOOR);
                kinder_door_open(door);
                work->submode = HOUSE_ENTER_CORRIDOR;
            }
            break;
        case HOUSE_WAIT:
            work->submode = HOUSE_WAIT_FOR_TEXT;
            break;
        case HOUSE_WAIT_FOR_TEXT:
            work->submode = HOUSE_SHOW_TEXT;
            break;
        case HOUSE_SHOW_TEXT:
            /* @hack redundant: the if around exit_label's assignment, whose
             * body is empty */
            if ((exit_label =
                     kinder_label_create(task, KINDER_CAPTION_EXIT,
                                         EXIT_LABEL_X, EXIT_LABEL_Y)) != NULL) {
            }
            /* @bug memory: kinder_label_create would read caption 19, past
             * the end of a language's eight. this call asks for it at
             * 0x0c9c0a54, in a phase that never runs, and the read is at
             * 0x0c9c781a */
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((choice_label =
                     kinder_label_create(task, CHOICE_CAPTION, CHOICE_LABEL_X,
                                         CHOICE_LABEL_Y)) != NULL) {
                struct kinder_label *label = kinder_label_data(choice_label);
                label->choice = CHOICE_LEAVE;
            }
            work->submode = HOUSE_WAIT_FOR_CHOICE;
            break;
        case HOUSE_WAIT_FOR_CHOICE: {
            struct kinder_label *label = kinder_label_data(choice_label);
            int flags = label->flags;
            if (flags & KINDER_LABEL_ACCEPT) {
                task_kill(choice_label);
                switch (label->choice) {
                case CHOICE_ENTER:
                    kinder_door_open(kinder_door(HOUSE_DOOR));
                    work->submode = HOUSE_ENTER_CORRIDOR;
                    break;
                /* @hack redundant: case CHOICE_LEAVE, which default already
                 * takes */
                case CHOICE_LEAVE:
                default:
                    work->submode = HOUSE_RETURN_TO_LOBBY;
                    break;
                case CHOICE_REENTER:
                    kinder_door_open(kinder_door(HOUSE_DOOR));
                    work->submode = HOUSE_REENTER_CORRIDOR;
                    break;
                }
            } else if (flags & KINDER_LABEL_CANCEL) {
                work->submode = HOUSE_RETURN_TO_LOBBY;
            }
            break;
        }
        case HOUSE_ENTER_CORRIDOR:
            player_set_speed(0, 0.0F, 0.0F, INWARD_SPEED);
            if (player_work[0]->pos.z < DOORWAY_Z) {
                kinder_door_close(kinder_door(HOUSE_DOOR));
                kinder_request_room(KINDER_CORRIDOR, CORRIDOR_FROM_HOUSE);
                pad_enable_player(0);
                work->submode = HOUSE_DONE;
            }
            break;
        case HOUSE_RETURN_TO_LOBBY:
            player_set_speed(0, 0.0F, 0.0F, OUTWARD_SPEED);
            if (player_work[0]->pos.z > LOBBY_EXIT_Z) {
                kinder_request_lobby();
                work->submode = HOUSE_DONE;
            }
            break;
        case HOUSE_REENTER_CORRIDOR:
            player_set_speed(0, 0.0F, 0.0F, INWARD_SPEED);
            if (player_work[0]->pos.z < DOORWAY_Z) {
                kinder_door_close(kinder_door(HOUSE_DOOR));
                kinder_request_room(KINDER_CORRIDOR, CORRIDOR_FROM_HOUSE);
                work->submode = HOUSE_DONE;
            }
            break;
        case HOUSE_DONE:
            break;
        }
        break;
    case HOUSE_FROM_CORRIDOR:
        START_WALKING(player, work);
        player_set_speed(0, 0.0F, 0.0F, OUTWARD_SPEED);
        if (player_work[0]->pos.z > LOBBY_EXIT_Z) {
            pad_enable_player(0);
            kinder_request_lobby();
        }
        break;
    }
}
