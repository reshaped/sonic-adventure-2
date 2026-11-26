/* PNP in Sky Rail's object list: a platform with three arms, which rests
 * REST_HEIGHT above its base. a falling player who lands on its hub bounces
 * off. the landing pushes the platform up and spins its arms, each by the
 * landing's speed. the platform then falls back to rest */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <mathf.h>

#include "collision/collision.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "stg06/pnp.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "vmu/vibration.h"

#include "stg06/pnp/text.inc"
#include "stg06/scenery_art/pnp.h"

/* the platform's record, in task->motion */
struct pnp {
    Angle turn; /* the arms draw turned by -turn about y */
    Float spin; /* the arms' spin, which sets the turn's gain a step */
    Float base_y; /* the work's pos.y as it was placed */
    Float lift; /* the platform's speed up a step */
    /* what lift gains each step of a landing's push, while the work's tick
     * counts the push down */
    Float push;
};

#define PNP(task) ((struct pnp *)(task)->motion.p)
/* game_state.time_stopped at a player's last touch, in task->unk_3c. a step
 * with no time stop clears it. while it isn't 0, the platform moves through a
 * time stop */
#define TOUCHED_IN_STOP(task) ((task)->unk_3c.l)

/* the platform rests REST_HEIGHT above base_y, and rises no higher than scl.x +
 * CEILING_HEIGHT above it */
#define REST_HEIGHT 19.0F
#define CEILING_HEIGHT 50.0F
/* the floor mark draws FLOOR_LIFT above base_y */
#define FLOOR_LIFT 0.01F
/* every shape's id, which the _with_id calls of collision/collision.h match */
#define SHAPE_ID 9
/* the hub is shape HUB_SHAPE, a ball of HUB_RADIUS whose middle lies HUB_DEPTH
 * below the work's pos */
#define HUB_DEPTH 8.0F
#define HUB_RADIUS 8.0F
#define HUB_SHAPE 0
/* the arms are ARMS shapes from FIRST_ARM on. each lies ARM_REACH out from the
 * hub's upright axis. the first lies at -turn - ARM_OFFSET about y, and each
 * next ARM_SPACING further */
#define FIRST_ARM 2
#define ARMS 3
#define SHAPES (FIRST_ARM + ARMS)
#define ARM_REACH 9.0F
#define ARM_OFFSET NJM_DEG_ANG(30)
#define ARM_SPACING (FULL_TURN / 3)
/* a landing's spin is LANDING_SPIN of the player's speed, and the arms take it
 * as their spin */
#define LANDING_SPIN 0.7F
/* a falling player bounces off the platform. it rises PLAYER_RAISE, and its
 * flat speed turns back. its speed up is the landing's spin, no less than
 * MIN_BOUNCE, plus BOUNCE_LIFT. its stick then does nothing for
 * STICK_LOCK_STEPS */
#define PLAYER_RAISE 0.1F
#define MIN_BOUNCE 1.0F
#define BOUNCE_LIFT 0.1F
#define STICK_LOCK_STEPS 13
/* a landing faster than FAST_LANDING scales the player's flat speed before the
 * bounce. a flat speed over FLAT_SPEED becomes FLAT_SPEED. one over
 * SLOW_FLAT_FLOOR but no more than FLAT_SPEED becomes SLOW_FLAT_SPEED. a slower
 * one stays as it is */
#define FAST_LANDING 2.0F
#define FLAT_SPEED 0.4F
#define SLOW_FLAT_SPEED 0.2F
#define SLOW_FLAT_FLOOR 0.01F
/* a fast falling landing turns the player to face the hub. its heading becomes
 * FACE_HUB less atan2f(dx, dz) * ANGLE_PER_RADIAN_F, where (dx, dz) is its
 * offset from the hub */
#define FACE_HUB 0xc000
/* a landing's push lasts PUSH_STEPS steps. the platform's record loses
 * COLLISION_TARGET for all but the push's last TARGET_STEPS steps. PUSH_SOUND
 * plays as PUSH_SOUND_STEP steps of the push remain */
#define PUSH_STEPS 23
#define TARGET_STEPS 18
#define PUSH_SOUND_STEP 10
/* a landing raises a ring of dust about the point LANDING_DUST_DEPTH below the
 * work's pos. it shakes the player's pad too */
#define LANDING_DUST_DEPTH 9.0F
#define LANDING_DUST_RADIUS 5.0F
#define LANDING_DUST_SIZE 2.8F
#define LANDING_DUST_PUFFS 7
#define VIBRATION_POWER 4
#define VIBRATION_FREQ 15
/* a player's touch that is no landing adds UNDER_SPIN to the spin, while
 * something stands on the platform */
