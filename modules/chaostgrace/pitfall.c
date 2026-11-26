/* the eight pitfalls of a course on the neutral land. a racer may fall in
 * only on TRACK_JWL_IL and TRACK_JWL_LONG, at odds its luck lowers.
 * race_choose_pitfall draws a pit for a racer's lane from those not drawn
 * yet. it returns 0 once all are drawn */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "chao/behavior.h"
#include "chao/life.h"
#include "chao/race.h"
#include "chaostgrace/course_models.h"
#include "chaostgrace/grid.h"
#include "chaostgrace/props.h"
#include "chaostgrace/race.h"
#include "chaostgrace/racer.h"
#include "collision/collision.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

enum { PITFALLS = 8 };
enum pitfall_mode { PITFALL_READY, PITFALL_PASSED, PITFALL_OPEN };
/* a racer falls in at odds of FALL_ODDS - FALL_LUCK * luck in FALL_DRAWS. it
 * then lies in the pit FALL_FRAMES frames */
#define FALL_ODDS 45000
#define FALL_LUCK 4
#define FALL_DRAWS 80000.0F
#define FALL_FRAMES 180
static Sint8 pitfall_count;
static Sint8 pitfall_used[PITFALLS];

#ifdef NONMATCHING
#include <stdint.h>
#include "system/areas.h"

/* this side reads the neutral land's texture list and covers in
 * ChaoStgRaceLandN.PRS, which lies at CHAO_RACE_LAND_AREA. it reads them
 * through that address with its top three bits clear, as the image does.
 * the texture list lies TEXLIST_OFFSET into the file, and the eight covers
 * from FIRST_COVER, COVER_STRIDE bytes apart */
#define TOP_BITS_CLEAR 0x1fffffffU
#define TEXLIST_OFFSET 0x0b7c
#define FIRST_COVER 0x2da88
#define COVER_STRIDE 0xccU
#define PITFALL_LAND_ADDRESS ((uintptr_t)CHAO_RACE_LAND_AREA & TOP_BITS_CLEAR)
#define PITFALL_TEXLIST ((NJS_TEXLIST *)(PITFALL_LAND_ADDRESS + TEXLIST_OFFSET))
#define PITFALL_LAND_COVER(lane)                                               \
    (*(NJS_CNK_OBJECT *)(PITFALL_LAND_ADDRESS + FIRST_COVER +                  \
                         (lane) * COVER_STRIDE))
#define race_pitfall_a PITFALL_LAND_COVER(0)
#define race_pitfall_b PITFALL_LAND_COVER(1)
#define race_pitfall_c PITFALL_LAND_COVER(2)
#define race_pitfall_d PITFALL_LAND_COVER(3)
#define race_pitfall_e PITFALL_LAND_COVER(4)
#define race_pitfall_f PITFALL_LAND_COVER(5)
#define race_pitfall_g PITFALL_LAND_COVER(6)
#define race_pitfall_h PITFALL_LAND_COVER(7)
#else
extern NJS_TEXLIST race_pitfall_texlist;
#define PITFALL_TEXLIST (&race_pitfall_texlist)
#endif

KEPT static NJS_CNK_OBJECT *pitfall_covers[PITFALLS] = {
#include "chaostgrace/pitfall/pitfall_covers.inc"
};

static NJS_POINT3 pitfall_positions[PITFALLS] = {
#include "chaostgrace/pitfall/pitfall_positions.inc"
};
/* @hack type: pitfall_shape an array of one, where one record would do */
/* clang-format off */
static const struct collision_shape pitfall_shape[] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNTURNED, {0.0F, 2.0F, 0.0F}, 3.0F, 0.0F, 0.0F, 0, {0, 0, 0}}};
/* clang-format on */

int race_choose_pitfall(void)
{
    int remaining = RANDOM_BELOW(PITFALLS - pitfall_count);
    int pit;
    for (pit = 0; pit < PITFALLS; pit++) {
        if (pitfall_used[pit] == 0) {
            if (!remaining) {
                pitfall_used[pit] = TRUE;
                pitfall_count++;
                return pit;
            }
            remaining--;
        }
    }
    return 0;
}

static void pitfall_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *chao;
    Uint16 luck;
    switch (work->mode) {
    case PITFALL_READY:
        chao = collision_first_with_id(task, RACE_FIELD_CHAO);
        if (chao) {
            luck = chao_skill_points(chao, CHAO_LUCK);
            if ((Float)(FALL_ODDS - (luck * FALL_LUCK)) >
                rand() * (FALL_DRAWS * RAND_SCALE)) {
                RACE_USE_STAGE(chao);
                chao_set_timed_behavior(chao, race_fall_in_pit, FALL_FRAMES);
                chao_queue_behavior(chao, race_run);
                work->mode = PITFALL_OPEN;
                work->submode = 0;
            } else {
                work->mode = PITFALL_PASSED;
                work->submode = 0;
            }
        }
        break;
    case PITFALL_PASSED:
        break;
    case PITFALL_OPEN:
        break;
    }
    collision_register(task);
}

static void pitfall_idle(struct task *task)
{
}

static void pitfall_draw(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case PITFALL_READY:
    case PITFALL_PASSED:
        njSetTexture(PITFALL_TEXLIST);
        draw_control_set(DRAW_CONTROL_SHADOW);
        njCnkEasyDrawObject(pitfall_covers[work->tick]);
        draw_control_clear(DRAW_CONTROL_SHADOW);
        break;
    case PITFALL_OPEN:
        break;
    }
}

static void pitfall_destroy(struct task *task)
{
}

void race_pitfalls_create(void)
{
    int pit;
    struct task *task;
    const struct garden_race_course *course;

    pitfall_count = 0;
    for (pit = 0; pit < PITFALLS; pit++) {
        pitfall_used[pit] = FALSE;
    }
    for (pit = 0; pit < PITFALLS; pit++) {
        task =
            task_create(TASK_WORK, 2, pitfall_step, "ALO_RacePitfallExecutor");
        task->draw = pitfall_draw;
        task->destructor = pitfall_destroy;
        task->work->tick = pit;
        task->work->pos = pitfall_positions[pit];
        course = garden_race_course();
        if (course->track != TRACK_JWL_IL && course->track != TRACK_JWL_LONG) {
            task->step = pitfall_idle;
        } else {
            collision_init(task, pitfall_shape, 1, COLLISION_OBJECT_MUTUAL);
        }
    }
}
