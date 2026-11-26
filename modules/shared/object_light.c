/* the lights objects cast on the players */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include <string.h>

#include "draw/light.h"
#include "player/player.h"
#include "shared/object_light.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/util.h"

#define LEVEL 1
/* the most object_light_jitter turns a light's dir either way about each axis,
 * in degrees */
#define JITTER 30.0F
/* a random factor from 0.5 up to 1, which object_light_jitter scales a light's
 * color by */
#define RANDOM_DIM() (((rand() * RAND_SCALE) + 1.0F) * 0.5F)
/* the task's queues, in task->motion. lights[player] holds the count[player]
 * lights queued for player since the task's last step */
struct object_light_queues {
    Sint16 count[GAME_PLAYERS];
    struct light lights[GAME_PLAYERS][OBJECT_LIGHT_MOST_QUEUED];
};

#define QUEUES(task) ((struct object_light_queues *)(task)->motion.p)

static void object_light_free(struct task *task);
static void object_light_step(struct task *task);
static void object_light_draw(struct task *task);

/* the task object_light_create made, or null */
static struct task *object_light_task = NULL;

void object_light_destroy(void)
{
    if (object_light_task != NULL) {
        task_destroy(object_light_task);
        object_light_task = NULL;
    }
}

struct task *object_light_create(void)
{
    struct task *task;

    if (object_light_task != NULL) {
        task_destroy(object_light_task);
    }
    task = task_create(TASK_WORK, LEVEL, object_light_step, "EfObjLightExec");
    object_light_task = task;
    if (task != NULL) {
        task->motion.p = syCalloc(1, sizeof(struct object_light_queues));
        if (task->motion.p != NULL) {
            /* @hack spelling: draw set before step, not in the members'
             * order */
            task->draw = object_light_draw;
            /* @hack redundant: the step set again, though task_create set it */
            task->step = object_light_step;
            task->destructor = object_light_free;
            /* @hack redundant: the counts cleared, though syCalloc cleared the
             * queues */
            QUEUES(task)->count[0] = 0;
            QUEUES(task)->count[1] = 0;
        } else {
            /* @hack redundant: the free and the clearing of a record that is
             * already null */
            syFree(task->motion.p);
            task->motion.p = NULL;
            task_destroy(task);
            object_light_task = NULL;
        }
    }
    return object_light_task;
}

void object_light_init(void)
{
    /* @hack spelling: task, a local for object_light_create's result, where
     * the call tested in place would do */
    struct task *task = object_light_create();

    /* @hack redundant: the test of object_light_create's result, where the call
     * alone would do: the function returns either way */
    if (task == NULL) {
        return;
    }
}

static void object_light_free(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task read in place
     * would do */
    struct task *owner = task;

    syFree(owner->motion.p);
    owner->motion.p = NULL;
}

/* set *out to player's light for this step, and return whether any light is
 * queued for player. with none queued, *out is lights[player].
 *
 * a brightness here is |r| + |g| + |b|. each queued light's weight is the
 * brightness of the queue's colors summed through its own. the queue's
 * weighted average then mixes with lights[player]. the queue weighs the
 * brightness of its summed colors, and lights[player] its own */
