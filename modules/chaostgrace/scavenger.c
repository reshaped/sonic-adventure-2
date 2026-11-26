/* the scavenger hunt's signs and answer panels. the two signs show the kind
 * of toy to find. each lane's panel judges the toy its racer brings. a wrong
 * toy drops an Omochao on the racer. scavenger_questions[0] is the signs'
 * picture before the question, and [kind + 1] each kind's */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#ifdef NONMATCHING
/* chao_set_behavior and race_run, by which this side's racers run on past a toy
 * that failed to spawn */
#include "chao/behavior.h"
#include "chaostgrace/race.h"
#endif
#include "chao/garden_field.h"
#include "chao/model.h"
#include "chao/toy_art.h"
#include "chao/work.h"
#include "chao/world.h"
#include "chaostgrace/hunt_data.h"
#include "chaostgrace/props.h"
#include "chaostgrace/scavenger.h"
#include "collision/collision.h"
#include "object/mover.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

/* the panels and signs are plain Ninja objects */
#ifdef NONMATCHING
/* this side repairs the bug tagged in the matching side by
 * garden_animate_texture. that function sets the mesh's textures as a scenery
 * mesh's */
#define SET_PANEL_TEXTURE(panel, texture)                                      \
    garden_animate_texture((panel)->model, (texture))
#else
/* @bug memory: garden_set_model_texture takes the signs' and the panels'
 * plain models for garden models, and reads saved_textures past their end,
 * at 0x0c510130. question_step passes it a sign at 0x0c9c4110, and
 * answer_step a panel at 0x0c9c4596 */
#define SET_PANEL_TEXTURE garden_set_model_texture
#endif

struct task *race_scavenger_items[SCAVENGER_ITEMS];
Sint32 race_scavenger_target;
static struct task *occupants[SCAVENGER_LANES];
static struct task *waiting[SCAVENGER_LANES];
#ifdef NONMATCHING
/* an item whose toy failed to spawn, or died before a racer took it. a toy a
 * racer takes is never missing: it spawns again once let go */
static Bool missing[SCAVENGER_ITEMS];
static Bool panels_ready;
#endif

enum question_mode { QUESTION_START, QUESTION_HIDDEN, QUESTION_SHOWN };
enum penalty_mode { PENALTY_FALLING, PENALTY_BOUNCED };
enum lane_mode { LANE_CLOSED, LANE_OPEN, LANE_JUDGING, LANE_JUDGED };
/* scavenger_answers' pictures, by what a panel shows */
enum { PANEL_BLANK, PANEL_RIGHT, PANEL_WRONG };
/* the question shows after more than QUESTION_DELAY frames, and the lanes
 * open after more than OPEN_DELAY */
#define QUESTION_DELAY 300
#define OPEN_DELAY 1800
/* a lane judges a toy JUDGE_DELAY frames after its racer submits it. the
 * verdict blinks VERDICT_FRAMES frames, and the lane's waiting racer comes
 * forward NEXT_RACER frames into the verdict */
#define JUDGE_DELAY 60
#define VERDICT_FRAMES 300
#define NEXT_RACER 150
#define RIGHT_SOUND 3
#define WRONG_SOUND 4
/* a right toy's field, RACE_FIELD_RIGHT_ANSWER, has RIGHT_RADIUS and lasts
 * RIGHT_FRAMES. a wrong one's Omochao drops from PENALTY_HEIGHT above the
 * panel */
#define RIGHT_RADIUS 3.0F
#define RIGHT_FRAMES 10
#define PENALTY_HEIGHT 30.0F
/* a falling Omochao bounces off the first racer it meets, at PENALTY_BOUNCE
 * of its speed and PENALTY_SCATTER aside. it vanishes more than
 * PENALTY_FRAMES frames later */
#define PENALTY_BOUNCE (-0.6F)
#define PENALTY_SCATTER 0.5F
#define PENALTY_FRAMES 60
#define PENALTY_SCALE 1.2F
#define SHADOW_DROP (-1.0F)
#define SHADOW_WIDTH 1.3F
#define SHADOW_HEIGHT 0.7F
/* a blinking picture shows BLINK_SHOWN frames of every BLINK_PERIOD */
#define BLINK_PERIOD 60
#define BLINK_SHOWN 30
#define BLINK_ON(timer) ((timer) % BLINK_PERIOD < BLINK_SHOWN)
/* a panel's lane, in its work */
#define PANEL_LANE(work) ((work)->tick)

