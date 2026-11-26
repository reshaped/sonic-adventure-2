/* the cocoon a Chao closes in to change its form or to die. the cocoon grows
 * and turns opaque, then fades and shrinks away. it is a child task of the
 * Chao */
#include <ninja.h>
#include <sg_xpt.h>
#include "chao/actor.h"
#include "chao/behavior.h"
#include "chao/body.h"
#include "chao/body_collision.h"
#include "chao/cocoon.h"
#include "chao/egg.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/garden_save.h"
#include "chao/genes.h"
#include "chao/icon.h"
#include "chao/identity.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/parts.h"
#include "chao/residents.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chao/world.h"
#include "draw/lines.h"
#include "object/animal.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

#define CHAOS_AFFECTION_THRESHOLD 80
#define CHAOS_REBIRTHS 2 /* the rebirths a Chao needs to become a Chaos */
/* a cocoon's work: its size, its opacity, and its opacity's change a frame */
#define SIZE(work) ((work)->scl.x)
#define OPACITY(work) ((work)->scl.y)
#define FADE_STEP(work) ((work)->scl.z)
/* a growing cocoon is whole past WHOLE_SIZE, and a shrinking one is gone below
 * GONE_SIZE. it draws DRAW_SCALE times its size */
#define WHOLE_SIZE 0.99F
#define GONE_SIZE 0.1F
#define DRAW_SCALE 1.3F
/* how much longer a dying Chao's cocoon waits after it fades */
#define DEATH_LINGER 30

enum cocoon_mode {
    COCOON_START,
    COCOON_GROW,
    COCOON_CLOSE,
    COCOON_CLOSED,
    COCOON_OPEN,
    COCOON_SHRINK
};

#ifdef NONMATCHING
/* this side makes the egg before it changes the Chao. where it can't, it
 * returns FALSE and leaves the Chao and its save as they were. the matching
 * side clears the saved Chao and unregisters this one first */
static Bool chao_reincarnate(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_gene gene = work->gene;
    struct chao_profile profile = work->profile;

    profile.parameters.reincarnations++;
    profile.parameters.reborn_egg = TRUE;
    chao_learn_traits(&gene, &profile);
    if (chao_create_egg_with_profile(&gene, &profile, &work->base.pos,
                                     EGG_REBORN, work->save) == NULL) {
        return FALSE;
    }
    work->gene = gene;
    work->profile.parameters.reincarnations = profile.parameters.reincarnations;
    work->profile.parameters.reborn_egg = TRUE;
    work->save = NULL;
    garden_unregister_object(chao);
    return TRUE;
}
#else
static void chao_reincarnate(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_gene *gene = &work->gene;
    struct chao_profile *profile = &work->profile;

    profile->parameters.reincarnations++;
    chao_learn_traits(gene, profile);
    if (work->save != NULL) {
        garden_clear_chao(work->save);
    }
    garden_unregister_object(chao);
    profile->parameters.reborn_egg = TRUE;
    chao_create_egg_with_profile(gene, profile, &chao->work->pos, EGG_REBORN,
                                 work->save);
    work->save = NULL;
}
#endif

