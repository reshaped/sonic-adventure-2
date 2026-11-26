#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "boss_last2/arena.h"
#include "boss_last2/body.h"
#include "boss_last2/body_art.h"
#include "boss_last2/motion_art.h"
#include "boss_last2/camera.h"
#include "boss_last2/joints.h"
#include "boss_last2/laser.h"
#include "boss_last2/panel_sparks.h"
#include "boss_last2/panels.h"
#include "boss_last2/reentry.h"
#include "boss_last2/sphere.h"
#include "boss_last2/stage.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "draw/light.h"
#include "draw/motion.h"
#include "player/player.h"
#include "shared/boss.h"
#include "shared/boss_title.h"
#include "shared/last_boss_arena.h"
#include "shared/last_boss_defend.h"
#include "shared/last_boss_sphere.h"
#include "shared/messenger.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"
#include "vmu/vibration.h"

/* the entries of body_motions */
enum body_entry {
    MOTION_INTRO,
    MOTION_IDLE,
    MOTION_HIT,
    MOTION_DEFEAT,
    MOTIONS
};
/* the work's flag that has the next step call finalhazard_camera_blend */
#define BODY_BLEND_CAMERA 0x100
/* the collision record's shapes, and its radius */
#define BODY_SHAPES 34
#define BODY_RADIUS 400.0F
/* the power the Finalhazard starts with. each hit on the weak point takes 1 */
#define BODY_POWER 6

/* whether frame lies between at and the frame after it */
#define WITHIN_FRAME(frame, at) ((frame) > (at) && (frame) < (at) + 1.0F)
/* the motions' frames at which sounds play */
#define INTRO_SOUND_FRAME 77.0F
#define HIT_SOUND_FRAME 10.0F
#define DEFEAT_HIT_FRAME 25.0F
#define DEFEAT_HIGH_HIT_FRAME 40.0F
#define DEFEAT_SOUND_FRAME 80.0F
/* the sounds' durations and volumes, as sound_play_for takes them. a hit
 * and the defeat play at SOUND_LOUDEST. the defeat's second hit plays at
 * HIGH_HIT_PITCH, the highest */
#define INTRO_DURATION 150
#define START_DURATION 150
#define START_VOLUME 120
#define HIT_DURATION 180
#define HIGH_HIT_PITCH 0x3fff
#define DEFEAT_DURATION 240
#define DEFEAT_LOOP_VOLUME 100
#define CONTACT_DURATION 30
#define CONTACT_VOLUME 30

/* the Finalhazard breathes only while the active player is within
 * BREATH_REACH of FINALHAZARD_JOINT_VOICE. each such step it breathes with
 * a chance of BREATH_CHANCE, at BREATH_VOLUME for BREATH_DURATION */
#define BREATH_REACH 300.0F
#define BREATH_CHANCE 0.005F
#define BREATH_VOLUME 90
#define BREATH_DURATION 240
/* player 0's pad's vibrations, by vibration_start's power, freq and time */
#define CONTACT_RUMBLE() vibration_start(0, 1, 5, VIBRATION_UNTIMED)
#define HIT_RUMBLE() vibration_start(0, 3, 24, 2)
#define HIT_MOTION_RUMBLE() vibration_start(0, 3, 24, 4)
#define EXPLOSION_RUMBLE() vibration_start(0, 3, 10, 1)
/* the intro's message is the stage message numbered by the retries, up to
 * LAST_MESSAGE */
#define LAST_MESSAGE 3
/* FINALHAZARD_EXPOSE's wait after it moves the weak point */
#define EXPOSE_STEPS 60
/* the steps from an attack's start to its volley, or to the start of its
 * lasers' LASER_STEPS */
#define START_STEPS 60
#define LASER_STEPS 600
/* FINALHAZARD_BOTH_REPEAT's steps between moves of the weak point */
#define PANEL_MOVE_STEPS 600
/* the steps of the links into the hit motion and back to the idle one */
#define HIT_LINK_STEPS 90
#define IDLE_LINK_STEPS 60
/* while its frame is below DEFEAT_LOOP_END, the defeat plays
 * SOUND_DEFEAT_LOOP every DEFEAT_LOOP_STEPS. below BURN_END it shows sparks
 * every SPARK_STEPS and an explosion every EXPLOSION_STEPS */
