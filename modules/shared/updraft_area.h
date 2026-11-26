/* the area test of Radical Highway's and Eternal Engine's updrafts */
#ifndef SHARED_UPDRAFT_AREA_H
#define SHARED_UPDRAFT_AREA_H
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/updraft.h"
#include "system/task.h"

/* whether pos lies in the area work's submode names, in the frame work's pos
 * and ang.y place. AREA_BOX's area is the box within scl.x, scl.y and scl.z of
 * the frame's origin. AREA_COLUMN's lies within scl.x and scl.z of it across,
 * anywhere below it */
static Bool updraft_contains(struct task_work *work, NJS_POINT3 *pos)
{
    NJS_POINT3 point;
    Float height;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, -work->ang.y);
    njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
    njCalcPoint(NULL, pos, &point);
    njPopMatrix(1);
    if (work->submode != AREA_COLUMN) {
        if (_builtin_fabsf(point.x) < work->scl.x &&
            _builtin_fabsf(point.z) < work->scl.z &&
            _builtin_fabsf(point.y) < work->scl.y) {
            return TRUE;
        }
    } else if (_builtin_fabsf(point.x) < work->scl.x &&
               _builtin_fabsf(point.z) < work->scl.z) {
        /* @hack spelling: height, a local for point.y, where point.y in the
         * test would do */
        height = point.y;
        if (height < 0.0F) {
            return TRUE;
        }
    }
    return FALSE;
}

#endif
