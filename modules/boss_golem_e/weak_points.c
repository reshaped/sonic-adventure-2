/* the weak points of weak_points.h, which ride the golem's GOLEM_SENSOR_LOW
 * node, and their targets. the source also holds the golem's rubble, which
 * hits, opening weak points and the golem's limbs throw.
 * boss_golem/head_parts.c holds BOSS_GOLEM.PRS's rubble. golem_e_create
 * creates the weak points, and no object list names any of this */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "boss_golem_e/attachment_art.h"
#include "boss_golem_e/golem.h"
#include "boss_golem_e/weak_points.h"
#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/lines.h"
#include "enemy/enemy.h"
#include "shared/black_smoke.h"
#include "shared/blast.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/flash.h"
#include "shared/golem.h"
#include "shared/golem_util.h"
#include "shared/object_builds.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/matrix.h"
#include "system/task.h"
#include "system/util.h"

struct task *golem_e_weak_points[GOLEM_E_WEAK_POINTS];

#define WEAK_POINT_LEVEL 2
#define WEAK_POINT_SHAPES 1
#define TARGET_SHAPES 1
/* the id of a target's shape, which the _with_id calls match */
#define TARGET_SHAPE_ID 10
/* a weak point's next_mode while no mode is requested */
#define NO_MODE (-1)
/* a weak point's modes, in the order it takes them */
enum weak_point_mode { MODE_CLOSED, MODE_OPEN, MODE_BROKEN, WEAK_POINT_MODES };
/* the weak point of index i takes the threshold (i + 1) * THRESHOLD_STEP */
#define THRESHOLD_STEP 20

/* an open weak point's glow cycles each GLOW_STEPS of game_state.steps. it
 * rises from 0 to 1 over the cycle's first GLOW_RISE_STEPS. it falls back to 0
 * over the rest */
#define GLOW_STEPS 60
#define GLOW_RISE_STEPS 5
#define GLOW_LAST (GLOW_STEPS - 1)
/* an open weak point draws its light A and its light B in turn, each for
 * LIGHT_STEPS. each weak point's turns run LIGHT_LEAD steps ahead of those of
 * the one before */
#define LIGHT_STEPS 30
#define LIGHT_LEAD 10
/* each broken weak point's pulse runs a quarter turn ahead of the one
 * before's */
#define PULSE_LEAD QUARTER_TURN

/* a weak point's sounds, and the keys they play for. a key is an address a
 * few bytes into the weak point's task. sound_set_volume finds a sound by its
 * key alone */
#define OPEN_SOUND 12
#define OPEN_SOUND_KEY(task) ((char *)(task) + 7)
#define BREAK_SOUND 13
#define BREAK_SOUND_KEY(task) ((char *)(task) + 6)
#define BREAK_POWER 1.501F

/* each model's index in part_builds */
enum part_build {
    BUILD_MECHANISM,
    BUILD_PIECE,
    BUILD_WEAK_POINT_A,
    BUILD_WEAK_POINT_B,
    BUILD_WEAK_POINT_C,
    BUILD_RUBBLE,
    PART_BUILDS
};

#define RUBBLE_PIECES 7
/* the bounds of the random factors. the spins, the distances and the puffs'
 * speeds take one from SPREAD_LOW to SPREAD_LOW + SPREAD. a piece's speed and
 * a puff's scale take one from WIDE_LOW to WIDE_LOW + WIDE_SPREAD. each part of
 * a smoke's speed takes one from that range too. a piece's size takes one from
 * SIZE_LOW to SIZE_LOW + SIZE_SPREAD. each spin turns either way at even
 * odds */
#define SPREAD_LOW 0.9F
#define SPREAD 0.2F
#define WIDE_LOW 0.8F
#define WIDE_SPREAD 0.4F
#define SIZE_LOW 0.75F
#define SIZE_SPREAD 0.5F
#define EVEN_ODDS 0.5F
/* the share of its size a piece loses over its life */
#define LIFE_SHRINK 0.5F
#define TILT_SWING 4096.0F
#define TARGET_FLASH_SCALE 25.0F

/* a target's work holds its offset from its weak point and its index */
#define TARGET_OFFSET(work) ((work)->scl)
#define TARGET_INDEX(work) ((work)->tick)

/* a piece of rubble, among RUBBLE_PIECES in its task's extra */
struct rubble {
    NJS_CNK_MODEL *model;
    NJS_TEXLIST *texlist;
    Bool live;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR scl;
    NJS_VECTOR spd;
    struct angles spin; /* what ang turns by in a step */
    Sint32 steps;
    Sint32 life; /* the steps the piece flies */
    Bool smokes; /* TRUE where the piece leaves puffs as it flies */
};
/* @hack type: extra read as a void * through its address, where
 * (task)->extra.p would do */
#define RUBBLE(task) ((struct rubble *)*(void **)&(task)->extra)
/* the rubble's task counts its live pieces in its unk_3c */
#define LIVE_PIECES(task) ((task)->unk_3c.l)

static void golem_weak_destroy(struct task *task);
static void golem_weak_draw(struct task *task);
static void golem_weak_step(struct task *task);
static void golem_weak_set_mode(struct task *task, int mode);
static void golem_weak_run(struct task *task);
static void golem_weak_closed_start(struct task *task);
static void golem_weak_closed_step(struct task *task);
static void golem_weak_open_start(struct task *task);
static void golem_weak_open_step(struct task *task);
static void golem_weak_broken_start(struct task *task);
static void golem_weak_broken_step(struct task *task);
static void golem_weak_draw_model(struct task *task, NJS_CNK_MODEL *model,
                                  NJS_POINT3 *pos, struct angles *ang,
                                  Float glow);
static void rubble_from_weak_point(struct task *weak_point);
static void rubble_destroy(struct task *task);
static void rubble_step(struct task *task);
static void rubble_draw(struct task *task);
static void weak_target_init(struct task *task);
static void weak_target_destroy(struct task *task);
static void weak_target_step(struct task *task);

