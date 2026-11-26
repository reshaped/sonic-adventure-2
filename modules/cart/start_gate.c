/* the start gate, GATE in the cart's object list. it counts the race in, then
 * shows each new lap of the leading racer and the race's end */
#include <ninja.h>
#include <sg_xpt.h>

#include "cart/course_art.h"
#include "cart/objects.h"
#include "cart/race.h"
#include "cart/texture_remap.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"

/* the gate model's number and signal, which gate_draw retextures as it does
 * the lamps. they are start3 and red in cart_object_textures */
#define NUMBER_TEXTURE 72
#define SIGNAL_TEXTURE 41
/* the textures gate_draw shows in their places, each named for its texture */
#define START1_TEXTURE 70
#define START2_TEXTURE 71
#define GREEN_TEXTURE 15
#define GOALGOAL_TEXTURE 13
#define GOALGOAL2_TEXTURE 14
/* after the count, the number and the signal swap goalgoal and goalgoal2. they
 * swap as FLASH_BIT of the work's timer flips */
#define FLASH_BIT 0x8
/* the sounds of the count's three, two and one, and of its go */
#define COUNT_SOUND 24
#define GO_SOUND 25

static Bool need_links = 1;
static Sint32 last_lap;
/* the gate model's tree holds one of the three lamp nodes at a time.
 * lamp_parent is the node that links to it as a child, and lamp_predecessor
 * as a sibling. either may be null. link_lamps gives the other two nodes the
 * countdown lamp's child and sibling */
static NJS_CNK_OBJECT *lamp_parent, *lamp_predecessor;
static void gate_draw(struct task *task);
static void gate_destroy(struct task *task);

/* what the gate shows, in its work's tick. the count runs while the clock is
 * held. NEW_LAP shows for NEW_LAP_STEPS steps once the leading racer starts a
 * lap. the work's scl.z counts them down. a gate's placement picks what it
 * shows first by its scl.x. below RACING_FROM it shows the count, below
 * FINAL_FROM RACING, and from there FINAL_LAP */
enum gate_signal {
    COUNT_THREE,
    COUNT_TWO,
    COUNT_ONE,
    COUNT_GO,
    RACING,
    NEW_LAP,
    FINAL_LAP,
    FINISHED
};
#define NEW_LAP_STEPS 600.0F
#define NEW_LAP_LEFT(work) ((work)->scl.z)
#define RACING_FROM 1.0F
#define FINAL_FROM 2.0F
static void link_lamps(void)
{
    NJS_CNK_OBJECT *lamp = cart_countdown_lamp;
    lamp_parent = cart_find_link(FALSE, cart_gate_model, lamp);
    lamp_predecessor = cart_find_link(TRUE, cart_gate_model, lamp);
    cart_final_lamp->child = lamp->child;
    cart_final_lamp->sibling = lamp->sibling;
    cart_race_lamp->child = lamp->child;
    cart_race_lamp->sibling = lamp->sibling;
    need_links = 0;
}
struct task *cart_start_gate_create(NJS_POINT3 *pos, struct angles *ang)
{
    struct task *task =
        task_create(TASK_WORK, 2, cart_start_gate_step, "CartGateExec");
    task->draw = gate_draw;
    task->destructor = gate_destroy;
    task->work->pos = *pos;
    task->work->ang = *ang;
    return task;
}
void cart_start_gate_step(struct task *task)
{
    struct task_work *work = task->work;
    int lap;
    if (cart_out_of_range(&work->pos)) {
        return;
    }
    if (work->mode == 0) {
        if (need_links) {
            link_lamps();
        }
        work->mode++;
        if (work->scl.x < RACING_FROM) {
            work->tick = COUNT_THREE;
        } else if (work->scl.x < FINAL_FROM) {
            work->tick = RACING;
        } else {
            work->tick = FINAL_LAP;
        }
        work->submode = work->tick;
        task->draw = gate_draw;
        task->destructor = gate_destroy;
    } else {
        work->timer++;
    }
    if (cart_clock_held) {
        work->tick = COUNT_GO - cart_start_count;
        if (work->submode != work->tick) {
            work->submode = work->tick;
            if (cart_start_count) {
                sound_play(COUNT_SOUND, NULL, 0, 0);
            } else {
                sound_play(GO_SOUND, NULL, 0, 0);
            }
        }
        NEW_LAP_LEFT(work) = NEW_LAP_STEPS;
        last_lap = cart_highest_lap();
    } else if (cart_finished) {
        work->tick = FINISHED;
    } else {
        NEW_LAP_LEFT(work) -= 1.0F;
        lap = cart_highest_lap();
        if (NEW_LAP_LEFT(work) < 0.0F) {
            if (lap != CART_LAPS - 1) {
                work->tick = RACING;
            } else {
                work->tick = FINAL_LAP;
            }
        }
        if (lap != last_lap) {
            NEW_LAP_LEFT(work) = NEW_LAP_STEPS;
            work->tick = NEW_LAP;
        }
        last_lap = lap;
    }
}
#ifdef DREAMCAST
#pragma inline(set_lamp)
#endif
static void set_lamp(NJS_CNK_OBJECT *lamp)
{
    if (lamp_parent) {
        lamp_parent->child = lamp;
    }
    if (lamp_predecessor) {
        lamp_predecessor->sibling = lamp;
    }
}
#define SET_TEXTURE(model, original, texture)                                  \
    cart_replace_texture(cart_texture_remaps, model, original, texture)