#define DEFEAT_LOOP_END 100.0F
#define DEFEAT_LOOP_STEPS 45
#define BURN_END 120.0F
#define SPARK_STEPS 30
#define EXPLOSION_STEPS 10

/* the Finalhazard's own block, in its record's own */
struct finalhazard_body {
    Sint8 mode; /* a copy of the work's mode, which nothing reads */
    Uint8 unused[3];
    /* the attack's place in its row of attack_order */
    Sint8 attack_index;
    Sint8 last_player; /* the active player at the last step */
    Uint16 stun_timer; /* the stun's steps left */
    Uint16 panel_timer; /* the steps to the weak point's next move */
    Bool stunned;
    struct task *panel; /* the weak point's task */
    struct task *wave; /* the wave of spheres, or null */
    struct task *lasers; /* finalhazard_fire_lasers's task, or null */
};

const NJS_POINT3 finalhazard_center = {0, 0, 0};
static const struct motion_table_entry body_motions[MOTIONS] = {
#include "boss_last2/body/body_motions.inc"
};

/* the title: TITLE_LETTERS letters above a bar of TITLE_BAR pieces. each
 * letter waits 25 steps. the title centers on (320, 240), at a scale of 2, in
 * white. once the intro ends it fades out by TITLE_FADE a step */
#define TITLE_BAR 13
#define TITLE_LETTERS 15
#define TITLE_FADE 0.05F
static struct boss_title_letter title_bar[TITLE_BAR] = {
#include "boss_last2/body/title_bar.inc"
};
static struct boss_title_letter title_letters[TITLE_LETTERS] = {
#include "boss_last2/body/title_letters.inc"
};
static struct boss_title body_title = {
#include "boss_last2/body/body_title.inc"
};
static Sint8 starting_power = BODY_POWER;
static struct task *body_task = NULL;
static Angle starting_yaw = -QUARTER_TURN;
/* the modes the Finalhazard takes in turn: row n - 1 for n power left.
 * NO_ATTACK ends a row, and the order goes back to the row's start */
#define NO_ATTACK (-1)
/* attack_index before its row's first attack */
#define BEFORE_FIRST (-1)
/* clang-format off */
static Sint8 attack_order[BODY_POWER][4] = {
    {FINALHAZARD_BOTH_REPEAT, NO_ATTACK},
    {FINALHAZARD_BOTH, FINALHAZARD_EXPOSE, NO_ATTACK},
    {FINALHAZARD_DOUBLE_SWEEP, FINALHAZARD_SPHERES_MANY, FINALHAZARD_EXPOSE, NO_ATTACK},
    {FINALHAZARD_SWEEP, FINALHAZARD_SPHERES_MANY, FINALHAZARD_EXPOSE, NO_ATTACK},
    {FINALHAZARD_PULSE, FINALHAZARD_SPHERES_NORMAL, FINALHAZARD_EXPOSE, NO_ATTACK},
    {FINALHAZARD_SPHERES_FEW, NO_ATTACK},
};
/* clang-format on */
static struct collision_shape body_shapes[BODY_SHAPES] = {
#include "boss_last2/body/body_shapes.inc"
};
/* where each of body_shapes lies: its joint plus offset. a joint of 0 or
 * below leaves the shape fixed */
static struct last_boss_sphere_obstacle body_joints[BODY_SHAPES] = {
#include "boss_last2/body/body_joints.inc"
};

static void finalhazard_step(struct task *task);
static void finalhazard_destroy(struct task *task);
static void body_set_mode(struct task *task, int mode);

