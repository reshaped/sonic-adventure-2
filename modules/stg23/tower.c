#include "stg23/tower_models.h"
#include "system/port.h"
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/stack.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/ocean_util.h"
#include "sound/sound.h"
#include "stg21/explosion.h"
#include "stg23/charge.h"
#include "stg23/tower.h"
#include "stg23/tower_textures.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

enum tier_mode { TIER_REST, TIER_FALL, TIER_LAND };
#define NO_NEXT_MODE (-1)
#define NEXT_MODE(task) ((task)->unk_3c.l)
/* the length of the run of tiers whose nibble of ang.z starts at shift. the
 * four shifts below name the runs, from the bottom up */
#define RUN_LENGTH(work, shift) (((work)->ang.z >> (shift)) & 15)
#define LOW_SUPPORTS 0
#define LOW_PLATFORMS 4
#define HIGH_SUPPORTS 8
#define HIGH_PLATFORMS 12
#define TIER_COUNT(work)                                                       \
    (RUN_LENGTH(work, HIGH_PLATFORMS) + RUN_LENGTH(work, HIGH_SUPPORTS) +      \
     RUN_LENGTH(work, LOW_PLATFORMS) + RUN_LENGTH(work, LOW_SUPPORTS))
/* the tower's count of tiers, which no code reads */
#define TOWER_TIERS(work) ((work)->tick)
/* the tower's tiers, from the bottom up. a tier's INDEX is its place there,
 * and BELOW the tier under it */
#define TIERS(tower) ((struct task **)(tower)->extra.p)
#define INDEX(work) ((work)->ang.z)
#define BELOW(task) (TIERS((task)->parent)[INDEX((task)->work) - 1])
/* nonzero once the tier has broken. a broken tier stands no height */
#define BROKEN(work) ((work)->timer)
#define HEIGHT(tier) (BROKEN((tier)->work) ? 0.0F : tier_height)
/* 1.0F for a platform, 0.0F for a support */
#define PLATFORM(work) ((work)->scl.z)
/* the frame of tower_motion a platform draws. it runs from 1.0F as the
 * platform starts to fall up to the motion's last. at 0.0F the platform draws
 * without the motion */
#define FRAME(work) ((work)->scl.x)
#define FALL_SPEED(work) ((work)->scl.y)
/* the turn of a support's charges about it: ang.x quarter turns */
#define CHARGE_ANGLE(work) ((Uint16)((work)->ang.x * QUARTER_TURN))
#define CHARGES 3
/* the submode once a support's every charge has blown up */
#define ALL_CHARGES ((1 << CHARGES) - 1)
#define GRAVITY 0.08F
#define DRAG 0.997F
/* the share of its speed a landing tier rebounds with */
#define REBOUND 0.0F
/* a falling platform's FRAME stops at IMPACT_FRAME, and goes to it as the
 * platform lands */
#define IMPACT_FRAME 36.0F
#define IMPACT_SOUND 6
/* a landing's dust: puffs of size DUST_SCALE on a square of side
 * 2 * DUST_HALF */
#define DUST_HALF 5.0F
#define DUST_SCALE 4.0F
#define DUST_DENSITY 2.0F
#define DUST_SPEED 1.0F
/* the camera a landing shakes, and the player it measures from and whose
 * pad it vibrates */
#define SHAKE_CAMERA 0
#define SHAKE_PLAYER 0
/* past SHAKE_NEAR, the shake's size is divided by the distance. past
 * VIBRATION_NEAR, the vibration's power is divided by VIBRATION_FALLOFF times
 * the distance */
#define SHAKE_NEAR 1.0F
#define VIBRATION_NEAR 10.0F
#define VIBRATION_FALLOFF 0.1F
#define FRAGMENTS 6
/* a broken support's fragments start FRAGMENT_HEIGHT above its pos. its blast
 * goes off BLAST_REACH out from it along CHARGE_ANGLE, and BLAST_HEIGHT up */
#define FRAGMENT_HEIGHT 15.0F
#define BLAST_REACH 15.0F
#define BLAST_HEIGHT 25.0F
/* a broken support's floor probe starts PROBE_LIFT above its pos, and
 * PROBE_Z along world z */
#define PROBE_LIFT 1.0F
#define PROBE_Z 25.1F
/* while player 0 or 1 is within CONTACT_RADIUS of a platform's world object,
 * the platform's work keeps WORLD_ADDED. world_collect_near_players takes the
 * object only then */
