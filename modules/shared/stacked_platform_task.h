/* DAI's init and callbacks, and those of its tiers and their targets. DAI is
 * a stack of tiers. stg21/stacked_platform.c compiles them for Sand Ocean, and
 * stg23/stacked_platform.c for Hidden Base under HIDDEN_BASE_STACK. the
 * includer defines the tiers' art by shared/stacked_platform_art.h. each tier
 * is a child task of the stack, and each target a child task of its tier */
#ifndef SHARED_STACKED_PLATFORM_TASK_H
#define SHARED_STACKED_PLATFORM_TASK_H

#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/stack.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "effect/burst.h"
#include "effect/particle.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/flash.h"
#include "shared/ocean_util.h"
#include "sound/sound.h"
#include "stg21/stacked_platform.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* a tier's modes, in its work's mode. tier_steps holds each one's step */
enum tier_mode { TIER_STILL, TIER_FALLING, TIER_REBOUND };
/* the phases of a tier's tint, in TINT_PHASE. in TINT_CLEAR the tint stays
 * COLOR_NONE until a target is hurt. TINT_BEGIN fades it to light_tint by
 * TINT_STEP a step. TINT_DARKEN then fades it to dark_tint by DARKEN_STEP a
 * step, and TINT_LIGHTEN back by TINT_STEP. those two take turns from then
 * on */
enum tint_phase { TINT_CLEAR, TINT_BEGIN, TINT_DARKEN, TINT_LIGHTEN };
#define TINT_STEP 0.05F
#define DARKEN_STEPS 30.0F
#define DARKEN_STEP (1.0F / DARKEN_STEPS)
#define TIER_KINDS 2
/* a tier's targets, as bits of MISSING_TARGETS and SAVED_TARGETS */
enum target_bit { LOWER_TARGET = 1, UPPER_TARGET = 2, BOTH_TARGETS = 3 };
#define MAX_TIERS 8
/* the stack's count of tiers, and its tiers' kind, from scl's whole parts */
#define TIER_COUNT(work) (((int)njAbs((work)->scl.z) % MAX_TIERS) + 1)
#define TIER_KIND(work) ((int)njAbs((work)->scl.x) % TIER_KINDS)
/* the count of tiers the init keeps in the stack's tick, which no code reads */
#define STACK_TIERS(work) ((work)->tick)
/* the stack's tiers, from the bottom up */
#define TIERS(stack) ((struct task **)(stack)->extra.p)
/* each tier's share of the placement's state, in bits. stack_step sets both
 * of a broken tier's, and SAVED_TARGETS reads them back */
#define TIER_STATE_BITS 2
#define SAVED_TARGETS(task, index)                                             \
    ((placement_get_state(task) >> ((index) * TIER_STATE_BITS)) & BOTH_TARGETS)
/* nonzero once the tier has broken */
#define BROKEN(work) ((work)->timer)
/* the tier's targets that are gone, hurt or never made, as target_bits */
#define MISSING_TARGETS(work) ((work)->submode)
/* a tier's place in TIERS, and BELOW the tier under it */
#define INDEX(work) ((work)->ang.x)
#define BELOW(task, work) (TIERS((task)->parent)[INDEX(work) - 1])
/* the tier above a tier rests TIER_HEIGHT above that tier's pos */
#define BROKEN_HEIGHT 15.0F
#define TIER_HEIGHT(work) (BROKEN(work) ? BROKEN_HEIGHT : WHOLE_HEIGHT)
/* the mode tier_take_mode gives the tier at its next step, or NO_NEXT_MODE */
#define NEXT_MODE(task) ((task)->unk_3c.l)
#define NO_NEXT_MODE (-1)
#define FALL_SPEED(work) ((work)->scl.y)
#define GRAVITY 0.08F
#define DRAG 0.997F
#define TINT_PHASE(work) ((work)->ang.z)
/* how far the tint has blended toward its phase's end, from 0 to 1 */
#define TINT_MIX(work) ((work)->scl.z)
/* the tier's tint, an ARGB color held in scl.x as a Float's bits */
#define TINT(work) (*(Uint32 *)&(work)->scl.x)
/* a hurt target sets bit TARGET_SHIFT, its tick, of its tier's
 * MISSING_TARGETS. tier_init means the shift to be 0 for the lower target and
 * 1 for the upper. where it makes both targets, it stores both shifts through
 * lower, which swaps the two */
