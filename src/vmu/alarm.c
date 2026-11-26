/* alarms: notes played on the buzzers of the memory cards */
#include <sg_pdtmr.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "system/task.h"
#include "system/util.h"
#include "vmu/ports.h"

/* the index whose next note is a list's first */
#define BEFORE_FIRST (-1)

/* the level the task's task_create gives it */
#define TASK_LEVEL 2

/* a list of notes ends at a note of 0 steps, which the task sends too */
struct alarm_note {
    Uint8 steps; /* the alarm task's steps the note sounds for */
    Uint8 tone;
};

/* the tones of alarm_play's alarms: BEEP for a beep, and SILENCE between
 * beeps and at each list's end */
#define BEEP 0x80
#define SILENCE 0

enum alarm_kind {
    ALARM_BEEP, /* one beep of 3 steps */
    ALARM_FAST_BEEPS, /* seven beeps of 1 step, 1 step apart */
    ALARM_SLOW_BEEPS /* four beeps of 3 steps, 7 steps apart */
};

/* what an alarm's task keeps in its extra */
struct alarm_extra {
    Sint32 port;
    const struct alarm_note *notes;
    /* the index in notes of the note sent last, or BEFORE_FIRST before the
     * first */
    Sint32 index;
    Sint32 timer; /* the steps until the next note is sent */
};

/* nothing calls alarm_play or alarm_stop_all. SHC leaves out a static
 * function that nothing calls */
/* @hack redundant: external linkage, which no call needs, where static would
 * do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* start the alarm of kind on port, as alarm_start does. a kind outside enum
 * alarm_kind does nothing */
void alarm_play(int port, enum alarm_kind kind);
void alarm_stop_all(void);
/* NOLINTEND(misc-use-internal-linkage) */

static int alarm_start(int port, const struct alarm_note *notes);

/* alarm_play's alarms, in the order of its kinds. leaving out const puts
 * them in D, where the image holds them */
/* clang-format off */
static struct alarm_note alarm_beep[] = {
    {3, BEEP}, {0, SILENCE},
};
static struct alarm_note alarm_fast_beeps[] = {
    {1, BEEP}, {1, SILENCE},
    {1, BEEP}, {1, SILENCE},
    {1, BEEP}, {1, SILENCE},
    {1, BEEP}, {1, SILENCE},
    {1, BEEP}, {1, SILENCE},
    {1, BEEP}, {1, SILENCE},
    {1, BEEP}, {0, SILENCE},
};
static struct alarm_note alarm_slow_beeps[] = {
    {3, BEEP}, {7, SILENCE},
    {3, BEEP}, {7, SILENCE},
    {3, BEEP}, {7, SILENCE},
    {3, BEEP}, {0, SILENCE},
};
/* clang-format on */
static struct alarm_note *alarms[] = {
    alarm_beep,
    alarm_fast_beeps,
    alarm_slow_beeps,
};
#define ALARMS ARRAY_COUNT(alarms)

/* the task alarm_start created for each port, or null where it has created
 * none. nothing clears one */
static struct task *alarm_tasks[PORTS];

static void alarm_destroy(struct task *task)
{
    syFree(task->extra.p);
    /* task_destroy frees a non-null extra after this returns */
    task->extra.p = NULL;
}

static void alarm_draw(struct task *task)
{
}

static void alarm_step(struct task *task)
{
    struct alarm_extra *extra;
    const struct alarm_note *note;
    Uint8 data[4];

    extra = task->extra.p;
    /* task_create leaves draw and destructor null, so only the first step
     * sets them */
    if (task->draw == NULL || task->destructor == NULL) {
        task->draw = alarm_draw;
        task->destructor = alarm_destroy;
    }
    if (--extra->timer != 0) {
        return;
    }
    note = &extra->notes[++extra->index];
    data[0] = note->tone;
    data[1] = note->tone / 2;
    data[2] = 0;
    data[3] = 0;
    /* @hack spelling: the assignment inside the second test, where a
     * statement of its own would do */
    if (pdTmrAlarm(extra->port, data) == PDD_TMRERR_BUSY) {
        /* the note wasn't sent: the next step sends it again */
        extra->timer = 1;
        extra->index--;
    } else if ((extra->timer = note->steps) == 0) {
        task_kill(task);
    }
}

/* a task for an alarm, with its extra. returns null when the heap can't give
 * both */
static struct task *alarm_create(void)
{
    /* @hack spelling: one return, of a task that starts null, where a return
     * at each failure would do */
    struct task *task = NULL;
    struct alarm_extra *extra;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((extra = syMalloc(sizeof(*extra))) != NULL) {
        /* @hack spelling: the assignment inside the test, as extra's above,
         * where a statement of its own would do */
        if ((task = task_create(0, TASK_LEVEL, alarm_step, "VmAlarmExec")) !=
            NULL) {
            task->extra.p = extra;
        } else {
            syFree(extra);
        }
    }
    return task;
}

void alarm_play(int port, enum alarm_kind kind)
{
    /* @hack type: kind compared unsigned once its sign is tested, where kind
     * < (int)ALARMS would do. SHC gives enum alarm_kind the type int, so
     * kind can be negative */
    if (kind >= 0 && (Uint32)kind < ALARMS) {
        alarm_start(port, alarms[kind]);
    }
}

/* play notes on the buzzer of port's memory card, from a task. the task
 * kills itself once it has sent the list's end. notes must stay in place
 * until then.
 *
 * nothing starts for a port outside 0 to 23, or for one that has had an
 * alarm. nothing starts either where pdTmrIsReady(port) returns 0, or where
 * the heap can't give the task. the result is 3 on every path, and
 * alarm_play drops it */
static int alarm_start(int port, const struct alarm_note *notes)
{
    struct task *task;
    struct alarm_extra *extra;

    /* @bug logic: refuses a port for good once it has had an alarm. the
     * port's task stays in alarm_tasks after task_destroy frees it, as
     * alarm_stop's tag shows, so TST R3,R3 at 0x8c0624dc finds it ever
     * after */
    if (port >= 0 && port < PORTS && alarm_tasks[port] == NULL &&
        pdTmrIsReady(port)) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((task = alarm_create()) != NULL) {
            extra = task->extra.p;
            alarm_tasks[port] = task;
            extra->port = port;
            extra->notes = notes;
            extra->index = BEFORE_FIRST;
            /* the task's first step sends the first note */
            extra->timer = 1;
        }
    }
    return 3;
}

/* kill the task alarm_start created for port, so that it sends no more
 * notes. nothing silences a tone that is sounding. a port outside 0 to 23,
 * or one with no task, is left alone */
static void alarm_stop(int port)
{
    struct task *task;

    /* @bug memory: kills a task that may be freed already. a port's task
     * stays in alarm_tasks after task_destroy frees it. alarm_start's MOV.L
     * R5,@(R0,R3) at 0x8c0624fc is the only store to alarm_tasks, and
     * alarm_destroy at 0x8c062360 clears only the task's extra. MOV.L
     * @(R0,R1),R3 at 0x8c062528 reads the port's task, and JMP @R3 at
     * 0x8c062534 has task_kill write its step */
    /* @hack spelling: task assigned inside the test, where alarm_tasks[port]
     * tested and passed to task_kill would do */
    if (port >= 0 && port < PORTS && (task = alarm_tasks[port]) != NULL) {
        task_kill(task);
    }
}

void alarm_stop_all(void)
{
    int port;

    for (port = 0; port < PORTS; port++) {
        alarm_stop(port);
    }
}
