/* Wild Canyon's lens flare, EFLENSF0 in its object list: the lens flare of
 * shared/lens_flare.h. it shows only from the canyon's high half, and from near
 * one spot below it. stg18/lagoon_lens_flare.c compiles this source again for
 * Dry Lagoon, with a CANYON_FLARE_HIDE of its own */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "player/player.h"
#include "shared/canyon_lens_flare.h"
#include "shared/canyon_util.h"
#include "shared/lens_flare.h"
#include "system/task.h"

/* the slots of canyon_flare_rivals */
#define RIVALS 6

/* the spot, in x and z, near which the flare draws below HALF_LINE: within
 * SPOT_RADIUS of it */
#define SPOT_X 200.0F
#define SPOT_Z 0.0F
#define SPOT_RADIUS 70.0F

/* the statements that end the draw while the flare hides. here they end it
 * while player 0 stands below HALF_LINE, away from the spot. a variant defines
 * its own first */
#ifndef CANYON_FLARE_HIDE
#define CANYON_FLARE_HIDE                                                      \
    {                                                                          \
        Float dx;                                                              \
        Float dz;                                                              \
        Float distance_squared;                                                \
                                                                               \
        dx = player_work[0]->pos.x - SPOT_X;                                   \
        dz = player_work[0]->pos.z - SPOT_Z;                                   \
        distance_squared = (dx * dx) + (dz * dz);                              \
        if (player_work[0]->pos.y < HALF_LINE &&                               \
            distance_squared > (SPOT_RADIUS * SPOT_RADIUS)) {                  \
            return;                                                            \
        }                                                                      \
    }
#endif

static void canyon_flare_step(struct task *task);
static void canyon_flare_draw(struct task *task);
static void canyon_flare_destroy(struct task *task);

/* copies of shared/lens_flare.c's cells and sprite */
static NJS_TEXANIM canyon_flare_cells[8] = {
#include "shared/canyon_lens_flare/canyon_flare_cells.inc"
};
static NJS_SPRITE canyon_flare_sprite = {
#include "shared/canyon_lens_flare/canyon_flare_sprite.inc"
};
/* the listed rivals, each in a slot of its own. a free slot is null */
static struct task *canyon_flare_rivals[RIVALS] = {
    NULL, NULL, NULL, NULL, NULL, NULL,
};

/* put task in a free slot of canyon_flare_rivals, and return TRUE. it
 * returns FALSE where no slot is free */
static Bool canyon_flare_add_rival(struct task *task)
{
    int i;

    for (i = 0; i < RIVALS; i++) {
        if (canyon_flare_rivals[i] == NULL) {
            canyon_flare_rivals[i] = task;
            return TRUE;
        }
    }
    return FALSE;
}

static void canyon_flare_remove_rival(struct task *task)
{
    int i;

    for (i = 0; i < RIVALS; i++) {
        if (canyon_flare_rivals[i] == task) {
            canyon_flare_rivals[i] = NULL;
        }
    }
}

/* whether task's work lies at least as near the current camera as each
 * listed rival's work */
static Bool canyon_flare_is_nearest_rival(struct task *task)
{
    int i;
    struct task *rival;
    Float distance;

    distance = njDistanceP2P(&camera_views[camera_current_index]->pos,
                             &task->work->pos);
    for (i = 0; i < RIVALS; i++) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((rival = canyon_flare_rivals[i]) != NULL &&
            distance > njDistanceP2P(&camera_views[camera_current_index]->pos,
                                     &rival->work->pos)) {
            return FALSE;
        }
    }
    return TRUE;
}

void canyon_lens_flare_init(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its two strings,
     * which SHC still places in this source's constants */
    if (0) {
        (void)"<- ALT";
        (void)"<- ALL";
    }
    task->step = canyon_flare_step;
    /* @hack spelling: destructor set before draw, not in the members'
     * order */
    task->destructor = canyon_flare_destroy;
    task->draw = canyon_flare_draw;
    if (task->work->ang.x & LENS_FLARE_RIVAL) {
        canyon_flare_add_rival(task);
    }
}

static void canyon_flare_step(struct task *task)
{
    /* @hack redundant: task's address taken, which nothing uses */
    (void)&task;
}

static void canyon_flare_draw(struct task *task)
{
    CANYON_FLARE_HIDE;
    if (!(task->work->ang.x & LENS_FLARE_RIVAL) ||
        canyon_flare_is_nearest_rival(task)) {
        lens_flare_draw(&task->work->pos);
    }
}

static void canyon_flare_destroy(struct task *task)
{
    canyon_flare_remove_rival(task);
}
