#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "hud/hud.h"
#include "object/animal.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "stg18/container.h"
#include "stg18/container_art.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the box is 2 * HALF_SIZE on a side, and its pos is the center of its base */
#define HALF_SIZE 10.0F

/* a box's modes, in its work's submode. a MODE_BROKEN box's pieces have yet
 * to fly. a MODE_GONE box's pieces have flown */
enum container_mode { MODE_WHOLE, MODE_BROKEN, MODE_GONE };

/* a box's variant: CONTAINER_BRIGHT where the whole part of scl.x's magnitude
 * is even, and CONTAINER_DARK where it is odd */
#define VARIANT(task) ((int)njAbs((task)->work->scl.x) % CONTAINER_VARIANTS)

/* a broken box's timer counts its steps from 0 at the break. the boxes above
 * it drop on the step the timer is DROP_STEP. its pieces fly once the timer
 * passes PIECES_STEP */
#define DROP_STEP 1
#define PIECES_STEP 2

/* the height of the floor under the box, kept in task->extra in place of a
 * pointer. task_destroy frees a non-null extra, so the destructor clears extra
 * first */
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define FLOOR(task) (*(Float *)&(task)->extra)

/* FLOOR(task) is NO_FLOOR of collision/surface.h until the box finds a floor.
 * a whole box looks for one every FLOOR_STEPS steps, a power of 2, from
 * FLOOR_LOOK_HEIGHT above its pos */
#define FLOOR_STEPS 32
#define FLOOR_LOOK_HEIGHT 5.0F

/* a stacked box's work holds its fall in scl.y and scl.z, not a scale.
 * FALL_LEFT is the height left to fall, which stack_drop raises. FALL_SPEED is
 * the box's speed in y, negative while it falls */
#define FALL_LEFT(work) ((work)->scl.y)
#define FALL_SPEED(work) ((work)->scl.z)
#define FALL_DRAG 0.99F
#define FALL_GRAVITY 0.08F

/* a box that lands within DUST_HEIGHT of its floor raises puffs along its
 * base's edges */
#define DUST_HEIGHT 5.0F
#define LAND_PUFF_SCALE 8.0F
#define LAND_PUFF_DENSITY 1.2F
#define LAND_PUFF_SPEED 0.5F

#define BREAK_SOUND 0x1010
#define BREAK_PRIORITY 1
#define BREAK_SOUND_DURATION 80
#define BREAK_POINTS 20
#define BREAK_PUFFS 14
#define BREAK_PUFF_RADIUS 7.0F
#define BREAK_PUFF_RADIUS_RANGE 3.0F
#define BREAK_PUFF_SPEED 0.5F
#define BREAK_PUFF_SCALE 2.5F
#define BREAK_PUFF_SCALE_RANGE 4.0F

/* vibration_start's power and freq for the player the break stops.
 * vibration_start raises VIBRATION_FREQ to 15, the least freq it takes */
#define VIBRATION_POWER 3
#define VIBRATION_FREQ 7

/* the work's tick holds the low byte of ang.z, the box's animal byte of
 * object/animal.h. its ANIMAL_CHANCE_BITS hold the chance, in percent, that
 * the box releases its animal. a chance of ANIMAL_CERTAIN or more is
 * certain */
#define ANIMAL(work) ((work)->tick)
#define ANIMAL_CERTAIN 10

/* the chance times TENTH twice is the animal's probability. the box releases
 * the animal where rand() * RAND_SCALE falls below it */
#define TENTH 0.1F

/* a bit of ang.x: the box's pieces fall through its floor */
#define PIECES_FALL_THROUGH 0x1

/* the floor a piece stops at */
#define PIECE_FLOOR(task, work)                                                \
    ((work)->ang.x & PIECES_FALL_THROUGH                                       \
         ? NO_FLOOR                                                            \
         : (FLOOR(task) != NO_FLOOR ? FLOOR(task) : (work)->pos.y))

/* a built piece flies at a speed from PIECE_SPEED up to PIECE_SPEED +
 * PIECE_SPEED_RANGE. a piece not built takes container_piece_speed and
 * container_piece_spread in their place */
#define PIECE_SPEED 0.7F
#define PIECE_SPEED_RANGE 1.2F
#define BUILT_PIECE_SPEED()                                                    \
    (((Float)rand() * RAND_SCALE * PIECE_SPEED_RANGE) + PIECE_SPEED)
#define UNBUILT_PIECE_SPEED()                                                  \
    (((Float)rand() * container_piece_spread * RAND_SCALE) +                   \
     container_piece_speed)

/* bright_container_builds holds the bright box's model at BOX_BUILD. from
 * PIECE_BUILD it holds bright_container_pieces' models, in that array's
 * order */
