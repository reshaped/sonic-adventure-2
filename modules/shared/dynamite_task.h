/* the charge's task. shared/dynamite.c compiles it as the dynamite, and
 * stg21/charge.c as Sand Ocean's charge, with SAND_OCEAN_CHARGE defined. the
 * CHARGE_ macros name the globals each defines */
#ifndef SHARED_DYNAMITE_TASK_H
#define SHARED_DYNAMITE_TASK_H
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/texture_anim.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/fall.h"
#include "object/placement.h"
#include "object/set.h"
#include "shared/blast.h"
#include "shared/common_object.h"
#include "shared/dynamite.h"
#ifndef SAND_OCEAN_CHARGE
#define CHARGE_TEXLISTS dynamite_texlists
#define CHARGE_BLASTS dynamite_blasts
#define CHARGE_BUILD dynamite_build
#define CHARGE_INIT dynamite_init
#define CHARGE_COUNT dynamite_count
#else
#include "shared/dynamite_assets.h"
#endif
#include "shared/item_box.h"
#include "sound/sound.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* the bit of dynamite_flags that marks the table valid. object_states_clear
 * clears it as a stage starts or restarts. dynamite_track empties the table as
 * it sets the bit again */
#define TABLE_READY 0x1
#define DYNAMITE_SLOTS 80

#define DYNAMITE_ID(work) ((Uint8)(work)->ang.x)

enum dynamite_mode { MODE_WHOLE, MODE_BLOWN };

/* the steps since the charge blew up, counting that step as 1. it is 0 while
 * the charge is whole */
#define BLAST_AGE(work) ((work)->tick)

/* the points a charge scores as it blows up. a whole charge whose placement
 * has stopped scores them too, as it ends */
#define CHARGE_SCORE 100
#define BLAST_SOUND 0x100c
#define BLAST_PRIORITY 0
#ifdef SAND_OCEAN_CHARGE
#define BLAST_VOLUME 0
#else
#define BLAST_VOLUME SOUND_LOUDEST
#endif
#define EXPLOSION_LIMIT 100.0F

#define VIBRATION_POWER 2
#define VIBRATION_FREQ 20
#define VIBRATION_TIME 0

/* the bytes of the charge's force record. it stays zeroed, and the charge's
 * code never reads it */
#define FORCE_SIZE 8

/* a charge in the table. a null entry marks a free slot */
struct dynamite_slot {
    struct set_entry *entry;
    Sint32 id;
    NJS_POINT3 position; /* where the charge stood as it joined */
};

static struct dynamite_slot dynamite_slots[DYNAMITE_SLOTS];

#define DYNAMITE_BUILDS 1
#define DYNAMITE_SHAPES 1

static void dynamite_destroy(struct task *task);
static void dynamite_step(struct task *task);
static void dynamite_draw(struct task *task);

NJS_TEXLIST *CHARGE_TEXLISTS[] = {
    &dynamite_texlist,
    &dynamite_flare_texlist,
    &dynamite_blink_texlist,
    &dynamite_glow_texlist,
    NULL,
};
static Bool dynamite_built = FALSE;
static struct model_build dynamite_builds[DYNAMITE_BUILDS] = {
    {MODEL_BUILD_ALONE, &dynamite_texlist, &dynamite_object, NULL},
};
/* clang-format off */
static struct collision_shape dynamite_shapes[DYNAMITE_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 0.0F, 0.0F}, 7.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
Sint32 CHARGE_BLASTS = 0;
/* the flare's center, in the charge's frame */
static NJS_POINT3 flare_pos = {0.0F, 10.0F, 5.0F};
static Sint32 dat_dynamite_2 = 0x300;
/* in pixels */
static Float flare_half_side = 15.0F;
static Float dat_dynamite_3 = 5.0F;
static Sint32 dat_dynamite_4 = 18;
static Sint32 dat_dynamite_5 = 0;
#ifdef SAND_OCEAN_CHARGE
static Float flare_distance = 400.0F;
#endif

Uint8 *CHARGE_BUILD(Uint8 *buf)
{
    if (model_builds_compile(&buf, dynamite_builds, DYNAMITE_BUILDS) ==
        DYNAMITE_BUILDS) {
        dynamite_built = TRUE;
    }
    return buf;
}

static void dynamite_slots_clear(void)
{
    int i;

    for (i = 0; i < DYNAMITE_SLOTS; i++) {
        dynamite_slots[i].entry = NULL;
        dynamite_slots[i].id = 0;
        dynamite_slots[i].position.x = 0.0F;
        dynamite_slots[i].position.y = 0.0F;
        dynamite_slots[i].position.z = 0.0F;
    }
}

/* add the set file entry of task's placement to the table, with the charge's
 * id and pos. a table that holds the entry already, or is full, takes nothing.
 * the call first empties a table that isn't valid */
