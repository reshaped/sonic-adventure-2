/* the garden's transfer machine. it sends the Chao the player puts in it to
 * a VMU. it delivers what a VMU sends back: a Chao, an egg and up to eight
 * seeds and fruit */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "chao/actor.h"
#include "chao/behavior.h"
#include "chao/egg.h"
#include "chao/fruit.h"
#include "chao/garden.h"
#include "chao/garden_scene.h"
#include "chao/life.h"
#include "chao/machine.h"
#include "chao/machine_art.h"
#include "chao/machine_collision.h"
#include "chao/orchard_art.h"
#include "chao/residents.h"
#include "chao/seed.h"
#include "chao/transfer.h"
#include "chao/work.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "object/mover.h"
#include "player/action.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"

enum machine_mode {
    MACHINE_RESET,
    MACHINE_WAIT,
    MACHINE_PACK,
    MACHINE_SEND_READY,
    MACHINE_UNUSED_4,
    MACHINE_SEND,
    MACHINE_SENT,
    MACHINE_UNUSED_7,
    MACHINE_RECEIVE_START,
    MACHINE_RECEIVE_READY,
    MACHINE_RECEIVE,
    MACHINE_RECEIVE_FAILED,
    MACHINE_OPEN_DELIVERY,
    MACHINE_RELEASE_CHAO,
    MACHINE_WAIT_CHAO,
    MACHINE_RELEASE_EGG,
    MACHINE_WAIT_ITEM,
    MACHINE_RELEASE_ITEM,
    MACHINE_UNUSED_18,
    MACHINE_OPEN_LOAD,
    MACHINE_LOAD,
    MACHINE_CLOSE_LOAD,
    MACHINE_REJECT,
    MACHINE_EJECT,
    MACHINE_FINISH
};
enum machine_door { DOOR_CLOSING, DOOR_SHUT, DOOR_OPENING, DOOR_OPEN };
enum machine_shape { MACHINE_BUTTON, MACHINE_INTAKE };
/* the index of each garden's machine in machine_art.h's tables */
enum machine_garden { NEUTRAL_MACHINE, HERO_MACHINE, DARK_MACHINE };
/* the machine's own work, in its task's extra */
struct machine_work {
    /* set once the button is down, and cleared once it is back up. a touch
     * presses the button only while this is clear */
    Bool pressed;
    enum machine_door door;
    Float door_open; /* from 0, shut, to 1, open */
    Float button_depth; /* the button's offset in y, from BUTTON_DOWN to 0 */
    /* the Chao the machine takes in, or the Chao or egg it delivers */
    struct task *chao;
    /* TRUE once the transfer's status is GARDEN_TRANSFER_DONE */
    Bool complete;
    Bool waited; /* the machine has spun for more than LEAST_SPIN steps */
    /* the angle of the pulse that swells the machine while it transfers */
    Angle spin;
    /* set to GARDEN_TRANSFER_DONE once a send succeeds. nothing reads it */
    Sint32 result;
    Uint8 unk_24[8];
};
/* the transfer under way, or null */
static struct garden_transfer *current_transfer;
#define MACHINE_MODE(work, next)                                               \
    {                                                                          \
        (work)->mode = (next);                                                 \
        (work)->submode = 0;                                                   \
        (work)->timer = 0;                                                     \
        (work)->tick = 0;                                                      \
    }
#define BUTTON_SOUND 0x800e
#define DOOR_SOUND(work) sound_play_at(0, NULL, 0, -20, &(work)->pos)
#define TRANSFER_SOUND(machine, work)                                          \
    sound_play_at_for(0x1018, (machine), 1, -40, 30, &(work)->pos)
#define DELIVERY_SOUND(machine)                                                \
    sound_play_at(0x1019, NULL, 0, 0, &(machine)->work->pos)
#define DOOR_POSITION(position, work, angle, reach)                            \
    {                                                                          \
        (position).x = (work)->pos.x - (njSin(angle) * (reach));               \
        (position).y = (work)->pos.y;                                          \
        (position).z = (work)->pos.z - (njCos(angle) * (reach));               \
    }
