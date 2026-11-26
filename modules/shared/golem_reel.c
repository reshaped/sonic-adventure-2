/* the golem fights' reel, UDREEL in their object lists: a handle on a wire.
 * it differs from shared/reel.c, the other stages' reel. it keeps the
 * handle's collision record in a child task, and never leaves by
 * placement_try_leave. its handle is a target only at the bottom. a grab at
 * the bottom turns player 0's pad off, and a held ride down turns it on. it
 * moves its held player only while game_state.cleared is 0 */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/golem_reel.h"
#include "shared/golem_reel/text.inc"
#ifdef NONMATCHING
/* reel.h's declarations, which this side's handle shares */
#include "shared/reel.h"
#endif
#include "shared/object_builds.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* a reel's modes, in its work's submode. in MODE_GRABBED, a handle grabbed at
 * the bottom dips lower before the ride up */
enum golem_reel_mode {
    MODE_DOWN,
    MODE_GRABBED,
    MODE_RAISING,
    MODE_UP,
    MODE_LOWERING
};

/* the bits of the work's mode. while REEL_HELD is set, a player hangs from
 * the handle, and the work's timer holds the player's slot */
#define REEL_HELD 0x1
/* the held player has stopped hanging. the next step throws the player */
#define REEL_LET_GO 0x2
/* the handle has reached the top or the bottom, and bounces there while
 * held */
#define REEL_AT_TOP 0x4
#define REEL_AT_BOTTOM 0x8

/* @hack type: the slot read as a Sint16, where the timer's Uint16 would do */
#define HELD_SLOT(work) ((Sint16)(work)->timer)

/* the wire paid out with the handle at the top, and at the bottom */
#define UP_LENGTH(work) ((work)->scl.x * GOLEM_REEL_LENGTH_UNIT)
#define DOWN_LENGTH(work) (((work)->scl.y + 1.0F) * GOLEM_REEL_LENGTH_UNIT)
/* the wire paid out never falls below LENGTH_MIN as the handle bounces at
 * the top */
#define LENGTH_MIN (-5.0F)

/* the wire drawn, WIRE_EXTRA longer than the wire paid out and never shorter
 * than WIRE_MIN */
#define WIRE_EXTRA 8.0F
#define WIRE_MIN 1.0F
#define WIRE_LENGTH(reel)                                                      \
    ((reel)->length > WIRE_MIN - WIRE_EXTRA ? (reel)->length + WIRE_EXTRA      \
                                            : WIRE_MIN)
/* the wire model's length, which the draw scales to WIRE_LENGTH */
#define WIRE_MODEL_LENGTH 20.0F
/* the handle hangs HANDLE_DROP below the wire's end. the child that holds
 * the handle's collision record lies SHAPE_DROP below that end */
#define HANDLE_DROP 3.5F
#define SHAPE_DROP 4.5F
/* the handle's y, relative to the reel's pos */
#define HANDLE_Y(reel) (-HANDLE_DROP - WIRE_LENGTH(reel))

/* a grab sets the length's speed to GRAB_SPEED. in MODE_GRABBED,
 * PULL_STIFFNESS is the stiffness of a spring that pulls the handle back up.
 * BOUNCE_STIFFNESS and BOUNCE_DAMPING make a damped spring of the bounce at
 * a ride's end */
#define GRAB_SPEED 1.0F
#define PULL_STIFFNESS 0.02F
#define BOUNCE_DAMPING 0.92F
#define BOUNCE_STIFFNESS 0.1F

/* the animation that PLAYER_REQUEST_HANG asks a grabbing player to play */
#define HANG_ANIM 75
/* how far below the handle a hanging player's pos lies, for a kind the
 * hang_drop_ values don't name */
#define HANG_DROP 16.5F
/* a held player's x and z move 1 - FOLLOW of the way to the handle each
 * step. each step also turns the player's ang.y FOLLOW of the way to
 * HALF_TURN minus the reel's ang.y. the turn takes the shorter way round.
 * the step neither moves nor turns the player while game_state.cleared isn't
 * 0 */
#define FOLLOW 0.35F

/* the handle can be grabbed only once it has gone unheld for more than
 * GRAB_DELAY steps. the work's tick counts those steps */
#define GRAB_DELAY 60

