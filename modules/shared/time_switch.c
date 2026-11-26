#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "draw/chunk.h"
#include "effect/particle.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/flash.h"
#include "shared/stage_util.h"
#include "shared/time_switch.h"
#include "shared/time_switch_art.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"

/* a switch's cap, in its task's extra: its tilt, its center's offset from
 * rest, and its speed */
struct switch_cap {
    struct angles tilt;
    NJS_POINT3 offset;
    NJS_VECTOR speed;
    Sint32 unk_24;
};
#define CAP(task) ((struct switch_cap *)(task)->extra.p)

/* the switch's modes, in its work's mode */
enum switch_mode { SWITCH_INIT, SWITCH_READY };

/* the cap's center rests CAP_HEIGHT above the base's pos */
#define CAP_HEIGHT 8.34F
#define CAP_TRAVEL 8.0F
/* the spring pulls the cap toward its rest point. each step, the cap's speed
 * loses SPRING times its offset from that point. the spring acts only while
 * the offset is longer than SPRING_SLACK */
#define SPRING 0.01875F
#define SPRING_SLACK 0.001F
/* each step keeps CAP_DAMPING of the cap's speed. the speed then gains
 * CAP_FOLLOW times the push. the push is the mean place the cap's pushers
 * push it to, less its place before them. the speed's length is then held to
 * CAP_SPEED_MAX */
#define CAP_DAMPING 0.95F
#define CAP_FOLLOW 0.2F
#define CAP_SPEED_MAX 25.0F
/* the players of the slots below PUSHING_PLAYERS push the cap, up to the
 * first empty slot */
#define PUSHING_PLAYERS 2
/* a player keeps the cap's center its radius plus CAP_RADIUS away */
#define CAP_RADIUS 3.5F
#define PUSH_DEPTH 20.0F
/* the cap is still where its offset is below REST_OFFSET and its speed's
 * square below REST_SPEED2. a press sets the countdown to its start and keeps
 * it from running. it runs again once the cap has been still REST_STEPS
 * steps, which REST_LEFT(work) counts down */
#define REST_OFFSET 1.0F
#define REST_SPEED2 0.25F
#define REST_STEPS 10
#define REST_LEFT(work) ((work)->tick)
#define SWITCH_DISTANCE2(a, b)                                                 \
    ((((a).x - (b).x) * ((a).x - (b).x)) +                                     \
     (((a).y - (b).y) * ((a).y - (b).y)) +                                     \
     (((a).z - (b).z) * ((a).z - (b).z)))

/* the cap's bob above its offset, from 0 to 2 * BOB_AMPLITUDE. the bob's angle
 * turns 1 << BOB_SHIFT a step */
#define BOB_SHIFT 9
#define BOB_AMPLITUDE 1.5F
#define CAP_BOB()                                                              \
    ((njCos(game_state.steps << BOB_SHIFT) + 1.0F) * BOB_AMPLITUDE)
/* the cap turns about y 1 << CAP_SPIN_SHIFT a step. its model stands
 * CAP_LIFT above the point it tilts about */
#define CAP_SPIN_SHIFT 8
#define CAP_LIFT 1.66F
/* the fast and slow rings' own turns about y a step. the slow ring draws
 * SLOW_RING_LIFT above the other two */
#define FAST_RING_TURN (-240)
#define SLOW_RING_TURN (-2)
#define SLOW_RING_LIFT 0.01F

/* the flash a strike makes at the target */
#define FLASH_SCALE 20.0F
#define FLASH_SPARKS 3
#define FLASH_DEPTH_BIAS 30.0F

#define TICK_SOUND 0x800e
#define END_SOUND 0x1002
/* the countdown's task level */
#define TIMER_LEVEL 2
/* the lamp's glow is GLOW_FULL while lit. once out, it falls GLOW_FADE a
 * step, and stops at GLOW_DIM */
#define GLOW_FULL 255
#define GLOW_FADE 30
#define GLOW_DIM 20
/* the lamp waits 1 step between changes once BLINK_STEPS or fewer steps are
 * left. before that, a lit lamp waits LIT_WAIT times the steps left above
 * BLINK_STEPS, plus 1. a lamp that is out waits DARK_WAIT times those steps,
 * plus 1 */
#define BLINK_STEPS 40
#define LIT_WAIT 0.06F
#define DARK_WAIT 0.04F

