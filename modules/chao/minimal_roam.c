/* the roam, the step every kind of small animal takes in MINIMAL_ROAM. on
 * land the animal walks, rests and turns. once the water's surface lies more
 * than a unit above it, it swims for a point garden_spawn_pick chooses. it
 * jumps at a wall it faces in the water. now and then an animal of a kind
 * with a voice calls */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/garden.h"
#include "chao/ground.h"
#include "chao/minimal.h"
#include "chao/movement.h"
#include "chao/spawn_grid.h"
#include "collision/collision.h"
#include "draw/motion.h"
#include "object/animal.h"
#include "object/mover.h"
#include "player/player.h"
#include "shared/water_ripple.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the animal may call once every CALL_STEPS steps, with a chance of
 * CALL_CHANCE each time */
#define CALL_STEPS 120
#define CALL_CHANCE 0.05F
#define JUMP_SOUND 0x1010
#define SWIM_SOUND 0x101f
#define DIVE_SOUND 0x1020
/* each voice, by the first of the kinds that call in it */
#define PEN_CALL 0x102e
#define CHIT_CALL 0x102c
#define GORI_CALL 0x102d
#define KUJA_CALL 0x102f
#define SHEP_CALL 0x1030
#define BATMAN_CALL 0x1031

/* work->base.submode in the roam */
enum roam_mode {
    ROAM_ENTER,
    ROAM_SETTLE,
    ROAM_WALK,
    ROAM_SLOW,
    ROAM_IDLE,
    ROAM_SWIM,
    ROAM_JUMP,
    ROAM_LAND,
    ROAM_TURN_LEFT,
    ROAM_TURN_RIGHT
};

static Bool minimal_moved(struct task *animal)
{
    struct mover *mover = animal->motion.p;

    /* @hack spelling: an if that returns TRUE or FALSE, where returning the
     * comparison would do */
    if (njDistanceP2P(&animal->work->pos, &mover->last_pos) > 0.02F) {
        return TRUE;
    }
    return FALSE;
}

#define MOVE_AND_LAND(animal)                                                  \
    {                                                                          \
        mover_step(animal);                                                    \
        garden_meet_floor(animal);                                             \
    }

#define MINIMAL_REST(work)                                                     \
    {                                                                          \
        (work)->base.timer = (int)(rand() * (301.0F * RAND_SCALE)) + 60;       \
        motion_link_frames(&(work)->motion, MINIMAL_IDLE, 15);                 \
        (work)->base.submode = ROAM_IDLE;                                      \
    }

#define MINIMAL_CHANGE_HEADING(work, change, direction)                        \
    {                                                                          \
        if ((work)->base.timer > 0) {                                          \
            (work)->base.ang.y change(direction);                              \
            (work)->base.timer--;                                              \
        } else {                                                               \
            (work)->base.timer = (int)(rand() * (181.0F * RAND_SCALE)) + 120;  \
            motion_link_frames(&(work)->motion, MINIMAL_WALK, 15);             \
            (work)->base.submode = ROAM_WALK;                                  \
        }                                                                      \
    }

