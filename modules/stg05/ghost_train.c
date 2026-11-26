/* the ghost train, GSTRAIN in Pumpkin Hill's object list: an engine and the car
 * it pulls. the train runs along a path, or to and fro along a line. the engine
 * puffs smoke and throws sparks */
#include <mathf.h>
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "draw/lines.h"
#include "effect/particle.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/emerald.h"
#include "shared/spark_point.h"
#include "shared/white_breath.h"
#include "sound/sound.h"
#include "stage/hunt.h"
#include "stage/path.h"
#include "stg05/ghost_train.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

/* the set editor's labels for the train's placement */
static const char ghost_train_edit_type[12] = "<- TYPE:";
static const char ghost_train_edit_angle_y[12] = "<- ANGLE Y";
static const char ghost_train_edit_id[8] = "<- ID";
static const char ghost_train_edit_train_speed_5[20] = "<- TRAIN SPEED(+5)";
static const char ghost_train_edit_emerald_id[20] = "<- EMERALD ID[  ]";
static const char ghost_train_edit_path[8] = "PATH";
static const char ghost_train_edit_kiteki_pos_0_to_100[24] =
    "<- KITEKI POS(0 TO 100)";
static const char ghost_train_edit_linear[8] = "LINEAR";
static const char ghost_train_edit_scl9_1_f[16] = "<- SCL9+1.f)";
static const char ghost_train_edit_abs[4] = "ABS";

#include "stg05/ghost_train/text.inc"
#include "stg05/scenery_art/ghost_train.h"

/* the train's record, in task->motion */
struct ghost_train {
    /* the car's copy of ghost_train_collision in the collision world, at the
     * car's center and turned by car_heading */
    NJS_OBJECT object;
    Angle car_heading;
    /* the lean the car's turns give the train, which adds to both rolls, at
     * most MAX_LEAN either way */
    Angle lean;
    /* the engine's rear point, frac of the way along segment seg of the path or
     * the line */
    Sint32 seg;
    Float frac;
    /* the car's sways about z and about x, then the engine's */
    Angle car_roll;
    Angle car_pitch;
    Angle roll;
    Angle pitch;
    Float bob; /* how far ghost_train_bobbing rides above its place */
    Float speed; /* the distance the train runs a step, backward below 0 */
    /* the engine's heading, halfway between the headings at its front point and
     * at its rear point */
    Angle heading;
    /* the engine's place, halfway between its front point and its rear point */
    NJS_POINT3 pos;
    Float fade; /* from 0, unseen, to 1, opaque */
    Sint32 unk_70;
};

#define TRAIN(task) ((struct ghost_train *)(task)->motion.p)
/* the first of task's two force records, player 0's, which is the only one the
 * train fills */
#define FORCE(task) ((struct task_force *)(task)->force.p)

/* in ang.x: the low four bits pick a path, modulo TRAIN_PATHS, or with 0 the
 * line. under PATH_IN_WORLD the path or line lies in the world's axes, not
 * about the work's pos */
#define PATH_BITS 0xf
#define PATH_IN_WORLD 0x10
#define TRAIN_PATHS 5
/* path_progress counts the way along a path's segments in PROGRESS_SCALE
 * parts. a train on a path whistles as it passes scl.z of them */
#define PROGRESS_SCALE 100.0F
/* the train runs at scl.x + SPEED_BASE a step */
#define SPEED_BASE 2.0F
/* the speed gains SPEED_STEP a step toward scl.x + SPEED_BASE */
#define SPEED_STEP 0.1F
/* the engine's front point lies ENGINE_LENGTH ahead of its rear point. the car
 * heads from where it hangs toward a place behind the rear point. a move of
 * CAR_LOOK_BACK along the path or the line takes the rear point there */
#define ENGINE_LENGTH 50.0F
#define CAR_LOOK_BACK (-60.0F)
/* ghost_trains lists up to GHOST_TRAINS trains. ghost_train_find and
 * ghost_train_nearest never meet a train past them */
#define GHOST_TRAINS 10
/* the bits of ang.z the first step keeps as the train's id */
#define ID_BITS 0x1f
/* a train turns back at a line's end only after TURN_STEPS steps clear of both
 * ends. the work's timer counts them */
#define TURN_STEPS 2
/* the car takes its size class from its model's radius plus REACH_MARGIN */
#define REACH_MARGIN 5.0F
/* the car's copy collides while a player lies within its model's radius plus
 * NEAR_MARGIN of the car. the engine puffs, throws sparks and whistles only
 * while a player lies within EFFECT_RANGE of its place */
#define NEAR_MARGIN 32.0F
#define EFFECT_RANGE 400.0F
/* the steps a whole fade out takes, and a whole fade in */
#define FADE_OUT_STEPS 72.0F
#define FADE_IN_STEPS 180.0F
/* the train's sounds in pumpkin_hill_sounds. each step a sound is due plays it
 * anew, for SOUND_DURATION + 1 steps */
#define RUN_SOUND 0
#define RATTLE_SOUND 1
#define WHISTLE_SOUND 2
#define SOUND_PRIORITY 1
#define SOUND_VOLUME 10
#define SOUND_DURATION 10
/* a whistle lasts WHISTLE_STEPS of the steps the train runs with a player
 * within EFFECT_RANGE. at each, the engine puffs smoke at WHISTLE_PUFF times
 * its speed */
#define WHISTLE_STEPS 50
#define WHISTLE_PUFF 4.0F
/* a puff of smoke also takes SMOKE_DRAG of the car's move that step */
#define SMOKE_DRAG 0.7F
/* the smoke leaves from smoke_point, raised SMOKE_HEIGHT and by the bob */
#define SMOKE_HEIGHT 3.0F
/* the sparks fly from four points in the engine's axes. the first is (SPARK_X,
 * SPARK_Y, SPARK_Z). the others negate its x, its z or both. each point throws
 * a spark with a chance of SPARK_CHANCE a step. the sparks bounce off a floor
 * SPARK_FLOOR below the engine's place */
#define SPARK_X 8.0F
#define SPARK_Y 1.0F
#define SPARK_Z 35.0F
#define SPARK_CHANCE 0.3F
#define SPARK_FLOOR 5.0F
/* each step the lean keeps LEAN_KEEP of itself. it gains the car's turn,
 * negated, times the speed and LEAN_GAIN. |cos| of twice the lean scales the
 * gain down as the lean grows */
#define LEAN_KEEP 0.97F
#define LEAN_GAIN 0.5F
#define MAX_LEAN NJM_DEG_ANG(20)
/* in scl.y: 1 plus the id of the emerald piece the train carries, or 0 for
 * none */
#define PIECE_BITS 0xff
/* the piece rides PIECE_HEIGHT above the car's center. its scl.y of PIECE_SCALE
 * gives it a radius of PIECE_SCALE + 1 */