static void dynamite_track(struct task *task)
{
    struct task_work *work = task->work;
    struct set_entry *entry;
    struct dynamite_slot *slot;
    int i;

    if (!(dynamite_flags & TABLE_READY)) {
        dynamite_flags |= TABLE_READY;
        dynamite_slots_clear();
    }
    if (task != NULL && task->placement != NULL) {
        /* @hack redundant: a test of the placement, which the test above has
         * made */
        if (task->placement != NULL) {
            if (task->placement->entry != NULL) {
                entry = task->placement->entry;
                /* @hack spelling: slot set before entry is tested again, where
                 * the first loop's start would set it */
                slot = dynamite_slots;
                /* @hack redundant: a test of entry, which the test above has
                 * made */
                if (entry != NULL) {
                    /* @hack spelling: i = i + 1, where i++ would do */
                    for (i = 0; i < DYNAMITE_SLOTS; i = i + 1, slot++) {
                        if (slot->entry == entry) {
                            return;
                        }
                    }
                    for (i = 0, slot = dynamite_slots; i < DYNAMITE_SLOTS;
                         i++, slot++) {
                        if (slot->entry == NULL) {
                            slot->entry = entry;
                            slot->id = DYNAMITE_ID(work);
                            slot->position = work->pos;
                            return;
                        }
                    }
                }
            }
        }
    }
}

/* take the set file entry of task's placement out of the table. a table that
 * isn't valid, or doesn't hold the entry, is left as it is */
static void dynamite_untrack(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: id, which nothing reads */
    int id = work->ang.x;
    struct set_entry *entry;
    struct dynamite_slot *slot;
    int i;

    if (dynamite_flags & TABLE_READY) {
        if (task != NULL && task->placement != NULL) {
            /* @hack redundant: a test of the placement, which the test above
             * has made */
            if (task->placement != NULL) {
                if (task->placement->entry != NULL) {
                    entry = task->placement->entry;
                    /* @hack spelling: slot set before entry is tested again,
                     * where the loop's start would set it */
                    slot = dynamite_slots;
                    /* @hack redundant: a test of entry, which the test above
                     * has made */
                    if (entry != NULL) {
                        for (i = 0; i < DYNAMITE_SLOTS; i++, slot++) {
                            if (slot->entry == entry) {
                                slot->entry = NULL;
                                return;
                            }
                        }
                        /* @hack redundant: a test of entry that no path
                         * reaches */
                        if (0) {
                            if (entry != NULL) {
                            }
                        }
                    }
                }
            }
        }
    }
}

int CHARGE_COUNT(int id)
{
    int count = 0;
    struct dynamite_slot *slot;
    int i;

    if (!(dynamite_flags & TABLE_READY)) {
        return 0;
    }
    for (i = 0, slot = dynamite_slots; i < DYNAMITE_SLOTS; i++, slot++) {
        if (slot->id == id && slot->entry != NULL) {
            count++;
        }
    }
    return count;
}

void CHARGE_INIT(struct task *task)
{
#ifndef SAND_OCEAN_CHARGE
    blast_load_texture();
#endif
    dynamite_track(task);
    if (placement_try_leave(task)) {
        return;
    }
    task->force.p = syCalloc(1, FORCE_SIZE);
    if (task->force.p == NULL) {
        return;
    }
    task->draw = dynamite_draw;
    task->step = dynamite_step;
    task->destructor = dynamite_destroy;
    collision_init(task, dynamite_shapes, DYNAMITE_SHAPES, COLLISION_ITEM);
    enemy_work_alloc(task);
}

static void dynamite_destroy(struct task *task)
{
#ifndef SAND_OCEAN_CHARGE
    struct task_work *work = task->work;
#endif

    enemy_destroy(task);
#ifndef SAND_OCEAN_CHARGE
    /* @hack spelling: the three tests nested, where one test joined by &&
     * would do */
    if (task->placement != NULL) {
        if (BLAST_AGE(work) == 0) {
            if (!(task->placement->flags &
                  (PLACEMENT_ACTIVE | PLACEMENT_LIVE))) {
                score_add(CHARGE_SCORE);
                dynamite_untrack(task);
            }
        }
    }
#endif
    syFree(task->force.p);
    task->force.p = NULL;
}

static void dynamite_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;

    if (placement_try_leave(task)) {
        return;
    }
    if (BLAST_AGE(work) != 0) {
        if (BLAST_AGE(work) > DYNAMITE_FADE_STEPS) {
            if (task->placement != NULL) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
        }
        BLAST_AGE(work)++;
        return;
    }
    enemy = task->motion.p;
    if (((work->flags & WORK_HURT) &&
         (enemy->shot_by == 0 || enemy->shot_by == 1)) ||
        ((work->collision->flags & COLLISION_TOUCHED) &&
         work->collision->hit->owner->work->id == WORK_ID_LOCKON_BLAST)) {
        work->mode = MODE_BLOWN;
        BLAST_AGE(work) = 1;
        score_add(CHARGE_SCORE);
        explosion_create_sized(&work->pos, EXPLOSION_LIMIT);
        blast_create(BLAST_MEDIUM_RAISED, &work->pos);
#ifdef SAND_OCEAN_CHARGE
        sound_play(BLAST_SOUND, NULL, BLAST_PRIORITY, BLAST_VOLUME);
#else
        sound_play_at(BLAST_SOUND, NULL, BLAST_PRIORITY, BLAST_VOLUME,
                      &work->pos);
#endif
        dynamite_untrack(task);
        CHARGE_BLASTS++;
#ifndef SAND_OCEAN_CHARGE
        vibration_start(enemy->shot_by, VIBRATION_POWER, VIBRATION_FREQ,
                        VIBRATION_TIME);
        if (task->placement != NULL) {
            placement_stop(task);
        }
#endif
    } else {
        collision_register(task);
    }
}

#include "shared/dynamite_draw.h"

#endif