struct task *finalhazard_create(void)
{
    struct task *task;
    struct task_work *work;
    struct boss_record *record;
    struct finalhazard_body *body;
    struct task_motion *motion;

#ifdef NONMATCHING
    /* this side makes no Finalhazard where MOTION_INTRO's speed isn't above
     * 0. the intro ends only once its motion has played, and the pads stay off
     * until then */
    if (body_motions[MOTION_INTRO].speed <= 0.0F) {
        return NULL;
    }
#endif
    task = task_create(TASK_WORK, 2, finalhazard_step, "LastBoss2Exec");
    if (!task) {
        return NULL;
    }
    work = task->work;
    task->draw = finalhazard_draw;
    task->destructor = finalhazard_destroy;
    record = boss_record_create_parts(task, FALSE, TRUE);
    body = boss_record_alloc_own(record, sizeof(*body));
    if (!body) {
        task_destroy(task);
        return NULL;
    }
    motion = syCalloc(sizeof(*motion), 1);
    if (!motion) {
        task_destroy(task);
        return NULL;
    }
    task->motion.p = motion;
    work->pos = finalhazard_center;
    work->ang.y = starting_yaw;
    work->scl.x = work->scl.y = work->scl.z = 1.0F;
    record->power = starting_power;
    collision_init(task, body_shapes, BODY_SHAPES, COLLISION_ENEMY);
    /* @bug memory: reads and writes through null at 0x8c5046e4 and
     * 0x8c5046e8 where collision_init, at 0x8c5046da, couldn't allocate the
     * record */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above. it destroys the task and returns
     * null where collision_init made no record */
    if (!task->work->collision) {
        task_destroy(task);
        return NULL;
    }
#endif
    task->work->collision->flags &= ~COLLISION_TARGET;
    collision_set_radius(task, BODY_RADIUS);
    motion_init(record->block, body_motions);
    body_task = task;
    finalhazard_reentry_ring(task, NULL);
    finalhazard_test_mode = FINALHAZARD_TEST_OFF;
    record->focus = work->pos;
#ifdef NONMATCHING
    /* this side makes the weak point first. a failure then leaves no camera
     * reading the freed record's focus */
    body->panel = finalhazard_panel_create(task, finalhazard_players->active);
    if (!body->panel) {
        task_destroy(task);
        return NULL;
    }
    finalhazard_camera_target(&record->focus);
#else
    finalhazard_camera_target(&record->focus);
    body->panel = finalhazard_panel_create(task, finalhazard_players->active);
#endif
    return task;
}

static void finalhazard_destroy(struct task *task)
{
#ifdef DREAMCAST
    /* @hack redundant: the reads of the record and its own block, which
     * nothing uses */
    struct boss_record *record = task->extra.p;
    struct finalhazard_body *body = record->own;
#endif
    if (task->motion.p) {
        syFree(task->motion.p);
        task->motion.p = NULL;
    }
    boss_record_free_parts(task);
    body_task = NULL;
}

static void point_beyond_player(const NJS_POINT3 *origin,
                                const NJS_POINT3 *player, Float reach,
                                NJS_POINT3 *out)
{
    NJS_VECTOR direction;
    Float distance = vector_distance(origin, player);
    Float length;
    vector_diff(player, origin, &direction);
    njUnitVector(&direction);
    length = reach + distance;
    direction.x *= length;
    direction.y *= length;
    direction.z *= length;
    vector_sum(origin, &direction, out);
}

static void body_contact(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct task_work *work = task->work;
    struct finalhazard_body *body = record->own;
    struct collision_hit *hit;
    struct collision_shape *shape;
    NJS_POINT3 pos;
    NJS_VECTOR direction;
    /* @hack redundant: a vector of the frame that nothing reads or writes */
    NJS_VECTOR unused;
    Float speed;
    /* @hack type: the weak point's answer narrowed to Sint8, where the int
     * alone would do */
    if ((Sint8)finalhazard_panel_hit(body->panel) == 1 &&
        work->mode != FINALHAZARD_HIT) {
        record->power--;
        if (record->power > 0.0F) {
            body_set_mode(task, FINALHAZARD_HIT);
        } else {
            body_set_mode(task, FINALHAZARD_DEFEAT);
        }
        work->flags |= WORK_HURT;
        return;
    }
    hit = collision_next_player_hit(task);
    if (!hit) {
        return;
    }
    shape = &task->work->collision->shapes[hit->own_shape];
    /* @hack redundant: the shape's address tested for null, which it never
     * is */
    if (!shape) {
        return;
    }
    /* a touch whose attack beats the player's guard knocks the player back,
     * and stuns the Finalhazard. any other throws the player off. the guard
     * flare shows as far beyond the player as the throw's speed */
    speed = FINALHAZARD_PUSH_SPEED;
    point_beyond_player(&shape->center,
                        &player_work[finalhazard_players->active]->pos, speed,
                        &pos);
    vector_diff(&pos, &shape->center, &direction);
    if (last_boss_hit_penetrates(task, hit) == 1) {
        finalhazard_push_player(NULL, FINALHAZARD_KNOCKBACK);
        finalhazard_stun();
    } else {
        finalhazard_push_player(&direction, speed);
        CONTACT_RUMBLE();
    }
    last_boss_defend(&pos, &direction);
    sound_play_for(SOUND_CONTACT, task, 1, 0, CONTACT_DURATION);
    sound_set_volume(task, SOUND_CONTACT, CONTACT_VOLUME);
}

