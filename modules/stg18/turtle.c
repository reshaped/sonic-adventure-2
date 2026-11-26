/* the turtle, KAME in Dry Lagoon's object list. the swimmer swims a path, and
 * a swimming Rouge can ride it. the ferry carries player 0 between two
 * stops */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camset.h"
#include "camera/stack.h"
#include "camera/zones.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/fade.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stg18/lagoon_util.h"
#include "stg18/spray.h"
#include "stg18/turtle.h"
#include "stg18/turtle_art.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/matrix.h"
#include "system/pad.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"
#include "vmu/vibration.h"

/* player 0's pos less its record's unk_1e8, which MODE_SWIM writes while
 * player 0 rides the swimmer. nothing reads it */
static NJS_VECTOR riding_player_offset;

/* the work's mode, which turtle_take_mode keeps as the placement's state. no
 * code takes MODE_NONE or MODE_NONE_2 */
enum {
    MODE_NONE,
    MODE_SWIM,
    MODE_NONE_2,
    MODE_WAIT,
    MODE_ARRIVE,
    MODE_SOUTH_STOP,
    MODE_TO_NORTH,
    MODE_NORTH_STOP,
    MODE_TO_SOUTH
};
/* the record's hop: how far a stop has brought player 0 onto its back */
enum {
    HOP_NONE, /* waiting for player 0 to stand on the turtle */
    HOP_UP, /* hopping along an arc to the seat */
    HOP_SEATED, /* on the seat, its stick locked until it presses A */
    HOP_FREE /* free to move */
};

/* the work's submode holds the mode the turtle takes at its next step, or
 * NO_NEXT_MODE */
#define NEXT_MODE(work) ((work)->submode)
#define NO_NEXT_MODE (-1)
/* the bit of the work's ang.z that makes the turtle the ferry */
#define FERRY 0x1
/* indexes of stop_pos and stop_ang. the south stop lies toward -z */
#define SOUTH 0
#define NORTH 1
/* the id of the swimmer's shape. rouge_offer_actions offers a swimming Rouge
 * PLAYER_MODE_ROUGE_RIDE_SWIMMER where she touches a shape of the id */
#define SWIM_SHAPE_ID 5
/* the ferry waits at the start while this enemy group holds an enemy */
#define ENEMY_GROUP 0xff

/* the turtle's record, in its task's extra. nothing reads home, home_ang,
 * last_pos or last_work_pos */
struct turtle {
    Sint32 hop; /* a HOP_ value */
    /* a hop's steps: up to its top, in all, and so far */
    Sint32 rise;
    Sint32 limit;
    Sint32 count;
    /* at a hop's step n, player 0 stands at start + spd * n, less
     * g * n * n / 2 in y. g is its record's consts.gravity */
    NJS_VECTOR spd;
    NJS_POINT3 start;
    Angle ang; /* player 0's heading through the hop */
    /* TRUE from a ride's end until the stop it ends at takes HOP_FREE, on its
     * first step */
    Bool arrived;
    /* the work's pos and angles as turtle_alloc found them */
    NJS_POINT3 home;
    struct angles home_ang;
    NJS_POINT3 pos; /* where the root node stands in the world */
    NJS_POINT3 last_pos; /* pos at the step before */
    NJS_POINT3 last_work_pos; /* the work's pos before MODE_SWIM's last move */
    /* MODE_SWIM's place on turtle_path, as patrol_path_move keeps it */
    Sint32 point;
    Float along;
    Sint32 roll_speed; /* what MODE_SWIM adds to ang.z each step */
    /* player 0's pos less the record's pos, as a ride starts */
    NJS_VECTOR offset;
    Uint8 unk_84[0x0c];
    /* a ride's steps so far. the end of a ride leaves it at 1, so each ride
     * after the first starts there */
    Sint32 timer;
    Sint32 frame; /* the frame motion draws at */
    Sint32 blink; /* the eyes' frame of a blink, or 0 between blinks */
    NJS_MATRIX *work_matrix; /* the work's pos and angles */
    /* work_matrix, then the motion's move and turn of the root node */
    NJS_MATRIX *root;
    NJS_MATRIX *last_root; /* root at the step before */
    NJS_MOTION *motion;
    Uint8 unk_ac[0x44];
    NJS_OBJECT *ground; /* the turtle's object in the collision world */
    struct camera_zone *zone; /* the camera's sphere about the turtle */
    Bool started; /* TRUE once the turtle has stepped */
};

#define TURTLE(task) ((struct turtle *)(task)->extra.p)

#define TURTLE_WORLD_FLAGS                                                     \
    (WORLD_SIZE_CLASS(2) | WORLD_TURNED | WORLD_MOVING | GROUND_GRIP_MORE |    \
     WORLD_SOLID)

/* a blink's frames. between blinks, each step starts one by a chance of 1 in
 * BLINK_CHANCE */
#define BLINK_FRAMES 8
#define BLINK_CHANCE 240.0F

/* the turtle's object collides only while a player lies within its model's
 * radius plus GROUND_MARGIN. it doesn't while player 0 rides the swimmer. at
 * the south stop, it doesn't while player 0 still rises in its hop */
#define GROUND_MARGIN 10.0F

/* MODE_SWIM moves scl.x times its speed along the path each step. the speed
 * is RIDDEN_SPEED while player 0 rides in PLAYER_MODE_ROUGE_RIDE_SWIMMER. it is
 * NEAR_SPEED while player 0 lies within NEAR_RANGE, and else 1 */
#define NEAR_RANGE 75.0F
#define NEAR_SPEED 0.5F
#define RIDDEN_SPEED 2.0F
/* MODE_SWIM rolls into its turns. the roll it seeks is TURN_ROLL times the
 * step's turn about y. each step, the roll's speed gains ROLL_EASE of the roll
 * it lacks, and loses ROLL_DRAG of itself */
#define TURN_ROLL 16.0F
#define ROLL_EASE 0.003F
#define ROLL_DRAG 0.04F

/* MODE_WAIT plays WAIT_SOUND by a chance of WAIT_CHANCE every WAIT_PERIOD
 * steps of game_state.steps. every WAIT_MOTION_2_PERIOD steps, it plays the
 * sound for certain, and turtle_wait_motion_2 once */
#define WAIT_PERIOD 60
#define WAIT_CHANCE 0.1F
#define WAIT_MOTION_2_PERIOD 500
/* the frames of turtle_arrive_motion that play RIDE_SOUND and ARRIVE_SOUND,
 * and that make the splash. ARRIVE_SOUND plays at ARRIVE_SOUND_FRAME(0) to
 * ARRIVE_SOUND_FRAME(4) */
