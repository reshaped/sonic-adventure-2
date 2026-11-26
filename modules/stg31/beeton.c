/* E BEETON, Green Hill's Beeton. it waits unseen until a player comes near,
 * then flies to its home from scl.x behind it. there it fires bursts while a
 * player stays near home. once none does, it flies off along its heading.
 * batabata.c makes the Batabata from this source, by including it with
 * BATABATA defined */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/light.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/enemy_bullet.h"
#include "shared/enemy_jet.h"
#include "sound/sound.h"
#include "stg31/enemies.h"
#include "stg31/flier_art.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* FLIER(name) is name with the flier's prefix: green_hill_beeton_ here, and
 * green_hill_batabata_ where batabata.c defines it first. batabata.c defines
 * its own FLIER_DEBRIS_SLOTS and FLIER_ACTION_COUNT too, the lengths of pieces
 * and actions */
#ifndef BATABATA
#define FLIER(name) green_hill_beeton_##name
#define TEXTURE_NAME "E_BEETONTEX"
enum { FLIER_DEBRIS_SLOTS = 6 };
#define BODY_RADIUS 7.0F
#else
#define TEXTURE_NAME "E_BATATEX"
#define BODY_RADIUS 12.0F
#endif

#ifndef BATABATA
enum { FLIER_ACTION_COUNT = 4 };
#endif

/* the flier's work->mode */
enum flight_mode {
    FLIGHT_INIT,
    /* unseen and untouchable. it ends once a player is near, and for the
     * Batabata once its timer has reached INTERVAL too */
    FLIGHT_WAIT,
    FLIGHT_APPROACH, /* the Beeton flies home, and the Batabata swoops down */
#ifdef BATABATA
    FLIGHT_ASCEND, /* the Batabata swoops up from home */
#else
    FLIGHT_FIRE, /* the Beeton fires bursts at home */
    FLIGHT_DEPART, /* the Beeton flies off along its heading */
#endif
    FLIGHT_DEFEATED,
    FLIGHT_DEAD
};

/* indexes in actions, the flier's table of motions */
enum flight_action {
    FLYING,
#ifndef BATABATA
    UNFOLD_GUN,
    AIM_GUN,
    FOLD_GUN
#else
    FLAP_WINGS
#endif
};

/* in the work's flags: the last search found a player near */
#define PLAYER_NEAR 0x1000
/* a search reaches RANGE_BASE further than the placement asks. a waiting
 * flier searches within scl.z's whole part plus RANGE_BASE. it searches from
 * home plus enemy->offset, which enemy_set_offset takes from scl.z's fraction.
 * a firing Beeton searches from home, within ATTACK_RANGE plus RANGE_BASE */
#define RANGE_BASE 60.0F
/* an angle's high byte. a placement's ang.y names the flier's group in its
 * low byte */
#define HEADING_BITS 0xff00

/* the flier's state, in the fields of its enemy_work that enemy.h leaves to
 * each kind of enemy */

/* the flier's heading: ang.y without its low byte. the Batabata adds its ang.z
 * to that */
#define FLIGHT_HEADING(enemy) ((enemy)->unk_01c)
#define FLIGHT_PITCH(enemy) ((enemy)->unk_020) /* the Batabata's pitch */
/* what fly adds to spd.x and spd.z each step. nothing sets its z */
#define ACCEL(enemy) ((enemy)->unk_00c)
/* fly bobs the flier up and down by scl.y as BOB_ANGLE turns BOB_STEP a
 * step */
#define BOB_ANGLE(enemy) ((enemy)->unk_1c4)
/* the steps the work's timer counts before the next attack: a Beeton's
 * burst, or a Batabata's swoop. a Beeton's is ang.z as placed, at least
 * MIN_INTERVAL */
#define INTERVAL(enemy) ((enemy)->unk_048)
/* the Beeton's attack range, scl.y as placed. the init then makes scl.y 1, the
 * bob's height */
#define ATTACK_RANGE(enemy) ((enemy)->unk_098)
/* the shots of a Beeton's burst, ang.x as placed. work->tick counts them */
#define SHOTS(enemy) ((enemy)->unk_047)
#define SHOT_SPEED(enemy) ((enemy)->unk_094) /* the Beeton's shot speed */
/* the Batabata's signed distance from home along its heading. a swoop starts
 * it at -scl.x, and ends once it has passed scl.x */