/* a player let go is thrown at a speed of the work's scl.z + THROW_SPEED.
 * the throw leans from straight up by the work's ang.z + THROW_TILT, about
 * the reel's own z axis */
#define THROW_SPEED 1.1F
#define THROW_TILT 0x3400

/* vibration_start's power and frequency for a grab and for a player let go */
#define GRAB_POWER 5
#define GRAB_FREQ 11
#define LET_GO_POWER 4
#define LET_GO_FREQ 15

/* the wire's sound, which plays at the handle while it travels between the
 * top and the bottom */
#define SOUND_WIRE 0x101d
#define SOUND_PRIORITY 1
#define SOUND_VOLUME 0
#define SOUND_DURATION 10

/* the size that picks the reel's WORLD_SIZE_CLASS is the collision model's
 * radius plus CLASS_MARGIN */
#define CLASS_MARGIN 5.0F
/* the step clears WORLD_ADDED while player_near finds no player within the
 * collision model's radius plus NEAR_MARGIN. world_collect_near_players then
 * passes over the reel's object */
#define NEAR_MARGIN 32.0F

/* the handle draws simply in the first SIMPLE_STEPS of each SIMPLE_PERIOD
 * of game_state.steps, and while held. a simple draw is njCnkSimpleDrawModel,
 * or the handle's build in golem_reel_draw_built. the handle draws by
 * njCnkEasyDrawModel otherwise */
#define SIMPLE_PERIOD 46
#define SIMPLE_STEPS 23
/* the shadow under the handle: shadow_object, scaled by SHADOW_SCALE_XZ
 * across and SHADOW_SCALE_Y up */
#define SHADOW_SCALE_XZ 3.6F
#define SHADOW_SCALE_Y 1.5F

/* the entries of golem_reel_builds */
enum golem_reel_build_entry {
    BUILD_TOP,
    BUILD_WIRE,
    BUILD_HANDLE,
    REEL_BUILDS
};
#define REEL_SHAPES 1

struct golem_reel {
    /* the copy of reel_collision that goes in the collision world,
     * placed and turned as the work is */
    NJS_OBJECT object;
    Float length; /* the wire paid out */
    Float speed; /* what length gains in a step */
    /* HANDLE_Y and the wire model's scale along y, which the step sets from
     * length for the draw */
    Float handle_y;
    Float wire_scale;
};

#define REEL(task) ((struct golem_reel *)(task)->motion.p)
/* the child that holds the handle's collision record, or null */
#define HANDLE(task) ((task)->extra.task)

#include "shared/golem_reel/reel.h"
#include "shared/golem_reel/reel_handle.h"
#include "shared/golem_reel/reel_wire.h"
#include "shared/golem_reel/reel_top.h"
#include "shared/golem_reel/reel_collision.h"

static void golem_reel_destroy(struct task *task);
static void golem_reel_step(struct task *task);
static void golem_reel_draw(struct task *task);
static void golem_reel_draw_built(struct task *task);

/* how far below the handle a hanging player's pos lies, by the player's
 * kind */
static Float hang_drop_sonic = 16.3F;
static Float hang_drop_shadow = 16.3F;
static Float hang_drop_knuckles = 17.7F;
static Float hang_drop_rouge = 15.4F;
static Float hang_drop_eggman_mech = 34.8F;
static Float hang_drop_tails_mech = 35.0F;
static Float hang_drop_amy = 16.2F;
static Float hang_drop_metalsonic = 16.8F;
static Float hang_drop_tical = 15.8F;
static Float hang_drop_chaos0 = 17.5F;
static Float hang_drop_bwalk = 34.9F;
static Float hang_drop_cwalk = 34.5F;

NJS_TEXLIST *golem_reel_texlists[2] = {
#include "shared/golem_reel/golem_reel_texlists.inc"
};
/* TRUE once one call of golem_reel_build has built all three models */
static Bool golem_reel_built = FALSE;
/* the builds of the top, the wire and the handle. golem_reel_draw_built
 * draws the top's and the handle's builds, but not the wire's */
static struct model_build golem_reel_builds[REEL_BUILDS] = {
#include "shared/golem_reel/golem_reel_builds.inc"
};
/* the handle's shape, in its child's collision record */
/* clang-format off */
static struct collision_shape golem_reel_shapes[REEL_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL), 0, SHAPE_UNTURNED,
     {0.0F, 0.0F, 0.0F}, 8.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* the most the length moves in a step of a ride, up or down */