/* clang-format off */
static const struct collision_shape weak_point_shapes[WEAK_POINT_SHAPES] = {
    {0, SHAPE_SPHERE, GOLEM_E_PUSH_RESISTS,
     SHAPE_GUARD_LEVEL(GOLEM_E_GUARD_LEVEL), SHAPE_UNK_2000,
     {0.0F, 0.0F, 25.0F}, 35.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void (*starts_by_mode[WEAK_POINT_MODES])(struct task *task) = {
    golem_weak_closed_start, golem_weak_open_start, golem_weak_broken_start};
static void (*steps_by_mode[WEAK_POINT_MODES])(struct task *task) = {
    golem_weak_closed_step, golem_weak_open_step, golem_weak_broken_step};
/* each weak point's place on its sensor's node, and its turn there in
 * degrees */
#ifdef NONMATCHING
/* this side keeps the datum in .data, where the image holds it. nothing writes
 * it, so GCC would fold it into the code or place it among the constants */
__attribute__((used, section(".data")))
#endif
static NJS_POINT3 weak_point_pos[GOLEM_E_WEAK_POINTS] = {
#include "boss_golem_e/weak_points/weak_point_pos.inc"
};
#ifdef NONMATCHING
/* this side keeps the datum, in .data, as for weak_point_pos */
__attribute__((used, section(".data")))
#endif
static NJS_POINT3 weak_point_angles[GOLEM_E_WEAK_POINTS] = {
#include "boss_golem_e/weak_points/weak_point_angles.inc"
};
/* a piece of rubble's speed loses rubble_gravity a step to gravity. it then
 * keeps rubble_drag of its speed and rubble_spin_keep of its spin */
static Float rubble_gravity = 0.08F;
static Float rubble_drag = 0.9775F;
static Float rubble_spin_keep = 0.95F;
/* each target's offset from its weak point, in the world's axes */
#ifdef NONMATCHING
/* this side keeps the datum, in .data, as for weak_point_pos */
__attribute__((used, section(".data")))
#endif
static NJS_VECTOR target_offsets[GOLEM_E_WEAK_POINT_TARGETS] = {
#include "boss_golem_e/weak_points/target_offsets.inc"
};
static Sint32 open_volume = 25;
static Sint32 break_volume = 75;
static Sint32 target_volume = 50;
static struct model_build part_builds[PART_BUILDS] = {
    {MODEL_BUILD_ALONE, &golem_e_mechanism_texlist, &golem_e_mechanism_object,
     NULL},
    {MODEL_BUILD_ALONE, &golem_e_piece_texlist, &golem_e_piece_object, NULL},
    {MODEL_BUILD_ALONE, &golem_e_weak_point_a_texlist,
     &golem_e_weak_point_a_object, NULL},
    {MODEL_BUILD_ALONE, &golem_e_weak_point_b_texlist,
     &golem_e_weak_point_b_object, NULL},
    {MODEL_BUILD_ALONE, &golem_e_weak_point_c_texlist,
     &golem_e_weak_point_c_object, NULL},
    {MODEL_BUILD_ALONE, &golem_e_rubble_texlist, &golem_e_rubble_object, NULL},
};
/* a broken weak point gives off black smoke with the chance smoke_chance a
 * step. a struck target throws dust once. each puff of smoke or dust is
 * smoke_size plus up to smoke_size_spread in size. it rises at smoke_spd.y. it
 * heads out at smoke_spd.z, within half of smoke_spread either way of the
 * golem's heading plus the weak point's ang.y. each part of its speed takes a
 * random factor */
static Float smoke_chance = 0.07F;
static Float smoke_size = 12.0F;
static Float smoke_size_spread = 3.0F;
static Angle smoke_spread = 0x2000;
static NJS_VECTOR smoke_spd = {0.0F, 0.6F, 0.9F};
/* the color a hit on a weak point gives it. it fades to white as the weak
 * point's flash_steps run out */
static Float flash_red = 1.0F;
static Float flash_green = 0.2F;
static Float flash_blue = 0.2F;
/* a broken weak point draws its mechanism in a constant color. the red swings
 * between pulse_red_low and pulse_red_high on a cosine that turns pulse_speed
 * a step */
static Float pulse_red_high = 0.75F;
static Float pulse_red_low = 0.25F;
static Float pulse_green = 0.3F;
static Float pulse_blue = 0.25F;
static Angle pulse_speed = 0x400;
/* a piece of a hit's rubble starts hit_dist from the hit, and flies at
 * hit_speed. it spins hit_spin a step about each axis, and flies hit_life steps
 * at the size hit_scale. the distance, the speed, the spins and the size each
 * take a random factor. every other piece starts a puff hit_puff_dist from its
 * own start, at the size hit_puff_scale */
static Float hit_dist = 0.25F;
static Float hit_speed = 0.75F;
static Angle hit_spin = 0x400;
static Sint32 hit_life = 90;
static Float hit_scale = 1.75F;
static Float dat_8c6093ec = 1.0F;
static Float dat_8c6093f0 = 10.0F;
static Float hit_puff_dist = 20.0F;
static Float hit_puff_scale = 10.0F;
static NJS_CNK_MODEL *hit_piece_model = golem_e_rubble_model;
/* the open_ values serve an opening weak point's rubble as the hit_ values
 * serve a hit's. a piece also flies with the weak point's move since its last
 * place. every other piece starts a puff open_puff_dist from a point of its
 * own. the point lies open_puff_dist_share of the piece's offset out from the
 * weak point. the puff flies at open_puff_speed_share of the piece's speed */
static Float open_dist = 10.0F;
static Float open_speed = 1.0F;
static Angle open_spin = 0x400;
static Sint32 open_life = 150;
static Float open_scale = 1.5F;
static Float open_puff_dist_share = 2.5F;
static Float open_puff_speed_share = 1.0F;
static Float open_puff_dist = 20.0F;
static Float open_puff_scale = 15.0F;
/* the throw_ values serve a throw's rubble as the open_ values serve an
 * opening weak point's. a throw's piece flies with throw_share of the
 * sensor's move. every piece of a throw starts a puff, where only every other
 * piece of an opening's does */
static Float throw_dist = 10.0F;
static Float throw_speed = 0.1F;
static Angle throw_spin = 0x400;
static Sint32 throw_life = 150;
static Float throw_scale = 2.0F;
static Float throw_puff_dist_share = 2.5F;
static Float throw_puff_speed_share = 1.0F;
static Float throw_puff_dist = 20.0F;
static Float throw_puff_scale = 15.0F;
/* a sensor that moves throw_move or less since its last place throws
 * nothing. one that moves more throws with the chance throw_chance */
static Float throw_move = 2.0F;
static Float throw_share = 0.2F;
static Float throw_chance = 0.0333F;
/* a throw has from 1 to throw_pieces - 1 pieces. a piece smokes with the
 * chance smoke_piece_chance */
static Sint32 throw_pieces = 2;
static Float smoke_piece_chance = 1.0F;
/* each step a smoking piece starts a puff with the chance puff_chance */
static Float puff_chance = 0.1F;
static Float flight_puff_scale = 10.0F;
/* no throw starts while golem_e's rubble_count is above rubble_limit */
static Sint32 rubble_limit = 32;
/* clang-format off */
static struct collision_shape target_shape = {
    TARGET_SHAPE_ID, SHAPE_SPHERE, GOLEM_PUSH_SOLID, 0, SHAPE_UNK_800000,
    {0.0F, 0.0F, 0.0F}, 2.0F, 0.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */
/* a target collides only while golem_player_heading lies within target_arc of
 * the golem's heading plus its weak point's ang.y */
static Angle target_arc = 0x2c00;
/* a struck target's flash lies out from the target. the offset is the
 * target's own along x and z, plus target_flash_z along z. it turns by the
 * golem's heading plus the weak point's ang.y */
static Float target_flash_z = 15.0F;

Uint8 *golem_e_parts_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, part_builds, PART_BUILDS) == PART_BUILDS) {
        golem_e_parts_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

#ifdef NONMATCHING
/* this side returns whether it made every weak point, so that golem_e_create
 * can destroy an unfinished golem */
Bool golem_e_weak_points_create(struct task *golem_task)
#else
void golem_e_weak_points_create(struct task *golem_task)
#endif
{
    struct task *tasks[GOLEM_E_WEAK_POINTS];
    int i;

    for (i = 0; i < GOLEM_E_WEAK_POINTS; i++) {
        tasks[i] = task_create(TASK_WORK, WEAK_POINT_LEVEL, golem_weak_step,
                               "GolemWeakExec");
#ifdef NONMATCHING
        /* this side returns FALSE where task_create fails. the matching side
         * writes through the null */
        if (tasks[i] == NULL) {
            return FALSE;
        }
#endif
        tasks[i]->destructor = golem_weak_destroy;
        tasks[i]->draw = golem_weak_draw;
        tasks[i]->extra.p = syMalloc(sizeof(struct golem_e_weak_point));
#ifdef NONMATCHING
        /* this side returns FALSE where the heap has no record. it destroys the
         * task without golem_weak_destroy, which reads the record */
        if (tasks[i]->extra.p == NULL) {
            tasks[i]->destructor = NULL;
            task_destroy(tasks[i]);
            return FALSE;
        }
#endif
        GOLEM_E_WEAK_POINT(tasks[i])->sensor = GOLEM_SENSOR_LOW;
        GOLEM_E_WEAK_POINT(tasks[i])->index = i;
        golem_weak_set_mode(tasks[i], MODE_CLOSED);
        golem_e_weak_points[i] = tasks[i];
        GOLEM_E_WEAK_POINT(tasks[i])->pos = weak_point_pos[i];
        GOLEM_E_WEAK_POINT(tasks[i])->ang.x =
            weak_point_angles[i].x * ANGLE_PER_DEGREE;
        GOLEM_E_WEAK_POINT(tasks[i])->ang.y =
            weak_point_angles[i].y * ANGLE_PER_DEGREE;
        GOLEM_E_WEAK_POINT(tasks[i])->ang.z =
            weak_point_angles[i].z * ANGLE_PER_DEGREE;
        GOLEM_E_WEAK_POINT(tasks[i])->threshold = (i + 1) * THRESHOLD_STEP;
        GOLEM_E_WEAK_POINT(tasks[i])->flash_steps = 0;
        collision_init(tasks[i], weak_point_shapes, WEAK_POINT_SHAPES,
                       COLLISION_OBJECT);
#ifdef NONMATCHING
        /* this side returns FALSE where collision_init gives the weak point no
         * record */
        if (tasks[i]->work->collision == NULL) {
            return FALSE;
        }
#endif
    }
    GOLEM_E_WEAK_POINT(tasks[0])->model = &golem_e_weak_point_a_model;
    GOLEM_E_WEAK_POINT(tasks[0])->texlist = &golem_e_weak_point_a_texlist;
    GOLEM_E_WEAK_POINT(tasks[1])->model = &golem_e_weak_point_b_model;
    GOLEM_E_WEAK_POINT(tasks[1])->texlist = &golem_e_weak_point_b_texlist;
    GOLEM_E_WEAK_POINT(tasks[2])->model = &golem_e_weak_point_c_model;
    GOLEM_E_WEAK_POINT(tasks[2])->texlist = &golem_e_weak_point_c_texlist;
#ifdef NONMATCHING
    /* this side returns that it made every weak point */
    return TRUE;
#endif
}

static void golem_weak_destroy(struct task *task)
{
#ifdef NONMATCHING
    /* this side clears the slot only where it holds this weak point.
     * golem_destroy empties the slot at the kill, before this destroy runs. a
     * later weak point may hold it by then */
    if (golem_e_weak_points[GOLEM_E_WEAK_POINT(task)->index] == task) {
        golem_e_weak_points[GOLEM_E_WEAK_POINT(task)->index] = NULL;
    }
#else
    golem_e_weak_points[GOLEM_E_WEAK_POINT(task)->index] = NULL;
#endif
    syFree(task->extra.p);
    task->extra.p = NULL;
}

static void golem_weak_draw(struct task *task)
{
    int phase = game_state.steps % GLOW_STEPS;
    Float glow;

    if (phase < GLOW_RISE_STEPS) {
        glow = phase * (1.0F / (GLOW_RISE_STEPS - 1));
    } else {
        glow = (GLOW_LAST - phase) * (1.0F / (GLOW_LAST - GLOW_RISE_STEPS));
    }
    njPushMatrixEx();
    matrix_multiply_before(
        &golem_e_sensor_matrices[GOLEM_E_WEAK_POINT(task)->sensor]);
    njSetTexture(GOLEM_E_WEAK_POINT(task)->texlist);
    golem_weak_draw_model(task, GOLEM_E_WEAK_POINT(task)->model,
                          &GOLEM_E_WEAK_POINT(task)->pos,
                          &GOLEM_E_WEAK_POINT(task)->ang, glow);
    njPopMatrixEx();
}

static void golem_weak_step(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    matrix_set(&golem_e_sensor_matrices[GOLEM_E_WEAK_POINT(task)->sensor]);
    njTranslateEx(&GOLEM_E_WEAK_POINT(task)->pos);
    njRotateXYZ(NULL, GOLEM_E_WEAK_POINT(task)->ang.x,
                GOLEM_E_WEAK_POINT(task)->ang.y,
                GOLEM_E_WEAK_POINT(task)->ang.z);
    njGetTranslation(NULL, &work->pos);
    current_matrix_angles_yxz(&work->ang.x, &work->ang.y, &work->ang.z);
    njPopMatrixEx();
    GOLEM_E_WEAK_POINT(task)->active = FALSE;
    golem_weak_run(task);
}

static void golem_weak_set_mode(struct task *task, int mode)
{
    GOLEM_E_WEAK_POINT(task)->mode = mode;
    GOLEM_E_WEAK_POINT(task)->next_mode = NO_MODE;
    GOLEM_E_WEAK_POINT(task)->mode_steps = 0;
    starts_by_mode[GOLEM_E_WEAK_POINT(task)->mode](task);
}

static void golem_weak_run(struct task *task)
{
    if (GOLEM_E_WEAK_POINT(task)->next_mode != NO_MODE) {
        GOLEM_E_WEAK_POINT(task)->mode = GOLEM_E_WEAK_POINT(task)->next_mode;
        GOLEM_E_WEAK_POINT(task)->next_mode = NO_MODE;
        GOLEM_E_WEAK_POINT(task)->mode_steps = 0;
        starts_by_mode[GOLEM_E_WEAK_POINT(task)->mode](task);
    }
    steps_by_mode[GOLEM_E_WEAK_POINT(task)->mode](task);
    GOLEM_E_WEAK_POINT(task)->mode_steps++;
}

/* NOLINTBEGIN(misc-use-internal-linkage) */
/* request mode for task's next step. only this source calls it, and SHC
 * expands each call. it is external so that SHC still writes its body, which
 * the image holds */
void golem_weak_request_mode(struct task *task, int mode)
{
    GOLEM_E_WEAK_POINT(task)->next_mode = mode;
}
/* NOLINTEND(misc-use-internal-linkage) */

static void golem_weak_closed_start(struct task *task)
{
}

static void golem_weak_closed_step(struct task *task)
{
    if (golem_e->hits >= GOLEM_E_WEAK_POINT(task)->threshold) {
        golem_weak_request_mode(task, MODE_OPEN);
    }
    collision_register(task);
    GOLEM_E_WEAK_POINT(task)->active = TRUE;
}

static void golem_weak_open_start(struct task *task)
{
    int i;
    struct task *target;

    rubble_from_weak_point(task);
    sound_play(OPEN_SOUND, OPEN_SOUND_KEY(task), 0, 0);
    sound_set_volume(OPEN_SOUND_KEY(task), SOUND_ANY_ID, open_volume);
    for (i = 0; i < GOLEM_E_WEAK_POINT_TARGETS; i++) {
        target = task_create_child(TASK_WORK, weak_target_init, task);
        TARGET_OFFSET(target->work) = target_offsets[i];
        TARGET_INDEX(target->work) = i;
        GOLEM_E_WEAK_POINT(task)->struck[i] = FALSE;
    }
}

static void golem_weak_open_step(struct task *task)
{
    Bool *struck;
    Bool unstruck = FALSE;

    for (struck = GOLEM_E_WEAK_POINT(task)->struck;
         struck < &GOLEM_E_WEAK_POINT(task)->struck[GOLEM_E_WEAK_POINT_TARGETS];
         struck++) {
        if (*struck == FALSE) {
            unstruck = TRUE;
        }
    }
    if (!unstruck) {
        golem_weak_request_mode(task, MODE_BROKEN);
    }
}

static void golem_weak_broken_start(struct task *task)
{
    struct task_work *work = task->work;

    blast_create(BLAST_HUGE, &work->pos);
    sound_play(BREAK_SOUND, BREAK_SOUND_KEY(task), 0, 0);
    sound_set_volume(BREAK_SOUND_KEY(task), SOUND_ANY_ID, break_volume);
    golem_e->power -= BREAK_POWER;
}

static void golem_weak_broken_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: twelve bytes of the frame that no code touches */
    unsigned char unused[12];
    NJS_VECTOR spd;
    Float x;
    Float z;
    Angle ang;

    if (rand() * RAND_SCALE < smoke_chance) {
        spd = smoke_spd;
        /* NOLINTBEGIN(bugprone-integer-division) */
        ang = golem_e->heading + GOLEM_E_WEAK_POINT(task)->ang.y +
              ((Float)smoke_spread * rand() * RAND_SCALE) - (smoke_spread / 2);
        /* NOLINTEND(bugprone-integer-division) */
        golem_polar_xz(ang, spd.z, &x, &z);
        spd.x = x * RANDOM_FROM(WIDE_LOW, WIDE_SPREAD);
        spd.z = z * RANDOM_FROM(WIDE_LOW, WIDE_SPREAD);
        spd.y *= RANDOM_FROM(WIDE_LOW, WIDE_SPREAD);
        black_smoke_rising_create(
            &work->pos, &spd,
            smoke_size + (rand() * smoke_size_spread * RAND_SCALE));
    }
}

static void golem_weak_draw_model(struct task *task, NJS_CNK_MODEL *model,
                                  NJS_POINT3 *pos, struct angles *ang,
                                  Float glow)
{
    struct model_build *builds;
    Float share;
    Float red;
    Float green;
    Float blue;

    njPushMatrixEx();
    /* @hack redundant: a block no path reaches, which saves the draw's
     * attributes and restores its state */
    if (0) {
        draw_attr_save();
        draw_control_restore();
    }
    /* @hack redundant: a block no path reaches, which draws model and reads
     * njDirectDrawObject, task and model */
    if (0) {
        njCnkSimpleDrawModel(model);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        /* NOLINTBEGIN(clang-diagnostic-pointer-bool-conversion) */
        /* NOLINTBEGIN(misc-redundant-expression) */
        if (njDirectDrawObject || njDirectDrawObject || njDirectDrawObject ||
            njDirectDrawObject || njDirectDrawObject || task || task || model ||
            model) {
        }
        /* NOLINTEND(misc-redundant-expression) */
        /* NOLINTEND(clang-diagnostic-pointer-bool-conversion) */
    }
    /* @hack redundant: builds, which no code reads */
    builds = part_builds;
    if (GOLEM_E_WEAK_POINT(task)->mode == MODE_CLOSED) {
        if (golem_e->flashing || GOLEM_E_WEAK_POINT(task)->flash_steps > 0) {
            draw_control_save();
            draw_attr_save();
            draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
            draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
            if (GOLEM_E_WEAK_POINT(task)->flash_steps > 0) {
                share = GOLEM_E_WEAK_POINT(task)->flash_steps *
                        (1.0F / GOLEM_E_HIT_FLASH_STEPS);
                red = 1.0F - ((1.0F - flash_red) * share);
                green = 1.0F - ((1.0F - flash_green) * share);
                blue = 1.0F - ((1.0F - flash_blue) * share);
                constant_material_set(1.0F, red, green, blue);
            } else {
                share = golem_e->flash_steps * (1.0F / GOLEM_E_HIT_FLASH_STEPS);
                red = 1.0F - ((1.0F - golem_e_flash_red) * share);
                green = 1.0F - ((1.0F - golem_e_flash_green) * share);
                blue = 1.0F - ((1.0F - golem_e_flash_blue) * share);
                constant_material_set(1.0F, red, green, blue);
            }
            njCnkSimpleDrawModel(model);
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            draw_attr_restore();
            draw_control_restore();
        } else {
            njCnkSimpleDrawModel(model);
        }
    }
    njTranslateEx(pos);
    njRotateXYZ(NULL, ang->x, ang->y, ang->z);
    if (GOLEM_E_WEAK_POINT(task)->mode == MODE_OPEN) {
        njSetTexture(&golem_e_mechanism_texlist);
        if (golem_e_parts_built) {
            njDirectDrawObject(part_builds[BUILD_MECHANISM].built);
        } else {
            njCnkEasyDrawModel(&golem_e_mechanism_model);
        }
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE);
        njSetCnkBlendMode(NJD_FBS_ONE | NJD_FBD_ONE);
        njSetConstantAttr(0xff, NJD_FST_UA | NJD_FST_IS | NJD_FST_IL);
        constant_material_set(glow, 1.0F, 1.0F, 1.0F);
        /* @hack type: game_state's steps read as game_state_steps, where
         * game_state.steps would do */
        if (((game_state_steps +
              (GOLEM_E_WEAK_POINT(task)->index * LIGHT_LEAD)) /
             LIGHT_STEPS) &
            1) {
            njCnkSimpleDrawModel(&golem_e_light_a_model);
        } else {
            njCnkSimpleDrawModel(&golem_e_light_b_model);
        }
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    } else if (GOLEM_E_WEAK_POINT(task)->mode == MODE_BROKEN) {
        /* @hack type: the steps read as an int through game_state_steps, where
         * game_state.steps would do */
        share = (njCos((pulse_speed * (int)game_state_steps) +
                       (GOLEM_E_WEAK_POINT(task)->index * PULSE_LEAD)) +
                 1.0F) *
                0.5F;
        red = pulse_red_low + ((pulse_red_high - pulse_red_low) * share);
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(DRAW_CONTROL_CONSTANT);
        njSetConstantAttr(~NJD_FST_IL, 0);
        constant_material_set(1.0F, red, pulse_green, pulse_blue);
        njSetTexture(&golem_e_mechanism_texlist);
        njCnkSimpleDrawModel(&golem_e_mechanism_model);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    }
    /* @hack redundant: a block no path reaches, which reads builds */
    if (0) {
        if (builds->kind || builds) {
        }
    }
    njPopMatrixEx();
}