#define TARGET_SHIFT(work) ((work)->tick)
#define TIER_SHAPES 1
#define TARGET_SHAPES 1
#define PARTS 7
#define FRAGMENT_FLAGS (DEBRIS_TRAIL | DEBRIS_LANDING_BURST | DEBRIS_VANISH)
/* a tier's break and a target's both play BREAK_SOUND */
#define BREAK_SOUND 0x8015
#define BREAK_PRIORITY 0
/* @hack spelling: SUPPORT_WORK and REST_INDEX, which read the tier's work
 * through the local in Hidden Base's build and through task->work in Sand
 * Ocean's, where one spelling in both builds would do. Hidden Base's
 * REST_INDEX also sets the local from tier */
#ifdef HIDDEN_BASE_STACK
#define SUPPORT_WORK(task, work) (work)
#define REST_INDEX(task, work, tier) INDEX((work) = (tier))
#else
#define SUPPORT_WORK(task, work) ((task)->work)
#define REST_INDEX(task, work, tier) INDEX((task)->work)
#endif
/* the targets stand LOWER_HEIGHT and UPPER_HEIGHT above their tier's pos */
#ifdef HIDDEN_BASE_STACK
#define WHOLE_HEIGHT 45.0F
#define LOWER_HEIGHT 25.0F
#define UPPER_HEIGHT 35.0F
#define IMPACT_SOUND 0
#define SHAKE_STEPS 1
#define SHAKE_SCALE 40.0F
#else
#define WHOLE_HEIGHT 70.0F
#define LOWER_HEIGHT 35.0F
#define UPPER_HEIGHT 50.0F
#define IMPACT_SOUND 3
#endif
#define IMPACT_PRIORITY 1
#define IMPACT_DURATION 90
/* a landing's dust: puffs of size DUST_SCALE along the edges of a square of
 * side 2 * DUST_HALF */
#define DUST_HALF 5.0F
#define DUST_SCALE 4.0F
#define DUST_DENSITY 2.0F
#define DUST_SPEED 1.0F
/* the camera a landing shakes, and the player whose distance from the tier
 * divides the shake's scale. Hidden Base's shake takes SHAKE_STEPS and
 * SHAKE_SCALE, and Sand Ocean's shake_steps and shake_scale */
#define SHAKE_CAMERA 0
#define SHAKE_PLAYER 0
/* a breaking tier's fragments start FRAGMENT_HEIGHT above its pos, and their
 * floor probe PROBE_LIFT above that */
#define FRAGMENT_HEIGHT 25.0F
#define PROBE_LIFT 1.0F
/* the burst a hurt target leaves at its place: a puff of dust and a flash */
#define HURT_DUST_SCALE 6.0F
#define HURT_FLASH_SCALE 20.0F
#define HURT_SPARKS 3
#define HURT_FLASH_DEPTH_BIAS 30.0F
/* the lift of the stack's shadow above its pos */
#define SHADOW_LIFT 0.01F

/* a tier's spring, at task->motion, which lifts the tier's draw by
 * displacement.y. each step the velocity keeps SPRING_KEEP of itself and
 * loses SPRING_PULL of the displacement, by axis. a hurt target kicks the
 * velocity up by TARGET_KICK */
struct tier_spring {
    NJS_VECTOR velocity;
    NJS_VECTOR displacement;
};
#define SPRING(task) ((struct tier_spring *)(task)->motion.p)
#define SPRING_KEEP_X 0.82F
#define SPRING_KEEP_Y 0.8F
#define SPRING_KEEP_Z 0.82F
#define SPRING_PULL_X 0.63F
#define SPRING_PULL_Y 0.62F
#define SPRING_PULL_Z 0.68F
#define TARGET_KICK 1.0F

static void stack_destroy(struct task *task);
static void stack_step(struct task *task);
static void stack_draw(struct task *task);
static void tier_init(struct task *task);
static void tier_destroy(struct task *task);
static void tier_step(struct task *task);
static void tier_draw(struct task *task);
static void tier_take_mode(struct task *task);
/* @hack redundant: stack_tier_queue_mode's external linkage, which no call
 * needs, where static would do. SHC would expand its every call in place and
 * leave out the body the image holds */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void stack_tier_queue_mode(struct task *task, int mode);
