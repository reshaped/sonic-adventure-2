/* the global light switch, G LIGHT SW in a stage's object list. its volumes
 * give LIGHT_VOLUME to a player inside them */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/light.h"
#include "object/set.h"
#include "player/player.h"
#include "system/game_state.h"
#include "system/task.h"

/* Egg Quarters compiles this source twice more, as stg26/hiding_light.c and
 * stg26/hiding_collision.c. each first defines VOLUME_INIT,
 * VOLUME_MANAGER_INIT, VOLUME_CONTACTS and VOLUME_PREVIOUS, to name the
 * functions and the flags. neither defines VOLUME_CREATES_MANAGER, since the
 * stage creates their managers itself. hiding_collision.c also defines
 * VOLUME_PRESERVES_LIGHT. its manager then changes no light, and its box,
 * sphere and first cylinder take SHAPE_NO_OTHER_HITS */
#ifndef VOLUME_INIT
#include "shared/global_light_switch.h"
#define VOLUME_CREATES_MANAGER
#define VOLUME_MANAGER_INIT global_light_manager_init
#define VOLUME_INIT global_light_switch_init
#define VOLUME_CONTACTS global_light_contacts
#define VOLUME_PREVIOUS global_light_previous
#endif
/* the flags of the box, the sphere and the first cylinder. those shapes pass
 * over items, enemies, objects and pickups */

/* a volume's shape, which the low two bits of ang.z pick */
enum global_light_switch_shape { CUBE, SPHERE, CYLINDER, OUT_OF_ORDER };
#define SHAPES 4
/* the byte of ang.z that holds the shape. the rest turns a box about z */
#define SHAPE_BYTE 0xff
#define SHAPE_BITS 0x3
#define SHAPE(work) ((work)->timer)

#define MANAGER_LEVEL 1

static void global_light_switch_draw(struct task *task);
static void global_light_switch_step(struct task *task);
static void global_light_manager_step(struct task *task);
#ifdef VOLUME_CREATES_MANAGER
static void global_light_manager_init(struct task *task);

/* forget the manager's task without destroying it, and clear
 * global_light_previous. nothing calls it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void global_light_manager_reset(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* for each player, whether a volume's step has found the player inside it
 * since the manager's last step */
static Bool global_light_contacts[GAME_PLAYERS] = {FALSE, FALSE};
/* for each player, global_light_contacts as the manager's last step found it */
static Bool global_light_previous[GAME_PLAYERS] = {FALSE, FALSE};
#else
/* stg26/hiding_spot.h declares these flags, which stg26/guard.c and
 * shared/color_torch.c read */
Bool VOLUME_CONTACTS[GAME_PLAYERS];
Bool VOLUME_PREVIOUS[GAME_PLAYERS];
#endif

/* a shape for each global_light_switch_shape. VOLUME_INIT sizes a volume's
 * copy by its work's scl */
static struct collision_shape global_light_switch_shapes[SHAPES] = {
#include "shared/global_light_switch/global_light_switch_shapes.inc"
};
/* the shapes' names, by global_light_switch_shape */
static char *global_light_switch_shape_names[SHAPES] = {
    "CUBE", "SPHERE", "CYLINDER", "OUT OF ORDER"};
static Sint8 dat_global_light_switch_1[SHAPES] = {
#include "shared/global_light_switch/dat_global_light_switch_1.inc"
};
#ifdef VOLUME_CREATES_MANAGER
/* the manager's task, or null before the first volume creates it */
static struct task *global_light_manager = NULL;
#endif

void VOLUME_INIT(struct task *task)
{
    struct task_work *work = task->work;
#ifdef VOLUME_CREATES_MANAGER
    struct task *manager;
#endif

    if (placement_try_leave(task)) {
        return;
    }
#ifdef VOLUME_CREATES_MANAGER
    if (global_light_manager == NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((manager = task_create(TASK_WORK, MANAGER_LEVEL,
                                   global_light_manager_init,
                                   "ObjectGlobalLightManager")) == NULL) {
            return;
        }
        global_light_manager = manager;
    }
#endif
    SHAPE(work) = work->ang.z & SHAPE_BITS;
    work->ang.x = 0;
    work->ang.z &= ~SHAPE_BYTE;
    collision_init(task, &global_light_switch_shapes[SHAPE(work)], 1,
                   COLLISION_OBJECT);
    work->collision->shapes->radius = work->scl.x;
    work->collision->shapes->size_y = work->scl.y;
    work->collision->shapes->size_z = work->scl.z;
    if (SHAPE(work) == CUBE) {
        work->collision->shapes->ang.z = work->ang.z;
    }
    collision_fit_record_radius(work->collision);
    task->step = global_light_switch_step;
    task->draw = global_light_switch_draw;
}

static void global_light_switch_draw(struct task *task)
{
}

/* whether collision kept a hit of the first shape of player's
 * COLLISION_PLAYER record */
static Bool global_light_switch_touches(struct collision_work *collision,
                                        int player)
{
    struct collision_hit *hit;
    struct collision_hit *end;

    hit = collision->hits;
    end = &collision->hits[COLLISION_HITS];
    for (; hit < end; hit++) {
        if (hit->hit_shape == HIT_UNUSED) {
            break;
        }
        if (hit->task->work->collision->kind == COLLISION_PLAYER &&
            hit->hit_shape == 0 && player_slot_of(hit->task) == player) {
            return TRUE;
        }
    }
    return FALSE;
}

static void global_light_switch_step(struct task *task)
{
    /* @hack spelling: work, a local for task's work, where task->work read
     * in place would do */
    struct task_work *work = task->work;
    struct collision_work *collision = work->collision;
    int player;

    if (placement_try_leave(task)) {
        return;
    }
    for (player = 0; player < GAME_PLAYERS; player++) {
        if (global_light_switch_touches(collision, player)) {
            VOLUME_CONTACTS[player] = TRUE;
        }
    }
    collision_register(task);
}

void VOLUME_MANAGER_INIT(struct task *task)
{
    task->step = global_light_manager_step;
}

#ifdef VOLUME_CREATES_MANAGER
/* @hack type: external linkage, where static would do */
void global_light_manager_reset(void)
{
    global_light_manager = NULL;
    global_light_previous[0] = FALSE;
    global_light_previous[1] = FALSE;
}
#endif

static void global_light_manager_step(struct task *task)
{
    int player;

    for (player = 0; player < GAME_PLAYERS; player++) {
#ifndef VOLUME_PRESERVES_LIGHT
        if (VOLUME_CONTACTS[player]) {
            if (!VOLUME_PREVIOUS[player]) {
                light_select(LIGHT_VOLUME, player);
            }
        } else if (VOLUME_PREVIOUS[player]) {
            light_select(light_camera, player);
        }
#endif
        VOLUME_PREVIOUS[player] = VOLUME_CONTACTS[player];
        VOLUME_CONTACTS[player] = FALSE;
    }
}
