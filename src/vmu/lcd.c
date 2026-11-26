/* the memory cards' screens: animations a task plays on them */
#include <sg_lcd.h>
#include <sg_xpt.h>
#include <string.h>

#include "system/game_state.h"
#include "system/task.h"
#include "vmu/lcd.h"
#include "vmu/ports.h"

/* the index whose next frame is an animation's first */
#define BEFORE_FIRST (-1)

/* the level the task's task_create gives it */
#define TASK_LEVEL 0

/* a port's animations, and the task's place in the one that plays. once
 * plays where it isn't null, and loop otherwise */
struct lcd_screen {
    const struct lcd_frame *loop;
    const struct lcd_frame *once;
    /* the index of the frame showing. it is BEFORE_FIRST before the first frame
     * shows, and while the last one shows */
    Sint32 index;
    Sint32 timer; /* the steps until the next frame shows */
};

/* nothing calls lcd_set_once or lcd_get_once. SHC leaves out a static
 * function that nothing calls */
/* @hack redundant: external linkage, which no call needs, where static would
 * do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* a port's once is an animation its screen plays one time through, in place
 * of its loop. lcd_set_once sets port's once to frames, not a copy, and
 * starts the port over as lcd_set_loop does. the once's last frame shows for
 * its steps, and then the loop plays from its first frame. lcd_get_once returns
 * port's once, or null for none. the once is none already while its last frame
 * shows. both calls ignore a port outside 0 to 23, and lcd_get_once returns
 * null for one */
void lcd_set_once(int port, const struct lcd_frame *frames);
const struct lcd_frame *lcd_get_once(int port);
/* NOLINTEND(misc-use-internal-linkage) */

static struct lcd_screen lcd_screens[PORTS];

/* the task that plays the animations, or null while there is none. the
 * initializer keeps lcd_task in D, where the image holds it */
static struct task *lcd_task = NULL;

static void lcd_destroy(struct task *task)
{
    /* lcd_task holds another task where lcd_start has run since lcd_stop
     * killed this one */
    if (lcd_task == task) {
        lcd_task = NULL;
    }
}

static void lcd_draw(struct task *task)
{
}

static void lcd_step(struct task *task)
{
    int port;
    struct lcd_screen *screen;
    /* @hack spelling: one variable for the animation playing and for the
     * frame of it to show, where the frame in a variable of its own would
     * do */
    const struct lcd_frame *frame;

    /* task_create leaves draw and destructor null, so only the first step
     * sets them */
    if (task->draw == NULL || task->destructor == NULL) {
        task->draw = lcd_draw;
        task->destructor = lcd_destroy;
    }
    if (game_state.card_state != CARD_IDLE) {
        return;
    }
    for (port = 0; port < PORTS; port++) {
        screen = &lcd_screens[port];
        /* @hack spelling: frame assigned inside the test, where frame set from
         * once, and from loop where once is null, ahead of the test would do */
        if (((frame = screen->once) != NULL ||
             (frame = screen->loop) != NULL) &&
            --screen->timer == 0) {
            frame += ++screen->index;
            pdVmsLcdWrite1(port, frame->image);
            screen->timer = frame->steps;
            /* @hack spelling: frame stepped to the next frame for the test,
             * where frame[1] would do */
            frame++;
            /* frame is the end, so the frame just written is the last. the
             * next to show is the loop's first */
            if (frame->steps == 0 || frame->image == NULL) {
                screen->once = NULL;
                screen->index = BEFORE_FIRST;
            }
        }
    }
}

void lcd_start(void)
{
    if (lcd_task == NULL) {
        lcd_task = task_create(0, TASK_LEVEL, lcd_step, "VmLcdExec");
        memset(lcd_screens, 0, sizeof(lcd_screens));
    }
}

void lcd_stop(void)
{
    if (lcd_task != NULL) {
        task_kill(lcd_task);
        lcd_task = NULL;
    }
}

void lcd_set_loop(int port, const struct lcd_frame *frames)
{
    struct lcd_screen *screen;

    if (port >= 0 && port < PORTS) {
        screen = &lcd_screens[port];
        screen->loop = frames;
        screen->index = BEFORE_FIRST;
        screen->timer = 1;
    }
}

void lcd_set_once(int port, const struct lcd_frame *frames)
{
    struct lcd_screen *screen;

    if (port >= 0 && port < PORTS) {
        screen = &lcd_screens[port];
        screen->once = frames;
        screen->index = BEFORE_FIRST;
        screen->timer = 1;
    }
}

const struct lcd_frame *lcd_get_loop(int port)
{
    /* @hack spelling: the screen through a local, where
     * lcd_screens[port].loop would do */
    struct lcd_screen *screen;

    /* @hack spelling: the null return last, after the branch for a port in
     * range, where an early return for a port out of range would do */
    if (port >= 0 && port < PORTS) {
        screen = &lcd_screens[port];
        return screen->loop;
    }
    return NULL;
}

const struct lcd_frame *lcd_get_once(int port)
{
    /* @hack spelling: the screen through a local, as in lcd_get_loop, where
     * lcd_screens[port].once would do */
    struct lcd_screen *screen;

    /* @hack spelling: the null return last, as in lcd_get_loop, where an
     * early return for a port out of range would do */
    if (port >= 0 && port < PORTS) {
        screen = &lcd_screens[port];
        return screen->once;
    }
    return NULL;
}