static void choose_target(void)
{
    /* @hack spelling: the store through (&race_scavenger_target)[0], where
     * race_scavenger_target = would do */
    (&race_scavenger_target)[0] =
        (Uint32)(rand() * (TOY_KIND_DRAW * RAND_SCALE));
}

static void question_step(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case QUESTION_START:
        SET_PANEL_TEXTURE(scavenger_sign_a, scavenger_questions[0]);
        SET_PANEL_TEXTURE(scavenger_sign_b, scavenger_questions[0]);
        work->mode++;
        work->submode = 0;
        break;
    case QUESTION_HIDDEN:
        if (work->timer > QUESTION_DELAY) {
            SET_PANEL_TEXTURE(scavenger_sign_a,
                              scavenger_questions[race_scavenger_target + 1]);
            SET_PANEL_TEXTURE(scavenger_sign_b,
                              scavenger_questions[race_scavenger_target + 1]);
            /* @hack redundant: an empty if (1) with an empty else */
            if (1) {
            } else {
            }
            work->mode++;
            work->submode = 0;
        }
        break;
    }
    work->timer++;
}

static void question_draw(struct task *task)
{
    struct task_work *work = task->work;
    njSetTexture(&race_fruit_texlist);
    if (BLINK_ON(work->timer)) {
        njCnkEasyDrawObject(scavenger_sign_a);
        njCnkEasyDrawObject(scavenger_sign_b);
    }
}

static void question_destroy(struct task *task)
{
}

static void penalty_step(struct task *task)
{
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;
    Angle yaw;
    switch (work->mode) {
    case PENALTY_FALLING:
        if (collision_first_with_id(task, RACE_FIELD_CHAO)) {
            if (mover->spd.y < 0.0F) {
                yaw = RANDOM_TURN();
                mover->spd.y *= PENALTY_BOUNCE;
                mover->spd.x = njSin(yaw) * PENALTY_SCATTER;
                mover->spd.z = njCos(yaw) * PENALTY_SCATTER;
            }
            work->mode++;
        }
        break;
    case PENALTY_BOUNCED:
        if (work->timer++ > PENALTY_FRAMES) {
            task_kill(task);
        }
        break;
    }
    mover_step(task);
    mover_meet_stage(task);
    collision_register(task);
}

static void penalty_draw(struct task *task)
{
    struct task_work *work = task->work;
    njSetTexture(&chao_body_texlist);
    njPushMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njScale(NULL, PENALTY_SCALE, PENALTY_SCALE, PENALTY_SCALE);
    njCnkEasyDrawObject(&race_omo_model);
    njTranslate(NULL, 0.0F, SHADOW_DROP, 0.0F);
    njScale(NULL, SHADOW_WIDTH, SHADOW_HEIGHT, SHADOW_WIDTH);
    njCnkModDrawModel(&garden_shadow_model);
    njPopMatrix(1);
}

static void penalty_destroy(struct task *task)
{
}

#ifdef NONMATCHING
/* this side returns whether it dropped the penalty */
static Bool drop_penalty(NJS_POINT3 *position)
#else
static void drop_penalty(NJS_POINT3 *position)
#endif
{
    struct task *task =
        task_create(TASK_WORK, 4, penalty_step, scavenger_penalty_name);
#ifdef NONMATCHING
    /* this side reads the task's work only once the test below finds the task
     * made */
    struct task_work *work;
#else
    struct task_work *work = task->work;
#endif
    struct mover *mover;

#ifdef NONMATCHING
    /* this side returns FALSE where task_create fails. the matching side writes
     * through the null */
    if (!task) {
        return FALSE;
    }
    work = task->work;
#endif
    task->destructor = penalty_destroy;
    task->draw = penalty_draw;
#ifdef NONMATCHING
    /* this side destroys the penalty and returns FALSE where collision_init
     * fails */
    if (!collision_init(task, scavenger_penalty_shape, 1, COLLISION_OBJECT)) {
        task_destroy(task);
        return FALSE;
    }
#else
    collision_init(task, scavenger_penalty_shape, 1, COLLISION_OBJECT);
#endif
    mover = mover_alloc(task);
#ifdef NONMATCHING
    /* this side destroys the penalty and returns FALSE where mover_alloc fails.
     * the matching side writes through the null */
    if (!mover) {
        task_destroy(task);
        return FALSE;
    }
#endif
    work->pos = *position;
    work->ang.y = RANDOM_TURN();
    mover_set_gravity(task, -0.05F);
    mover->probe_height = 3.0F;
    mover->floor_bounce = 0.0F;
    mover->friction = 0.7F;
    mover->hold_turn = 0xd00;
#ifdef NONMATCHING
    /* this side returns that it dropped the penalty */
    return TRUE;
#endif
}

