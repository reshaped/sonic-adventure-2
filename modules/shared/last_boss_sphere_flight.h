/* an aimed sphere's flight and its fade, which each fight's spheres.c includes
 * once, ahead of its sphere_explode. a sphere follows its curve while it lies
 * farther than SPHERE_FINAL_APPROACH from its target. it leaves the curve
 * sooner once its curve_time passes CURVE_END, the curve's end. it then flies
 * straight at the target. a
 * sphere farther than SPHERE_ARENA_LIMIT from the origin bursts. its trails'
 * parent is SPHERE_TRAIL_PARENT. Finalhazard's fight defines
 * SPHERE_TRACK_PLAYER too, under which the target follows the active player
 * for the work's timer steps */
static void sphere_explode(struct task *task);

/* a sphere bursts within HIT_DISTANCE of its target. the sound of its flight
 * lasts FLIGHT_SOUND_STEPS, and its impact plays at IMPACT_VOLUME */
#define HIT_DISTANCE 10.0F
#define FLIGHT_SOUND_STEPS 5
#ifdef SPHERE_TRACK_PLAYER
#define IMPACT_VOLUME 30
/* while the Finalhazard is stunned, a sphere farther than STUNNED_REACH from
 * z = 0 bursts */
#define STUNNED_REACH 1450.0F
#else
#define IMPACT_VOLUME 10
/* the impact's volume where a sphere leaves the arena */
#define ESCAPE_VOLUME (-10)
#endif
/* a sphere's curve_accel, which adds to its step along the curve, grows
 * CURVE_ACCEL_STEP a step while below CURVE_ACCEL_MOST */
#define CURVE_ACCEL_STEP 0.001F
#define CURVE_ACCEL_MOST 0.01F
#define CURVE_END 1.0F

static void sphere_fly(struct task *task)
{
    struct task_work *work = task->work;
    struct last_boss_sphere *sphere = task->extra.p;
    Float distance;
    NJS_POINT3 previous;
    NJS_VECTOR direction;
    /* @hack redundant: unused, two vectors nothing reads, which size the
     * frame */
    NJS_VECTOR unused[2];

    sound_play_repeating_at(SOUND_SPHERE_FLIGHT, task, 1, 0, FLIGHT_SOUND_STEPS,
                            &work->pos);
    previous = work->pos;
    distance = njSqrt(SPHERE_SQ(sphere->target.x - work->pos.x) +
                      SPHERE_SQ(sphere->target.y - work->pos.y) +
                      SPHERE_SQ(sphere->target.z - work->pos.z));
    if (distance < HIT_DISTANCE) {
#ifdef SPHERE_TRACK_PLAYER
        sound_play_at(SOUND_SPHERE_IMPACT, NULL, 0, IMPACT_VOLUME, &work->pos);
        sphere_explode(task);
#else
        sphere_explode(task);
        sound_play_at(SOUND_SPHERE_IMPACT, NULL, 0, IMPACT_VOLUME, &work->pos);
#endif
        sound_play_at(SOUND_SPHERE_BREAK, NULL, 0, 0, &work->pos);
        return;
    }
    if (njSqrt(SPHERE_SQ(work->pos.x) + SPHERE_SQ(work->pos.y) +
               SPHERE_SQ(work->pos.z)) > SPHERE_ARENA_LIMIT) {
#ifndef SPHERE_TRACK_PLAYER
        sound_play_at(SOUND_SPHERE_IMPACT, NULL, 0, ESCAPE_VOLUME, &work->pos);
#endif
        sphere_explode(task);
        return;
    }
#ifdef SPHERE_TRACK_PLAYER
    if (FINALHAZARD_STUNNED() == 1 &&
        _builtin_fabsf(work->pos.z) > STUNNED_REACH) {
        sphere_explode(task);
        return;
    }
#endif
    if (distance > SPHERE_FINAL_APPROACH && sphere->curve_time <= CURVE_END) {
        NJS_POINT3 points[4];
        NJS_POINT3 point;
#ifdef SPHERE_TRACK_PLAYER
        if (work->timer > 0) {
            if (FINALHAZARD_STUNNED() == 0) {
                sphere->target = player_work[finalhazard_players->active]->pos;
            }
            work->timer--;
        }
#endif
        points[0] = sphere->start;
        points[1] = sphere->first_control;
        points[2] = sphere->second_control;
        points[3] = sphere->target;
        njCubicBezierSpline(&points[0].x, &point.x, NULL, sphere->curve_time);
        work->pos = point;
    } else {
        NJS_POINT3 next;
        Float speed;
#ifdef SPHERE_TRACK_PLAYER
        Float remaining;
        vector_diff(&sphere->target, &work->pos, &direction);
        speed = njScalor(&sphere->velocity);
        remaining = njScalor(&direction);
        if (speed > remaining) {
            speed = remaining;
        }
        last_boss_vector_length(&direction, speed, &direction);
#else
        vector_diff(&sphere->target, &work->pos, &direction);
        njUnitVector(&direction);
        speed = njScalor(&sphere->velocity);
        SCALE_COMPONENTS(direction, speed);
#endif
        vector_sum(&work->pos, &direction, &next);
        work->pos = next;
    }
    sphere->curve_time += sphere->curve_speed;
    sphere->curve_time += sphere->curve_accel;
    if (sphere->curve_accel < CURVE_ACCEL_MOST) {
        sphere->curve_accel += CURVE_ACCEL_STEP;
    }
    vector_diff(&work->pos, &previous, &sphere->velocity);
    last_boss_sphere_trail(SPHERE_TRAIL_PARENT, &previous, &work->pos);
}

/* a fading sphere bursts as it starts, and again FADE_STEPS / 2 steps later.
 * it ends FADE_STEPS steps after it starts, or sooner where its fade falls
 * below FADE_GONE. the fade falls FADE_RATE a step. FADE_FIRST is the fade's
 * submode at its first step, and FADE_REST at the steps after */
enum { FADE_FIRST, FADE_REST };
#define FADE_STEPS 60
#define FADE_RATE 0.016F
#define FADE_GONE (-1.0F)

static void sphere_fade(struct task *task)
{
    struct task_work *work = task->work;
    struct last_boss_sphere *sphere = task->extra.p;
    switch (work->submode) {
    case FADE_FIRST:
        last_boss_sphere_burst(task, &work->pos, LAST_BOSS_BURST_SHORT);
        work->timer = FADE_STEPS;
        work->submode++;
        /* fall through */
    case FADE_REST:
        if (work->timer == FADE_STEPS / 2) {
            last_boss_sphere_burst(task, &work->pos, LAST_BOSS_BURST_SHORT);
        }
        if (work->timer-- == 0) {
            task_kill(task);
#ifdef SPHERE_TRACK_PLAYER
            return;
#endif
        }
        break;
    }
    sphere->fade -= FADE_RATE;
    if (sphere->fade < FADE_GONE) {
        task_kill(task);
#ifdef SPHERE_TRACK_PLAYER
        return;
#endif
    }
}
