/* the ring, CART RING in the cart's object list. a touch by any racer takes
 * it, but only a player's kart gains it. a ring comes back RETURN_STEPS after
 * it is taken */
#include <ninja.h>
#include <sg_xpt.h>

#include "cart/objects.h"
#include "cart/vehicle.h"
#include "collision/collision.h"
#include "hud/hud.h"
#include "object/set.h"
#include "shared/ring.h"
#include "shared/ring_sparkle.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"

/* a ring's modes, in its work's mode. a MODE_LIVE ring grows to its full size
 * in GROW_STEPS steps. nothing in this source puts a ring in MODE_KILL */
enum ring_mode {
    MODE_INIT,
    MODE_RESET,
    MODE_LIVE,
    MODE_TAKEN,
    MODE_GONE,
    MODE_KILL
};
#define GROW_STEPS 30
#define RETURN_STEPS 3600
/* a ring's turn about y a step */
#define RING_TURN ((Angle)(2 * ANGLE_PER_DEGREE))

/* a taken ring gives SPARKLES sparkles round it, SPARKLE_RADIUS out and a
 * quarter turn apart. they stand SPARKLE_RISE apart in height, the lowest at
 * SPARKLE_RISE - SPARKLE_DROP above the ring. each starts rising
 * SPARKLE_SPEED a step */
#define SPARKLES 4
#define SPARKLE_RADIUS 4.0F
#define SPARKLE_DROP 1.5F
#define SPARKLE_RISE 1.8F
#define SPARKLE_SPEED 0.02F
#define SPARKLE_SCALE 2.5F

/* clang-format off */
static struct collision_shape ring_shape[] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_PLAYERS | PUSH_ENEMIES | PUSH_PICKUPS), 0,
     SHAPE_IGNORE_PLAYERS | SHAPE_IGNORE_PICKUPS, {0, 0, 0}, 5, 0, 0, 0,
     {0, 0, 0}}};
/* clang-format on */
static void ring_draw(struct task *task);
static void ring_destroy(struct task *task);
static void release_sparkles(struct task *task);

void cart_ring_create(NJS_POINT3 *pos)
{
    struct task *task =
        task_create(TASK_WORK, 2, cart_ring_step, "CartRingExec");
    task->work->pos = *pos;
    task->work->mode = MODE_INIT;
}
void cart_ring_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *other;
    struct cart_vehicle *kart;
    int i;
    if (cart_out_of_range(&work->pos) && work->mode != MODE_GONE) {
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
        task->draw = ring_draw;
        task->destructor = ring_destroy;
        collision_init(task, ring_shape, 1, COLLISION_OBJECT_MUTUAL);
        work->mode = MODE_RESET;
        break;
    case MODE_RESET:
        work->scl.z = 0;
        work->mode = MODE_LIVE;
        collision_register(task);
        break;
    case MODE_LIVE:
        work->scl.z += 1.0F / GROW_STEPS;
        if (work->scl.z > 1) {
            work->scl.z = 1;
        }
        if (work->collision->flags & COLLISION_TOUCHED) {
            for (i = 0; i < work->collision->hit_count; i++) {
                other = work->collision->hits[i].task;
                if (cart_is_racer(other)) {
                    kart = other->motion.p;
                    work->mode = MODE_TAKEN;
                    if (kart->start.player_controlled) {
                        ring_add(kart->driver, 1);
                        sound_play(SOUND_RING_ID, NULL, 0, 0);
                        kart->boost_rings++;
                    }
                }
            }
        }
        collision_register(task);
        break;
    case MODE_TAKEN:
        release_sparkles(task);
        work->mode = MODE_GONE;
        work->timer = RETURN_STEPS;
        break;
    case MODE_GONE:
        if (!--work->timer) {
            work->mode = MODE_RESET;
        }
        break;
    case MODE_KILL:
        if (task->placement) {
            placement_kill(task);
        } else {
            task_destroy(task);
        }
        return;
    }
    work->ang.y += RING_TURN;
}
static void ring_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float scale;
    if (!cart_out_of_view(&work->pos) && work->mode == MODE_LIVE) {
        njSetTexture(&ring_texlist);
        njPushMatrixEx();
        njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
        njRotateY(NULL, work->ang.y);
        scale = work->scl.z;
        njScale(NULL, scale, scale, scale);
        njCnkSimpleDrawModel(ring_model);
        njPopMatrixEx();
    }
}
static void ring_destroy(struct task *task)
{
}
static void release_sparkles(struct task *task)
{
    struct task_work *work;
    Float x;
    Float y;
    Float z;
    NJS_POINT3 pos;
    NJS_VECTOR velocity;
    /* @hack spelling: i set in its declaration, where the for's start would
     * do */
    int i = SPARKLES;
    Angle angle = 0;
    /* @hack spelling: the quarter turn held in step, where
     * angle += QUARTER_TURN would do */
    Angle step = QUARTER_TURN;
    /* @hack spelling: work set by a statement of its own, where its
     * declaration's initializer would do */
    work = task->work;
    /* @hack spelling: the work's pos held in x, y and z, where work->pos at
     * each use would do */
    x = work->pos.x;
    y = work->pos.y;
    z = work->pos.z;
    velocity.x = 0;
    velocity.y = SPARKLE_SPEED;
    velocity.z = 0;
    for (; i > 0; i--, angle += step) {
        pos.x = (njSin(angle) * SPARKLE_RADIUS) + x;
        pos.y = (i * SPARKLE_RISE) + y - SPARKLE_DROP;
        pos.z = (njCos(angle) * SPARKLE_RADIUS) + z;
        ring_sparkle_create(&pos, &velocity, SPARKLE_SCALE);
    }
}
