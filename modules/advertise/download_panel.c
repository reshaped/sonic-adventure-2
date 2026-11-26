/* the download panel. its task's step and draw are empty */
#include <sg_xpt.h>

#include "advertise/download_panel.h"
#include "advertise/menus.h"
#include "system/task.h"

#define TASK_LEVEL 2

/* destroy the task, where download_panel_create made one that this hasn't
 * destroyed. nothing calls download_panel_kill, and no header declares it.
 * it is external only because SHC leaves out a static function that nothing
 * calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void download_panel_kill(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* download_panel_reset restores this word from the panel's initial state. */
static Sint32 reset_word;

static Sint32 initial_reset_word = 0;
static struct task *panel_task = NULL;

static void download_panel_step(struct task *task)
{
}

static void download_panel_draw(struct task *task)
{
}

static void download_panel_destroy(struct task *task)
{
    download_panel_release();
}

void download_panel_create(void)
{
    struct task *task;
    Uint8 *byte;
    int left;

    task =
        task_create(TASK_WORK, TASK_LEVEL, download_panel_step, "downloadExec");
    task->draw = download_panel_draw;
    task->destructor = download_panel_destroy;
    panel_task = task;
    byte = (Uint8 *)&download_panel_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(download_panel_work); left != 0; --left) {
        *byte++ = 0;
    }
    download_panel_reset();
}

void download_panel_reset(void)
{
    Sint32 loaded;
    Uint8 *byte;
    int left;

    loaded = download_panel_work.loaded;
    byte = (Uint8 *)&download_panel_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(download_panel_work); left != 0; --left) {
        *byte++ = 0;
    }
    reset_word = initial_reset_word;
    download_panel_work.loaded = loaded;
}

void download_panel_kill(void)
{
    if (panel_task) {
        task_destroy(panel_task);
        panel_task = NULL;
    }
}

void download_panel_load(void)
{
    if (!download_panel_work.loaded) {
        download_panel_work.loaded = FALSE;
    }
}

void download_panel_release(void)
{
    if (download_panel_work.loaded) {
        download_panel_work.loaded = FALSE;
    }
}