#define UNDER_SPIN 0.05F
/* the spin keeps SPIN_KEEP of itself twice a step. a platform out of its push
 * keeps LIFT_KEEP of its lift. the lift then loses GRAVITY */
#define SPIN_KEEP 0.98F
#define LIFT_KEEP 0.99F
#define GRAVITY 0.1F
/* where the platform meets its rest height with a lift over REST_HIT_SPEED
 * either way, it hits the rest. a hit raises an outer and an inner ring of
 * dust about the point REST_DUST_DEPTH below its pos. it then plays
 * REST_HIT_SOUND. at the rest height the lift is held within MAX_LIFT either
 * way. the lift then keeps REST_KEEP of itself */
#define REST_HIT_SPEED 1.0F
#define REST_DUST_DEPTH 22.0F
#define REST_OUTER_RADIUS 8.0F
#define REST_OUTER_SIZE 2.5F
#define REST_OUTER_PUFFS 8
#define REST_INNER_RADIUS 5.0F
#define REST_INNER_SIZE 1.5F
#define REST_INNER_PUFFS 6
#define MAX_LIFT 1.0F
#define REST_KEEP 0.97F
/* each step's rise adds RISE_SPIN of itself to the spin */
#define RISE_SPIN 0.02F
/* a spin of 1 turns the arms a full turn every 10 * 3.14 steps */
#define SPIN_TURN (FULL_TURN / (10.0F * 3.14F))
/* the platform's sounds, entries of the stage's sound list */
#define LANDING_SOUND 0
#define PUSH_SOUND 1
#define REST_HIT_SOUND 2
#define SOUND_PRIORITY 0
#define SOUND_VOLUME 0
/* the arms' sibling draws up to SINK_DEPTH below its place, by the steps left
 * of the push */
#define SINK_DEPTH 5.0F

/* pnp_edit_draw does nothing. nothing calls it, and no header declares it. it
 * is external because the image holds it, and SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void pnp_edit_draw(struct task *task);
/* NOLINTEND(misc-use-internal-linkage) */

NJS_TEXLIST *pnp_texlists[] = {&pnp_texlist, &pnp_ceiling_texlist,
                               &pnp_floor_texlist, &dust_texlist, NULL};
/* a landing sets the platform's lift to pnp_sink times the landing's spin. the
 * push then adds pnp_push times that spin to the lift, spread over
 * PUSH_STEPS */
static Float pnp_sink = -0.33F;
static Float pnp_push = 1.75F;
/* the hub, the post, and the three arms */
/* clang-format off */
static struct collision_shape pnp_shapes[SHAPES] = {
    {SHAPE_ID, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, -HUB_DEPTH, 0.0F}, HUB_RADIUS, 0.0F, 0.0F, 0, {0, 0, 0}},
    {SHAPE_ID, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, -16.44F, 0.0F}, 6.0F, 10.0F, 0.0F, 0, {0, 0, 0}},
    {SHAPE_ID, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, -16.0F, 0.0F}, 7.0F, 6.5F, 1.0F, 0, {0, 0, 0}},
    {SHAPE_ID, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, -16.0F, 0.0F}, 7.0F, 6.5F, 1.0F, 0, {0, 0, 0}},
    {SHAPE_ID, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, -16.0F, 0.0F}, 7.0F, 6.5F, 1.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void pnp_destroy(struct task *task);
static void pnp_step(struct task *task);
static void pnp_draw(struct task *task);

void pnp_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct pnp));
    if (task->motion.p == NULL) {
        return;
    }
    work->submode = 0;
    collision_init(task, pnp_shapes, SHAPES, COLLISION_OBJECT);
    work->collision->flags |= COLLISION_TARGET;
    task->draw = pnp_draw;
    task->step = pnp_step;
    PNP(task)->base_y = work->pos.y;
    work->pos.y += REST_HEIGHT;
    task->work->id = WORK_ID_HOMING_TARGET;
    task->destructor = pnp_destroy;
}

static void pnp_destroy(struct task *task)
{
    /* @hack spelling: pnp, a copy of task, where task would do */
    struct task *pnp = task;

    syFree(pnp->motion.p);
    pnp->motion.p = NULL;
}