/* the lowest the cap's center sinks above the base's pos */
static Float cap_lowest = 8.3F;
/* the countdown's task, or null */
static struct task *timer_task = NULL;
/* the steps left of the countdown, which every switch shares. previous_time
 * is time_left as the timer's last step left it */
static Sint32 time_left = 0;
static Sint32 previous_time = 0;
/* the steps the lamp waits before it next changes */
static Sint32 blink_wait = 0;
static Uint8 glow = GLOW_FULL;
static Bool lit = FALSE;
/* whether the countdown runs, which a press of the cap stops */
static Bool counting = TRUE;
/* the square of the distance from player 0 within which a switch moves its
 * cap and draws */
static Float switch_range2 = 400.0F * 400.0F;
NJS_TEXLIST *time_switch_texlists[] = {&time_switch_cap_texlist,
                                       &time_switch_base_texlist,
                                       &time_switch_effect_texlist, NULL};
static struct collision_shape base_shape = {
#include "shared/time_switch/base_shape.inc"
};
/* the volume of the countdown's ticks and of its end */
static Sint32 timer_volume = 127;
/* a player pushes the cap from player_y_offset above its pos */
static Float player_y_offset = 7.5F;
/* a push of the cap's center past press_fraction of CAP_TRAVEL is a press */
static Float press_fraction = 0.1F;
/* the cap's vertex colors: light_color at a glow of 0, dark_color at
 * GLOW_FULL */
static Uint32 dark_color = 0xff30304f;
static Uint32 light_color = 0xffeeeedd;
static struct collision_shape target_shape = {
#include "shared/time_switch/target_shape.inc"
};

static void time_switch_init(struct task *task);
static void cap_alloc(struct task *task);
static void cap_update(struct task *task);
static void switch_ready(struct task *task);
static void switch_draw(struct task *task);
static void switch_destroy(struct task *task);
static void cap_release(struct task *task);
static void target_init(struct task *task);
static void target_destroy(struct task *task);
static void target_step(struct task *task);
static void set_lamp(struct task *task, Bool on);
static void blink_schedule(struct task *task);
static void timer_step(struct task *task);
static void timer_destroy(struct task *task);
static void timer_create(void);

void time_switch_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: the set editor's readout and calls of the init and the
     * cap, in a block no path reaches */
    if (0) {
        switch (work->submode) {
        case 0:
            time_switch_init(task);
            break;
        case 1:
            cap_release(task);
            cap_alloc(task);
            break;
        }
        njPrintC(NJM_LOCATION(17, 16), "<- ANGLE X");
        njPrintC(NJM_LOCATION(17, 17), "<- ANGLE Y");
        njPrintC(NJM_LOCATION(17, 18), "<- ANGLE Z");
        njPrintC(NJM_LOCATION(17, 21), "<- TIMER");
    } else {
        switch (work->mode) {
        case SWITCH_INIT:
            time_switch_init(task);
            break;
        case SWITCH_READY:
            switch_ready(task);
            break;
        }
    }
}

static void time_switch_init(struct task *task)
{
    struct task_work *work = task->work;
    if (placement_try_leave(task)) {
        return;
    }
#ifdef NONMATCHING
    /* this side destroys the switch where the cap, its shape or its target
     * can't be made. target_init then makes the timer and gives the switch its
     * draw and destructor, once the target is whole */
    cap_alloc(task);
    if (task->extra.p == NULL ||
        !collision_init(task, &base_shape, 1, COLLISION_OBJECT) ||
        task_create_child(TASK_WORK, target_init, task) == NULL) {
        task_destroy(task);
        return;
    }
    work->mode = SWITCH_READY;
#else
    if (timer_task == NULL) {
        timer_create();
    }
    task->destructor = switch_destroy;
    task->draw = switch_draw;
    cap_alloc(task);
    work->mode = SWITCH_READY;
    /* @hack spelling: the comma between the two calls, where two
     * statements would do */
    collision_init(task, &base_shape, 1, COLLISION_OBJECT),
        task_create_child(TASK_WORK, target_init, task);
#endif
}

