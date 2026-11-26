#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include "boss_bigbogy/holder.h"
#include "boss_bigbogy/hourglass.h"
#include "boss_bigbogy/stage.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "shared/sandglass.h"
#include "shared/sandglass_registry.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"

#define SHADOW_BITS (NJD_CONTROL_3D_SHADOW | NJD_CONTROL_3D_TRANS_MODIFIER)
#define SQUARE(value) ((value) * (value))

/* the sandglass starts GLASS_LIFT above the ghost */
#define GLASS_LIFT 10.0F
/* the ghost carries the sandglass CARRY_REACH from it */
#define CARRY_REACH 20.0F
/* the shadow lies SHADOW_DROP below the sandglass, but no lower than
 * SHADOW_FLOOR above the floor. its model takes a scale of SHADOW_SCALE
 * across the ground */
#define SHADOW_DROP 5.0F
#define SHADOW_FLOOR 0.2F
#define SHADOW_SCALE 9.0F

struct task *bigbogy_sandglass_create(struct task *boss, struct task *holder)
{
    NJS_POINT3 pos;
    pos = holder->child->work->pos;
    pos.y += GLASS_LIFT;
    return sandglass_create(&pos, holder->child->work->ang.y,
                            BIGBOGY_SANDGLASS_ID, BIGBOGY_SAND_STEPS);
}

Bool bigbogy_sandglass_carry(struct task *glass, struct task *holder)
{
    struct task_work *work;
    NJS_POINT3 pos;
    Angle direction;
    Bool drop;

    if (!glass) {
        return FALSE;
    }
    /* @hack spelling: drop, a flag of the two tests, where one test of both
     * would do */
    drop = FALSE;
    if (!holder->child) {
        drop = TRUE;
    } else if (holder->child->work->collision) {
        if (holder->child->work->collision->shapes->flags & SHAPE_OFF) {
            drop = TRUE;
        }
    }
    if (drop == TRUE) {
        work = glass->work;
        sandglass_throw(glass,
                        (Angle)(atan2f(work->pos.x - bigbogy_arena_center.x,
                                       work->pos.z - bigbogy_arena_center.z) *
                                ANGLE_PER_RADIAN));
        return FALSE;
    }
    /* @hack spelling: an else after the return, where the statements after
     * the if would do */
    else {
        pos = holder->child->work->pos;
        if (bigbogy_holder_is_dashing(holder) == TRUE) {
            direction = holder->child->work->ang.y + QUARTER_TURN;
            pos.x += njSin(direction) * CARRY_REACH;
            pos.z += njCos(direction) * CARRY_REACH;
        } else {
            pos.y += CARRY_REACH;
        }
        sandglass_carry(glass, &pos, QUARTER_TURN);
        return TRUE;
    }
}

void bigbogy_sandglass_shadow(struct task *glass)
{
    if (glass) {
        NJS_POINT3 pos;
        /* @hack redundant: unused, a point set to 0 that nothing reads */
        NJS_POINT3 unused = {0.0F, 0.0F, 0.0F};
        Float floor;
        (void)unused;
        pos = glass->work->pos;
        pos.y -= SHADOW_DROP;
        floor = bigbogy_arena_center.y + SHADOW_FLOOR;
        if (floor > pos.y) {
            pos.y = floor;
        }
        draw_control_set(SHADOW_BITS);
        njPushMatrixEx();
        njTranslateV(NULL, &pos);
        njScale(NULL, SHADOW_SCALE, 1.0F, SHADOW_SCALE);
        njCnkModDrawObject(&shadow_object);
        njPopMatrixEx();
        draw_control_clear(SHADOW_BITS);
    }
}

void bigbogy_wall_points(const NJS_POINT3 *pos, Sint8 count, NJS_POINT3 *out)
{
    NJS_VECTOR radial;
    NJS_POINT3 *point;
    Angle step;
    Angle angle;
    Sint8 i;

#ifdef NONMATCHING
    /* this side sets nothing for a count below 1, which the header rules out.
     * the matching side divides by a count of 0 below */
    if (count <= 0) {
        return;
    }
#endif
    vector_diff(pos, &bigbogy_arena_center, &radial);
    point = out;
    step = FULL_TURN / count;
    njPushMatrix(&_nj_unit_matrix_);
    njTranslateV(NULL, (NJS_VECTOR *)&bigbogy_arena_center);
    angle = 0;
    for (i = 0; i < count; i++) {
        NJS_POINT3 origin = {0.0F, 0.0F, 0.0F};
        njPushMatrixEx();
        njRotateY(NULL, angle);
        njTranslateV(NULL, &radial);
        njCalcPoint(NULL, &origin, point);
        njPopMatrixEx();
        angle += step;
        point->y = BIGBOGY_WALL_HEIGHT;
        point++;
    }
    njPopMatrix(1);
}

void bigbogy_nearest_wall_point(Sint8 count, const NJS_POINT3 *points,
                                const NJS_POINT3 *pos, NJS_POINT3 *out)
{
    NJS_POINT3 best;
    const NJS_POINT3 *point;
    Float nearest;
    Float distance;
    Sint8 i;

#ifdef NONMATCHING
    /* this side leaves *out alone for a count below 1, which the header rules
     * out. the matching side copies best unset */
    if (count <= 0) {
        return;
    }
#endif
    point = points;
    for (i = 0; i < count; i++, point++) {
        distance =
            njSqrt(SQUARE(pos->x - point->x) + SQUARE(pos->z - point->z));
        /* @hack spelling: a branch of its own for the first point, where
         * i == 0 || nearest > distance would do */
        if (i == 0) {
            nearest = distance;
            best = *point;
        } else if (nearest > distance) {
            nearest = distance;
            best = *point;
        }
    }
    *out = best;
}