#define SWING_OFFSET(enemy) ((enemy)->unk_094)
/* the offset advance_sweep moves. no other code reads it */
#define SWEEP_OFFSET(enemy) ((enemy)->unk_09c)

/* the most ang.y turns in a step, toward a player or back to the heading. a
 * shot's heading lies within TURN_STEP of ang.y */
#define TURN_STEP 0x800
#define BOB_STEP 0x300
#define SIGHT_ANGLE 0x71c7
/* a Beeton's least INTERVAL. once it arrives home, its timer counts up from
 * INTERVAL less MIN_INTERVAL */
#define MIN_INTERVAL 60
/* the countdown in the work's submode from one shot of a burst to the
 * next */
#define SHOT_DELAY 40
/* the Beeton's acceleration as it flies home or off, and its top speed */
#define APPROACH_ACCELERATION 0.1F
#define TOP_SPEED 2.0F
/* the index in lights of the light the flier draws under */
#define FLIER_LIGHT 8
#define SHADOW_SIZE 6.0F
#define SHADOW_RATIO 1.5F
/* the texture task's level */
#define TEXTURE_LEVEL 0
/* a shot leaves at MUZZLE_SPEED. it leaves MUZZLE_X from the work's pos along
 * the heading ang.y gives, and MUZZLE_Y above it */
#define MUZZLE_X 4.0F
#define MUZZLE_Y (-13.4F)
#define MUZZLE_SPEED 5.0F
/* AIM_PITCH is an eighth turn down. a shot's pitch is the pitch of the
 * player's center from the muzzle, held within EIGHTH_TURN of AIM_PITCH. it
 * lies from level to straight down. with the player's center at the muzzle,
 * the player's pitch counts as POINT_BLANK_PITCH */
#define AIM_PITCH 0xe000
#define POINT_BLANK_PITCH 0xd000
/* the exhaust ports lie at -JET_X and JET_X, JET_Y and JET_Z in the body
 * node's axes. a jet leaves along the node's z at JET_SPEED plus
 * JET_SPEED_SHARE times spd.x */
#define JET_X 4.9F
#define JET_Y (-3.3F)
#define JET_Z (-3.9F)
#define JET_SPEED (-0.4F)
#define JET_SPEED_SHARE 0.5F
/* how far a swing moves the Batabata along its swoop in a step */
#define SWING_STEP 2.0F
/* the Batabata's pitch is -SWING_PITCH as a swoop starts, and SWING_PITCH as
 * the swoop passes scl.x */
#define SWING_PITCH ((Float)QUARTER_TURN)
/* the Batabata flaps up from home once it comes this near */
#define SWING_HOME_DISTANCE 30.0F
/* the Batabata's INTERVAL after its first swoop */
#define SWING_WAIT 180
/* the points score_add_enemy adds for a defeated flier: SCORE, or
 * SCORE_KNOCK where player_touch_attacks returns TRUE */
#define SCORE 100
#define SCORE_KNOCK 200
/* the scale a defeated flier's ball grows to, under ENEMY_IN_MAGIC_HAND */
#define BALL_SCALE 0.17F

/* the two jets' points and velocities in world space, which the draw keeps
 * for the next step's jets. a flier's task->extra holds the record */
struct exhaust_ports {
    NJS_POINT3 position[2];
    NJS_VECTOR velocity[2];
};

/* the drawing flier's ports and enemy_work, and the inverse of the matrix
 * its draw starts under. capture_exhaust, which takes no task, reads them */
static NJS_MATRIX FLIER(inverse_view);
static struct exhaust_ports *FLIER(drawing_ports);
static struct enemy_work *FLIER(drawing_enemy);

/* clang-format off */
static struct collision_shape body_shape = {
    0, SHAPE_SPHERE,
    PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(1),
    SHAPE_UNTURNED | SHAPE_HURTABLE,
    {0.0F, 0.0F, 0.0F}, BODY_RADIUS, 0.0F, 0.0F, 0, {0, 0, 0}};
/* clang-format on */
static NJS_CNK_MODEL *pieces[FLIER_DEBRIS_SLOTS] = {
#include "stg31/beeton/pieces.inc"
};
static struct enemy_action actions[FLIER_ACTION_COUNT] = {
#include "stg31/beeton/actions.inc"
};
/* the task whose destructor releases the texture list, which the first
 * flier's init loads */