static void cap_alloc(struct task *task)
{
    task->extra.p = syCalloc(1, sizeof(struct switch_cap));
    REST_LEFT(task->work) = 0;
#ifdef NONMATCHING
#else
    /* @hack redundant: the cap's fields set to 0, which syCalloc has
     * cleared where it succeeds */
    CAP(task)->tilt.x = 0;
    CAP(task)->tilt.y = 0;
    CAP(task)->tilt.z = 0;
    CAP(task)->unk_24 = 0;
    CAP(task)->offset.x = 0;
    CAP(task)->offset.y = 0;
    CAP(task)->offset.z = 0;
    CAP(task)->speed.x = 0;
    CAP(task)->speed.y = 0;
    CAP(task)->speed.z = 0;
#endif
}

/* cap is the cap's center, and player the player's point. return cap pushed
 * out to reach from player, where the player lies nearer than reach. a player
 * above the center pushes it along the line between them. a player below
 * pushes it only across. the player's depth below the center counts toward
 * their distance only past PUSH_DEPTH */
#ifdef NONMATCHING
/* this side returns the point by value, as its repair of the bug tagged at the
 * return needs */
static NJS_POINT3 cap_push(NJS_POINT3 *cap, NJS_POINT3 *player, Float reach)
#else
static NJS_POINT3 *cap_push(NJS_POINT3 *cap, NJS_POINT3 *player, Float reach)
#endif
{
    NJS_POINT3 pushed;
    Float xy2;
    Float reach2;
    Float x;
    Float y;
    Float z;
    Float px;
    Float py;
    Float pz;
    Float dx;
    Float dy;
    Float dz;
    Float dx2;
    Float dz2;
    Float distance2;
    Float distance;
    Float scale;
    reach2 = reach * reach;
    x = cap->x;
    y = cap->y;
    z = cap->z;
    px = player->x;
    py = player->y;
    pz = player->z;
    dx = px - x;
    dy = py - y;
    dz = pz - z;
    dx2 = dx * dx;
    dz2 = dz * dz;
    if (dy > 0.0F) {
        distance2 = dx2 + (dy * dy) + dz2;
        distance = njSqrt(distance2);
        if (distance2 >= reach2 && distance2 != 0.0F) {
            pushed.x = x;
            pushed.y = y;
            pushed.z = z;
        } else {
            scale = reach / distance;
            pushed.x = px - (dx * scale);
            pushed.y = py - (dy * scale);
            pushed.z = pz - (dz * scale);
        }
    } else {
        dy = njAbs(dy);
        xy2 = dx2;
        if (!(dy < PUSH_DEPTH)) {
            dy += -PUSH_DEPTH;
            xy2 += dy * dy;
        }
        distance2 = xy2 + dz2;
#ifdef NONMATCHING
        /* this side repairs the bug tagged below. a player at the cap's center
         * leaves the cap where it is */
        if (distance2 == 0.0F) {
            return *cap;
        }
#endif
        distance = njSqrt(distance2);
        if (distance2 >= reach2 && distance2 != 0.0F) {
            pushed.x = x;
            pushed.y = y;
            pushed.z = z;
        } else {
            /* @bug arithmetic: divides by a distance of 0 for a player at the
             * cap's x and z, level with its center or no more than PUSH_DEPTH
             * below. the push's x and z then come out NaN (the FDIV at 0x286
             * of the object's code) */
            scale = reach / distance;
            pushed.x = px - (dx * scale);
            pushed.y = y;
            pushed.z = pz - (dz * scale);
        }
    }
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side: it returns the
     * point, not its address */
    return pushed;
#else
    /* @bug memory: returns the address of pushed, whose frame the return
     * ends. cap_update copies the point at 0x4ee of the object's code,
     * before any call can reuse the frame */
    return &pushed; /* NOLINT(clang-diagnostic-return-stack-address) */
#endif
}

/* @hack spelling: timer_reset, a function for one store, where the store in
 * cap_update would do */
static void timer_reset(struct task *task)
{
    time_left = task->work->scl.z;
}

/* move the cap a step. a spring pulls its center toward rest, and the
 * players push it out of their reach. a press sets the countdown to its start
 * and keeps it from running, as REST_STEPS says */