static void body_register(struct task *task)
{
    struct last_boss_sphere_obstacle *obstacle = body_joints;
    Sint8 i;
    for (i = 0; i < BODY_SHAPES; i++, obstacle++) {
        if (obstacle->joint > 0) {
            vector_sum(&finalhazard_joint_positions[obstacle->joint],
                       &obstacle->offset, &obstacle->position);
            collision_set_shape_center(task, i, &obstacle->position);
        }
    }
    collision_register(task);
}

struct task *finalhazard_task(void)
{
    /* @hack redundant: body_task tested for null before it is returned,
     * where returning it would do */
    if (!body_task) {
        return NULL;
    }
    return body_task;
}

struct task *finalhazard_weak_point(void)
{
    struct task *task = finalhazard_task();
    struct boss_record *record;
    struct finalhazard_body *body;
    /* @hack spelling: the lookup in an else, where statements after the
     * return would do */
    if (!task) {
        return NULL;
    } else {
        record = task->extra.p;
        body = record->own;
        /* @hack redundant: body->panel tested for null before it is
         * returned, where returning it would do */
        if (!body->panel) {
            return NULL;
        }
        return body->panel;
    }
}

static struct finalhazard_body *body_state(void)
{
    struct task *task = finalhazard_task();
    struct boss_record *record;
    if (!task) {
        return NULL;
    }
    record = task->extra.p;
    if (!record) {
        return NULL;
    }
    return record->own;
}

Sint8 finalhazard_hurt(const void *unused)
{
    struct task *task = finalhazard_task();
    struct task_work *work;
    if (!task) {
        return -1;
    }
    work = task->work;
    if (work->mode == FINALHAZARD_DEFEAT || (work->flags & WORK_HURT)) {
        return 1;
    }
    return 0;
}

/* kill the task an attack holds, where it lives, and clear the hold */
#define END_ATTACK(t)                                                          \
    do {                                                                       \
        if (t) {                                                               \
            task_kill(t);                                                      \
            (t) = NULL;                                                        \
        }                                                                      \
    } while (0)
#define END_ATTACKS(body)                                                      \
    do {                                                                       \
        END_ATTACK((body)->wave);                                              \
        END_ATTACK((body)->lasers);                                            \
    } while (0)

void finalhazard_clear_attacks(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct finalhazard_body *body = record->own;
    END_ATTACKS(body);
}

void finalhazard_blend_next_frame(struct task *task)
{
    struct task_work *work = task->work;
    work->flags |= BODY_BLEND_CAMERA;
}

static void body_breathe(struct task *task)
{
    Float distance =
        vector_distance(&player_work[finalhazard_players->active]->pos,
                        &finalhazard_joint_positions[FINALHAZARD_JOINT_VOICE]);
    if (distance < BREATH_REACH && rand() * RAND_SCALE < BREATH_CHANCE) {
        sound_play_at_for(
            SOUND_BREATH, task, 1, BREATH_VOLUME, BREATH_DURATION,
            &finalhazard_joint_positions[FINALHAZARD_JOINT_VOICE]);
        sound_set_pitch(task, SOUND_BREATH,
                        njSin(RANDOM_TURN()) * FINALHAZARD_BREATH_PITCH);
    }
}

int finalhazard_stunned(void)
{
    struct finalhazard_body *body = body_state();
    if (!body) {
        return -1;
    }
    return body->stunned;
}

int finalhazard_stun(void)
{
    struct task *task = finalhazard_task();
    struct finalhazard_body *body;
    if (!task) {
        return -1;
    }
    if (finalhazard_hurt(task) == 1) {
        return -1;
    }
    body = body_state();
    if (!body) {
        return -1;
    }
    body->stun_timer = FINALHAZARD_STUN_STEPS;
    body->stunned = 1;
    return FINALHAZARD_STUN_STEPS;
}

static int body_unstun(void)
{
    struct finalhazard_body *body = body_state();
    if (!body) {
        return -1;
    }
    body->stun_timer = 0;
    body->stunned = 0;
    return 0;
}