#define DELIVERY_SPEED(velocity, angle, speed, rise)                           \
    {                                                                          \
        (velocity).x = njSin(angle) * (speed);                                 \
        (velocity).y = (rise);                                                 \
        (velocity).z = njCos(angle) * (speed);                                 \
    }
/* a zeroed garden_vms_chao, which fills two VMU blocks of 512 bytes */
#define VMU_BUFFER() syCalloc(2, sizeof(struct garden_vms_chao) / 2)
/* how far from the machine its door puts what it delivers, and the Chao it
 * ejects */
#define DELIVERY_REACH 13.3F
#define EJECT_REACH 13.8F
/* how far spin turns in a step of a transfer */
#define SPIN_TURN 0xc00
/* a transfer spins the machine for more than LEAST_SPIN steps */
#define LEAST_SPIN 120
/* the button sinks BUTTON_SINK a step while a player touches it, down to
 * BUTTON_DOWN. otherwise it rises BUTTON_RISE a step, back to 0 */
#define BUTTON_SINK 0.04F
#define BUTTON_RISE 0.08F
#define BUTTON_DOWN (-0.49F)
/* door_open moves by DOOR_STEP each step the door opens or closes */
#define DOOR_STEP 0.024F
/* while the machine runs, each step pulls the player PULL of the way to
 * it. the step also turns the player PLAYER_TURN toward it */
#define PULL 0.2F
#define PLAYER_TURN 0x500
/* the machine takes in a Chao over LOAD_FRAMES frames. it waits
 * RELEASE_FRAMES frames after each thing it delivers, and FAILED_FRAMES
 * after a receive that fails */
#define LOAD_FRAMES 60
#define RELEASE_FRAMES 30
#define FAILED_FRAMES 300
/* the turn spin settles back to 0 by, a step, once a transfer ends */
#define SPIN_SETTLE 0x800