void golem_e_rubble_hit(struct task *task, NJS_POINT3 *pos)
{
    struct task *rubble;
    /* @hack spelling: at, a copy of pos, where pos would do */
    NJS_POINT3 *at = pos;
    struct rubble *piece;
    Float dist;
    Float scale;
    Float sine;
    struct angles ang;
    NJS_VECTOR offset;
    NJS_VECTOR spd;
    NJS_POINT3 start;
    int i;

    /* @hack spelling: the throw inside a test of rubble, where a return where
     * it is null would do */
    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if ((rubble = task_create_child(TASK_WORK, rubble_step, task)) != NULL) {
        if (!(rubble->extra.p =
                  syCalloc(RUBBLE_PIECES, sizeof(struct rubble)))) {
            task_kill(rubble);
            return;
        }
        rubble->draw = rubble_draw;
        rubble->destructor = rubble_destroy;
        for (i = 0; i < RUBBLE_PIECES; i++) {
            piece = RUBBLE(rubble) + i;
            /* @hack spelling: the sine set ahead of the angle, where its value
             * in place would do */
            /* @hack redundant: rand() times 1, here and in the tilt's angle
             * below, where (Float)rand() would do */
            sine = njSin((Angle)(-QUARTER_TURN + (rand() * 1.0F)));
            ang.y = (Angle)((sine * QUARTER_TURN) +
                            camera_views[camera_current_index]->ang.y) ^
                    HALF_TURN;
            ang.x =
                njSin((Angle)(-QUARTER_TURN + (rand() * 1.0F))) * EIGHTH_TURN;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, ang.y);
            njRotateX(NULL, ang.x);
            njPushMatrixEx();
            njTranslate(NULL, 0.0F, 0.0F,
                        -(hit_dist * RANDOM_FROM(SPREAD_LOW, SPREAD)));
            njGetTranslation(NULL, &offset);
            njPopMatrixEx();
            piece->pos.x = at->x + offset.x;
            piece->pos.y = at->y + offset.y;
            piece->pos.z = at->z + offset.z;
            njPushMatrixEx();
            njTranslate(NULL, 0.0F, 0.0F,
                        -(hit_speed * RANDOM_FROM(WIDE_LOW, WIDE_SPREAD)));
            njGetTranslation(NULL, &piece->spd);
            njPopMatrixEx();
            piece->spin.x = hit_spin * RANDOM_FROM(SPREAD_LOW, SPREAD);
            if (EVEN_ODDS > rand() * RAND_SCALE) {
                piece->spin.x = -piece->spin.x;
            }
            piece->spin.y = hit_spin * RANDOM_FROM(SPREAD_LOW, SPREAD);
            if (EVEN_ODDS > rand() * RAND_SCALE) {
                piece->spin.y = -piece->spin.y;
            }
            piece->spin.z = hit_spin * RANDOM_FROM(SPREAD_LOW, SPREAD);
            if (EVEN_ODDS > rand() * RAND_SCALE) {
                piece->spin.z = -piece->spin.z;
            }
            piece->scl.x = hit_scale * RANDOM_FROM(SIZE_LOW, SIZE_SPREAD);
            piece->scl.y = hit_scale * RANDOM_FROM(SIZE_LOW, SIZE_SPREAD);
            piece->scl.z = hit_scale * RANDOM_FROM(SIZE_LOW, SIZE_SPREAD);
            piece->steps = 0;
            piece->life = hit_life;
            piece->live = TRUE;
            piece->smokes = FALSE;
            piece->model = golem_e_rubble_model;
            piece->texlist = &golem_e_rubble_texlist;
            njPopMatrixEx();
            start = piece->pos;
            spd.x = offset.x * RANDOM_FROM(SPREAD_LOW, SPREAD);
            spd.y = offset.y * RANDOM_FROM(SPREAD_LOW, SPREAD);
            spd.z = offset.z * RANDOM_FROM(SPREAD_LOW, SPREAD);
            dist = hit_puff_dist * RANDOM_FROM(SPREAD_LOW, SPREAD);
            scale = hit_puff_scale * RANDOM_FROM(WIDE_LOW, WIDE_SPREAD);
            if (i % 2 == 0) {
                golem_dust_smoke_behind(&start, &spd, dist, scale);
                /* @hack redundant: a block no path reaches, which reads
                 * dist */
                if (0) {
                    if (dist) {
                    }
                }
            }
            /* @hack redundant: an else with no statement */
            else {
            }
        }
        /* @bug logic: adds none of the pieces to golem_e's rubble_count, though
         * each takes 1 off it as it ends. so each hit lowers the count by
         * RUBBLE_PIECES, and more thrown rubble flies than rubble_limit allows.
         * the store is at 0x8c503880 */
        LIVE_PIECES(rubble) = RUBBLE_PIECES;
        /* @hack redundant: a block no path reaches, which pops the matrix
         * twice and reads hit_spin and hit_scale */
        if (0) {
            njPopMatrixEx();
            njPopMatrixEx();
            if (hit_spin) {
            }
            if (hit_scale) {
            }
        }
    }
}