void pnp_edit_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, where an empty function would
     * do. it is a stand-in that gives the function the image's registers and
     * frame, and places the set editor's labels in this source's constants */
    if (0) {
        njPrintC(NJM_LOCATION(17, 17), "<- ANGLE Y");
        njPrintC(NJM_LOCATION(17, 19), "<- HIGH HEIGHT(+50)");
        njPushMatrixEx();
        njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
        njCnkEasyDrawModel(&pnp_ceiling_model);
        njPopMatrix(1);
        njPushMatrixEx();
        njTranslate(NULL, work->pos.x, work->pos.y + REST_HEIGHT, work->pos.z);
        njCnkEasyDrawModel(&pnp_ceiling_model);
        njPopMatrix(1);
        njPushMatrixEx();
        njTranslate(NULL, work->pos.x,
                    work->pos.y + work->scl.x + CEILING_HEIGHT, work->pos.z);
        njRotateY(NULL, PNP(task)->turn);
        njCnkEasyDrawModel(&pnp_ceiling_model);
        njPopMatrix(1);
    }
}

/* a player's touch on the hub is that player's landing. a touch elsewhere is a
 * landing too where a player lies within HUB_RADIUS of the hub's middle. the
 * landing is then that player's */
static void pnp_step(struct task *task)
{
    struct task_work *work;
    int slot;
    Float speed;
    /* @hack spelling: point, one local for the hub's middle, the dust's place
     * and each arm's point, where a local for each would do */
    NJS_POINT3 point;
    NJS_VECTOR spd;
    NJS_VECTOR flat;
    NJS_POINT3 pos;
    NJS_POINT3 *player_pos;
    Float scale;
    Float spin;
    Float last_y;
    /* @hack spelling: face, a local for the player's new heading, where the
     * difference in the assignment would do */
    Angle face;
    Angle ang;
    Uint32 i;

    work = task->work;
    if (!game_state.time_stopped) {
        TOUCHED_IN_STOP(task) = 0;
    }
    if ((work->collision->flags & COLLISION_TOUCHED) &&
        (slot = player_slot_of(work->collision->hit->owner)) >= 0) {
        point = work->pos;
        point.y -= HUB_DEPTH;
        if (work->collision->own_shape == HUB_SHAPE ||
            (slot = player_near(&point, HUB_RADIUS) - 1) >= 0) {
            sound_play(LANDING_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
            TOUCHED_IN_STOP(task) = game_state.time_stopped;
            player_pos = &player_work[slot]->pos;
            flat = spd =
                ((struct player_motion *)player_tasks[slot]->motion.p)->spd;
            flat.y = 0.0F;
            speed = spin = njScalor(&spd);
            scale = njScalor(&flat);
            if (speed > FAST_LANDING) {
                if (scale > FLAT_SPEED) {
                    scale = FLAT_SPEED / scale;
                    flat.x *= scale;
                    flat.z *= scale;
                } else if (scale > SLOW_FLAT_FLOOR) {
                    scale = SLOW_FLAT_SPEED / scale;
                    flat.x *= scale;
                    flat.z *= scale;
                }
            }
            PNP(task)->spin = spin *= LANDING_SPIN;
            if (spd.y < 0.0F) {
                if (spin < MIN_BOUNCE) {
                    spin = MIN_BOUNCE;
                }
                player_set_pos(slot, player_pos->x,
                               player_pos->y + PLAYER_RAISE, player_pos->z);
                /* @hack redundant: spd.y tested again, which the test about
                 * the call settles, where spin + BOUNCE_LIFT alone would do */
                player_set_speed(slot, -flat.x,
                                 spd.y < 0.0F ? spin + BOUNCE_LIFT : 0.0F,
                                 -flat.z);
                if (speed > FAST_LANDING) {
                    face =
                        FACE_HUB - (Angle)(atan2f(player_pos->x - work->pos.x,
                                                  player_pos->z - work->pos.z) *
                                           ANGLE_PER_RADIAN_F);
                    player_work[slot]->ang.y = face;
                }
                player_records[slot]->stick_lock = STICK_LOCK_STEPS;
            }
            PNP(task)->lift = _builtin_fabsf(spin) * pnp_sink;
            work->tick = PUSH_STEPS;
            PNP(task)->push =
                _builtin_fabsf(pnp_push * spin) * (1.0F / PUSH_STEPS);
            pos = work->pos;
            pos.y += -LANDING_DUST_DEPTH;
            dust_short_ring(&pos, LANDING_DUST_RADIUS, LANDING_DUST_SIZE,
                            LANDING_DUST_PUFFS);
            vibration_start(slot, VIBRATION_POWER, VIBRATION_FREQ,
                            VIBRATION_UNTIMED);
        } else {
            TOUCHED_IN_STOP(task) = game_state.time_stopped;
            if (work->collision->flags & COLLISION_UNDER) {
                PNP(task)->spin += UNDER_SPIN;
            }
        }
    }
    if (TOUCHED_IN_STOP(task) || !game_state.time_stopped) {
        if (work->tick) {
            work->tick--;
            PNP(task)->lift += PNP(task)->push;
            if (work->tick < TARGET_STEPS) {
                work->collision->flags |= COLLISION_TARGET;
            } else {
                work->collision->flags &= ~COLLISION_TARGET;
            }
            if (work->tick == PUSH_SOUND_STEP) {
                sound_play(PUSH_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
            }
        }
    }
    /* @hack redundant: the test above made again for a block of its own,
     * where the block above would hold this one too */
    if (TOUCHED_IN_STOP(task) || !game_state.time_stopped) {
        last_y = work->pos.y;
        PNP(task)->spin *= SPIN_KEEP;
        if (!work->tick) {
            PNP(task)->lift *= LIFT_KEEP;
        }
        PNP(task)->lift -= GRAVITY;
        work->pos.y += PNP(task)->lift;
        if (work->pos.y < PNP(task)->base_y + REST_HEIGHT) {
            PNP(task)->lift += GRAVITY;
            if (_builtin_fabsf(PNP(task)->lift) > REST_HIT_SPEED) {
                point = work->pos;
                point.y -= REST_DUST_DEPTH;
                dust_short_ring(&point, REST_OUTER_RADIUS, REST_OUTER_SIZE,
                                REST_OUTER_PUFFS);
                dust_ring(&point, REST_INNER_RADIUS, REST_INNER_SIZE,
                          REST_INNER_PUFFS);
                sound_play(REST_HIT_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
            }
            if (PNP(task)->lift > MAX_LIFT) {
                PNP(task)->lift = MAX_LIFT;
            }
            if (PNP(task)->lift < -MAX_LIFT) {
                PNP(task)->lift = -MAX_LIFT;
            }
            PNP(task)->lift *= REST_KEEP;
            work->pos.y = PNP(task)->base_y + REST_HEIGHT;
        }
        if (work->pos.y > PNP(task)->base_y + work->scl.x + CEILING_HEIGHT) {
            work->pos.y = PNP(task)->base_y + work->scl.x + CEILING_HEIGHT;
        }
        last_y = work->pos.y - last_y;
        PNP(task)->spin = (PNP(task)->spin * SPIN_KEEP) + (last_y * RISE_SPIN);
        PNP(task)->turn += (Angle)(PNP(task)->spin * SPIN_TURN);
        ang = -PNP(task)->turn - ARM_OFFSET;
        for (i = FIRST_ARM; i < FIRST_ARM + ARMS; i++) {
            point.x = ARM_REACH;
            point.y = 0.0F;
            point.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, ang);
            njCalcPoint(NULL, &point, &point);
            njPopMatrix(1);
            work->collision->shapes[i].ang.y = ang;
            work->collision->shapes[i].center.x = point.x;
            work->collision->shapes[i].center.z = point.z;
            ang += ARM_SPACING;
        }
    }
    collision_fit_record_radius(work->collision);
    collision_register(task);
}

static void pnp_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    Float *pos;
    Uint8 tick;
    Float sink;

    njSetTexture(&pnp_texlist);
    object = &pnp_object;
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y - REST_HEIGHT, work->pos.z);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    object = object->child;
    njPushMatrixEx();
    pos = object->pos;
    njTranslateV(NULL, (NJS_VECTOR *)pos);
    njRotateY(NULL, -PNP(task)->turn);
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
    object = object->sibling;
    tick = work->tick;
    sink = tick ? -(tick * (SINK_DEPTH / PUSH_STEPS)) : 0.0F;
    pos = object->pos;
    njTranslate(NULL, pos[0], pos[1] + sink, pos[2]);
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
    njFogDisable();
    njPushMatrixEx();
    njSetTexture(&pnp_floor_texlist);
    njTranslate(NULL, work->pos.x, PNP(task)->base_y + FLOOR_LIFT, work->pos.z);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(pnp_floor_object->model);
    matrix_load_parent();
    njSetTexture(&pnp_ceiling_texlist);
    njTranslate(NULL, work->pos.x,
                PNP(task)->base_y + work->scl.x + CEILING_HEIGHT, work->pos.z);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(pnp_ceiling_object->model);
    njPopMatrixEx();
    njFogEnable();
}