static void machine_step(struct task *machine)
{
    struct machine_work *transport;
    struct task_work *work;
    struct task *hit;
    struct task *held;
    struct task *chao;
    struct task_work *player;
    struct player_motion *motion;
    struct garden_vms_chao *packed;
#ifdef NONMATCHING
#else
    Uint32 buffer;
#endif
    NJS_POINT3 *position;
    NJS_POINT3 door;
    NJS_VECTOR velocity;
    struct chao_save *resident;
    Angle yaw;
    int result;
    Uint8 kind;
    int item;

    work = machine->work;
    transport = machine->extra.p;
    hit = collision_next_player_by_shape(machine, MACHINE_BUTTON);
    if (hit) {
        transport->button_depth -= BUTTON_SINK;
        if (transport->button_depth < BUTTON_DOWN) {
            transport->button_depth = BUTTON_DOWN;
            transport->pressed = TRUE;
        }
    } else {
        transport->button_depth += BUTTON_RISE;
        if (transport->button_depth > 0) {
            transport->pressed = FALSE;
            transport->button_depth = 0;
        }
    }
    switch (transport->door) {
    case DOOR_OPENING:
        transport->door_open += DOOR_STEP;
        if (transport->door_open > 1.0F) {
            transport->door_open = 1.0F;
            transport->door = DOOR_OPEN;
        }
        break;
    case DOOR_OPEN:
        break;
    case DOOR_CLOSING:
        transport->door_open -= DOOR_STEP;
        if (transport->door_open < 0) {
            transport->door_open = 0;
            transport->door = DOOR_SHUT;
        }
        break;
    case DOOR_SHUT:
        break;
    }
    if (!pad_is_enabled()) {
        if (work->mode != MACHINE_WAIT) {
            player = player_work[0];
            motion = player_motion[0];
            /* @hack spelling: position taken before the motion is cleared,
             * where after it would do */
            position = &player->pos;
            motion->spd.x = 0;
            motion->spd.z = 0;
            motion->acc.x = 0;
            motion->acc.z = 0;
            position->x += (work->pos.x - position->x) * PULL;
            position->z += (work->pos.z - position->z) * PULL;
            player->ang.y = angle_approach(
                player->ang.y, HALF_TURN - work->ang.y, PLAYER_TURN);
            garden_machine_in_use = TRUE;
        }
    } else {
        garden_machine_in_use = FALSE;
    }
    switch (work->mode) {
    case MACHINE_RESET:
        collision_shape_off(machine, MACHINE_INTAKE);
        pad_enable();
        pause_enable();
        transport->door = DOOR_CLOSING;
        transport->complete = FALSE;
        transport->waited = FALSE;
        MACHINE_MODE(work, MACHINE_WAIT);
        /* @hack redundant: a clear of the timer, which MACHINE_MODE has just
         * cleared */
        work->timer = 0;
        /* fall through */
    case MACHINE_WAIT:
        if (hit && !transport->pressed) {
            held = player_records[0]->held;
            sound_play_at(BUTTON_SOUND, NULL, 0, 0, &work->pos);
            pad_disable();
            pause_disable();
            if (held) {
                result = garden_object_category(held);
                switch (result) {
                case GARDEN_OBJECT_CHAO:
                    transport->chao = held;
                    MACHINE_MODE(work, MACHINE_OPEN_LOAD);
                    collision_shape_on(machine, MACHINE_INTAKE);
                    transport->door = DOOR_OPENING;
                    DOOR_SOUND(work);
                    break;
                default:
                    transport->chao = NULL;
                    MACHINE_MODE(work, MACHINE_RECEIVE_START);
                    break;
                }
            } else {
                transport->chao = NULL;
                MACHINE_MODE(work, MACHINE_RECEIVE_START);
                /* @hack redundant: a break ahead of the case's own */
                break;
            }
        }
        break;
    case MACHINE_OPEN_LOAD:
        if (transport->door == DOOR_OPEN) {
            player_throw_at_touch(player_work[0], player_records[0]);
            MACHINE_MODE(work, MACHINE_LOAD);
        }
        break;
    case MACHINE_LOAD:
        if (work->timer++ > LOAD_FRAMES) {
            /* @hack redundant: a clear of the timer, which MACHINE_MODE
             * clears below */
            work->timer = 0;
            collision_shape_off(machine, MACHINE_INTAKE);
            MACHINE_MODE(work, MACHINE_CLOSE_LOAD);
            transport->door = DOOR_CLOSING;
            DOOR_SOUND(work);
        }
        break;
    case MACHINE_CLOSE_LOAD:
        if (transport->door == DOOR_SHUT) {
            MACHINE_MODE(work, MACHINE_PACK);
        }
        break;
    case MACHINE_PACK:
        chao = transport->chao;
        result = garden_object_category(chao);
        switch (result) {
        case GARDEN_OBJECT_EGG:
            kind = CHAO_EGG;
            break;
        case GARDEN_OBJECT_CHAO:
            kind = CHAO_WORK(chao)->profile.parameters.type;
            break;
        default:
            kind = CHAO_NONE;
            break;
        }
        current_transfer = syCalloc(1, sizeof(*current_transfer));
        /* @bug memory: MACHINE_PACK writes through null where syCalloc
         * fails. it tests neither allocation: it stores the buffer through
         * the transfer at 0x0c527a36 or 0x0c527a60, and packs into the buffer
         * by the call at 0x0c527a3c or 0x0c527a66. MACHINE_RECEIVE_START
         * tests the transfer at 0x0c527bee */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: without a transfer, the
         * machine rejects */
        if (!current_transfer) {
            MACHINE_MODE(work, MACHINE_REJECT);
            break;
        }
#endif
        if (kind == CHAO_EGG) {
            current_transfer->egg = VMU_BUFFER();
#ifdef NONMATCHING
            /* this side rejects where VMU_BUFFER has no buffer to give. the
             * matching side packs through the null */
            if (!current_transfer->egg) {
                MACHINE_MODE(work, MACHINE_REJECT);
                break;
            }
#endif
            garden_pack_egg(transport->chao, &current_transfer->egg->saved);
            current_transfer->chao = NULL;
        } else if (kind != CHAO_NONE) {
            current_transfer->chao = VMU_BUFFER();
#ifdef NONMATCHING
            /* this side rejects where VMU_BUFFER has no buffer to give. the
             * matching side packs through the null */
            if (!current_transfer->chao) {
                MACHINE_MODE(work, MACHINE_REJECT);
                break;
            }
#endif
            garden_pack_chao(transport->chao, &current_transfer->chao->saved);
            current_transfer->egg = NULL;
        }
        result = garden_open_vmu_send(current_transfer);
        if (result == 0) {
            MACHINE_MODE(work, MACHINE_SEND_READY);
        } else {
            MACHINE_MODE(work, MACHINE_REJECT);
        }
        break;
    case MACHINE_SEND_READY:
        if (current_transfer->status != GARDEN_TRANSFER_IDLE) {
            switch (current_transfer->status) {
            case GARDEN_TRANSFER_BUSY:
                MACHINE_MODE(work, MACHINE_SEND);
                break;
            case GARDEN_TRANSFER_CANCELLED:
            case GARDEN_TRANSFER_FAILED:
            default:
                MACHINE_MODE(work, MACHINE_REJECT);
                break;
            }
        }
        break;
    case MACHINE_SEND:
        switch (current_transfer->status) {
        case GARDEN_TRANSFER_IDLE:
        case GARDEN_TRANSFER_BUSY:
            if (current_transfer->phase == GARDEN_TRANSFER_PHASE_INSTALLED) {
                chao_remove_resident(transport->chao);
                transport->chao = NULL;
                MACHINE_MODE(work, MACHINE_SENT);
            }
            break;
        case GARDEN_TRANSFER_CANCELLED:
        case GARDEN_TRANSFER_FAILED:
            MACHINE_MODE(work, MACHINE_REJECT);
            break;
        }
        TRANSFER_SOUND(machine, work);
        transport->spin += SPIN_TURN;
        break;
    case MACHINE_SENT:
        switch (current_transfer->status) {
        case GARDEN_TRANSFER_DONE:
            if (current_transfer->phase == GARDEN_TRANSFER_PHASE_INSTALLED &&
                !transport->complete) {
                transport->complete = TRUE;
            }
            break;
        case GARDEN_TRANSFER_CANCELLED:
        case GARDEN_TRANSFER_FAILED:
            MACHINE_MODE(work, MACHINE_FINISH);
            break;
        }
        TRANSFER_SOUND(machine, work);
        if (work->timer++ > LEAST_SPIN) {
            transport->waited = TRUE;
        }
        transport->spin += SPIN_TURN;
        if (transport->complete && transport->waited) {
            transport->result = GARDEN_TRANSFER_DONE;
            MACHINE_MODE(work, MACHINE_FINISH);
        }
        break;
    case MACHINE_UNUSED_7:
        break;
    case MACHINE_RECEIVE_START:
        current_transfer = syCalloc(1, sizeof(*current_transfer));
        if (!current_transfer) {
            MACHINE_MODE(work, MACHINE_RECEIVE_FAILED);
            break;
        }
        current_transfer->chao = VMU_BUFFER();
        if (!current_transfer->chao) {
            syFree(current_transfer);
            current_transfer = NULL;
            MACHINE_MODE(work, MACHINE_RECEIVE_FAILED);
            break;
        }
        current_transfer->egg = VMU_BUFFER();
        if (!current_transfer->egg) {
            syFree(current_transfer->chao);
            syFree(current_transfer);
            current_transfer = NULL;
            MACHINE_MODE(work, MACHINE_RECEIVE_FAILED);
            break;
        }
        result = garden_open_vmu_receive(current_transfer);
        if (result == 0) {
            MACHINE_MODE(work, MACHINE_RECEIVE_READY);
        } else {
            MACHINE_MODE(work, MACHINE_RECEIVE_FAILED);
        }
        break;
    case MACHINE_RECEIVE_READY:
        if (current_transfer->status != GARDEN_TRANSFER_IDLE) {
            switch (current_transfer->status) {
            case GARDEN_TRANSFER_BUSY:
                MACHINE_MODE(work, MACHINE_RECEIVE);
                break;
            case GARDEN_TRANSFER_DONE:
            case GARDEN_TRANSFER_CANCELLED:
            case GARDEN_TRANSFER_FAILED:
            default:
                MACHINE_MODE(work, MACHINE_FINISH);
                break;
            }
        }
        break;
    case MACHINE_RECEIVE:
        if (work->timer++ > LEAST_SPIN) {
            transport->waited = TRUE;
        }
        transport->spin += SPIN_TURN;
        if (!transport->complete) {
            switch (current_transfer->status) {
            case GARDEN_TRANSFER_IDLE:
            case GARDEN_TRANSFER_BUSY:
                break;
            case GARDEN_TRANSFER_DONE:
                transport->complete = TRUE;
                break;
            case GARDEN_TRANSFER_CANCELLED:
            case GARDEN_TRANSFER_FAILED:
            default:
                MACHINE_MODE(work, MACHINE_FINISH);
                break;
            }
        }
        TRANSFER_SOUND(machine, work);
        if (transport->complete && transport->waited) {
            MACHINE_MODE(work, MACHINE_OPEN_DELIVERY);
            transport->door = DOOR_OPENING;
            DOOR_SOUND(work);
        }
        break;
    case MACHINE_OPEN_DELIVERY:
        if (transport->door == DOOR_OPEN) {
            if (work->timer++ > 15) {
                MACHINE_MODE(work, MACHINE_RELEASE_CHAO);
            }
        }
        /* @bug logic: the release doesn't wait for the door. this mode runs
         * on into MACHINE_RELEASE_CHAO at its first step, while the door is
         * still opening. the tests of the door at 0x0c527dc6 and of the delay
         * at 0x0c527dd4 both branch to the release at 0x0c527de2 */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: the release waits for the
         * door and the delay */
        if (work->mode != MACHINE_RELEASE_CHAO) {
            break;
        }
#endif
        /* fall through */
    case MACHINE_RELEASE_CHAO:
        if (current_transfer->chao_returned) {
            packed = current_transfer->chao;
            DOOR_POSITION(door, work, work->ang.y + QUARTER_TURN,
                          DELIVERY_REACH);
            yaw = work->ang.y + QUARTER_TURN;
            resident = garden_unused_chao();
            transport->chao = chao_create(&packed->saved.record.saved.gene,
                                          &packed->saved.record.live, NULL,
                                          &door, yaw, resident);
            if (transport->chao) {
                chao_set_behavior(transport->chao, chao_wait_behavior);
                DELIVERY_SPEED(velocity, work->ang.y + QUARTER_TURN, 0.7F,
                               1.9F);
                mover_set_spd(transport->chao, &velocity);
                DELIVERY_SOUND(machine);
            }
            MACHINE_MODE(work, MACHINE_WAIT_CHAO);
        } else {
            MACHINE_MODE(work, MACHINE_RELEASE_EGG);
        }
        break;
    case MACHINE_WAIT_CHAO:
        if (work->timer++ > RELEASE_FRAMES) {
            MACHINE_MODE(work, MACHINE_RELEASE_EGG);
        }
        break;
    case MACHINE_RELEASE_EGG:
        if (current_transfer->egg_received) {
#ifdef NONMATCHING
            /* the plain form the hack tag names */
            packed = current_transfer->egg;
#define RECEIVED_GENE (&packed->saved.record.saved.gene)
#else
            /* @hack type: buffer, a Uint32 copy of current_transfer->egg
             * that the gene is read through, where packed would do */
            buffer = (Uint32)current_transfer->egg;
            packed = (struct garden_vms_chao *)buffer;
#define RECEIVED_GENE (&((struct chao_save *)buffer)->record.saved.gene)
#endif
            if (packed->saved.record.saved.parameters.type == CHAO_EGG) {
                DELIVERY_SOUND(machine);
                yaw = work->ang.y + QUARTER_TURN;
                DOOR_POSITION(door, work, yaw, DELIVERY_REACH);
                DELIVERY_SPEED(velocity, yaw, 0.6F, 1.5F);
                resident = garden_unused_chao();
                if (packed->saved.record.saved.parameters.reborn_egg) {
                    transport->chao = chao_create_egg_with_profile(
                        RECEIVED_GENE, &packed->saved.record.live, &door,
                        EGG_TRANSFERRED, resident);
                } else {
                    transport->chao = chao_create_egg(
                        RECEIVED_GENE, &door, EGG_TRANSFERRED, resident);
                }
#undef RECEIVED_GENE
#ifdef NONMATCHING
                /* this side sets no speed where the egg can't be made. the
                 * matching side writes through the null */
                if (transport->chao)
#endif
                {
                    mover_set_spd(transport->chao, &velocity);
                }
            }
            MACHINE_MODE(work, MACHINE_WAIT_ITEM);
        } else {
            MACHINE_MODE(work, MACHINE_RELEASE_ITEM);
        }
        break;
    case MACHINE_WAIT_ITEM:
        if (work->timer++ > RELEASE_FRAMES) {
            MACHINE_MODE(work, MACHINE_RELEASE_ITEM);
        }
        break;
    case MACHINE_RELEASE_ITEM:
        /* @bug memory: a card's item count above 8 reads past items. the
         * release tests only that item_count is above 0, at 0x0c52801a,
         * before it reads the item at item_count - 1, at 0x0c528050 */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: the count stops at the items'
         * size */
        if (current_transfer->item_count >
            (int)sizeof(current_transfer->items)) {
            current_transfer->item_count = sizeof(current_transfer->items);
        }
#endif
        if (current_transfer->item_count > 0) {
#ifdef NONMATCHING
            /* this side reads the last item without taking it. it takes the
             * item below only once its pickup is made */
            struct task *pickup;
            item = current_transfer->items[current_transfer->item_count - 1];
#else
            DELIVERY_SOUND(machine);
            current_transfer->item_count--;
            item = current_transfer->items[current_transfer->item_count];
#endif
            yaw = work->ang.y + QUARTER_TURN;
            DOOR_POSITION(door, work, yaw, DELIVERY_REACH);
            yaw +=
                (Angle)(-(rand() * (128.0F * ANGLE_PER_DEGREE * RAND_SCALE)));
            DELIVERY_SPEED(velocity, yaw, 0.5F, 2.0F);
#ifdef NONMATCHING
            /* this side takes the item and sounds the delivery only once its
             * pickup is made. an item past the fruit's kinds goes with no
             * pickup */
            if (item < GARDEN_SEED_TYPES) {
                pickup = garden_seed_create(item, &door, yaw, &velocity);
            } else if (item + GARDEN_FRUIT_ITEM_OFFSET < GARDEN_FRUIT_TYPES) {
                pickup = garden_fruit_create(item + GARDEN_FRUIT_ITEM_OFFSET,
                                             &door, yaw, &velocity);
            } else {
                DELIVERY_SOUND(machine);
                current_transfer->item_count--;
                MACHINE_MODE(work, MACHINE_WAIT_ITEM);
                break;
            }
            if (pickup) {
                current_transfer->item_count--;
                DELIVERY_SOUND(machine);
            }
#else
            if (item < GARDEN_SEED_TYPES) {
                garden_seed_create(item, &door, yaw, &velocity);
            } else {
                item += GARDEN_FRUIT_ITEM_OFFSET;
                garden_fruit_create(item, &door, yaw, &velocity);
            }
#endif
            MACHINE_MODE(work, MACHINE_WAIT_ITEM);
        } else {
            MACHINE_MODE(work, MACHINE_FINISH);
        }
        break;
    case MACHINE_RECEIVE_FAILED:
        if (work->timer++ > FAILED_FRAMES) {
            MACHINE_MODE(work, MACHINE_FINISH);
        }
        break;
    case MACHINE_UNUSED_18:
        break;
    case MACHINE_REJECT:
        transport->door = DOOR_OPENING;
        DOOR_SOUND(work);
        MACHINE_MODE(work, MACHINE_EJECT);
        /* fall through */
    case MACHINE_EJECT:
        if (transport->door == DOOR_OPEN && transport->chao) {
            position = &transport->chao->work->pos;
            DOOR_POSITION((*position), work, work->ang.y + QUARTER_TURN,
                          EJECT_REACH);
            /* @hack spelling: the speed built in door, where velocity would
             * do */
            DELIVERY_SPEED(door, work->ang.y + QUARTER_TURN, 0.7F, 1.9F);
            DELIVERY_SOUND(machine);
            mover_set_spd(transport->chao, &door);
            CHAO_WORK(transport->chao)->flags |= CHAO_MOVES;
            MACHINE_MODE(work, MACHINE_FINISH);
            chao_set_behavior(transport->chao, chao_wait_behavior);
            CHAO_WORK(transport->chao)->flags &= ~CHAO_COLLIDES;
        }
        break;
    case MACHINE_FINISH:
        /* @bug memory: MACHINE_FINISH reads through a null transfer. a
         * failed allocation in MACHINE_RECEIVE_START leaves current_transfer
         * null, as its test at 0x0c527bee finds, and MACHINE_RECEIVE_FAILED
         * then comes here, which reads current_transfer->chao at 0x0c52825a */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: without a transfer, it reads
         * none */
        if (current_transfer)
#endif
        {
            if (current_transfer->chao) {
                syFree(current_transfer->chao);
            }
            if (current_transfer->egg) {
                syFree(current_transfer->egg);
            }
            syFree(current_transfer);
            current_transfer = NULL;
        }
        transport->chao = NULL;
        MACHINE_MODE(work, MACHINE_RESET);
        break;
    }
    switch (work->mode) {
    case MACHINE_RECEIVE:
    case MACHINE_SEND:
    case MACHINE_SENT:
        break;
    default:
        transport->spin = angle_approach(transport->spin, 0, SPIN_SETTLE);
        break;
    }
    collision_register(machine);
}

