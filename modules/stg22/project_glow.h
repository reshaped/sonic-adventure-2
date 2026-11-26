/* a case of gravity.c's switch on down, for the gravity axis that
 * GLOW_AXIS_X, _Y or _Z names. it probes four points gadget_glow_reach from
 * player 0's center, two along each axis across gravity. the glow takes its
 * height and tilt from the pair whose ground differs more. a point that finds
 * no ground, with a wall in reach, takes a mean height. it is the mean of the
 * points one and three after it, mod 4. ground with GROUND_FOOTSTEPS under any
 * point leaves no glow */
#ifdef GLOW_AXIS_Y
#define AXIS_A x
#define AXIS_B z
#define FIRST_WALL SURFACE_NEG_X
#define SECOND_WALL SURFACE_NEG_Z
#elif defined(GLOW_AXIS_X)
#define AXIS_A y
#define AXIS_B z
#define FIRST_WALL SURFACE_NEG_Y
#define SECOND_WALL SURFACE_NEG_Z
#else
#define AXIS_A x
#define AXIS_B y
#define FIRST_WALL SURFACE_NEG_X
#define SECOND_WALL SURFACE_NEG_Y
#endif

{
    /* @hack redundant: projected_center, a copy of center that no code
     * reads */
    NJS_POINT3 projected_center;
    struct surface_probe probes[SAMPLES];
    NJS_VECTOR slope;
    struct surface_side *surface;
    first = &probes[0];
    first->pos = center;
    first->pos.AXIS_A += gadget_glow_reach;
#ifdef GLOW_AXIS_X
    surface_probe_sides(first);
#else
    surface_probe_sides(&probes[0]);
#endif
    probes[1].pos = center;
    probes[1].pos.AXIS_A -= gadget_glow_reach;
    surface_probe_sides(&probes[1]);
    probes[2].pos = center;
    probes[2].pos.AXIS_B += gadget_glow_reach;
    surface_probe_sides(&probes[2]);
    probes[3].pos = center;
    probes[3].pos.AXIS_B -= gadget_glow_reach;
    surface_probe_sides(&probes[3]);
#ifdef GLOW_AXIS_Y
    /* @hack redundant: a second probe of probes[3] that never runs */
    if (0) {
        surface_probe_sides(&probes[3]);
    }
#endif
    near_wall = 0;
    for (sample = first; sample < &probes[SAMPLES]; sample += 1) {
        if (sample->sides[down].flags & GROUND_FOOTSTEPS) {
            near_wall = 1;
            break;
        }
    }
    if (near_wall) {
        break;
    }
    missing = 0;
    for (sample_index = 0; sample_index < SAMPLES; ++sample_index) {
        if (probes[sample_index].sides[down].found == 0) {
            near_wall = 0;
#define WALL_IN_REACH(test_side, distance_side, component)                     \
    if (probes[sample_index].sides[test_side].found &&                         \
        njAbs(probes[sample_index].sides[distance_side].pos -                  \
              center.component) < WALL_DISTANCE) {                             \
        near_wall = 1;                                                         \
    }
            switch (sample_index) {
            case 0:
                /* points 0 and 1 test the found flags of the x sides,
                 * whatever the axis. under GLOW_AXIS_X they measure the
                 * distance to the y sides */
                WALL_IN_REACH(SURFACE_NEG_X, FIRST_WALL, AXIS_A)
                break;
            case 1:
                WALL_IN_REACH(SURFACE_POS_X, FIRST_WALL + 1, AXIS_A)
                break;
            case 2:
                WALL_IN_REACH(SECOND_WALL, SECOND_WALL, AXIS_B)
                break;
            case 3:
                WALL_IN_REACH(SECOND_WALL + 1, SECOND_WALL + 1, AXIS_B)
                break;
            }
#undef WALL_IN_REACH
            ++missing;
            if (missing != 1 || !near_wall) {
                break;
            }
            adjacent = (sample_index + 1) % SAMPLES;
            opposite = (adjacent + 2) % SAMPLES;
            if (probes[adjacent].sides[down].found &&
                probes[opposite].sides[down].found) {
                surface = &probes[sample_index].sides[down];
                /* @hack redundant: an empty loop that never runs */
                while (0) {
                }
                surface->pos = (probes[adjacent].sides[down].pos +
                                probes[opposite].sides[down].pos) *
                               0.5F;
            } else {
                break;
            }
        }
    }
    if (missing > 1) {
        break;
    }
    rise_a = probes[0].sides[down].pos - probes[1].sides[down].pos;
    rise_b = probes[2].sides[down].pos - probes[3].sides[down].pos;
    if (njAbs(rise_a) > gadget_glow_reach * 2.1F ||
        njAbs(rise_b) > gadget_glow_reach * 2.1F) {
        break;
    }
    if (njAbs(rise_a) > njAbs(rise_b)) {
        slope.x = probes[0].pos.AXIS_A - probes[1].pos.AXIS_A;
        slope.y = rise_a;
        slope.z = probes[0].pos.AXIS_B - probes[1].pos.AXIS_B;
        projected_center = center;
        height = (probes[0].sides[down].pos + probes[1].sides[down].pos) * 0.5F;
        pitch = 0;
#ifdef GLOW_AXIS_Z
        roll =
            (Angle)(atan2f(slope.y, slope.x) * ANGLE_PER_RADIAN) + QUARTER_TURN;
#else
        roll = (Angle)(atan2f(slope.y, slope.x) * ANGLE_PER_RADIAN);
#ifdef GLOW_AXIS_X
        roll = roll + QUARTER_TURN;
#endif
#endif
    } else {
#ifdef GLOW_AXIS_X
        NJS_VECTOR *direction = &slope;
#define SLOPE (*direction)
#else
#define SLOPE slope
#endif
        SLOPE.x = probes[2].pos.AXIS_A - probes[3].pos.AXIS_A;
        SLOPE.y = rise_b;
        SLOPE.z = probes[2].pos.AXIS_B - probes[3].pos.AXIS_B;
        projected_center = center;
        height = (probes[2].sides[down].pos + probes[3].sides[down].pos) * 0.5F;
#ifdef GLOW_AXIS_X
        roll = QUARTER_TURN;
#else
        roll = 0;
#endif
#ifdef GLOW_AXIS_Z
        pitch = (Angle)(atan2f(-SLOPE.y, SLOPE.z) * ANGLE_PER_RADIAN) +
                QUARTER_TURN;
#else
        pitch = (Angle)(atan2f(-SLOPE.y, SLOPE.z) * ANGLE_PER_RADIAN);
#endif
#undef SLOPE
    }
    (void)projected_center;
    GADGET_GLOW(task)->sides[0].found = down;
    GADGET_GLOW(task)->sides[0].ang_x = pitch;
    GADGET_GLOW(task)->sides[0].ang_z = roll;
    GADGET_GLOW(task)->sides[0].pos = height;
    GADGET_GLOW(task)->pos = center;
    found = 1;
}
#undef AXIS_A
#undef AXIS_B
#undef FIRST_WALL
#undef SECOND_WALL