#define RIDE_SOUND_FRAME 45
#define ARRIVE_SOUND_FIRST 108
#define ARRIVE_SOUND_GAP 60
#define ARRIVE_SOUND_FRAME(n) (ARRIVE_SOUND_FIRST + (ARRIVE_SOUND_GAP * (n)))
#define SPLASH_FRAME 364
#define SPLASH_SCALE 1.0F

/* a ride ends once the record's timer reaches RIDE_STEPS. the turtle sinks
 * SINK_SPEED a step until the timer reaches DIVE_STEPS. the turtle then moves
 * to RISE_DEPTH below the other stop, and rises SINK_SPEED a step. so it
 * reaches the stop's height as the ride ends */
#define DIVE_STEPS 90
#define RIDE_STEPS 180
#define RISE_DEPTH ((RIDE_STEPS - DIVE_STEPS) * SINK_SPEED)
#define SINK_SPEED 0.25F
/* a ride puts camera 0 by player 0 at the other stop. at the north stop it
 * stands (0, NORTH_CAMERA_Y, NORTH_CAMERA_Z) from player 0. at the south
 * stop it stands (SOUTH_CAMERA_X, SOUTH_CAMERA_Y, 0) */
#define NORTH_CAMERA_Y (-10.0F)
#define NORTH_CAMERA_Z 50.0F
#define SOUTH_CAMERA_X 50.0F
#define SOUTH_CAMERA_Y 10.0F

/* the turtle's sounds in the stage's sound list */
#define WAIT_SOUND 0
#define RIDE_SOUND 1
#define ARRIVE_SOUND 2
#define SPLASH_SOUND 3
#define DIVE_SOUND 4
#define SWIM_SOUND 5

/* the animation player 0 plays through a hop */
#define HOP_ANIM 215
/* HOP_UP and HOP_SEATED set player 0's stick_lock to this each step */
#define STICK_LOCK_STEPS 2

/* the turtle draws only while player 0 lies within DRAW_RANGE of the work's
 * pos */
#define DRAW_RANGE 500.0F

static void turtle_destroy(struct task *task);
static void turtle_step(struct task *task);
static void turtle_draw(struct task *task);
static struct turtle *turtle_alloc(struct task *task);
static void turtle_free(struct task *task);
static void turtle_take_mode(struct task *task);
static void turtle_none(struct task *task);
static void turtle_swim(struct task *task);
static void turtle_none_2(struct task *task);
static Bool turtle_enemies_gone(struct task *task);
static void turtle_wait(struct task *task);
static void turtle_arrive(struct task *task);
static void turtle_south_stop(struct task *task);
static void turtle_to_north(struct task *task);
static void turtle_north_stop(struct task *task);
static void turtle_to_south(struct task *task);
static Bool patrol_path_move(struct path *path, Sint32 *point, Float *along,
                             Float speed, NJS_POINT3 *pos, struct angles *ang);
static void turtle_sphere_create(struct task *task);
static void turtle_sphere_step(struct task *task);
static void turtle_aim_hop(struct task *task, NJS_VECTOR *spd);

/* where a rider of the swimmer sits, in the root node's axes */
static const NJS_POINT3 swim_seat = {
#include "stg18/turtle/swim_seat.inc"
};
/* where MODE_ARRIVE's splash starts */
static const NJS_POINT3 splash_pos = {
#include "stg18/turtle/splash_pos.inc"
};

/* the swimmer's patrol, which MODE_SWIM swims toward lower indexes */
static struct path turtle_path = {
#include "stg18/turtle/turtle_path.inc"
};
/* the eyes' model for each frame of a blink */
KEPT static NJS_CNK_MODEL *turtle_eye_models[BLINK_FRAMES] = {
#include "stg18/turtle/turtle_eye_models.inc"
};
NJS_TEXLIST *turtle_texture_group[2] = {
#include "stg18/turtle/turtle_texture_group.inc"
};
/* clang-format off */
static struct collision_shape turtle_shape = {
    SWIM_SHAPE_ID, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
    SHAPE_GUARD_LEVEL(SHAPE_TOP_LEVEL), SHAPE_UNK_2000, {0.0F, 5.0F, 17.0F},
    25.0F, 0.0F, 0.0F, 0, {0, 0, 0}};
/* the ferry's sphere. turtle_sphere_step sets its center's y and z to sphere_y
 * and sphere_z, and its radius to sphere_radius */
static struct collision_shape sphere_shape = {
    0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_GUARD_LEVEL(SHAPE_TOP_LEVEL), SHAPE_UNK_2000, {0.0F, -3.5F, -21.0F},
    6.0F, 0.0F, 0.0F, 0, {0, 0, 0}};
/* clang-format on */
/* where the ferry's work stands at the start and at each stop, and its
 * angles there. turtle_wait_motion and turtle_arrive_motion move its model
 * from the start */
static NJS_POINT3 start_pos = {
#include "stg18/turtle/start_pos.inc"
};
static struct angles start_ang = {0, 0, 0};
static NJS_POINT3 stop_pos[2] = {
#include "stg18/turtle/stop_pos.inc"
};
static struct angles stop_ang[2] = {
    {0, -QUARTER_TURN, 0},
    {0, 0, 0},
};
/* a stop takes HOP_NONE again once player 0 lies farther than stay_radius
 * from the work's pos. the distance leaves out y */
static Float stay_radius = 12.0F;
/* the vibration of player 0's pad as a ride starts */
static Sint32 ride_vibration_power = 3;
static Sint32 ride_vibration_freq = 6;
static Sint32 ride_vibration_time = 0;
static Sint32 wait_volume = 95;
static Sint32 ride_volume = 125;
static Sint32 arrive_volume = 50;
static Sint32 splash_volume = 125;
static Sint32 dive_volume = 125;
static Sint32 swim_volume = 25;
static Sint32 dat_8c5bf6b0 = 120;
static void (*turtle_modes[])(struct task *task) = {
    turtle_none,     turtle_swim,       turtle_none_2,
    turtle_wait,     turtle_arrive,     turtle_south_stop,
    turtle_to_north, turtle_north_stop, turtle_to_south,
};
static Float zone_radius = 20.0F;
static Float draw_range_sq = DRAW_RANGE * DRAW_RANGE;
/* the frame of turtle_swim_motion that plays SWIM_SOUND */
static Sint32 swim_sound_frame = 15;
/* a hop's step turns player 0's heading 1 / (hop_heading_weight + 1) of the
 * way to the seat's. the seat faces the turtle's ang.y turned half round */
static Float hop_heading_weight = 8.0F;
/* the ride's step that plays DIVE_SOUND */
static Sint32 dive_sound_step = 30;
/* the center of the ferry's sphere, in the root node's axes, and its
 * radius */
static Float sphere_y = -3.5F;
static Float sphere_z = 21.0F;
static Float sphere_radius = 6.0F;
/* a hop's seat lies seat_height above the work's pos, and seat_ahead along
 * the z axis its ang.y turns. the hop rises hop_height above the seat */