static void machine_destroy(struct task *machine)
{
    if (current_transfer) {
        if (current_transfer->chao) {
            syFree(current_transfer->chao);
        }
        if (current_transfer->egg) {
            syFree(current_transfer->egg);
        }
        syFree(current_transfer);
        current_transfer = NULL;
    }
    garden_transfer_machine = NULL;
}

#define MACHINE_DRAW_CONTROL                                                   \
    (NJD_CONTROL_3D_SHADOW | NJD_CONTROL_3D_TRANS_MODIFIER)
/* the pulse of spin swells the machine's width by up to PULSE. its height
 * shrinks by as much */
#define PULSE 0.05F
/* door_open, from 0 to 1, spans a door motion's frames 0 to
 * DOOR_MOTION_FRAMES */
#define DOOR_MOTION_FRAMES 7.0F
#define MACHINE_DRAW_BEGIN(work, scale, height, index)                         \
    njSetTexture(machine_textures[index]);                                     \
    draw_control_set(MACHINE_DRAW_CONTROL);                                    \
    njPushMatrixEx();                                                          \
    njTranslateV(NULL, &(work)->pos);                                          \
    njRotateY(NULL, (work)->ang.y);                                            \
    njScale(NULL, (scale), (height), (scale))
#define MACHINE_DRAW_BUTTON(transport, index)                                  \
    {                                                                          \
        njPushMatrixEx();                                                      \
        njTranslate(NULL, 0, (transport)->button_depth, 0);                    \
        njCnkEasyDrawObject(machine_buttons[index]);                           \
        njPopMatrixEx();                                                       \
    }
