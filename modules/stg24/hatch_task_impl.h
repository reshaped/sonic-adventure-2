/* a hatch's init, destructor and step, which hatch.c includes for each
 * hatch */
#include <shinobi.h>
#include "effect/burst.h"
#include "system/angle.h"

void PART(init)(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *collision;
    Float radius;
    Uint32 flags;

    if (!placement_try_leave(task)) {
        SET_HATCH(task, syCalloc(1, sizeof(struct hatch)));
        if (HATCH(task)) {
            if (models_ready) {
                task->draw = PART(draw_direct);
            } else {
                task->draw = PART(draw);
            }
            task->step = PART(step);
            task->destructor = PART(destroy);
            work->mode = SHUT;
            HATCH(task)->collision = PART(collision);
            collision = &HATCH(task)->collision;
            collision->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
            collision->ang[2] = 0;
            collision->ang[0] = 0;
            collision->ang[1] = work->ang.y;
            collision->pos[0] = work->pos.x;
            collision->pos[1] = work->pos.y;
            collision->pos[2] = work->pos.z;
            radius = collision->model->r + CLASS_MARGIN;
            flags = WORLD_SOLID;
            if (!(radius > WORLD_SIZE_CLASS_1_MAX)) {
                if (radius > WORLD_SIZE_CLASS_2_MAX) {
                    flags = WORLD_SIZE_CLASS(1) | WORLD_SOLID;
                } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
                    flags = WORLD_SIZE_CLASS(2) | WORLD_SOLID;
                } else {
                    flags = WORLD_SIZE_CLASS(3) | WORLD_SOLID;
                }
            }
            world_add(flags, task, collision);
            field_add(task, PART(field));
        }
    }
}

static void PART(destroy)(struct task *task)
{
    if (HATCH(task)->collision.model) {
        world_remove(task, &HATCH(task)->collision);
    }
    field_remove(task);
    syFree(HATCH(task));
    SET_HATCH(task, NULL);
}

static void PART(step)(struct task *task)
{
    struct task_work *work = task->work;
#ifndef WALL_HATCH
    NJS_POINT3 pos;
    NJS_VECTOR speed;
    NJS_POINT3 preview[2];
    NJS_POINT3 air;
    struct angles ang;
#endif
    int charge_id;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for its strings, the set
     * editor's labels, and for the frame the floor's preview gives the step */
    if (0) {
        (void)"<- ID";
        (void)"<- ANGLE Y";
#ifndef WALL_HATCH
        (void)preview;
#endif
    }
    if ((work->mode == SHUT || HATCH(task)->depth < CLEARANCE) &&
        player_near(&work->pos, LID_RANGE)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((charge_id = CHARGE_ID(work)) != 0 && !dynamite_count(charge_id)) {
        if (work->mode == SHUT) {
#ifdef WALL_HATCH
            NJS_POINT3 pos;
            NJS_VECTOR speed;
            speed.y = speed.z = 0.0F;
            speed.x = BURST_SPEED;
#else
            speed.x = speed.z = 0.0F;
            speed.y = BURST_SPEED;
#endif
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            njCalcPoint(NULL, &PART(lid_model)->center, &pos);
            njCalcVector(NULL, &speed, &speed);
            njPopMatrixEx();
            burst_create_wobbling(&pos, &speed, BURST_SCALE);
            HATCH(task)->sink_speed = SINK_START_SPEED;
            HATCH(task)->bounce = 0.0F;
            HATCH(task)->bounce_speed = 0.0F;
        }
        work->mode = OPEN;
    } else {
        work->mode = SHUT;
        HATCH(task)->bounce = 0.0F;
        HATCH(task)->bounce_speed = 0.0F;
        HATCH(task)->sink_speed = 0.0F;
        HATCH(task)->depth = 0.0F;
        HATCH(task)->phase[0] = 0;
        HATCH(task)->phase[1] = 0;
#ifdef WALL_HATCH
        if (task->child) {
            task_kill(task->child);
        }
#endif
    }
    if (work->mode != OPEN) {
        return;
    }
    sound_play_at_for(OPEN_SOUND, task, OPEN_SOUND_PRIORITY, OPEN_SOUND_VOLUME,
                      OPEN_SOUND_UPDATES, &work->pos);
    HATCH(task)->sink_speed =
        (HATCH(task)->sink_speed * SINK_DRAG) + PART(acceleration);
    HATCH(task)->depth += HATCH(task)->sink_speed;
    if (HATCH(task)->depth > DEPTH_MAX) {
        HATCH(task)->depth = DEPTH_MAX;
    }
    HATCH(task)->bounce += HATCH(task)->bounce_speed;
    HATCH(task)->bounce_speed =
        ((SPRING_TOP - HATCH(task)->bounce) * SPRING_PULL) +
        (HATCH(task)->bounce_speed * SPRING_DRAG);
    if (HATCH(task)->bounce > SPRING_TOP) {
        HATCH(task)->bounce = SPRING_TOP;
        HATCH(task)->bounce_speed =
            -(njAbs(HATCH(task)->bounce_speed) * SPRING_REBOUND);
    }
    HATCH(task)->phase[0] = (Angle)(HATCH(task)->depth * FIRST_PHASE_RATE);
    HATCH(task)->phase[1] = (Angle)(HATCH(task)->depth * SECOND_PHASE_RATE);
    if (!task->child) {
#ifdef WALL_HATCH
        NJS_POINT3 air;
        struct angles ang;
        air.x = WALL_AIR_X;
        air.y = WALL_AIR_Y;
        air.z = 0.0F;
#else
        air.x = 0.0F;
        air.y = FLOOR_AIR_Y;
        air.z = 0.0F;
#endif
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &air, &air);
        njPopMatrixEx();
#ifdef WALL_HATCH
        ang.z = ang.x = 0;
        ang.y = work->ang.y + QUARTER_TURN;
#else
        ang.x = -QUARTER_TURN;
        ang.y = ang.z = 0;
#endif
        hatch_air_create(&air, &ang, air_length, air_specks, FALSE, air_size,
                         task);
    }
}