static Float reel_speed_max = 2.5F;
/* the share of reel_speed_max that the speed gains toward the ride's end in
 * each step of a ride */
static Float reel_speed_gain = 0.1F;

Uint8 *golem_reel_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, golem_reel_builds, REEL_BUILDS) ==
        REEL_BUILDS) {
        golem_reel_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

#ifdef NONMATCHING
/* object_builds_run calls reel_build, which the golem fights' images place at
 * golem_reel_build's address. this side gives reel_build a body
 * that calls golem_reel_build */
Uint8 *reel_build(Uint8 *buf)
{
    return golem_reel_build(buf);
}
#endif

/* the handle's child does nothing of its own: the reel's step moves it */
static void golem_reel_handle_step(struct task *task)
{
}

#ifdef NONMATCHING
/* this side makes the reel's handle and its shape, and returns FALSE where
 * either can't be made */
static Bool create_handle(struct task *reel)
{
    struct task *handle;

    handle = task_create_child(TASK_WORK, golem_reel_handle_step, reel);
    if (handle == NULL) {
        return FALSE;
    }
    handle->work->pos = reel->work->pos;
    if (!collision_init(handle, golem_reel_shapes, REEL_SHAPES,
                        COLLISION_OBJECT)) {
        task_destroy(handle);
        return FALSE;
    }
    reel->extra.task = handle;
    return TRUE;
}
#endif

void golem_reel_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float hi;
    Float lo;
    Float radius;
    Uint32 flags;

    task->motion.p = syCalloc(1, sizeof(struct golem_reel));
    if (task->motion.p == NULL) {
        return;
    }
#ifdef NONMATCHING
    /* this side frees the reel's state and tries again the next step where its
     * handle can't be made */
    if (!create_handle(task)) {
        syFree(task->motion.p);
        task->motion.p = NULL;
        return;
    }
#endif
    /* @hack spelling: draw set before step, not in struct task's order */
    if (golem_reel_built) {
        task->draw = golem_reel_draw_built;
    } else {
        task->draw = golem_reel_draw;
    }
    task->step = golem_reel_step;
    task->destructor = golem_reel_destroy;
    lo = work->scl.x;
    hi = work->scl.y;
    /* @hack redundant: the else, which stores scl.x and scl.y back as they
     * were */
    if (hi < lo) {
        work->scl.x = hi;
        work->scl.y = lo;
    } else {
        work->scl.x = lo;
        work->scl.y = hi;
    }
    work->submode = MODE_DOWN;
    REEL(task)->length = DOWN_LENGTH(work);
    REEL(task)->speed = 0.0F;
    work->mode = 0;
#ifdef NONMATCHING
    /* create_handle made the handle and its collision record already */
#else
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task->extra.task = task_create_child(TASK_WORK, golem_reel_handle_step,
                                              task)) == NULL) {
        return;
    }
    /* @hack spelling: task->work read again, where work would do */
    HANDLE(task)->work->pos = task->work->pos;
    collision_init(HANDLE(task), golem_reel_shapes, REEL_SHAPES,
                   COLLISION_OBJECT);
#endif
    object = &REEL(task)->object;
    *object = reel_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    /* @hack spelling: the angles set y, z, x, not in the array's order */
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID;
    radius = object->model->r + CLASS_MARGIN;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    if (!(work->ang.x & GOLEM_REEL_RETURNS)) {
        REEL(task)->speed = 0.0F;
        REEL(task)->length = UP_LENGTH(work);
        work->submode = MODE_UP;
    }
}