static Bool object_light_blend(struct task *task, struct light *out, int player)
{
    int i;
    Float weight;
    Float total;
    Float queued;
    Float own;
    Float queued_weight;
    Float own_weight;
    Float peak;
    NJS_VECTOR dir;

    /* @hack spelling: the copy for an empty queue in an else, after the
     * blend, where an early return would do */
    if (QUEUES(task)->count[player] != 0) {
        /* @hack spelling: the color cleared first, not in the members'
         * order */
        out->r = 0.0F;
        out->g = 0.0F;
        out->b = 0.0F;
        out->dir.x = 0.0F;
        out->dir.y = 0.0F;
        out->dir.z = 0.0F;
        out->intensity = 0.0F;
        out->ambient = 0.0F;
        total = 0.0F;
        for (i = 0; i < QUEUES(task)->count[player]; i++) {
            out->r += QUEUES(task)->lights[player][i].r;
            out->g += QUEUES(task)->lights[player][i].g;
            out->b += QUEUES(task)->lights[player][i].b;
            weight = njAbs(out->r) + njAbs(out->g) + njAbs(out->b);
            out->dir.x += QUEUES(task)->lights[player][i].dir.x * weight;
            out->dir.y += QUEUES(task)->lights[player][i].dir.y * weight;
            out->dir.z += QUEUES(task)->lights[player][i].dir.z * weight;
            out->intensity +=
                QUEUES(task)->lights[player][i].intensity * weight;
            out->ambient += QUEUES(task)->lights[player][i].ambient * weight;
            total += weight;
        }
        /* @hack spelling: the direction copied member by member, where dir =
         * out->dir would do */
        dir.x = out->dir.x;
        dir.y = out->dir.y;
        dir.z = out->dir.z;
        /* @bug arithmetic: makes dir a unit vector without a test for zero.
         * dir is zero where every queued light is black, and njUnitVector then
         * makes each part of it not a number (JSR @R3 at 0x256 of the object's
         * code) */
        njUnitVector(&dir);
        queued = njAbs(out->r) + njAbs(out->g) + njAbs(out->b);
        own = njAbs(lights[player].r) + njAbs(lights[player].g) +
              njAbs(lights[player].b);
        /* @bug arithmetic: divides by queued + own, which is 0 where the queued
         * colors sum to black and lights[player] is black too. both weights
         * are then 0 / 0, and the mix makes dir, intensity and ambient not
         * numbers (FDIV FR6,FR12 and FDIV FR6,FR14 at 0x2ba and 0x2bc of the
         * object's code) */
        queued_weight = queued / (queued + own);
        own_weight = own / (queued + own);
        dir.x = (dir.x * queued_weight) + (lights[player].dir.x * own_weight);
        dir.y = (dir.y * queued_weight) + (lights[player].dir.y * own_weight);
        dir.z = (dir.z * queued_weight) + (lights[player].dir.z * own_weight);
        njUnitVector(&dir);
        /* @hack spelling: the direction copied member by member, where
         * out->dir = dir would do */
        out->dir.x = dir.x;
        out->dir.y = dir.y;
        out->dir.z = dir.z;
        out->r += lights[player].r;
        out->g += lights[player].g;
        out->b += lights[player].b;
        peak = out->r;
        if (out->g > peak) {
            peak = out->g;
        }
        if (out->b > peak) {
            peak = out->b;
        }
        if (peak > 1.0F) {
            /* @hack spelling: peak reused for its own reciprocal, where the
             * reciprocal in a local of its own would do */
            peak = 1.0F / peak;
            out->r *= peak;
            out->g *= peak;
            out->b *= peak;
        }
        /* @bug arithmetic: divides by total, which is 0 where every queued
         * light is black. intensity and ambient are then 0 / 0, and the tests
         * below let that through (FDIV FR13,FR3 at 0x36e and 0x37e of the
         * object's code) */
        out->intensity = (out->intensity / total * queued_weight) +
                         (lights[player].intensity * own_weight);
        out->ambient = (out->ambient / total * queued_weight) +
                       (lights[player].ambient * own_weight);
        if (out->r > 1.0F) {
            out->r = 1.0F;
        }
        if (out->r < 0.0F) {
            out->r = 0.0F;
        }
        if (out->g > 1.0F) {
            out->g = 1.0F;
        }
        if (out->g < 0.0F) {
            out->g = 0.0F;
        }
        if (out->b > 1.0F) {
            out->b = 1.0F;
        }
        if (out->b < 0.0F) {
            out->b = 0.0F;
        }
        if (out->intensity > 1.0F) {
            out->intensity = 1.0F;
        }
        if (out->intensity < 0.0F) {
            out->intensity = 0.0F;
        }
        if (out->ambient > 1.0F) {
            out->ambient = 1.0F;
        }
        if (out->ambient < 0.0F) {
            out->ambient = 0.0F;
        }
        return TRUE;
    } else {
        *out = lights[player];
        return FALSE;
    }
}

static void object_light_step(struct task *task)
{
    /* @hack spelling: a blend for each player, where one for both would do */
    struct light blend[GAME_PLAYERS];
    int player;

    for (player = 0; player < GAME_PLAYERS && player_work[player] != NULL;
         player++) {
        if (QUEUES(task)->count[player] != 0) {
            object_light_blend(task, &blend[player], player);
            light_set(OBJECT_LIGHT_BLEND + player, &blend[player]);
            light_select(OBJECT_LIGHT_BLEND + player, player);
            QUEUES(task)->count[player] = 0;
        } else if (light_selected[player] == OBJECT_LIGHT_BLEND + player) {
            light_select(player, player);
        }
    }
}