#define CONTACT_RADIUS 183.0F
#define PUSH_GROUPS PUSH_SOLID(PUSH_ALL_BUT_PICKUPS)

static void tower_destroy(struct task *task);
static void tower_step(struct task *task);
static void tower_draw(struct task *task);
static void tier_init(struct task *task);
static void platform_destroy(struct task *task);
static void tier_step(struct task *task);
static void tier_draw(struct task *task);
static void take_mode(struct task *task);
/* @hack type: tower_queue_mode external, where static would do. the tiers'
 * steps inline its calls, and the image holds its body too */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void tower_queue_mode(struct task *task, int mode);
static void tier_rest(struct task *task);
static void tier_fall(struct task *task);
static void tier_land(struct task *task);
static void support_break(struct task *task);

#include "stg23/tower_art.h"

/* the share of its speed a landing tier of shared/stacked_platform_task.h
 * rebounds with. no code here reads it: tier_fall rebounds by REBOUND */
static Float rebound_share = 0.11F;
static Float tier_height = 50.0F;
/* a platform's height, which no code reads. HEIGHT gives platforms and
 * supports alike tier_height */
static Float platform_height = 50.0F;
KEPT static NJS_POINT3 charge_offsets[CHARGES] = {
#include "stg23/tower/charge_offsets.inc"
};
static Sint32 impact_volume = 125;
static void (*steps[])(struct task *) = {tier_rest, tier_fall, tier_land};
static Float shake_size = 150.0F;
static Sint32 shake_steps = 3;
static Sint32 vibration_power = 18;
static Sint32 vibration_frequency = 20;
static Sint32 vibration_time = 1;
static Float blast_start = 7.5F;
static Float blast_end = 15.5F;
static Sint32 blast_life = 25;
static Float fragment_speed = 0.7F;
static Float fragment_spread = 0.35F;
static Float fragment_rise = -0.25F;

#ifdef NONMATCHING
/* whether any charge offset isn't 0. the disc-free census builds
 * charge_offsets as zeros, and in that build a support places no charges */
static Bool tower_has_charge_mounts(void)
{
    int i;

    for (i = 0; i < CHARGES; i++) {
        if (charge_offsets[i].x != 0.0F || charge_offsets[i].y != 0.0F ||
            charge_offsets[i].z != 0.0F) {
            return TRUE;
        }
    }
    return FALSE;
}
#endif

void base_tower_init(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    int index;
    int end;
    Float height;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = tower_draw;
    task->step = tower_step;
    task->destructor = tower_destroy;
    TOWER_TIERS(task->work) = 0;
    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((task->extra.p =
             syCalloc(sizeof(struct task *), TIER_COUNT(task->work))) == NULL) {
        /* @bug memory: without its tier table, the tower keeps the step it
         * was given, which reads the table through null for each tier. the
         * store of tower_step at 0x8c5038f2 comes before the call to syCalloc
         * at 0x8c503924. tower_step reads through the null at 0x8c503bfc,
         * though tower_destroy tests the table for null */
#ifdef NONMATCHING
        /* this side repairs the tag above: without its tier table, the init
         * destroys the tower, as it does where a tier can't be made, and
         * set.c makes it again on a later pass */
        task_destroy(task);
#endif
        return;
    }
    TOWER_TIERS(task->work) = TIER_COUNT(task->work);
    height = work->pos.y;
    for (index = 0; index < TIER_COUNT(task->work); index++) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((TIERS(task)[index] =
                 task_create_child(TASK_WORK, tier_init, task)) == NULL) {
            task_destroy(task);
            return;
        } else {
            INDEX(TIERS(task)[index]->work) = index;
            BROKEN(TIERS(task)[index]->work) =
                (placement_get_state(task) >> index) & 1;
            TIERS(task)[index]->work->pos = work->pos;
            TIERS(task)[index]->work->pos.y = height;
            TIERS(task)[index]->work->ang.x = work->ang.x;
            TIERS(task)[index]->work->ang.y = task->work->ang.y;
            height += HEIGHT(TIERS(task)[index]);
        }
    }
    for (index = 0; index < RUN_LENGTH(task->work, LOW_SUPPORTS); index++) {
        PLATFORM(TIERS(task)[index]->work) = 0.0F;
    }
    for (end = index; end < index + RUN_LENGTH(task->work, LOW_PLATFORMS);
         end++) {
        PLATFORM(TIERS(task)[end]->work) = 1.0F;
    }
    for (index = end; index < end + RUN_LENGTH(task->work, HIGH_SUPPORTS);
         index++) {
        PLATFORM(TIERS(task)[index]->work) = 0.0F;
    }
    for (end = index; end < index + RUN_LENGTH(task->work, HIGH_PLATFORMS);
         end++) {
        PLATFORM(TIERS(task)[end]->work) = 1.0F;
    }
}