static void cap_update(struct task *task)
{
    struct task_work *work;
    struct switch_cap *cap;
    NJS_POINT3 *player_point;
    NJS_POINT3 center;
    NJS_POINT3 sum;
    NJS_POINT3 relative;
    NJS_POINT3 pushed;
    NJS_POINT3 position;
    Float x;
    Float y;
    Float z;
    Float vx;
    Float vy;
    Float vz;
    Float length;
    Float scale;
    int i;
    int players;
    cap = CAP(task);
    work = task->work;
    vx = cap->speed.x;
    vy = cap->speed.y;
    vz = cap->speed.z;
    x = cap->offset.x;
    y = cap->offset.y;
    z = cap->offset.z;
    /* @hack spelling: x = x + vx, where x += vx as y's would do */
    x = x + vx;
    y += vy;
    z += vz;
    length = njSqrt((x * x) + (y * y) + (z * z));
    if (length > SPRING_SLACK) {
        scale = (length * SPRING) / length;
        vx += -(x * scale);
        vz += -(z * scale);
        vy += -(y * scale);
    }
    if (REST_OFFSET > length &&
        (vx * vx) + (vy * vy) + (vz * vz) < REST_SPEED2 &&
        REST_LEFT(task->work) > 0) {
        if (--REST_LEFT(task->work) == 0) {
            counting = TRUE;
        }
    }
    if (length > press_fraction * CAP_TRAVEL) {
        REST_LEFT(task->work) = REST_STEPS;
        timer_reset(task);
        counting = FALSE;
    }
    if (length > CAP_TRAVEL) {
        scale = CAP_TRAVEL / length;
        /* @hack spelling: x and z scaled as x = x * scale, where *= as y's
         * would do */
        x = x * scale;
        y *= scale;
        z = z * scale;
    }
    if (y < cap_lowest - CAP_HEIGHT) {
        y = cap_lowest - CAP_HEIGHT;
    }
    /* @hack spelling: vx and vz damped as vx = vx * CAP_DAMPING, where *= as
     * vy's would do */
    vx = vx * CAP_DAMPING;
    vy *= CAP_DAMPING;
    vz = vz * CAP_DAMPING;
    center.x = x;
    center.y = y + CAP_HEIGHT;
    center.z = z;
    players = 0;
    sum.x = 0;
    sum.y = 0;
    sum.z = 0;
    for (i = 0; i < PUSHING_PLAYERS && player_work[i] != NULL; i++) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        position = player_work[i]->pos;
        position.y += player_y_offset;
        njRotateY(NULL, -work->ang.y);
        njRotateX(NULL, -work->ang.x);
        njRotateZ(NULL, -work->ang.z);
        njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
        njCalcPoint(NULL, &position, &relative);
        njPopMatrixEx();
        /* @hack spelling: player_point, a pointer to relative, where
         * &relative would do */
        player_point = &relative;
#ifdef NONMATCHING
        /* this side takes the point cap_push returns */
        pushed = cap_push(&center, player_point,
                          player_records[i]->consts.radius + CAP_RADIUS);
#else
        pushed = *cap_push(&center, player_point,
                           player_records[i]->consts.radius + CAP_RADIUS);
#endif
        /* @hack spelling: players = players + 1, where players++ would do */
        players = players + 1;
        sum.x += pushed.x;
        sum.y += pushed.y;
        sum.z += pushed.z;
    }
    if (players > 1) {
        scale = 1.0F / players;
        sum.x *= scale;
        sum.y *= scale;
        sum.z *= scale;
    } else if (players == 0) {
        sum = center;
    }
    x = sum.x;
    y = sum.y - CAP_HEIGHT;
    z = sum.z;
    vx += (x - center.x) * CAP_FOLLOW;
    vy += (sum.y - center.y) * CAP_FOLLOW;
    vz += (z - center.z) * CAP_FOLLOW;
    length = njSqrt((vx * vx) + (vy * vy) + (vz * vz));
    if (length > CAP_SPEED_MAX) {
        scale = CAP_SPEED_MAX / length;
        vx *= scale;
        vz *= scale;
        vy *= scale;
    }
    if (y < cap_lowest - CAP_HEIGHT) {
        y = cap_lowest - CAP_HEIGHT;
    }
    CAP(task)->offset.x = x;
    CAP(task)->offset.y = y;
    CAP(task)->offset.z = z;
    CAP(task)->speed.x = vx;
    CAP(task)->speed.y = vy;
    CAP(task)->speed.z = vz;
    length = njSqrt((x * x) + (z * z));
    y += CAP_HEIGHT;
    if (!(CAP_TRAVEL > length)) {
        scale = CAP_TRAVEL / length;
        x *= scale;
        y *= scale;
        z *= scale;
    }
    direction_angles_xz(x, y, z, &CAP(task)->tilt.x, &CAP(task)->tilt.z);
}