#define MACHINE_DRAW_END(index)                                                \
    {                                                                          \
        njCnkEasyDrawObject(machine_bases[index]);                             \
        njPopMatrixEx();                                                       \
        draw_control_clear(MACHINE_DRAW_CONTROL);                              \
    }

static void draw_neutral_machine(struct task *machine)
{
    struct task_work *work = machine->work;
    struct machine_work *transport = machine->extra.p;
    Float scale = 1.0F + (njSin(transport->spin) * PULSE);
    Float height = 2.0F - scale;

    MACHINE_DRAW_BEGIN(work, scale, height, NEUTRAL_MACHINE);
    njCnkSimpleDrawObject(machine_models[NEUTRAL_MACHINE]);
    MACHINE_DRAW_BUTTON(transport, NEUTRAL_MACHINE);
    njPushMatrixEx();
    njTranslate(NULL, -12.0F, 6.0F, 0);
    njRotateZ(NULL, (Angle)(transport->door_open * QUARTER_TURN));
    njCnkSimpleDrawModel(machine_doors[NEUTRAL_MACHINE]->model);
    njPopMatrixEx();
    MACHINE_DRAW_END(NEUTRAL_MACHINE);
}

static void draw_hero_machine(struct task *machine)
{
    struct task_work *work = machine->work;
    struct machine_work *transport = machine->extra.p;
    Float scale = 1.0F + (njSin(transport->spin) * PULSE);
    Float height = 2.0F - scale;

    MACHINE_DRAW_BEGIN(work, scale, height, HERO_MACHINE);
    njCnkEasyDrawMotion(machine_models[HERO_MACHINE],
                        machine_door_motions[HERO_MACHINE],
                        transport->door_open * DOOR_MOTION_FRAMES);
    njCnkEasyDrawObject(machine_hero_trim[HERO_MACHINE]);
    MACHINE_DRAW_BUTTON(transport, HERO_MACHINE);
    MACHINE_DRAW_END(HERO_MACHINE);
}

