/* a garden's Chao egg. left to rest, it cracks and hatches after 100 to 189
 * seconds. once player 0 has shaken it enough, it hatches 1 to 3 seconds
 * after it comes to rest. its kindness alleles then rise, and its aggression
 * alleles fall. an egg that hits a wall fast enough breaks open, and its
 * alleles move the other way */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "chao/actor.h"
#include "chao/behavior.h"
#include "chao/held.h"
#include "chao/body.h"
#include "chao/body_collision.h"
#include "chao/bounce.h"
#include "chao/cocoon.h"
#include "chao/egg.h"
#include "chao/emotion.h"
#include "chao/garden.h"
#include "chao/genes.h"
#include "chao/ground.h"
#include "chao/identity.h"
#include "chao/life.h"
#include "chao/movement.h"
#include "chao/residents.h"
#include "chao/toy_art.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "draw/lines.h"
#include "object/fall.h"
#include "object/mover.h"
#include "player/player.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* EGG_HATCH_POP opens the egg by egg_hatch_motion_206 while its Chao pops
 * out by CHAO_ANIM_UMARERU_PON. EGG_HATCH_LOOK opens it by
 * egg_hatch_motion_205 while its Chao looks about by CHAO_ANIM_UMARERU_KYORO */
enum egg_mode {
    EGG_REST = 0,
    EGG_ROLL = 1,
    EGG_HELD = 2,
    EGG_CRACK = 3,
    EGG_HATCH_POP = 4,
    EGG_HATCH_LOOK = 6,
    EGG_BREAK = 8,
    EGG_EMPTY = 9
};
/* EGG_HATCH_SPACE is on only while the egg cracks, and keeps everything but
 * pickups off it */
enum egg_shape { EGG_PICKUP, EGG_CONTACT, EGG_HATCH_SPACE, EGG_SHAPES };

#define SHELL_SHADOW (NJD_CONTROL_3D_SHADOW | NJD_CONTROL_3D_TRANS_MODIFIER)
/* a held egg cracks once it has started or stopped moving SHAKES_NEEDED
 * times. it starts as its acceleration rises past MOVING_ACCELERATION. it
 * stops as its acceleration falls below STILL_ACCELERATION */
#define SHAKES_NEEDED 8
#define MOVING_ACCELERATION 0.2F
#define STILL_ACCELERATION 0.1F
/* a let-go egg keeps RELEASE_DAMPING of its speed across */
#define RELEASE_DAMPING 0.7F
/* the garden's shadow under an egg or a loose top, scaled SHADOW_WIDTH
 * across and SHADOW_HEIGHT up */
#define SHADOW_WIDTH 1.8F
#define SHADOW_HEIGHT 0.7F
/* an egg floats FLOAT_DEPTH under a flat surface, as water's. a rolling egg
 * rests after more than REST_STEPS steps in a row on the floor slower than
 * REST_SPEED */
#define FLOAT_DEPTH 2.5F
#define REST_SPEED 0.05F
#define REST_STEPS 30
/* a cracking egg shakes CRACK_SHAKES times, or MORE_CRACK_SHAKES with chance
 * 0.4, and may sound once each CRACK_SOUND_INTERVAL steps */
#define CRACK_SHAKES 2
#define MORE_CRACK_SHAKES 3
#define CRACK_SOUND_INTERVAL 30
#define MOTION_STEP 0.16F
#define SHELL_DROP_FRAME 15.0F
#define TOP_ALLELE 4
#define BREAK_SPEED 0.8F
#define SOUND_CRACKING 0x100f
#define SOUND_HATCH 0x1010
#define SOUND_BREAK 0x1011
/* @hack type: the alleles read as Sint8, where the gene's Uint8 would do for
 * alleles of 0 to 4 */
#define PERSONALITY_GENE(egg, kind)                                            \
    ((Sint8 *)CHAO_WORK(egg)->gene.personality[(kind) - CHAO_CURIOSITY])
/* the shell's top drops SHELL_DROP_HEIGHT above the egg, at the egg's speed.
 * it goes SHELL_LIFT faster up. along x and z it goes up to SHELL_SCATTER
 * faster either way */
#define SHELL_DROP_HEIGHT 1.5F
#define SHELL_LIFT 1.3F
#define SHELL_SCATTER 0.2F
/* drop the shell's top as headgear at position, at a random yaw */
#define DROP_SHELL(position, velocity)                                         \
    (velocity).y += SHELL_LIFT;                                                \
    (velocity).x +=                                                            \
        SHELL_SCATTER - (rand() * ((2.0F * SHELL_SCATTER) * RAND_SCALE));      \
    (velocity).z +=                                                            \
        SHELL_SCATTER - (rand() * ((2.0F * SHELL_SCATTER) * RAND_SCALE));      \
    garden_drop_headgear(GARDEN_HEADGEAR_SHELL, &(position),                   \
                         (int)RANDOM_TURN(), &(velocity))