Bool race_join_scavenger_lane(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    int lane;
    for (lane = 0; lane < SCAVENGER_LANES; lane++) {
        if (occupants[lane] == NULL) {
            occupants[lane] = chao;
            work->race_slot = lane;
            ((struct mover *)chao->motion.p)->target =
                scavenger_lane_positions[lane];
            return TRUE;
        }
    }
    for (lane = 0; lane < SCAVENGER_LANES; lane++) {
        if (waiting[lane] == NULL) {
            waiting[lane] = chao;
            work->race_slot = lane;
            ((struct mover *)chao->motion.p)->target =
                scavenger_lane_positions[lane];
            return FALSE;
        }
    }
    /* no place is free. the image's code returns no value here, which no
     * caller reads */
#ifdef NONMATCHING
    /* this side returns FALSE, where the image returns no value */
    return FALSE;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

struct task *race_scavenger_occupant(Sint32 lane)
{
    return occupants[lane];
}

void race_advance_scavenger_lane(int lane)
{
    occupants[lane] = waiting[lane];
    waiting[lane] = NULL;
}

#define CHECK_ANSWER(task, work)                                               \
    do {                                                                       \
        collision_first_with_id((task), RACE_FIELD_ENTRANT);                   \
        if (race_scavenger_submitted[PANEL_LANE(work)]) {                      \
            race_scavenger_submitted[PANEL_LANE(work)] = FALSE;                \
            SET_PANEL_TEXTURE(scavenger_panels[PANEL_LANE(work)],              \
                              scavenger_answers[PANEL_BLANK]);                 \
            (work)->mode = LANE_JUDGING;                                       \
            (work)->submode = 0;                                               \
            (work)->timer = 0;                                                 \
        }                                                                      \
    } while (0)

static void answer_step(struct task *panel)
{
    /* @hack redundant: task, a copy of panel */
    struct task *task = panel;
    struct task_work *work = task->work;
    struct garden_entry *entry;
    struct task *toy;
    NJS_POINT3 position;
    switch (work->mode) {
    case LANE_CLOSED:
        if (work->timer++ > OPEN_DELAY) {
            work->timer = 0;
            work->mode = LANE_OPEN;
            work->submode = 0;
        }
        return;
    case LANE_OPEN:
        CHECK_ANSWER(task, work);
        break;
    case LANE_JUDGING:
        if (work->timer++ > JUDGE_DELAY) {
            entry = garden_mutual_partner(occupants[PANEL_LANE(work)]);
            if (entry) {
                toy = entry->task;
                if (toy) {
                    if (TOY_KIND(toy->work) != race_scavenger_target) {
                        position = work->pos;
                        position.y += PENALTY_HEIGHT;
#ifdef NONMATCHING
                        /* race_submit_answer waits for the falling
                         * Omochao's RACE_FIELD_PENALTY, so where none
                         * drops, the racer runs on */
                        if (!drop_penalty(&position)) {
                            chao_set_behavior(occupants[PANEL_LANE(work)],
                                              race_run);
                        }
#else
                        drop_penalty(&position);
#endif
                        SET_PANEL_TEXTURE(scavenger_panels[PANEL_LANE(work)],
                                          scavenger_answers[PANEL_WRONG]);
                        sound_play(WRONG_SOUND, NULL, 0, 0);
                    } else {
#ifdef NONMATCHING
                        if (!garden_timed_enemy_field(RACE_FIELD_RIGHT_ANSWER,
                                                      &work->pos, RIGHT_RADIUS,
                                                      RIGHT_FRAMES)) {
                            /* race_submit_answer waits for the field, so
                             * the lane judges again next frame */
                            work->timer--;
                            break;
                        }
                        SET_PANEL_TEXTURE(scavenger_panels[PANEL_LANE(work)],
                                          scavenger_answers[PANEL_RIGHT]);
#else
                        SET_PANEL_TEXTURE(scavenger_panels[PANEL_LANE(work)],
                                          scavenger_answers[PANEL_RIGHT]);
                        garden_timed_enemy_field(RACE_FIELD_RIGHT_ANSWER,
                                                 &work->pos, RIGHT_RADIUS,
                                                 RIGHT_FRAMES);
#endif
                        sound_play(RIGHT_SOUND, NULL, 0, 0);
                    }
                }
            }
            work->mode = LANE_JUDGED;
            work->submode = 0;
        }
        break;
    case LANE_JUDGED:
        if (work->timer++ > VERDICT_FRAMES) {
            work->timer = 0;
            work->mode = LANE_OPEN;
            work->submode = 0;
        }
        if (work->timer == NEXT_RACER) {
            race_advance_scavenger_lane(PANEL_LANE(work));
        }
        CHECK_ANSWER(task, work);
        break;
    }
    collision_register(task);
}

static void answer_draw(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case LANE_CLOSED:
        return;
    case LANE_JUDGING:
        njSetTexture(&race_fruit_texlist);
        njCnkEasyDrawObject(scavenger_panels[PANEL_LANE(work)]);
        break;
    case LANE_JUDGED:
        if (BLINK_ON(work->timer)) {
            njSetTexture(&race_fruit_texlist);
            njCnkEasyDrawObject(scavenger_panels[PANEL_LANE(work)]);
        }
        break;
    }
}