#define PIECE_HEIGHT 15.0F
#define PIECE_SCALE 10.0F
/* the train's children, by their work's tick: the engine's shapes, the car's
 * shapes and the emerald piece. the shapes collide while the train isn't
 * GHOST_TRAIN_GONE */
#define CHILD_ENGINE 0
#define CHILD_CAR 1
#define CHILD_PIECE 2
#define ENGINE_SHAPES 2
#define CAR_SHAPES 1
/* the train draws while a ball of DRAW_RADIUS about its place lies partly ahead
 * of the camera. the place must also lie no more than DRAW_REACH ahead */
#define DRAW_RADIUS 100.0F
#define DRAW_REACH 1000.0F
/* a fading train draws in a constant material, whose alpha is its fade. the
 * material's red stays full. its green and blue fall to FADED_GREEN_BLUE at a
 * fade of 0. njSetConstantAttr ANDs each strip chunk's header with
 * FADE_ATTR_AND, which keeps the strip's type and clears its flags. it then
 * ORs in FADE_ATTR_OR, which makes every strip blend by alpha */
#define FADE_ATTR_AND 0xff
#define FADE_ATTR_OR NJD_FST_UA
#define FADED_GREEN_BLUE 0.3F
/* the spinning node turns SPIN_RATE a step */
#define SPIN_RATE 0x200
/* the train's path, and its points, which are NJS_POINT3s on a path of flags 0,
 * PATH_LAYOUT_POS */
#define PATH(work)                                                             \
    (ghost_train_paths[((work)->ang.x & PATH_BITS) % TRAIN_PATHS])
#define PATH_POINTS(work) ((NJS_POINT3 *)PATH(work)->points)

/* a spark flies at spark_speed, with the train's speed added to z. the two
 * points of negated x negate its x too. the engine's axes then turn it.
 * spark_spread is spark_point_create's spread */
static Float spark_spread = 0.08F;
static NJS_VECTOR spark_speed = {0.5F, 1.0F, -1.0F};
/* the paths, by the low four bits of ang.x modulo TRAIN_PATHS. entries 0 and 2
 * hold the same path, and no index reaches the null */
KEPT static struct path *ghost_train_paths[TRAIN_PATHS + 1] = {
#include "stg05/ghost_train/ghost_train_paths.inc"
};
NJS_TEXLIST *ghost_train_texlists[] = {&ghost_train_texlist,
                                       &white_breath_texlist, NULL};
/* the engine's shapes are a cylinder that pushes and a box that hurts. the
 * car's is a box that hurts */
/* clang-format off */
static struct collision_shape engine_shapes[ENGINE_SHAPES] = {
    {0, SHAPE_CYLINDER2,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 30.0F, 0.0F}, 17.0F, 35.0F, 0.0F, 0,
     {QUARTER_TURN, 0, QUARTER_TURN}},
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(2) | SHAPE_GUARD_LEVEL(2) | SHAPE_HURTS_ALL, 0,
     {0.0F, 5.0F, 0.0F}, 12.0F, 5.0F, 34.0F, 0, {0, 0, 0}},
};
static struct collision_shape car_shapes[CAR_SHAPES] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(2) | SHAPE_GUARD_LEVEL(2) | SHAPE_HURTS_ALL, 0,
     {0.0F, 3.0F, 2.8F}, 8.0F, 5.0F, 12.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* a puff of smoke leaves from smoke_point at smoke_speed, both in the engine's
 * axes */
static NJS_POINT3 smoke_point = {-2.0F, 45.0F, 17.0F};
static NJS_VECTOR smoke_speed = {0.0F, 1.0F, 0.0F};
/* a puff comes each step with a chance of smoke_chance. its size is
 * smoke_size_least plus up to smoke_size_range */
static Float smoke_chance = 0.3F;
static Float smoke_size_least = 2.0F;
static Float smoke_size_range = 3.0F;
/* the listed trains, each in the entry first free as it started, and nulls */
static struct task *ghost_trains[GHOST_TRAINS] = {NULL};
static Uint32 smoke_color = 0xffc090ff;

static void train_unlist(struct task *task);
static struct task **train_list(struct task *task);
static Float path_progress(struct task_work *work, int seg, Float frac);
static Angle heading(NJS_POINT3 *from, NJS_POINT3 *to);
static int line_advance(Float length, int *seg, Float *frac, Float distance,
                        NJS_POINT3 *out, Angle *ang);
static int path_advance(struct task_work *work, int *seg, Float *frac,
                        Float distance, NJS_POINT3 *out, Angle *ang);
static void path_to_world(struct task_work *work, NJS_POINT3 *pos, Angle *ang);
static void ghost_train_destroy(struct task *task);
static void car_place(struct task *task, NJS_POINT3 *center, NJS_POINT3 *pivot);
static void ghost_train_move(struct task *task);
static void ghost_train_step(struct task *task);
static void ghost_train_draw(struct task *task);
static void engine_start(struct task *task);
static void engine_step(struct task *task);
static void car_start(struct task *task);
static void car_step(struct task *task);

/* nothing calls path_place or path_point, and no header declares them. they are
 * external because the image holds them, and SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* set *seg and *frac to the place progress hundredths of the way along, as
 * path_progress counts. *seg stops at the last segment */
void path_place(struct task_work *work, int *seg, Float *frac, Float progress);
/* set *out to the place frac of the way along segment seg of the path */
void path_point(struct task_work *work, int seg, NJS_POINT3 *out, Float frac);
/* NOLINTEND(misc-use-internal-linkage) */

Bool ghost_train_fade_in(struct task *task)
{
    struct task_work *work;

    work = task->work;
    work->mode = GHOST_TRAIN_FADING_IN;
    return TRUE;
}

Bool ghost_train_fade_out(struct task *task)
{
    struct task_work *work;

    work = task->work;
    work->mode = GHOST_TRAIN_FADING_OUT;
    return TRUE;
}

struct task *ghost_train_find(Sint8 id)
{
    int i;
    struct task_work *work;

    for (i = 0; i < GHOST_TRAINS; i++) {
        if (ghost_trains[i] != NULL) {
            work = ghost_trains[i]->work;
            if ((Uint8)work->ang.z == id) {
                return ghost_trains[i];
            }
        }
    }
    return NULL;
}

static void train_unlist(struct task *task)
{
    int i;

    for (i = 0; i < GHOST_TRAINS; i++) {
        if (ghost_trains[i] == task) {
            ghost_trains[i] = NULL;
        }
    }
}

/* list task in the first free entry of ghost_trains. returns the entry, or null
 * where none was free */
static struct task **train_list(struct task *task)
{
    int i;