static void draw_dark_machine(struct task *machine)
{
    struct task_work *work = machine->work;
    struct machine_work *transport = machine->extra.p;
    Float scale = 1.0F + (njSin(transport->spin) * PULSE);
    Float height = 2.0F - scale;

    MACHINE_DRAW_BEGIN(work, scale, height, DARK_MACHINE);
    njTranslate(NULL, -8.0F, 0, 0);
    njCnkEasyDrawMotion(machine_models[DARK_MACHINE],
                        machine_door_motions[DARK_MACHINE],
                        transport->door_open * DOOR_MOTION_FRAMES);
    MACHINE_DRAW_BUTTON(transport, DARK_MACHINE);
    MACHINE_DRAW_END(DARK_MACHINE);
}

static void machine_init(struct task *machine)
{
    struct task_work *work = machine->work;
    collision_init(machine, machine_shapes, MACHINE_SHAPES, COLLISION_OBJECT);
    machine->extra.p = syCalloc(1, sizeof(struct machine_work));
#ifdef NONMATCHING
    /* this side kills the machine where the heap has no work for it. the
     * matching side writes through the null */
    if (!machine->extra.p) {
        task_kill(machine);
        return;
    }
#endif
    machine->step = machine_step;
    machine->destructor = machine_destroy;
    /* @hack redundant: the stores of the garden's machine into tick, which
     * MACHINE_RESET clears before anything reads it */
    switch (chao_current_area()) {
    case CHAO_AREA_NEUTRAL:
        work->tick = NEUTRAL_MACHINE;
        machine->draw = draw_neutral_machine;
        break;
    case CHAO_AREA_HERO:
        work->tick = HERO_MACHINE;
        machine->draw = draw_hero_machine;
        break;
    case CHAO_AREA_DARK:
        work->tick = DARK_MACHINE;
        machine->draw = draw_dark_machine;
        break;
    }
    garden_machine_in_use = FALSE;
    garden_transfer_machine = machine;
}

void garden_transfer_machine_create(NJS_POINT3 *position, Angle yaw)
{
    struct task *machine =
        task_create(TASK_WORK, 2, machine_init, "ALO_OsanpoMachine");
    struct task_work *work;
#ifdef NONMATCHING
    /* this side makes no machine where task_create fails. the matching side
     * writes through the null */
    if (!machine) {
        return;
    }
#endif
    work = machine->work;
    work->pos = *position;
    work->ang.y = yaw;
}
