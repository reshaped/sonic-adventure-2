/* the light volume of stg27/light.h, which Lost Colony's platforms carry */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/light.h"
#include "object/set.h"
#include "player/player.h"
#include "stg27/light.h"
#include "system/game_state.h"
#include "system/task.h"

#define ATTACHED 1
#define RADIUS_UNIT 35.0F
#define RADIUS_LEAST 1.0F /* in RADIUS_UNITs: the radius where scl.x is 0 */
/* the light the volume names, which no code reads past the init */
#define LIGHT(work) ((work)->ang.x)

/* a flag for each player, in the bytes of task->unk_3c. task_create clears
 * the flags, and no code sets one */
/* @hack type: unk_3c read as bytes through its address, where
 * (task)->unk_3c.b would do */
#define PLAYER_FLAGS(task) ((Sint8 *)&(task)->unk_3c)

/* the set editor's label for the volume's ang.x */
static const char editor_light[12] = "<- LIGHTNUM";

static void light_zone_init(struct task *task);
static void light_zone_destroy(struct task *task);
static void light_zone_step(struct task *task);
static void light_zone_draw(struct task *task);

struct task *colony_light_zone_create(struct task *parent)
{
    struct task *task =
        task_create_child(TASK_WORK | TASK_FORCE, light_zone_init, parent);

    if (task != NULL) {
        task->work->submode = ATTACHED;
        LIGHT(task->work) = FLASH_LIGHT;
    }
    return task;
}

static void light_zone_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!work->submode && placement_try_leave(task)) {
        return;
    }
    task->draw = light_zone_draw;
    task->step = light_zone_step;
    task->destructor = light_zone_destroy;
    LIGHT(work) = (Uint8)LIGHT(work);
    if (LIGHT(work) >= LIGHTS) {
        LIGHT(work) = LIGHTS;
    }
}

static void light_zone_destroy(struct task *task)
{
    int player;

#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side: it tests each
     * player, where the matching side tests once with player unset */
    for (player = 0; player < GAME_PLAYERS; player++) {
        if (player_work[player] && PLAYER_FLAGS(task)[player]) {
            PLAYER_FLAGS(task)[player] = 0;
            light_select(player, player);
        }
    }
#else
    /* @bug memory: indexes player_work and PLAYER_FLAGS by player, which no
     * path sets. SHC gives player the stack pointer's value, so the two reads
     * lie outside player_work and the task. where neither read is 0, the store
     * lies outside the task too. that path also sets light_selected[0] to the
     * stack pointer's low byte. MOV R15,R3 at 0x8c50594a sets player. the
     * reads are at 0x8c50594e and 0x8c505958, the store at 0x8c505968, and the
     * jump to light_select at 0x8c50596a */
    /* NOLINTNEXTLINE(clang-diagnostic-uninitialized) */
    if (player_work[player] && PLAYER_FLAGS(task)[player]) {
        PLAYER_FLAGS(task)[player] = 0;
        light_select(player, player);
    }
#endif
}

static void light_zone_step(struct task *task)
{
    struct task_work *work = task->work;
    int player;

    if (!work->submode && placement_try_leave(task)) {
        return;
    }
    for (player = 0; player < GAME_PLAYERS; player++) {
        if (player_work[player] != NULL) {
            if (njDistanceP2P(&player_work[player]->collision->shapes->center,
                              &work->pos) <
                (work->scl.x + RADIUS_LEAST) * RADIUS_UNIT) {
                colony_light_request[player] = TRUE;
            }
        }
        /* @hack redundant: a block no path reaches, a stand-in that gives
         * the step the image's registers */
        if (0) {
            if (work && work->mode) {
            }
        }
    }
}

static void light_zone_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    /* @hack redundant: a block no path reaches, a stand-in that gives the
     * draw the image's frame */
    if (0) {
        njPushMatrixEx();
        pos = work->pos;
        njTranslateEx(&pos);
        njPopMatrixEx();
    }
}