    for (i = 0; i < GHOST_TRAINS; i++) {
        if (ghost_trains[i] == NULL) {
            ghost_trains[i] = task;
            return &ghost_trains[i];
        }
    }
    return NULL;
}

struct task *ghost_train_nearest(NJS_POINT3 *pos, NJS_POINT3 *engine)
{
    int i;
    struct task *task;
    struct task *nearest;
    Float least;
    Float distance;

    nearest = NULL;
    least = -1.0F;
    for (i = 0; i < GHOST_TRAINS; i++) {
        task = ghost_trains[i];
        if (task != NULL && task->work->mode != GHOST_TRAIN_GONE) {
            distance = njDistanceP2P(pos, &TRAIN(task)->pos);
            if (distance < least || least < 0.0F) {
                nearest = task;
                least = distance;
            }
        }
    }
    if (nearest != NULL) {
        /* @hack spelling: the record read through task, a copy of nearest,
         * where nearest would do */
        task = nearest;
        *engine = TRAIN(task)->pos;
    }
    return nearest;
}

/* the share of the train's path behind the place frac of the way along
 * segment seg. it counts segments, not length. returns the share times
 * PROGRESS_SCALE, from 0 to PROGRESS_SCALE */
static Float path_progress(struct task_work *work, int seg, Float frac)
{
    Float progress;

    progress = (seg + frac) / (PATH(work)->count - 1);
    if (progress >= 1.0F) {
        progress = 1.0F;
    }
    progress *= PROGRESS_SCALE;
    return progress;
}

void path_place(struct task_work *work, int *seg, Float *frac, Float progress)
{
    progress = progress * (PATH(work)->count - 1) * (1.0F / PROGRESS_SCALE);
    *seg = progress;
    if (*seg >= PATH(work)->count - 1) {
        *seg = PATH(work)->count - 2;
    }
    *frac = progress - *seg;
    if (*frac > 1.0F) {
        *frac = 1.0F;
    }
}

void path_point(struct task_work *work, int seg, NJS_POINT3 *out, Float frac)
{
    NJS_POINT3 *from;
    NJS_POINT3 *to;
    NJS_POINT3 *points;
    Float rest;

    points = PATH_POINTS(work);
    rest = 1.0F - frac;
    from = &points[seg];
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    to = from + 1;
#else
    seg++;
    /* @hack type: the next point's address held in seg, where
     * to = &points[seg + 1] would do */
    seg = (int)&points[seg];
    to = (NJS_POINT3 *)seg;
#endif
    out->x = (from->x * rest) + (to->x * frac);
    out->y = (from->y * rest) + (to->y * frac);
    out->z = (from->z * rest) + (to->z * frac);
}

/* the angle by which njRotateY turns z to point from from toward to */
static Angle heading(NJS_POINT3 *from, NJS_POINT3 *to)
{
    Float dx;
    Float dz;
    Angle ang;

    dx = from->x - to->x;
    dz = from->z - to->z;
    ang = (Angle)(atan2f(-dx, -dz) * ANGLE_PER_RADIAN);
    return ang;
}

/* move a place distance along a line on z, backward below 0. the line's
 * segments are length times GHOST_TRAIN_LINE_SCALE long, and segment 0 is
 * centered on 0. the place lies *frac of the way along segment *seg. *out
 * takes the place, and *ang a heading of 0. returns 1 where the place crossed
 * into another segment, else 0 */
static int line_advance(Float length, int *seg, Float *frac, Float distance,
                        NJS_POINT3 *out, Angle *ang)
{
    int crossed;
    /* @hack spelling: zero, a local for 0, where 0 in place would do */
    int zero;
    /* @hack redundant: one declared register, where a plain local would do */
    /* @hack spelling: one, a local for 1, where 1 in place would do */
    register int one;
    Float half;
    /* @hack spelling: fzero, a local for 0.0F, where 0.0F in place would
     * do */
    Float fzero;
    /* @hack spelling: start, a copy of distance, where distance would do */
    Float start;
    Float along;
    /* @hack spelling: place, a copy of distance, where distance would do */
    Float place;

    zero = 0;
    length *= GHOST_TRAIN_LINE_SCALE;
    crossed = zero;
    fzero = 0.0F;
    one = 1;
    half = length * 0.5F;
    if (distance > 0.0F) {
        along = *frac * length;
        start = distance;
        distance = start + along;
        while (distance > length) {
            distance -= length;
            crossed = one;
            ++*seg;
        }
    } else {
        along = *frac * length;
        start = distance;
        distance = start + along;
        while (0.0F > distance) {
            distance += length;
            --*seg;
            crossed = one;
        }
    }
    place = distance;
    *frac = place / length;
    *ang = zero;
    out->x = fzero;
    out->y = fzero;
    out->z = ((*seg * length) - half) + place;
    return crossed;
}

/* move a place distance along the train's path, as line_advance moves one along
 * a line. the path wraps from its last point to its first. *ang takes the
 * heading at the place: its segment's, turned frac of the way to the next
 * segment's. returns 1 where the place wrapped, else 0. on a path whose flags
 * aren't 0 it changes nothing, and returns 0 */