static void answer_destroy(struct task *task)
{
}

void race_scavenger_start(void)
{
    struct task *task;
    int i;
#ifdef NONMATCHING
    /* this side empties the lanes and the toys' slots before it makes the
     * panels. the panels set panels_ready once all are made */
    struct task *question;
    struct task *panels[SCAVENGER_LANES];
    panels_ready = FALSE;
    for (i = 0; i < SCAVENGER_LANES; ++i) {
        occupants[i] = NULL;
        waiting[i] = NULL;
        race_scavenger_submitted[i] = FALSE;
    }
    for (i = 0; i < SCAVENGER_ITEMS; ++i) {
        missing[i] = FALSE;
        race_scavenger_items[i] = NULL;
    }
#endif
    task = task_create(TASK_WORK, 2, question_step, scavenger_question_name);
#ifdef NONMATCHING
    /* this side sets up the question only where task_create made it. the
     * matching side writes through the null */
    question = task;
    if (task)
#endif
    {
        task->draw = question_draw;
        task->destructor = question_destroy;
    }
    for (i = 0; i < SCAVENGER_LANES; i++) {
        task = task_create(TASK_WORK, 2, answer_step, scavenger_answer_name);
#ifdef NONMATCHING
        /* this side returns where a panel or its shape can't be made. it
         * destroys the question and the panels made so far */
        if (!task || !collision_init(task, scavenger_panel_shape, 1,
                                     COLLISION_OBJECT_MUTUAL)) {
            if (task) {
                task_destroy(task);
            }
            while (i) {
                task_destroy(panels[--i]);
            }
            if (question) {
                task_destroy(question);
            }
            return;
        }
        task->draw = answer_draw;
        task->destructor = answer_destroy;
#else
        task->draw = answer_draw;
        task->destructor = answer_destroy;
        collision_init(task, scavenger_panel_shape, 1, COLLISION_OBJECT_MUTUAL);
#endif
        PANEL_LANE(task->work) = i;
        task->work->pos = scavenger_lane_positions[i];
#ifdef NONMATCHING
        /* this side keeps each panel for a failure, and has emptied the lanes
         * above */
        panels[i] = task;
#else
        race_scavenger_submitted[i] = FALSE;
        occupants[i] = NULL;
        waiting[i] = NULL;
#endif
    }
#ifdef NONMATCHING
    /* this side marks the panels made, which race_retry_scavenger_items waits
     * for */
    panels_ready = TRUE;
#endif
    choose_target();
    for (i = 0; i < SCAVENGER_ITEMS; i++) {
        race_respawn_scavenger_item(i);
    }
}

void race_respawn_scavenger_item(int index)
{
    NJS_VECTOR velocity;
    velocity = scavenger_drop_velocity;
    race_scavenger_items[index] = race_scavenger_item_create(
        index, &scavenger_item_positions[index], RANDOM_TURN(), &velocity);
#ifdef NONMATCHING
    /* this side marks a toy that failed to spawn, for
     * race_retry_scavenger_items to spawn again */
    missing[index] = race_scavenger_items[index] == NULL;
#endif
}

#ifdef NONMATCHING
/* this side marks a toy missing as it dies, and spawns each missing toy again.
 * race_retry_scavenger_items returns whether any was missing */
void race_forget_scavenger_item(struct task *toy)
{
    int index = toy->work->ang.x;
    if (race_scavenger_items[index] == toy) {
        race_scavenger_items[index] = NULL;
        missing[index] = TRUE;
    }
}

Bool race_retry_scavenger_items(void)
{
    int i;
    Bool shortage = FALSE;
    if (!panels_ready) {
        return TRUE;
    }
    for (i = 0; i < SCAVENGER_ITEMS; ++i) {
        if (missing[i]) {
            race_respawn_scavenger_item(i);
            shortage = TRUE;
        }
    }
    return shortage;
}
#endif