void garden_minimal_roam(struct task *task)
{
    /* @hack spelling: animal, a copy of task, where task would do */
    struct task *animal = task;
    struct garden_minimal_work *work = MINIMAL_WORK(animal);
    struct mover *mover = animal->motion.p;
    NJS_POINT3 position;
    NJS_VECTOR ripple;
    Float speed;
    /* @hack spelling: the turn's step and the jump timer's scale held in
     * locals, where constants would do */
    Angle turn_step = 0x222;
    Float jump_time_scale = 30.999998F * RAND_SCALE;
    Angle heading;
    int terrain;

    switch (work->base.submode) {
    case ROAM_ENTER:
        if (!work->released) {
            collision_shape_on(animal, MINIMAL_PICKUP);
        }
        collision_shape_on(animal, MINIMAL_CONTACT);
        collision_shape_off(animal, MINIMAL_LOWER);
        motion_link(&work->motion, MINIMAL_IDLE);
        work->base.submode++;
        /* fall through */
    case ROAM_SETTLE:
        MOVE_AND_LAND(animal);
        /* @hack spelling: the floor's and the speed's tests nested, where one
         * test with && would do */
        if (work->base.flags & MOVER_WORK_ON_FLOOR) {
            if (njScalor(&mover->spd) < 0.05F) {
                MINIMAL_REST(work);
            }
        }
        break;
    case ROAM_WALK:
        work->speed += 0.01F;
        if (work->speed > 0.15F) {
            work->speed = 0.15F;
        }
        if (work->base.timer > 0) {
            garden_move_forward(animal, work->speed);
            MOVE_AND_LAND(animal);
            work->base.timer--;
        } else {
            work->base.submode = ROAM_SLOW;
        }
        if ((mover->flags & MOVER_FACING_WALL) &&
            work->base.pos.y < mover->flat_y) {
            /* @hack spelling: launch_speed, one zero for spd.x and spd.z,
             * where 0.0F for each would do */
            Float launch_speed = 0.0F;
            work->base.timer = (int)(rand() * jump_time_scale) + 30;
            mover->spd.y = 1.0F;
            mover->spd.z = mover->spd.x = launch_speed;
            work->base.submode = ROAM_JUMP;
            sound_play_at(JUMP_SOUND, NULL, 0, -10, &animal->work->pos);
            break;
        }
        heading = work->base.ang.y;
        position.x = (njSin(heading) * 10.0F) + work->base.pos.x;
        position.y = work->base.pos.y;
        position.z = (njCos(heading) * 10.0F) + work->base.pos.z;
        terrain = garden_spawn_kind(&position);
        if ((mover->flags & MOVER_FACING_WALL) || terrain == TERRAIN_WATER ||
            (terrain == 8 && work->base.pos.y > mover->flat_y)) {
            work->base.timer = (int)(rand() * jump_time_scale) + 30;
            work->base.submode =
                (rand() * RAND_SCALE < 0.5F) ? ROAM_TURN_LEFT : ROAM_TURN_RIGHT;
        }
        break;
    case ROAM_SLOW:
        work->speed -= 0.015F;
        if (work->speed < 0.0F) {
            work->speed = 0.0F;
            MINIMAL_REST(work);
        }
        speed = work->speed;
        work->base.pos.x += njSin(work->base.ang.y) * speed;
        work->base.pos.z += njCos(work->base.ang.y) * speed;
        MOVE_AND_LAND(animal);
        break;
    case ROAM_IDLE:
        if (work->base.timer > 0) {
            work->base.timer--;
        } else {
            work->base.timer = (int)(rand() * (10.999999F * RAND_SCALE)) + 15;
            if (rand() * RAND_SCALE < 0.5F) {
                motion_link_frames(&work->motion, MINIMAL_WALK, 15);
                work->base.submode = ROAM_TURN_LEFT;
            } else {
                motion_link_frames(&work->motion, MINIMAL_WALK, 15);
                work->base.submode = ROAM_TURN_RIGHT;
            }
        }
        break;
    case ROAM_TURN_LEFT:
        MINIMAL_CHANGE_HEADING(work, +=, turn_step);
        break;
    case ROAM_TURN_RIGHT:
        MINIMAL_CHANGE_HEADING(work, -=, turn_step);
        break;
    case ROAM_SWIM:
        mover_turn_to_target_by(animal, 0x200);
        sound_play_at_for(SWIM_SOUND, animal, 1, -25, 90, &work->base.pos);
        if (work->base.pos.y + 1.0F > mover->flat_y - 2.0F) {
            /* @hack redundant: the factor 1.0F, garden_apply_buoyancy's
             * 1 / depth at a depth of 1 */
            mover->acc.y += -(mover->gravity *
                              (mover->flat_y - work->base.pos.y - 1.0F) * 1.0F);
        } else {
            mover->acc.y += -(mover->gravity * 2.0F);
        }
        mover->spd.x *= 0.9F;
        mover->spd.y *= 0.9F;
        mover->spd.z *= 0.9F;
        work->speed += 0.01F;
        if (work->speed > 0.15F) {
            work->speed = 0.15F;
        }
        if (--work->base.timer > 0) {
            garden_move_forward(animal, work->speed);
            MOVE_AND_LAND(animal);
        } else {
            work->base.timer = (int)(rand() * (501.0F * RAND_SCALE)) + 1200;
            garden_spawn_pick(8, &mover->target);
        }
        if (mover->flags & MOVER_FACING_WALL) {
            work->base.timer = (int)(rand() * jump_time_scale) + 30;
            mover->spd.y = 1.0F;
            mover->spd.x = 0.0F;
            mover->spd.z = 0.0F;
            work->base.submode = ROAM_JUMP;
            sound_play_at(JUMP_SOUND, NULL, 0, -10, &animal->work->pos);
        }
        if (work->base.timer % 10 == 0 && rand() * RAND_SCALE < 0.4F) {
            position.x = work->base.pos.x;
            position.y = mover->flat_y;
            position.z = work->base.pos.z;
            ripple.x = 0.03F;
            ripple.y = 0.0F;
            ripple.z = 0.7F;
            ripple_create(&position, &ripple, 0.2F);
        }
        break;
    case ROAM_JUMP:
        if (mover->spd.y < 0.2F) {
            work->base.submode = ROAM_LAND;
        }
        mover->acc.y += 0.015F;
        garden_move_forward(animal, 0.2F);
        MOVE_AND_LAND(animal);
        break;
    case ROAM_LAND:
        garden_move_forward(animal, 0.25F);
        mover->acc.y += 0.015F;
        MOVE_AND_LAND(animal);
        if (mover->flags & MOVER_ON_FLOOR) {
            work->base.submode = ROAM_WALK;
        }
        break;
    }
    /* @hack spelling: the step count's and the chance's tests nested, where
     * one test with && would do */
    if (ROAM_STEPS(work)++ % CALL_STEPS == 0) {
        if (rand() * RAND_SCALE < CALL_CHANCE) {
            switch (MINIMAL_KIND(work)) {
            case ANIMAL_PEN:
            case ANIMAL_GOMA:
            case ANIMAL_RAKO:
            case ANIMAL_HAN:
                sound_play_at(PEN_CALL, NULL, 0, -20, &animal->work->pos);
                break;
            case ANIMAL_CHIT:
            case ANIMAL_INO:
            case ANIMAL_KUMA:
            case ANIMAL_TORA:
            case ANIMAL_DRA:
                sound_play_at(CHIT_CALL, NULL, 0, -20, &animal->work->pos);
                break;
            case ANIMAL_GORI:
                sound_play_at(GORI_CALL, NULL, 0, -20, &animal->work->pos);
                break;
            case ANIMAL_USA:
            case ANIMAL_SKA:
            case ANIMAL_ARA:
            case ANIMAL_BONE:
            case ANIMAL_UNI:
                break;
            case ANIMAL_KUJA:
            case ANIMAL_OUM:
            case ANIMAL_KON:
            case ANIMAL_PHEN:
                sound_play_at(KUJA_CALL, NULL, 0, -20, &animal->work->pos);
                break;
            case ANIMAL_SHEP:
                sound_play_at(SHEP_CALL, NULL, 0, -20, &animal->work->pos);
                break;
            case ANIMAL_BATMAN:
                sound_play_at(BATMAN_CALL, NULL, 0, -20, &animal->work->pos);
                break;
            }
        }
    }
    switch (work->base.submode) {
    case ROAM_SWIM:
    case ROAM_JUMP:
        break;
    default:
        if (mover->flat_y > work->base.pos.y + 1.0F) {
            work->base.timer = (int)(rand() * (501.0F * RAND_SCALE)) + 1200;
            switch (chao_current_area()) {
            case CHAO_AREA_NEUTRAL:
                garden_spawn_pick(TERRAIN_WANDER, &mover->target);
                break;
            /* @hack spelling: a case for each garden, where Hero and Dark
             * could share one */
            case CHAO_AREA_HERO:
                garden_spawn_pick(8, &mover->target);
                break;
            case CHAO_AREA_DARK:
                garden_spawn_pick(8, &mover->target);
                break;
            }
            motion_link_frames(&work->motion, MINIMAL_WALK, 15);
            sound_play_at(DIVE_SOUND, NULL, 0, 0, &animal->work->pos);
            work->base.submode = ROAM_SWIM;
        }
    }
    switch (work->base.submode) {
    case ROAM_IDLE:
    case ROAM_TURN_LEFT:
    case ROAM_TURN_RIGHT:
        if (minimal_moved(animal)) {
            work->base.submode = ROAM_SETTLE;
        }
        break;
    }
    if (work->base.flags & WORK_HELD_BY_PLAYER) {
        work->base.mode = MINIMAL_HELD;
        work->base.submode = 0;
    }
}