static int path_advance(struct task_work *work, int *seg, Float *frac,
                        Float distance, NJS_POINT3 *out, Angle *ang)
{
    NJS_POINT3 *points;
    NJS_POINT3 *from;
    NJS_POINT3 *to;
    int i;
    int next;
    int after;
    int wrapped;
    Float length;
    Float rest_frac;
    Float left;
    Float rest;
    Float start;
    Angle first;
    int turn;
    Float share;
    Float turned;
    Float behind;

    wrapped = 0;
    left = distance;
    if (PATH(work)->flags == 0) {
        points = PATH_POINTS(work);
        if (left > 0.0F) {
            i = *seg;
            start = *frac;
            for (;;) {
                i = i % PATH(work)->count;
                next = (i + 1) % PATH(work)->count;
                after = (i + 2) % PATH(work)->count;
                length = njDistanceP2P(&points[i], &points[next]);
                /* @hack spelling: rest written a statement a step, where
                 * rest = length - (length * start) would do */
                rest = length;
                rest *= start;
                rest = length - rest;
                if (0.0F > rest) {
                    rest = 0.0F;
                }
                if (left > rest) {
                    left -= rest;
                } else {
                    rest_frac = (rest - left) / length;
                    share = 1.0F - rest_frac;
                    if (0.0F > share) {
                        share = 0.0F;
                        rest_frac = 1.0F;
                    }
                    *seg = i;
                    *frac = share;
                    from = &points[i];
                    to = &points[next];
                    out->x = (from->x * rest_frac) + (to->x * share);
                    out->y = (from->y * rest_frac) + (to->y * share);
                    out->z = (from->z * rest_frac) + (to->z * share);
                    first = heading(from, to);
                    turn = heading(to, &points[after]);
                    turn -= first;
                    /* @hack spelling: turned written a statement a step,
                     * where turned = (Sint16)turn * share would do */
                    turned = (Sint16)turn;
                    turned *= share;
                    /* @hack spelling: the sum assigned to turn inside a
                     * comma, then to *ang, where *ang = the sum would do */
                    *ang = (turn = (Sint16)turned + (Sint16)first, turn);
                    return wrapped;
                }
                /* @hack spelling: the step written i = i + 1, where i++
                 * would do */
                i = i + 1;
                start = 0.0F;
                if (i >= PATH(work)->count) {
                    wrapped = 1;
                }
            }
        } else {
            i = *seg;
            start = *frac;
            for (;;) {
                i = i % PATH(work)->count;
                next = (i + 1) % PATH(work)->count;
                after = (i + 2) % PATH(work)->count;
                length = njDistanceP2P(&points[i], &points[next]);
                /* @hack spelling: rest written a statement a step, and the
                 * test's length - rest held in behind, where rest = length *
                 * start and a test of length - rest would do */
                rest = length;
                rest *= start;
                behind = length - rest;
                if (0.0F > behind) {
                    rest = length;
                }
                if (-left > rest) {
                    left += rest;
                } else {
                    rest += left;
                    share = rest / length;
                    rest_frac = 1.0F - share;
                    if (0.0F > rest_frac) {
                        share = 1.0F;
                        rest_frac = 0.0F;
                    }
                    *seg = i;
                    *frac = share;
                    from = &points[i];
                    to = &points[next];
                    out->x = (from->x * rest_frac) + (to->x * share);
                    out->y = (from->y * rest_frac) + (to->y * share);
                    out->z = (from->z * rest_frac) + (to->z * share);
                    first = heading(from, to);
                    turn = heading(to, &points[after]);
                    turn -= first;
                    /* @hack spelling: turned written a statement a step,
                     * where turned = (Sint16)turn * share would do */
                    turned = (Sint16)turn;
                    turned *= share;
                    /* @hack spelling: the sum assigned to turn inside a
                     * comma, then to *ang, where *ang = the sum would do */
                    *ang = (turn = (Sint16)turned + (Sint16)first, turn);
                    return wrapped;
                }
                /* @hack spelling: the step written i = i - 1, where i--
                 * would do */
                i = i - 1;
                start = 1.0F;
                if (i < 0) {
                    i = PATH(work)->count - 1;
                    wrapped = 1;
                }
            }
        }
    }
    return 0;
}

/* move pos and ang from the path's or the line's axes into the world's. pos
 * goes about the work's pos, turned by its ang.y, and ang turns by the same. a
 * null pos or ang is passed over. under PATH_IN_WORLD the two sets of axes are
 * one, and nothing changes */
static void path_to_world(struct task_work *work, NJS_POINT3 *pos, Angle *ang)
{
    if (!(work->ang.x & PATH_IN_WORLD)) {
        if (pos != NULL) {
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateV(NULL, &work->pos);
            njRotateY(NULL, work->ang.y);
            njCalcPoint(NULL, pos, pos);
            njPopMatrix(1);
        }
        if (ang != NULL) {
            *ang += work->ang.y;
        }
    }
}