static void rubble_from_weak_point(struct task *weak_point)
{
    struct task *task;
    struct rubble *piece;
    Float dist;
    Float scale;
    Float sine;
    struct angles ang;
    NJS_VECTOR offset;
    NJS_VECTOR spd;
    NJS_VECTOR move;
    NJS_POINT3 base;
    NJS_POINT3 last;
    NJS_POINT3 place;
    NJS_POINT3 start;
    NJS_VECTOR out;
    int i;

    /* @hack spelling: the throw inside a test of task, where a return where
     * it is null would do */
    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if ((task = task_create_child(TASK_WORK, rubble_step, weak_point)) !=
        NULL) {
        if (!(task->extra.p = syCalloc(RUBBLE_PIECES, sizeof(struct rubble)))) {
            task_kill(task);
            return;
        }
        task->draw = rubble_draw;
        task->destructor = rubble_destroy;
        njPushMatrixEx();
        matrix_set(
            &golem_e_sensor_bases[GOLEM_E_WEAK_POINT(weak_point)->sensor]);
        njCalcPoint(NULL, &GOLEM_E_WEAK_POINT(weak_point)->pos, &last);
        njPopMatrixEx();
        njPushMatrixEx();
        matrix_set(
            &golem_e_sensor_matrices[GOLEM_E_WEAK_POINT(weak_point)->sensor]);
        njTranslateEx(&GOLEM_E_WEAK_POINT(weak_point)->pos);
        njGetTranslation(NULL, &place);
        move.x = place.x - last.x;
        move.y = place.y - last.y;
        move.z = place.z - last.z;
        njRotateXYZ(NULL, GOLEM_E_WEAK_POINT(weak_point)->ang.x,
                    GOLEM_E_WEAK_POINT(weak_point)->ang.y,
                    GOLEM_E_WEAK_POINT(weak_point)->ang.z);
        njGetTranslation(NULL, &base);
        /* @hack spelling: the count set to 0 ahead of the block below, where
         * the loop's start would do */
        i = 0;
        /* @hack redundant: a block no path reaches, which reads open_scale
         * and stores 0.75 */
        if (0) {
            if (open_scale) {
            }
            move.x = 0.75F;
        }
        for (; i < RUBBLE_PIECES; i++) {
            piece = RUBBLE(task) + i;
            /* @hack spelling: the sine set ahead of the angle, where its value
             * in place would do */
            /* @hack redundant: rand() times 1, here and in the tilt's angle
             * below, where (Float)rand() would do */
            sine = njSin((Angle)(-QUARTER_TURN + (rand() * 1.0F)));
            /* @hack spelling: the sine scaled in a statement of its own,
             * where the product in place would do */
            sine *= QUARTER_TURN;
            ang.y = (Angle)sine ^ HALF_TURN;
            ang.x =
                njSin((Angle)(-QUARTER_TURN + (rand() * 1.0F))) * EIGHTH_TURN;
            njPushMatrixEx();
            njRotateY(NULL, ang.y);
            njRotateX(NULL, ang.x);
            njPushMatrixEx();
            out.x = out.y = 0.0F;
            out.z = -(open_dist * RANDOM_FROM(SPREAD_LOW, SPREAD));
            njCalcVector(NULL, &out, &offset);
            njPopMatrixEx();
            piece->pos.x = base.x + offset.x;
            piece->pos.y = base.y + offset.y;
            piece->pos.z = base.z + offset.z;
            njPushMatrixEx();
            out.x = out.y = 0.0F;
            out.z = -(open_speed * RANDOM_FROM(WIDE_LOW, WIDE_SPREAD));
            njCalcVector(NULL, &out, &piece->spd);
            piece->spd.x += move.x;
            piece->spd.y += move.y;
            piece->spd.z += move.z;
            njPopMatrixEx();
            piece->spin.x = open_spin * RANDOM_FROM(SPREAD_LOW, SPREAD);
            if (EVEN_ODDS > rand() * RAND_SCALE) {
                piece->spin.x = -piece->spin.x;
            }
            piece->spin.y = open_spin * RANDOM_FROM(SPREAD_LOW, SPREAD);
            if (EVEN_ODDS > rand() * RAND_SCALE) {
                piece->spin.y = -piece->spin.y;
            }
            piece->spin.z = open_spin * RANDOM_FROM(SPREAD_LOW, SPREAD);
            if (EVEN_ODDS > rand() * RAND_SCALE) {
                piece->spin.z = -piece->spin.z;
            }
            piece->scl.x = open_scale * RANDOM_FROM(SIZE_LOW, SIZE_SPREAD);
            piece->scl.y = open_scale * RANDOM_FROM(SIZE_LOW, SIZE_SPREAD);
            piece->scl.z = open_scale * RANDOM_FROM(SIZE_LOW, SIZE_SPREAD);
            piece->steps = 0;
            piece->life = open_life;
            piece->live = TRUE;
            piece->smokes = FALSE;
            piece->model = golem_e_piece_model;
            piece->texlist = &golem_e_piece_texlist;
            njPopMatrixEx();
            start.x = base.x + (offset.x * open_puff_dist_share *
                                RANDOM_FROM(SPREAD_LOW, SPREAD));
            start.y = base.y + (offset.y * open_puff_dist_share *
                                RANDOM_FROM(SPREAD_LOW, SPREAD));
            start.z = base.z + (offset.z * open_puff_dist_share *
                                RANDOM_FROM(SPREAD_LOW, SPREAD));
            spd.x = piece->spd.x * open_puff_speed_share *
                    RANDOM_FROM(SPREAD_LOW, SPREAD);
            spd.y = piece->spd.y * open_puff_speed_share *
                    RANDOM_FROM(SPREAD_LOW, SPREAD);
            spd.z = piece->spd.z * open_puff_speed_share *
                    RANDOM_FROM(SPREAD_LOW, SPREAD);
            dist = open_puff_dist * RANDOM_FROM(SPREAD_LOW, SPREAD);
            scale = open_puff_scale * RANDOM_FROM(WIDE_LOW, WIDE_SPREAD);
            if (i % 2 == 0) {
                golem_dust_behind(&start, &spd, dist, scale);
            }
        }
        /* @bug logic: like golem_e_rubble_hit's store, adds none of the pieces
         * to golem_e's rubble_count. the store is at 0x8c504e76 */
        LIVE_PIECES(task) = RUBBLE_PIECES;
        njPopMatrixEx();
        /* @hack redundant: a block no path reaches, which stores three
         * constants */
        if (0) {
            move.x = 0.2F * RAND_SCALE;
            move.x = RAND_SCALE;
            move.x = RAND_SCALE;
        }
    }
}