int chao_hatch_pop_behavior(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    /* @hack spelling: a one-case switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_UMARERU_PON, 15);
        behavior->mode++;
        break;
    }
    if (chao_motion_ended(chao)) {
        return BEHAVIOR_FINISH;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_hatch_look_behavior(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    /* @hack spelling: a one-case switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_UMARERU_KYORO, 15);
        behavior->mode++;
        break;
    }
    if (chao_motion_ended(chao)) {
        return BEHAVIOR_FINISH;
    }
    return BEHAVIOR_CONTINUE;
}

static Uint8 choose_adult_form(struct task *chao)
{
    struct chao_parameters *parameters = &CHAO_WORK(chao)->profile.parameters;
    Float run_power = parameters->run_power;
    Float fly_swim = parameters->fly_swim;
    Float alignment = parameters->alignment;
    Float balanced = chao_tuning.balanced_form_range;
    Float neutral = chao_tuning.neutral_alignment_range;
    int form;
    Bool has_all;
    int i;

    if (run_power > -balanced && run_power < balanced && fly_swim > -balanced &&
        fly_swim < balanced) {
        has_all = TRUE;
        form = CHAO_NORMAL_ADULT;
        for (i = 0; i < ANIMAL_KINDS; i++) {
            if (!chao_has_eaten_minimal(chao, i)) {
                has_all = FALSE;
                break;
            }
        }
        if (has_all && parameters->affection > CHAOS_AFFECTION_THRESHOLD &&
            parameters->reincarnations >= CHAOS_REBIRTHS) {
            form = CHAO_CHAOS;
        }
    } else if (fly_swim > run_power) {
        if (fly_swim > -run_power) {
            form = CHAO_FLY_ADULT;
        } else {
            form = CHAO_RUN_ADULT;
        }
    } else if (fly_swim > -run_power) {
        form = CHAO_POWER_ADULT;
    } else {
        form = CHAO_SWIM_ADULT;
    }
    if (alignment <= -neutral || alignment >= neutral) {
        if (alignment > 0.0F) {
            form++;
            garden_flag_set(GARDEN_HERO_OPEN);
            jingle_play("chao_g_born_h2.adx");
        } else {
            form += 2;
            garden_flag_set(GARDEN_DARK_OPEN);
            jingle_play("chao_g_born_d2.adx");
        }
    }
    parameters->type = form;
    parameters->evolution = 0.0F;
    parameters->run_power = 0.0F;
    parameters->fly_swim = 0.0F;
    return form;
}

static void cocoon_step(struct task *cocoon)
{
    struct task_work *work = cocoon->work;
    if (cocoon->parent != NULL) {
        work->pos = cocoon->parent->work->pos;
    }
    switch (work->mode) {
    case COCOON_START:
        OPACITY(work) = chao_tuning.cocoon_min_alpha;
        work->mode = COCOON_GROW;
        work->submode = 0;
        /* fall through */
    case COCOON_GROW:
        SIZE(work) += (1.0F - SIZE(work)) * chao_tuning.cocoon_growth_rate;
        if (SIZE(work) > WHOLE_SIZE) {
            SIZE(work) = 1.0F;
            FADE_STEP(work) = (1.0F - chao_tuning.cocoon_min_alpha) /
                              chao_tuning.cocoon_close_frames;
            work->mode = COCOON_CLOSE;
            work->submode = 0;
        }
        break;
    case COCOON_CLOSE:
        OPACITY(work) += FADE_STEP(work);
        if (OPACITY(work) > 1.0F) {
            OPACITY(work) = 1.0F;
            FADE_STEP(work) = (chao_tuning.cocoon_min_alpha - 1.0F) /
                              chao_tuning.cocoon_fade_frames;
            work->mode = COCOON_CLOSED;
            work->submode = 0;
        }
        sound_play_at_for(0x101c, cocoon, 1, -5, 60, &work->pos);
        break;
    case COCOON_CLOSED:
        break;
    case COCOON_OPEN:
        OPACITY(work) += FADE_STEP(work);
        if (OPACITY(work) < chao_tuning.cocoon_min_alpha) {
            sound_play_at(0x101d, NULL, 0, -5, &work->pos);
            OPACITY(work) = chao_tuning.cocoon_min_alpha;
            work->mode = COCOON_SHRINK;
            work->submode = 0;
        }
        break;
    case COCOON_SHRINK:
        SIZE(work) = SIZE(work) * chao_tuning.cocoon_shrink_rate;
        if (SIZE(work) < GONE_SIZE) {
            task_kill(cocoon);
        }
        break;
    }
}