static struct task *texture_task = NULL;

static void draw(struct task *task);
static void destroy(struct task *task);

static void textures_release(struct task *task)
{
    njReleaseTexture(&FLIER(texlist));
    texture_task = NULL;
}

static void textures_step(struct task *task)
{
}

static void flight_init(struct task *task, struct task_work *work)
{
    struct enemy_work *enemy;
    struct exhaust_ports *ports;

#ifdef NONMATCHING
    /* a disc-free build's actions hold zeros. a blend of 0 into FLYING never
     * passes, so the flier is killed and its placement stopped */
    if (actions[FLYING].blend == 0.0F) {
        placement_kill(task);
        return;
    }
#endif
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((ports = syCalloc(1, sizeof(*ports))) == NULL) {
        return;
    }
    task->extra.p = ports;
    enemy = enemy_work_alloc_grouped(task);
#ifdef NONMATCHING
    if (enemy == NULL) {
        /* enemy_work_alloc_grouped returns null once it has killed task.
         * task_destroy then frees ports through task->extra */
        return;
    }
#endif
    collision_init(task, &body_shape, 1, COLLISION_ENEMY);
#ifdef BATABATA
    SWING_OFFSET(enemy) = -work->scl.x;
    enemy->motion.object = &FLIER(body);
    FLIGHT_HEADING(enemy) = (work->ang.y & HEADING_BITS) + work->ang.z;
    work->ang.x = 0;
#else
    SHOTS(enemy) = work->ang.x;
    INTERVAL(enemy) = work->ang.z;
    if (INTERVAL(enemy) < MIN_INTERVAL) {
        INTERVAL(enemy) = MIN_INTERVAL;
    }
    SHOT_SPEED(enemy) = MUZZLE_SPEED;
    ATTACK_RANGE(enemy) = work->scl.y;
    enemy->motion.object = &FLIER(body);
    enemy_bullet_load();
    FLIGHT_HEADING(enemy) = work->ang.y & HEADING_BITS;
    work->ang.x = 0;
    work->ang.z = 0;
    work->scl.y = 1.0F;
#endif
    enemy_set_offset(work, enemy);
    enemy->sight_distance2 = RANGE_BASE * RANGE_BASE;
    enemy->sight_angle = SIGHT_ANGLE;
    enemy->near_distance2 =
        (work->scl.z + RANGE_BASE) * (work->scl.z + RANGE_BASE);
    enemy->spd.x = 0.0F;
#ifndef BATABATA
    enemy->turn_speed = TURN_STEP;
#endif
    enemy_set_home(work, enemy);
    enemy->motion.table = actions;
    work->mode = FLIGHT_WAIT;
    enemy->motion.next = FLYING;
    enemy_motion_step(&enemy->motion);
    enemy->shadow_size = SHADOW_SIZE;
    enemy->shadow_ratio = SHADOW_RATIO;
    enemy->flags |= ENEMY_BURST;
    enemy->flags |= ENEMY_RELEASES_ANIMAL;
    task->draw = draw;
    task->destructor = destroy;
#ifndef BATABATA
    enemy_jet_load_texture();
#endif
    if (!texture_task) {
        texture_load_pvm(TEXTURE_NAME, &FLIER(texlist));
        texture_task = task_create(0, TEXTURE_LEVEL, textures_step, "ManTex");
#ifdef NONMATCHING
        /* where task_create fails, this side releases the textures it just
         * loaded and kills the flier, with no draw left to use them. the
         * matching side writes the destructor through the null */
        if (texture_task == NULL) {
            njReleaseTexture(&FLIER(texlist));
            task->draw = NULL;
            task_kill(task);
            return;
        }
#endif
        texture_task->destructor = textures_release;
    }
}

#ifndef BATABATA
/* @hack redundant: ports, a parameter face_player never reads */
static void face_player(struct task_work *work, struct enemy_work *enemy,
                        struct exhaust_ports *ports)
{
    int player;

    if (game_state.time_stopped) {
        return;
    }
    if (work->flags & PLAYER_NEAR) {
        player = player_nearest(&work->pos);
        if (player == 0 || player == 1) {
            enemy_turn_to_player(work, enemy, player);
        }
    }
}
#endif