void golem_e_rubble_throw(struct task *golem_task, int sensor)
{
    struct task *task;
    struct rubble *piece;
    struct rubble *cursor;
    Sint32 count;
    int i;
    Float dist;
    Float scale;
    Float sine;
    struct angles ang;
    NJS_VECTOR offset;
    NJS_VECTOR spd;
    NJS_VECTOR move;
    NJS_POINT3 base;
    NJS_POINT3 last;
    NJS_POINT3 place;
    NJS_POINT3 pos;
    NJS_VECTOR out;

    count = (Sint32)((rand() * (Float)(throw_pieces - 1) * RAND_SCALE) + 1.0F);
    njPushMatrixEx();
    matrix_set(&golem_e_sensor_bases[sensor]);
    njGetTranslation(NULL, &last);
    matrix_set(&golem_e_sensor_matrices[sensor]);
    njGetTranslation(NULL, &place);
    njPopMatrixEx();
    move.x = place.x - last.x;
    move.y = place.y - last.y;
    move.z = place.z - last.z;
    /* @hack spelling: the second test as the first's else, where a test of
     * its own would do */
    if (golem_e->rubble_count > rubble_limit) {
        return;
    } else if (njScalor2(&move) <= throw_move * throw_move) {
        return;
    }
    if (rand() * RAND_SCALE >= throw_chance) {
        return;
    }
    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if (!(task = task_create_child(TASK_WORK, rubble_step, golem_task))) {
        return;
    }
    if (!(task->extra.p = syCalloc(RUBBLE_PIECES, sizeof(struct rubble)))) {
        task_kill(task);
        return;
    }
    task->draw = rubble_draw;
    task->destructor = rubble_destroy;
    njPushMatrixEx();
    matrix_set(&golem_e_sensor_matrices[sensor]);
    njGetTranslation(NULL, &base);
    /* @hack spelling: the count stepped as i = i + 1, where i++ would do */
    /* @hack spelling: piece, a copy of cursor, where cursor would do */
    for (i = 0, cursor = task->extra.p; i < count; i = i + 1, cursor++) {
        piece = cursor;
        /* @hack spelling: the sine set ahead of the angle, where its value in
         * place would do */
        /* @hack redundant: rand() times 1, where (Float)rand() would do */
        sine = njSin((Angle)(-QUARTER_TURN + (rand() * 1.0F)));
        ang.y = (Angle)((sine * QUARTER_TURN) +
                        camera_views[camera_current_index]->ang.y) ^
                HALF_TURN;
        /* @bug arithmetic: every piece leaves at the same tilt. the tilt's
         * sine takes rand() * RAND_SCALE as an Angle, which truncates to 0 at
         * 0x8c5050a2. the sine of 0 is 0 */
        ang.x = (Angle)((njSin((Angle)(rand() * RAND_SCALE)) * TILT_SWING) -
                        QUARTER_TURN);
        njPushMatrixEx();
        njRotateY(NULL, ang.y);
        njRotateX(NULL, ang.x);
        njPushMatrixEx();
        out.x = out.y = 0.0F;
        out.z = -(throw_dist * RANDOM_FROM(SPREAD_LOW, SPREAD));
        njCalcVector(NULL, &out, &offset);
        njPopMatrixEx();
        piece->pos.x = base.x + offset.x;
        piece->pos.y = base.y + offset.y;
        piece->pos.z = base.z + offset.z;
        njPushMatrixEx();
        out.x = out.y = 0.0F;
        out.z = -(throw_speed * RANDOM_FROM(WIDE_LOW, WIDE_SPREAD));
        njCalcVector(NULL, &out, &piece->spd);
        piece->spd.x += move.x * throw_share;
        piece->spd.y += move.y * throw_share;
        piece->spd.z += move.z * throw_share;
        njPopMatrixEx();
        piece->spin.x = throw_spin * RANDOM_FROM(SPREAD_LOW, SPREAD);
        if (rand() * RAND_SCALE < EVEN_ODDS) {
            piece->spin.x = -piece->spin.x;
        }
        piece->spin.y = throw_spin * RANDOM_FROM(SPREAD_LOW, SPREAD);
        if (rand() * RAND_SCALE < EVEN_ODDS) {
            piece->spin.y = -piece->spin.y;
        }
        piece->spin.z = throw_spin * RANDOM_FROM(SPREAD_LOW, SPREAD);
        if (rand() * RAND_SCALE < EVEN_ODDS) {
            piece->spin.z = -piece->spin.z;
        }
        piece->scl.x = throw_scale * RANDOM_FROM(SIZE_LOW, SIZE_SPREAD);
        piece->scl.y = throw_scale * RANDOM_FROM(SIZE_LOW, SIZE_SPREAD);
        piece->scl.z = throw_scale * RANDOM_FROM(SIZE_LOW, SIZE_SPREAD);
        piece->steps = 0;
        piece->life = throw_life;
        piece->live = TRUE;
        piece->smokes = rand() * RAND_SCALE <= smoke_piece_chance;
        piece->model = golem_e_rubble_model;
        piece->texlist = &golem_e_rubble_texlist;
        njPopMatrixEx();
        pos.x = base.x + (offset.x * throw_puff_dist_share *
                          RANDOM_FROM(SPREAD_LOW, SPREAD));
        pos.y = base.y + (offset.y * throw_puff_dist_share *
                          RANDOM_FROM(SPREAD_LOW, SPREAD));
        pos.z = base.z + (offset.z * throw_puff_dist_share *
                          RANDOM_FROM(SPREAD_LOW, SPREAD));
        spd.x = piece->spd.x * throw_puff_speed_share *
                RANDOM_FROM(SPREAD_LOW, SPREAD);
        spd.y = piece->spd.y * throw_puff_speed_share *
                RANDOM_FROM(SPREAD_LOW, SPREAD);
        spd.z = piece->spd.z * throw_puff_speed_share *
                RANDOM_FROM(SPREAD_LOW, SPREAD);
        dist = throw_puff_dist * RANDOM_FROM(SPREAD_LOW, SPREAD);
        scale = throw_puff_scale * RANDOM_FROM(WIDE_LOW, WIDE_SPREAD);
        golem_dust_smoke_behind(&pos, &spd, dist, scale);
    }
    LIVE_PIECES(task) = count;
    /* @hack spelling: the count read back from the task, where count would
     * do */
    golem_e->rubble_count += LIVE_PIECES(task);
    njPopMatrixEx();
    /* @hack redundant: a block no path reaches, which pushes and pops the
     * matrix, stores 0.75 and reads task */
    if (0) {
        njPushMatrixEx();
        njPushMatrixEx();
        njPopMatrixEx();
        throw_puff_dist_share = 0.75F;
        if (task) {
        }
    }
}