static void golem_reel_destroy(struct task *task)
{
    if (HANDLE(task) != NULL) {
        task_kill(HANDLE(task));
        task->extra.task = NULL;
    }
    if (REEL(task)->object.model != NULL) {
        world_remove(task, &REEL(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void golem_reel_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *handle;
    struct collision_work *collision;
    /* @hack spelling: one vec for the throw's speed and the handle's place,
     * where the throw would take its own */
    NJS_VECTOR vec;
    /* @hack redundant: spare_0 and spare_1, which nothing reads */
    /* NOLINTBEGIN(clang-diagnostic-unused-variable) */
    NJS_VECTOR spare_0;
    NJS_VECTOR spare_1;
    /* NOLINTEND(clang-diagnostic-unused-variable) */
    Angle spare_2;
    Angle spare_3;
    Angle turn;
    Sint16 *half;
    Float part;
    Float offset;
    int player;

    if (game_state.cleared == 1) {
        work->mode &= ~(REEL_HELD | REEL_LET_GO);
    }
    if (work->mode & REEL_LET_GO) {
        work->mode &= ~(REEL_HELD | REEL_LET_GO);
        /* @hack spelling: vec.y set first, not in the members' order */
        vec.y = work->scl.z + THROW_SPEED;
        vec.x = 0.0F;
        vec.z = 0.0F;
        njPushMatrixEx();
        /* @hack type: the matrix passed as a bare 0, where NULL would do */
        njUnitMatrix(0);
        njRotateY(0, work->ang.y);
        njRotateZ(0, work->ang.z + THROW_TILT);
        njCalcVector(0, &vec, &vec);
        njPopMatrixEx();
        /* @hack spelling: the thrown player's ang.x and ang.z cleared in one
         * chained assignment, where two would do */
        player_work[HELD_SLOT(work)]->ang.x =
            player_work[HELD_SLOT(work)]->ang.z = 0;
        player_work[HELD_SLOT(work)]->ang.y = HALF_TURN - work->ang.y;
        player_set_speed(HELD_SLOT(work), vec.x, vec.y, vec.z);
    }
    REEL(task)->handle_y = HANDLE_Y(REEL(task));
    REEL(task)->wire_scale =
        WIRE_LENGTH(REEL(task)) * (1.0F / WIRE_MODEL_LENGTH);
    switch (work->submode) {
    case MODE_RAISING:
        if (HANDLE(task) != NULL) {
            HANDLE(task)->work->collision->flags &= ~COLLISION_TARGET;
        }
        if (work->mode & REEL_AT_TOP) {
            REEL(task)->length += REEL(task)->speed;
            offset = REEL(task)->length - UP_LENGTH(work);
            /* @hack redundant: a read of the length that nothing uses */
            (void)REEL(task)->length;
            if (REEL(task)->length < LENGTH_MIN) {
                REEL(task)->speed = njAbs(REEL(task)->speed);
                REEL(task)->length = LENGTH_MIN;
            }
            REEL(task)->speed = (REEL(task)->speed * BOUNCE_DAMPING) -
                                (offset * BOUNCE_STIFFNESS);
            if (!(work->mode & REEL_HELD)) {
                REEL(task)->speed = 0.0F;
                REEL(task)->length = UP_LENGTH(work);
                work->submode = (work->ang.x & GOLEM_REEL_RETURNS)
                                    ? MODE_LOWERING
                                    : MODE_UP;
                work->mode &= ~REEL_AT_TOP;
            }
        } else {
            /* @hack redundant: a test no path reaches, a read of
             * reel_speed_gain */
            if (0) {
                if (reel_speed_gain) {
                }
            }
            /* @hack spelling: the gain added as a negative, where -= would
             * do */
            REEL(task)->speed += -(reel_speed_max * reel_speed_gain);
            if (REEL(task)->speed < -reel_speed_max) {
                REEL(task)->speed = -reel_speed_max;
            }
            REEL(task)->length += REEL(task)->speed;
            if (REEL(task)->length < UP_LENGTH(work)) {
                work->mode |= REEL_AT_TOP;
            }
            vec = work->pos;
            vec.y += HANDLE_Y(REEL(task));
            sound_play_at_for(SOUND_WIRE, work, SOUND_PRIORITY, SOUND_VOLUME,
                              SOUND_DURATION, &vec);
        }
        /* @hack spelling: the two tests nested, where one test joined by &&
         * would do */
        if (work->mode & REEL_HELD) {
            if (player_work[HELD_SLOT(work)]->mode != PLAYER_MODE_HANG) {
                work->mode |= REEL_LET_GO;
                vibration_start(HELD_SLOT(work), LET_GO_POWER, LET_GO_FREQ,
                                VIBRATION_UNTIMED);
            }
        }
        break;
    case MODE_UP:
        if (HANDLE(task) != NULL) {
            HANDLE(task)->work->collision->flags &= ~COLLISION_TARGET;
        }
        REEL(task)->speed = 0.0F;
        REEL(task)->length = UP_LENGTH(work);
        if (work->mode & REEL_HELD) {
            /* @hack redundant: the speed cleared again, though the case has
             * just cleared it */
            REEL(task)->speed = 0.0F;
            work->submode = MODE_LOWERING;
        }
        break;
    case MODE_LOWERING:
        if (work->mode & REEL_AT_BOTTOM) {
            REEL(task)->length += REEL(task)->speed;
            offset = REEL(task)->length - DOWN_LENGTH(work);
            REEL(task)->speed = (REEL(task)->speed * BOUNCE_DAMPING) -
                                (offset * BOUNCE_STIFFNESS);
            if (!(work->mode & REEL_HELD)) {
                REEL(task)->speed = 0.0F;
                REEL(task)->length = DOWN_LENGTH(work);
                work->submode = MODE_DOWN;
                work->mode &= ~REEL_AT_BOTTOM;
            }
        } else {
            REEL(task)->length += REEL(task)->speed;
            REEL(task)->speed += reel_speed_max * reel_speed_gain;
            if (REEL(task)->speed > reel_speed_max) {
                REEL(task)->speed = reel_speed_max;
            }
            if (REEL(task)->length >= DOWN_LENGTH(work)) {
                work->mode |= REEL_AT_BOTTOM;
            }
            vec = work->pos;
            vec.y += HANDLE_Y(REEL(task));
            sound_play_at_for(SOUND_WIRE, work, SOUND_PRIORITY, SOUND_VOLUME,
                              SOUND_DURATION, &vec);
        }
        if (work->mode & REEL_HELD) {
            pad_enable_player(0);
            if (HANDLE(task) != NULL) {
                HANDLE(task)->work->collision->flags &= ~COLLISION_TARGET;
            }
            if (player_work[HELD_SLOT(work)]->mode != PLAYER_MODE_HANG) {
                work->mode |= REEL_LET_GO;
                vibration_start(HELD_SLOT(work), LET_GO_POWER, LET_GO_FREQ,
                                VIBRATION_UNTIMED);
            }
        }
        break;
    case MODE_GRABBED:
        if (HANDLE(task) != NULL) {
            HANDLE(task)->work->collision->flags &= ~COLLISION_TARGET;
        }
        REEL(task)->length += REEL(task)->speed;
        offset = REEL(task)->length - DOWN_LENGTH(work);
        /* @hack spelling: speed = speed - its loss, where -= would do */
        REEL(task)->speed = REEL(task)->speed - (offset * PULL_STIFFNESS);
        if (REEL(task)->speed < 0.0F) {
            /* @hack spelling: the gain added as a negative here too, where -=
             * would do */
            REEL(task)->speed += -(reel_speed_max * reel_speed_gain);
            if (REEL(task)->speed < -reel_speed_max) {
                REEL(task)->speed = -reel_speed_max;
            }
            work->submode = MODE_RAISING;
            work->mode &= ~REEL_AT_TOP;
        }
        break;
    /* @hack spelling: the last case without a break, where a break would
     * do */
    case MODE_DOWN:
        if (HANDLE(task) != NULL) {
            HANDLE(task)->work->collision->flags |= COLLISION_TARGET;
        }
        if (work->mode & REEL_HELD) {
            pad_disable_player(0);
            work->submode = MODE_GRABBED;
        }
    }
    if (REEL(task)->object.model != NULL) {
        if (player_near((NJS_POINT3 *)REEL(task)->object.pos,
                        REEL(task)->object.model->r + NEAR_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            /* @hack spelling: the flag cleared as work->flags & ~WORLD_ADDED,
             * where &= would do */
            work->flags = work->flags & ~WORLD_ADDED;
        }
    }
    if (!(work->mode & REEL_HELD)) {
        /* @bug memory: reads the handle's child through null where
         * task_create_child had no task to give (MOV.L at 0x844 of the
         * object's code). golem_reel_init then returns with this step set.
         * each case above tests the child for null first (TST at 0x442) */
        handle = HANDLE(task)->work;
        collision = handle->collision;
        if ((collision->flags & COLLISION_TOUCHED) && work->tick > GRAB_DELAY &&
            collision->hit->kind == COLLISION_PLAYER &&
            (player = player_slot_of(collision->hit->owner)) >= 0) {
            work->mode |= REEL_HELD;
            work->timer = player;
            REEL(task)->speed = GRAB_SPEED;
            /* @hack spelling: the slot read back from the work's timer in
             * both calls, where player would do */
            player_request(HELD_SLOT(work), PLAYER_REQUEST_HANG, HANG_ANIM);
            vibration_start(HELD_SLOT(work), GRAB_POWER, GRAB_FREQ,
                            VIBRATION_UNTIMED);
        } else {
            handle->pos.y = work->pos.y - WIRE_LENGTH(REEL(task)) - SHAPE_DROP;
            collision_fit_record_radius(handle->collision);
            collision_register(HANDLE(task));
            if (work->tick <= GRAB_DELAY) {
                handle->collision->shapes->flags |= SHAPE_OFF;
                work->tick++;
            } else {
                handle->collision->shapes->flags &= ~SHAPE_OFF;
            }
            /* @hack redundant: a block no path reaches, where nothing would
             * do. the image holds none of its code, but SHC still keeps
             * spare_2 and spare_3 in the frame */
            if (0) {
                half = (Sint16 *)&spare_2;
                half = (Sint16 *)&spare_3;
                if (spare_2 + spare_2) {
                }
                if (spare_3 + spare_3) {
                }
            }
        }
    } else {
        /* @hack spelling: the hang point written through hang, which points
         * to vec, where vec itself would do */
        NJS_POINT3 *hang = &vec;

        *hang = work->pos;
        hang->y += HANDLE_Y(REEL(task));
        switch (player_records[HELD_SLOT(work)]->kind) {
        case PLAYER_SONIC:
            hang->y -= hang_drop_sonic;
            break;
        case PLAYER_SHADOW:
            hang->y -= hang_drop_shadow;
            break;
        case PLAYER_KNUCKLES:
            hang->y -= hang_drop_knuckles;
            break;
        case PLAYER_ROUGE:
            hang->y -= hang_drop_rouge;
            break;
        case PLAYER_EGGMAN_MECH:
            hang->y -= hang_drop_eggman_mech;
            break;
        case PLAYER_TAILS_MECH:
            hang->y -= hang_drop_tails_mech;
            break;
        case PLAYER_KIND_AMY:
            hang->y -= hang_drop_amy;
            break;
        case PLAYER_KIND_METALSONIC:
            hang->y -= hang_drop_metalsonic;
            break;
        case PLAYER_KIND_TICAL:
            hang->y -= hang_drop_tical;
            break;
        case PLAYER_KIND_CHAOS0:
            hang->y -= hang_drop_chaos0;
            break;
        case PLAYER_KIND_BWALK:
            hang->y -= hang_drop_bwalk;
            break;
        case PLAYER_KIND_CWALK:
            hang->y -= hang_drop_cwalk;
            break;
        default:
            hang->y -= HANG_DROP;
            break;
        }
        if (game_state.cleared == 0) {
            /* @hack spelling: each move added back as a term of its own,
             * where += would do */
            hang->x =
                ((player_work[HELD_SLOT(work)]->pos.x - hang->x) * FOLLOW) +
                hang->x;
            hang->z =
                ((player_work[HELD_SLOT(work)]->pos.z - hang->z) * FOLLOW) +
                hang->z;
            turn =
                HALF_TURN - work->ang.y - player_work[HELD_SLOT(work)]->ang.y;
            /* @hack spelling: turn's low half scaled through the pointer half
             * and the float part, where *(Sint16 *)&turn *= FOLLOW would do */
            half = (Sint16 *)&turn;
            part = *half;
            part *= FOLLOW;
            *half = part;
            player_set_pos(HELD_SLOT(work), hang->x, hang->y, hang->z);
            player_work[HELD_SLOT(work)]->ang.y += turn;
            /* @hack spelling: the held player's ang.x and ang.z cleared in one
             * chained assignment, where two would do */
            player_work[HELD_SLOT(work)]->ang.x =
                player_work[HELD_SLOT(work)]->ang.z = 0;
        }
        /* @hack redundant: a block no path reaches, where nothing would do.
         * the image holds none of its code, but SHC counts its uses as it
         * assigns the step's registers */
        if (0) {
            njPopMatrixEx();
            njPopMatrixEx();
            njPopMatrixEx();
            njPopMatrixEx();
            njPopMatrixEx();
            njRotateZ(NULL, work->ang.z + THROW_TILT);
            njRotateZ(NULL, work->ang.z + THROW_TILT);
            njRotateZ(NULL, work->ang.z + THROW_TILT);
            REEL(task)->handle_y = -HANDLE_DROP;
            REEL(task)->handle_y = -HANDLE_DROP;
            REEL(task)->handle_y = -HANDLE_DROP;
            REEL(task)->handle_y = -HANDLE_DROP;
            REEL(task)->length = UP_LENGTH(work);
            REEL(task)->length = UP_LENGTH(work);
            REEL(task)->length = UP_LENGTH(work);
            REEL(task)->speed = 1.0F;
            REEL(task)->speed = 1.0F;
            REEL(task)->speed = 1.0F;
            player_near((NJS_POINT3 *)REEL(task)->object.pos,
                        REEL(task)->object.model->r);
            player_near((NJS_POINT3 *)REEL(task)->object.pos,
                        REEL(task)->object.model->r);
            REEL(task)->speed = 0.0F;
            REEL(task)->speed = 0.0F;
            REEL(task)->speed = 0.0F;
        }
        work->tick = 0;
    }
}

static void golem_reel_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    Float radius;

    /* @hack redundant: a block no path reaches, which leaves its strings in
     * the object */
    if (0) {
        (void)"<- MODE(U/D)";
        (void)"<- ANGLE Y";
        (void)"<- THROW ANG";
        (void)"        _   ";
        (void)"          _ ";
        (void)"<- WIRE UP SCL";
        (void)"<- WIRE DOWN SCL";
        (void)"<- ACC OFS";
    }
    njSetTexture(&reel_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    object = &reel_top_object;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(object->model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    object = object->child;
    njPushMatrixEx();
    /* njCnkEasyDrawModel clips a model by the sphere of its radius, but not a
     * model of radius 0. the scale stretches the wire past its radius, so the
     * wire draws with the radius set to 0 */
    radius = object->model->r;
    object->model->r = 0.0F;
    njScale(NULL, 1.0F, REEL(task)->wire_scale, 1.0F);
    njCnkEasyDrawModel(object->model);
    object->model->r = radius;
    njPopMatrixEx();
    object = object->child;
    njTranslate(NULL, 0.0F, REEL(task)->handle_y, 0.0F);
    if (objects_fog_off) {
        njFogDisable();
    }
    if (game_state.steps % SIMPLE_PERIOD < SIMPLE_STEPS ||
        (work->mode & REEL_HELD)) {
        njCnkSimpleDrawModel(object->model);
    } else {
        njCnkEasyDrawModel(object->model);
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    njScale(NULL, SHADOW_SCALE_XZ, SHADOW_SCALE_Y, SHADOW_SCALE_XZ);
    njCnkModDrawObject(&shadow_object);
    njPopMatrixEx();
}

/* as golem_reel_draw, but the top draws from its build, and so does the
 * handle where it draws simply */
static void golem_reel_draw_built(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    Float radius;

    njSetTexture(&reel_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: object set to the top only for its child below, where
     * object = reel_top_object.child would do */
    object = &reel_top_object;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njDirectDrawObject(golem_reel_builds[BUILD_TOP].built);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    object = object->child;
    njPushMatrixEx();
    radius = object->model->r;
    object->model->r = 0.0F;
    njScale(NULL, 1.0F, REEL(task)->wire_scale, 1.0F);
    njCnkEasyDrawModel(object->model);
    object->model->r = radius;
    njPopMatrixEx();
    object = object->child;
    njTranslate(NULL, 0.0F, REEL(task)->handle_y, 0.0F);
    if (objects_fog_off) {
        njFogDisable();
    }
    if (game_state.steps % SIMPLE_PERIOD < SIMPLE_STEPS ||
        (work->mode & REEL_HELD)) {
        njDirectDrawObject(golem_reel_builds[BUILD_HANDLE].built);
    } else {
        njCnkEasyDrawModel(object->model);
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    njScale(NULL, SHADOW_SCALE_XZ, SHADOW_SCALE_Y, SHADOW_SCALE_XZ);
    njCnkModDrawObject(&shadow_object);
    njPopMatrixEx();
}