static void switch_ready(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 player;
    player = player_work[0]->pos;
    if (!(SWITCH_DISTANCE2(player, work->pos) > switch_range2)) {
        cap_update(task);
    }
    if (work->ang.x == 0 && work->ang.z == 0) {
        collision_register(task);
    }
}

static void switch_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 player;
    Float mix;
    Float bob;
    player = player_work[0]->pos;
    if (SWITCH_DISTANCE2(player, work->pos) > switch_range2) {
        return;
    }
    /* @hack redundant: njPushMatrixEx named in a statement that does
     * nothing */
    (void)njPushMatrixEx;
    njSetTexture(&time_switch_base_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(&time_switch_base);
    mix = glow * (1.0F / GLOW_FULL);
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_IL | NJD_FST_IS);
    njFogDisable();
    bob = CAP_BOB();
    njTranslate(NULL, 0.0F, CAP(task)->offset.y + bob, 0.0F);
    njRotateZ(NULL, CAP(task)->tilt.z);
    njRotateX(NULL, CAP(task)->tilt.x);
    njRotateY(NULL, game_state.steps << CAP_SPIN_SHIFT);
    vlist_set_colors(time_switch_cap_vertices.head, TIME_SWITCH_CAP_VERTICES,
                     color_blend(light_color, dark_color, mix));
    njTranslate(NULL, 0.0F, CAP_LIFT, 0.0F);
    njSetTexture(&time_switch_cap_texlist);
    njCnkEasyDrawObject(&time_switch_cap);
    njPushMatrixEx();
    njCnkEasyDrawObject(&time_switch_ring);
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ONE);
    njPushMatrixEx();
    njRotateY(NULL, game_state.steps * FAST_RING_TURN);
    njCnkEasyDrawObject(&time_switch_fast_ring);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, SLOW_RING_LIFT, 0.0F);
    njRotateY(NULL, game_state.steps * SLOW_RING_TURN);
    njCnkEasyDrawObject(&time_switch_slow_ring);
    njPopMatrixEx();
    draw_control_restore();
    njPopMatrixEx();
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ONE);
    njSetTexture(&time_switch_effect_texlist);
    njCnkEasyDrawMotion(&time_switch_effect, time_switch_motion,
                        game_state.steps % time_switch_motion->nbFrame);
    draw_control_restore();
    njFogEnable();
    njPopMatrixEx();
}

static void switch_destroy(struct task *task)
{
    cap_release(task);
    game_state.time_stopped = 0;
}

static void cap_release(struct task *task)
{
    /* @hack redundant: work, which nothing reads */
    struct task_work *work = task->work;
    /* @hack redundant: a return no path reaches */
    if (0) {
        return;
    }
    syFree(task->extra.p);
    task->extra.p = NULL;
}

static void target_init(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *work = task->work;
#ifdef NONMATCHING
#else
    task->step = target_step;
    task->destructor = target_destroy;
#endif
    work->pos.x = parent->work->pos.x + CAP(parent)->offset.x;
    work->pos.y = parent->work->pos.y + CAP(parent)->offset.y + CAP_HEIGHT;
    work->pos.z = parent->work->pos.z + CAP(parent)->offset.z;
#ifdef NONMATCHING
    if (!collision_init(task, &target_shape, 1, COLLISION_ITEM) ||
        enemy_work_alloc(task) == NULL) {
        task_destroy(parent);
        return;
    }
    task->destructor = target_destroy;

    /* a task's children step right after it, so the switch can't draw before
     * this init runs. the switch takes its draw and destructor only once the
     * target has its shape and its enemy work. the countdown needs its task
     * by then too */
    timer_create();
    if (timer_task == NULL) {
        task_destroy(parent);
        return;
    }
    parent->destructor = switch_destroy;
    parent->draw = switch_draw;
    task->step = target_step;
#else
    collision_init(task, &target_shape, 1, COLLISION_ITEM);
    enemy_work_alloc(task);
#endif
}