static void rubble_destroy(struct task *task)
{
    syFree(task->extra.p);
    task->extra.p = NULL;
}

static void rubble_step(struct task *task)
{
    struct rubble *piece;
    Sint32 i;

    for (i = 0; i < RUBBLE_PIECES; i++) {
        piece = &RUBBLE(task)[i];
        if (piece->live) {
            piece->pos.x += piece->spd.x;
            piece->pos.y += piece->spd.y;
            piece->pos.z += piece->spd.z;
            piece->spd.y -= rubble_gravity;
            piece->spd.x *= rubble_drag;
            piece->spd.y *= rubble_drag;
            piece->spd.z *= rubble_drag;
            piece->ang.x += piece->spin.x;
            piece->ang.y += piece->spin.y;
            piece->ang.z += piece->spin.z;
            piece->spin.x = (Angle)(piece->spin.x * rubble_spin_keep);
            piece->spin.y = (Angle)(piece->spin.y * rubble_spin_keep);
            piece->spin.z = (Angle)(piece->spin.z * rubble_spin_keep);
            if (piece->smokes && rand() * RAND_SCALE < puff_chance) {
                /* @bug logic: a flying piece's puff takes a scale from 9 to
                 * 9.2, where flight_puff_scale, 10, times the random factor
                 * would give 9 to 11. the random share of SPREAD is added
                 * after the product at 0x8c50561e, not within it */
                golem_dust_smoke_behind(&piece->pos, &piece->spd, 0.0F,
                                        (flight_puff_scale * SPREAD_LOW) +
                                            (rand() * (SPREAD * RAND_SCALE)));
            }
            if (++piece->steps >= piece->life || piece->pos.y < 0.0F) {
                piece->live = FALSE;
                LIVE_PIECES(task)--;
                golem_e->rubble_count--;
            }
        }
    }
    if (LIVE_PIECES(task) <= 0) {
        golem_e->rubble_count -= LIVE_PIECES(task);
        task_kill(task);
    }
}

