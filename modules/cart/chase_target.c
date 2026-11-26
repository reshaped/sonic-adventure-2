/* what a story race chases: the limousine on Tails's course, the Tornado on
 * Rouge's. the racer that closes the target's lead to CAUGHT_LEAD clears the
 * race */
#include <ninja.h>
#include <sg_xpt.h>

#include "cart/kart_art.h"
#include "cart/course.h"
#include "cart/objects.h"
#include "cart/race.h"
#include "cart/vehicle.h"
#include "sound/adx.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/texture.h"

/* the target's lead along x on its racer, kept in its work's scl.x. the lead
 * starts at LEAD_START. each step adds TARGET_SPEED less the racer's speed
 * along x, up to LEAD_MAX. once the race is clear, the target holds CLEAR_LEAD
 * ahead */
#define LEAD(work) ((work)->scl.x)
#define LEAD_START 2000
#define LEAD_MAX 4000
#define CAUGHT_LEAD 100
#define CLEAR_LEAD 100
#define TARGET_SPEED 7.0F
/* the target starts by the point TARGET_LANE of its racer's gate. the
 * Tornado flies TORNADO_RISE above the racer as it starts */
#define TARGET_LANE 2
#define TORNADO_RISE 100
#define TAILS_CAUGHT_VOICE 0x701
#define ROUGE_CAUGHT_VOICE 0x708

static Float flight_frame = 0;
static void chase_step(struct task *task);
static void chase_draw(struct task *task);
static void chase_destroy(struct task *task);

struct task *cart_create_chase_target(struct task *racer)
{
    struct cart_vehicle *kart = racer->motion.p;
    struct task *task;
    NJS_POINT3 pos;
    task = task_create(TASK_WORK, 2, chase_step, "CartLimousineExec");
    task->draw = chase_draw;
    task->destructor = chase_destroy;
    pos.x = cart_gates[kart->gate][TARGET_LANE].x;
    pos.y = racer->work->pos.y;
    pos.z = cart_gates[kart->gate][TARGET_LANE].z;
    task->motion.p = racer->work;
    task->extra.p = racer->motion.p;
    LEAD(task->work) = LEAD_START;
    task->work->pos = pos;
    task->work->pos.x += LEAD(task->work);
    if (cart_setup.course_mode == CART_ROUGE) {
        task->work->pos.y += TORNADO_RISE;
    }
    return task;
}
static void chase_step(struct task *task)
{
    struct task_work *work = task->work;
    struct cart_vehicle *kart = task->extra.p;
    if (cart_race.countdown <= 0) {
        return;
    }
    if (cart_story_clear) {
        work->pos.x = kart->previous.pos.x + CLEAR_LEAD;
    } else {
        /* @hack spelling: the lead read and written through task->work, here
         * and below, where work would do */
        LEAD(task->work) += TARGET_SPEED - kart->velocity.x;
        if (LEAD(task->work) > LEAD_MAX) {
            LEAD(task->work) = LEAD_MAX;
        } else if (LEAD(task->work) < CAUGHT_LEAD) {
            /* @hack redundant: a test that cart_story_clear isn't 1, which
             * holds in this arm, where it is 0 */
            if (cart_story_clear != 1) {
                switch (cart_setup.course_mode) {
                case CART_TAILS:
                    voice_play_priority_2(TAILS_CAUGHT_VOICE);
                    break;
                case CART_ROUGE:
                    voice_play_priority_2(ROUGE_CAUGHT_VOICE);
                    break;
                }
            }
            cart_story_clear = 1;
        }
        work->pos.x = kart->previous.pos.x + LEAD(task->work);
    }
    task->draw = chase_draw;
    task->destructor = chase_destroy;
}
static void chase_draw(struct task *task)
{
    struct task_work *work = task->work;
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    if (cart_setup.course_mode == CART_TAILS) {
        njRotateY(NULL, -QUARTER_TURN);
        njSetTexture(&cart_limo_textures);
        njCnkSimpleDrawObject(&cart_limo_model);
    } else {
        njRotateY(NULL, HALF_TURN);
        njSetTexture(&cart_tornado_textures);
        njCnkSimpleDrawMotion(&cart_tornado_model, cart_tornado_motion,
                              flight_frame);
        flight_frame += 1;
        if (flight_frame >= (Float)(cart_tornado_motion->nbFrame - 1)) {
            flight_frame = 0;
        }
    }
    njPopMatrixEx();
}
static void chase_destroy(struct task *task)
{
    /* motion and extra hold the racer's work and kart, which task_destroy
     * would otherwise free */
    task->motion.p = NULL;
    task->extra.p = NULL;
}
void cart_load_chase_textures(void)
{
    if (cart_setup.course_mode == CART_TAILS) {
        njLoadTexturePvmFile("cartLim.pvm", &cart_limo_textures);
    } else {
        texture_load_pvm("limtails", &cart_tornado_textures);
    }
}
void cart_release_chase_textures(void)
{
    if (cart_setup.course_mode == CART_TAILS) {
        njReleaseTexture(&cart_limo_textures);
    } else {
        njReleaseTexture(&cart_tornado_textures);
    }
}
