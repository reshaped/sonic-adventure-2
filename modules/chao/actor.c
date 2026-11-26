/* a garden Chao's task: its shapes, step, draw and destructor, and the calls
 * that create and remove it */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "chao/actor.h"
#include "chao/behavior.h"
#include "chao/body.h"
#ifdef NONMATCHING
/* chao_matrix_slot_available, by which this side's draw tests the matrix
 * stack */
#include "chao/body_matrix.h"
#endif
#include "chao/body_collision.h"
#include "chao/garden.h"
#include "chao/face.h"
#include "chao/genes.h"
#include "chao/ground.h"
#include "chao/identity.h"
#include "chao/icon.h"
#include "chao/minimal.h"
#include "chao/model.h"
#include "chao/motion.h"
#include "chao/parts.h"
#include "chao/perception.h"
#include "chao/race_chao.h"
#include "chao/residents.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "draw/motion.h"
#include "object/fall.h"
#include "object/mover.h"
#include "player/player.h"
#include "system/task.h"

#define SOLID_CONTACT (SHAPE_HURTABLE | SHAPE_UNK_2000)
#define SENSOR_CONTACT                                                         \
    (SHAPE_NO_OWN_HITS | SHAPE_UNK_2000 | SHAPE_IGNORE_ALL_BUT_PLAYERS)
#define FIRM_CONTACT PUSH_FIRM(PUSH_ALL_BUT_PICKUPS)
#define CHAO_SHAPES 5
#define GRAVITY (-0.055F)

extern NJS_CNK_OBJECT chao_special_model_1, chao_special_model_2;