static void tier_still(struct task *task);
static void tier_fall(struct task *task);
static void tier_rebound(struct task *task);
static void tier_break(struct task *task);
static void target_init(struct task *task);
static void target_destroy(struct task *task);
static void target_step(struct task *task);
static void tier_update_color(struct task *task);

static struct collision_shape whole_shape = {
#include "shared/stacked_platform_task/whole_shape.inc"
};
static struct collision_shape base_shape = {
#include "shared/stacked_platform_task/base_shape.inc"
};
static Float rebound_share = 0.11F;
static Uint32 dark_tint = {
#include "shared/stacked_platform_task/dark_tint.inc"
};
static Uint32 light_tint = {
#include "shared/stacked_platform_task/light_tint.inc"
};
static Uint32 broken_tint = {
#include "shared/stacked_platform_task/broken_tint.inc"
};
static Uint32 fragment_tint = {
#include "shared/stacked_platform_task/fragment_tint.inc"
};
static Float smoke_chance = {
#include "shared/stacked_platform_task/smoke_chance.inc"
};
static Float smoke_size = {
#include "shared/stacked_platform_task/smoke_size.inc"
};
static Float smoke_spread = {
#include "shared/stacked_platform_task/smoke_spread.inc"
};
static NJS_VECTOR smoke_speed = {
#include "shared/stacked_platform_task/smoke_speed.inc"
};
static Float smoke_height = {
#include "shared/stacked_platform_task/smoke_height.inc"
};
static Float smoke_radius = {
#include "shared/stacked_platform_task/smoke_radius.inc"
};
static Angle smoke_angle = {
#include "shared/stacked_platform_task/smoke_angle.inc"
};
static Sint32 break_volume = 75;
static Sint32 target_volume = 50;
static Sint32 impact_volume = 50;
static void (*tier_steps[])(struct task *task) = {tier_still, tier_fall,
                                                  tier_rebound};
#ifndef HIDDEN_BASE_STACK
static Float shake_scale = 40.0F;
static Sint32 shake_steps = 1;
#endif
static Float fragment_speed = 1.2F;
static Float fragment_spread = 0.6F;
static Float burst_size = 24.0F;
#ifdef HIDDEN_BASE_STACK
static Float burst_height = 30.0F;
#define BURST_HEIGHT burst_height
#else
#define BURST_HEIGHT 35.0F
#endif
static struct collision_shape target_shape = {
#include "shared/stacked_platform_task/target_shape.inc"
};

void stacked_platform_init(struct task *task)
{
    struct task_work *work = task->work;
    int index;
    Float height;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = stack_draw;
    task->step = stack_step;
    task->destructor = stack_destroy;
    STACK_TIERS(task->work) = 0;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    /* @hack spelling: syCalloc's size before its count, where the count first
     * would allocate the same */
    if ((task->extra.p =
             syCalloc(sizeof(struct task *), TIER_COUNT(task->work))) == NULL) {
        /* @bug memory: where syCalloc has no room for the tiers, the stack
         * keeps its step, which reads the list of tiers through null.
         * stack_destroy tests the list for null. in Sand Ocean, 0x8c50505c
         * stores the null, and 0x8c505268 reads through it */
#ifdef NONMATCHING
        /* this side repairs the tag above: it destroys the platform, and
         * set.c makes it again on a later pass */
        task_destroy(task);
#endif
        return;
    }
    /* @hack spelling: task->work read for the count, where work would do */
    STACK_TIERS(task->work) = TIER_COUNT(task->work);
    height = work->pos.y;
    for (index = 0; index < TIER_COUNT(task->work); index++) {
        if ((TIERS(task)[index] = task_create_child(TASK_WORK | TASK_MOTION,
                                                    tier_init, task)) == NULL) {
            task_destroy(task);
            return;
        }
        /* @hack spelling: the setup in an else after the return, where
         * statements after the if would do */
        else {
            INDEX(TIERS(task)[index]->work) = index;
            /* @bug logic: tiers five to eight come back broken exactly when
             * tier four was. a stack holds up to eight tiers, but the state's
             * byte holds TIER_STATE_BITS for each of four. SAVED_TARGETS
             * shifts the byte sign-extended, so both bits of a later tier are
             * copies of the byte's top bit. in Sand Ocean, 0x8c505054 allows
             * eight tiers, and 0x8c505112 and 0x8c505132 shift the byte */
            BROKEN(TIERS(task)[index]->work) =
                SAVED_TARGETS(task, index) == BOTH_TARGETS;
            MISSING_TARGETS(TIERS(task)[index]->work) =
                SAVED_TARGETS(task, index);
            TIERS(task)[index]->work->pos = work->pos;
            TIERS(task)[index]->work->pos.y = height;
            TIERS(task)[index]->work->ang.y = work->ang.y;
            height += TIER_HEIGHT(TIERS(task)[index]->work);
        }
    }
}