static int body_next_attack(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct finalhazard_body *body = record->own;
    Sint8 index = body->attack_index + 1;
    Sint8 row = (int)record->power - 1;
    if (attack_order[row][index] < 0) {
        index = 0;
    }
    body->attack_index = index;
    return attack_order[row][index];
}

static void body_set_mode(struct task *task, int mode)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    /* @hack redundant: the read of the own block, which nothing uses */
    struct finalhazard_body *body = record->own;
    /* @bug logic: refuses every change of mode while the Finalhazard is
     * hurt, FINALHAZARD_HIT and FINALHAZARD_DEFEAT included. the tests at
     * 0x8c504dae and 0x8c504db4 ask whether mode differs from FINALHAZARD_HIT
     * or from FINALHAZARD_DEFEAT, which every mode does */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above. it refuses a change only to a
     * mode other than FINALHAZARD_HIT and FINALHAZARD_DEFEAT */
    if (finalhazard_hurt(record) && mode != FINALHAZARD_HIT &&
        mode != FINALHAZARD_DEFEAT)
#else
    /* @hack type: mode narrowed to Sint8 in each test, where mode alone
     * would do */
    if (finalhazard_hurt(record) &&
        /* NOLINTNEXTLINE(misc-redundant-expression) */
        ((Sint8)mode != FINALHAZARD_HIT || (Sint8)mode != FINALHAZARD_DEFEAT))
#endif
    {
        return;
    }
    work->mode = mode;
    work->submode = 0;
}

static void body_intro(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct finalhazard_body *body = record->own;
    struct motion_control *motion = record->block;
    struct motion_play *current;
    int message;

    switch (work->submode) {
    case 0:
        pad_disable_player(PAD_BOTH_PLAYERS);
        finalhazard_camera_motion(FINALHAZARD_CAMERA_INTRO, 0,
                                  body_motions[MOTION_INTRO].speed);
        motion_set(motion, MOTION_INTRO);
        boss_gauge_create(task, &record->power);
        boss_title_create(task, &body_title);
        work->submode++;
        /* fall through */
    case 1:
        /* @hack spelling: the assignment of current inside the test, where
         * a statement of its own would do */
        if ((current = &motion->current)->frame > INTRO_SOUND_FRAME &&
            current->frame < INTRO_SOUND_FRAME + 1.0F) {
            sound_play_for(SOUND_INTRO, task, 1, 0, INTRO_DURATION);
            /* @bug logic: sets the volume of SOUND_START at 0x8c504e52, not
             * of SOUND_INTRO, which plays at 0x8c504e46 */
            sound_set_volume(task, SOUND_START, SOUND_LOUDEST);
        }
        if (motion_held(motion)) {
            pad_enable_player(finalhazard_players->active);
            motion_link(motion, MOTION_IDLE);
            body->attack_index = BEFORE_FIRST;
            boss_title_fade(TITLE_FADE);
            message = stage_get_retries();
            if (message > LAST_MESSAGE) {
                message = LAST_MESSAGE;
            }
            messenger_create(&work->pos, 0, 0, message, MESSENGER_TALK_WINDOW,
                             task);
            body_set_mode(task, FINALHAZARD_EXPOSE);
            sound_play_for(SOUND_START, task, 1, 0, START_DURATION);
            sound_set_volume(task, SOUND_START, START_VOLUME);
        }
        break;
    }
    finalhazard_players->ring_timer = 0;
}

static void body_expose(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct finalhazard_body *body = record->own;
    switch (work->submode) {
    case 0:
        work->timer = EXPOSE_STEPS;
        finalhazard_panel_move(body->panel, finalhazard_players->active);
        work->submode++;
        /* fall through */
    case 1:
        if (--work->timer == 0) {
            body_set_mode(task, body_next_attack(task));
        }
        break;
    }
}

static void body_wait(struct task *task)
{
    /* @hack redundant: the reads of the work, the record, its own block and
     * its motion control, which nothing uses */
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct finalhazard_body *body = record->own;
    struct motion_control *motion = record->block;
}

