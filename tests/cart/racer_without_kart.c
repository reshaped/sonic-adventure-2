/* cart_create_racer where the heap has no kart: the create stops the game on
 * port_stop with HEAP and the task's name. port_stop, which stops the console
 * on its crash screen, ends this program instead */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <ninja.h>
#include <shinobi.h>

#include "cart/kart_art.h"
#include "cart/vehicle.h"
#include "collision/collision.h"
#include "engine/dc/crash.h"
#include "player/player.h"
#include "system/task.h"

struct task_work *player_work[PLAYER_SLOTS];
/* player_markers, which the port keeps whether or not code reads it, names
 * them */
NJS_CNK_OBJECT cart_player_marker_1, cart_player_marker_2;

static struct task racer;
static struct task_work racer_work;

struct task *task_create(Uint16 flags, int level,
                         void (*step)(struct task *task), const char *name)
{
    (void)flags;
    (void)level;
    racer.step = step;
    racer.work = &racer_work;
    racer.name = name;
    return &racer;
}

/* the heap has no kart to give */
void *syCalloc(Uint32 count, Uint32 size)
{
    (void)count;
    (void)size;
    return NULL;
}

Bool collision_init(struct task *task, const struct collision_shape *shapes,
                    int count, Uint8 kind)
{
    (void)task;
    (void)shapes;
    (void)count;
    (void)kind;
    assert(!"the create went on without its kart");
    return FALSE;
}

void port_stop(const char *what, const char *name, Uint32 number)
{
    (void)number;
    assert(strcmp(what, "HEAP") == 0);
    assert(name == racer.name);
    exit(0);
}

int main(void)
{
    struct cart_start start;

    memset(&start, 0, sizeof(start));
    cart_create_racer(&start);
    assert(!"cart_create_racer returned without its kart");
    return 1;
}
