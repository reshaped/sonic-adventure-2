/* rings: the rings an object list places, and those that scatter from a
 * player. one task draws all but the SG rings from a chain of nodes */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/mesh.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "shared/ring.h"
#include "shared/ring_sparkle.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"

/* a placed ring with this in the low 16 bits of its ang.x has no floor */
#define RING_PLACED_FLOORLESS 1
/* a placed ring with this scl.x follows its floor */
#define RING_FOLLOWS_FLOOR (-1.0F)
/* a placed ring's turn about y a step: about two degrees */
#define RING_TURN 364

/* a placed ring's modes, in its work's mode. ring_attach sets
 * MODE_INIT_LINKED. task->motion holds a ring_link or null in every mode but
 * MODE_INIT. on a MODE_GONE ring's next step, the ring kills its task. it
 * first sets RING_SPENT in its link, if it has one */
enum ring_mode { MODE_INIT, MODE_LIVE, MODE_INIT_LINKED, MODE_GONE };

/* a scattered ring's modes, in its work's mode. a MODE_TAKEN ring sparkles
 * on its next step. a MODE_EXPIRED or MODE_TAKEN ring destroys itself on its
 * next step */
enum scattered_ring_mode {
    MODE_LAUNCH,
    MODE_FLY,
    MODE_REST,
    MODE_EXPIRED,
    MODE_TAKEN
};

/* a ring that follows its floor looks for it once in FLOOR_WAIT + 1 steps.
 * it looks only within FLOOR_RANGE of the camera. its work's submode counts
 * down to the next look. the submode is -1 for a ring that doesn't follow its
 * floor. where the floor has moved FLOOR_JUMP or more, a scattered ring takes
 * the ring's place, FLOOR_DROP below it */
#define FLOOR_WAIT 8
#define FLOOR_RANGE 400.0F
#define FLOOR_JUMP 8.0F
#define FLOOR_DROP 10.0F

/* a bit of a ring_state's flags: the ring was placed without a floor */
#define RING_FLOORLESS 0x1

/* the state each ring keeps at task->force */
struct ring_state {
    Sint32 unk_00;
    Float spin; /* a scattered ring's angle about y, in degrees */
    /* a scattered ring's floor as the ring comes to rest */
    Float rest_floor;
    /* the value game_state.time_stopped held when a scattered ring was made,
     * and 0 for any other ring. while game_state.time_stopped isn't 0, a ring
     * whose made_in_stop is 0 doesn't turn, fall or count its steps */
    Sint32 made_in_stop;
    Sint32 flags; /* the RING_FLOORLESS bit */
#ifdef NONMATCHING
    /* the ring's entry in the chain, which this side keeps here. the matching
     * side keeps it in unk_3c, a Sint32 */
    struct ring_entry *entry;
#endif
};

/* what a scattered ring keeps at task->extra */
struct scattered_ring {
    /* where the surface toward gravity lies, as a coordinate on the axis
     * vector_side picks from world_gravity */
    Float floor;
    Float probe_wait; /* the steps to the next look for the surface */
};

/* a ring's entry in the chain. the chain is the list, linked by sibling, of
 * the nodes one task draws the rings from. an entry starts with its node, so
 * a node's address is its entry's. an SG ring has no entry */
struct ring_entry {
    NJS_CNK_OBJECT object;
    struct angles shadow_ang;
    Float shadow_y; /* the shadow's floor height, or NO_FLOOR for none */
#ifdef NONMATCHING
    /* the ring_state of the ring this entry draws, or null once
     * ring_entry_remove has run. the chain's task can go before the ring, so
     * ring_chain_destroy clears the ring's entry through it */
    struct ring_state *owner;
#endif
};

#define LINK(task) ((struct ring_link *)(task)->motion.p)
#define STATE(task) ((struct ring_state *)(task)->force.p)
#ifdef NONMATCHING
/* the entry by the ring's state, where this side keeps it */
static struct ring_entry *ring_task_entry(const struct task *task)
{
    const struct ring_state *state = task->force.p;
    return state != NULL ? state->entry : NULL;
}
#define ENTRY(task) ring_task_entry(task)
#else
/* the task's unk_3c holds its ring's entry in the chain, or null */
#define ENTRY(task) ((struct ring_entry *)(task)->unk_3c.p)
#endif

/* a scattered ring first moves SCATTER_RISE a step against gravity and
 * SCATTER_SPEED a step along its heading. each step adds FALL_GRAVITY of
 * world_gravity to its speed. the speed along each axis stays within
 * FALL_FASTEST, and keeps FALL_DRAG of itself each step. the ring bounces
 * within RING_RADIUS of the surface. a bounce sends it off the surface at
 * BOUNCE of the speed it struck at. a bounce keeps BOUNCE_DRAG of its speed
 * along the surface */
#define SCATTER_RISE 1.8F
#define SCATTER_SPEED 0.4F
#define FALL_GRAVITY 0.08F
#define FALL_FASTEST 4.0F
#define FALL_DRAG 0.995F
#define RING_RADIUS 3.44F
#define BOUNCE 0.4F
#define BOUNCE_DRAG 0.95F

/* a scattered ring starts SCATTER_START against gravity from where it is
 * made, and spins SCATTER_SPIN degrees a step. a player can take it once it
 * has flown CATCH_STEPS steps. it comes to rest after FLY_STEPS steps, and
 * expires REST_STEPS steps later. while it rests, it shows only on steps
 * where its work's timer has BLINK_BIT set. it looks for the surface once in
 * PROBE_STEPS + 1 steps. until its first look, its floor lies FAR_FLOOR away
 * toward gravity. a look that finds no surface puts the floor FAR_FLOOR away
 * again */
#define SCATTER_START 10.0F
#define SCATTER_SPIN 5.0F
#define CATCH_STEPS 47
#define FLY_STEPS 240
#define REST_STEPS 180
#define BLINK_BIT 0x2
#define PROBE_STEPS 8.0F
#define FAR_FLOOR 100.0F

/* a shadow draws SHADOW_LIFT above its floor. a scattered ring's shadow
 * draws SCATTERED_SHADOW_LIFT higher still. a placed ring draws its shadow
 * only while its work's tick is below SHADOW_TICKS */
#define SHADOW_LIFT 0.2F
#define SCATTERED_SHADOW_LIFT 0.4F
#define SHADOW_TICKS 50

/* the most rings ring_scatter scatters */
#define SCATTER_MOST 20

/* the points a placed ring adds to the score when it is taken */
#define RING_POINTS 10

/* the sound ring_scatter plays */
#define SCATTER_SOUND 0x8014

/* a player takes an SG ring within SG_REACH of the player's first collision
 * shape */
#define SG_REACH 14.0F

/* the radius of a ring's collision record, as far as its sphere reaches */
#define RING_COLLISION_RADIUS 5.0F

/* what a taken ring adds to a mech's gauge */
#define RING_GAUGE 0.1F

/* a ring's depth is its z under the matrix the chain draws with. a ring at a
 * depth under NEAR_DEPTH draws from its near build. one under MIDDLE_DEPTH
 * draws from its middle build, and one further off from ring_object's. no
 * ring draws at a depth of BEHIND_DEPTH or less */
#define NEAR_DEPTH 65.0F
#define MIDDLE_DEPTH 130.0F
#define BEHIND_DEPTH (-10.0F)