#define BOX_BUILD 0
#define PIECE_BUILD 1

static void container_destroy(struct task *task);
static void container_step(struct task *task);
static void container_draw(struct task *task);

Uint8 *lagoon_container_build(Uint8 *buf)
{
    if (model_builds_compile(&buf, bright_container_builds, CONTAINER_BUILDS) ==
        CONTAINER_BUILDS) {
        container_built = TRUE;
    }
    return buf;
}

void lagoon_container_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = container_draw;
    task->step = container_step;
    task->destructor = container_destroy;
    work->submode = MODE_WHOLE;
    FLOOR(task) = NO_FLOOR;
    FALL_LEFT(work) = 0.0F;
    FALL_SPEED(work) = 0.0F;
    collision_init(task, container_shape, CONTAINER_SHAPES, COLLISION_OBJECT);
    ANIMAL(work) = work->ang.z;
    stack_add(task);
}

static void container_destroy(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task read in place
     * would do */
    struct task *owner = task;

    stack_remove(owner);
    owner->extra.p = NULL;
}

static void container_step(struct task *task)
{
    struct task_work *work = task->work;
    int i;
    int j;
    int hit;
    Bool broken;
    int chance;
    /* @hack spelling: nibble_mask, a local for ANIMAL_KIND_BITS, where the
     * name in place would do */
    int nibble_mask;

    if (placement_try_leave(task)) {
        return;
    }
    nibble_mask = ANIMAL_KIND_BITS;
    /* @hack spelling: the fall's test as a loop that only jumps to resting,
     * and the jumps to resting and landing, where if and else would do */
    while (!(FALL_LEFT(work) > 0.0F)) {
        goto resting;
    }
    {
        /* @hack spelling: height, a local for FALL_LEFT(work), where
         * FALL_LEFT(work) read in place would do */
        Float height = FALL_LEFT(work);
        Float step = FALL_SPEED(work);
#ifdef NONMATCHING
#else
        /* @hack redundant: a block no path reaches, with a store to
         * container_built and one to the floor */
        if (0) {
            container_built = TRUE;
            FLOOR(task) = 1.0F;
        }
#endif
        if (height < -step) {
            step = -FALL_LEFT(work);
        }
        work->pos.y += step;
        FALL_LEFT(work) += step;
        FALL_SPEED(work) = (FALL_SPEED(work) * FALL_DRAG) - FALL_GRAVITY;
        /* a falling box's child stands where the box will land. the child
         * has a copy of the box's shape, and no model */
        if (task->child == NULL) {
            model_object_create(task, NULL, NULL, container_shape,
                                CONTAINER_SHAPES, 0);
        }
        if (task->child != NULL) {
            struct task_work *child = task->child->work;

            child->pos.x = work->pos.x;
            child->pos.z = work->pos.z;
            child->pos.y = work->pos.y - FALL_LEFT(work);
        }
        goto landing;
    }
resting:
    if (task->child != NULL) {
        task_destroy_children(task);
    }
landing:
    if (FALL_LEFT(work) <= 0.0F && FALL_SPEED(work) != 0.0F) {
        FALL_LEFT(work) = 0.0F;
        FALL_SPEED(work) = 0.0F;
        if (njAbs(work->pos.y - FLOOR(task)) < DUST_HEIGHT) {
            dust_short_rect(&work->pos, HALF_SIZE, HALF_SIZE, &work->ang,
                            LAND_PUFF_SCALE, LAND_PUFF_DENSITY,
                            LAND_PUFF_SPEED);
        }
    }
    switch (work->submode) {
    case MODE_WHOLE:
        hit = collision_get_hit_count(task);
        broken = FALSE;
        while (hit--) {
            if (player_attacks_task(task, collision_get_hit(task, hit)->task)) {
                broken = TRUE;
                break;
            }
        }
        if (broken) {
            NJS_POINT3 center;
            int player;

            center = work->pos;
            center.y += HALF_SIZE;
            for (i = 0; i < BREAK_PUFFS; i++) {
                dust_short_sphere(
                    &center,
                    ((Float)rand() * RAND_SCALE * BREAK_PUFF_RADIUS_RANGE) +
                        BREAK_PUFF_RADIUS,
                    ((Float)rand() * RAND_SCALE) + BREAK_PUFF_SPEED,
                    ((Float)rand() * RAND_SCALE * BREAK_PUFF_SCALE_RANGE) +
                        BREAK_PUFF_SCALE);
            }
            work->timer = 0;
            work->submode = MODE_BROKEN;
            sound_play_at_for(BREAK_SOUND, work, BREAK_PRIORITY, SOUND_LOUDEST,
                              BREAK_SOUND_DURATION, &work->pos);
            if (work->collision->hit->owner->work->id == WORK_ID_PLAYER) {
                /* @hack spelling: the assignment to player inside the test,
                 * where a statement of its own would do */
                if ((player = player_slot_of(work->collision->hit->owner)) !=
                        -1 &&
                    (player_records[player]->character != PLAYER_SONIC ||
                     player_work[player]->mode != PLAYER_MODE_SONIC_BOUNCE)) {
                    player_set_speed(player, 0.0F, 0.0F, 0.0F);
                    vibration_start(player, VIBRATION_POWER, VIBRATION_FREQ,
                                    VIBRATION_UNTIMED);
                }
            }
            score_add(BREAK_POINTS);
        } else {
            work->timer++;
            if (FLOOR(task) == NO_FLOOR && !(work->timer & (FLOOR_STEPS - 1))) {
                struct angles ang;

                ang.x = 0;
                ang.y = work->ang.y;
                ang.z = 0;
                FLOOR(task) = surface_probe_floor(
                    work->pos.x, work->pos.y + FLOOR_LOOK_HEIGHT, work->pos.z,
                    &ang);
            }
            collision_register(task);
        }
        break;
    case MODE_BROKEN:
        if (work->timer == DROP_STEP) {
            /* @hack spelling: at, a copy of the work's pos, where &work->pos
             * would do: stack_drop doesn't write through pos */
            NJS_POINT3 at;

            at = work->pos;
            stack_drop(&at);
        }
        if (work->timer++ > PIECES_STEP) {
            NJS_POINT3 center;
            NJS_VECTOR scale;

            center = work->pos;
            center.y += HALF_SIZE;
            scale.z = scale.y = scale.x = HALF_SIZE;
            if (container_built) {
                for (i = 0; i < CONTAINER_PIECES; i++) {
                    for (j = 0; j < bright_container_pieces[i].count; j++) {
                        debris_throw_built(
                            &center, DEBRIS_THROW_SQUARE, &scale, work->ang.y,
                            BUILT_PIECE_SPEED(), PIECE_FLOOR(task, work),
                            bright_container_pieces[i].model,
                            bright_container_builds[i + PIECE_BUILD].built,
                            bright_container_pieces[i].textures);
                    }
                }
#ifdef NONMATCHING
#else
                /* @hack redundant: a block no path reaches, with a store of
                 * PIECE_SPEED to scale.x */
                if (0) {
                    scale.x = PIECE_SPEED;
                }
#endif
            } else {
                for (i = 0; i < CONTAINER_PIECES; i++) {
                    for (j = 0; j < bright_container_pieces[i].count; j++) {
                        if (VARIANT(task) == CONTAINER_BRIGHT) {
                            debris_throw(&center, DEBRIS_THROW_SQUARE, &scale,
                                         work->ang.y, UNBUILT_PIECE_SPEED(),
                                         PIECE_FLOOR(task, work),
                                         bright_container_pieces[i].model,
                                         bright_container_pieces[i].textures);
                        } else {
                            debris_throw(&center, DEBRIS_THROW_SQUARE, &scale,
                                         work->ang.y, UNBUILT_PIECE_SPEED(),
                                         PIECE_FLOOR(task, work),
                                         dark_container_pieces[i].model,
                                         dark_container_pieces[i].textures);
                        }
                    }
                }
            }
            work->submode = MODE_GONE;
        }
        collision_register(task);
        break;
    case MODE_GONE:
        /* @hack redundant: the mask with nibble_mask after the shift, where
         * the shift alone would do */
        chance = ((ANIMAL(work) & ANIMAL_CHANCE_BITS) >> ANIMAL_CHANCE_SHIFT) &
                 nibble_mask;
        if (chance) {
            /* @hack spelling: odds *= TENTH in a statement of its own, where
             * (Float)chance * TENTH * TENTH would do */
            Float odds = (Float)chance * TENTH;

            odds *= TENTH;
            if (odds > ((Float)rand() * RAND_SCALE) ||
                chance >= ANIMAL_CERTAIN) {
                animal_release(
                    work->pos.x, work->pos.y + HALF_SIZE, work->pos.z,
                    (ANIMAL(work) & nibble_mask) % ANIMAL_RELEASE_KINDS);
            }
        }
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        break;
    }
}

#define BOX_DRAW container_draw
#define BOX_STEP container_step
#define BOX_TEXTURES(task) container_texlists[VARIANT(task)]
#define BOX_BUILT container_built
#define BOX_COMPILED bright_container_builds[BOX_BUILD].built
#define BOX_MODEL(task) container_models[VARIANT(task)]
#include "shared/stack_box_draw.h"
