/* LandModChg, the task that swaps the models of land entries */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/world.h"
#include "shared/land_draw.h"
#include "shared/land_model_change.h"
#include "system/task.h"

#define LAND_MODEL_CHANGE_LEVEL 1
#define SERIES_SHIFT 8

static void land_model_change_step_unused(struct task *task);
static void land_model_change_destroy(struct task *task);
static void land_model_change_step(struct task *task);

/* the lowest and highest series of land's entries, or -1 for each where
 * land has none */
static void land_series_range(struct land_table *land, int *lowest,
                              int *highest)
{
    struct land_entry *entry;
    int count;
    int series;

    *lowest = *highest = -1;
    count = land->count;
    entry = land->entries;
    while (count--) {
        series = (Uint8)(entry->blocks >> SERIES_SHIFT);
        if (*lowest < 0 || *lowest > series) {
            *lowest = series;
        }
        if (*highest < 0 || *highest < series) {
            *highest = series;
        }
        entry++;
    }
}

/* the chunk model of land's first entry whose key is key, or null */
static NJS_CNK_MODEL *land_find_model(struct land_table *land, int key)
{
    struct land_entry *entry;
    int count;
    /* @hack type: entry_key, the key held in a Uint16 local, where the blocks
     * cast to Uint16 in the test would do */
    Uint16 entry_key;

    count = land->count;
    entry = land->entries;
    while (count--) {
        entry_key = entry->blocks;
        if (entry_key == key) {
            return (NJS_CNK_MODEL *)entry->object->model;
        }
        entry++;
    }
    return NULL;
}

static int land_count_entries(struct land_table *land, int key)
{
    struct land_entry *entry;
    int count;
    int found;
    /* @hack type: entry_key, the key held in a Uint16 local, where the blocks
     * cast to Uint16 in the test would do */
    Uint16 entry_key;

    count = land->count;
    entry = land->entries;
    for (found = 0; count--; entry++) {
        entry_key = entry->blocks;
        if (entry_key == key) {
            found++;
        }
    }
    return found;
}

/* copy land's entries whose key is key to out, in order */
static void land_copy_entries(struct land_table *land, struct land_entry *out,
                              int key)
{
    struct land_entry *entry;
    int count;
    /* @hack type: entry_key, the key held in a Uint16 local, where the blocks
     * cast to Uint16 in the test would do */
    Uint16 entry_key;

    count = land->count;
    for (entry = land->entries; count--; entry++) {
        entry_key = entry->blocks;
        /* @hack spelling: a continue past the copy, where the copy under the
         * test would do */
        if (entry_key != key) {
            continue;
        }
        *out = *entry;
        out++;
    }
}

struct task *land_model_change_create(struct land_table *land)
{
    struct task *task;
    struct land_model_group *groups;
    NJS_CNK_MODEL *model;
    int lowest;
    int highest;
    int count;
    int series;
    int i;

    task = task_create(TASK_WORK, LAND_MODEL_CHANGE_LEVEL,
                       land_model_change_step_unused, "LandModChg");
    if (task == NULL) {
        return NULL;
    }
    task->draw = land_model_change_draw;
    task->step = land_model_change_step;
    task->destructor = land_model_change_destroy;
    land_series_range(land, &lowest, &highest);
    count = 0;
    groups = syCalloc(highest - lowest + 1, sizeof(struct land_model_group));
    for (series = lowest; series <= highest; series++) {
        for (i = 0;; i++) {
            model = land_find_model(land, series << SERIES_SHIFT | i);
            if (model == NULL) {
                break;
            }
            groups[count].model_count++;
        }
        if (groups[count].model_count) {
            groups[count].models =
                syCalloc(groups[count].model_count, sizeof(NJS_CNK_MODEL *));
            for (i = 0;; i++) {
                model = land_find_model(land, series << SERIES_SHIFT | i);
                if (model == NULL) {
                    break;
                }
                groups[count].models[i] = model;
            }
            groups[count].entry_count =
                land_count_entries(land, series << SERIES_SHIFT);
            if (groups[count].entry_count) {
                groups[count].entries = syCalloc(groups[count].entry_count,
                                                 sizeof(struct land_entry));
                land_copy_entries(land, groups[count].entries,
                                  series << SERIES_SHIFT);
            }
            count++;
        }
    }
    task->unk_3c.p = groups;
    task->work->timer = count;
    task->extra.p = land->texlist;
    return task;
}

/* the step task_create takes, which land_model_change_create replaces
 * before it runs */
static void land_model_change_step_unused(struct task *task)
{
}

static void land_model_change_destroy(struct task *task)
{
    struct land_model_group *group;

    if (LAND_MODEL_GROUPS(task)) {
        group = LAND_MODEL_GROUPS(task);
        /* @bug memory: the destructor frees only the first group's models
         * and entries, at 0x1e4 and 0x1ee of the object's code.
         * land_model_change_create allocates every group's, at 0x150 and
         * 0x18a, so the other groups' leak */
#ifdef NONMATCHING
        /* this side repairs the tag above: it frees the models and entries
         * of each of the groups the work's timer counts */
        {
            int i;

            for (i = 0; i < task->work->timer; i++) {
                syFree(group[i].models);
                syFree(group[i].entries);
            }
        }
#else
        if (group->models) {
            syFree(group->models);
        }
        if (group->entries) {
            syFree(group->entries);
        }
#endif
        syFree(group);
    }
    task->extra.p = NULL;
}

static void land_model_change_step(struct task *task)
{
}