static void search_near_offset(struct task_work *work, struct enemy_work *enemy)
{
    if (enemy_find_player_near_offset(work, enemy)) {
        work->flags |= PLAYER_NEAR;
    } else {
        work->flags &= ~PLAYER_NEAR;
    }
}

static void check_hit(struct task *task, struct task_work *work,
                      struct enemy_work *enemy)
{
    NJS_VECTOR offset;
    NJS_VECTOR scale;
    int points;
    int player;

    if (!enemy_take_hit(work, enemy)) {
        return;
    }
    /* @hack redundant: an empty test of game_state.time_stopped */
    if (game_state.time_stopped) {
    }
    work->mode = FLIGHT_DEFEATED;
    work->collision->shapes->flags |= SHAPE_OFF;
    work->timer = 0;
    enemy_set_home(work, enemy);
    enemy_break(work, pieces, &FLIER(texlist));
    points = SCORE;
    if (player_touch_attacks(task)) {
        points = SCORE_KNOCK;
    }
    score_add_enemy(points);
    enemy_defeat(work, enemy);
    if (enemy->flags & ENEMY_IN_MAGIC_HAND) {
        player = enemy->shot_by;
        offset.x = 0.0F;
        offset.y = 0.0F;
        offset.z = 0.0F;
        /* @hack spelling: one assignment to the three, where a statement for
         * each would do */
        scale.x = scale.y = scale.z = BALL_SCALE;
        enemy_ball_create(player, &FLIER(body), &FLIER(texlist), &scale,
                          &offset);
    }
}

#ifdef BATABATA
/* move the Batabata SWING_STEP further along its swoop, a parabola through
 * home. with f as SWING_OFFSET over |scl.x|, it lies scl.y times f squared
 * above home. its pitch is SWING_PITCH times f squared, with f's sign */
static void swing(struct task_work *work, struct enemy_work *enemy)
{
    Float fraction;

    if (game_state.time_stopped) {
        return;
    }
    SWING_OFFSET(enemy) += SWING_STEP;
    fraction = SWING_OFFSET(enemy) * njInvertSqrt(work->scl.x * work->scl.x);
    work->pos.x =
        enemy->home.x + (SWING_OFFSET(enemy) * njCos(FLIGHT_HEADING(enemy)));
    work->pos.y = enemy->home.y + (work->scl.y * fraction * fraction);
    work->pos.z =
        enemy->home.z - (SWING_OFFSET(enemy) * njSin(FLIGHT_HEADING(enemy)));
    FLIGHT_PITCH(enemy) = fraction * njAbs(fraction) * SWING_PITCH;
}
#else
static void search_near_home(struct task_work *work, struct enemy_work *enemy)
{
    if (enemy_find_player_near_home(work, enemy)) {
        work->flags |= PLAYER_NEAR;
    } else {
        work->flags &= ~PLAYER_NEAR;
    }
}

static void fly(struct task_work *work, struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    enemy->spd.x += ACCEL(enemy).x;
    enemy->spd.z += ACCEL(enemy).z;
    if (enemy->spd.x > TOP_SPEED) {
        enemy->spd.x = TOP_SPEED;
    }
    work->pos.x += enemy->spd.x * njCos(FLIGHT_HEADING(enemy));
    /* @hack spelling: an addition of the negated product, where -= would
     * do */
    work->pos.z += -(enemy->spd.x * njSin(FLIGHT_HEADING(enemy)));
    work->pos.y -= njSin(BOB_ANGLE(enemy)) * work->scl.y;
    BOB_ANGLE(enemy) += BOB_STEP;
    work->pos.y += njSin(BOB_ANGLE(enemy)) * work->scl.y;
}

static void muzzle_to_world(struct task_work *work, NJS_POINT3 *position)
{
    njPushMatrix(&_nj_unit_matrix_);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, position, position);
    njPopMatrixEx();
}

