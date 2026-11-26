/* the balloon, BALOON in the cart's object list. a touch by any racer pops it,
 * but only a player's kart gets its reward */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "cart/objects.h"
#include "cart/vehicle.h"
#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "hud/hud.h"
#include "object/set.h"
#include "shared/item_balloon.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* a balloon's modes, in its work's mode. nothing in this source puts a balloon
 * in MODE_KILL */
enum balloon_mode {
    MODE_INIT,
    MODE_RESET,
    MODE_GROW,
    MODE_WAIT,
    MODE_POP, /* it grows and fades */
    MODE_GONE,
    MODE_KILL
};

/* a balloon's reward, in its placement's ang.x. any other value draws one of
 * the REWARD_KINDS rewards from REWARD_5_RINGS on, at random */
enum balloon_reward {
    REWARD_RANDOM,
    REWARD_5_RINGS,
    REWARD_10_RINGS,
    REWARD_20_RINGS,
    REWARD_INVINCIBILITY
};
#define REWARD_KINDS 4

/* a growing balloon's scale grows from 0 to 1 over GROW_STEPS steps. a waiting
 * one's x and z scales wobble about 1 by up to WOBBLE. they follow the sine and
 * cosine of an angle that turns WOBBLE_STEP a step */
#define GROW_STEPS 30.0F
#define WOBBLE 0.02F
#define WOBBLE_STEP 256
/* a popped balloon grows by POP_GROWTH a step for POP_STEPS steps. its alpha
 * falls in a line from POP_ALPHA to 0. it then stays gone for RESPAWN_STEPS */
#define POP_GROWTH 0.03F
#define POP_STEPS 60
#define POP_ALPHA 0.7F
#define RESPAWN_STEPS 3600

#define BREAK_SOUND 0x800d
#define VIBRATION_POWER 1
#define VIBRATION_FREQ 48

/* clang-format off */
static struct collision_shape shape[] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_PLAYERS | PUSH_ENEMIES | PUSH_PICKUPS), 0,
     SHAPE_IGNORE_PLAYERS | SHAPE_IGNORE_PICKUPS, {0, -4.13F, 0}, 38, 0, 0, 0,
     {0, 0, 0}}};
/* clang-format on */
/* the rings of REWARD_5_RINGS to REWARD_20_RINGS */
static Sint32 rewards[] = {5, 10, 20};
static void balloon_draw(struct task *task);
static void balloon_destroy(struct task *task);

