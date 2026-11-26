/* a hatch's field, which hatch.c includes for each hatch. it acts only while
 * the hatch is open and its depth is past CLEARANCE. outside the mouth, it
 * pulls toward the hatch, weaker with distance, out to PART(reach). inside the
 * wall hatch, it also marks the points it pulls FIELD_LIFTS */
static void PART(field)(struct task *task, NJS_POINT3 *pos,
                        struct field_sample *sample)
{
    struct task_work *work = task->work;
    NJS_VECTOR pull;
    NJS_POINT3 local;
    NJS_VECTOR direction;
    Float distance;
    Float strength;
    Bool lethal;
    Bool lifts;

    if (work->mode != OPEN) {
        return;
    }
    lifts = FALSE;
    lethal = FALSE;
    if (!(HATCH(task)->depth > CLEARANCE)) {
        return;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, -work->ang.y);
    njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
    njCalcPoint(NULL, pos, &local);
#ifdef WALL_HATCH
#define OUTSIDE_MOUTH (local.x > WALL_MOUTH_X)
#else
#define OUTSIDE_MOUTH (local.y > FLOOR_MOUTH_Y)
#endif
    if (OUTSIDE_MOUTH) {
        pull.x = -local.x;
#ifdef WALL_HATCH
        pull.y = -local.y;
#else
        pull.y = -local.y + FLOOR_MOUTH_Y;
#endif
        pull.z = -local.z;
        njUnitVector(&pull);
    } else {
#ifdef WALL_HATCH
        pull.y = pull.z = 0.0F;
        pull.x = -INNER_PULL;
#else
        pull.z = pull.x = 0.0F;
        pull.y = -INNER_PULL;
#endif
    }
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    pull.x *= PART(pull);
    pull.y *= PART(pull);
    pull.z *= PART(pull);
#else
    {
        /* @hack spelling: a Float pointer stepped through pull, where the
         * members scaled one by one would do */
        Float *axis = &pull.x;
        *axis++ *= PART(pull);
        *axis++ *= PART(pull);
        *axis *= PART(pull);
    }
#endif
    distance = njScalor(&local);
    if (OUTSIDE_MOUTH) {
        if (distance > PART(reach)) {
            pull.x = 0.0F;
            pull.y = 0.0F;
            pull.z = 0.0F;
        } else {
            strength = 1.0F - (distance / PART(reach));
            pull.x *= strength;
            pull.y *= strength;
            pull.z *= strength;
        }
#ifdef WALL_HATCH
        if (distance < PART(reach) && local.y < WALL_KILL_EDGE) {
            lethal = TRUE;
        }
#endif
    } else {
#ifdef WALL_HATCH
        if (distance < PART(reach) &&
            (local.x < WALL_KILL_EDGE || local.y < WALL_KILL_EDGE)) {
            lethal = TRUE;
        }
#define INNER_REACH PART(reach)
#else
        if (distance < FLOOR_KILL_RANGE && local.y < FLOOR_KILL_Y) {
            lethal = TRUE;
        }
#define INNER_REACH FLOOR_INNER_REACH
#endif
        if (distance > INNER_REACH) {
            pull.x = 0.0F;
            pull.y = 0.0F;
            pull.z = 0.0F;
        }
#ifdef WALL_HATCH
        else {
            lifts = TRUE;
        }
#endif
#undef INNER_REACH
    }
    njUnitMatrix(NULL);
    njRotateY(NULL, work->ang.y);
    if (lethal) {
        sample->flags |= FIELD_KILLS;
    }
    if (njScalor(&pull) > NO_PULL) {
        njCalcVector(NULL, &pull, &direction);
        sample->flags |= FIELD_SPD;
        if (lifts) {
            sample->flags |= FIELD_LIFTS;
        }
        sample->spd = direction;
    }
    njPopMatrixEx();
}
#undef OUTSIDE_MOUTH
