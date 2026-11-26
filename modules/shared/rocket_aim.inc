/* @hack spelling: the way to the place as -pos + scl, where
 * scl - pos would do */
vec.x = -BODY_WORK(task)->pos.x + work->scl.x;
vec.y = -BODY_WORK(task)->pos.y + work->scl.y;
vec.z = -BODY_WORK(task)->pos.z + work->scl.z;
#ifdef FINAL_RUSH_ROCKET
/* @hack spelling: the z component and angle scale are held separately. */
toward_z = vec.z;
angle_units = ANGLE_PER_RADIAN;
aim.z = (Angle)(atan2f(vec.y, njSqrt((toward_z * toward_z) +
                                     (vec.x * vec.x))) * angle_units);
aim.y = -(Angle)(atan2f(vec.z, vec.x) * angle_units);
#else
aim.z = (Angle)(atan2f(vec.y,
                       njSqrt((vec.z * vec.z) + (vec.x * vec.x))) *
                ANGLE_PER_RADIAN);
aim.y = -(Angle)(atan2f(vec.z, vec.x) * ANGLE_PER_RADIAN);
#endif
/* @hack spelling: each turn as -angle + aim, where aim - angle
 * would do */
turn_z = -BODY_WORK(task)->ang.z + aim.z;
turn_y = -BODY_WORK(task)->ang.y + aim.y;
if (turn_y > 0) {
    if (turn_y > AIM_TURN) {
        turn_y = AIM_TURN;
    }
} else if (turn_y < -AIM_TURN) {
    turn_y = -AIM_TURN;
}
if (turn_y == 0) {
    if (turn_z > 0) {
        if (turn_z > AIM_TURN) {
            turn_z = AIM_TURN;
        }
    } else if (turn_z < -AIM_TURN) {
        turn_z = -AIM_TURN;
    }
} else {
    turn_z = 0;
}
BODY_WORK(task)->ang.y += turn_y;
BODY_WORK(task)->ang.z += turn_z;
BODY_WORK(task)->pos.y = work->pos.y + body_rise;
if (rand() * RAND_SCALE < THRUST_CHANCE) {
    tail.x = 0.0F;
    tail.y = AIM_TAIL_Y;
    tail.z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    ROCKET_TRANSLATE(&BODY_WORK(task)->pos);
    njRotateY(NULL, BODY_WORK(task)->ang.y);
    njRotateZ(NULL, BODY_WORK(task)->ang.z);
    njRotateX(NULL, BODY_WORK(task)->ang.x);
    njRotateY(NULL, QUARTER_TURN);
    njRotateZ(NULL, -QUARTER_TURN);
    njRotateX(NULL, QUARTER_TURN);
    /* @hack spelling: a call in each branch, where one call that
     * picks the sign of body_model_offset.x would do */
    if (work->submode == ROCKET_UPRIGHT) {
        njTranslate(NULL, body_model_offset.x, -body_model_offset.y,
                    -body_model_offset.z);
    } else {
        njTranslate(NULL, -body_model_offset.x,
                    -body_model_offset.y, -body_model_offset.z);
    }
    njCalcPoint(NULL, &tail, &tail);
    njCalcVector(NULL, &thrust_spd, &vec);
    ROCKET_POP();
    vec.x += ((rand() * RAND_SCALE) - 0.5F) * thrust_jitter;
    vec.y += ((rand() * RAND_SCALE) - 0.5F) * thrust_jitter;
    vec.z += ((rand() * RAND_SCALE) - 0.5F) * thrust_jitter;
    size =
        (rand() * thrust_size_range * RAND_SCALE) + thrust_size_min;
    rocket_thrust_settle_create(&tail, &vec, size,
                                work->pos.y + thrust_rise + size);
}
