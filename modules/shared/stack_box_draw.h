/* the wooden box's draw, as shared/wood_box_impl.h has it, for Dry Lagoon's
 * container to include. the includer names the draw BOX_DRAW and the box's
 * step BOX_STEP. BOX_TEXTURES(task) and BOX_MODEL(task) give a box's art,
 * BOX_COMPILED its build, and BOX_BUILT whether the build is made */
#ifndef SHARED_STACK_BOX_DRAW_H
#define SHARED_STACK_BOX_DRAW_H
#include <ninja.h>
#include <sg_xpt.h>
#include "camera/camera.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "shared/common_object.h"
#include "stage/stage.h"
#include "system/task.h"
#include "system/vector.h"

/* in STAGE_BOSS_BIGFOOT, STAGE_BOSS_HOTSHOT and STAGE_BOSS_FDOG, a box near
 * the current camera draws see-through or not at all. a box is near where its
 * distance from the camera, squared, is below SEE_THROUGH_RANGE2. of the near
 * boxes in stack_tasks that step by BOX_STEP, the one nearest the camera's
 * target draws see-through. a box TARGET_RANGE or farther from the target is
 * never that one */
#define SEE_THROUGH_RANGE2 900.0F
#define TARGET_RANGE 10000.0F
/* the see-through box draws in a white constant material of alpha
 * SEE_THROUGH_ALPHA. each strip's own flags give way to SEE_THROUGH_ATTR_OR's,
 * so every strip uses alpha */
#define SEE_THROUGH_ALPHA 0.5F
#define SEE_THROUGH_ATTR_AND CHUNK_TYPE_BITS
#define SEE_THROUGH_ATTR_OR NJD_FST_UA

static void BOX_DRAW(struct task *task)
{
    /* @hack spelling: see_through set before work, where work first would
     * do */
    Bool see_through = FALSE;
    struct task_work *work = task->work;

    /* @hack spelling: the switch's one case without a break, where a break
     * after it would do */
    switch (stage_current_id()) {
    case STAGE_BOSS_BIGFOOT:
    case STAGE_BOSS_HOTSHOT:
    case STAGE_BOSS_FDOG: {
        Float best = TARGET_RANGE;
        struct task *nearest = NULL;
        NJS_VECTOR offset;
        Float distance;
        int i;

        vector_diff(&work->pos, &camera_views[camera_current_index]->pos,
                    &offset);
        if (njScalor2(&offset) < SEE_THROUGH_RANGE2) {
            see_through = TRUE;
            for (i = 0; i < STACK_TASKS; i++) {
                /* @bug memory: reads stack_tasks[i] at 0x818 of the object's
                 * code without testing stack_tasks for null, where
                 * stack_remove and stack_drop test it. the list is null where
                 * stack_add couldn't allocate it, and the box still draws then
                 */
#ifdef NONMATCHING
                /* this side repairs the tag above: without the list, the scan
                 * finds no box, as for a list this box isn't in */
                if (stack_tasks == NULL) {
                    break;
                }
#endif
                if (stack_tasks[i] == NULL) {
                    continue;
                }
                if (stack_tasks[i]->step != BOX_STEP) {
                    continue;
                }
                vector_diff(&stack_tasks[i]->work->pos,
                            &camera_views[camera_current_index]->pos, &offset);
                if (njScalor2(&offset) > SEE_THROUGH_RANGE2) {
                    continue;
                }
                vector_diff(&camera_views[camera_current_index]->target,
                            &stack_tasks[i]->work->pos, &offset);
                /* @hack spelling: the assignment to distance inside the
                 * test, where a statement of its own would do */
                if ((distance = njScalor(&offset)) < best) {
                    best = distance;
                    nearest = stack_tasks[i];
                }
            }
            if (nearest != task) {
                return;
            }
            draw_control_save();
            draw_attr_save();
            draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
            draw_control_set(DRAW_CONTROL_CONSTANT);
            njSetConstantAttr(SEE_THROUGH_ATTR_AND, SEE_THROUGH_ATTR_OR);
            constant_material_set(SEE_THROUGH_ALPHA, 1.0F, 1.0F, 1.0F);
        }
    }
    }
    njSetTexture(BOX_TEXTURES(task));
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    if (BOX_BUILT && !see_through) {
        njDirectDrawObject(BOX_COMPILED);
    } else {
        njCnkEasyDrawModel(BOX_MODEL(task));
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    if (see_through) {
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    }
    njPopMatrixEx();
}

#endif