void cart_balloon_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *other;
    struct cart_vehicle *kart;
    int i;
    int item;
    /* @hack spelling: amount, step and full, locals for WOBBLE, WOBBLE_STEP
     * and 1, where the constants in place would do */
    Float amount;
    Float step;
    Float full;
    if (cart_out_of_range(&work->pos) && work->mode != MODE_POP) {
        /* @hack spelling: a goto to the function's end, where return would
         * do */
        goto done;
    }
    amount = WOBBLE;
    full = 1;
    step = WOBBLE_STEP;
    switch (work->mode) {
    case MODE_INIT:
        collision_init(task, shape, 1, COLLISION_OBJECT_MUTUAL);
        task->destructor = balloon_destroy;
        task->draw = balloon_draw;
        /* fall through */
    case MODE_RESET:
        work->scl.x = 0;
        work->scl.y = 0;
        work->scl.z = 0;
        work->mode = MODE_GROW;
        /* fall through */
    case MODE_GROW:
        work->scl.x += 1.0F / GROW_STEPS;
        work->scl.y += 1.0F / GROW_STEPS;
        work->scl.z += 1.0F / GROW_STEPS;
        if (work->scl.x >= full) {
            work->mode = MODE_WAIT;
        }
        collision_register(task);
        break;
    case MODE_WAIT:
        if (work->collision->flags & COLLISION_TOUCHED) {
            for (i = 0; i < work->collision->hit_count; i++) {
                other = work->collision->hits[i].task;
                if (cart_is_racer(other)) {
                    kart = other->motion.p;
                    work->mode = MODE_POP;
                    work->timer = 0;
                    if (kart->driver >= 0) {
                        if (kart->start.player_controlled) {
                            vibration_start(kart->player, VIBRATION_POWER,
                                            VIBRATION_FREQ, VIBRATION_UNTIMED);
                            switch (work->ang.x) {
#ifdef NONMATCHING
                            /* this side repairs the bug tagged in the matching
                             * side: a set reward adds its own rings alone */
                            case REWARD_5_RINGS:
                            case REWARD_10_RINGS:
                            case REWARD_20_RINGS:
                                item = work->ang.x - REWARD_5_RINGS;
                                kart->boost_rings += rewards[item];
                                ring_add(kart->driver, rewards[item]);
                                break;
#else
                            /* @bug uninitialized: a set reward adds
                             * rewards[item] rings besides its own, but only a
                             * random reward sets item. 0x8c509936 reads item
                             * unset before the switch, and the set rewards'
                             * cases add rewards[item] at 0x8c509952,
                             * 0x8c509968 and 0x8c5099c0 */
                            case REWARD_5_RINGS:
                                ring_add(kart->driver, rewards[item]);
                                kart->boost_rings += 5;
                                ring_add(kart->driver, 5);
                                break;
                            case REWARD_10_RINGS:
                                ring_add(kart->driver, rewards[item]);
                                kart->boost_rings += 10;
                                ring_add(kart->driver, 10);
                                break;
                            case REWARD_20_RINGS:
                                ring_add(kart->driver, rewards[item]);
                                kart->boost_rings += 20;
                                ring_add(kart->driver, 20);
                                break;
#endif
                            case REWARD_INVINCIBILITY:
                                cart_grant_invincibility(other);
                                break;
                            default:
                                item = rand() * (REWARD_KINDS * RAND_SCALE);
                                if (item ==
                                    REWARD_INVINCIBILITY - REWARD_5_RINGS) {
                                    cart_grant_invincibility(other);
                                } else {
                                    ring_add(kart->driver, rewards[item]);
                                    kart->boost_rings += rewards[item];
                                }
                                break;
                            }
                        }
                        sound_play(BREAK_SOUND, NULL, 0, 0);
                    }
                }
            }
        }
        /* @hack redundant: a test of item no path reaches */
        if (0) {
            if (item) {
            }
        }
        work->scl.x = (amount * njSin((Uint32)work->tick * step)) + full;
        work->scl.y = full;
        work->scl.z = (amount * njCos((Uint32)work->tick * step)) + full;
        work->tick++;
        collision_register(task);
        break;
    case MODE_POP:
        work->scl.x += POP_GROWTH;
        work->scl.y += POP_GROWTH;
        work->scl.z += POP_GROWTH;
        work->timer++;
        if (work->timer > POP_STEPS) {
            work->mode = MODE_GONE;
            work->timer = RESPAWN_STEPS;
        }
        break;
    case MODE_GONE:
        if (--work->timer == 0) {
            work->mode = MODE_RESET;
        }
        break;
    case MODE_KILL:
        if (task->placement) {
            placement_kill(task);
        } else {
            task_destroy(task);
        }
        break;
    }
done:
    return;
}
static void balloon_draw(struct task *task)
{
    struct task_work *work = task->work;
    if (cart_out_of_view(&work->pos) || work->mode == MODE_GONE) {
        return;
    }
    if (work->mode == MODE_POP) {
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(DRAW_CONTROL_CONSTANT);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        constant_material_set(
            (POP_STEPS - work->timer) * (POP_ALPHA / POP_STEPS), 1, 1, 1);
    }
    njPushMatrixEx();
    njSetTexture(&item_balloon_texlist);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njScaleEx(&work->scl);
    njCnkSimpleDrawObject(&item_balloon_object);
    njPopMatrixEx();
    if (work->mode == MODE_POP) {
        draw_control_restore();
        draw_attr_restore();
    }
}
static void balloon_destroy(struct task *task)
{
}