/* clang-format off */
static struct collision_shape chao_shapes[CHAO_SHAPES] = {
    {-111, SHAPE_CYLINDER, FIRM_CONTACT, 0, SENSOR_CONTACT,
     {0.0F, 2.0F, 0.0F}, 15.0F, 4.0F, 0.0F, 0, {0, 0, 0}},
    {(Sint8)CHAO_BODY_HIT, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS, 0, SOLID_CONTACT,
     {0.0F, 2.0F, 0.0F}, 1.5F, 2.5F, 0.7F, 0, {0, 0, 0}},
    {-110, SHAPE_CYLINDER, FIRM_CONTACT, 0, SENSOR_CONTACT,
     {0.0F, 2.0F, 0.0F}, 5.0F, 4.0F, 0.0F, 0, {0, 0, 0}},
    {-107, SHAPE_SPHERE, FIRM_CONTACT,
     SHAPE_ATTACK_LEVEL(2) | SHAPE_GUARD_LEVEL(3) | SHAPE_HURTS_ALL,
     SOLID_CONTACT | SHAPE_OFF, {0.0F, 2.0F, 3.7F}, 2.8F, 0.0F, 0.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_SPHERE, FIRM_CONTACT, 0, SOLID_CONTACT | SHAPE_OFF,
     {0.0F, 2.0F, 0.0F}, 1.4F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void chao_actor_step(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    if (work->frames <= 2) {
        chao_icon_place(chao);
    }
    /* @bug mismatched-call: the step calls garden_objects_running with no
     * argument, where it takes the object's task (0x0c53f060). the call works
     * only because garden_objects_running never reads its argument */
#ifdef NONMATCHING
    /* this side keeps the tag's result, as garden_objects_running reads no
     * argument. it calls the function by its own type, since a call through
     * another function type is undefined C */
    if (!garden_objects_running(chao)) {
        return;
    }
#else
    /* NOLINTNEXTLINE(clang-diagnostic-cast-function-type-mismatch) */
    if (!((Bool (*)(void))garden_objects_running)()) {
        return;
    }
#endif
    chao_behavior_step(chao);
    chao_motion_step(chao);
    chao_icon_step(chao);
    chao_face_step(chao);
    chao_step_body(chao);
    if (work->flags & CHAO_MOVES) {
        mover_step(chao);
    }
    chao_meet_floor(chao);
    work->frames++;
    if (work->flags & CHAO_COLLIDES) {
        collision_register(chao);
    } else {
        collision_reset(chao);
    }
    /* @hack spelling: a one-case switch on the body, where an if would do */
    switch (work->profile.parameters.body) {
    case CHAO_BODY_MINIMAL:
        motion_step(&work->minimal_motion);
        break;
    }
}

static void chao_actor_destroy(struct task *chao)
{
    chao_free_body(chao);
    chao_behavior_destroy(chao);
    garden_unregister_object(chao);
}

#pragma inline(push_body_pose)
static void push_body_pose(struct chao_work *work)
{
    njPushMatrixEx();
    njTranslateV(NULL, &work->base.pos);
    njRotateZ(NULL, work->base.ang.z);
    njRotateX(NULL, work->base.ang.x);
    njRotateY(NULL, work->base.ang.y);
}

void chao_actor_draw(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_parameters *parameters = &work->profile.parameters;
#ifdef NONMATCHING
    /* this side draws no Chao while Ninja's matrix stack has no room for the
     * draw's push */
    if (!chao_matrix_slot_available()) {
        return;
    }
#endif
    switch (parameters->body) {
    case CHAO_BODY_STANDARD:
    case CHAO_BODY_ALTERNATE:
        chao_draw_body(chao);
        break;
    case CHAO_BODY_MODEL_1:
        push_body_pose(work);
        njSetTexture(&chao_body_texlist);
        motion_draw_simple(&chao_special_model_1, &work->motion);
        njPopMatrixEx();
        break;
    case CHAO_BODY_MODEL_2:
        push_body_pose(work);
        njSetTexture(&chao_body_texlist);
        motion_draw_simple(&chao_special_model_2, &work->motion);
        njPopMatrixEx();
        break;
    case CHAO_BODY_MINIMAL:
        push_body_pose(work);
        njSetTexture(&garden_animal_texlist);
        motion_draw_simple(garden_minimal_models[parameters->minimal],
                           &work->minimal_motion);
        njPopMatrixEx();
        break;
    }
}

struct task *chao_create(const struct chao_gene *gene,
                         const struct chao_profile *profile,
                         const struct garden_race_chao *appearance,
                         NJS_POINT3 *position, Angle yaw,
                         struct chao_save *save)
{
    struct task *chao;
    struct task_work *base;
    struct chao_work *work;
    struct mover *mover;
    int area;

    chao = task_create(0, 2, chao_actor_step, "CHAO");
#ifdef NONMATCHING
    /* this side returns null where task_create fails. the matching side writes
     * through the null */
    if (chao == NULL) {
        return NULL;
    }
#endif
    chao->work = syCalloc(1, sizeof(*work));
#ifdef NONMATCHING
    /* this side destroys the task and returns null where the heap has no work.
     * the matching side writes through the null */
    if (chao->work == NULL) {
        task_destroy(chao);
        return NULL;
    }
#endif
    base = chao->work;
    if (position != NULL) {
        base->pos = *position;
    }
    base->ang.y = yaw;
#ifdef NONMATCHING
#else
    garden_register_object(GARDEN_OBJECT_CHAO, chao, 0);
#endif
    work = CHAO_WORK(chao);
    chao->destructor = chao_actor_destroy;
    chao->draw = chao_actor_draw;
#ifdef NONMATCHING
    /* this side destroys the Chao and returns null where collision_init
     * fails */
    if (!collision_init(chao, chao_shapes, CHAO_SHAPES, COLLISION_UNK_9)) {
        task_destroy(chao);
        return NULL;
    }
#else
    collision_init(chao, chao_shapes, CHAO_SHAPES, COLLISION_UNK_9);
#endif
    work->race_progress = 0.0F;
    mover = mover_alloc(chao);
#ifdef NONMATCHING
    /* this side destroys the Chao and returns null where mover_alloc fails. the
     * matching side writes through the null mover below */
    if (mover == NULL) {
        task_destroy(chao);
        return NULL;
    }
#endif
    mover_set_gravity(chao, GRAVITY);
#ifdef NONMATCHING
#else
    mover->floor_calls = base->tick;
#endif
    mover->probe_height = 3.0F;
    mover->unk_b4 = 1.5F;
    mover->bottom = -3.0F;
    mover->floor_bounce = chao_tuning.floor_bounce;
    mover->friction = chao_tuning.friction;
    if (player_character(0) != PLAYER_EGGMAN) {
        mover->hold_turn = 0x1400;
    } else {
        mover->hold_turn = -0x1000;
    }
    mover->flags |= MOVER_CAN_REST;
    if (gene != NULL) {
        work->gene = *gene;
    } else {
        chao_new_gene(&work->gene);
    }
    if (profile != NULL) {
        work->profile = *profile;
    } else {
        chao_hatch_traits(chao);
    }
    area = chao_current_area();
    if (area <= CHAO_AREA_DARK) {
        /* @hack spelling: the garden written through chao->work, where work
         * would do */
        CHAO_WORK(chao)->profile.parameters.garden = area;
    }
    if (appearance != NULL) {
        chao_unpack_racer(&work->profile.parameters, appearance);
    }
    chao_perception_init(chao);
    chao_behavior_init(chao);
    chao_motion_init(chao);
    /* @bug memory: where chao_build_body fails, the constructor goes on to
     * set up the face and parts of a body it hasn't got (0x0c53f482) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it destroys the Chao and returns
     * null where chao_build_body fails. it registers the Chao only once it is
     * built, and destroys it too where no slot is free. floor_calls then takes
     * the slot that the registration puts in work->tick */
    if (!chao_build_body(chao) ||
        !garden_register_object(GARDEN_OBJECT_CHAO, chao, 0)) {
        task_destroy(chao);
        return NULL;
    }
    mover->floor_calls = base->tick;
#else
    chao_build_body(chao);
#endif
    chao_icon_init(chao);
    chao_face_init(chao);
    chao_restore_animal_parts(chao);
    fall_set_kind(base, chao->motion.p, FALL_KIND_NORMAL);
    /* @hack spelling: each flag set or cleared in a statement of its own, and
     * through chao->work, where one mask through work would do */
    CHAO_WORK(chao)->flags |= CHAO_VISIBLE;
    CHAO_WORK(chao)->flags |= CHAO_COLLIDES;
    CHAO_WORK(chao)->flags |= CHAO_MOVES;
    CHAO_WORK(chao)->flags |= CHAO_ANIMATES;
    CHAO_WORK(chao)->flags |= CHAO_RUNS_BEHAVIOR;
    CHAO_WORK(chao)->flags |= CHAO_AGES;
    CHAO_WORK(chao)->flags |= CHAO_BODY_SPRINGS;
    CHAO_WORK(chao)->flags |= CHAO_COURSE_CONSTRAIN;
    CHAO_WORK(chao)->flags &= ~CHAO_RACE_STAGE_COLLISION;
    CHAO_WORK(chao)->flags &= ~CHAO_NO_INTENTIONS;
    CHAO_WORK(chao)->flags |= CHAO_UNK_40000;
    if (save != NULL) {
        work->save = save;
        garden_pack_chao(chao, work->save);
    }
    if (profile != NULL && profile->parameters.body == CHAO_BODY_MINIMAL) {
        garden_minimal_motion_table(work->minimal_clips,
                                    profile->parameters.minimal);
        motion_init(&work->minimal_motion, work->minimal_clips);
        motion_link(&work->minimal_motion, MINIMAL_WALK);
    }
    return chao;
}

/* chao_create without appearance or save, which no code calls */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
struct task *chao_create_guest(const struct chao_gene *gene,
                               const struct chao_profile *profile,
                               NJS_POINT3 *position, Angle yaw)
{
    return chao_create(gene, profile, NULL, position, yaw, NULL);
}

/* chao_create from appearance alone, which no code calls */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
struct task *chao_create_race_guest(const struct garden_race_chao *appearance,
                                    NJS_POINT3 *position, Angle yaw)
{
    return chao_create(NULL, NULL, appearance, position, yaw, NULL);
}

void chao_remove_resident(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    garden_clear_chao(work->save);
    work->save = NULL;
    task_kill(chao);
}