/* a magnetic player pulls a ring within PULL_REACH of it. the pull's speed
 * starts as the ring's distance, at most PULL_REACH, times PULL_RATE. the
 * speed is held between PULL_SLOWEST and PULL_FASTEST. it then grows by
 * PULL_PER_SPEED of itself for each unit of the player's speed. PULL_SPIN
 * adds to the ring_state's spin, which no placed ring reads */
#define PULL_REACH 50.0F
#define PULL_RATE (1.3F / PULL_REACH)
#define PULL_SLOWEST 0.85F
#define PULL_FASTEST 5.0F
#define PULL_PER_SPEED 0.5F
#define PULL_SPIN 3.0F

/* a taken ring gives SPARKLES sparkles round it, SPARKLE_RADIUS out and a
 * quarter turn apart. they stand SPARKLE_RISE apart in height, the lowest at
 * SPARKLE_LOW + SPARKLE_RISE above the ring. each starts rising
 * SPARKLE_SPEED a step */
#define SPARKLES 4
#define SPARKLE_RADIUS 4.0F
#define SPARKLE_LOW (-1.5F)
#define SPARKLE_RISE 1.8F
#define SPARKLE_SPEED 0.02F
#define SPARKLE_SCALE 2.5F

/* the degrees in a turn, round which a scatter spreads its rings */
#define DEGREES_PER_TURN 360
/* ring_pull's set of the magnetic players' slots holds SLOT_BIT(slot) for
 * each */
#define SLOT_BIT(slot) (1 << (slot))

/* the task levels of a scattered ring and of the chain's task */
#define SCATTERED_LEVEL 2
#define CHAIN_LEVEL 1

/* the chain and ring_build's four builds. an initializer places each in the
 * data section, where every image has it, not in bss */
static NJS_CNK_OBJECT *ring_chain = NULL; /* the first entry's node */
/* whether ring_entry_remove has left an entry for the chain's task to free */
static Bool ring_entries_removed = FALSE;
static struct task *ring_chain_task = NULL;
static Bool ring_built = FALSE; /* whether ring_build has made all four */
static NJS_DIRECT_HEAD *ring_far_build = NULL;
static NJS_DIRECT_HEAD *ring_middle_build = NULL;
static NJS_DIRECT_HEAD *ring_near_build = NULL;
static NJS_DIRECT_HEAD *ring_shadow_build = NULL;

static void sg_ring_draw(struct task *task);
static void ring_destroy(struct task *task);
static Bool ring_pull(struct task *task);
static struct ring_entry *ring_entry_add(void);
static void ring_entry_remove(struct ring_entry *entry);
static void ring_chain_destroy(struct task *task);
static void ring_chain_step(struct task *task);
static void ring_chain_draw(struct task *task);
static void ring_chain_start(void);

#ifdef NONMATCHING
#define RING_NO_COLLISION (-1)

/* give task its ring_state, and a collision record of collision_kind unless
 * that is RING_NO_COLLISION. it returns FALSE, and keeps neither, where the
 * heap can't give both. the task then stays in its first mode and tries
 * again on its next step */
static Bool ring_prepare(struct task *task, int collision_kind)
{
    struct ring_state *state = syCalloc(1, sizeof(*state));
    if (state == NULL) {
        return FALSE;
    }
    if (collision_kind >= 0 &&
        !collision_init_in_place(task, &ring_collision_shape, 1,
                                 (Uint8)collision_kind)) {
        syFree(state);
        return FALSE;
    }
    task->force.p = state;
    return TRUE;
}

static void ring_attach_entry(struct task *task)
{
    struct ring_state *owner = STATE(task);
    owner->entry = ring_entry_add();
    if (owner->entry != NULL) {
        owner->entry->owner = owner;
    }
}
#endif

void ring_reset_floor_phase(void)
{
    ring_floor_phase = 0;
}

void ring_attach(struct task *ring, struct ring_link *link)
{
    struct task_work *work = ring->work;

    if (work->mode == MODE_INIT) {
        ring->motion.p = link;
#ifdef NONMATCHING
        /* a ring destroyed before its first step clears its link's ring too.
         * the group then never holds a freed task */
        ring->destructor = ring_destroy;
#endif
        /* @hack spelling: the link's ring set through ring->motion, where
         * link->ring would do */
        LINK(ring)->ring = ring;
        work->mode = MODE_INIT_LINKED;
    }
}

void ring_detach(struct task *ring)
{
    if (ring->motion.p != NULL) {
        LINK(ring)->flags &= ~RING_LEFT;
        LINK(ring)->ring = NULL;
        ring->motion.p = NULL;
    }
}

/* make the sparkles of a ring taken at task's pos */
static void ring_sparkle(struct task *task)
{
    struct task_work *work;
    Float x;
    Float y;
    Float z;
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    int i;
    /* @hack spelling: the quarter turn held in step, and step declared
     * before ang, where ang += QUARTER_TURN would do */
    Angle step = QUARTER_TURN;
    Angle ang = 0;

    /* @hack spelling: work set by a statement of its own, where its
     * declaration's initializer would do */
    work = task->work;
    /* @hack spelling: the work's pos held in x, y and z, where work->pos at
     * each use would do */
    x = work->pos.x;
    y = work->pos.y;
    z = work->pos.z;
    spd.x = 0.0F;
    spd.y = SPARKLE_SPEED;
    spd.z = 0.0F;
    for (i = SPARKLES; i > 0; i--, ang += step) {
        pos.x = (njSin(ang) * SPARKLE_RADIUS) + x;
        pos.y = (i * SPARKLE_RISE) + y + SPARKLE_LOW;
        pos.z = (njCos(ang) * SPARKLE_RADIUS) + z;
        ring_sparkle_create(&pos, &spd, SPARKLE_SCALE);
    }
}