/* @hack redundant: ports, a parameter aim_shot never reads */
static void aim_shot(struct task_work *work, struct exhaust_ports *ports,
                     NJS_POINT3 *muzzle, NJS_VECTOR *velocity)
{
    NJS_POINT3 target;
    Angle pitch;
    Angle heading;
    Angle target_pitch;
    Angle target_heading;
    Float distance;
    Float rise;

    target = player_work[player_nearest(&work->pos)]->collision->shapes->center;
    distance = njDistanceP2P(muzzle, &target);
    rise = target.y - muzzle->y;
    if (distance != 0.0F) {
        target_pitch = asinf(rise / distance) * ANGLE_PER_RADIAN;
        target_heading = atan2f(muzzle->z - target.z, target.x - muzzle->x) *
                         ANGLE_PER_RADIAN;
    } else {
        target_pitch = POINT_BLANK_PITCH;
        target_heading = work->ang.y;
    }
    pitch = angle_approach(AIM_PITCH, target_pitch, EIGHTH_TURN);
    heading = angle_approach(work->ang.y, target_heading, TURN_STEP);
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, heading);
    njRotateZ(NULL, pitch);
    njCalcVector(NULL, velocity, velocity);
    njPopMatrixEx();
}

static void fire_shot(struct task *task, struct task_work *work,
                      struct enemy_work *enemy)
{
    struct exhaust_ports *ports = task->extra.p;
    NJS_POINT3 muzzle;
    NJS_VECTOR velocity;

    muzzle.x = MUZZLE_X;
    muzzle.y = MUZZLE_Y;
    muzzle.z = 0.0F;
    muzzle_to_world(work, &muzzle);
    velocity.x = SHOT_SPEED(enemy);
    velocity.y = 0.0F;
    velocity.z = 0.0F;
    aim_shot(work, ports, &muzzle, &velocity);
    enemy_bullet_fire(task, ENEMY_BULLET_STOPS, &velocity, &muzzle);
}
#endif

static void destroy(struct task *task)
{
    enemy_destroy(task);
#ifndef BATABATA
    sound_stop_key(task);
    /* @hack redundant: a return after the last call */
    return; /* NOLINT(readability-redundant-control-flow) */
#endif
}

static void wait_for_player(struct task *task, struct task_work *work,
                            struct enemy_work *enemy)
{
    search_near_offset(work, enemy);
    if (!game_state.time_stopped) {
        work->timer++;
#ifdef BATABATA
        if (INTERVAL(enemy) <= work->timer) {
            work->timer = INTERVAL(enemy);
        }
#endif
    }
#ifdef BATABATA
    if (work->timer >= INTERVAL(enemy) && (work->flags & PLAYER_NEAR)) {
        work->mode = FLIGHT_APPROACH;
        work->timer = 0;
        SWING_OFFSET(enemy) = -work->scl.x;
        work->pos.x =
            enemy->home.x - (work->scl.x * njCos(FLIGHT_HEADING(enemy)));
        work->pos.y = enemy->home.y + work->scl.y;
        work->pos.z =
            enemy->home.z + (work->scl.x * njSin(FLIGHT_HEADING(enemy)));
    }
#else
    if (work->flags & PLAYER_NEAR) {
        work->mode = FLIGHT_APPROACH;
        work->timer = 0;
        enemy->near_distance2 = (ATTACK_RANGE(enemy) + RANGE_BASE) *
                                (ATTACK_RANGE(enemy) + RANGE_BASE);
        /* @hack spelling: an addition of the negated product, where -= would
         * do */
        work->pos.x += -(work->scl.x * njCos(FLIGHT_HEADING(enemy)));
        work->pos.z += work->scl.x * njSin(FLIGHT_HEADING(enemy));
        enemy->spd.x = 0.0F;
        ACCEL(enemy).x = APPROACH_ACCELERATION;
    }
#endif
}

static void approach(struct task *task, struct task_work *work,
                     struct enemy_work *enemy)
{
    if (!game_state.time_stopped) {
        work->timer++;
#ifdef BATABATA
        swing(work, enemy);
        if (njDistanceP2P(&enemy->home, &work->pos) < SWING_HOME_DISTANCE) {
            enemy->motion.next = FLAP_WINGS;
            work->mode = FLIGHT_ASCEND;
            work->timer = 0;
        }
#else
        fly(work, enemy);
        if (njDistanceP2P(&enemy->home, &work->pos) < njScalor(&enemy->spd)) {
            work->pos = enemy->home;
            enemy->motion.next = UNFOLD_GUN;
            work->mode = FLIGHT_FIRE;
            work->timer = INTERVAL(enemy) - MIN_INTERVAL;
            enemy->spd.x = 0.0F;
            ACCEL(enemy).x = 0.0F;
        }
#endif
    }
    check_hit(task, work, enemy);
    /* @hack redundant: a return after the last call */
    return; /* NOLINT(readability-redundant-control-flow) */
}