static void cocoon_draw(struct task *cocoon)
{
    struct task_work *work = cocoon->work;
    Float scale = SIZE(work) * DRAW_SCALE;
    Float alpha = OPACITY(work);

    draw_control_save();
    draw_attr_save();
    if (alpha < 1.0F) {
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        draw_attr_set(0, NJD_FST_UA);
        draw_attr_clear(0, STRIP_FLAGS_BUT_UA);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        constant_material_set(alpha - 1.0F, 0.0F, 0.0F, 0.0F);
    }
    njSetTexture(&egg_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njScale(NULL, scale, scale, scale);
    njCnkEasyDrawObject(&cocoon_model);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
}

static void cocoon_destroy(struct task *cocoon)
{
    if (cocoon->parent != NULL) {
        CHAO_WORK(cocoon->parent)->cocoon = NULL;
    }
}

static void cocoon_create(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct task *cocoon;
    if (work->cocoon == NULL) {
        cocoon = task_create_child(TASK_WORK, cocoon_step, chao);
#ifdef NONMATCHING
        /* this side makes no cocoon where task_create_child fails. the matching
         * side writes through the null */
        if (cocoon == NULL) {
            return;
        }
#endif
        cocoon->draw = cocoon_draw;
        cocoon->destructor = cocoon_destroy;
        work->cocoon = cocoon;
    }
}

static void cocoon_open(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct task_work *shell;
    /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    if (work->cocoon != NULL) {
        shell = work->cocoon->work;
    }
    /* @bug uninitialized: the mode is written through shell, which no path
     * sets where the Chao has no cocoon. shell then holds what the caller
     * left in R5 (0x0c52fcc6) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: without a cocoon, it returns */
    else {
        return;
    }
#endif
    shell->mode = COCOON_OPEN;
}

enum cocoon_phase {
    COCOON_PREPARE,
    COCOON_WAIT_TO_CLOSE,
    COCOON_WAIT_TO_CHANGE,
    COCOON_WAIT_TO_OPEN,
    COCOON_WAIT_TO_FADE,
    COCOON_WAIT_TO_FINISH
};

#pragma inline(cocoon_enter_phase)
static void cocoon_enter_phase(struct chao_behavior *behavior, int next)
{
    behavior->mode = next;
    behavior->timer = 0;
    behavior->count = 0;
}

#ifdef NONMATCHING
/* enter COCOON_WAIT_TO_CHANGE once the cocoon exists. where its task can't
 * be made, the Chao waits chao_tuning.cocoon_start_frames again */
static void cocoon_begin_change(struct task *chao,
                                struct chao_behavior *behavior)
{
    struct chao_work *work = CHAO_WORK(chao);

    cocoon_create(chao);
    if (work->cocoon != NULL) {
        cocoon_enter_phase(behavior, COCOON_WAIT_TO_CHANGE);
    } else {
        behavior->timer = 0;
    }
}
#endif

int chao_evolve_behavior(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    switch (behavior->mode) {
    case COCOON_PREPARE:
        chao_resist_displacement(chao);
        chao_disable_body_shapes(chao);
        switch (chao_motion_pose(chao)) {
        case CHAO_STANDING:
            chao_link_motion_frames(chao, CHAO_ANIM_GENKI_SUWARI, 15);
            break;
        case CHAO_ON_BACK:
            chao_link_motion_frames(chao, CHAO_ANIM_OKIRU, 15);
            break;
        case CHAO_ON_FRONT:
            chao_link_motion_frames(chao, CHAO_ANIM_OKIRU, 35);
            break;
        case CHAO_SITTING:
        default:
            chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT, 15);
            break;
        }
        cocoon_enter_phase(behavior, COCOON_WAIT_TO_CLOSE);
        /* fall through */
    case COCOON_WAIT_TO_CLOSE:
        if ((Uint32)behavior->timer++ > chao_tuning.cocoon_start_frames) {
#ifdef NONMATCHING
            /* this side waits for the change only once cocoon_begin_change has
             * made the cocoon. otherwise it starts the timer again */
            cocoon_begin_change(chao, behavior);
#else
            cocoon_enter_phase(behavior, COCOON_WAIT_TO_CHANGE);
            cocoon_create(chao);
#endif
        }
        break;
    case COCOON_WAIT_TO_CHANGE:
        if ((Uint32)behavior->timer++ > chao_tuning.cocoon_close_frames) {
            chao_change_form(chao, choose_adult_form(chao));
            cocoon_enter_phase(behavior, COCOON_WAIT_TO_OPEN);
        }
        break;
    case COCOON_WAIT_TO_OPEN:
        if ((Uint32)behavior->timer++ > chao_tuning.cocoon_hold_frames) {
            cocoon_enter_phase(behavior, COCOON_WAIT_TO_FADE);
            cocoon_open(chao);
            /* @hack spelling: the type read through chao->work, where work
             * would do */
            switch (CHAO_WORK(chao)->profile.parameters.type) {
            case CHAO_CHAOS:
            case CHAO_HERO_CHAOS:
            case CHAO_DARK_CHAOS:
                jingle_play("chao_g_born_c.adx");
                break;
            default:
                if (chao_is_hero(chao)) {
                    jingle_play("chao_g_born_h.adx");
                } else if (chao_is_dark(chao)) {
                    jingle_play("chao_g_born_d.adx");
                }
                break;
            }
        }
        break;
    case COCOON_WAIT_TO_FADE:
        if ((Uint32)behavior->timer++ > chao_tuning.cocoon_fade_frames) {
            cocoon_enter_phase(behavior, COCOON_WAIT_TO_FINISH);
        }
        break;
    case COCOON_WAIT_TO_FINISH:
        if ((Uint32)behavior->timer++ > chao_tuning.cocoon_end_frames) {
            chao_allow_displacement(chao);
            chao_enable_body_shapes(chao);
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

static Bool can_reincarnate(struct task *chao)
{
    struct chao_parameters *parameters = &CHAO_WORK(chao)->profile.parameters;
    if (parameters->affection > chao_tuning.rebirth_affection) {
        if (chao_emotion_get(chao, CHAO_STRESS) <
            chao_tuning.rebirth_stress_limit) {
            return TRUE;
        }
    }
    return FALSE;
}

int chao_die_behavior(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    switch (behavior->mode) {
    case COCOON_PREPARE:
        chao_resist_displacement(chao);
        chao_disable_body_shapes(chao);
        chao_set_timed_eye(chao, 3, CHAO_FACE_HOLD);
        chao_set_timed_mouth(chao, 4, CHAO_FACE_HOLD);
        chao_show_mark(chao, CHAO_MARK_ANGER, 300);
        switch (chao_motion_pose(chao)) {
        case CHAO_STANDING:
            chao_link_motion_frames(chao, CHAO_ANIM_GENKI_SUWARI, 15);
            break;
        case CHAO_SITTING:
            chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT, 15);
            break;
        case CHAO_ON_FRONT:
            chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT, 45);
            break;
        case CHAO_ON_BACK:
            chao_link_motion_frames(chao, CHAO_ANIM_OKIRU, 15);
            break;
        }
        cocoon_enter_phase(behavior, COCOON_WAIT_TO_CLOSE);
        /* fall through */
    case COCOON_WAIT_TO_CLOSE:
        if ((Uint32)behavior->timer++ > chao_tuning.cocoon_start_frames) {
#ifdef NONMATCHING
            /* this side waits for the change only once cocoon_begin_change has
             * made the cocoon. otherwise it starts the timer again */
            cocoon_begin_change(chao, behavior);
#else
            cocoon_enter_phase(behavior, COCOON_WAIT_TO_CHANGE);
            cocoon_create(chao);
#endif
        }
        break;
    case COCOON_WAIT_TO_CHANGE:
        if ((Uint32)behavior->timer++ > chao_tuning.cocoon_close_frames) {
            if (can_reincarnate(chao)) {
#ifdef NONMATCHING
                /* this side starts the timer again where chao_reincarnate can't
                 * make the egg, and tries at its end */
                if (!chao_reincarnate(chao)) {
                    behavior->timer = 0;
                    return BEHAVIOR_CONTINUE;
                }
#else
                chao_reincarnate(chao);
#endif
            } else {
                jingle_play("chao_g_dead.adx");
            }
            CHAO_WORK(chao)->flags &= ~CHAO_VISIBLE;
            cocoon_enter_phase(behavior, COCOON_WAIT_TO_OPEN);
        }
        break;
    case COCOON_WAIT_TO_OPEN:
        if ((Uint32)behavior->timer++ > chao_tuning.cocoon_hold_frames) {
            cocoon_enter_phase(behavior, COCOON_WAIT_TO_FADE);
            cocoon_open(chao);
        }
        break;
    case COCOON_WAIT_TO_FADE:
        if ((Uint32)behavior->timer++ >
            chao_tuning.cocoon_fade_frames + DEATH_LINGER) {
            chao_remove_resident(chao);
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