static void rubble_draw(struct task *task)
{
    struct rubble *piece;
    Sint32 i;
    Float age;
    Float size;
    NJS_VECTOR scl;

    njSetTexture(RUBBLE(task)->texlist);
    for (i = 0; i < RUBBLE_PIECES; i++) {
        piece = &RUBBLE(task)[i];
        if (piece->live) {
            age = (Float)piece->steps / (Float)piece->life;
            size = 1.0F - (age * LIFE_SHRINK);
            scl.x = piece->scl.x * size;
            scl.y = piece->scl.y * size;
            scl.z = piece->scl.z * size;
            njPushMatrixEx();
            njTranslateEx(&piece->pos);
            njRotateXYZ(NULL, piece->ang.x, piece->ang.y, piece->ang.z);
            if (golem_e_parts_built) {
                njScaleEx(&scl);
                if (piece->model == golem_e_rubble_model) {
                    njDirectDrawObject(part_builds[BUILD_RUBBLE].built);
                } else if (piece->model == golem_e_piece_model) {
                    njDirectDrawObject(part_builds[BUILD_PIECE].built);
                }
            } else {
                golem_model_draw_scaled_v(piece->model, &scl);
            }
            njPopMatrixEx();
        }
    }
}

static void weak_target_init(struct task *task)
{
    struct task *weak_point = task->parent;
    struct task_work *work = task->work;

    task->step = weak_target_step;
    task->destructor = weak_target_destroy;
    work->pos = weak_point->work->pos;
    /* @hack spelling: the offset read through task->work, where work would
     * do */
    work->pos.x += TARGET_OFFSET(task->work).x;
    work->pos.y += TARGET_OFFSET(task->work).y;
    work->pos.z += TARGET_OFFSET(task->work).z;
    collision_init(task, &target_shape, TARGET_SHAPES, COLLISION_ITEM);
    enemy_work_alloc(task);
}