static void body_hit(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct finalhazard_body *body = record->own;
    struct motion_control *motion = record->block;
    struct motion_play *current;

    switch (work->submode) {
    case 0:
        END_ATTACKS(body);
        motion_link_frames(motion, MOTION_HIT, HIT_LINK_STEPS);
        HIT_RUMBLE();
        work->submode++;
        break;
    case 1:
        if (!(motion->flags & MOTION_LINKING)) {
            finalhazard_camera_motion(FINALHAZARD_CAMERA_HIT, 0,
                                      body_motions[MOTION_HIT].speed);
            finalhazard_panel_hide(body->panel);
            HIT_MOTION_RUMBLE();
            pad_disable_player(PAD_BOTH_PLAYERS);
            player_stop(player_motion[0], player_records[0]);
            stage_place_player_again(0);
            player_stop(player_motion[1], player_records[1]);
            stage_place_player_again(1);
            work->submode++;
        }
        break;
    case 2:
        /* @hack spelling: the assignment of current inside the test, where
         * a statement of its own would do */
        if ((current = &motion->current)->frame > HIT_SOUND_FRAME &&
            current->frame < HIT_SOUND_FRAME + 1.0F) {
            sound_play_for(SOUND_HIT, task, 1, 0, HIT_DURATION);
            sound_set_volume(task, SOUND_HIT, SOUND_LOUDEST);
        }
        if (motion_ended(motion)) {
            work->flags &= ~WORK_HURT;
            finalhazard_panel_rearm(body->panel);
            body->attack_index = BEFORE_FIRST;
            motion_link_frames(motion, MOTION_IDLE, IDLE_LINK_STEPS);
            body_set_mode(task, FINALHAZARD_WAIT);
            finalhazard_players->hit_switch = 1;
            finalhazard_switch_players();
        }
        break;
    }
    finalhazard_players->ring_timer = 0;
}

/* whether query says the attack in task has finished */
#ifdef NONMATCHING
/* this side counts a missing attack as finished, as finalhazard_sphere_finished
 * does already, where finalhazard_lasers_finished returns -1 */
#define ATTACK_FINISHED(query, task) (!(task) || (query)(task) == 1)
#else
#define ATTACK_FINISHED(query, task) ((query)(task) == 1)
#endif

static void body_launch_spheres(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct task_work *work = task->work;
    struct finalhazard_body *body = record->own;
    Sint8 density;
    body_breathe(task);
    switch (work->submode) {
    case 0:
        switch (work->mode) {
        case FINALHAZARD_SPHERES_FEW:
            density = WAVE_FEW;
            break;
        case FINALHAZARD_SPHERES_NORMAL:
            density = WAVE_NORMAL;
            break;
        case FINALHAZARD_SPHERES_MANY:
            density = WAVE_MANY;
            break;
#ifdef NONMATCHING
        /* this side launches no wave for a mode the switch doesn't name. for
         * such a mode, the matching side leaves density unset */
        default:
            return;
#endif
        }
        body->wave = finalhazard_sphere_wave(task, NULL, 0, density);
        work->timer = START_STEPS;
        work->submode++;
        /* fall through */
    case 1:
        if (--work->timer == 0) {
            finalhazard_sphere_volley(body->wave);
            work->submode++;
        }
        break;
    case 2:
        if (ATTACK_FINISHED(finalhazard_sphere_finished, body->wave)) {
            END_ATTACK(body->wave);
            body_set_mode(task, body_next_attack(task));
        }
        break;
    }
}

/* set attack to count_ lasers of kind_ from the two cannons, at panel_ */
#define AIM_CANNONS(attack, kind_, count_, panel_)                             \
    do {                                                                       \
        (attack).kind = (kind_);                                               \
        (attack).count = (count_);                                             \
        (attack).first =                                                       \
            &finalhazard_joint_positions[FINALHAZARD_JOINT_CANNON_A];          \
        (attack).second =                                                      \
            &finalhazard_joint_positions[FINALHAZARD_JOINT_CANNON_B];          \
        (attack).target = (panel_);                                            \
    } while (0)