void ring_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *other;
    struct task *scattered;
    struct task_work *scattered_work;
    int player;
    Float dx;
    Float dy;
    Float dz;
    Float shift;
    Float floor;

    if (placement_try_leave(task)) {
        if (work->mode != MODE_INIT) {
            if (task->motion.p != NULL) {
                LINK(task)->flags |= RING_LEFT;
                LINK(task)->ring = NULL;
                task->motion.p = NULL;
            }
            /* @hack redundant: motion cleared again, which the test above
             * has cleared where it was set */
            task->motion.p = NULL;
        }
        return;
    }
    if (ENTRY(task) != NULL) {
        ENTRY(task)->object.pos[0] = work->pos.x;
        ENTRY(task)->object.pos[1] = work->pos.y;
        ENTRY(task)->object.pos[2] = work->pos.z;
        ENTRY(task)->object.ang[1] = work->ang.y;
        ENTRY(task)->shadow_ang = work->ang;
        ENTRY(task)->shadow_y =
            (work->tick < SHADOW_TICKS && work->scl.y != NO_FLOOR) ? work->scl.y
                                                                   : NO_FLOOR;
    }
    /* @hack spelling: MODE_INIT_LINKED labelled ahead of MODE_INIT, where
     * MODE_INIT first would do */
    switch (work->mode) {
    case MODE_INIT_LINKED:
    case MODE_INIT:
        if (work->mode != MODE_INIT_LINKED) {
            task->motion.p = NULL;
        }
        if (task->motion.p != NULL) {
            LINK(task)->flags &= ~RING_LEFT;
            LINK(task)->ring = task;
        }
#ifdef NONMATCHING
        /* this side makes the state and the shape by ring_prepare. it waits a
         * step where either can't be made */
        if (!ring_prepare(task, COLLISION_PICKUP)) {
            return;
        }
#else
        task->force.p = syCalloc(1, sizeof(struct ring_state));
        collision_init_in_place(task, &ring_collision_shape, 1,
                                COLLISION_PICKUP);
#endif
        if (work->collision != NULL) {
            /* @hack spelling: the collision record reached through
             * task->work, where work would do */
            task->work->collision->flags |= COLLISION_TARGET;
            work->collision->radius = RING_COLLISION_RADIUS;
        }
        if (work->scl.x == RING_FOLLOWS_FLOOR) {
            work->submode = ring_floor_phase;
            ring_floor_phase++;
            if (ring_floor_phase > FLOOR_WAIT) {
                ring_floor_phase = 0;
            }
        } else {
            work->submode = -1;
        }
        if ((Uint16)work->ang.x == RING_PLACED_FLOORLESS) {
            STATE(task)->flags |= RING_FLOORLESS;
            work->scl.y = NO_FLOOR;
        } else {
            work->scl.y = surface_probe_floor(work->pos.x, work->pos.y,
                                              work->pos.z, &work->ang);
        }
        work->mode = MODE_LIVE;
        STATE(task)->spin = 0.0F;
        work->scl.x = -2.0F;
        work->scl.z = -4.0F;
        work->id = WORK_ID_RING;
        task->destructor = ring_destroy;
#ifdef NONMATCHING
        /* this side links the ring and its entry both ways. the chain's destroy
         * can then let go of the ring */
        ring_attach_entry(task);
#else
        task->unk_3c.p = ring_entry_add();
#endif
        if (ENTRY(task) != NULL) {
            ENTRY(task)->object.pos[0] = work->pos.x;
            ENTRY(task)->object.pos[1] = work->pos.y;
            ENTRY(task)->object.pos[2] = work->pos.z;
            ENTRY(task)->object.ang[1] = work->ang.y;
            ENTRY(task)->shadow_ang = work->ang;
            ENTRY(task)->shadow_y = NO_FLOOR;
            /* @hack redundant: a return, where the switch's break would
             * end the function too */
            return;
        }
        break;
    case MODE_LIVE:
        /* @bug memory: reads the collision record's flags through null on
         * each step of a ring that collision_init_in_place gave none. the first
         * step's test of work->collision allows for that case. the TST is at
         * 0x29c of the object's code, and the read at 0x3c2 */
        /* @hack spelling: the assignment inside the test, where a
         * statement of its own would do */
        if ((work->collision->flags & COLLISION_TOUCHED) &&
            (other = collision_next_player(task)) != NULL) {
            work->mode = MODE_GONE;
            player = player_slot_of(other);
            if ((player == 0 && !(player_records[0]->flags & PLAYER_MISSED)) ||
                (player == 1 && !(player_records[1]->flags & PLAYER_MISSED))) {
                score_add(RING_POINTS);
                ring_add(player, 1);
                player_add_gauge(player, RING_GAUGE);
                sound_play_ring(player);
            }
            ring_sparkle(task);
        }
        if (!ring_pull(task)) {
            collision_register(task);
            if (STATE(task)->made_in_stop != 0 || !game_state.time_stopped) {
                work->ang.y += RING_TURN;
            }
        } else {
            work->scl.y = NO_FLOOR;
        }
        if (work->scl.y == NO_FLOOR || (STATE(task)->flags & RING_FLOORLESS) ||
            work->submode < 0) {
            break;
        }
        work->submode--;
        if (work->submode != -1) {
            break;
        }
        work->submode = FLOOR_WAIT;
        dx = camera_task_work->pos.x - work->pos.x;
        dy = camera_task_work->pos.y - work->pos.y;
        dz = camera_task_work->pos.z - work->pos.z;
        if (vector_length2(dx, dy, dz) < FLOOR_RANGE * FLOOR_RANGE) {
            floor = surface_probe_floor(work->pos.x, work->pos.y, work->pos.z,
                                        &work->ang);
            shift = floor - work->scl.y;
            if (njAbs(shift) < FLOOR_JUMP) {
                work->scl.y = floor;
                work->pos.y += shift;
            } else {
                work->mode = MODE_GONE;
                scattered = task_create(TASK_WORK | TASK_EXTRA, SCATTERED_LEVEL,
                                        scattered_ring_step, "Tobitiri");
                if (scattered != NULL) {
                    scattered_work = scattered->work;
                    scattered_work->pos = work->pos;
                    scattered_work->pos.y -= FLOOR_DROP;
                    scattered_work->ang.y = RANDOM_TURN();
                }
            }
        }
        break;
    case MODE_GONE:
        if (task->motion.p != NULL) {
            LINK(task)->flags |= RING_SPENT;
            LINK(task)->ring = NULL;
            task->motion.p = NULL;
        }
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        break;
    }
}

void sg_ring_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *scattered;
    struct task_work *scattered_work;
    int i;
    int player;
    Float dx;
    Float dy;
    Float dz;
    Float shift;
    Float floor;

    if (placement_try_leave(task)) {
        if (work->mode != MODE_INIT) {
            if (task->motion.p != NULL) {
                LINK(task)->flags |= RING_LEFT;
                LINK(task)->ring = NULL;
                task->motion.p = NULL;
            }
            /* @hack redundant: motion cleared again, which the test above
             * has cleared where it was set */
            task->motion.p = NULL;
        }
        return;
    }
    /* @hack spelling: MODE_INIT_LINKED labelled ahead of MODE_INIT, where
     * MODE_INIT first would do */
    switch (work->mode) {
    case MODE_INIT_LINKED:
    case MODE_INIT:
        if (work->mode != MODE_INIT_LINKED) {
            task->motion.p = NULL;
        }
        if (task->motion.p != NULL) {
            LINK(task)->flags &= ~RING_LEFT;
            LINK(task)->ring = task;
        }
#ifdef NONMATCHING
        /* this side makes the state by ring_prepare, and waits a step where it
         * can't */
        if (!ring_prepare(task, RING_NO_COLLISION)) {
            return;
        }
#else
        task->force.p = syCalloc(1, sizeof(struct ring_state));
#endif
        if (work->scl.x == RING_FOLLOWS_FLOOR) {
            work->submode = ring_floor_phase;
            ring_floor_phase++;
            if (ring_floor_phase > FLOOR_WAIT) {
                ring_floor_phase = 0;
            }
        } else {
            work->submode = -1;
        }
        work->scl.y = surface_probe_floor(work->pos.x, work->pos.y, work->pos.z,
                                          &work->ang);
        work->mode = MODE_LIVE;
        STATE(task)->spin = 0.0F;
        work->scl.x = -2.0F;
        work->scl.z = -4.0F;
        work->id = WORK_ID_RING;
        task->destructor = ring_destroy;
        task->draw = sg_ring_draw;
        break;
    case MODE_LIVE:
        player = -1;
        for (i = 0; i < GAME_PLAYERS && player_work[i] != NULL &&
                    player_work[i]->collision != NULL;
             i++) {
            if ((game_state.sg_players & (1 << i)) &&
                njDistanceP2P(&player_work[i]->collision->shapes->center,
                              &work->pos) < SG_REACH) {
                player = i;
                break;
            }
        }
        if (player >= 0) {
            work->mode = MODE_GONE;
            if ((player == 0 && !(player_records[0]->flags & PLAYER_MISSED)) ||
                (player == 1 && !(player_records[1]->flags & PLAYER_MISSED))) {
                score_add(RING_POINTS);
                ring_add(player, 1);
                player_add_gauge(player, RING_GAUGE);
                sound_play_ring(player);
            }
            ring_sparkle(task);
        }
        if (!ring_pull(task)) {
            if (STATE(task)->made_in_stop != 0 || !game_state.time_stopped) {
                work->ang.y += RING_TURN;
            }
        } else {
            work->scl.y = NO_FLOOR;
        }
        if (work->scl.y == NO_FLOOR || work->submode < 0) {
            break;
        }
        work->submode--;
        if (work->submode != -1) {
            break;
        }
        work->submode = FLOOR_WAIT;
        dx = camera_task_work->pos.x - work->pos.x;
        dy = camera_task_work->pos.y - work->pos.y;
        dz = camera_task_work->pos.z - work->pos.z;
        if (vector_length2(dx, dy, dz) < FLOOR_RANGE * FLOOR_RANGE) {
            floor = surface_probe_floor(work->pos.x, work->pos.y, work->pos.z,
                                        &work->ang);
            shift = floor - work->scl.y;
            if (njAbs(shift) < FLOOR_JUMP) {
                work->scl.y = floor;
                work->pos.y += shift;
            } else {
                work->mode = MODE_GONE;
                scattered = task_create(TASK_WORK | TASK_EXTRA, SCATTERED_LEVEL,
                                        scattered_ring_step, "Tobitiri");
                if (scattered != NULL) {
                    scattered_work = scattered->work;
                    scattered_work->pos = work->pos;
                    scattered_work->pos.y -= FLOOR_DROP;
                    scattered_work->ang.y = RANDOM_TURN();
                }
            }
        }
        break;
    case MODE_GONE:
        if (task->motion.p != NULL) {
            LINK(task)->flags |= RING_SPENT;
            LINK(task)->ring = NULL;
            task->motion.p = NULL;
        }
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        break;
    }
}

