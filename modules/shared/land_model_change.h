/* LandModChg, the task that swaps the models of land entries. each group
 * of its entries shows a series of models in turn */
#ifndef SHARED_LAND_MODEL_CHANGE_H
#define SHARED_LAND_MODEL_CHANGE_H
#include <ninja.h>
#include <sg_xpt.h>

struct land_entry;
struct land_table;
struct task;

/* land_model_change_create reads the low 16 bits of each entry's blocks as a
 * key. a key holds a series in its high byte, and a frame in its low byte. a
 * group is one series. models[j] is the model of the series' first entry of
 * frame j. j runs from 0 up to the first frame no entry has. entries holds
 * copies of the series' entries of frame 0, which show the models in turn */
struct land_model_group {
    int model_count;
    NJS_CNK_MODEL **models;
    int entry_count;
    struct land_entry *entries;
};

/* the task's groups, an array it keeps in unk_3c. its work's timer counts
 * them */
#define LAND_MODEL_GROUPS(task) ((struct land_model_group *)(task)->unk_3c.p)

/* make a task named LandModChg that draws land's series of models. it makes
 * a group for each series whose frame 0 has a model. the groups run from the
 * lowest series to the highest. the task's extra takes land's texlist, and its
 * draw is land_model_change_draw. it returns the task, or null where
 * task_create fails. the heap must have room for the groups, unchecked */
struct task *land_model_change_create(struct land_table *land);

#endif