static void body_fire_cannons(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct task_work *work = task->work;
    struct finalhazard_body *body = record->own;
    struct finalhazard_attack attack;
    body_breathe(task);
    switch (work->submode) {
    case 0:
        switch (work->mode) {
        case FINALHAZARD_PULSE:
            AIM_CANNONS(attack, FINALHAZARD_LASER_PULSE, 1, body->panel);
            break;
        case FINALHAZARD_SWEEP:
            AIM_CANNONS(attack, FINALHAZARD_LASER_SWEEP, 1, body->panel);
            break;
        case FINALHAZARD_DOUBLE_SWEEP:
            AIM_CANNONS(attack, FINALHAZARD_LASER_SWEEP, 2, body->panel);
            break;
#ifdef NONMATCHING
        /* this side fires no lasers for a mode the switch doesn't name. for
         * such a mode, the matching side leaves attack unset */
        default:
            return;
#endif
        }
        body->lasers = finalhazard_fire_lasers(task, &attack);
        work->timer = START_STEPS;
        work->submode++;
        /* fall through */
    case 1:
        if (--work->timer == 0) {
            work->timer = LASER_STEPS;
            work->submode++;
        }
        break;
    case 2:
        if (--work->timer == 0) {
            finalhazard_lasers_retract(body->lasers);
            work->submode++;
        }
        break;
    case 3:
        if (ATTACK_FINISHED(finalhazard_lasers_finished, body->lasers)) {
            END_ATTACK(body->lasers);
            body_set_mode(task, body_next_attack(task));
        }
        break;
    }
}

static void body_combined_attack(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct task_work *work = task->work;
    struct finalhazard_body *body = record->own;
    struct finalhazard_attack attack;
    body_breathe(task);
    switch (work->submode) {
    case 0:
        AIM_CANNONS(attack, FINALHAZARD_LASER_SWEEP, 2, body->panel);
        body->lasers = finalhazard_fire_lasers(task, &attack);
        body->wave = finalhazard_sphere_wave(task, NULL, 0, WAVE_MANY);
        work->timer = START_STEPS;
        work->submode++;
        /* fall through */
    case 1:
        if (--work->timer == 0) {
            finalhazard_sphere_volley(body->wave);
            if (work->mode == FINALHAZARD_BOTH) {
                work->submode++;
            } else {
                finalhazard_sphere_repeat(body->wave);
                body->panel_timer = PANEL_MOVE_STEPS;
                work->submode = 4;
            }
        }
        break;
    case 2:
        if (ATTACK_FINISHED(finalhazard_sphere_finished, body->wave)) {
            END_ATTACK(body->wave);
            finalhazard_lasers_retract(body->lasers);
            work->submode++;
        }
        break;
    case 3:
        if (ATTACK_FINISHED(finalhazard_lasers_finished, body->lasers)) {
            END_ATTACK(body->lasers);
            body_set_mode(task, body_next_attack(task));
        }
        break;
    case 4:
        if (--body->panel_timer == 0) {
            finalhazard_panel_move(body->panel, finalhazard_players->active);
            body->panel_timer = PANEL_MOVE_STEPS;
        }
        break;
    }
}

static void body_defeat(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct finalhazard_body *body = record->own;
    struct motion_control *motion = record->block;
    Float frame;
    switch (work->submode) {
    case 0:
        END_ATTACKS(body);
        motion_set(motion, MOTION_DEFEAT);
        finalhazard_camera_motion(FINALHAZARD_CAMERA_DEFEAT, 0,
                                  body_motions[MOTION_DEFEAT].speed);
        finalhazard_panel_hide(body->panel);
        work->flags &= ~WORK_HURT;
        pad_disable_player(PAD_BOTH_PLAYERS);
        work->submode++;
        /* fall through */
    case 1:
        frame = motion->current.frame;
        if (frame < DEFEAT_LOOP_END &&
            game_state.steps % DEFEAT_LOOP_STEPS == 0) {
            sound_play(SOUND_DEFEAT_LOOP, NULL, 0, 0);
            /* @bug logic: sets the volume of SOUND_WEAK_POINT_HIT at
             * 0x8c50556c, not of SOUND_DEFEAT_LOOP, which plays at
             * 0x8c505564 */
            sound_set_volume(NULL, SOUND_WEAK_POINT_HIT, DEFEAT_LOOP_VOLUME);
        }
        if (WITHIN_FRAME(frame, DEFEAT_HIT_FRAME)) {
            sound_play_for(SOUND_HIT, task, 1, 0, HIT_DURATION);
            sound_set_volume(task, SOUND_HIT, SOUND_LOUDEST);
        }
        if (WITHIN_FRAME(frame, DEFEAT_HIGH_HIT_FRAME)) {
            void *key;
            /* @hack spelling: key assigned inside the first call, where
             * FINALHAZARD_SOUND_KEY(task, 1) in each would do */
            sound_play_for(SOUND_HIT, key = FINALHAZARD_SOUND_KEY(task, 1), 1,
                           0, HIT_DURATION);
            sound_set_volume(key, SOUND_HIT, SOUND_LOUDEST);
            sound_set_pitch(FINALHAZARD_SOUND_KEY(task, 1), SOUND_HIT,
                            HIGH_HIT_PITCH);
        }
        if (WITHIN_FRAME(frame, DEFEAT_SOUND_FRAME)) {
            sound_play_for(SOUND_DEFEAT, task, 1, 0, DEFEAT_DURATION);
            sound_set_volume(task, SOUND_DEFEAT, SOUND_LOUDEST);
        }
        if (game_state.steps % SPARK_STEPS == 0 &&
            motion->current.frame < BURN_END) {
            finalhazard_body_sparks(task);
        }
        if (game_state.steps % EXPLOSION_STEPS == 0 &&
            motion->current.frame < BURN_END) {
            EXPLOSION_RUMBLE();
            finalhazard_body_explosion();
        }
        if (motion_ended(motion)) {
            camera_remove_mode(0, camera_top(0));
            stage_clear(finalhazard_players->active);
            work->submode++;
        }
        break;
    case 2:
        break;
    }
    finalhazard_players->ring_timer = 0;
}