#ifdef BATABATA
static void ascend(struct task *task, struct task_work *work,
                   struct enemy_work *enemy)
{
    if (!game_state.time_stopped) {
        work->timer++;
        swing(work, enemy);
        if (work->scl.x < SWING_OFFSET(enemy)) {
            work->mode = FLIGHT_WAIT;
            enemy->motion.next = FLYING;
            INTERVAL(enemy) = SWING_WAIT;
            work->timer = 0;
        }
    }
    check_hit(task, work, enemy);
    /* @hack redundant: a return after the last call */
    return; /* NOLINT(readability-redundant-control-flow) */
}
#else
static void fire_bursts(struct task *task, struct task_work *work,
                        struct enemy_work *enemy)
{
    search_near_home(work, enemy);
    face_player(work, enemy, task->extra.p);
    fly(work, enemy);
    check_hit(task, work, enemy);
    if (work->mode == FLIGHT_DEFEATED) {
        return;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (!(work->flags & PLAYER_NEAR)) {
        work->timer = 0;
        enemy->motion.next = FOLD_GUN;
        work->mode = FLIGHT_DEPART;
        enemy->spd.x = 0.0F;
        ACCEL(enemy).x = APPROACH_ACCELERATION;
    } else if (SHOTS(enemy) > 0 && work->timer > INTERVAL(enemy) &&
               enemy->motion.action == AIM_GUN) {
        if (work->submode < 0) {
            fire_shot(task, work, enemy);
            work->tick++;
            work->submode = SHOT_DELAY;
        } else {
            work->submode--;
        }
        if (work->tick >= SHOTS(enemy)) {
            work->tick = 0;
            work->timer = 0;
        }
    }
}

static void depart(struct task *task, struct task_work *work,
                   struct enemy_work *enemy)
{
    if (!game_state.time_stopped) {
        work->timer++;
        work->ang.y =
            angle_approach(work->ang.y, FLIGHT_HEADING(enemy), TURN_STEP);
        if (angle_distance(work->ang.y, FLIGHT_HEADING(enemy)) < TURN_STEP) {
            fly(work, enemy);
        }
    }
    check_hit(task, work, enemy);
    /* @hack redundant: a return after the last call */
    return; /* NOLINT(readability-redundant-control-flow) */
}
#endif

/* move SWEEP_OFFSET toward scl.x by SWING_STEP, or back to -scl.x once it is
 * there. no code calls it, but the image holds it in the Beeton's object and
 * in the Batabata's. it is external, since SHC leaves out a static function
 * that its source never names */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void FLIER(advance_sweep)(struct task_work *work, struct enemy_work *enemy)
{
    if (work->scl.x > SWEEP_OFFSET(enemy)) {
        SWEEP_OFFSET(enemy) =
            float_approach(SWEEP_OFFSET(enemy), work->scl.x, SWING_STEP);
    } else {
        SWEEP_OFFSET(enemy) = -work->scl.x;
    }
}
/* NOLINTEND(misc-use-internal-linkage) */

/* the draw's motion callback. at the body's node it keeps the two ports' points
 * and velocities in drawing_ports. inverse_view takes them from the view's axes
 * to the world's. the Batabata keeps them too, though it makes no jets */
static void capture_exhaust(NJS_CNK_OBJECT *object)
{
    NJS_POINT3 position;
    NJS_VECTOR velocity;

    if (object == &FLIER(body)) {
        position.x = JET_X;
        position.y = JET_Y;
        position.z = JET_Z;
        njCalcPoint(NULL, &position, &position);
        njCalcPoint(&FLIER(inverse_view), &position,
                    &FLIER(drawing_ports)->position[1]);
        position.x = -JET_X;
        position.y = JET_Y;
        position.z = JET_Z;
        njCalcPoint(NULL, &position, &position);
        njCalcPoint(&FLIER(inverse_view), &position,
                    &FLIER(drawing_ports)->position[0]);
        velocity.z =
            JET_SPEED + (JET_SPEED_SHARE * FLIER(drawing_enemy)->spd.x);
        velocity.y = 0.0F;
        velocity.x = 0.0F;
        njCalcVector(NULL, &velocity, &velocity);
        njCalcVector(&FLIER(inverse_view), &velocity,
                     &FLIER(drawing_ports)->velocity[1]);
        velocity.z =
            JET_SPEED + (JET_SPEED_SHARE * FLIER(drawing_enemy)->spd.x);
        velocity.y = 0.0F;
        velocity.x = 0.0F;
        njCalcVector(NULL, &velocity, &velocity);
        njCalcVector(&FLIER(inverse_view), &velocity,
                     &FLIER(drawing_ports)->velocity[0]);
    }
}

static void draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    struct angles rotation;
    NJS_VECTOR scale;
    NJS_POINT3 position;
#ifdef BATABATA
    int group;
#else
    Float search_range;
#endif

