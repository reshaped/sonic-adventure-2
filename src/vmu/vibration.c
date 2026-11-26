/* vibration: shaking a pad through the pack in one of its sockets */
#include <sg_pdvib.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "stage/stage.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* the port found where no socket of the pad holds a ready pack */
#define NO_PORT 0xffffffff

/* the expansion sockets in a pad */
#define SOCKETS 2

/* the bounds vibration.h gives power, freq and time */
#define POWER_MAX 7
#define FREQ_MIN 15
#define FREQ_MAX 59
#define TIME_MAX 255

/* the PDS_VIBPARAM inc that vibration_start_fading divides by its span */
#define FADE_INC 255

/* how many players vibration_stop stops the pads of, from player 0 */
#define STOPPED_PLAYERS 2

/* the level the task's task_create gives it, and the step, counted from 0,
 * that starts the vibration */
#define TASK_LEVEL 0
#define START_STEP 1

/* the pack's unit that the pdVibMx calls drive */
#define MX_UNIT 1

/* what the task that starts a vibration keeps in its extra */
struct vibration_extra {
    Uint32 port;
    Sint32 steps; /* the task's steps so far */
};

#define EXTRA(task) ((struct vibration_extra *)(task)->extra.p)

/* each pad's sockets, side by side */
static const Uint32 vibration_ports[] = {
    PDD_PORT_A1, PDD_PORT_A2, PDD_PORT_B1, PDD_PORT_B2,
    PDD_PORT_C1, PDD_PORT_C2, PDD_PORT_D1, PDD_PORT_D2,
};

/* on the task's second step, start the vibration and destroy the task.
 * task_destroy frees the parameters in its force */
static void vibration_step(struct task *task)
{
    /* @hack spelling: param, a local read ahead of the tests, where
     * task->force at the call would do */
    PDS_VIBPARAM *param;

    param = task->force.p;
    if (game_state.card_state == CARD_SAVING) {
        task_destroy(task);
        return;
    }
    /* @hack spelling: the extra read through EXTRA at each use, where a local
     * set once would do */
    if (EXTRA(task)->steps++ == START_STEP) {
        pdVibMxStart(EXTRA(task)->port, param);
        task_destroy(task);
    }
}

/* start a vibration of the pack at port, through a task that waits a step.
 * nothing starts when an allocation fails */
static void vibration_queue(Uint32 port, int flag, int power, int freq, int inc)
{
    struct task *task;
    PDS_VIBPARAM *param;

    /* @hack spelling: task assigned inside the test, not apart */
    if ((task = task_create(TASK_EXTRA, TASK_LEVEL, vibration_step,
                            "VibTask")) != NULL) {
        /* @hack spelling: param assigned inside the test, as task is, not
         * apart */
        /* @hack spelling: syMalloc's failure in an else, where an early
         * return would do */
        if ((param = syMalloc(sizeof(*param))) != NULL) {
            task->force.p = param;
            EXTRA(task)->port = port;
            param->unit = MX_UNIT;
            param->flag = flag;
            param->power = power;
            param->freq = freq;
            param->inc = inc;
        } else {
            task_destroy(task);
        }
    }
}