static void finalhazard_step(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct finalhazard_body *body = record->own;
    body->mode = work->mode;
    if (game_state.time_stopped) {
        return;
    }
    if (game_state.mini_event_running) {
        motion_step(record->block);
        return;
    }
    /* a switch of players ends the attacks, and the next turn begins with
     * FINALHAZARD_EXPOSE */
    if (finalhazard_players->transition) {
        if (body->last_player != finalhazard_players->active) {
            END_ATTACKS(body);
        }
        motion_step(record->block);
        return;
    }
    if (body->last_player != finalhazard_players->active) {
        body_unstun();
        finalhazard_camera_follow(finalhazard_players->active);
        body->attack_index = BEFORE_FIRST;
        body_set_mode(task, FINALHAZARD_EXPOSE);
    }
    if (body->stunned == 1) {
        if (--body->stun_timer == 0) {
            body_unstun();
        }
    } else {
        switch (work->mode) {
        case FINALHAZARD_INTRO:
            body_intro(task);
            break;
        case FINALHAZARD_WAIT:
            body_wait(task);
            break;
        case FINALHAZARD_EXPOSE:
            body_expose(task);
            break;
        case FINALHAZARD_HIT:
            body_hit(task);
            break;
        case FINALHAZARD_SPHERES_FEW:
        case FINALHAZARD_SPHERES_NORMAL:
        case FINALHAZARD_SPHERES_MANY:
            body_launch_spheres(task);
            break;
        case FINALHAZARD_PULSE:
        case FINALHAZARD_SWEEP:
        case FINALHAZARD_DOUBLE_SWEEP:
            body_fire_cannons(task);
            break;
        case FINALHAZARD_BOTH:
        case FINALHAZARD_BOTH_REPEAT:
            body_combined_attack(task);
            break;
        case FINALHAZARD_DEFEAT:
            body_defeat(task);
            break;
        }
    }
    motion_step(record->block);
    /* the camera's target is the weak point mirrored */
    if (body->panel) {
        finalhazard_camera_mirror(&body->panel->work->pos, &record->focus,
                                  finalhazard_players->active);
        if (work->flags & BODY_BLEND_CAMERA) {
            finalhazard_camera_blend();
            work->flags &= ~BODY_BLEND_CAMERA;
        }
    }
    body_contact(task);
    body_register(task);
    body->last_player = finalhazard_players->active;
}

#define BODY_DRAW_FLAGS (NJD_CONTROL_3D_SHADOW | NJD_CONTROL_3D_TRANS_MODIFIER)

void finalhazard_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;

    draw_control_set(BODY_DRAW_FLAGS);
    njPushMatrixEx();
    finalhazard_bind_joints();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njRotateY(NULL, QUARTER_TURN);
    njScaleEx(&work->scl);
    njSetTexture(&finalhazard_model_texlist);
    light_apply(FINALHAZARD_LIGHT);
    motion_draw_simple(&finalhazard_root, record->block);
    light_apply(light_after_draw);
    njCnkSetMotionCallback(NULL);
    njPopMatrixEx();
    draw_control_clear(BODY_DRAW_FLAGS);
    /* @hack redundant: an if (0) around a test of the task and its record,
     * which nothing needs */
    if (0) {
        if (task && record) {
        }
    }
}