/* clang-format off */
static const struct collision_shape egg_shapes[EGG_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_HURTABLE | SHAPE_UNK_2000, {0.0F, 1.5F, 0.0F}, 15.0F, 3.0F, 0.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_ENEMIES | PUSH_OBJECTS, SHAPE_GUARD_LEVEL(3), 0,
     {0.0F, 2.0F, 0.0F}, 2.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_OFF, {0.0F, 2.0F, 0.0F}, 2.0F, 0.0F, 0.0F, 0,
     {0, 0, 0}}};
/* clang-format on */

/* a Chao reborn from an egg keeps REBORN_SHARE of its run_power, fly_swim and
 * alignment. it keeps REBORN_EVOLUTION_SHARE of its evolution. it keeps its
 * hat with chance REBORN_KEEPS, and its costume the same */
#define REBORN_SHARE 0.2F
#define REBORN_EVOLUTION_SHARE 0.1F
#define REBORN_KEEPS 0.3F
static struct task *egg_hatch(struct task *egg)
{
    struct chao_work *work = CHAO_WORK(egg);
    struct chao_parameters *previous = &work->profile.parameters;
    struct chao_parameters *newborn;
    struct task *chao;

#ifdef NONMATCHING
    /* this side makes the Chao before it lets go of the egg. where
     * chao_create fails, the egg stays registered and its save as it was */
    chao = chao_create(&work->gene, NULL, NULL, &work->base.pos,
                       work->base.ang.y, work->save);
    if (chao == NULL) {
        return NULL;
    }
    garden_unregister_object(egg);
#else
    if (work->save != NULL) {
        garden_clear_chao(work->save);
    }
    garden_unregister_object(egg);
    chao = chao_create(&work->gene, NULL, NULL, &work->base.pos,
                       work->base.ang.y, work->save);
#endif
    if (previous->reborn_egg) {
        newborn = &CHAO_WORK(chao)->profile.parameters;
        newborn->affection = previous->affection;
        if (previous->name[0]) {
            njMemCopy(newborn->name, previous->name, sizeof(newborn->name));
        }
        newborn->reincarnations = previous->reincarnations;
        if (previous->unk_2c[0]) {
            njMemCopy(newborn->unk_2c, previous->unk_2c,
                      sizeof(newborn->unk_2c));
        }
        newborn->run_power = previous->run_power * REBORN_SHARE;
        newborn->fly_swim = previous->fly_swim * REBORN_SHARE;
        newborn->alignment = previous->alignment * REBORN_SHARE;
        newborn->evolution = previous->evolution * REBORN_EVOLUTION_SHARE;
        njMemCopy(newborn->personal_records, previous->personal_records,
                  sizeof(newborn->personal_records));
        if (rand() * RAND_SCALE < REBORN_KEEPS) {
            newborn->hat = previous->hat;
        }
        if (rand() * RAND_SCALE < REBORN_KEEPS) {
            newborn->costume = previous->costume;
        }
        newborn->medal = previous->medal;
        newborn->color = previous->color;
        newborn->texture = previous->texture;
        newborn->shiny = previous->shiny;
        newborn->body = previous->body;
        newborn->minimal = previous->minimal;
        chao_morph_body(chao);
    }
    chao->work->ang.y = work->base.ang.y;
    work->save = NULL;
    return chao;
}

enum shell_mode { SHELL_START, SHELL_SOLID, SHELL_FADE };
/* a loose top stays solid more than SHELL_SOLID_STEPS. it then fades by
 * SHELL_FADE_STEP a step, and goes at SHELL_FADE_END */
#define SHELL_SOLID_STEPS 120
#define SHELL_FADE_STEP 0.04F
#define SHELL_FADE_END 0.01F
#define SHELL_OPACITY(work) ((work)->scl.x)

static void egg_shell_top_step(struct task *shell)
{
    struct task_work *work = shell->work;
    switch (work->mode) {
    case SHELL_START:
        SHELL_OPACITY(work) = 1.0F;
        work->mode++;
        work->submode = 0;
        /* fall through */
    case SHELL_SOLID:
        if (work->timer++ > SHELL_SOLID_STEPS) {
            work->mode++;
            work->submode = 0;
        }
        break;
    case SHELL_FADE:
        SHELL_OPACITY(work) -= SHELL_FADE_STEP;
        if (SHELL_OPACITY(work) < SHELL_FADE_END) {
            SHELL_OPACITY(work) = SHELL_FADE_END;
            task_kill(shell);
        }
        break;
    }
    mover_step(shell);
    mover_save_pos(shell);
    garden_meet_floor(shell);
}