    light_apply(FLIER_LIGHT);
    if (work->mode != FLIGHT_WAIT) {
        FLIER(drawing_enemy) = enemy;
        FLIER(drawing_ports) = task->extra.p;
        njCnkSetMotionCallback(capture_exhaust);
        njGetMatrix(&FLIER(inverse_view));
        njInvertMatrix(&FLIER(inverse_view));
        njPushMatrixEx();
        njTranslateEx(&work->pos);
#ifdef BATABATA
        njRotateY(NULL, FLIGHT_HEADING(enemy) + QUARTER_TURN);
        njRotateX(NULL, FLIGHT_PITCH(enemy));
#else
        njRotateY(NULL, work->ang.y + QUARTER_TURN);
#endif
        njSetTexture(&FLIER(texlist));
        if (enemy->motion.mode != ENEMY_MOTION_BLENDING) {
            njCnkSimpleDrawMotion(
                enemy->motion.object,
                enemy->motion.table[enemy->motion.action].motion,
                enemy->motion.frame);
        } else {
            njCnkSimpleDrawMotionLink(enemy->motion.object, &enemy->motion.link,
                                      enemy->motion.frame);
        }
        njPopMatrixEx();
        njCnkSetMotionCallback(NULL);
        enemy_draw_shadow(work, enemy);
    }
    /* @hack redundant: a block no path reaches, the SET editor's view. SHC
     * still places the block's strings in this source's constants, and the
     * block changes the code SHC writes for the rest of the draw. none of the
     * block's own code is in the image */
    if (0) {
#ifdef BATABATA
        group = (Uint8)work->ang.y;
        position = enemy->home;
        njPushMatrixEx();
        njTranslateEx(&position);
        njRotateY(NULL, FLIGHT_HEADING(enemy) + QUARTER_TURN);
        njCnkSimpleDrawMotion(&FLIER(body), actions[FLYING].motion, 0.0F);
        njRotateY(NULL, HALF_TURN);
        njCnkSimpleDrawMotion(&FLIER(body), actions[FLAP_WINGS].motion, 0.0F);
        njCnkSimpleDrawMotion(enemy->motion.object,
                              enemy->motion.table[enemy->motion.action].motion,
                              enemy->motion.frame);
        njPopMatrixEx();
        rotation.x = rotation.y = rotation.z = 0;
        scale.z = scale.x = work->scl.z + RANGE_BASE;
        scale.y = 1.0F;
        debug_draw_model_unlit(&work->pos, &rotation, &scale,
                               DEBUG_MODEL_CYLINDER);
        njPrintC(NJM_LOCATION(17, 16), "->");
        njPrintC(NJM_LOCATION(17, 17), "-> PLANE ROT Y");
        njPrintC(NJM_LOCATION(17, 18), "-> MOVE H RANGE");
        njPrintC(NJM_LOCATION(17, 19), "-> MOVE V RANGE");
        njPrintC(NJM_LOCATION(17, 20), "-> SEARCH RANGE/SCOPE");
        njPrintC(NJM_LOCATION(17, 21), "-> ANG Y ");
        njPrintH(NJM_LOCATION(26, 21), work->ang.y & 0xff00, 4);
        njPrintC(NJM_LOCATION(30, 21), " EME ");
        njPrintD(NJM_LOCATION(35, 21), group, 3);
#else
        njPrintC(NJM_LOCATION(17, 16), "-> RAPID ");
        njPrintC(NJM_LOCATION(17, 17), "-> INTERVAL ");
        njPrintC(NJM_LOCATION(34, 17), " FRAMES");
        njPrintC(NJM_LOCATION(17, 18), "-> START FROM");
        njPrintC(NJM_LOCATION(17, 19), "-> ATTACK RANGE");
        njPrintC(NJM_LOCATION(17, 20), "-> SEARCH RANGE/SCOPE");
        njPrintC(NJM_LOCATION(17, 21), "-> ANG Y ");
        njPrintC(NJM_LOCATION(30, 21), " EME ");
        njPrintD(NJM_LOCATION(26, 16), (Uint8)work->ang.x, 3);
        njPrintD(NJM_LOCATION(29, 17), work->ang.z, 5);
        njPrintH(NJM_LOCATION(26, 21), work->ang.y & 0xff00, 4);
        njPrintD(NJM_LOCATION(35, 21), (Uint8)work->ang.y, 3);
        position.x = work->pos.x - (work->scl.x * njCos(work->ang.y));
        position.y = work->pos.y;
        position.z = work->pos.z + (work->scl.x * njSin(work->ang.y));
        njPushMatrixEx();
        njTranslateEx(&position);
        njRotateY(NULL, work->ang.y + QUARTER_TURN);
        njCnkSimpleDrawMotion(enemy->motion.object,
                              enemy->motion.table[FLYING].motion, 0.0F);
        njCnkSimpleDrawMotion(enemy->motion.object,
                              enemy->motion.table[UNFOLD_GUN].motion, 0.0F);
        njCnkSimpleDrawMotion(enemy->motion.object,
                              enemy->motion.table[enemy->motion.action].motion,
                              enemy->motion.frame);
        njPopMatrixEx();
        rotation.x = rotation.y = rotation.z = 0;
        scale.z = scale.x = work->scl.y + RANGE_BASE;
        scale.y = 1.0F;
        debug_draw_model_unlit(&work->pos, &rotation, &scale,
                               DEBUG_MODEL_CYLINDER);
        search_range = work->scl.z;
        enemy_set_offset(work, enemy);
        rotation.x = rotation.y = rotation.z = 0;
        scale.z = scale.x = njRoundOff(work->scl.z) + RANGE_BASE;
        scale.y = 1.0F;
        position = work->pos;
        njAddVector(&position, &enemy->offset);
        debug_draw_model_unlit(&position, &rotation, &scale,
                               DEBUG_MODEL_CYLINDER);
        work->scl.z = search_range;
#endif
    } else {
        /* @hack redundant: an empty else */
    }
    light_apply(light_after_draw);
}