void vibration_start(int player, int power, int freq, int time)
{
    Uint32 port;
    int i;
    /* @hack spelling: socket, which holds the pad's number and then walks
     * vibration_ports beside the counter i, where the pad in a variable of
     * its own and vibration_ports[(pad * SOCKETS) + i] would do */
    int socket;
    int flag;

    if (game_state.demo != 0 || game_state.vibration == 0 ||
        game_state.card_state == CARD_SAVING) {
        return;
    }
    if (game_state.two_players == 0 && player != 0 &&
        stage_current_id() != STAGE_CHAO) {
        return;
    }
    port = NO_PORT;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((socket = player_pad_ports[player]) != NO_PAD) {
        socket *= SOCKETS;
        for (i = 0; i < SOCKETS; socket++, i++) {
            if (pdVibMxIsReady(vibration_ports[socket])) {
                port = vibration_ports[socket];
                break;
            }
        }
    }
    if (port == NO_PORT) {
        return;
    }
    if (power > POWER_MAX) {
        power = POWER_MAX;
    } else if (power < -POWER_MAX) {
        power = -POWER_MAX;
    }
    if (power == 1) {
        power = 2;
    } else if (power == -1) {
        power = -2;
    }
    if (freq > FREQ_MAX) {
        freq = FREQ_MAX;
    } else if (freq < FREQ_MIN) {
        freq = FREQ_MIN;
    }
    if (time > TIME_MAX) {
        time = TIME_MAX;
    }
    if (time < 0) {
        time = 0;
    }
    if (time == 0) {
        flag = 0;
    } else {
        flag = PDD_VIB_FLAG_CONTINUOUS;
        pdVibMxSetStopTime(port, time);
    }
    vibration_queue(port, flag, power, freq, 0);
}

void vibration_start_fading(int player, int power, int freq, int length)
{
    Uint32 port;
    /* @hack spelling: socket, as in vibration_start, where the pad in a
     * variable of its own and vibration_ports[(pad * SOCKETS) + i] would
     * do */
    int socket;
    int i;
    int span;
    int flag;
    /* @hack redundant: eight bytes nothing reads or writes */
    Uint8 unused[8];

    if (game_state.demo != 0 || game_state.vibration == 0 ||
        game_state.card_state == CARD_SAVING) {
        return;
    }
    if (game_state.two_players == 0 && player != 0 &&
        stage_current_id() != STAGE_CHAO) {
        return;
    }
    port = NO_PORT;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((socket = player_pad_ports[player]) != NO_PAD) {
        /* @hack spelling: socket scaled in the for's first clause, where a
         * statement before the for would do, as in vibration_start */
        for (i = 0, socket *= SOCKETS; i < SOCKETS; socket++, i++) {
            if (pdVibMxIsReady(vibration_ports[socket])) {
                port = vibration_ports[socket];
                break;
            }
        }
    }
    if (port == NO_PORT) {
        return;
    }
    if (power > POWER_MAX) {
        power = POWER_MAX;
    } else if (power < -POWER_MAX) {
        power = -POWER_MAX;
    }
    if (power == 1) {
        power = 2;
    } else if (power == -1) {
        power = -2;
    }
    if (freq > FREQ_MAX) {
        freq = FREQ_MAX;
    } else if (freq < FREQ_MIN) {
        freq = FREQ_MIN;
    }
    /* @hack spelling: the flag through a local set ahead of the divisions,
     * not passed as PDD_VIB_FLAG_CONVERGENCE */
    flag = PDD_VIB_FLAG_CONVERGENCE;
    span = freq * length / (power * 4);
    if (span <= 0) {
        span = 1;
    }
    vibration_queue(port, flag, power, freq, FADE_INC / span);
}

void vibration_stop(void)
{
    int player;
    Uint32 port;
    /* @hack spelling: socket, as in vibration_start, where the pad in a
     * variable of its own and vibration_ports[(pad * SOCKETS) + i] would
     * do */
    int socket;
    int i;
    /* @hack redundant: eight bytes nothing reads or writes */
    Uint8 unused[8];

    for (player = 0; player < STOPPED_PLAYERS; player++) {
        port = NO_PORT;
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((socket = player_pad_ports[player]) != NO_PAD) {
            /* @hack spelling: socket scaled in the for's first clause, as in
             * vibration_start_fading, where a statement before the for would
             * do */
            for (i = 0, socket *= SOCKETS; i < SOCKETS; socket++, i++) {
                if (pdVibMxIsReady(vibration_ports[socket])) {
                    port = vibration_ports[socket];
                    break;
                }
            }
        }
        /* @hack spelling: the continue, where if (port != NO_PORT) around
         * the call would do */
        if (port == NO_PORT) {
            continue;
        }
        pdVibMxStop(port);
    }
}