/* draw task's shadow on a floor at height y, tilted by the work's angles */
static void ring_draw_shadow(struct task *task, Float y)
{
    struct task_work *work = task->work;

    njSetTexture(&ring_shadow_texlist);
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, y + SHADOW_LIFT, work->pos.z);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njScale(NULL, ring_shadow_scale_x, 1.0F, ring_shadow_scale_z);
    njCnkEasyDrawModel(&ring_shadow_model);
    njPopMatrixEx();
}

/* draw an SG ring where the current camera's bit of game_state.sg_players
 * is set. while game_state.unk_09 isn't 0, draw it on every screen */
static void sg_ring_draw(struct task *task)
{
    struct task_work *work = task->work;

    if (!(game_state.sg_players & (1 << camera_current_index)) &&
        game_state.unk_09 == 0) {
        return;
    }
    /* @hack spelling: the two tests nested, where one test joined by && would
     * do */
    if (work->tick < SHADOW_TICKS) {
        if (work->scl.y != NO_FLOOR) {
            ring_draw_shadow(task, work->scl.y);
        }
    }
    if (objects_fog_off != 0) {
        njFogDisable();
    }
    njSetTexture(&ring_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkSimpleDrawModel(ring_model);
    njPopMatrixEx();
    if (objects_fog_off != 0) {
        njFogEnable();
    }
}

void ring_draw_model(void)
{
    njSetTexture(&ring_texlist);
    if (objects_fog_off != 0) {
        njFogDisable();
    }
    njCnkSimpleDrawModel(ring_model);
    if (objects_fog_off != 0) {
        njFogEnable();
    }
}

void ring_scatter(int slot)
{
    struct task *scattered;
    struct task_work *scattered_work;
    Float base;
    int count;
    int character;
    int i;

    base = rand() * (DEGREES_PER_TURN * RAND_SCALE);
    count = ring_count(slot);
    if (count > SCATTER_MOST) {
        count = SCATTER_MOST;
    } else if (count == 0) {
        character = player_records[slot]->character;
        if (character != PLAYER_EGGMAN_MECH && character != PLAYER_TAILS_MECH) {
            player_miss(slot);
            fun_8c05bce0();
        }
        return;
    }
    ring_add(slot, -ring_count(slot));
    for (i = 0; i < count; i++) {
        scattered = task_create(TASK_WORK | TASK_EXTRA, SCATTERED_LEVEL,
                                scattered_ring_step, "Tobitiri");
        if (scattered != NULL) {
            scattered_work = scattered->work;
            scattered_work->pos = player_work[slot]->pos;
            scattered_work->ang.y =
                ((Sint32)(i * DEGREES_PER_TURN / count) + base) *
                ANGLE_PER_DEGREE;
        }
    }
    sound_play(SCATTER_SOUND, NULL, 0, 0);
}

#define AXIS_LEAN 0.9F

/* the side of a surface_probe that dir points to. it lies on the axis along
 * which dir's part passes AXIS_LEAN either way. where two parts do, z wins
 * over y and y over x. where none does, the side is SURFACE_NEG_X */
static int vector_side(NJS_VECTOR *dir)
{
    int side = SURFACE_NEG_X;

    if (dir->x > AXIS_LEAN) {
        side = SURFACE_POS_X;
    }
    if (dir->x < -AXIS_LEAN) {
        side = SURFACE_NEG_X;
    }
    if (dir->y > AXIS_LEAN) {
        side = SURFACE_POS_Y;
    }
    if (dir->y < -AXIS_LEAN) {
        side = SURFACE_NEG_Y;
    }
    if (dir->z > AXIS_LEAN) {
        side = SURFACE_POS_Z;
    }
    if (dir->z < -AXIS_LEAN) {
        side = SURFACE_NEG_Z;
    }
    return side;
}

void scattered_ring_step(struct task *task)
{
    struct task_work *work = task->work;
    struct scattered_ring *scattered = task->extra.p;
    struct task *hit;
    /* @hack spelling: a second local for the task a ring hits, where hit
     * would do */
    struct task *other;
    int player;
    Float x;
    Float y;
    Float z;

    if (placement_try_leave(task)) {
        return;
    }
    if (ENTRY(task) != NULL) {
        ENTRY(task)->object.pos[0] = work->pos.x;
        ENTRY(task)->object.pos[1] = work->pos.y;
        ENTRY(task)->object.pos[2] = work->pos.z;
        ENTRY(task)->object.ang[1] =
            (Angle)(STATE(task)->spin * ANGLE_PER_DEGREE);
        ENTRY(task)->shadow_ang.x = work->ang.x;
        ENTRY(task)->shadow_ang.y =
            (Angle)(STATE(task)->spin * ANGLE_PER_DEGREE);
        ENTRY(task)->shadow_ang.z = work->ang.z;
        /* @bug logic: draws the shadow at a height taken from the floor,
         * which is an x or a z where gravity lies along x or z (FADD FR12,FR3
         * at 0xdbe and 0xe26 of the object's code) */
        if (work->mode != MODE_FLY) {
            ENTRY(task)->shadow_y =
                STATE(task)->rest_floor + SCATTERED_SHADOW_LIFT;
            if (work->timer & BLINK_BIT) {
                ENTRY(task)->object.evalflags &= ~NJD_EVAL_HIDE;
            } else {
                ENTRY(task)->object.evalflags |= NJD_EVAL_HIDE;
            }
        } else {
            ENTRY(task)->shadow_y = scattered->floor + SCATTERED_SHADOW_LIFT;
            if (STATE(task)->made_in_stop == 0 &&
                game_state.time_stopped != 0) {
                /* @hack spelling: the test written > CATCH_STEPS - 1, where
                 * >= CATCH_STEPS would do */
                if (work->submode > CATCH_STEPS - 1) {
                    ENTRY(task)->object.evalflags &= ~NJD_EVAL_HIDE;
                } else {
                    ENTRY(task)->object.evalflags |= NJD_EVAL_HIDE;
                }
            }
        }
    }
    switch (work->mode) {
    case MODE_LAUNCH:
#ifdef NONMATCHING
        /* this side makes the state and the shape by ring_prepare. it waits a
         * step where either can't be made */
        if (!ring_prepare(task, COLLISION_PICKUP_TARGET)) {
            return;
        }
#else
        task->force.p = syCalloc(1, sizeof(struct ring_state));
        if (task->force.p == NULL) {
            return;
        }
#endif
        STATE(task)->made_in_stop = game_state.time_stopped;
        /* @hack spelling: side and spd declared in a block of their own,
         * where declarations at the function's head would do */
        {
            int side;
            NJS_VECTOR spd;

            side = vector_side(&world_gravity);
            /* @hack spelling: each positive side labelled ahead of its
             * negative side, where the sides' own order would do */
            switch (side) {
            case SURFACE_POS_X:
                scattered->floor = work->pos.x + FAR_FLOOR;
                break;
            case SURFACE_NEG_X:
                scattered->floor = work->pos.x - FAR_FLOOR;
                break;
            case SURFACE_POS_Y:
                scattered->floor = work->pos.y + FAR_FLOOR;
                break;
            case SURFACE_NEG_Y:
            /* @hack redundant: a default that no side reaches */
            default:
                scattered->floor = work->pos.y - FAR_FLOOR;
                break;
            case SURFACE_POS_Z:
                scattered->floor = work->pos.z + FAR_FLOOR;
                break;
            case SURFACE_NEG_Z:
                scattered->floor = work->pos.z - FAR_FLOOR;
                break;
            }
            scattered->probe_wait = ring_floor_phase;
            ring_floor_phase++;
            if (ring_floor_phase > FLOOR_WAIT) {
                ring_floor_phase = 0;
            }
#ifdef NONMATCHING
            /* this side's shape, made at the launch, takes its radius */
            work->collision->radius = RING_COLLISION_RADIUS;
#else
            collision_init_in_place(task, &ring_collision_shape, 1,
                                    COLLISION_PICKUP_TARGET);
            if (work->collision != NULL) {
                work->collision->radius = RING_COLLISION_RADIUS;
            }
#endif
            work->mode = MODE_FLY;
            work->pos.x -= world_gravity.x * SCATTER_START;
            work->pos.y -= world_gravity.y * SCATTER_START;
            work->pos.z -= world_gravity.z * SCATTER_START;
            /* @hack spelling: spd.y set ahead of spd.x, where x, y, z order
             * would do */
            spd.y = SCATTER_RISE;
            spd.x = njSin(work->ang.y) * SCATTER_SPEED;
            spd.z = njCos(work->ang.y) * SCATTER_SPEED;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateZ(NULL, world_gravity_ang_z);
            njRotateX(NULL, world_gravity_ang_x);
            njCalcVector(NULL, &spd, &work->scl);
            njPopMatrixEx();
        }
        /* @hack type: the spin converted to a whole number, which
         * rand() * 2.0F already is */
        STATE(task)->spin = (Sint32)(rand() * 2.0F);
        /* @bug memory: reads and writes the collision record's flags through
         * null where collision_init_in_place gave the task none. the test above
         * allows for that case, with TST at 0xf76 of the object's code. the
         * read and the write are at 0x1036 and 0x103a. the steps of MODE_FLY
         * and MODE_REST then read the flags through null, at 0x10c8 and
         * 0x1622 */
        work->collision->flags &= ~COLLISION_TARGET;
        task->destructor = ring_destroy;
#ifdef NONMATCHING
        /* this side links the ring and its entry both ways. the chain's destroy
         * can then let go of the ring */
        ring_attach_entry(task);
#else
        task->unk_3c.p = ring_entry_add();
#endif
        if (ENTRY(task) == NULL) {
            return;
        }
        ENTRY(task)->object.pos[0] = work->pos.x;
        ENTRY(task)->object.pos[1] = work->pos.y;
        ENTRY(task)->object.pos[2] = work->pos.z;
        ENTRY(task)->object.ang[1] =
            (Angle)(STATE(task)->spin * ANGLE_PER_DEGREE);
        ENTRY(task)->shadow_ang.x = 0;
        ENTRY(task)->shadow_ang.y =
            (Angle)(STATE(task)->spin * ANGLE_PER_DEGREE);
        ENTRY(task)->shadow_ang.z = 0;
        ENTRY(task)->shadow_y = work->pos.y + RING_RADIUS;
        break;
    case MODE_FLY:
        if (STATE(task)->made_in_stop != 0 || !game_state.time_stopped) {
            STATE(task)->spin += SCATTER_SPIN;
        }
        /* @hack spelling: the assignment inside the test, where a
         * statement of its own would do */
        if ((work->collision->flags & COLLISION_TOUCHED) &&
            (hit = collision_next_player(task)) != NULL) {
            work->collision->flags &= ~COLLISION_TOUCHED;
            STATE(task)->spin = 0.0F;
            work->scl.x = -2.0F;
            work->scl.z = -4.0F;
            work->pos.y += RING_RADIUS;
            work->mode = MODE_TAKEN;
            player = player_slot_of(hit);
            if ((player == 0 && !(player_records[0]->flags & PLAYER_MISSED)) ||
                (player == 1 && !(player_records[1]->flags & PLAYER_MISSED))) {
                ring_add(player, 1);
                sound_play_ring(player);
                player_add_gauge(player, RING_GAUGE);
            }
            break;
        }
        scattered->probe_wait -= 1.0F;
        /* @hack type: probe_wait read again through a volatile view, where
         * the value just stored would do */
        /* @hack spelling: the test written 0.0F > probe_wait, where
         * probe_wait < 0.0F would do */
        if (0.0F > ((volatile struct scattered_ring *)scattered)->probe_wait) {
            struct surface_probe probe;
            int side;

            side = vector_side(&world_gravity);
            probe.pos = work->pos;
            probe.pos.x -= world_gravity.x * RING_RADIUS;
            probe.pos.y -= world_gravity.y * RING_RADIUS;
            probe.pos.z -= world_gravity.z * RING_RADIUS;
            surface_probe_sides(&probe);
            if (probe.sides[side].found) {
                scattered->floor = probe.sides[side].pos;
                work->ang.x = probe.sides[side].ang_x;
                work->ang.z = probe.sides[side].ang_z;
            } else {
                /* @hack spelling: each positive side labelled ahead of its
                 * negative side, where the sides' own order would do */
                switch (side) {
                case SURFACE_POS_X:
                    scattered->floor = work->pos.x + FAR_FLOOR;
                    break;
                case SURFACE_NEG_X:
                    scattered->floor = work->pos.x - FAR_FLOOR;
                    break;
                case SURFACE_POS_Y:
                    scattered->floor = work->pos.y + FAR_FLOOR;
                    break;
                case SURFACE_NEG_Y:
                /* @hack redundant: a default that no side reaches */
                default:
                    scattered->floor = work->pos.y - FAR_FLOOR;
                    break;
                case SURFACE_POS_Z:
                    scattered->floor = work->pos.z + FAR_FLOOR;
                    break;
                case SURFACE_NEG_Z:
                    scattered->floor = work->pos.z - FAR_FLOOR;
                    break;
                }
            }
            scattered->probe_wait = PROBE_STEPS;
        }
        if (STATE(task)->made_in_stop != 0 || !game_state.time_stopped) {
            NJS_POINT3 floor_point;
            int side;
            NJS_POINT3 local_pos;
            /* @hack redundant: 12 bytes the code never reads, which keep
             * the frame's layout */
            Uint32 unused[3];
            NJS_VECTOR local_spd;

            work->timer++;
            if (work->timer > FLY_STEPS) {
                work->mode = MODE_REST;
                STATE(task)->rest_floor = scattered->floor;
                work->scl.x = 0.0F;
                work->scl.z = 0.0F;
                work->timer = 0;
                break;
            }
            side = vector_side(&world_gravity);
            /* @hack spelling: each positive side labelled ahead of its
             * negative side, where the sides' own order would do */
            switch (side) {
            case SURFACE_POS_X:
            case SURFACE_NEG_X:
                floor_point.x = scattered->floor;
                floor_point.y = work->pos.y;
                floor_point.z = work->pos.z;
                break;
            case SURFACE_POS_Y:
            case SURFACE_NEG_Y:
            /* @hack redundant: a default that no side reaches */
            default:
                floor_point.x = work->pos.x;
                floor_point.y = scattered->floor;
                floor_point.z = work->pos.z;
                break;
            case SURFACE_POS_Z:
            case SURFACE_NEG_Z:
                floor_point.x = work->pos.x;
                floor_point.y = work->pos.y;
                floor_point.z = scattered->floor;
                break;
            }
            /* @hack spelling: the negated pull subtracted, where
             * world_gravity.y * FALL_GRAVITY added would do */
            work->scl.y -= -(world_gravity.y * FALL_GRAVITY);
            if (work->scl.y < -FALL_FASTEST) {
                work->scl.y = -FALL_FASTEST;
            }
            if (work->scl.y > FALL_FASTEST) {
                work->scl.y = FALL_FASTEST;
            }
            work->pos.y += work->scl.y;
            /* @hack spelling: the negated pull subtracted, where
             * world_gravity.x * FALL_GRAVITY added would do */
            work->scl.x -= -(world_gravity.x * FALL_GRAVITY);
            if (work->scl.x < -FALL_FASTEST) {
                work->scl.x = -FALL_FASTEST;
            }
            if (work->scl.x > FALL_FASTEST) {
                work->scl.x = FALL_FASTEST;
            }
            work->pos.x += work->scl.x;
            /* @hack spelling: the negated pull subtracted, where
             * world_gravity.z * FALL_GRAVITY added would do */
            work->scl.z -= -(world_gravity.z * FALL_GRAVITY);
            if (work->scl.z < -FALL_FASTEST) {
                work->scl.z = -FALL_FASTEST;
            }
            if (work->scl.z > FALL_FASTEST) {
                work->scl.z = FALL_FASTEST;
            }
            work->pos.z += work->scl.z;
            work->scl.x *= FALL_DRAG;
            work->scl.y *= FALL_DRAG;
            work->scl.z *= FALL_DRAG;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateX(NULL, -work->ang.x);
            njRotateZ(NULL, -work->ang.z);
            njTranslate(NULL, -floor_point.x, -floor_point.y, -floor_point.z);
            njCalcPoint(NULL, &work->pos, &local_pos);
            njCalcVector(NULL, &work->scl, &local_spd);
            if (local_pos.y < RING_RADIUS) {
                local_pos.y = RING_RADIUS;
                local_spd.y = njAbs(local_spd.y) * BOUNCE;
                local_spd.x *= BOUNCE_DRAG;
                local_spd.z *= BOUNCE_DRAG;
                /* @hack spelling: side set to -1 to mark the bounce, where the
                 * bounce's own block would do */
                side = -1;
            }
            if (side == -1) {
                njUnitMatrix(NULL);
                njTranslate(NULL, floor_point.x, floor_point.y, floor_point.z);
                njRotateZ(NULL, work->ang.z);
                njRotateX(NULL, work->ang.x);
                njCalcPoint(NULL, &local_pos, &work->pos);
                njCalcVector(NULL, &local_spd, &work->scl);
            }
            njPopMatrixEx();
            /* @hack redundant: the block's test made again, which nothing
             * in the block can change */
            if (STATE(task)->made_in_stop != 0 || !game_state.time_stopped) {
                work->submode++;
            }
        }
        /* @hack spelling: the test written > CATCH_STEPS - 1, where >=
         * CATCH_STEPS would do */
        if (work->submode > CATCH_STEPS - 1) {
            work->submode = CATCH_STEPS;
            y = work->pos.y;
            work->pos.y += RING_RADIUS;
            collision_register(task);
            work->pos.y = y;
        }
        break;
    case MODE_REST:
        if (STATE(task)->made_in_stop != 0 || !game_state.time_stopped) {
            STATE(task)->spin += SCATTER_SPIN;
        }
        /* @hack spelling: the assignment inside the test, where a
         * statement of its own would do */
        if ((work->collision->flags & COLLISION_TOUCHED) &&
            (other = collision_next_player(task)) != NULL) {
            work->collision->flags &= ~COLLISION_TOUCHED;
            work->mode = MODE_TAKEN;
            STATE(task)->spin = 0.0F;
            work->scl.x = -2.0F;
            work->scl.z = -4.0F;
            player = player_slot_of(other);
            if ((player == 0 && !(player_records[0]->flags & PLAYER_MISSED)) ||
                (player == 1 && !(player_records[1]->flags & PLAYER_MISSED))) {
                ring_add(player, 1);
                sound_play_ring(player);
                player_add_gauge(player, RING_GAUGE);
            }
            break;
        }
        /* @hack spelling: the pos saved, moved and restored y first, where
         * x, y, z order would do */
        y = work->pos.y;
        x = work->pos.x;
        z = work->pos.z;
        work->pos.y += RING_RADIUS;
        work->pos.x += work->scl.x;
        work->pos.z += work->scl.z;
        collision_register(task);
        work->pos.y = y;
        work->pos.x = x;
        work->pos.z = z;
        if (STATE(task)->made_in_stop != 0 || !game_state.time_stopped) {
            work->timer++;
            if (work->timer >= REST_STEPS) {
                work->mode = MODE_EXPIRED;
                break;
            }
        }
        /* @hack redundant: a return, where the switch's break would end
         * the function too */
        return;
    case MODE_TAKEN:
        ring_sparkle(task);
        /* fall through */
    case MODE_EXPIRED:
        task_destroy(task);
        break;
    }
}

/* the destructor of each kind of ring */
static void ring_destroy(struct task *task)
{
    /* @bug memory: writes into the ring's chain entry after
     * ring_chain_destroy has freed it, where the chain's task is destroyed
     * first. task_destroy_all destroys the chain's CHAIN_LEVEL ahead of a
     * scattered ring's SCATTERED_LEVEL (MOV.L R0,@R4 at 0x1c2a of the object's
     * code) */
    if (ENTRY(task) != NULL) {
        ring_entry_remove(ENTRY(task));
    }
    if (task->motion.p != NULL) {
        LINK(task)->flags &= ~RING_LEFT;
        LINK(task)->ring = NULL;
        task->motion.p = NULL;
    }
    syFree(task->force.p);
    task->force.p = NULL;
    /* @hack redundant: motion cleared again, which the test above has
     * cleared where it was set */
    task->motion.p = NULL;
}

/* pull task's ring toward the first collision shape of the nearer player
 * with PLAYER_MAGNETIC_BARRIER set. the ring moves where it lies within
 * PULL_REACH of that player, or has moved before. its work's timer counts
 * the moves.
 *
 * ring_pull returns TRUE where it picks a player, and then registers the
 * ring's collision. it returns FALSE where no player is magnetic. it also
 * returns FALSE where two are, at the same distance from the ring */
static Bool ring_pull(struct task *task)
{
    struct task_work *work = task->work;
    struct player *record;
    NJS_POINT3 target;
    Float distance0;
    Float distance1;
    Float distance;
    Float speed;
    int magnetic;
    int player;
    int i;

    /* @hack spelling: magnetic cleared in the loop's start, where a
     * statement ahead of the loop would do */
    for (i = 0, magnetic = 0; i < GAME_PLAYERS; i++) {
        if (player_records[i] != NULL &&
            (player_records[i]->flags & PLAYER_MAGNETIC_BARRIER)) {
            magnetic |= SLOT_BIT(i);
        }
    }
    if (magnetic == 0) {
        return FALSE;
    }
    if (magnetic == (SLOT_BIT(0) | SLOT_BIT(1))) {
        distance0 = njDistanceP2P(&work->pos, &player_work[0]->pos);
        distance1 = njDistanceP2P(&work->pos, &player_work[1]->pos);
        if (distance1 > distance0) {
            player = 0;
        } else if (distance0 > distance1) {
            player = 1;
        } else {
            return FALSE;
        }
    } else if (magnetic == SLOT_BIT(0)) {
        player = 0;
        /* @hack redundant: the test of magnetic == SLOT_BIT(1) and its else,
         * which no other value of magnetic reaches */
    } else if (magnetic == SLOT_BIT(1)) {
        player = 1;
    } else {
        return FALSE;
    }
    /* @hack spelling: the player's record held in record ahead of the pull,
     * where player_records[player] at its use would do */
    record = player_records[player];
    distance = njDistanceP2P(&work->pos, &player_work[player]->pos);
    if (distance < PULL_REACH || work->timer >= 1) {
        /* @bug memory: reads the player's collision shapes through null where
         * the player's work has no collision record. sg_ring_step tests a
         * player's collision record for null at 0x756 of the object's code.
         * ring_pull reads through it at 0x187e */
#ifdef NONMATCHING
        /* this side repairs the tag above: a player without a collision
         * record pulls the ring toward its pos */
        if (player_work[player]->collision != NULL) {
            target = player_work[player]->collision->shapes->center;
        } else {
            target = player_work[player]->pos;
        }
#else
        target = player_work[player]->collision->shapes->center;
#endif
        if (distance > PULL_REACH) {
            distance = PULL_REACH;
        }
        speed = distance * PULL_RATE;
        if (speed > PULL_FASTEST) {
            speed = PULL_FASTEST;
        } else if (speed < PULL_SLOWEST) {
            speed = PULL_SLOWEST;
        }
        speed *= (njScalor(&record->spd) * PULL_PER_SPEED) + 1.0F;
        point_toward(&work->pos, &target, speed, &work->pos);
        work->timer++;
    }
    STATE(task)->spin += PULL_SPIN;
    collision_register(task);
    return TRUE;
}

void ring_scatter_at(Float x, Float y, Float z, int count)
{
    struct task *scattered;
    struct task_work *scattered_work;
    Float base;
    int i;

    base = rand() * (DEGREES_PER_TURN * RAND_SCALE);
    for (i = 0; i < count; i++) {
        scattered = task_create(TASK_WORK | TASK_EXTRA, SCATTERED_LEVEL,
                                scattered_ring_step, "Tobitiri");
        if (scattered != NULL) {
            scattered_work = scattered->work;
            scattered_work->pos.x = x;
            scattered_work->pos.y = y;
            scattered_work->pos.z = z;
            scattered_work->ang.y =
                ((Sint32)(i * DEGREES_PER_TURN / count) + base) *
                ANGLE_PER_DEGREE;
        }
    }
}

Uint8 *ring_build(Uint8 *buf)
{
    NJS_CNK_OBJECT shadow;

    if (ring_built) {
        return buf;
    }
    if (objects_fog_off != 0) {
        njFogDisable();
    }
    njSetTexture(&ring_texlist);
    ring_far_build = model_build(&buf, ring_object[0].model);
    ring_middle_build = model_build(&buf, ring_middle_object[0].model);
    ring_near_build = model_build(&buf, ring_near_object[0].model);
    njSetTexture(&ring_shadow_texlist);
    shadow = ring_shadow_object;
    shadow.evalflags &= ~NJD_EVAL_UNIT_SCL;
    shadow.pos[0] = 0.0F;
    shadow.pos[1] = 0.0F;
    shadow.pos[2] = 0.0F;
    shadow.scl[0] = ring_shadow_scale_x;
    shadow.scl[1] = 0.0F;
    shadow.scl[2] = ring_shadow_scale_z;
    ring_shadow_build = object_tree_build(&buf, &shadow);
    if (ring_far_build != NULL && ring_middle_build != NULL &&
        ring_near_build != NULL && ring_shadow_build != NULL) {
        ring_built = TRUE;
    }
    if (objects_fog_off != 0) {
        njFogEnable();
    }
    return buf;
}

/* a new entry at the head of the chain, a copy of ring_entry_template. its
 * node's NJD_EVAL_UNIT_POS and NJD_EVAL_UNIT_ANG are cleared, so that its pos
 * and ang apply. it starts the chain's task where there is none. it returns
 * null where the heap has no room for the entry */
static struct ring_entry *ring_entry_add(void)
{
    struct ring_entry *entry;

    entry = syCalloc(1, sizeof(struct ring_entry));
    /* @hack spelling: the failure's return in an else, where a return ahead
     * of the work would do */
    if (entry != NULL) {
        ring_chain_start();
#ifdef NONMATCHING
        /* this side frees the entry and returns null where the chain's task
         * can't be made */
        if (ring_chain_task == NULL) {
            syFree(entry);
            return NULL;
        }
#endif
        entry->object = ring_entry_template;
        entry->object.evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
        entry->shadow_y = NO_FLOOR;
    } else {
        return NULL;
    }
    if (ring_chain != NULL) {
        entry->object.sibling = ring_chain;
    }
    ring_chain = &entry->object;
    return entry;
}

/* hide entry and clear its model, which marks the entry for the chain's
 * task to free. entry may be null */
static void ring_entry_remove(struct ring_entry *entry)
{
    if (entry != NULL) {
#ifdef NONMATCHING
        /* this side clears the entry's owner, the ring_state that ring_destroy
         * frees next. so ring_chain_destroy writes through no freed state */
        entry->owner = NULL;
#endif
        entry->object.evalflags |= NJD_EVAL_HIDE;
        entry->object.model = NULL;
        ring_entries_removed = TRUE;
    }
}

/* the chain task's destructor: free every entry */
static void ring_chain_destroy(struct task *task)
{
    NJS_CNK_OBJECT *next;

    while (ring_chain != NULL) {
        next = ring_chain->sibling;
#ifdef NONMATCHING
        /* this side clears each ring's link to an entry the chain frees */
        {
            struct ring_entry *entry = (struct ring_entry *)ring_chain;
            if (entry->owner != NULL) {
                entry->owner->entry = NULL;
            }
        }
#endif
        syFree(ring_chain);
        ring_chain = next;
    }
    ring_entries_removed = FALSE;
    if (ring_chain_task == task) {
        ring_chain_task = NULL;
    }
}

/* the chain task's step. it frees each entry ring_entry_remove left without a
 * model, and destroys the task once the chain is empty */
static void ring_chain_step(struct task *task)
{
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *prev;
    NJS_CNK_OBJECT *next;

    if (ring_entries_removed) {
        prev = NULL;
        for (object = ring_chain; object != NULL; object = next) {
            next = object->sibling;
            if (object->model == NULL) {
#ifdef NONMATCHING
                /* this side unlinks the object before it frees it. the matching
                 * side frees it first, then reads the chain through it */
                if (prev == NULL) {
                    ring_chain = next;
                } else {
                    prev->sibling = next;
                }
                syFree(object);
#else
                syFree(object);
                if (prev != NULL) {
                    prev->sibling = next;
                }
                if (ring_chain == object) {
                    ring_chain = next;
                }
#endif
            } else {
                prev = object;
            }
        }
        ring_entries_removed = FALSE;
    }
    if (ring_chain == NULL) {
        task_destroy(task);
    }
}

/* the chain task's draw, which draws nothing while game_state.hidden_cameras
 * isn't 0. while game_state.builds_drawn is 0, it draws the chain under
 * ring_chain_root, and then each shadow. otherwise each ring draws from the
 * build its depth picks, and each shadow from the shadow's build. until
 * ring_build has made the builds, each draw compiles ring_object and
 * ring_shadow_object into WORK_AREA instead */
static void ring_chain_draw(struct task *task)
{
    NJS_CNK_OBJECT root;
    /* @hack redundant: 12 bytes the code never reads, which keep the
     * frame's layout */
    Uint32 unused[3];
    NJS_POINT3 view;
    NJS_DIRECT_HEAD *build;
    NJS_CNK_OBJECT *object;
    struct ring_entry *entry;
    Float depth;
    Float *pos;

    if (game_state.hidden_cameras != 0) {
        return;
    }
    if (objects_fog_off != 0) {
        njFogDisable();
    }
    if (!game_state.builds_drawn) {
        root = ring_chain_root;
        root.child = ring_chain;
        njSetTexture(&ring_texlist);
        njCnkSimpleDrawObject(&root);
        /* @hack redundant: a read of njTranslate's address that never
         * runs */
        if (0) {
            if (&njTranslate) {
            }
        }
        /* @hack redundant: a read of njRotateY's address that never runs */
        if (0) {
            if (&njRotateY) {
            }
        }
        njSetTexture(&ring_shadow_texlist);
        /* @hack spelling: object set ahead of njPushMatrixEx, where the
         * loop's start would do */
        object = ring_chain;
        njPushMatrixEx();
        for (; object != NULL; object = object->sibling) {
            entry = (struct ring_entry *)object;
            if (entry->shadow_y != NO_FLOOR && entry->shadow_y != -NO_FLOOR) {
                /* @hack spelling: the node's place held in pos, where
                 * object->pos would do */
                pos = object->pos;
                njTranslate(NULL, pos[0], entry->shadow_y + SHADOW_LIFT,
                            pos[2]);
                njRotateZ(NULL, entry->shadow_ang.z);
                njRotateX(NULL, entry->shadow_ang.x);
                njRotateY(NULL, entry->shadow_ang.y);
                njScale(NULL, ring_shadow_scale_x, 1.0F, ring_shadow_scale_z);
                njCnkEasyDrawModel(&ring_shadow_model);
                matrix_load_parent();
            }
        }
        njPopMatrixEx();
    } else {
        njSetTexture(&ring_texlist);
        njPushMatrixEx();
        if (ring_built) {
            for (object = ring_chain; object != NULL;
                 object = object->sibling) {
                if (!(object->evalflags & NJD_EVAL_HIDE)) {
                    njCalcPoint(NULL, (NJS_POINT3 *)object->pos, &view);
                    /* @hack spelling: view.z held in depth, where view.z
                     * would do */
                    depth = view.z;
                    if (depth > BEHIND_DEPTH) {
                        njTranslateEx((NJS_VECTOR *)object->pos);
                        njRotateY(NULL, object->ang[1]);
                        if (depth < NEAR_DEPTH) {
                            njDirectDrawObject(ring_near_build);
                        } else if (depth < MIDDLE_DEPTH) {
                            njDirectDrawObject(ring_middle_build);
                        } else {
                            njDirectDrawObject(ring_far_build);
                        }
                        matrix_load_parent();
                    }
                }
            }
        } else {
            build = (NJS_DIRECT_HEAD *)WORK_AREA;
            root = ring_object[0];
            root.evalflags = NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG |
                             NJD_EVAL_UNIT_SCL | NJD_EVAL_BREAK;
            root.pos[0] = 0.0F;
            root.pos[1] = 0.0F;
            root.pos[2] = 0.0F;
            njCnkDirectObjectCompile(&root, build);
            /* @hack redundant: a block that never runs */
            if (0) {
            }
            /* @hack spelling: a do-while under a test of its first node,
             * where a for loop would do */
            object = ring_chain;
            if (object != NULL) {
                do {
                    if (!(object->evalflags & NJD_EVAL_HIDE)) {
                        njTranslateEx((NJS_VECTOR *)object->pos);
                        njRotateY(NULL, object->ang[1]);
                        njDirectDrawObject(build);
                        matrix_load_parent();
                    }
                } while ((object = object->sibling) != NULL);
            }
        }
        njSetTexture(&ring_shadow_texlist);
        if (ring_built) {
            build = ring_shadow_build;
        } else {
            /* @hack spelling: WORK_AREA passed through a local, where
             * build = WORK_AREA would do */
            NJS_DIRECT_HEAD *area = (NJS_DIRECT_HEAD *)WORK_AREA;

            build = area;
            root = ring_shadow_object;
            /* @hack spelling: NJD_EVAL_UNIT_SCL set and then cleared, where
             * flags without it would do */
            root.evalflags = NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG |
                             NJD_EVAL_UNIT_SCL | NJD_EVAL_BREAK;
            root.evalflags &= ~NJD_EVAL_UNIT_SCL;
            root.pos[0] = 0.0F;
            root.pos[1] = 0.0F;
            root.pos[2] = 0.0F;
            root.scl[0] = ring_shadow_scale_x;
            root.scl[1] = 0.0F;
            root.scl[2] = ring_shadow_scale_z;
            njCnkDirectObjectCompile(&root, build);
        }
        /* @hack spelling: a do-while under a test of its first node, where a
         * for loop would do */
        object = ring_chain;
        if (object != NULL) {
            do {
                entry = (struct ring_entry *)object;
                if (entry->shadow_y != NO_FLOOR &&
                    entry->shadow_y != -NO_FLOOR) {
                    njTranslate(NULL, object->pos[0],
                                entry->shadow_y + SHADOW_LIFT, object->pos[2]);
                    njRotateZ(NULL, entry->shadow_ang.z);
                    njRotateX(NULL, entry->shadow_ang.x);
                    njRotateY(NULL, entry->shadow_ang.y);
                    njDirectDrawObject(build);
                    matrix_load_parent();
                }
            } while ((object = object->sibling) != NULL);
        }
        njPopMatrixEx();
    }
    if (objects_fog_off != 0) {
        njFogEnable();
    }
}

/* create the chain's task, where there is none */
static void ring_chain_start(void)
{
    struct task *task;

    if (ring_chain_task == NULL) {
        task = task_create(TASK_WORK, CHAIN_LEVEL, ring_chain_step,
                           "RingModelExec");
        if (task != NULL) {
            ring_chain_task = task;
            /* @hack redundant: the step set again, which task_create has
             * set */
            task->step = ring_chain_step;
            task->draw = ring_chain_draw;
            task->destructor = ring_chain_destroy;
        }
    }
}