static void weak_target_destroy(struct task *task)
{
    /* @bug memory: reads through a null enemy work where enemy_work_alloc
     * found no room. that call then kills the target, and this destructor runs
     * with no enemy work in the target's motion. enemy_destroy reads the motion
     * unchecked. weak_target_init sets the destructor at 0x8c505fea, before the
     * call at 0x8c50603a */
    enemy_destroy(task);
}

/* a hit marks the target struck in its weak point's record, and kills it */
static void weak_target_step(struct task *task)
{
    struct task *weak_point = task->parent;
    struct task_work *work = task->work;
    struct task_work *weak_point_work;
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    NJS_POINT3 offset;
    Float x;
    Float z;
    Angle ang;

    /* @hack spelling: weak_point_work, a copy of the weak point's work, where
     * weak_point->work would do */
    weak_point_work = weak_point->work;
    work->pos = weak_point_work->pos;
    /* @hack spelling: the offset read through task->work, where work would
     * do */
    work->pos.x += TARGET_OFFSET(task->work).x;
    work->pos.y += TARGET_OFFSET(task->work).y;
    work->pos.z += TARGET_OFFSET(task->work).z;
    if (work->flags & WORK_HURT) {
        spd = smoke_spd;
        /* NOLINTBEGIN(bugprone-integer-division) */
        ang = golem_e->heading + GOLEM_E_WEAK_POINT(weak_point)->ang.y +
              ((Float)smoke_spread * rand() * RAND_SCALE) - (smoke_spread / 2);
        /* NOLINTEND(bugprone-integer-division) */
        golem_polar_xz(ang, spd.z, &x, &z);
        spd.x = x * RANDOM_FROM(WIDE_LOW, WIDE_SPREAD);
        spd.z = z * RANDOM_FROM(WIDE_LOW, WIDE_SPREAD);
        spd.y *= RANDOM_FROM(WIDE_LOW, WIDE_SPREAD);
        pos = work->pos;
        dust_create(&pos, &spd,
                    smoke_size + (rand() * smoke_size_spread * RAND_SCALE));
        /* @hack spelling: the offset read through task->work, where work
         * would do */
        golem_point_turn_y(
            TARGET_OFFSET(task->work).x,
            TARGET_OFFSET(task->work).z + target_flash_z,
            golem_e->heading + GOLEM_E_WEAK_POINT(weak_point)->ang.y, &offset);
        flash_manager_create(pos.x + offset.x, pos.y, pos.z + offset.z,
                             TARGET_FLASH_SCALE);
        /* @hack spelling: the index read through task->work, where work
         * would do */
        GOLEM_E_WEAK_POINT(weak_point)->struck[TARGET_INDEX(task->work)] = TRUE;
        sound_play(BREAK_SOUND, BREAK_SOUND_KEY(weak_point), 0, 0);
        sound_set_volume(BREAK_SOUND_KEY(weak_point), SOUND_ANY_ID,
                         target_volume);
        task_kill(task);
    } else {
        /* @hack spelling: the turn in two statements, where one would do */
        ang = golem_turn_to_player();
        ang -= GOLEM_E_WEAK_POINT(weak_point)->ang.y;
        if (_builtin_fabsf((Float)ang) < target_arc) {
            collision_register(task);
        }
    }
}