static Float seat_height = 21.0F;
static Float hop_height = 2.5F;
static Float seat_ahead = -2.0F;

#pragma inline(turtle_set_next_mode)
static void turtle_set_next_mode(struct task *task, int mode)
{
    NEXT_MODE(task->work) = mode;
}

void turtle_init(struct task *task)
{
    struct task_work *work;
    /* @hack spelling: masked, a local for task->work read again, where work
     * would do */
    struct task_work *masked;
    NJS_OBJECT *ground;
    struct zone_shape shape;

    work = task->work;
    /* @hack spelling: the flag compared with FERRY, where the flag tested
     * alone would do */
    if ((work->ang.z & FERRY) == FERRY) {
        switch (placement_get_state(task)) {
        case MODE_ARRIVE:
        case MODE_SOUTH_STOP:
        case MODE_TO_SOUTH:
            work->pos = stop_pos[SOUTH];
            work->ang.x = stop_ang[SOUTH].x;
            work->ang.y = stop_ang[SOUTH].y;
            masked = task->work;
            masked->ang.z &= FERRY;
            break;
        case MODE_NORTH_STOP:
        case MODE_TO_NORTH:
            work->pos = stop_pos[NORTH];
            work->ang.x = stop_ang[NORTH].x;
            work->ang.y = stop_ang[NORTH].y;
            masked = task->work;
            masked->ang.z &= FERRY;
            break;
        case MODE_NONE:
        case MODE_WAIT:
        default:
            work->pos = start_pos;
            work->ang.x = start_ang.x;
            work->ang.y = start_ang.y;
            masked = task->work;
            masked->ang.z &= FERRY;
            if (placement_try_leave(task)) {
                task_destroy(task);
                return;
            }
            break;
        }
    }
    if (turtle_alloc(task) == NULL) {
        return;
    }
    task->draw = turtle_draw;
    task->step = turtle_step;
    task->destructor = turtle_destroy;
    ground = world_pool_alloc();
    TURTLE(task)->ground = ground;
    /* @bug memory: copies turtle_ground through null where world_pool_alloc
     * has none to lend, though turtle_destroy tests the record's ground for
     * null. turtle_step writes through it untested: the copy at 0x8c500ccc,
     * the test at 0x8c500ee2 and the first write at 0x8c501206 */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the turtle, and set.c makes it again on a later pass.
     * turtle_destroy tests the record's zone, which turtle_alloc leaves as the
     * heap left it, so the init clears it first */
    if (ground == NULL) {
        TURTLE(task)->zone = NULL;
        task_destroy(task);
        return;
    }
#endif
    *ground = turtle_ground[0];
    ground->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    ground->scl[0] = ground->scl[1] = ground->scl[2] = 1.0F;
    ground->ang[0] = task->work->ang.x;
    ground->ang[1] = task->work->ang.y;
    ground->ang[2] = 0;
    ground->pos[0] = work->pos.x;
    ground->pos[1] = work->pos.y;
    ground->pos[2] = work->pos.z;
    world_add(TURTLE_WORLD_FLAGS, task, ground);
    shape.shape = ZONE_SPHERE;
    shape.pos = task->work->pos;
    shape.ang.x = 0;
    shape.ang.y = 0;
    shape.ang.z = 0;
    shape.scl.x = zone_radius;
    shape.scl.z = zone_radius * zone_radius;
    TURTLE(task)->zone = camset_add_marked(&shape);
    if (!(task->work->ang.z & FERRY)) {
        TURTLE(task)->motion = &turtle_swim_motion;
        task->work->mode = MODE_SWIM;
        masked = task->work;
        masked->ang.z &= FERRY;
        collision_init(task, &turtle_shape, 1, COLLISION_OBJECT);
    } else {
        work->id = WORK_ID_NONE;
        switch (placement_get_state(task)) {
        case MODE_ARRIVE:
        case MODE_SOUTH_STOP:
        case MODE_TO_SOUTH:
            TURTLE(task)->motion = &turtle_stop_motion;
            task->work->mode = MODE_SOUTH_STOP;
            task->work->id = WORK_ID_TURTLE;
            break;
        case MODE_NORTH_STOP:
        case MODE_TO_NORTH:
            TURTLE(task)->motion = &turtle_stop_motion;
            task->work->mode = MODE_NORTH_STOP;
            task->work->id = WORK_ID_TURTLE;
            break;
        case MODE_NONE:
        case MODE_WAIT:
        default:
            TURTLE(task)->motion = &turtle_wait_motion;
            task->work->mode = MODE_WAIT;
            break;
        }
        turtle_sphere_create(task);
        TURTLE(task)->started = FALSE;
    }
    NEXT_MODE(task->work) = NO_NEXT_MODE;
    turtle_eyes->model = turtle_eye_models[0];
}

static void turtle_destroy(struct task *task)
{
    if (TURTLE(task)->zone != NULL) {
        camset_free_zone(TURTLE(task)->zone);
    }
    if (TURTLE(task)->ground != NULL) {
        world_remove(task, TURTLE(task)->ground);
        world_pool_free(TURTLE(task)->ground);
    }
    turtle_free(task);
}