static void target_destroy(struct task *task)
{
    enemy_destroy(task);
}

static void target_step(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *base = parent->work;
    struct task_work *work = task->work;
    /* @hack redundant: unused, a point nothing reads, which sizes the frame */
    NJS_POINT3 unused;
    /* @hack redundant: speed, set to 0 and never read */
    NJS_VECTOR speed;
    NJS_POINT3 pos;
    Float bob;
    bob = CAP_BOB();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&base->pos);
    njRotateZ(NULL, base->ang.z);
    njRotateX(NULL, base->ang.x);
    njRotateY(NULL, base->ang.y);
    njTranslate(NULL, 0.0F, CAP(parent)->offset.y + bob, 0.0F);
    njRotateZ(NULL, CAP(parent)->tilt.z);
    njRotateX(NULL, CAP(parent)->tilt.x);
    njTranslate(NULL, 0.0F, CAP_HEIGHT, 0.0F);
    njGetTranslation(NULL, &work->pos);
    njPopMatrixEx();
    /* @hack redundant: a test of base no path reaches */
    if (0) {
        if (base) {
        }
    }
    if (work->flags & WORK_HURT) {
        work->flags &= ~WORK_HURT;
        speed.x = 0.0F;
        speed.y = 0.0F;
        speed.z = 0.0F;
        pos = work->pos;
        flash_manager_create_sparks_depth(pos.x, pos.y, pos.z, FLASH_SCALE,
                                          FLASH_SPARKS, FLASH_DEPTH_BIAS);
        time_left = parent->work->scl.z;
        counting = TRUE;
    } else {
        collision_register(task);
    }
}

static void set_lamp(struct task *task, Bool on)
{
    /* @hack redundant: work, which nothing reads */
    struct task_work *work = task->work;
    if (on) {
        lit = TRUE;
        glow = GLOW_FULL;
    } else {
        if (lit) {
            sound_play(TICK_SOUND, task, 0, 0);
            sound_set_volume(task, SOUND_ANY_ID, timer_volume);
        }
        lit = FALSE;
    }
}

static void blink_schedule(struct task *task)
{
    /* @hack spelling: wait, a pointer to blink_wait, where blink_wait
     * itself would do */
    Sint32 *wait = &blink_wait;
    Sint32 remaining = time_left - BLINK_STEPS;
    if (remaining > 0) {
        if (lit) {
            *wait = (Sint32)(remaining * LIT_WAIT) + 1;
        } else {
            *wait = (Sint32)(remaining * DARK_WAIT) + 1;
        }
    } else {
        *wait = 1;
    }
}

static void timer_step(struct task *task)
{
    if (previous_time != time_left) {
        sound_play(TICK_SOUND, task, 0, 0);
        sound_set_volume(task, SOUND_ANY_ID, timer_volume);
        set_lamp(task, TRUE);
        blink_schedule(task);
    } else if (time_left > 0 && counting) {
        if (blink_wait > 0) {
            blink_wait--;
        } else {
            set_lamp(task, !lit);
            blink_schedule(task);
        }
    }
    if (time_left > 0) {
        if (counting) {
            if (--time_left == 0) {
                sound_play(END_SOUND, task, 0, 0);
                sound_set_volume(task, SOUND_ANY_ID, timer_volume);
                set_lamp(task, FALSE);
            }
        }
        game_state.time_stopped = TIME_STOPPED_BIT(1);
    } else {
        blink_wait = 0;
        game_state.time_stopped = 0;
    }
    if (!lit) {
        if (glow >= GLOW_DIM + GLOW_FADE) {
            glow -= GLOW_FADE;
        } else {
            glow = GLOW_DIM;
        }
    }
    previous_time = time_left;
}

static void timer_destroy(struct task *task)
{
    timer_task = NULL;
}

static void timer_create(void)
{
    if (timer_task == NULL) {
        timer_task = task_create(TASK_WORK, TIMER_LEVEL, timer_step,
                                 "CountDownTimerManagerTaskExec");
#ifdef NONMATCHING
        /* this side makes no timer where task_create fails. the matching side
         * writes through the null */
        if (timer_task == NULL) {
            return;
        }
#endif
        timer_task->destructor = timer_destroy;
        blink_wait = previous_time = time_left = 0;
        glow = GLOW_FULL;
        lit = FALSE;
    }
}
