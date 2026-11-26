/* draws of the land tables a stage holds beside its land */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "shared/land_draw.h"
#include "shared/land_model_change.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"

/* how many of game_state.steps each model of a series shows for */
static Uint32 steps_per_model = 1;

void land_model_change_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct land_model_group *group;
    struct land_entry *entry;
    NJS_OBJECT *object;
    NJS_CNK_MODEL *model;
    int i;
    int j;
    int count;
    /* @hack redundant: unused, a local nothing reads, which only sizes the
     * frame */
    NJS_POINT3 unused;

    if (task->extra.p) {
        njSetTexture(task->extra.p);
    }
    group = LAND_MODEL_GROUPS(task);
    njPushMatrixEx();
    for (i = 0; i < work->timer; i++, group++) {
        model = group->models[(game_state.steps / steps_per_model) %
                              group->model_count];
        count = group->entry_count;
        entry = group->entries;
        for (j = 0; j < count; j++, entry++) {
            object = entry->object;
            njTranslateEx((NJS_VECTOR *)object->pos);
            njRotateEx(object->ang, 0);
            njCnkEasyDrawModel(model);
            matrix_load_parent();
        }
    }
    njPopMatrixEx();
}

void land_table_draw(struct land_table *land)
{
    /* @hack spelling: table, a local for land, where land itself would do */
    struct land_table *table = land;
    int count;
    Sint32 blocks;
    struct land_entry *entry;
    NJS_OBJECT *object;
    NJS_POINT3 pos;
    /* @hack spelling: state, a local for &game_state, where game_state read
     * in place would do */
    struct game_state *state = &game_state;

    /* @hack spelling: the camera's blocks picked by a test of its index,
     * where land_blocks[camera_current_index] would do for a split screen's
     * two cameras */
    if (camera_current_index == 0) {
        blocks = state->land_blocks[0];
    } else {
        blocks = state->land_blocks[1];
    }
    count = table->count;
    entry = table->entries;
    njPushMatrixEx();
    njSetTexture(table->texlist);
    while (count--) {
        if (!entry->blocks || (entry->blocks & blocks)) {
            njCalcPoint(NULL, &entry->center, &pos);
            if (((entry->flags & LAND_FAR) || pos.z < table->clip) &&
                pos.z > -entry->radius) {
                object = entry->object;
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                njCnkEasyDrawModel((NJS_CNK_MODEL *)object->model);
                matrix_load_parent();
            }
        }
        /* @hack redundant: a block no path reaches */
        if (0) {
            if (blocks) {
            }
        }
        entry++;
    }
}