static void turtle_step(struct task *task)
{
    struct task_work *work;
    NJS_OBJECT *ground;
    Float pos[3];
    Angle ang[3];
    struct angles turn;
    NJS_POINT3 player_pos;
    struct angles player_ang;

    work = task->work;
    /* @hack spelling: the mode read through task->work, where work would
     * do */
    if (task->work->mode == MODE_WAIT && placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the flag compared with FERRY, where the flag tested
     * alone would do */
    if ((task->work->ang.z & FERRY) == FERRY && !TURTLE(task)->started &&
        turtle_enemies_gone(task)) {
        task->work->pos = stop_pos[SOUTH];
        task->work->ang.x = stop_ang[SOUTH].x;
        task->work->ang.y = stop_ang[SOUTH].y;
        TURTLE(task)->frame = 0;
        TURTLE(task)->motion = &turtle_stop_motion;
        turtle_set_next_mode(task, MODE_SOUTH_STOP);
        task->work->id = WORK_ID_TURTLE;
    }
    TURTLE(task)->started = TRUE;
    turtle_take_mode(task);
    turtle_modes[task->work->mode](task);
    njSetCnkCurrentMotion(TURTLE(task)->motion, TURTLE(task)->frame);
    if (((NJS_MDATA2 *)TURTLE(task)->motion->mdata)->p[0] != NULL) {
        njGetCnkMotionTranslate(pos, 0);
    } else {
        pos[0] = turtle_object.pos[0];
        pos[1] = turtle_object.pos[1];
        pos[2] = turtle_object.pos[2];
    }
    if (((NJS_MDATA2 *)TURTLE(task)->motion->mdata)->p[1] != NULL) {
        njGetCnkMotionRotate(ang, 0);
    } else {
        ang[0] = turtle_object.ang[0];
        ang[1] = turtle_object.ang[1];
        ang[2] = turtle_object.ang[2];
    }
    TURTLE(task)->last_pos = TURTLE(task)->pos;
    njSetMatrix(TURTLE(task)->last_root, TURTLE(task)->root);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njRotateZ(NULL, work->ang.z);
    matrix_get(TURTLE(task)->work_matrix);
    njTranslate(NULL, pos[0], pos[1], pos[2]);
    njRotateEx(ang, 0);
    matrix_get(TURTLE(task)->root);
    njGetTranslation(NULL, &TURTLE(task)->pos);
    current_matrix_angles_xyz(&turn.x, &turn.y, &turn.z);
    njPopMatrixEx();
    /* @bug memory: writes the zone's pos through null where
     * camset_add_marked found its table full, at 0x8c501126. turtle_destroy
     * tests the record's zone for null at 0x8c500ece */
#ifdef NONMATCHING
    /* this side repairs the tag above: a turtle without a zone moves none */
    if (TURTLE(task)->zone != NULL) {
        TURTLE(task)->zone->pos = TURTLE(task)->pos;
    }
#else
    TURTLE(task)->zone->pos = TURTLE(task)->pos;
#endif
    ground = TURTLE(task)->ground;
    /* @hack spelling: work read again from task->work, which it holds
     * already */
    work = task->work;
    /* carry player 0's place by the root's move from last_root to root. task's
     * first force record takes how far that moves and turns player 0 */
    njPushMatrixEx();
    matrix_set(TURTLE(task)->last_root);
    njInvertMatrix(NULL);
    njTranslateEx(&player_work[0]->pos);
    njRotateZ(NULL, player_work[0]->ang.z);
    njRotateX(NULL, player_work[0]->ang.x);
    njRotateY(NULL, player_work[0]->ang.y);
    matrix_multiply_after_pairs(TURTLE(task)->root);
    njGetTranslation(NULL, &player_pos);
    current_matrix_angles_yxz(&player_ang.x, &player_ang.y, &player_ang.z);
    njPopMatrixEx();
    TASK_FORCES_OF(task)->speed.x = player_pos.x - player_work[0]->pos.x;
    TASK_FORCES_OF(task)->speed.y = player_pos.y - player_work[0]->pos.y;
    TASK_FORCES_OF(task)->speed.z = player_pos.z - player_work[0]->pos.z;
    TASK_FORCES_OF(task)->turn.x = player_ang.x - player_work[0]->ang.x;
    TASK_FORCES_OF(task)->turn.y = player_ang.y - player_work[0]->ang.y;
    TASK_FORCES_OF(task)->turn.z = player_ang.z - player_work[0]->ang.z;
    ground->pos[0] = TURTLE(task)->pos.x;
    ground->pos[1] = TURTLE(task)->pos.y;
    ground->pos[2] = TURTLE(task)->pos.z;
    ground->ang[0] = turn.x;
    ground->ang[1] = turn.y;
    ground->ang[2] = turn.z;
    if (player_work[0]->mode != PLAYER_MODE_ROUGE_RIDE_SWIMMER &&
        player_near((NJS_POINT3 *)ground->pos,
                    turtle_ground->model->r + GROUND_MARGIN) &&
        /* @hack spelling: the flag compared with FERRY, where the flag
         * tested alone would do */
        ((task->work->ang.z & FERRY) != FERRY ||
         task->work->mode != MODE_SOUTH_STOP || TURTLE(task)->hop != HOP_UP ||
         TURTLE(task)->count >= TURTLE(task)->rise)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    if (!(task->work->ang.z & FERRY)) {
        collision_register(task);
    }
    if (TURTLE(task)->blink) {
        if (++TURTLE(task)->blink >= BLINK_FRAMES) {
            TURTLE(task)->blink = 0;
        }
        turtle_eyes->model = turtle_eye_models[TURTLE(task)->blink];
    } else if ((int)(rand() * (BLINK_CHANCE * RAND_SCALE)) == 0) {
        TURTLE(task)->blink = 1;
        turtle_eyes->model = turtle_eye_models[TURTLE(task)->blink];
    }
}

static void turtle_draw(struct task *task)
{
    struct task_work *work;
    NJS_POINT3 pos;

    work = task->work;
    pos = player_work[0]->pos;
    if (((pos.x - work->pos.x) * (pos.x - work->pos.x)) +
            ((pos.y - work->pos.y) * (pos.y - work->pos.y)) +
            ((pos.z - work->pos.z) * (pos.z - work->pos.z)) >
        draw_range_sq) {
        return;
    }
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    matrix_multiply_before(TURTLE(task)->work_matrix);
    njSetTexture(&turtle_texlist);
    njCnkSimpleDrawMotion(&turtle_object, TURTLE(task)->motion,
                          TURTLE(task)->frame);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

/* give task its record and the record's three matrices from the Shinobi
 * heap, and return the record. where the heap can't give all four, it frees
 * what it took and returns null */
static struct turtle *turtle_alloc(struct task *task)
{
    struct task_work *work;
    NJS_MATRIX *matrix;
    NJS_MATRIX *root;
    NJS_MATRIX *last_root;

    work = task->work;
    /* @hack spelling: each assignment inside its test, where a statement of
     * its own would do */
    if ((matrix = syMalloc(sizeof(NJS_MATRIX))) == NULL) {
        return NULL;
    }
    if ((root = syMalloc(sizeof(NJS_MATRIX))) == NULL) {
        syFree(matrix);
        return NULL;
    }
    if ((last_root = syMalloc(sizeof(NJS_MATRIX))) == NULL) {
        syFree(matrix);
        syFree(root);
        return NULL;
    }
    task->extra.p = syMalloc(sizeof(struct turtle));
    if (task->extra.p == NULL) {
        syFree(matrix);
        syFree(root);
        syFree(last_root);
        return NULL;
    }
    TURTLE(task)->hop = HOP_NONE;
    TURTLE(task)->arrived = FALSE;
    TURTLE(task)->timer = 0;
    TURTLE(task)->frame = 0;
    TURTLE(task)->blink = 0;
    TURTLE(task)->home = task->work->pos;
    TURTLE(task)->home_ang = task->work->ang;
    TURTLE(task)->roll_speed = 0;
    TURTLE(task)->last_work_pos = task->work->pos;
    TURTLE(task)->pos = task->work->pos;
    TURTLE(task)->last_pos = task->work->pos;
    TURTLE(task)->point = 0;
    TURTLE(task)->along = 0.0F;
    TURTLE(task)->offset.x = 0.0F;
    TURTLE(task)->offset.y = 0.0F;
    TURTLE(task)->offset.z = 0.0F;
    TURTLE(task)->work_matrix = matrix;
    TURTLE(task)->root = root;
    TURTLE(task)->last_root = last_root;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njRotateZ(NULL, work->ang.z);
    matrix_get(TURTLE(task)->work_matrix);
    matrix_get(TURTLE(task)->root);
    njGetTranslation(NULL, &TURTLE(task)->pos);
    njPopMatrixEx();
    return task->extra.p;
}

/* free task's record and its matrices, and set task->extra to null. a task
 * without a record is left alone */
static void turtle_free(struct task *task)
{
    if (task->extra.p != NULL) {
        if (TURTLE(task)->work_matrix != NULL) {
            syFree(TURTLE(task)->work_matrix);
        }
        if (TURTLE(task)->root != NULL) {
            syFree(TURTLE(task)->root);
        }
        if (TURTLE(task)->last_root != NULL) {
            syFree(TURTLE(task)->last_root);
        }
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

/* take the next mode, if any, and keep it as the placement's state. the new
 * mode starts at HOP_NONE */
static void turtle_take_mode(struct task *task)
{
    if (NEXT_MODE(task->work) != NO_NEXT_MODE) {
        task->work->mode = NEXT_MODE(task->work);
        NEXT_MODE(task->work) = NO_NEXT_MODE;
        placement_set_state(task, task->work->mode);
        TURTLE(task)->hop = HOP_NONE;
    }
}

static void turtle_none(struct task *task)
{
}

/* MODE_SWIM: swim turtle_path, rolling into its turns. while player 0 rides in
 * PLAYER_MODE_ROUGE_RIDE_SWIMMER, hold it at swim_seat */
static void turtle_swim(struct task *task)
{
    struct task_work *work;
    Angle last_ang_y;
    int roll;
    Angle ang_z;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_POINT3 seat_point;
    NJS_POINT3 rider;

    work = task->work;
    last_ang_y = work->ang.y;
    if (player_work[0]->mode != PLAYER_MODE_ROUGE_RIDE_SWIMMER &&
        vector_distance(&work->pos, &player_work[0]->pos) < NEAR_RANGE) {
        patrol_path_move(&turtle_path, &TURTLE(task)->point,
                         &TURTLE(task)->along,
                         -(task->work->scl.x * NEAR_SPEED), &pos, &ang);
    } else if (player_work[0]->mode == PLAYER_MODE_ROUGE_RIDE_SWIMMER) {
        patrol_path_move(&turtle_path, &TURTLE(task)->point,
                         &TURTLE(task)->along,
                         -(task->work->scl.x * RIDDEN_SPEED), &pos, &ang);
    } else {
        patrol_path_move(&turtle_path, &TURTLE(task)->point,
                         &TURTLE(task)->along, -task->work->scl.x, &pos, &ang);
    }
    TURTLE(task)->last_work_pos = task->work->pos;
    task->work->ang.x = ang.x;
    task->work->ang.y = ang.y;
    roll = (int)((lagoon_angle_turn(last_ang_y, ang.y) * TURN_ROLL) -
                 (Sint16)task->work->ang.z);
    TURTLE(task)->roll_speed +=
        (roll * ROLL_EASE) - (TURTLE(task)->roll_speed * ROLL_DRAG);
    ang_z = task->work->ang.z + TURTLE(task)->roll_speed;
    task->work->ang.z = (task->work->ang.z & FERRY) | (ang_z & ~FERRY);
    task->work->pos.x = pos.x;
    task->work->pos.y = pos.y + task->work->scl.y;
    task->work->pos.z = pos.z;
    if (player_work[0]->mode == PLAYER_MODE_ROUGE_RIDE_SWIMMER) {
        seat_point = swim_seat;
        riding_player_offset.x =
            player_work[0]->pos.x - player_records[0]->unk_1e8.x;
        riding_player_offset.y =
            player_work[0]->pos.y - player_records[0]->unk_1e8.y;
        riding_player_offset.z =
            player_work[0]->pos.z - player_records[0]->unk_1e8.z;
        njCalcPoint(TURTLE(task)->root, &seat_point, &rider);
        player_set_pos(0, rider.x, rider.y, rider.z);
        player_set_angles(0, task->work->ang.x, task->work->ang.y ^ HALF_TURN,
                          task->work->ang.z);
        player_set_acc(0, 0.0F, 0.0F, 0.0F);
    }
    TURTLE(task)->frame++;
    if ((Uint32)TURTLE(task)->frame >= TURTLE(task)->motion->nbFrame) {
        TURTLE(task)->frame = 0;
    }
    if (TURTLE(task)->frame == swim_sound_frame) {
        sound_play_at(SWIM_SOUND, NULL, 0, swim_volume, &TURTLE(task)->pos);
    }
}

static void turtle_none_2(struct task *task)
{
}

/* whether ENEMY_GROUP holds no enemy. task isn't read */
static Bool turtle_enemies_gone(struct task *task)
{
    return enemy_group_empty(ENEMY_GROUP);
}

/* MODE_WAIT: wait at the start while ENEMY_GROUP holds an enemy, then take
 * MODE_ARRIVE */
static void turtle_wait(struct task *task)
{
    int gone;

    if (game_state.steps % WAIT_PERIOD == 0) {
        if ((Float)rand() * RAND_SCALE < WAIT_CHANCE) {
            sound_play_at(WAIT_SOUND, NULL, 0, wait_volume, &TURTLE(task)->pos);
        }
    }
    if (game_state.steps % WAIT_MOTION_2_PERIOD == 0) {
        sound_play_at(WAIT_SOUND, NULL, 0, wait_volume, &TURTLE(task)->pos);
        TURTLE(task)->motion = &turtle_wait_motion_2;
        TURTLE(task)->frame = 0;
    }
    gone = enemy_group_empty(ENEMY_GROUP);
    /* @hack spelling: the test's empty body, where the else's statements
     * under the test of gone would do */
    if (gone == 0) {
    } else {
        TURTLE(task)->motion = &turtle_arrive_motion;
        TURTLE(task)->frame = 0;
        turtle_set_next_mode(task, MODE_ARRIVE);
    }
    TURTLE(task)->frame++;
    if ((Uint32)TURTLE(task)->frame >= TURTLE(task)->motion->nbFrame) {
        TURTLE(task)->frame = 0;
        if (TURTLE(task)->motion == &turtle_wait_motion_2) {
            TURTLE(task)->motion = &turtle_wait_motion;
        }
    }
}

/* MODE_ARRIVE: play turtle_arrive_motion, with its sounds and its splash,
 * then take MODE_SOUTH_STOP */
static void turtle_arrive(struct task *task)
{
    NJS_POINT3 pos;

    if (TURTLE(task)->frame == RIDE_SOUND_FRAME) {
        sound_play(RIDE_SOUND, task, 0, 0);
        sound_set_volume(task, SOUND_ANY_ID, ride_volume);
    }
    if (TURTLE(task)->frame == ARRIVE_SOUND_FRAME(0) ||
        TURTLE(task)->frame == ARRIVE_SOUND_FRAME(1) ||
        TURTLE(task)->frame == ARRIVE_SOUND_FRAME(2) ||
        TURTLE(task)->frame == ARRIVE_SOUND_FRAME(3) ||
        TURTLE(task)->frame == ARRIVE_SOUND_FRAME(4)) {
        sound_play_at(ARRIVE_SOUND, NULL, 0, arrive_volume, &TURTLE(task)->pos);
    }
    if ((Uint32)++TURTLE(task)->frame >= TURTLE(task)->motion->nbFrame) {
        task->work->id = WORK_ID_TURTLE;
        task->work->pos = stop_pos[SOUTH];
        task->work->ang.x = stop_ang[SOUTH].x;
        task->work->ang.y = stop_ang[SOUTH].y;
        TURTLE(task)->motion = &turtle_stop_motion;
        TURTLE(task)->frame = 0;
        turtle_set_next_mode(task, MODE_SOUTH_STOP);
    }
    if (TURTLE(task)->frame == SPLASH_FRAME) {
        pos = splash_pos;
        spray_create(&pos, SPLASH_SCALE);
        sound_play_at(SPLASH_SOUND, NULL, 0, splash_volume, &TURTLE(task)->pos);
    }
}

/* MODE_SOUTH_STOP: hop player 0 to the seat once it stands on the turtle.
 * start the ride north once player 0 takes PLAYER_MODE_ROUGE_RIDE_FERRY */
static void turtle_south_stop(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    Float steps;
    int count;
    Float dx;
    Float dz;

    if (TURTLE(task)->arrived) {
        TURTLE(task)->hop = HOP_FREE;
        TURTLE(task)->arrived = FALSE;
    }
    task->work->pos = stop_pos[SOUTH];
    task->work->ang.x = stop_ang[SOUTH].x;
    task->work->ang.y = stop_ang[SOUTH].y;
    switch (TURTLE(task)->hop) {
    case HOP_NONE:
        if (player_work[0] != NULL &&
            (player_work[0]->flags & PLAYER_WORK_ON_GROUND) &&
            player_records[0] != NULL &&
            player_records[0]->ground_task != NULL &&
            player_records[0]->ground_task->work != NULL &&
            player_records[0]->ground_task->work->id == WORK_ID_TURTLE) {
            turtle_aim_hop(task, &TURTLE(task)->spd);
            TURTLE(task)->hop = HOP_UP;
        }
        TURTLE(task)->frame++;
        if ((Uint32)TURTLE(task)->frame >= TURTLE(task)->motion->nbFrame) {
            TURTLE(task)->frame = 0;
        }
        break;
    case HOP_UP:
        player_records[0]->stick_lock = STICK_LOCK_STEPS;
        /* @hack spelling: count, a local for the record's count, with steps
         * set inside the first sum, where steps set from the count first
         * would do */
        count = TURTLE(task)->count;
        pos.x = TURTLE(task)->start.x + (TURTLE(task)->spd.x * (steps = count));
        pos.y = TURTLE(task)->start.y + (TURTLE(task)->spd.y * steps);
        pos.z = TURTLE(task)->start.z + (TURTLE(task)->spd.z * steps);
        player_set_pos(
            0, pos.x,
            pos.y - (player_records[0]->consts.gravity * steps * steps * 0.5F),
            pos.z);
        player_request(0, PLAYER_REQUEST_ANIM, HOP_ANIM);
        TURTLE(task)->ang =
            angle_interpolate(work->ang.y ^ HALF_TURN, TURTLE(task)->ang,
                              hop_heading_weight, 1.0F);
        player_set_angles(0, 0, TURTLE(task)->ang, 0);
        if (++TURTLE(task)->count >= TURTLE(task)->limit) {
            TURTLE(task)->hop = HOP_SEATED;
            player_set_angles(0, 0, work->ang.y ^ HALF_TURN, 0);
            player_request(0, PLAYER_REQUEST_RELEASE, 0);
        }
        break;
    case HOP_SEATED:
    case HOP_FREE:
        if (TURTLE(task)->hop == HOP_SEATED) {
            player_records[0]->stick_lock = STICK_LOCK_STEPS;
            if (player_pads[0]->press & PDD_DGT_TA) {
                TURTLE(task)->hop = HOP_FREE;
            }
        }
        dx = player_work[0]->pos.x - work->pos.x;
        dz = player_work[0]->pos.z - work->pos.z;
        if ((dx * dx) + (dz * dz) > stay_radius * stay_radius) {
            TURTLE(task)->hop = HOP_NONE;
        }
        if (player_work[0]->mode == PLAYER_MODE_ROUGE_RIDE_FERRY) {
            TURTLE(task)->offset.x =
                player_work[0]->pos.x - TURTLE(task)->pos.x;
            TURTLE(task)->offset.y =
                player_work[0]->pos.y - TURTLE(task)->pos.y;
            TURTLE(task)->offset.z =
                player_work[0]->pos.z - TURTLE(task)->pos.z;
            sound_play(RIDE_SOUND, task, 0, 0);
            sound_set_volume(task, SOUND_ANY_ID, ride_volume);
            turtle_set_next_mode(task, MODE_TO_NORTH);
            vibration_start(0, ride_vibration_power, ride_vibration_freq,
                            ride_vibration_time);
            game_state.idle_dim = 0;
            game_state.pause_disabled = 1;
            fade_reset_idle();
        }
        TURTLE(task)->frame++;
        if ((Uint32)TURTLE(task)->frame >= TURTLE(task)->motion->nbFrame) {
            TURTLE(task)->frame = 0;
        }
        break;
    }
}

/* MODE_TO_NORTH: carry player 0 to the north stop, held at its offset from
 * the root. the turtle sinks as the screen fades out, and rises at the north
 * stop as it fades in. camera 0 stays by player 0 from the step the turtle
 * reaches the north stop */
static void turtle_to_north(struct task *task)
{
    NJS_POINT3 pos;
    /* @hack spelling: camera_z and camera_y, locals for the camera's offset,
     * where NORTH_CAMERA_Z and NORTH_CAMERA_Y in place would do */
    Float camera_z;
    Float camera_y;
    Float y;
    struct angles *arrival;
    struct game_state *state;

    player_set_pos(0, TURTLE(task)->pos.x + TURTLE(task)->offset.x,
                   TURTLE(task)->pos.y + TURTLE(task)->offset.y,
                   TURTLE(task)->pos.z + TURTLE(task)->offset.z);
    if (TURTLE(task)->timer == dive_sound_step) {
        sound_play(DIVE_SOUND, task, 0, 0);
        sound_set_volume(task, SOUND_ANY_ID, dive_volume);
    }
    if (TURTLE(task)->timer < DIVE_STEPS) {
        task->work->pos.y -= SINK_SPEED;
        fade_out_step();
    }
    camera_z = NORTH_CAMERA_Z;
    camera_y = NORTH_CAMERA_Y;
    if (TURTLE(task)->timer == DIVE_STEPS) {
        /* @hack spelling: y, a local for the height read before the store of
         * x, where the height in place would do */
        y = stop_pos[NORTH].y - RISE_DEPTH;
        task->work->pos.x = stop_pos[NORTH].x;
        task->work->pos.y = y;
        task->work->pos.z = stop_pos[NORTH].z;
        arrival = &stop_ang[NORTH];
        task->work->ang.x = arrival->x;
        task->work->ang.y = arrival->y;
        player_work[0]->ang.y += -arrival->y + stop_ang[SOUTH].y;
        player_motion[0]->aim_y = player_work[0]->ang.y;
        player_set_pos(0, task->work->pos.x + TURTLE(task)->offset.x,
                       task->work->pos.y + TURTLE(task)->offset.y,
                       task->work->pos.z + TURTLE(task)->offset.z);
        pos.x = task->work->pos.x + TURTLE(task)->offset.x;
        pos.y = task->work->pos.y + TURTLE(task)->offset.y + camera_y;
        pos.z = task->work->pos.z + TURTLE(task)->offset.z + camera_z;
        camera_put_at(0, &pos);
        lagoon_land_clip();
        fade_set_black();
    }
    if (TURTLE(task)->timer > DIVE_STEPS) {
        task->work->pos.y += SINK_SPEED;
        pos.x = TURTLE(task)->pos.x + TURTLE(task)->offset.x;
        pos.y = TURTLE(task)->pos.y + TURTLE(task)->offset.y + camera_y;
        pos.z = TURTLE(task)->pos.z + TURTLE(task)->offset.z + camera_z;
        camera_put_at(0, &pos);
        fade_in_step();
    }
    if (TURTLE(task)->timer == RIDE_STEPS) {
        TURTLE(task)->timer = 0;
        player_request(0, PLAYER_REQUEST_RELEASE, 0);
        turtle_set_next_mode(task, MODE_NORTH_STOP);
        TURTLE(task)->arrived = TRUE;
        /* @hack spelling: state, a local for game_state's address, where
         * game_state's fields in place would do */
        state = &game_state;
        state->idle_dim = 1;
        state->pause_disabled = 0;
    }
    TURTLE(task)->timer++;
    TURTLE(task)->frame++;
    if ((Uint32)TURTLE(task)->frame >= TURTLE(task)->motion->nbFrame) {
        TURTLE(task)->frame = 0;
    }
}

/* MODE_NORTH_STOP: as MODE_SOUTH_STOP, at the north stop, and start the ride
 * south */
static void turtle_north_stop(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    Float steps;
    int count;
    Float dx;
    Float dz;

    if (TURTLE(task)->arrived) {
        TURTLE(task)->hop = HOP_FREE;
        TURTLE(task)->arrived = FALSE;
    }
    task->work->pos = stop_pos[NORTH];
    task->work->ang.x = stop_ang[NORTH].x;
    task->work->ang.y = stop_ang[NORTH].y;
    switch (TURTLE(task)->hop) {
    case HOP_NONE:
        if (player_work[0] != NULL &&
            (player_work[0]->flags & PLAYER_WORK_ON_GROUND) &&
            player_records[0] != NULL &&
            player_records[0]->ground_task != NULL &&
            player_records[0]->ground_task->work != NULL &&
            player_records[0]->ground_task->work->id == WORK_ID_TURTLE) {
            turtle_aim_hop(task, &TURTLE(task)->spd);
            TURTLE(task)->hop = HOP_UP;
        }
        TURTLE(task)->frame++;
        if ((Uint32)TURTLE(task)->frame >= TURTLE(task)->motion->nbFrame) {
            TURTLE(task)->frame = 0;
        }
        break;
    case HOP_UP:
        player_records[0]->stick_lock = STICK_LOCK_STEPS;
        /* @hack spelling: count, a local for the record's count, with steps
         * set inside the first sum, where steps set from the count first
         * would do */
        count = TURTLE(task)->count;
        pos.x = TURTLE(task)->start.x + (TURTLE(task)->spd.x * (steps = count));
        pos.y = TURTLE(task)->start.y + (TURTLE(task)->spd.y * steps);
        pos.z = TURTLE(task)->start.z + (TURTLE(task)->spd.z * steps);
        player_set_pos(
            0, pos.x,
            pos.y - (player_records[0]->consts.gravity * steps * steps * 0.5F),
            pos.z);
        player_request(0, PLAYER_REQUEST_ANIM, HOP_ANIM);
        TURTLE(task)->ang =
            angle_interpolate(work->ang.y ^ HALF_TURN, TURTLE(task)->ang,
                              hop_heading_weight, 1.0F);
        player_set_angles(0, 0, TURTLE(task)->ang, 0);
        if (++TURTLE(task)->count >= TURTLE(task)->limit) {
            TURTLE(task)->hop = HOP_SEATED;
            player_set_angles(0, 0, work->ang.y ^ HALF_TURN, 0);
            player_request(0, PLAYER_REQUEST_RELEASE, 0);
        }
        break;
    case HOP_SEATED:
    case HOP_FREE:
        if (TURTLE(task)->hop == HOP_SEATED) {
            player_records[0]->stick_lock = STICK_LOCK_STEPS;
            if (player_pads[0]->press & PDD_DGT_TA) {
                TURTLE(task)->hop = HOP_FREE;
            }
        }
        dx = player_work[0]->pos.x - work->pos.x;
        dz = player_work[0]->pos.z - work->pos.z;
        if ((dx * dx) + (dz * dz) > stay_radius * stay_radius) {
            TURTLE(task)->hop = HOP_NONE;
        }
        if (player_work[0]->mode == PLAYER_MODE_ROUGE_RIDE_FERRY) {
            TURTLE(task)->offset.x =
                player_work[0]->pos.x - TURTLE(task)->pos.x;
            TURTLE(task)->offset.y =
                player_work[0]->pos.y - TURTLE(task)->pos.y;
            TURTLE(task)->offset.z =
                player_work[0]->pos.z - TURTLE(task)->pos.z;
            sound_play(RIDE_SOUND, task, 0, 0);
            sound_set_volume(task, SOUND_ANY_ID, ride_volume);
            turtle_set_next_mode(task, MODE_TO_SOUTH);
            vibration_start(0, ride_vibration_power, ride_vibration_freq,
                            ride_vibration_time);
            game_state.idle_dim = 0;
            game_state.pause_disabled = 1;
            fade_reset_idle();
        }
        TURTLE(task)->frame++;
        if ((Uint32)TURTLE(task)->frame >= TURTLE(task)->motion->nbFrame) {
            TURTLE(task)->frame = 0;
        }
        break;
    }
}

/* MODE_TO_SOUTH: carry player 0 to the south stop, as MODE_TO_NORTH carries
 * it north. camera 0 moves by player 0 only on the step the turtle reaches
 * the south stop */
static void turtle_to_south(struct task *task)
{
    Float step;
    NJS_POINT3 pos;
    Float y;

    /* @hack redundant: a read of game_state's address that nothing uses */
    (void)&game_state;
    player_set_pos(0, TURTLE(task)->pos.x + TURTLE(task)->offset.x,
                   TURTLE(task)->pos.y + TURTLE(task)->offset.y,
                   TURTLE(task)->pos.z + TURTLE(task)->offset.z);
    step = SINK_SPEED;
    if (TURTLE(task)->timer < DIVE_STEPS) {
        task->work->pos.y -= step;
        if (TURTLE(task)->timer == dive_sound_step) {
            sound_play(DIVE_SOUND, task, 0, 0);
            sound_set_volume(task, SOUND_ANY_ID, dive_volume);
        }
        fade_out_step();
    }
    if (TURTLE(task)->timer == DIVE_STEPS) {
        /* @hack spelling: y, a local for the height read before the store of
         * x, where the height in place would do */
        /* @hack spelling: the south stop read through stop_pos as a pointer,
         * where stop_pos[SOUTH] would do */
        y = stop_pos->y - RISE_DEPTH;
        task->work->pos.x = stop_pos->x;
        task->work->pos.y = y;
        task->work->pos.z = stop_pos->z;
        task->work->ang.x = stop_ang[SOUTH].x;
        task->work->ang.y = stop_ang[SOUTH].y;
        player_work[0]->ang.y += -stop_ang[SOUTH].y + stop_ang[NORTH].y;
        player_motion[0]->aim_y = player_work[0]->ang.y;
        player_set_pos(0, task->work->pos.x + TURTLE(task)->offset.x,
                       task->work->pos.y + TURTLE(task)->offset.y,
                       task->work->pos.z + TURTLE(task)->offset.z);
        pos.x = task->work->pos.x + TURTLE(task)->offset.x + SOUTH_CAMERA_X;
        pos.y = task->work->pos.y + TURTLE(task)->offset.y + SOUTH_CAMERA_Y;
        pos.z = task->work->pos.z + TURTLE(task)->offset.z;
        camera_put_at(0, &pos);
        lagoon_land_clip();
        fade_set_black();
    }
    if (TURTLE(task)->timer > DIVE_STEPS) {
        task->work->pos.y += step;
        fade_in_step();
    }
    if (TURTLE(task)->timer == RIDE_STEPS) {
        TURTLE(task)->timer = 0;
        player_request(0, PLAYER_REQUEST_RELEASE, 0);
        turtle_set_next_mode(task, MODE_SOUTH_STOP);
        TURTLE(task)->arrived = TRUE;
        game_state.idle_dim = 1;
        game_state.pause_disabled = 0;
    }
    TURTLE(task)->timer++;
    TURTLE(task)->frame++;
    if ((Uint32)TURTLE(task)->frame >= TURTLE(task)->motion->nbFrame) {
        TURTLE(task)->frame = 0;
    }
}

#include "shared/patrol_path.h"

/* give the ferry a child whose collision record is a sphere the root
 * carries */
static void turtle_sphere_create(struct task *task)
{
    struct task *child;

    child = task_create_child(TASK_WORK, turtle_sphere_step, task);
    collision_init(child, &sphere_shape, 1, COLLISION_OBJECT);
}

/* put the sphere where the turtle's root carries it. it collides only in
 * MODE_ARRIVE and at a stop */
static void turtle_sphere_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *turtle = task->parent;

    work->collision->shapes->center.y = sphere_y;
    work->collision->shapes->center.z = sphere_z;
    work->collision->shapes->radius = sphere_radius;
    njPushMatrixEx();
    matrix_set(TURTLE(turtle)->root);
    njGetTranslation(NULL, &work->pos);
    current_matrix_angles_yxz(&work->ang.x, &work->ang.y, &work->ang.z);
    njPopMatrixEx();
    switch (turtle->work->mode) {
    case MODE_ARRIVE:
    case MODE_SOUTH_STOP:
    case MODE_NORTH_STOP:
        collision_register(task);
    }
}

/* aim player 0's hop from where it stands to the seat. the aim is *spd and
 * the record's start, ang, rise, limit and count */
static void turtle_aim_hop(struct task *task, NJS_VECTOR *spd)
{
    struct task_work *work = task->work;
    NJS_VECTOR way;
    NJS_POINT3 from;
    NJS_POINT3 to;
    NJS_POINT3 target;
    Float ahead_z;
    Float ahead_x;
    Angle ang;
    Float top;
    Float height;
    Float rise;
    Float fall;
    Float steps;

    lagoon_polar_xz(work->ang.y, seat_ahead, &ahead_x, &ahead_z);
    target.x = work->pos.x + ahead_x;
    target.y = work->pos.y + seat_height;
    target.z = work->pos.z + ahead_z;
    TURTLE(task)->start = from = player_work[0]->pos;
    to = target;
    /* @hack redundant: top set to the higher of the hop's two ends, which
     * the sum after it replaces */
    top = from.y;
    if (top < to.y) {
        top = to.y;
    }
    way.x = to.x - from.x;
    way.y = 0.0F;
    way.z = to.z - from.z;
    TURTLE(task)->ang = ang = lagoon_heading(way.x, way.z);
    player_set_angles(0, 0, ang, 0);
    njScalor(&way);
    top = to.y + hop_height;
    height = top - from.y;
    TURTLE(task)->rise = rise =
        njSqrt(njAbs((height * 2.0F) / player_records[0]->consts.gravity));
    fall = njSqrt(
        njAbs(((top - to.y) * 2.0F) / player_records[0]->consts.gravity));
    /* @hack spelling: steps set in the store of the limit, where a statement
     * of its own would do */
    TURTLE(task)->limit = steps = rise + fall;
    TURTLE(task)->count = 0;
    spd->x = (to.x - from.x) / steps;
    spd->y = player_records[0]->consts.gravity * rise;
    spd->z = (to.z - from.z) / steps;
}