#define COUNT_LAMPS(lamp, number, signal)                                      \
    do {                                                                       \
        set_lamp(cart_countdown_lamp);                                         \
        SET_TEXTURE(cart_gate_model, CART_LAMP_TEXTURE, lamp);                 \
        SET_TEXTURE(cart_gate_model, NUMBER_TEXTURE, number);                  \
        SET_TEXTURE(cart_gate_model, SIGNAL_TEXTURE, signal);                  \
    } while (0)
static void gate_draw(struct task *task)
{
    struct task_work *work = task->work;
    if (cart_out_of_view(&work->pos)) {
        return;
    }
    switch (work->tick) {
    case COUNT_THREE:
        COUNT_LAMPS(CART_START_TEXTURE, NUMBER_TEXTURE, SIGNAL_TEXTURE);
        break;
    case COUNT_TWO:
        COUNT_LAMPS(CART_START_TEXTURE, START2_TEXTURE, SIGNAL_TEXTURE);
        break;
    case COUNT_ONE:
        COUNT_LAMPS(CART_START_TEXTURE, START1_TEXTURE, SIGNAL_TEXTURE);
        break;
    case COUNT_GO:
        COUNT_LAMPS(CART_LAMP_TEXTURE, GREEN_TEXTURE, GREEN_TEXTURE);
        break;
    case RACING:
        set_lamp(cart_race_lamp);
        SET_TEXTURE(cart_race_lamp, CART_LAMP_TEXTURE, CART_START_TEXTURE);
        break;
    case NEW_LAP:
        set_lamp(cart_race_lamp);
        SET_TEXTURE(cart_race_lamp, CART_LAMP_TEXTURE, CART_LAMP_TEXTURE);
        break;
    case FINAL_LAP:
        set_lamp(cart_final_lamp);
        SET_TEXTURE(cart_final_lamp, CART_LAMP_TEXTURE, CART_START_TEXTURE);
        break;
    case FINISHED:
        set_lamp(cart_final_lamp);
        SET_TEXTURE(cart_final_lamp, CART_LAMP_TEXTURE, CART_LAMP_TEXTURE);
        break;
    }
    if (work->tick >= RACING) {
        if (work->timer & FLASH_BIT) {
            SET_TEXTURE(cart_gate_model, NUMBER_TEXTURE, GOALGOAL_TEXTURE);
            SET_TEXTURE(cart_gate_model, SIGNAL_TEXTURE, GOALGOAL_TEXTURE);
        } else {
            SET_TEXTURE(cart_gate_model, NUMBER_TEXTURE, GOALGOAL2_TEXTURE);
            SET_TEXTURE(cart_gate_model, SIGNAL_TEXTURE, GOALGOAL2_TEXTURE);
        }
    }
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&cart_object_textures);
    njCnkEasyDrawObject(cart_gate_model);
    njPopMatrixEx();
}
static void gate_destroy(struct task *task)
{
}