void FLIER(step)(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
#ifndef BATABATA
    struct exhaust_ports *ports = task->extra.p;
#endif

    if (work->mode != FLIGHT_INIT && work->mode != FLIGHT_APPROACH &&
        placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case FLIGHT_INIT:
        /* @hack spelling: a jump to the call of flight_init after the
         * return, where the call and a return here would do */
        goto init;
    case FLIGHT_WAIT:
        wait_for_player(task, work, enemy);
        break;
    case FLIGHT_APPROACH:
        approach(task, work, enemy);
        break;
#ifdef BATABATA
    case FLIGHT_ASCEND:
        ascend(task, work, enemy);
        break;
#else
    case FLIGHT_FIRE:
        fire_bursts(task, work, enemy);
        break;
    case FLIGHT_DEPART:
        depart(task, work, enemy);
        break;
#endif
    case FLIGHT_DEFEATED:
        work->mode = FLIGHT_DEAD;
        /* fall through */
    case FLIGHT_DEAD:
        placement_kill(task);
        /* @hack spelling: a jump to the function's end, where a return would
         * do */
        goto end;
    }
    /* @hack spelling: an empty arm for a time stop and the updates in the
     * else, where the updates under the negated test would do */
    if (game_state.time_stopped) {
    } else {
        enemy_save_pos(work, enemy);
        enemy_motion_step(&enemy->motion);
#ifndef BATABATA
        enemy_jet_create(NULL, ENEMY_JET_SMALL, &ports->position[1],
                         &ports->velocity[1]);
        enemy_jet_create(NULL, ENEMY_JET_SMALL, &ports->position[0],
                         &ports->velocity[0]);
#endif
    }
#ifndef BATABATA
    /* @hack redundant: a test no path reaches, which keeps work live through
     * the updates */
    if (0) {
        if (work) {
        }
    }
#endif
    if (work->mode != FLIGHT_WAIT && work->mode != FLIGHT_DEFEATED) {
        collision_register(task);
    }
    return;
init:
    flight_init(task, work);
    return;
end:;
}