static void object_light_draw(struct task *task)
{
}

struct light *object_light_add(const struct light *light, int player)
{
    struct light *queued;

    /* @hack spelling: the test against the last player's index, where player
     * < GAME_PLAYERS would do */
    /* @hack spelling: the null return last, after the queueing, where an
     * early return would do */
    if (object_light_task != NULL && player <= GAME_PLAYERS - 1 &&
        QUEUES(object_light_task)->count[player] < OBJECT_LIGHT_MOST_QUEUED) {
        queued =
            &QUEUES(object_light_task)
                 ->lights[player][QUEUES(object_light_task)->count[player]++];
        *queued = *light;
        return queued;
    }
    return NULL;
}

struct light *object_light_add_near(const struct light *light, int player,
                                    const NJS_POINT3 *pos, Float range)
{
    /* @hack spelling: task, a local for object_light_task, where
     * object_light_task read in place would do */
    struct task *task = object_light_task;
    struct light *queued;
    NJS_VECTOR to_player;
    Float distance;
    Float scale;
    Float fade;

    /* @hack spelling: the test against the last player's index, where player
     * >= GAME_PLAYERS would do */
    if (task == NULL || player > GAME_PLAYERS - 1 ||
        QUEUES(task)->count[player] >= OBJECT_LIGHT_MOST_QUEUED ||
        player_work[player] == NULL) {
        return NULL;
    }
    to_player.x = player_work[player]->pos.x - pos->x;
    to_player.y = player_work[player]->pos.y - pos->y;
    to_player.z = player_work[player]->pos.z - pos->z;
    distance = njScalor(&to_player);
    /* @hack spelling: the null return last, after the queueing, where an
     * early return unless distance < range would do */
    if (distance < range) {
        queued = &QUEUES(task)->lights[player][QUEUES(task)->count[player]++];
        *queued = *light;
        /* @bug arithmetic: divides by distance, which is 0 for a player at
         * pos. scale is then infinite, and each part of dir, 0 times it, is
         * not a number (FDIV FR4,FR5 at 0x662 of the object's code) */
        scale = 1.0F / distance;
        queued->dir.x = to_player.x * scale;
        queued->dir.y = to_player.y * scale;
        queued->dir.z = to_player.z * scale;
        fade = (range - distance) / range;
        queued->r *= fade;
        queued->g *= fade;
        queued->b *= fade;
        return queued;
    }
    return NULL;
}

void object_light_jitter(struct light *light)
{
    NJS_VECTOR dir;
    Float scale;

    /* @hack spelling: the direction copied member by member, where dir =
     * light->dir would do */
    dir.x = light->dir.x;
    dir.y = light->dir.y;
    dir.z = light->dir.z;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, RANDOM_ANGLE(JITTER));
    njRotateX(NULL, RANDOM_ANGLE(JITTER));
    njCalcVector(NULL, &dir, &dir);
    njPopMatrix(1);
    /* @hack spelling: the direction copied member by member, where
     * light->dir = dir would do */
    light->dir.x = dir.x;
    light->dir.y = dir.y;
    light->dir.z = dir.z;
    scale = RANDOM_DIM();
    light->r *= scale;
    light->g *= scale;
    light->b *= scale;
}

Bool lights_load(const char *name)
{
    Sint32 size;

#ifdef NONMATCHING
    /* this side reads at most a sector, and takes the file only where it holds
     * lights whole. the matching side copies whatever size the read gives */
    size = loader_read_file_checked(name, WORK_AREA, GDD_FS_SCTSIZE);
    if (size != (Sint32)sizeof(lights)) {
        return FALSE;
    }
    memcpy(lights, WORK_AREA, sizeof(lights));
    return TRUE;
#else
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    /* @hack spelling: the FALSE return last, after the copy, where an early
     * return would do */
    if ((size = loader_read_file_get_size(name, WORK_AREA)) > 0) {
        memcpy(lights, WORK_AREA, size);
        return TRUE;
    }
    return FALSE;
#endif
}