static void tower_destroy(struct task *task)
{
    if (task->extra.p) {
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

static void tower_step(struct task *task)
{
    int index;
    Sint8 bits;

    if (placement_try_leave(task)) {
        return;
    }
    bits = placement_get_state(task);
    for (index = 0; index < TIER_COUNT(task->work); index++) {
        if (BROKEN(TIERS(task)[index]->work)) {
            bits |= 1 << index;
        }
    }
    placement_set_state(task, bits);
}

static void tower_draw(struct task *task)
{
}

static void tier_init(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    NJS_POINT3 pos;
    Float x;
    Float z;
    int index;

    task->draw = tier_draw;
    task->step = tier_step;
    task->destructor = platform_destroy;
    if (PLATFORM(task->work) == 0.0F) {
        if (!BROKEN(task->work)) {
#ifdef NONMATCHING
            /* this side places charges only where tower_has_charge_mounts finds
             * a mount. the matching side's test, which the hack tag names,
             * always passes here */
            if (tower_has_charge_mounts())
#else
            /* @hack redundant: the test of PLATFORM, which is 0.0F here */
            if (PLATFORM(task->work) == 0.0F)
#endif
            {
                for (index = 0; index < CHARGES; index++) {
                    ocean_turn_xz(charge_offsets[index].x,
                                  charge_offsets[index].z,
                                  CHARGE_ANGLE(task->work), &x, &z);
                    pos.x = x;
                    pos.y = charge_offsets[index].y;
                    pos.z = z;
                    base_charge_attach(task, &pos, CHARGE_ANGLE(task->work),
                                       index);
                }
            }
            collision_init(task, &shape, 1, COLLISION_OBJECT_MUTUAL);
        }
    } else {
        task->motion.p = object = world_pool_alloc();
        *object = tower_collision;
        object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
        object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
        object->ang[0] = 0;
        object->ang[1] = task->work->ang.y;
        object->ang[2] = 0;
        object->pos[0] = work->pos.x;
        object->pos[1] = work->pos.y;
        object->pos[2] = work->pos.z;
        world_add(WORLD_SIZE_CLASS(1) | WORLD_MOVING | WORLD_SOLID, task,
                  object);
    }
    task->work->mode = TIER_REST;
    NEXT_MODE(task) = NO_NEXT_MODE;
    FALL_SPEED(task->work) = 0.0F;
}

#include "stg23/platform_destroy_impl.h"

static void tier_step(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    take_mode(task);
    steps[task->work->mode](task);
    if (PLATFORM(task->work) == 0.0F) {
        if (!BROKEN(task->work) && task->work->submode == ALL_CHARGES) {
            support_break(task);
            BROKEN(task->work) = 1;
            return;
        }
        if (!BROKEN(task->work)) {
            collision_register(task);
        }
    } else {
        object = task->motion.p;
        object->pos[1] = work->pos.y;
        if (player_near((NJS_POINT3 *)object->pos, CONTACT_RADIUS)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void tier_draw(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;

    if (BROKEN(task->work)) {
        return;
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, task->work->ang.y);
    if (PLATFORM(task->work) == 0.0F) {
        njSetTexture(&tower_support_texlist);
        njCnkEasyDrawModel(&tower_support_model);
    } else {
        draw_control_set(DRAW_CONTROL_SHADOW);
        njSetTexture(&tower_platform_texlist);
        if (FRAME(task->work) == 0.0F) {
            njCnkEasyDrawModel(&tower_platform_model);
        } else {
            njCnkEasyDrawMotion(&tower_platform, tower_motion,
                                FRAME(task->work));
        }
        draw_control_clear(DRAW_CONTROL_SHADOW);
    }
    njPopMatrixEx();
}

static void take_mode(struct task *task)
{
    if (NEXT_MODE(task) != NO_NEXT_MODE) {
        task->work->mode = NEXT_MODE(task);
        NEXT_MODE(task) = NO_NEXT_MODE;
    }
}

void tower_queue_mode(struct task *task, int mode)
{
    NEXT_MODE(task) = mode;
}

static void tier_rest(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    struct task *below;
    Float floor;

    if (INDEX(task->work) > 0) {
        /* @hack spelling: the assignment of below inside the read, where a
         * statement of its own would do */
        floor = (below = BELOW(task))->work->pos.y;
        floor += HEIGHT(below);
        if (FRAME(task->work) != 0.0F) {
            FRAME(task->work) += 1.0F;
            if (FRAME(task->work) >= tower_motion->nbFrame) {
                FRAME(task->work) = 0.0F;
            }
        }
        if (work->pos.y > floor) {
            tower_queue_mode(task, TIER_FALL);
            FRAME(task->work) = 1.0F;
        }
    }
}

static void tier_fall(struct task *task)
{
    struct task_work *work;
    struct task *below;
    Float floor;
    Float distance;
    Float size;
    Float power;

    /* @hack spelling: the assignment of below inside the read, where a
     * statement of its own would do */
    floor = (below = BELOW(task))->work->pos.y;
    floor += HEIGHT(below);
    /* @hack spelling: work set after the floor is read, where its declaration
     * would set it */
    /* @hack spelling: task->work read again below, where work would do */
    work = task->work;
    FALL_SPEED(work) -= GRAVITY;
    FALL_SPEED(task->work) *= DRAG;
    work->pos.y += FALL_SPEED(task->work);
    if (FRAME(task->work) < IMPACT_FRAME) {
        FRAME(task->work) += 1.0F;
    }
    if (work->pos.y <= floor) {
        work->pos.y = floor;
        FALL_SPEED(task->work) = -(FALL_SPEED(task->work) * REBOUND);
        dust_rect(&work->pos, DUST_HALF, DUST_HALF, &work->ang, DUST_SCALE,
                  DUST_DENSITY, DUST_SPEED);
        tower_queue_mode(task, TIER_LAND);
        distance = njDistanceP2P(&work->pos, &player_work[SHAKE_PLAYER]->pos);
        size = shake_size;
        power = vibration_power;
        if (distance > SHAKE_NEAR) {
            size /= distance;
        }
        if (distance > VIBRATION_NEAR) {
            power /= distance * VIBRATION_FALLOFF;
        }
        camera_shake(SHAKE_CAMERA, shake_steps, size);
        vibration_start(SHAKE_PLAYER, power, vibration_frequency,
                        vibration_time);
        sound_play_at(IMPACT_SOUND, NULL, 0, impact_volume, &work->pos);
        FRAME(task->work) = IMPACT_FRAME;
    }
}

static void tier_land(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    struct task *below;
    Float floor;

    /* @hack spelling: the assignment of below inside the read, where a
     * statement of its own would do */
    floor = (below = BELOW(task))->work->pos.y;
    floor += HEIGHT(below);
    FALL_SPEED(work) -= GRAVITY;
    FALL_SPEED(task->work) *= DRAG;
    work->pos.y += FALL_SPEED(task->work);
    if (work->pos.y <= floor) {
        work->pos.y = floor;
        tower_queue_mode(task, TIER_REST);
    }
    if (FRAME(task->work) < tower_motion->nbFrame - 1) {
        FRAME(task->work)++;
    }
}

static void support_break(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR speed;
    Float x;
    Float z;
    Float floor;

    pos = work->pos;
    floor =
        surface_probe_floor(pos.x, pos.y + PROBE_LIFT, pos.z + PROBE_Z, &ang);
    pos.y += FRAGMENT_HEIGHT;
    speed.x = 0.0F;
    speed.y = fragment_rise;
    speed.z = 0.0F;
    debris_break_flags(&pos, task->work->ang.y, fragments, FRAGMENTS,
                       fragment_speed, fragment_spread, floor, &speed,
                       DEBRIS_TRAIL | DEBRIS_VANISH);
    ocean_polar_xz(CHARGE_ANGLE(task->work), BLAST_REACH, &x, &z);
    pos = work->pos;
    pos.x += x;
    pos.y += BLAST_HEIGHT;
    pos.z += z;
    ocean_explosion_create(&pos, blast_start, blast_end, blast_life);
}
