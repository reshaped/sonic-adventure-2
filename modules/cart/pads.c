/* the task that keeps the karts' pads, which pads.h describes */
#include <shinobi.h>
#include <sg_xpt.h>

#include "cart/pads.h"
#include "system/pad.h"
#include "system/task.h"

static Bool stopping = 1;
PDS_PERIPHERAL cart_pad_history[CART_PAD_FRAMES][CART_PLAYERS];
int cart_pad_frame;
static int delay;
static int next_frame;
static void sample_pads(struct task *task);

static void reset_history(void)
{
    next_frame = 0;
    cart_set_pad_delay(0);
}
void cart_pads_start(void)
{
    task_create(0, 0, sample_pads, "delaykeyExec");
    reset_history();
    stopping = 0;
}
void cart_pads_stop(void)
{
    stopping = 1;
}
void cart_set_pad_delay(int frames)
{
    delay = frames;
}
static void sample_pads(struct task *task)
{
    int i;
    if (stopping) {
        task_destroy(task);
    }
    for (i = 0; i < CART_PLAYERS; i++) {
        cart_pad_history[next_frame][i] = player_pad_records[i];
    }
    cart_pad_frame = next_frame - delay;
    if (cart_pad_frame < 0) {
        cart_pad_frame += CART_PAD_FRAMES;
    }
    next_frame++;
    if (next_frame >= CART_PAD_FRAMES) {
        next_frame = 0;
    }
}