static void egg_shell_top_draw(struct task *shell)
{
    struct task_work *work = shell->work;
    Float opacity = SHELL_OPACITY(work);

    draw_control_save();
    draw_attr_save();
    if (opacity < 1.0F) {
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        draw_attr_set(0, NJD_FST_UA);
        draw_attr_clear(0, STRIP_FLAGS_BUT_UA);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        constant_material_set(opacity - 1.0F, 0.0F, 0.0F, 0.0F);
    }
    njSetTexture(&egg_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(SHELL_SHADOW);
    njCnkEasyDrawModel(egg_shell_top->model);
    draw_control_clear(SHELL_SHADOW);
    njTranslate(NULL, 0.0F, 0.5F, 0.0F);
    if (opacity > 0.2F) {
        njScale(NULL, opacity * SHADOW_WIDTH, SHADOW_HEIGHT,
                opacity * SHADOW_WIDTH);
        njCnkModDrawModel(&garden_shadow_model);
    }
    njPopMatrix(1);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
}

static void egg_shell_top_destroy(struct task *shell)
{
}

/* a loose top of an egg's shell, which falls and bounces, and then fades. it
 * is external, since SHC leaves out an uncalled static function */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
struct task *egg_create_shell_top(NJS_POINT3 *position)
{
    struct task *shell;
    struct task_work *work;
    struct mover *mover;

    shell = task_create(TASK_WORK, 2, egg_shell_top_step, "AL_EggTopExecutor");
#ifdef NONMATCHING
    /* this side returns null where task_create fails. the matching side writes
     * through the null */
    if (shell == NULL) {
        return NULL;
    }
#endif
    work = shell->work;
    mover = mover_alloc(shell);
#ifdef NONMATCHING
    /* this side destroys the shell and returns null where mover_alloc fails.
     * the matching side writes through the null */
    if (mover == NULL) {
        task_destroy(shell);
        return NULL;
    }
#endif
    work->pos = *position;
    mover_set_gravity(shell, -0.07F);
    mover->probe_height = 1.5F;
    mover->unk_b4 = 1.5F;
    mover->bottom = -1.5F;
    mover->floor_bounce = 0.6F;
    mover->friction = 0.85F;
    mover->hold_turn = 0x700;
    mover_save_pos(shell);
    shell->destructor = egg_shell_top_destroy;
    shell->draw = egg_shell_top_draw;
    return shell;
}

/* garden_bounce_on_floor for an egg. it is external, since SHC leaves out an
 * uncalled static function */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void egg_bounce_floor(struct task *egg)
{
    garden_bounce_on_floor(egg);
}

/* a resting egg settles for SETTLE_STEPS and a random part of SETTLE_SPAN
 * before its kindness grows */
#define SETTLE_STEPS 60
#define SETTLE_SPAN 120.0F
static void egg_rest(struct task *egg)
{
    struct task_work *work = egg->work;
    struct egg_state *state = egg->extra.p;

    switch (work->submode) {
    case 0:
        collision_shape_on(egg, EGG_PICKUP);
        collision_shape_on(egg, EGG_CONTACT);
        collision_shape_off(egg, EGG_HATCH_SPACE);
        work->timer = (int)(rand() * (SETTLE_SPAN * RAND_SCALE)) + SETTLE_STEPS;
        /* @hack spelling: submode + 1 assigned, where ++ would do */
        work->submode = work->submode + 1;
        break;
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        work->mode = EGG_HELD;
        work->submode = 0;
        return;
    }
    if (state->shakes_left == 0 && work->timer-- == 0) {
        if (PERSONALITY_GENE(egg, CHAO_KINDNESS)[0] < TOP_ALLELE) {
            PERSONALITY_GENE(egg, CHAO_KINDNESS)[0]++;
        }
        if (PERSONALITY_GENE(egg, CHAO_KINDNESS)[1] < TOP_ALLELE) {
            PERSONALITY_GENE(egg, CHAO_KINDNESS)[1]++;
        }
        if (PERSONALITY_GENE(egg, CHAO_AGGRESSION)[0] > 0) {
            PERSONALITY_GENE(egg, CHAO_AGGRESSION)[0]--;
        }
        if (PERSONALITY_GENE(egg, CHAO_AGGRESSION)[1] > 0) {
            PERSONALITY_GENE(egg, CHAO_AGGRESSION)[1]--;
        }
        work->mode = EGG_CRACK;
        work->submode = 0;
    }
    if (state->rest_frames-- < 0) {
        work->mode = EGG_CRACK;
        work->submode = 0;
    }
    if (collision_pushed(egg)) {
        work->mode = EGG_ROLL;
        work->submode = 0;
    }
}

static void egg_roll(struct task *egg)
{
    struct task_work *work = egg->work;
    struct mover *mover = egg->motion.p;
    Float speed;

    /* @hack redundant: a test of garden_objects_running with nothing under it,
     * where the bare call would do */
    if (garden_objects_running(egg)) {
    }
    switch (work->submode) {
    case 0:
        collision_shape_on(egg, EGG_PICKUP);
        collision_shape_on(egg, EGG_CONTACT);
        collision_shape_off(egg, EGG_HATCH_SPACE);
        work->submode++;
        work->timer = 0;
        break;
    }
    garden_apply_buoyancy(egg, FLOAT_DEPTH);
    if (CHAO_WORK(egg)->flags & CHAO_MOVES) {
        mover_step(egg);
    }
    mover_save_pos(egg);
    garden_meet_floor(egg);
    if (mover->flags & MOVER_AT_WALL) {
        speed = njScalor(&mover->spd);
        if (speed > BREAK_SPEED) {
            work->mode = EGG_BREAK;
            work->submode = 0;
        }
    }
    if (work->flags & MOVER_WORK_ON_FLOOR) {
        if (njScalor(&mover->spd) < REST_SPEED) {
            if (work->timer++ > REST_STEPS) {
                mover_clear_spd(egg);
                mover_clear_acc(egg);
                work->mode = EGG_REST;
                work->submode = 0;
            }
        } else {
            work->timer = 0;
        }
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        work->mode = EGG_HELD;
        work->submode = 0;
    }
}

static void egg_count_shakes(struct task *egg)
{
    /* @hack redundant: work, which nothing reads */
    struct task_work *work = egg->work;
    Float speed = njScalor(&((struct mover *)egg->motion.p)->acc);
    struct egg_state *state = egg->extra.p;

    if (speed < STILL_ACCELERATION && state->moving) {
        state->moving = FALSE;
        state->shakes_left--;
    }
    if (speed > MOVING_ACCELERATION && !state->moving) {
        state->moving = TRUE;
        state->shakes_left--;
    }
    if (state->shakes_left < 0) {
        state->shakes_left = 0;
    }
}

static void egg_held(struct task *egg)
{
    struct task_work *work = egg->work;
    struct mover *mover = egg->motion.p;

    switch (work->submode) {
    case 0:
        collision_shape_off(egg, EGG_PICKUP);
        collision_shape_off(egg, EGG_CONTACT);
        mover->aim_y = work->ang.y + player_work[0]->ang.y;
        work->submode++;
        break;
    }
    egg_count_shakes(egg);
    /* @hack spelling: the negated heading added, where aim_y less the
     * heading would do */
    work->ang.y = -player_work[0]->ang.y + mover->aim_y;
    if (!(work->flags & WORK_HELD_BY_PLAYER)) {
        mover->spd.x *= RELEASE_DAMPING;
        mover->spd.z *= RELEASE_DAMPING;
        mover_clear_acc(egg);
        mover_save_pos(egg);
        work->mode = EGG_ROLL;
        work->submode = 0;
    }
}

static void egg_crack(struct task *egg)
{
    struct task_work *work = egg->work;
    struct egg_state *state = egg->extra.p;

    switch (work->submode) {
    case 0:
        collision_shape_off(egg, EGG_PICKUP);
        collision_shape_off(egg, EGG_CONTACT);
        /* @hack spelling: egg->work read again, here and for both sounds
         * below, where work holds it */
        egg->work->flags &= ~(FALL_UNK_200 | FALL_OFFERABLE);
        collision_shape_on(egg, EGG_HATCH_SPACE);
        work->submode++;
        if (rand() * RAND_SCALE < 0.6F) {
            work->tick = CRACK_SHAKES;
        } else {
            work->tick = MORE_CRACK_SHAKES;
        }
        break;
    }
    state->frame += MOTION_STEP;
    if (state->frame > egg_shake_motion->nbFrame) {
        state->frame -= egg_shake_motion->nbFrame;
        work->tick--;
        if (work->tick == 0) {
            sound_play_at(SOUND_HATCH, NULL, 0, 0, &egg->work->pos);
            if (rand() * RAND_SCALE < 0.5F) {
                work->mode = EGG_HATCH_POP;
                work->submode = 0;
            } else {
                work->mode = EGG_HATCH_LOOK;
                work->submode = 0;
            }
        }
    }
    if (work->timer++ % CRACK_SOUND_INTERVAL == 0 &&
        rand() * RAND_SCALE < 0.5F) {
        sound_play_at(SOUND_CRACKING, NULL, 0, 0, &egg->work->pos);
    }
    mover_step(egg);
    mover_save_pos(egg);
    garden_meet_floor(egg);
}

#ifdef NONMATCHING
/* this side's hatch steps. each tries again the next step where egg_hatch or
 * the shell's drop fails. the matching side sets a behavior on egg_hatch's
 * null. it goes on from a shell that didn't drop */
static int drop_shell(struct task_work *work, NJS_VECTOR velocity)
{
    NJS_POINT3 position = work->pos;
    position.y += SHELL_DROP_HEIGHT;
    velocity.y += SHELL_LIFT;
    velocity.x +=
        SHELL_SCATTER - (rand() * ((2.0F * SHELL_SCATTER) * RAND_SCALE));
    velocity.z +=
        SHELL_SCATTER - (rand() * ((2.0F * SHELL_SCATTER) * RAND_SCALE));
    return garden_drop_headgear(GARDEN_HEADGEAR_SHELL, &position,
                                (int)RANDOM_TURN(), &velocity) != NULL;
}

static void finish_hatch(struct task *egg)
{
    struct egg_state *state = egg->extra.p;
    NJS_VECTOR velocity = {0.0F, 0.0F, 0.0F};
    if ((state->frame += MOTION_STEP) > SHELL_DROP_FRAME) {
        if (!drop_shell(egg->work, velocity)) {
            state->frame = SHELL_DROP_FRAME;
            return;
        }
        egg->work->mode = EGG_EMPTY;
        egg->work->submode = 0;
    }
}

static int start_hatch_behavior(struct task *egg, chao_behavior_fn behavior)
{
    struct task *chao = egg_hatch(egg);
    if (!chao) {
        return FALSE;
    }
    chao_set_behavior(chao, behavior);
    return TRUE;
}
#endif

static void egg_hatch_pop(struct task *egg)
{
    struct task_work *work = egg->work;
    struct egg_state *state = egg->extra.p;
    switch (work->submode) {
    case 0:
        collision_shape_off(egg, EGG_PICKUP);
        collision_shape_off(egg, EGG_CONTACT);
        collision_shape_off(egg, EGG_HATCH_SPACE);
#ifdef NONMATCHING
        /* this side tries the hatch again the next step where egg_hatch makes
         * no Chao. the matching side sets the behavior through the null */
        if (!start_hatch_behavior(egg, chao_hatch_pop_behavior)) {
            return;
        }
#else
        chao_set_behavior(egg_hatch(egg), chao_hatch_pop_behavior);
#endif
        state->frame = 0.0F;
        work->submode++;
        work->tick = 0;
        break;
    }
#ifdef NONMATCHING
    /* this side drops the shell by finish_hatch, which tries again the next
     * step where the drop fails */
    finish_hatch(egg);
#else
    if ((state->frame += MOTION_STEP) > SHELL_DROP_FRAME) {
        NJS_POINT3 position;
        NJS_VECTOR velocity = {0.0F, 0.0F, 0.0F};
        position = work->pos;
        position.y += SHELL_DROP_HEIGHT;
        DROP_SHELL(position, velocity);
        work->mode = EGG_EMPTY;
        work->submode = 0;
    }
#endif
}

static void egg_hatch_look(struct task *egg)
{
    struct task_work *work = egg->work;
    struct egg_state *state = egg->extra.p;
    switch (work->submode) {
    case 0:
        collision_shape_off(egg, EGG_PICKUP);
        collision_shape_off(egg, EGG_CONTACT);
        collision_shape_off(egg, EGG_HATCH_SPACE);
#ifdef NONMATCHING
        /* this side tries the hatch again the next step where egg_hatch makes
         * no Chao. the matching side sets the behavior through the null */
        if (!start_hatch_behavior(egg, chao_hatch_look_behavior)) {
            return;
        }
#else
        chao_set_behavior(egg_hatch(egg), chao_hatch_look_behavior);
#endif
        state->frame = 0.0F;
        work->submode++;
        work->tick = 0;
        break;
    }
#ifdef NONMATCHING
    /* this side drops the shell by finish_hatch, which tries again the next
     * step where the drop fails */
    finish_hatch(egg);
#else
    if ((state->frame += MOTION_STEP) > SHELL_DROP_FRAME) {
        NJS_POINT3 position;
        NJS_VECTOR velocity = {0.0F, 0.0F, 0.0F};
        position = work->pos;
        position.y += SHELL_DROP_HEIGHT;
        DROP_SHELL(position, velocity);
        work->mode = EGG_EMPTY;
        work->submode = 0;
    }
#endif
}

static void egg_break(struct task *egg)
{
    struct task_work *work = egg->work;
    struct mover *mover = egg->motion.p;
    struct task *chao;
    NJS_VECTOR *newborn_velocity;
#ifdef NONMATCHING
    /* the gene this side puts back where the hatch fails. SHELL_PENDING is the
     * submode of a shell still to drop */
    enum { SHELL_PENDING = 2 };
    struct chao_gene previous_gene;
#else
    NJS_POINT3 position;
    NJS_VECTOR velocity;
#endif
    chao_behavior_fn behavior;

    switch (work->submode) {
    case 0:
        collision_shape_off(egg, EGG_PICKUP);
        collision_shape_off(egg, EGG_CONTACT);
        collision_shape_off(egg, EGG_HATCH_SPACE);
#ifdef NONMATCHING
        /* this side keeps the gene, which the lines below change, for a hatch
         * that fails */
        previous_gene = CHAO_WORK(egg)->gene;
#endif
        if (PERSONALITY_GENE(egg, CHAO_KINDNESS)[0] > 0) {
            PERSONALITY_GENE(egg, CHAO_KINDNESS)[0]--;
        }
        if (PERSONALITY_GENE(egg, CHAO_KINDNESS)[1] > 0) {
            PERSONALITY_GENE(egg, CHAO_KINDNESS)[1]--;
        }
        if (PERSONALITY_GENE(egg, CHAO_AGGRESSION)[0] < TOP_ALLELE) {
            PERSONALITY_GENE(egg, CHAO_AGGRESSION)[0]++;
        }
        if (PERSONALITY_GENE(egg, CHAO_AGGRESSION)[1] < TOP_ALLELE) {
            PERSONALITY_GENE(egg, CHAO_AGGRESSION)[1]++;
        }
        chao = egg_hatch(egg);
#ifdef NONMATCHING
        /* this side puts the gene back and tries again the next step where
         * egg_hatch makes no Chao. the matching side writes through the null */
        if (!chao) {
            CHAO_WORK(egg)->gene = previous_gene;
            return;
        }
#endif
        newborn_velocity = &((struct mover *)chao->motion.p)->spd;
        *newborn_velocity = mover->spd;
        newborn_velocity->y += 1.0F;
        /* @hack spelling: the behavior through a local, where the call could
         * name it */
        behavior = chao_knockback_behavior;
        chao_set_behavior(chao, behavior);
#ifdef NONMATCHING
        /* the Chao has hatched and owns the save. where drop_shell fails,
         * the next step tries the drop again, and nothing else */
        /* fall through */
    case SHELL_PENDING:
        if (!drop_shell(work, mover->spd)) {
            work->submode = SHELL_PENDING;
            return;
        }
#else
        position = work->pos;
        position.y += SHELL_DROP_HEIGHT;
        velocity = mover->spd;
        DROP_SHELL(position, velocity);
#endif
        /* @hack spelling: egg->work read again, where work holds it */
        sound_play_at(SOUND_BREAK, NULL, 0, 0, &egg->work->pos);
        mover->spd.y += 0.7F;
#ifdef NONMATCHING
        /* this side sets submode 1, as the matching side's ++ does from 0. this
         * side also comes here from SHELL_PENDING */
        work->submode = 1;
#else
        work->submode++;
#endif
        break;
    }
    mover_step(egg);
    mover_save_pos(egg);
    garden_meet_floor(egg);
    if ((work->flags & MOVER_WORK_ON_FLOOR) && njScalor(&mover->spd) < 0.3F) {
        work->mode = EGG_EMPTY;
        work->submode = 0;
    }
}

static void egg_empty(struct task *egg)
{
    struct task_work *work = egg->work;

    switch (work->submode) {
    case 0:
        collision_shape_off(egg, EGG_PICKUP);
        collision_shape_off(egg, EGG_CONTACT);
        collision_shape_off(egg, EGG_HATCH_SPACE);
        work->submode++;
        break;
    }
}

static void egg_step(struct task *egg)
{
    struct task_work *work = egg->work;
    struct egg_state *state = egg->extra.p;

    switch (work->mode) {
    case EGG_REST:
        egg_rest(egg);
        break;
    case EGG_ROLL:
        egg_roll(egg);
        break;
    case EGG_HELD:
        egg_held(egg);
        break;
    case EGG_CRACK:
        egg_crack(egg);
        break;
    case EGG_HATCH_POP:
        egg_hatch_pop(egg);
        break;
    case EGG_HATCH_LOOK:
        egg_hatch_look(egg);
        break;
    case EGG_BREAK:
        egg_break(egg);
        break;
    case EGG_EMPTY:
        egg_empty(egg);
        break;
    }
    if (state->size < 1.0F) {
        state->size += 0.06F;
    }
    state->wobble_speed += (1.0F - state->wobble) * 0.15F;
    state->wobble += state->wobble_speed;
    state->wobble_speed *= 0.9F;
    if (state->collision_delay <= 0) {
        if (state->size > 0.8F) {
            collision_register(egg);
        }
    } else {
        state->collision_delay--;
    }
}

static void egg_draw(struct task *egg)
{
    struct egg_state *state = egg->extra.p;
    struct task_work *work = egg->work;
    Float width = state->wobble * state->size;
    Float height = (2.0F - state->wobble) * state->size;
    struct garden_entry *entry;

    if (garden_visible_at_height(egg, 3.0F, 2.5F)) {
        njSetTexture(&egg_texlist);
        njPushMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        if (work->flags & WORK_HELD_BY_PLAYER) {
            njTranslate(NULL, 0.0F, -2.2F, 0.0F);
        }
        if (garden_hit_kind(egg, EGG_CONTACT, GARDEN_SHADE_HIT)) {
            draw_control_set(SHELL_SHADOW);
        }
        njRotateY(NULL, work->ang.y);
        njScale(NULL, width, height, width);
        switch (work->mode) {
        case EGG_CRACK:
            njCnkEasyDrawMotion(egg_whole_model, egg_shake_motion,
                                state->frame);
            break;
        case EGG_HATCH_POP:
            njCnkEasyDrawMotion(egg_open_model, &egg_hatch_motion_206,
                                state->frame);
            break;
        case EGG_HATCH_LOOK:
            njCnkEasyDrawMotion(egg_open_model, &egg_hatch_motion_205,
                                state->frame);
            break;
        case EGG_BREAK:
        case EGG_EMPTY:
            njCnkEasyDrawObject(egg_shell_bottom);
            break;
        default:
            njCnkEasyDrawObject(egg_whole_model);
            break;
        }
        draw_control_clear(SHELL_SHADOW);
        /* @hack spelling: two nested tests, where one && would do */
        entry = egg->force.p;
        if (entry != NULL) {
            if (entry->distance < chao_tuning.large_object_shadow_distance) {
                if (garden_hit_kind(egg, EGG_CONTACT, GARDEN_SHADE_HIT)) {
                    njTranslate(NULL, 0.0F, 0.01F, 0.0F);
                } else {
                    njTranslate(NULL, 0.0F, 0.5F, 0.0F);
                }
                njScale(NULL, SHADOW_WIDTH, SHADOW_HEIGHT, SHADOW_WIDTH);
                njCnkModDrawModel(&garden_shadow_model);
            }
        }
        njPopMatrix(1);
    }
}

static void egg_destroy(struct task *egg)
{
    garden_unregister_object(egg);
}

/* an egg rests REST_SECONDS and a random part of REST_SECONDS_SPAN before it
 * cracks. a laid egg starts at LAID_SIZE, its wobble at LAID_WOBBLE. a reborn
 * egg collides REBORN_DELAY frames after its cocoon's hold and fade, and a
 * transferred one after TRANSFER_DELAY */
#define REST_SECONDS 100
#define REST_SECONDS_SPAN 90.0F
#define LAID_SIZE 0.1F
#define LAID_WOBBLE 0.2F
#define REBORN_DELAY 35
#define TRANSFER_DELAY 60
struct task *chao_create_egg_with_profile(struct chao_gene *gene,
                                          struct chao_profile *profile,
                                          NJS_POINT3 *position,
                                          enum chao_egg_origin origin,
                                          struct chao_save *save)
{
    struct task *egg;
    struct chao_work *work;
    struct mover *mover;
    struct egg_state *state;
    int garden;
    struct chao_parameters *parameters;

    egg = task_create(0, 2, egg_step, "EGG");
#ifdef NONMATCHING
    /* this side returns null where task_create fails. the matching side writes
     * through the null */
    if (egg == NULL) {
        return NULL;
    }
#endif
    egg->work = syCalloc(1, sizeof(*work));
    work = CHAO_WORK(egg);
#ifdef NONMATCHING
    /* this side destroys the egg and returns null where the heap has no work or
     * collision_init fails */
    if (work == NULL ||
        !collision_init(egg, egg_shapes, EGG_SHAPES, COLLISION_OBJECT_MUTUAL)) {
        task_destroy(egg);
        return NULL;
    }
#else
    collision_init(egg, egg_shapes, EGG_SHAPES, COLLISION_OBJECT_MUTUAL);
#endif
    mover = mover_alloc(egg);
#ifdef NONMATCHING
    /* this side destroys the egg and returns null where mover_alloc fails. the
     * matching side writes through the null */
    if (mover == NULL) {
        task_destroy(egg);
        return NULL;
    }
#endif
    work->base.pos = *position;
    work->base.ang.y = RANDOM_TURN();
    mover_set_gravity(egg, -0.07F);
    mover->probe_height = 3.0F;
    mover->unk_b4 = 1.5F;
    mover->bottom = -3.0F;
    mover->floor_bounce = 0.7F;
    mover->friction = 0.85F;
    if (player_character(0) != PLAYER_EGGMAN) {
        mover->hold_turn = 0x700;
    } else {
        mover->hold_turn = -0xd00;
    }
    mover_save_pos(egg);
    fall_set_kind(&work->base, egg->motion.p, FALL_KIND_NORMAL);
    egg->destructor = egg_destroy;
    egg->draw = egg_draw;
    work->base.mode = EGG_ROLL;
    work->base.submode = 0;
#ifdef NONMATCHING
#else
    garden_register_object(GARDEN_OBJECT_EGG, egg, 0);
#endif
    egg->extra.p = state = syCalloc(1, sizeof(*state));
#ifdef NONMATCHING
    /* this side registers the egg only once its state is made. it destroys the
     * egg where the state or the slot fails */
    if (state == NULL || !garden_register_object(GARDEN_OBJECT_EGG, egg, 0)) {
        task_destroy(egg);
        return NULL;
    }
#endif
    state->origin = origin;
    state->unk_0c = 0;
    state->shakes_left = SHAKES_NEEDED;
    state->rest_frames =
        ((int)(rand() * (REST_SECONDS_SPAN * RAND_SCALE)) + REST_SECONDS) *
        STEPS_PER_SECOND;
    switch (origin) {
    case EGG_RESIDENT:
        state->wobble = 1.0F;
        state->size = 1.0F;
        break;
    case EGG_LAID:
        state->wobble = LAID_WOBBLE;
        state->size = LAID_SIZE;
        break;
    case EGG_REBORN:
        state->wobble = 1.0F;
        state->size = 1.0F;
        state->collision_delay = chao_tuning.cocoon_hold_frames +
                                 chao_tuning.cocoon_fade_frames + REBORN_DELAY;
        break;
    case EGG_TRANSFERRED:
        state->wobble = 1.0F;
        state->size = 1.0F;
        state->collision_delay = TRANSFER_DELAY;
        break;
    }
    if (gene != NULL) {
        work->gene = *gene;
    } else {
        chao_new_gene(&work->gene);
    }
    if (profile != NULL) {
        work->profile = *profile;
    }
    work->profile.parameters.type = CHAO_EGG;
    garden = chao_current_area();
    if (garden >= CHAO_AREA_NEUTRAL && garden <= CHAO_AREA_DARK) {
        work->profile.parameters.garden = garden;
    } else {
        /* @bug memory: outside the three gardens, the save's garden is read
         * at 0x0c508b2e before save is tested for null at 0x0c508b40 */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: without a save, the garden is
         * CHAO_AREA_NONE. it drops the sum the hack tag names */
        parameters = save != NULL ? &save->record.saved.parameters : NULL;
        garden = parameters != NULL ? parameters->garden : CHAO_AREA_NONE;
#else
        parameters = &save->record.saved.parameters;
        /* @hack redundant: a sum of two fields, which the next line
         * overwrites */
        garden = parameters->affection + parameters->classroom_slot;
        garden = parameters->garden;
#endif
        if (garden >= CHAO_AREA_NEUTRAL && garden <= CHAO_AREA_DARK) {
            work->profile.parameters.garden = garden;
        }
    }
    if (save != NULL) {
        work->save = save;
        garden_pack_egg(egg, save);
    }
    /* @hack spelling: the flags reached through egg->work, where work holds
     * the same */
    CHAO_WORK(egg)->flags |= CHAO_MOVES;
    return egg;
}

struct task *chao_create_egg(struct chao_gene *gene, NJS_POINT3 *position,
                             enum chao_egg_origin origin,
                             struct chao_save *save)
{
    return chao_create_egg_with_profile(gene, NULL, position, origin, save);
}