static void stack_destroy(struct task *task)
{
    if (task->extra.p != NULL) {
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

static void stack_step(struct task *task)
{
    Sint8 bits;
    int index;

    if (placement_try_leave(task)) {
        return;
    }
    bits = placement_get_state(task);
    for (index = 0; index < TIER_COUNT(task->work); index++) {
        if (BROKEN(TIERS(task)[index]->work)) {
            bits |= BOTH_TARGETS << (index * TIER_STATE_BITS);
        }
    }
    placement_set_state(task, bits);
}

static void stack_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njTranslate(NULL, 0.0F, SHADOW_LIFT, 0.0F);
#ifndef HIDDEN_BASE_STACK
    njSetTexture(&stack_shadow_texlist);
    njCnkEasyDrawModel(&stack_shadow);
#endif
    njPopMatrixEx();
}

static void tier_init(struct task *task)
{
    struct task *lower;
    struct task *upper;
    Bool collision_ready;

#ifdef NONMATCHING
    /* this side starts lower at null, which its repair of the memory bug tagged
     * below tests */
    lower = NULL;
#endif
    task->draw = tier_draw;
    task->step = tier_step;
    task->destructor = tier_destroy;
    if (BROKEN(task->work)) {
        collision_ready = collision_init(task, &base_shape, TIER_SHAPES,
                                         COLLISION_OBJECT_MUTUAL);
    } else {
        if (!(MISSING_TARGETS(task->work) & LOWER_TARGET)) {
            lower = task_create_child(TASK_WORK, target_init, task);
#ifdef NONMATCHING
            /* this side gives up the tier where the lower target can't be made.
             * the matching side writes through the null */
            if (lower == NULL) {
                goto failed;
            }
#endif
            lower->work->scl.y = LOWER_HEIGHT;
            TARGET_SHIFT(lower->work) = 0;
        }
        if (!(MISSING_TARGETS(task->work) & UPPER_TARGET)) {
            upper = task_create_child(TASK_WORK, target_init, task);
#ifdef NONMATCHING
            /* this side gives up the tier where the upper target can't be made.
             * the matching side writes through the null */
            if (upper == NULL) {
                goto failed;
            }
#endif
            upper->work->scl.y = UPPER_HEIGHT;
            /* @bug memory: the upper target's shift is stored through lower,
             * not upper. where MISSING_TARGETS holds LOWER_TARGET, no lower
             * target was made, and lower is unset. in Sand Ocean, 0x8c50536e
             * skips the store to lower, and 0x8c5053ac stores through it.
             * where both targets are made, each then sets the other's bit of
             * MISSING_TARGETS */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: without a lower target,
             * the upper target takes the bit */
            if (lower == NULL) {
                lower = upper;
            }
#endif
            TARGET_SHIFT(lower->work) = 1;
        }
        collision_ready = collision_init(task, &whole_shape, TIER_SHAPES,
                                         COLLISION_OBJECT_MUTUAL);
    }
#ifdef NONMATCHING
    /* this side gives up the tier where its shape can't be made */
    if (!collision_ready) {
        goto failed;
    }
#else
    (void)collision_ready;
#endif
    task->work->mode = TIER_STILL;
    NEXT_MODE(task) = NO_NEXT_MODE;
    FALL_SPEED(task->work) = 0.0F;
    TINT(task->work) = COLOR_NONE;
    TINT_MIX(task->work) = 0.0F;
    TINT_PHASE(task->work) = TINT_CLEAR;
#ifdef NONMATCHING
    return;

failed:
    /* GCC's build destroys the whole stack where a tier fails. that takes
     * every tier with it, and the targets made so far */
    task_destroy(task->parent);
#endif
}

static void tier_destroy(struct task *task)
{
}

static void tier_step(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_VECTOR speed;
    Float x;
    Float z;
    Float vx;
    Float vz;

    tier_take_mode(task);
    tier_steps[task->work->mode](task);
    if (!BROKEN(task->work) && MISSING_TARGETS(task->work)) {
        int missing;
        /* @hack spelling: the assignment of missing inside the first test,
         * where a statement of its own would do */
        if (((missing = MISSING_TARGETS(task->work)) == LOWER_TARGET ||
             missing == UPPER_TARGET) &&
            rand() * RAND_SCALE < smoke_chance) {
            pos = work->pos;
            ocean_polar_xz(work->ang.y + smoke_angle, smoke_radius, &x, &z);
            pos.x += x;
            pos.y += smoke_height;
            pos.z += z;
            speed = smoke_speed;
            ocean_polar_xz(work->ang.y + smoke_angle, speed.z, &vx, &vz);
            speed.x = vx;
            speed.z = vz;
            dust_short_create(&pos, &speed,
                              smoke_size +
                                  (rand() * smoke_spread * RAND_SCALE));
            pos.x = work->pos.x - x;
            pos.z = work->pos.z - z;
            speed.x = -vx;
            speed.z = -vz;
            dust_short_create(&pos, &speed,
                              smoke_size +
                                  (rand() * smoke_spread * RAND_SCALE));
        }
    }
    if (!BROKEN(task->work) && MISSING_TARGETS(task->work) == BOTH_TARGETS) {
        tier_break(task);
        BROKEN(task->work) = TRUE;
        return;
    }
    collision_register(task);
    SPRING(task)->displacement.x += SPRING(task)->velocity.x;
    SPRING(task)->displacement.y += SPRING(task)->velocity.y;
    SPRING(task)->displacement.z += SPRING(task)->velocity.z;
    SPRING(task)->velocity.x = (SPRING(task)->velocity.x * SPRING_KEEP_X) -
                               (SPRING(task)->displacement.x * SPRING_PULL_X);
    SPRING(task)->velocity.y = (SPRING(task)->velocity.y * SPRING_KEEP_Y) -
                               (SPRING(task)->displacement.y * SPRING_PULL_Y);
    SPRING(task)->velocity.z = (SPRING(task)->velocity.z * SPRING_KEEP_Z) -
                               (SPRING(task)->displacement.z * SPRING_PULL_Z);
    tier_update_color(task);
}

static void tier_draw(struct task *task)
{
    struct task *parent = task->parent;
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njTranslate(NULL, 0.0F, SPRING(task)->displacement.y, 0.0F);
    if (BROKEN(task->work)) {
        draw_control_set(DRAW_CONTROL_SHADOW);
        njSetTexture(base_texlists[TIER_KIND(parent->work)]);
        ocean_model_draw_tinted_stack(bases[TIER_KIND(parent->work)],
                                      broken_tint);
        draw_control_clear(DRAW_CONTROL_SHADOW);
    } else {
        njSetTexture(base_texlists[TIER_KIND(parent->work)]);
        njCnkEasyDrawModel(bases[TIER_KIND(parent->work)]);
        njSetTexture(shell_texlists[TIER_KIND(parent->work)]);
        ocean_model_draw_tinted_stack(shells[TIER_KIND(parent->work)],
                                      TINT(task->work));
    }
    njPopMatrixEx();
}

static void tier_take_mode(struct task *task)
{
    if (NEXT_MODE(task) != NO_NEXT_MODE) {
        task->work->mode = NEXT_MODE(task);
        NEXT_MODE(task) = NO_NEXT_MODE;
    }
}

void stack_tier_queue_mode(struct task *task, int mode)
{
#ifndef HIDDEN_BASE_STACK
    /* @hack redundant: a block no path reaches, which increments mode */
    if (0.0F) {
        mode++;
    }
#endif
    NEXT_MODE(task) = mode;
}

static void tier_still(struct task *task)
{
#ifdef HIDDEN_BASE_STACK
    /* @hack spelling: work set from tier inside the test of the index, where
     * work set at its declaration would do */
    struct task_work *tier = task->work;
    struct task_work *work;
#else
    struct task_work *work = task->work;
#endif
    struct task *below;
    Float floor;

    if (REST_INDEX(task, work, tier) > 0) {
        /* @hack spelling: the assignment of below inside the read, where a
         * statement of its own would do */
        floor = (below = BELOW(task, task->work))->work->pos.y;
        floor += TIER_HEIGHT(below->work);
        if (work->pos.y > floor) {
            stack_tier_queue_mode(task, TIER_FALLING);
        }
    }
}

static void tier_fall(struct task *task)
{
    struct task_work *work = task->work;
    struct task *below;
    /* @hack spelling: the assignment of below inside floor's initializer,
     * where a statement of its own would do */
    Float floor = (below = BELOW(task, SUPPORT_WORK(task, work)))->work->pos.y;
    Float distance;
#ifndef HIDDEN_BASE_STACK
    int steps;
#endif

    floor += TIER_HEIGHT(below->work);
    /* @hack spelling: task->work read again below, where work would do */
    FALL_SPEED(work) -= GRAVITY;
    FALL_SPEED(task->work) *= DRAG;
    work->pos.y += FALL_SPEED(task->work);
    if (work->pos.y <= floor) {
        work->pos.y = floor;
        FALL_SPEED(task->work) = -(FALL_SPEED(task->work) * rebound_share);
        dust_rect(&work->pos, DUST_HALF, DUST_HALF, &work->ang, DUST_SCALE,
                  DUST_DENSITY, DUST_SPEED);
        stack_tier_queue_mode(task, TIER_REBOUND);
        distance = njDistanceP2P(&work->pos, &player_work[SHAKE_PLAYER]->pos);
#ifdef HIDDEN_BASE_STACK
        /* @hack redundant: distance assigned to itself */
        distance = distance; /* NOLINT(clang-diagnostic-self-assign) */
        camera_shake(SHAKE_CAMERA, SHAKE_STEPS, SHAKE_SCALE / distance);
#else
        /* @hack spelling: steps, a local for shake_steps, where shake_steps
         * in place would do */
        steps = shake_steps;
        camera_shake(SHAKE_CAMERA, steps, shake_scale / distance);
#endif
        sound_play_at_for(IMPACT_SOUND, SOUND_SECOND_KEY(task), IMPACT_PRIORITY,
                          impact_volume, IMPACT_DURATION, &work->pos);
    }
}

static void tier_rebound(struct task *task)
{
#ifdef HIDDEN_BASE_STACK
    /* @hack spelling: work copied from tier, where work set from task->work
     * would do */
    struct task_work *tier = task->work;
    struct task_work *work = tier;
#else
    struct task_work *work = task->work;
#endif
    struct task *below;
    /* @hack spelling: the assignment of below inside floor's initializer,
     * where a statement of its own would do */
    Float floor = (below = BELOW(task, SUPPORT_WORK(task, work)))->work->pos.y;

    floor += TIER_HEIGHT(below->work);
    /* @hack spelling: task->work read again below, where work would do */
    FALL_SPEED(work) -= GRAVITY;
    FALL_SPEED(task->work) *= DRAG;
    work->pos.y += FALL_SPEED(task->work);
    if (work->pos.y <= floor) {
        work->pos.y = floor;
        stack_tier_queue_mode(task, TIER_STILL);
    }
}

static void tier_break(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR speed;
    NJS_VECTOR burst_speed;
    Float floor;

    pos = work->pos;
    pos.y += FRAGMENT_HEIGHT;
    speed.x = 0.0F;
    speed.y = 0.0F;
    speed.z = 0.0F;
    floor = surface_probe_floor(pos.x, pos.y + PROBE_LIFT, pos.z, &ang);
    debris_break_flags(&pos, work->ang.y, fragments, PARTS, fragment_speed,
                       fragment_spread, floor, &speed, FRAGMENT_FLAGS);
    burst_speed.x = burst_speed.y = burst_speed.z = 0.0F;
    pos = work->pos;
    pos.y += BURST_HEIGHT;
    burst_create_wobbling(&pos, &burst_speed, burst_size);
    collision_free(task);
    collision_init(task, &base_shape, TIER_SHAPES, COLLISION_OBJECT_MUTUAL);
    TINT(task->work) = light_tint;
    sound_play_at(BREAK_SOUND, NULL, BREAK_PRIORITY, break_volume, &work->pos);
}

static void target_init(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *work = task->work;

    task->step = target_step;
    task->destructor = target_destroy;
    work->pos = parent->work->pos;
    /* @hack spelling: task->work read again for the offset, where work would
     * do */
    work->pos.x += task->work->scl.x;
    work->pos.y += task->work->scl.y;
    work->pos.z += task->work->scl.z;
#ifdef NONMATCHING
    /* this side destroys the platform where the target's shape or work can't be
     * made */
    if (!collision_init(task, &target_shape, TARGET_SHAPES, COLLISION_ITEM) ||
        enemy_work_alloc(task) == NULL) {
        task_destroy(parent->parent);
    }
#else
    collision_init(task, &target_shape, TARGET_SHAPES, COLLISION_ITEM);
    enemy_work_alloc(task);
#endif
}

static void target_destroy(struct task *task)
{
    enemy_destroy(task);
}

static void target_step(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *work = task->work;
    struct task_work *parent_work = parent->work;
    NJS_VECTOR speed;
    NJS_POINT3 pos;

    work->pos = parent_work->pos;
    /* @hack spelling: task->work read again for the offset, where work would
     * do */
    work->pos.x += task->work->scl.x;
    work->pos.y += task->work->scl.y;
    work->pos.z += task->work->scl.z;
    if (work->flags & WORK_HURT) {
        MISSING_TARGETS(parent->work) |= 1 << TARGET_SHIFT(task->work);
        speed.x = 0.0F;
        speed.y = 0.0F;
        speed.z = 0.0F;
        pos = work->pos;
        dust_create(&pos, &speed, HURT_DUST_SCALE);
        flash_manager_create_sparks_depth(pos.x, pos.y, pos.z, HURT_FLASH_SCALE,
                                          HURT_SPARKS, HURT_FLASH_DEPTH_BIAS);
        SPRING(parent)->velocity.y += TARGET_KICK;
        task_kill(task);
        sound_play_at(BREAK_SOUND, NULL, BREAK_PRIORITY, target_volume,
                      &work->pos);
    } else {
        collision_register(task);
    }
}

static void tier_update_color(struct task *task)
{
    switch (TINT_PHASE(task->work)) {
    case TINT_CLEAR:
        if (MISSING_TARGETS(task->work)) {
            TINT_PHASE(task->work) = TINT_BEGIN;
            TINT_MIX(task->work) = 0.0F;
            TINT(task->work) = COLOR_NONE;
        }
        break;
    case TINT_BEGIN:
        TINT_MIX(task->work) += TINT_STEP;
        if (TINT_MIX(task->work) >= 1.0F) {
            TINT_MIX(task->work) = 0.0F;
            TINT(task->work) = light_tint;
            TINT_PHASE(task->work) = TINT_DARKEN;
        } else {
            TINT(task->work) =
                color_blend(COLOR_NONE, light_tint, TINT_MIX(task->work));
        }
        break;
    case TINT_DARKEN:
        TINT_MIX(task->work) += DARKEN_STEP;
        if (TINT_MIX(task->work) >= 1.0F) {
            TINT_MIX(task->work) = 0.0F;
            TINT(task->work) = dark_tint;
            TINT_PHASE(task->work) = TINT_LIGHTEN;
        } else {
            TINT(task->work) =
                color_blend(light_tint, dark_tint, TINT_MIX(task->work));
        }
        break;
    case TINT_LIGHTEN:
        TINT_MIX(task->work) += TINT_STEP;
        if (TINT_MIX(task->work) >= 1.0F) {
            TINT_MIX(task->work) = 0.0F;
            TINT(task->work) = light_tint;
            TINT_PHASE(task->work) = TINT_DARKEN;
        } else {
            TINT(task->work) =
                color_blend(dark_tint, light_tint, TINT_MIX(task->work));
        }
        break;
    }
}

void stacked_platform_tint_parts(void)
{
    ocean_model_tint(&tier_fragment_0, fragment_tint);
    ocean_model_tint(&tier_fragment_1, fragment_tint);
    ocean_model_tint(&tier_fragment_2, fragment_tint);
    ocean_model_tint(&tier_fragment_3, fragment_tint);
    ocean_model_tint(&tier_fragment_4, fragment_tint);
    ocean_model_tint(&tier_fragment_5, fragment_tint);
    ocean_model_tint(&tier_fragment_6, fragment_tint);
}

#endif