void ghost_train_init(struct task *task)
{
    struct task_work *work;
    NJS_OBJECT *object;
    struct task *child;
    Angle front_ang;
    Angle rear_ang;
    Float front_frac;
    Float rear_frac;
    int front_seg;
    int rear_seg;
    NJS_POINT3 front;
    NJS_POINT3 rear;
    Float distance;
    Float ahead;
    Float reach;
    Uint32 flags;
    Angle unused; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    work = task->work;
    task->motion.p = syCalloc(1, sizeof(struct ghost_train));
    if (task->motion.p == NULL) {
        return;
    }
    TRAIN(task)->fade = 1.0F;
    task->draw = ghost_train_draw;
    task->step = ghost_train_step;
    task->destructor = ghost_train_destroy;
    work->mode = GHOST_TRAIN_RUNNING;
    work->timer = TURN_STEPS;
    if (work->submode) {
        work->mode = GHOST_TRAIN_FADING_IN;
        TRAIN(task)->fade = 0.0F;
    } else if ((Uint8)work->ang.z != 0) {
        work->mode = GHOST_TRAIN_GONE;
        TRAIN(task)->fade = 0.0F;
    }
    /* @hack redundant: a read of car_pitch that nothing uses */
    unused = TRAIN(task)->car_pitch;
    front_seg = rear_seg = TRAIN(task)->seg;
    front_frac = rear_frac = TRAIN(task)->frac;
    distance = TRAIN(task)->speed;
    if (work->mode != GHOST_TRAIN_RUNNING) {
        distance = 0.0F;
    }
    ahead = distance + ENGINE_LENGTH;
    if (work->ang.x & PATH_BITS) {
        path_advance(work, &front_seg, &front_frac, ahead, &front, &front_ang);
        path_advance(work, &rear_seg, &rear_frac, distance, &rear, &rear_ang);
    } else {
        line_advance(work->scl.z + 1.0F, &front_seg, &front_frac, ahead, &front,
                     &front_ang);
        line_advance(work->scl.z + 1.0F, &rear_seg, &rear_frac, distance, &rear,
                     &rear_ang);
    }
    path_to_world(work, &front, &front_ang);
    path_to_world(work, &rear, &rear_ang);
    TRAIN(task)->pos.x = (front.x + rear.x) * 0.5F;
    TRAIN(task)->pos.y = (front.y + rear.y) * 0.5F;
    TRAIN(task)->pos.z = (front.z + rear.z) * 0.5F;
    object = &TRAIN(task)->object;
    *object = ghost_train_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = 0;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID | WORLD_MOVING;
    reach = object->model->r + REACH_MARGIN;
    if (reach <= WORLD_SMALL_SIZE_CLASS_1_MAX) {
        if (reach > WORLD_SMALL_SIZE_CLASS_2_MAX) {
            flags = WORLD_SOLID | WORLD_MOVING | WORLD_SIZE_CLASS(1);
        } else if (reach > WORLD_SMALL_SIZE_CLASS_3_MAX) {
            flags = WORLD_SOLID | WORLD_MOVING | WORLD_SIZE_CLASS(2);
        } else {
            flags = WORLD_SOLID | WORLD_MOVING | WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    work->flags &= ~WORLD_ADDED;
    work->ang.z &= ID_BITS;
    train_list(task);
    child = task_create_child(TASK_WORK, engine_start, task);
    if (child != NULL) {
        child->work->pos = TRAIN(task)->pos;
        child->work->ang.x = 0;
        child->work->ang.z = 0;
        /* @bug logic: sets ang.x, which the line above cleared, where
         * ang.y was meant. the child keeps the train's ang.y until the
         * train's next step: 0x8c5011e8 clears ang.x, and 0x8c5011f6 stores
         * the heading there. the car's child does the same at 0x8c501220
         * and 0x8c50122e */
        child->work->ang.x = TRAIN(task)->heading;
        child->work->tick = CHILD_ENGINE;
    }
    child = task_create_child(TASK_WORK, car_start, task);
    if (child != NULL) {
        child->work->pos = TRAIN(task)->pos;
        child->work->ang.x = 0;
        child->work->ang.z = 0;
        child->work->ang.x = TRAIN(task)->heading;
        child->work->tick = CHILD_CAR;
    }
    if (((int)work->scl.y & PIECE_BITS) > 0) {
        /* @hack type: the piece's id taken 1U from the low byte of scl.y,
         * where 1 would do */
        child = emerald_create_plain_child(
            task, &TRAIN(task)->pos, EMERALD_KIND_A_PATHMOVE,
            ((int)work->scl.y & PIECE_BITS) - 1U);
        if (child != NULL) {
            child->work->tick = CHILD_PIECE;
        }
    }
}

static void ghost_train_destroy(struct task *task)
{
    struct task *child;

    /* @bug logic: the search for the piece's child does nothing with the
     * child it finds. the match at 0x8c501298 branches to the call of
     * task_destroy_children at 0x8c5012a8, as the search's end does */
    if (task->child != NULL) {
        child = task->child;
        while (child != NULL) {
            if (child->work->tick == CHILD_PIECE) {
                break;
            }
            if ((child = child->next) == task->child) {
                break;
            }
        }
    }
    task_destroy_children(task);
    if (TRAIN(task)->object.model != NULL) {
        world_remove(task, task->motion.p);
    }
    train_unlist(task);
    syFree(task->motion.p);
    task->motion.p = NULL;
}

/* set *pivot to the point the car hangs from, and *center to the car model's
 * center. both are world positions, and either may be null */
static void car_place(struct task *task, NJS_POINT3 *center, NJS_POINT3 *pivot)
{
    NJS_POINT3 origin;

    origin.x = 0.0F;
    origin.y = 0.0F;
    origin.z = 0.0F;
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslateV(NULL, &TRAIN(task)->pos);
    njRotateY(NULL, TRAIN(task)->heading);
    njTranslateV(NULL, (NJS_VECTOR *)ghost_train_car[0].pos);
    if (pivot != NULL) {
        njCalcPoint(NULL, &origin, pivot);
    }
    njRotateY(NULL, TRAIN(task)->car_heading - TRAIN(task)->heading);
    if (center != NULL) {
        njCalcPoint(NULL, &ghost_train_car[0].model->center, center);
    }
    njPopMatrix(1);
}

static void ghost_train_move(struct task *task)
{
    struct task_work *work;
    struct ghost_train *train;
    NJS_OBJECT *object;
    struct particle *puff;
    int in_range;
    Uint32 steps;
    Angle pitch;
    Angle turn;
    Angle old_heading;
    Sint16 lean;
    int rear_end;
    int front_end;
    Angle rear_ang;
    Angle front_ang;
    Angle back_ang;
    Float now;
    Float before;
    Float rear_frac;
    Float front_frac;
    Float back_frac;
    int rear_seg;
    int front_seg;
    int back_seg;
    Float distance;
    Float ahead;
    Float kept;
    Float added;
    Sint16 mean;
    Sint16 short_turn;
    /* @hack redundant: room, twelve bytes of the frame that nothing reads */
    Uint8 room[12];
    NJS_POINT3 rider;
    NJS_POINT3 carried;
    NJS_POINT3 moved;
    NJS_POINT3 pivot;
    NJS_POINT3 back;
    NJS_POINT3 front;
    NJS_POINT3 rear;
    NJS_POINT3 spark_2;
    NJS_POINT3 spark_3;
    NJS_POINT3 spark_at;
    NJS_VECTOR spark_spd_0;
    NJS_VECTOR spark_spd_1;
    NJS_VECTOR spark_spd_2;
    NJS_VECTOR spark_spd_3;
    NJS_POINT3 spark_spd;
    NJS_POINT3 spark_spd_x;

    work = task->work;
    switch (work->mode) {
    case GHOST_TRAIN_GONE:
        work->flags &= ~WORLD_ADDED;
        return;
    case GHOST_TRAIN_FADING_OUT:
        sound_play_at_for(RUN_SOUND, work, SOUND_PRIORITY, SOUND_VOLUME,
                          SOUND_DURATION, &work->pos);
        if (TRAIN(task)->fade > 0.0F) {
            TRAIN(task)->fade -= 1.0F / FADE_OUT_STEPS;
        }
        if (TRAIN(task)->fade < 0.0F) {
            TRAIN(task)->fade = 0.0F;
            work->mode = GHOST_TRAIN_GONE;
        }
        break;
    case GHOST_TRAIN_FADING_IN:
        sound_play_at_for(RUN_SOUND, work, SOUND_PRIORITY, SOUND_VOLUME,
                          SOUND_DURATION, &work->pos);
        if (TRAIN(task)->fade < 1.0F) {
            TRAIN(task)->fade += 1.0F / FADE_IN_STEPS;
            if (TRAIN(task)->fade > 1.0F) {
                TRAIN(task)->fade = 1.0F;
            }
        } else {
            work->mode = GHOST_TRAIN_RUNNING;
        }
        break;
    default:
        sound_play_at_for(RUN_SOUND, task, SOUND_PRIORITY, SOUND_VOLUME,
                          SOUND_DURATION, &work->pos);
        sound_play_at_for(RATTLE_SOUND, work, SOUND_PRIORITY, SOUND_VOLUME,
                          SOUND_DURATION, &work->pos);
        break;
    }
    /* @hack spelling: train assigned inside the assignment of object, where
     * a statement of its own would do */
    object = &(train = task->motion.p)->object;
    in_range = player_near(&train->pos, EFFECT_RANGE);
    back_seg = TRAIN(task)->seg;
    back_frac = TRAIN(task)->frac;
    if (work->ang.x & PATH_BITS) {
        path_advance(work, &back_seg, &back_frac, CAR_LOOK_BACK, &back,
                     &back_ang);
    } else {
        line_advance(work->scl.z + 1.0F, &back_seg, &back_frac, CAR_LOOK_BACK,
                     &back, &back_ang);
    }
    path_to_world(work, &back, &back_ang);
    old_heading = TRAIN(task)->car_heading;
    car_place(task, NULL, &moved);
    TRAIN(task)->car_heading =
        (Angle)(atan2f(-back.x + moved.x, -back.z + moved.z) *
                ANGLE_PER_RADIAN);
    pitch = TRAIN(task)->car_pitch;
    steps = game_state.steps;
    front_seg = rear_seg = TRAIN(task)->seg;
    front_frac = rear_frac = TRAIN(task)->frac;
    before = path_progress(work, TRAIN(task)->seg, TRAIN(task)->frac);
    distance = TRAIN(task)->speed;
    if (work->mode != GHOST_TRAIN_RUNNING) {
        distance = 0.0F;
    }
    ahead = distance + ENGINE_LENGTH;
    if (work->ang.x & PATH_BITS) {
        path_advance(work, &front_seg, &front_frac, ahead, &front, &front_ang);
        path_advance(work, &rear_seg, &rear_frac, distance, &rear, &rear_ang);
        front_end = rear_end = 0;
    } else {
        front_end = line_advance(work->scl.z + 1.0F, &front_seg, &front_frac,
                                 ahead, &front, &front_ang);
        rear_end = line_advance(work->scl.z + 1.0F, &rear_seg, &rear_frac,
                                distance, &rear, &rear_ang);
    }
    path_to_world(work, &front, &front_ang);
    path_to_world(work, &rear, &rear_ang);
    if (work->timer && !front_end && !rear_end) {
        work->timer--;
    }
    /* @hack redundant: a block no path reaches, which sets front_end to 60
     * and tests rear_end against it */
    if (0) {
        front_end = 60;
        if (rear_end > 60) {
        }
    }
    /* a train running forward turns back where its front point passes the
     * line's far end. one running back turns where its rear point passes the
     * near end. its speed drops to 0. scl.x changes so that scl.x +
     * SPEED_BASE, the speed it gains toward, is negated */
    if (work->scl.x + SPEED_BASE > 0.0F) {
        if (front_end && !work->timer) {
            TRAIN(task)->speed = 0.0F;
            work->scl.x = -work->scl.x - (2.0F * SPEED_BASE);
            work->timer = TURN_STEPS;
            if (work->submode == GHOST_TRAIN_FADES_AT_END) {
                work->mode = GHOST_TRAIN_FADING_OUT;
            }
        }
    } else {
        if (rear_end && !work->timer) {
            TRAIN(task)->speed = 0.0F;
            work->scl.x = -work->scl.x - (2.0F * SPEED_BASE);
            work->timer = TURN_STEPS;
            if (work->submode == GHOST_TRAIN_FADES_AT_END) {
                work->mode = GHOST_TRAIN_FADING_OUT;
            }
        }
    }
    TRAIN(task)->pos.x = (front.x + rear.x) * 0.5F;
    TRAIN(task)->pos.y = (front.y + rear.y) * 0.5F;
    TRAIN(task)->pos.z = (front.z + rear.z) * 0.5F;
    /* @hack spelling: mean written a statement a step, where
     * mean = (Sint16)(front_ang - rear_ang) / 2 + rear_ang would do */
    mean = front_ang - rear_ang;
    mean = mean / 2;
    mean += rear_ang;
    TRAIN(task)->heading = mean;
    TRAIN(task)->seg = rear_seg;
    TRAIN(task)->frac = rear_frac;
    now = path_progress(work, TRAIN(task)->seg, TRAIN(task)->frac);
    /* each sway, and the bob, is a sum of sines and cosines of
     * game_state.steps. in each term, the factor of steps is how far the
     * angle turns a step. the other factor is the term's size: degrees in a
     * sway, and a distance in the bob */
    TRAIN(task)->car_pitch = DEGREES(njSin(steps * 0x1300) * 0.6F);
    TRAIN(task)->car_roll = DEGREES((njSin(steps * 0x1e00) * 0.7F) +
                                    (njCos(steps * 0x2f10) * 0.6F) +
                                    (njSin(steps * 0x800) * 0.8F)) +
                            TRAIN(task)->lean;
    TRAIN(task)->pitch = DEGREES(njSin(steps * 0xb00) * 0.5F);
    TRAIN(task)->roll = DEGREES((njSin(steps * 0xf00) * 0.25F) +
                                (njCos(steps * 0x3410) * 0.2F) +
                                (njSin(steps * 0x400) * 0.5F)) +
                        TRAIN(task)->lean;
    TRAIN(task)->bob = njSin(steps * 0x1110) * 2.5F;
    /* @hack redundant: a block no path reaches, which takes the sine of pitch
     * twice */
    if (0) {
        njSin(pitch);
        njSin(pitch);
    }
    pitch = TRAIN(task)->car_pitch - pitch;
    if (task->force.p == NULL || TRAIN(task)->object.model == NULL) {
        return;
    }
    /* a train on a path starts its whistle as it passes scl.z hundredths of the
     * way along. it does so running either way. in a step that wraps past the
     * path's end, it tests only the part past the wrap */
    if ((work->ang.x & PATH_BITS) && !work->tick) {
        if (TRAIN(task)->speed > 0.0F) {
            if (now < before) {
                if (work->scl.z < now) {
                    work->tick = WHISTLE_STEPS;
                }
            } else if (before <= work->scl.z && work->scl.z < now) {
                work->tick = WHISTLE_STEPS;
            }
        } else {
            if (before < now) {
                if (work->scl.z > now) {
                    work->tick = WHISTLE_STEPS;
                }
            } else if (before >= work->scl.z && work->scl.z > now) {
                work->tick = WHISTLE_STEPS;
            }
        }
    }
    car_place(task, (NJS_POINT3 *)TRAIN(task)->object.pos, &pivot);
    TRAIN(task)->object.ang[1] = TRAIN(task)->car_heading;
    /* @hack spelling: the record read through train, assigned again, where
     * TRAIN(task) would do */
    train = task->motion.p;
    turn = train->car_heading - old_heading;
    lean = train->lean;
    /* @hack spelling: short_turn, a Sint16 local for turn, where (Sint16)turn
     * in place would do */
    short_turn = turn;
    kept = (Float)lean;
    added = (Float)short_turn;
    kept *= LEAN_KEEP;
    added *= -(train->speed * njAbs(njCos(lean * 2)) * LEAN_GAIN);
    /* @hack spelling: the lean assigned inside the test, where a statement
     * of its own would do */
    if ((lean = kept + added) > MAX_LEAN) {
        lean = MAX_LEAN;
    }
    if (lean < -MAX_LEAN) {
        lean = -MAX_LEAN;
    }
    TRAIN(task)->lean = lean;
    rider.x = player_work[0]->pos.x - pivot.x;
    rider.y = player_work[0]->pos.y - pivot.y;
    rider.z = player_work[0]->pos.z - pivot.z;
    moved.x = pivot.x - moved.x;
    moved.y = pivot.y - moved.y;
    moved.z = pivot.z - moved.z;
    if (in_range && !work->mode && work->tick) {
        sound_play_at_for(WHISTLE_SOUND, &work->pos, SOUND_PRIORITY,
                          SOUND_VOLUME, SOUND_DURATION, &work->pos);
    }
    /* @hack spelling: front and rear, free once the train's place is set,
     * hold the puff's place and speed, where locals of their own would do */
    if (in_range && work->mode == GHOST_TRAIN_RUNNING &&
        ((rand() * RAND_SCALE) < smoke_chance || work->tick)) {
        rear = smoke_speed;
        njPushMatrix(NULL);
        njUnitMatrix(NULL);
        njTranslateV(NULL, &TRAIN(task)->pos);
        njRotateY(NULL, TRAIN(task)->heading);
        njRotateX(NULL, TRAIN(task)->pitch);
        njRotateZ(NULL, TRAIN(task)->roll);
        njTranslate(NULL, 0.0F, TRAIN(task)->bob + SMOKE_HEIGHT, 0.0F);
        njCalcPoint(NULL, &smoke_point, &front);
        njCalcVector(NULL, &smoke_speed, &rear);
        njPopMatrix(1);
        if (work->tick != 0) {
            rear.x *= WHISTLE_PUFF;
            rear.y *= WHISTLE_PUFF;
            rear.z *= WHISTLE_PUFF;
            work->tick--;
        }
        rear.x += SMOKE_DRAG * moved.x;
        rear.y += SMOKE_DRAG * moved.y;
        rear.z += SMOKE_DRAG * moved.z;
        puff = white_breath_short_create(
            &front, &rear,
            smoke_size_least + ((Float)rand() * smoke_size_range * RAND_SCALE));
        if (puff != NULL) {
            puff->color = smoke_color;
        }
    }
    /* @hack spelling: front and rear hold the first two sparks' points,
     * where locals of their own would do */
    if (in_range != 0 && work->mode == GHOST_TRAIN_RUNNING) {
        spark_at.z = SPARK_Z;
        spark_at.y = SPARK_Y;
        spark_at.x = SPARK_X;
        spark_spd = spark_speed;
        spark_spd_x = spark_speed;
        spark_spd.z = TRAIN(task)->speed + spark_spd.z;
        spark_spd_x.z = spark_spd.z;
        spark_spd_x.x = -spark_spd_x.x;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, &TRAIN(task)->pos);
        njRotateY(NULL, TRAIN(task)->heading);
        njRotateX(NULL, TRAIN(task)->pitch);
        njRotateZ(NULL, TRAIN(task)->roll);
        njCalcPoint(NULL, &spark_at, &front);
        njCalcVector(NULL, &spark_spd, &spark_spd_0);
        njPushMatrixEx();
        njCalcVector(NULL, &spark_spd_x, &spark_spd_1);
        njScale(NULL, -1.0F, 1.0F, 1.0F);
        njCalcPoint(NULL, &spark_at, &rear);
        njPopMatrix(1);
        njPushMatrixEx();
        njCalcVector(NULL, &spark_spd, &spark_spd_2);
        njScale(NULL, 1.0F, 1.0F, -1.0F);
        njCalcPoint(NULL, &spark_at, &spark_2);
        njPopMatrix(1);
        njPushMatrixEx();
        njCalcVector(NULL, &spark_spd_x, &spark_spd_3);
        njScale(NULL, -1.0F, 1.0F, -1.0F);
        njCalcPoint(NULL, &spark_at, &spark_3);
        njPopMatrix(1);
        njPopMatrix(1);
        /* @hack type: game_state's two_players read as game_state_two_players,
         * where game_state.two_players would do */
        if (game_state_two_players == 0) {
            /* @hack redundant: a block no path reaches, which tests
             * spark_spread */
            if (0) {
                if (spark_spread) {
                }
            }
            if ((rand() * RAND_SCALE) < SPARK_CHANCE) {
                spark_point_create(&front, &spark_spd_0, spark_spread,
                                   TRAIN(task)->pos.y - SPARK_FLOOR);
            }
            /* @hack redundant: a block no path reaches, which tests each axis
             * of the first spark's speed against 0.3 and places the smoke's
             * point twice */
            if (0) {
                if (spark_spd_0.y < 0.3F) {
                    if (spark_spd_0.x < 0.3F) {
                        if (spark_spd_0.z < 0.3F) {
                            njPushMatrixEx();
                            njCalcPoint(NULL, &smoke_point, &front);
                            njCalcPoint(NULL, &smoke_point, &front);
                            njPopMatrix(1);
                        }
                    }
                }
            }
            if ((rand() * RAND_SCALE) < SPARK_CHANCE) {
                spark_point_create(&rear, &spark_spd_1, spark_spread,
                                   TRAIN(task)->pos.y - SPARK_FLOOR);
            }
            if ((rand() * RAND_SCALE) < SPARK_CHANCE) {
                spark_point_create(&spark_2, &spark_spd_2, spark_spread,
                                   TRAIN(task)->pos.y - SPARK_FLOOR);
            }
            if ((rand() * RAND_SCALE) < SPARK_CHANCE) {
                spark_point_create(&spark_3, &spark_spd_3, spark_spread,
                                   TRAIN(task)->pos.y - SPARK_FLOOR);
            }
        }
    }
    /* the force moves a rider with the car's pivot. it also swings the rider
     * about the pivot by the car's turn this step, about y. it swings the
     * rider about x by the change in the car's pitch */
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, turn);
    njRotateX(NULL, pitch);
    njCalcPoint(NULL, &rider, &carried);
    njPopMatrix(1);
    /* @bug logic: carries player 0 alone. the train never writes the second
     * of its two force records, by which every other player rides. it fills
     * the first, player 0's, at 0x8c501f34, 0x8c501f4c, 0x8c501f68 and
     * 0x8c501f84 */
    FORCE(task)->turn.y = -turn;
    FORCE(task)->speed.x = carried.x - rider.x + moved.x;
    FORCE(task)->speed.y = carried.y - rider.y + moved.y;
    FORCE(task)->speed.z = carried.z - rider.z + moved.z;
    if (player_near((NJS_POINT3 *)object->pos,
                    object->model->r + NEAR_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    if (work->scl.x + SPEED_BASE > 0.0F) {
        if (TRAIN(task)->speed < work->scl.x + SPEED_BASE) {
            TRAIN(task)->speed += SPEED_STEP;
        } else {
            TRAIN(task)->speed = work->scl.x + SPEED_BASE;
        }
    } else {
        if (TRAIN(task)->speed > work->scl.x + SPEED_BASE) {
            TRAIN(task)->speed -= SPEED_STEP;
        } else {
            TRAIN(task)->speed = work->scl.x + SPEED_BASE;
        }
    }
}

static void ghost_train_step(struct task *task)
{
    struct task_work *work;
    struct task *child;
    NJS_POINT3 *points;
    Sint32 i;
    int seg;
    Float frac;
    Angle ang[2];
    NJS_POINT3 pos[2];

    work = task->work;
    /* @hack redundant: a block no path reaches, which draws the train's two
     * points and its path as lines */
    if (0) {
        points = PATH_POINTS(work);
        if (work->ang.x & PATH_BITS) {
            path_advance(work, &seg, &frac, 0.0F, &pos[0], &ang[0]);
        } else {
            line_advance(work->scl.z + 1.0F, &seg, &frac, 0.0F, &pos[0],
                         &ang[0]);
        }
        if (work->ang.x & PATH_BITS) {
            path_advance(work, &seg, &frac, ENGINE_LENGTH, &pos[1], &ang[1]);
        } else {
            line_advance(work->scl.z + 1.0F, &seg, &frac, ENGINE_LENGTH,
                         &pos[1], &ang[1]);
        }
        if (!(work->ang.x & PATH_IN_WORLD)) {
            njPushMatrixEx();
            njTranslateV(NULL, &work->pos);
            njRotateY(NULL, work->ang.y);
        }
        line_draw_points(&work->pos, &pos[0]);
        line_draw_points(&pos[0], &pos[1]);
        if (work->ang.x & PATH_BITS) {
            line_draw_points(&work->pos, &pos[1]);
        }
        line_draw_points(&work->pos, &pos[1]);
        line_draw_points(&pos[1], &pos[0]);
        for (i = 0; i < PATH(work)->count - 1; i++) {
            line_draw_points(&points[i], &points[i + 1]);
        }
        if (work->ang.x & PATH_IN_WORLD) {
        } else {
            njPopMatrix(1);
        }
    } else {
        ghost_train_move(task);
        if (task->child == NULL) {
            return;
        }
        child = task->child;
        while (child != NULL) {
            switch (child->work->tick) {
            case CHILD_ENGINE:
                child->work->pos = TRAIN(task)->pos;
                child->work->ang.y = TRAIN(task)->heading;
                break;
            case CHILD_CAR:
                child->work->pos = *(NJS_POINT3 *)TRAIN(task)->object.pos;
                child->work->pos.y = TRAIN(task)->pos.y;
                child->work->ang.y = TRAIN(task)->object.ang[1];
                break;
            case CHILD_PIECE:
                child->work->pos = *(NJS_POINT3 *)TRAIN(task)->object.pos;
                child->work->pos.y += PIECE_HEIGHT;
                child->work->scl.y = PIECE_SCALE;
                emerald_set_shown(child,
                                  work->mode == GHOST_TRAIN_GONE ? 0 : 1);
            }
            if ((child = child->next) == task->child) {
                break;
            }
        }
    }
}

/* the car draws under DRAW_CONTROL_SHADOW, and the engine without it */
/* @hack spelling: ghost_train_draw_models expanded where ghost_train_draw
 * calls it, where a call would do */
#pragma inline(ghost_train_draw_models)
static void ghost_train_draw_models(struct task *task)
{
    struct ghost_train *train;

    njSetTexture(&ghost_train_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &TRAIN(task)->pos);
    njRotateY(NULL, TRAIN(task)->heading);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateV(NULL, (NJS_VECTOR *)ghost_train_car->pos);
    /* @hack spelling: train assigned inside a comma in the call, where a
     * statement of its own would do */
    njRotateY(NULL,
              (train = task->motion.p, train->car_heading - train->heading));
    njRotateX(NULL, TRAIN(task)->car_pitch);
    njRotateZ(NULL, TRAIN(task)->car_roll);
    njCnkEasyDrawModel(ghost_train_car->model);
    njPopMatrix(1);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njRotateX(NULL, TRAIN(task)->pitch);
    njRotateZ(NULL, TRAIN(task)->roll);
    njCnkEasyDrawModel(ghost_train_engine[0].model);
    njPushMatrixEx();
    njTranslateV(NULL, (NJS_VECTOR *)ghost_train_spin->pos);
    njRotateZ(NULL, game_state.steps * SPIN_RATE);
    njCnkEasyDrawModel(ghost_train_spin->model);
    njPopMatrix(1);
    njPushMatrixEx();
    njTranslateV(NULL, (NJS_VECTOR *)ghost_train_bobbing->pos);
    njTranslate(NULL, 0.0F, TRAIN(task)->bob, 0.0F);
    njCnkEasyDrawModel(ghost_train_bobbing->model);
    njPopMatrix(1);
    njPopMatrix(1);
}

static void ghost_train_draw(struct task *task)
{
    struct ghost_train *train;
    Float fade;

    if (!sphere_in_front_within(&TRAIN(task)->pos, DRAW_RADIUS, DRAW_REACH)) {
        return;
    }
    if (TRAIN(task)->fade == 1.0F) {
        ghost_train_draw_models(task);
    } else if (TRAIN(task)->fade > 0.0F) {
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(DRAW_CONTROL_CONSTANT);
        njSetConstantAttr(FADE_ATTR_AND, FADE_ATTR_OR);
        /* @hack spelling: train assigned inside the assignment, where a
         * statement of its own would do */
        fade = (train = task->motion.p)->fade;
        constant_material_set(
            fade, 1.0F,
            FADED_GREEN_BLUE + ((1.0F - FADED_GREEN_BLUE) * train->fade),
            FADED_GREEN_BLUE + ((1.0F - FADED_GREEN_BLUE) * train->fade));
        /* @hack redundant: a block no path reaches, a stand-in that gives
         * the draw the image's registers */
        if (0) {
            if (fade != 1.0F) {
                njPushMatrixEx();
                njRotateX(NULL, 0);
            }
        }
        ghost_train_draw_models(task);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    }
}

static void engine_start(struct task *task)
{
    /* @hack spelling: engine, a copy of task, where task would do */
    struct task *engine = task;

    collision_init(engine, engine_shapes, ENGINE_SHAPES, COLLISION_OBJECT);
    engine->step = engine_step;
}

static void engine_step(struct task *task)
{
    struct task_work *work = task->parent->work;

    if (work->mode != GHOST_TRAIN_GONE) {
        collision_register(task);
    }
}

static void car_start(struct task *task)
{
    /* @hack spelling: car, a copy of task, where task would do */
    struct task *car = task;

    collision_init(car, car_shapes, CAR_SHAPES, COLLISION_OBJECT);
    car->step = car_step;
}

static void car_step(struct task *task)
{
    struct task_work *work = task->parent->work;

    if (work->mode != GHOST_TRAIN_GONE) {
        collision_register(task);
    }
}
