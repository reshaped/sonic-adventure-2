/* collision records, the checks that test them each step, and the calls that
 * read what the checks found */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/mesh.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/matrix.h"
#include "system/task.h"

#ifdef NONMATCHING
/* this side turns a vector by zam_vec3_turn_y and zam_vec3_turn_z. for a
 * finite vector they give the shipped bits, as turn.h says */
#include "engine/dc/turn.h"
#endif

/* the checks list a target near a player within NEAR_REACH of it. they
 * measure the squared distance, against NEAR_REACH2 */
#define NEAR_REACH 100.0F
#define NEAR_REACH2 (NEAR_REACH * NEAR_REACH)
/* a record's radius is FIT_MARGIN times its shapes' farthest reach, or
 * FIT_DEFAULT where that reach is under FIT_MIN */
#define FIT_MARGIN 1.01F
#define FIT_MIN 0.1F
#define FIT_DEFAULT 25.0F
/* the least distance a nearest call starts from. a task this far or farther is
 * never the nearest */
#define NEAREST_REACH 1000000.0F
/* a direction between two shapes that has no length becomes (0, 0, FALLBACK_Z).
 * its length is then FALLBACK_Z */
#define FALLBACK_Z 0.1F
/* how far a landing or a push leaves a shape clear of the one it meets. some
 * tests leave CLEARANCE and others the smaller CLEARANCE_FINE */
#define CLEARANCE 0.01F
#define CLEARANCE_FINE 0.001F
/* a push out of an upright cylinder, a SHAPE_CYLINDER2 or a capsule falls
 * PUSH_SHORT short of the overlap */
#define PUSH_SHORT 0.1F
/* a touch of two spheres pushes each SPHERE_PUSH_SHARE of the overlap. a side
 * touch with an upright cylinder pushes the sphere or first cylinder
 * CYLINDER_PUSH_SHARE of it. against an upright cylinder with PUSH_PLAYERS, a
 * SHAPE_STANDING shape is pushed STANDING_PUSH_SHARE of the overlap less
 * PUSH_SHORT */
#define SPHERE_PUSH_SHARE 0.49F
#define CYLINDER_PUSH_SHARE 0.45F
#define STANDING_PUSH_SHARE 0.5F
/* a SHAPE_STANDING shape moving down also lands on a SHAPE_BOX or a
 * SHAPE_AXIS_BOX by LAND_SLACK. its bottom must have lain less than LAND_SLACK
 * below the box's top the step before */
#define LAND_SLACK 0.1F
/* a shape whose top lies above another's bottom by less than BOTTOM_REACH
 * strikes that bottom from below */
#define BOTTOM_REACH 2.0F
/* a SHAPE_BOX2 takes a landing or a strike within SURFACE_REACH of its face. a
 * SHAPE_CYLINDER2 pushes a SHAPE_STANDING shape out of an end within
 * SURFACE_REACH of it */
#define SURFACE_REACH 3.0F
/* a sweep steps SWEEP_STEP at a time, so it takes SWEEP_STEPS_PER_UNIT steps to
 * a unit of its length, rounded up */
#define SWEEP_STEP 2.5F
#define SWEEP_STEPS_PER_UNIT (1.0F / SWEEP_STEP)
/* a sweep that moves no distance measures as SWEEP_STILL */
#define SWEEP_STILL 0.01F
/* a SHAPE_SWEPT_SPHERE's first sweep starts SWEEP_NUDGE along x from its task's
 * position */
#define SWEEP_NUDGE 0.001F
/* collide_standing_box counts a strike on a SHAPE_BOX's bottom only within
 * STRIKE_SHARE of the reach. it measures across x and z from the box's center.
 * the reach is the box's half size plus the shape's radius. the other strikes
 * take the whole reach */
#define STRIKE_SHARE 0.99F
/* a SHAPE_BOX2 pushes a shape out of a side only within a band about its
 * center. the band reaches the box's half height plus the shape's radius each
 * way. BOX2_TRIM_TOP comes off its top, and BOX2_TRIM_BOTTOM off its bottom.
 * the push moves the shape BOX2_PUSH_SHARE of the way to a mark just inside the
 * side. the mark lies BOX2_SIDE_SHARE of the reach from the box's center. the
 * reach is the box's half size plus the shape's radius */
#define BOX2_TRIM_TOP 4.0F
#define BOX2_TRIM_BOTTOM 2.0F
#define BOX2_PUSH_SHARE 0.3F
#define BOX2_SIDE_SHARE 0.999F
/* a shape lands on an end of a SHAPE_CYLINDER2 that faces within about 49
 * degrees of straight up. END_UP is that angle's cosine */
#define END_UP 0.65F
/* a SHAPE_WALL puts a shape back WALL_CLEARANCE in front of its face. it does
 * so only while the shape's center lies less than WALL_REACH behind the face */
#define WALL_CLEARANCE 0.1F
#define WALL_REACH 30.0F
/* the z of a normal no side has set, which no unit normal reaches */
#define NO_SIDE_Z 100.0F

#define SQUARE(v) ((v) * (v))
/* LAND_LEVEL is the height 0.4 of the way up from bottom to top.
 * SPHERE_LAND_LEVEL is 0.8 of the way, and CYLINDER_LAND_LEVEL 0.7. a shape
 * lands on a box where the box's top lies between the shape's bottom and its
 * LAND_LEVEL. a SHAPE_STANDING shape lands so on an upright cylinder too. a
 * SHAPE_STANDING shape also lands on a box as LAND_SLACK says. where a sphere
 * meets an upright cylinder, either lands where its bottom lies above the
 * other's SPHERE_LAND_LEVEL. between two upright cylinders the level is
 * CYLINDER_LAND_LEVEL */
#define LAND_LEVEL(bottom, top) (((bottom) * 0.6F) + ((top) * 0.4F))
#define SPHERE_LAND_LEVEL(bottom, top) (((top) * 0.8F) + ((bottom) * 0.2F))
#define CYLINDER_LAND_LEVEL(bottom, top) (((top) * 0.7F) + ((bottom) * 0.3F))

/* collision_get_form reads a shape only below this index */
#define FORM_SHAPES 16

/* the place the scans share, as the last call left it */
struct collision_scan {
    struct task *task; /* the task whose hits it walked */
    const void *scan; /* the scan's _hit form, as its key */
    Sint32 index; /* the next call starts one past it */
    /* scan_start_with_id tests it against its id, but every store writes -1 */
    Sint32 id;
};

struct near_target near_targets[NEAR_TARGETS + 1];
struct near_target near_rings[NEAR_RINGS + 1];
struct near_target near_targets_player1[NEAR_TARGETS + 1];
struct near_target near_rings_player1[NEAR_RINGS + 1];
Uint16 collision_player_count;
Uint16 collision_attack_count;
Uint16 collision_item_count;
Uint16 collision_enemy_count;
Uint16 collision_last_item_count;
Uint16 collision_last_enemy_count;
Uint16 collision_object_count;
Uint16 collision_mutual_object_count;
Uint16 collision_pickup_count;
Uint16 collision_pickup_target_count;
Uint16 collision_wall_count;
Uint16 collision_unk_9_count;
struct task *collision_players[COLLISION_PLAYERS];
struct task *collision_attacks[COLLISION_ATTACKS];
struct task *collision_items[COLLISION_ITEMS];
struct task *collision_enemies[COLLISION_ENEMIES];
static struct task *collision_objects[COLLISION_OBJECTS];
static struct task *collision_mutual_objects[COLLISION_MUTUAL_OBJECTS];
static struct task *collision_pickups[COLLISION_PICKUPS];
static struct task *collision_pickup_targets[COLLISION_PICKUP_TARGETS];
static struct task *collision_walls[COLLISION_WALLS];
static struct task *collision_unk_9s[COLLISION_UNK_9S];
/* the entries the near lists hold */
static Uint16 near_target_count;
static Uint16 near_ring_count;
static Uint16 near_target_count_player1;
static Uint16 near_ring_count_player1;
static struct collision_scan collision_scan;
static int collision_up_axis; /* COLLISION_UP_NEG_X to COLLISION_UP_Z */

/* tables by the kind of a record, a row a kind. each holds the bits a shape
 * needs to act on a record of the kind. -1 is every bit */
/* clang-format off */
/* the bits of a shape's push that push a record of the kind */
static Sint8 pushed_by[COLLISION_KINDS] = {
    PUSH_PLAYERS, /* COLLISION_PLAYER */
    0,            /* COLLISION_ATTACK */
    PUSH_ENEMIES, /* COLLISION_ITEM */
    PUSH_ENEMIES, /* COLLISION_ENEMY */
    PUSH_OBJECTS, /* COLLISION_OBJECT */
    PUSH_OBJECTS, /* COLLISION_OBJECT_MUTUAL */
    PUSH_PICKUPS, /* COLLISION_PICKUP */
    PUSH_PICKUPS, /* COLLISION_PICKUP_TARGET */
    -1,           /* COLLISION_WALL */
    PUSH_PLAYERS, /* COLLISION_UNK_9 */
};
/* the bits of a shape's push that resist a push by a record of the kind. they are PUSH_FIRM bits, but for an attack every bit */
static Sint8 firm_against[COLLISION_KINDS] = {
    PUSH_FIRM(PUSH_PLAYERS),        /* COLLISION_PLAYER */
    -1,                             /* COLLISION_ATTACK */
    PUSH_FIRM(PUSH_ENEMIES),        /* COLLISION_ITEM */
    PUSH_FIRM(PUSH_ENEMIES),        /* COLLISION_ENEMY */
    PUSH_FIRM(PUSH_OBJECTS),        /* COLLISION_OBJECT */
    PUSH_FIRM(PUSH_OBJECTS),        /* COLLISION_OBJECT_MUTUAL */
    (Sint8)PUSH_FIRM(PUSH_PICKUPS), /* COLLISION_PICKUP */
    (Sint8)PUSH_FIRM(PUSH_PICKUPS), /* COLLISION_PICKUP_TARGET */
    0,                              /* COLLISION_WALL */
    PUSH_FIRM(PUSH_PLAYERS),        /* COLLISION_UNK_9 */
};
/* the bit of a shape's levels that hurts a record of the kind */
static Sint8 hurt_by[COLLISION_KINDS] = {
    SHAPE_HURTS_PLAYERS,        /* COLLISION_PLAYER */
    0,                          /* COLLISION_ATTACK */
    SHAPE_HURTS_ENEMIES,        /* COLLISION_ITEM */
    SHAPE_HURTS_ENEMIES,        /* COLLISION_ENEMY */
    (Sint8)SHAPE_HURTS_OBJECTS, /* COLLISION_OBJECT */
    (Sint8)SHAPE_HURTS_OBJECTS, /* COLLISION_OBJECT_MUTUAL */
    0,                          /* COLLISION_PICKUP */
    0,                          /* COLLISION_PICKUP_TARGET */
    0,                          /* COLLISION_WALL */
    SHAPE_HURTS_PLAYERS,        /* COLLISION_UNK_9 */
};
/* the flag of a shape that passes a record of the kind over */
static Uint32 ignored_by[COLLISION_KINDS] = {
    SHAPE_IGNORE_PLAYERS, /* COLLISION_PLAYER */
    0,                    /* COLLISION_ATTACK */
    SHAPE_IGNORE_ENEMIES, /* COLLISION_ITEM */
    SHAPE_IGNORE_ENEMIES, /* COLLISION_ENEMY */
    SHAPE_IGNORE_OBJECTS, /* COLLISION_OBJECT */
    SHAPE_IGNORE_OBJECTS, /* COLLISION_OBJECT_MUTUAL */
    SHAPE_IGNORE_PICKUPS, /* COLLISION_PICKUP */
    SHAPE_IGNORE_PICKUPS, /* COLLISION_PICKUP_TARGET */
    0,                    /* COLLISION_WALL */
    SHAPE_IGNORE_UNK_9,   /* COLLISION_UNK_9 */
};
/* clang-format on */

static void collide_players(void);
static void collision_empty_lists(void);
static void near_clear(void);
static int scan_start(struct task *task, const void *scan);
static int scan_start_with_id(struct task *task, const void *scan, int id);

Bool collision_init(struct task *task, const struct collision_shape *shapes,
                    int count, Uint8 kind)
{
    struct collision_work *collision;
    struct collision_shape *copy;

    if (task->work == NULL) {
        return FALSE;
    }
    collision = syCalloc(1, sizeof(struct collision_work));
    if (collision == NULL) {
        return FALSE;
    }
    copy = syCalloc(count, sizeof(struct collision_shape));
    if (copy == NULL) {
#ifdef NONMATCHING
        /* this side frees the record it just made, which collision_free can't
         * reach: task->work->collision doesn't hold it yet */
        syFree(collision);
#else
        collision_free(task);
#endif
        return FALSE;
    }
    njMemCopy(copy, shapes, count * sizeof(struct collision_shape));
    collision->shapes = copy;
    collision->flags |= COLLISION_OWN_SHAPES;
    if (kind == COLLISION_ITEM || kind == COLLISION_ENEMY ||
        kind == COLLISION_PICKUP_TARGET) {
        collision->flags |= COLLISION_TARGET;
    }
    collision->kind = kind;
    collision->count = count;
    task->work->collision = collision;
    collision->owner = task;
    collision_reset(task);
    collision_fit_radius(task);
    return TRUE;
}

Bool collision_init_in_place(struct task *task, struct collision_shape *shapes,
                             int count, Uint8 kind)
{
    struct collision_work *collision;

    if (task->work == NULL) {
        return FALSE;
    }
    collision = syCalloc(1, sizeof(struct collision_work));
    if (collision == NULL) {
        return FALSE;
    }
    collision->shapes = shapes;
    collision->flags &= ~COLLISION_OWN_SHAPES;
    if (kind == COLLISION_ITEM || kind == COLLISION_ENEMY ||
        kind == COLLISION_PICKUP_TARGET) {
        collision->flags |= COLLISION_TARGET;
    }
    collision->kind = kind;
    collision->count = count;
    task->work->collision = collision;
    collision->owner = task;
    collision_reset(task);
    collision_fit_radius(task);
    return TRUE;
}

Bool collision_init_attack(struct task *task, Float radius, Uint32 flags,
                           int form)
{
    struct collision_work *collision;
    struct collision_shape *shape;

    if (task->work == NULL) {
        return FALSE;
    }
    collision = syCalloc(1, sizeof(struct collision_work));
    if (collision == NULL) {
        return FALSE;
    }
    shape = syCalloc(1, sizeof(struct collision_shape));
    if (shape == NULL) {
#ifdef NONMATCHING
        /* this side frees the record it just made, which collision_free can't
         * reach: task->work->collision doesn't hold it yet */
        syFree(collision);
#else
        collision_free(task);
#endif
        return FALSE;
    }
    shape->form = form;
    shape->flags = flags | SHAPE_UNK_40;
    shape->radius = radius;
    collision->shapes = shape;
    collision->flags |= COLLISION_OWN_SHAPES;
    collision->kind = COLLISION_ATTACK;
    collision->count = 1;
    task->work->collision = collision;
    collision->owner = task;
    collision_reset(task);
    return TRUE;
}

Bool collision_init_attack_sphere(struct task *task, Float radius, Uint32 flags)
{
    return collision_init_attack(task, radius, flags, SHAPE_SPHERE);
}

Bool collision_init_attack_swept(struct task *task, Float radius, Uint32 flags)
{
    return collision_init_attack(task, radius, flags, SHAPE_SWEPT_SPHERE);
}

void collision_reset(struct task *task)
{
    struct collision_work *collision = task->work->collision;
    Uint16 count = collision->count;
    struct collision_shape *shape = collision->shapes;
    struct collision_hit *hit = collision->hits;
    struct collision_hit *end = &collision->hits[COLLISION_HITS];

    collision->hit_count = 0;
    collision->flags &= ~COLLISION_CHECKS_FLAGS;
    collision->own_shape = -1;
    collision->hit_shape = -1;
    collision->hit = NULL;
    while (count--) {
        shape->levels &= ~SHAPE_HURT;
        shape++;
    }
    while (hit != end) {
        hit->flags &= ~(HIT_STANDS_ON | HIT_STOOD_ON);
        hit->own_shape = -1;
        hit->hit_shape = HIT_UNUSED;
        hit->task = NULL;
        hit++;
    }
}

Bool collision_register(struct task *task)
{
    struct collision_work *collision = task->work->collision;

    if (collision == NULL) {
        return FALSE;
    }
    if (collision->owner->step == task_destroy) {
        return FALSE;
    }
    collision_reset(task);
    switch (collision->kind) {
    case COLLISION_PLAYER:
        if (collision_player_count < COLLISION_PLAYERS) {
            collision_players[collision_player_count] = task;
            collision_player_count++;
            return TRUE;
        }
        break;
    case COLLISION_ATTACK:
        if (collision_attack_count < COLLISION_ATTACKS) {
            collision_attacks[collision_attack_count] = task;
            collision_attack_count++;
            return TRUE;
        }
        break;
    case COLLISION_ITEM:
        if (collision_item_count < COLLISION_ITEMS) {
            collision_items[collision_item_count] = task;
            collision_item_count++;
            return TRUE;
        }
        break;
    case COLLISION_ENEMY:
        if (collision_enemy_count < COLLISION_ENEMIES) {
            collision_enemies[collision_enemy_count] = task;
            collision_enemy_count++;
            return TRUE;
        }
        break;
    case COLLISION_OBJECT:
        if (collision_object_count < COLLISION_OBJECTS) {
            collision_objects[collision_object_count] = task;
            collision_object_count++;
            return TRUE;
        }
        break;
    case COLLISION_OBJECT_MUTUAL:
        if (collision_mutual_object_count < COLLISION_MUTUAL_OBJECTS) {
            collision_mutual_objects[collision_mutual_object_count] = task;
            collision_mutual_object_count++;
            return TRUE;
        }
        break;
    case COLLISION_PICKUP:
        if (collision_pickup_count < COLLISION_PICKUPS) {
            collision_pickups[collision_pickup_count] = task;
            collision_pickup_count++;
            return TRUE;
        }
        break;
    case COLLISION_PICKUP_TARGET:
        if (collision_pickup_target_count < COLLISION_PICKUP_TARGETS) {
            collision_pickup_targets[collision_pickup_target_count] = task;
            collision_pickup_target_count++;
            return TRUE;
        }
        break;
    case COLLISION_WALL:
        if (collision_wall_count < COLLISION_WALLS) {
            collision_walls[collision_wall_count] = task;
            collision_wall_count++;
            return TRUE;
        }
        break;
    case COLLISION_UNK_9:
        if (collision_unk_9_count < COLLISION_UNK_9S) {
            collision_unk_9s[collision_unk_9_count] = task;
            collision_unk_9_count++;
            return TRUE;
        }
        break;
    }
    return FALSE;
}

/* list other's work, with distance, in the near lists of work's player.
 * distance is squared. it lists only where work's record is a player's, other's
 * a target, and distance under NEAR_REACH2. a full list takes nothing */
static void near_add(struct task_work *work, struct task_work *other,
                     Float distance)
{
    struct near_target *entry;

    /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
    if (distance < NEAR_REACH2 && work->collision->kind == COLLISION_PLAYER &&
        (other->collision->flags & COLLISION_TARGET)) {
        /* @bug memory: entry is never set for a player's record that is
         * neither player 0's nor player 1's. the test at 0x8c018d68 then
         * leaves the stores below, at 0x8c018da6 to 0x8c018dae, writing
         * through the register that held work, over the first three words of
         * that player's own work */
#ifdef NONMATCHING
        /* this side keeps the result of the tag above: entry starts at work,
         * so it makes the stores the tag describes */
        entry = (struct near_target *)work;
#endif
        if (work == player_work[0]) {
            if (other->id == WORK_ID_RING) {
                if (near_ring_count >= NEAR_RINGS) {
                    return;
                }
                entry = &near_rings[near_ring_count];
                near_ring_count++;
            } else {
                if (near_target_count >= NEAR_TARGETS) {
                    return;
                }
                entry = &near_targets[near_target_count];
                near_target_count++;
            }
        } else if (work == player_work[1]) {
            if (other->id == WORK_ID_RING) {
                if (near_ring_count_player1 >= NEAR_RINGS) {
                    return;
                }
                entry = &near_rings_player1[near_ring_count_player1];
                near_ring_count_player1++;
            } else {
                if (near_target_count_player1 >= NEAR_TARGETS) {
                    return;
                }
                entry = &near_targets_player1[near_target_count_player1];
                near_target_count_player1++;
            }
        }
        entry->work = other;
        entry->distance = distance;
        entry++;
        entry->work = NULL;
    }
    /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
}

/* whether task's and other's positions lie nearer than their records' radii's
 * sum. either way, near_add then lists other near task where it is a target
 * near a player */
static Bool list_near_and_test_reach(struct task *task, struct task *other)
{
    struct task_work *work;
    struct task_work *other_work;
    Bool in_reach = FALSE;
    Float radius;
    Float other_radius;
    Float sum2;
    Float distance2;

    /* @hack spelling: the two works set in statements after in_reach's
     * initializer, where their declarations would do */
    work = task->work;
    other_work = other->work;
    radius = work->collision->radius;
    other_radius = other_work->collision->radius;
    sum2 = (radius + other_radius) * (radius + other_radius);
    distance2 = mesh_point_distance2(&work->pos, &other_work->pos);
    if (distance2 < sum2) {
        in_reach = TRUE;
    }
    near_add(work, other_work, distance2);
    return in_reach;
}

/* try a push of pushed_part by pusher_part. set COLLISION_TOUCHED in pushed.
 * where the push goes ahead, set COLLISION_PUSHED there too and return TRUE.
 * for that, pusher_part's push must name the group of pushed's kind.
 * pushed_part's push must not be firm against pusher's kind. nor may both parts
 * have SHAPE_HURT_NO_PUSH while pusher_part is hurt. otherwise it returns FALSE
 */
static Bool try_push(struct collision_work *pushed,
                     struct collision_shape *pushed_part,
                     struct collision_work *pusher,
                     struct collision_shape *pusher_part)
{
    Uint32 kind = pushed->kind;
    /* @hack type: an int for pusher's kind, where kind's Uint32 would do */
    int pusher_kind = pusher->kind;
    Uint16 push = pushed_part->push;
    Uint16 pusher_push = pusher_part->push;

    pushed->flags |= COLLISION_TOUCHED;
    if (!(pushed_by[kind] & pusher_push)) {
        return FALSE;
    } else if ((pushed_part->flags & SHAPE_HURT_NO_PUSH) &&
               (pusher_part->flags & SHAPE_HURT_NO_PUSH) &&
               (pusher_part->levels & SHAPE_HURT)) {
        return FALSE;
    }
    if (firm_against[pusher_kind] & push) {
        return FALSE;
    }
    pushed->flags |= COLLISION_PUSHED;
    return TRUE;
}

/* hurt each SHAPE_HURTABLE shape whose guard the other's attack beats, where
 * the other's levels hurt its kind. a hurt shape takes SHAPE_HURT, its record
 * COLLISION_HURT, and its task's work WORK_HURT */
static void hurt_shapes(struct task *task, struct collision_shape *shape,
                        struct task *other, struct collision_shape *other_shape)
{
    struct collision_work *collision = task->work->collision;
    struct collision_work *other_collision = other->work->collision;
    Uint32 kind;
    Uint8 attack;
    Uint8 guard;

    kind = other_collision->kind;
    attack = shape->levels & SHAPE_ATTACK;
    guard = (other_shape->levels & SHAPE_GUARD) >> SHAPE_GUARD_SHIFT;
    if (other_shape->flags & SHAPE_HURTABLE) {
        if (attack > guard) {
            if (shape->levels & hurt_by[kind]) {
                other_collision->flags |= COLLISION_HURT;
                other_shape->levels |= SHAPE_HURT;
                other->work->flags |= WORK_HURT;
            }
        }
    }
    kind = collision->kind;
    attack = other_shape->levels & SHAPE_ATTACK;
    guard = (shape->levels & SHAPE_GUARD) >> SHAPE_GUARD_SHIFT;
    if (shape->flags & SHAPE_HURTABLE) {
        if (attack > guard) {
            if (other_shape->levels & hurt_by[kind]) {
                collision->flags |= COLLISION_HURT;
                shape->levels |= SHAPE_HURT;
                task->work->flags |= WORK_HURT;
            }
        }
    }
}

/* TRUE where either shape ignores the other's kind */
static Bool shapes_ignore(struct task *task, struct collision_shape *shape,
                          struct task *other,
                          struct collision_shape *other_shape)
{
    struct collision_work *collision = task->work->collision;
    struct collision_work *other_collision = other->work->collision;

    if ((ignored_by[collision->kind] & other_shape->flags) ||
        (ignored_by[other_collision->kind] & shape->flags)) {
        return TRUE;
    }
    return FALSE;
}

/* keep the touch of the two shapes in each record whose shapes' flags let it.
 * such a record takes COLLISION_TOUCHED, and the touch as its latest. it keeps
 * the touch in its first unused hit too, where it has one left */
static void keep_touch(struct task *task, struct collision_shape *shape,
                       struct task *other, struct collision_shape *other_shape)
{
    struct collision_work *collision = task->work->collision;
    struct collision_work *other_collision = other->work->collision;
    int own_shape;
    int hit_shape;
    struct collision_shape *walk;

    walk = collision->shapes;
    own_shape = 0;
    while (walk < shape) {
        walk++;
        own_shape++;
    }

    walk = other_collision->shapes;
    hit_shape = 0;
    while (walk < other_shape) {
        walk++;
        hit_shape++;
    }

    if (!(shape->flags & SHAPE_NO_OWN_HITS) &&
        !(other_shape->flags & SHAPE_NO_OTHER_HITS)) {
        struct collision_hit *end = collision->hits + COLLISION_HITS;
        /* @hack type: the hits' address cast from the array's, where
         * collision->hits would do */
        struct collision_hit *hit = (struct collision_hit *)&collision->hits;

        collision->flags |= COLLISION_TOUCHED;
        while (hit < end) {
            if (hit->hit_shape == HIT_UNUSED) {
                collision->hit_count++;
                hit->own_shape = own_shape;
                hit->hit_shape = hit_shape;
                hit->task = other;
                break;
            }
            hit++;
        }
        collision->hit = other_collision;
        collision->hit_shape = hit_shape;
        collision->own_shape = own_shape;
    }

    if (!(other_shape->flags & SHAPE_NO_OWN_HITS) &&
        !(shape->flags & SHAPE_NO_OTHER_HITS)) {
        struct collision_hit *end = other_collision->hits + COLLISION_HITS;
        /* @hack type: as above */
        struct collision_hit *hit =
            (struct collision_hit *)&other_collision->hits;

        other_collision->flags |= COLLISION_TOUCHED;
        while (hit < end) {
            if (hit->hit_shape == HIT_UNUSED) {
                other_collision->hit_count++;
                hit->own_shape = hit_shape;
                hit->hit_shape = own_shape;
                hit->task = task;
                break;
            }
            hit++;
        }
        other_collision->hit = collision;
        other_collision->hit_shape = own_shape;
        other_collision->own_shape = hit_shape;
    }
}

/* mark task's shape as standing on other's: HIT_STANDS_ON in task's latest hit,
 * and HIT_STOOD_ON in other's. each is set only where that hit is of these two
 * shapes. neither is set where task's record kept no hit. call it after
 * keep_touch */
static void mark_standing(struct task *task, struct collision_shape *shape,
                          struct task *other,
                          struct collision_shape *other_shape)
{
    struct collision_work *collision = task->work->collision;
    struct collision_work *other_collision = other->work->collision;
    struct collision_hit *hit;

    if (collision->hit_count < 1) {
        return;
    }
    if (!(shape->flags & SHAPE_NO_OWN_HITS) &&
        !(other_shape->flags & SHAPE_NO_OTHER_HITS)) {
        hit = &collision->hits[collision->hit_count - 1];
        if (hit->task == other && &collision->shapes[hit->own_shape] == shape &&
            &other_collision->shapes[hit->hit_shape] == other_shape) {
            hit->flags |= HIT_STANDS_ON;
        }
    }
    if (other_collision->hit_count >= 1 &&
        !(shape->flags & SHAPE_NO_OTHER_HITS) &&
        !(other_shape->flags & SHAPE_NO_OWN_HITS)) {
        hit = &other_collision->hits[other_collision->hit_count - 1];
        if (hit->task == task &&
            &other_collision->shapes[hit->own_shape] == other_shape &&
            &collision->shapes[hit->hit_shape] == shape) {
            hit->flags |= HIT_STOOD_ON;
        }
    }
}

/* the middle of a SHAPE_STANDING shape whose foot is pos: size_y up from it
 * along the up axis */
static void shape_get_standing_center(NJS_POINT3 *pos,
                                      struct collision_shape *shape,
                                      NJS_POINT3 *center)
{
    /* @hack spelling: the axis in a local, where the switch on
     * collision_up_axis itself would do */
    int axis = collision_up_axis;

    switch (axis) {
    case COLLISION_UP_NEG_X:
        center->x = pos->x - shape->size_y;
        center->y = pos->y;
        center->z = pos->z;
        return;
    case COLLISION_UP_X:
        center->x = pos->x + shape->size_y;
        center->y = pos->y;
        center->z = pos->z;
        return;
    case COLLISION_UP_NEG_Y:
        center->x = pos->x;
        center->y = pos->y - shape->size_y;
        center->z = pos->z;
        return;
    case COLLISION_UP_Y:
        center->x = pos->x;
        center->y = pos->y + shape->size_y;
        center->z = pos->z;
        return;
    case COLLISION_UP_NEG_Z:
        center->x = pos->x;
        center->y = pos->y;
        center->z = pos->z - shape->size_y;
        return;
    case COLLISION_UP_Z:
        center->x = pos->x;
        center->y = pos->y;
        center->z = pos->z + shape->size_y;
        return;
    default:
        return;
    }
}

void collision_get_shape_center(struct task_work *work,
                                const struct collision_shape *shape,
                                NJS_POINT3 *pos)
{
    if (shape->flags & SHAPE_IN_WORLD) {
        /* @hack spelling: the copy through stepped pointers, where three
         * assignments would do */
        const Float *from = &shape->center.x;
        Float *to = &pos->x;

        *to++ = *from++;
        *to++ = *from++;
        *to = *from;
    } else if (!(shape->flags & SHAPE_UNTURNED)) {
        matrix_push_unit();
        njTranslateEx(&work->pos);
        if (shape->flags & SHAPE_TURN_YZX) {
            njRotateY(NULL, work->ang.y);
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
        } else {
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, work->ang.y);
        }
        njCalcPoint(NULL, (NJS_POINT3 *)&shape->center, pos);
        njPopMatrixEx();
    } else {
        /* @hack spelling: a Float cursor stepped across work->pos's members,
         * then shape->center's, and another down pos's from past its z, where
         * the members in the same order would do */
        const Float *part = &work->pos.x;
        Float x = *part++;
        Float y = *part++;
        Float z = *part;
        Float cx;
        Float cy;
        Float cz;
        Float *out;

        part = &shape->center.x;
        cx = *part++;
        cy = *part++;
        cz = *part;
        x += cx;
        y += cy;
        z += cz;
        out = &pos->z + 1;
        *--out = z;
        *--out = y;
        *--out = x;
    }
}

/* collision_get_shape_center for the checks, inside the matrix
 * collision_collide_all pushed. it turns the center in that matrix rather
 * than pushing one of its own. matrix_load_parent then reloads the FPU's
 * matrix from the one below it on the stack */
static void shape_get_check_center(struct task_work *work,
                                   struct collision_shape *shape,
                                   NJS_POINT3 *pos)
{
    if (shape->flags & SHAPE_IN_WORLD) {
        /* @hack spelling: as in collision_get_shape_center */
        Float *from = &shape->center.x;
        Float *to = &pos->x;

        *to++ = *from++;
        *to++ = *from++;
        *to = *from;
    } else if (!(shape->flags & SHAPE_UNTURNED)) {
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        if (shape->flags & SHAPE_TURN_YZX) {
            njRotateY(NULL, work->ang.y);
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
        } else {
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, work->ang.y);
        }
        njCalcPoint(NULL, &shape->center, pos);
        matrix_load_parent();
    } else {
        /* @hack spelling: as in collision_get_shape_center */
        Float *part = &work->pos.x;
        Float x = *part++;
        Float y = *part++;
        Float z = *part;
        Float cx;
        Float cy;
        Float cz;

        part = &shape->center.x;
        cx = *part++;
        cy = *part++;
        cz = *part;
        x += cx;
        y += cy;
        z += cz;
        part = &pos->z + 1;
        *--part = z;
        *--part = y;
        *--part = x;
    }
}

/* collision_get_shape_center's result, from a copy of its own. the tests
 * of upright cylinders, SHAPE_CYLINDER2s and capsules find their centers by
 * it */
static void shape_get_center(struct task_work *work,
                             struct collision_shape *shape, NJS_POINT3 *pos)
{
    if (shape->flags & SHAPE_IN_WORLD) {
        pos->x = shape->center.x;
        pos->y = shape->center.y;
        pos->z = shape->center.z;
    } else if (!(shape->flags & SHAPE_UNTURNED)) {
        matrix_push_unit();
        njTranslateEx(&work->pos);
        if (shape->flags & SHAPE_TURN_YZX) {
            njRotateY(NULL, work->ang.y);
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
        } else {
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, work->ang.y);
        }
        njCalcPoint(NULL, &shape->center, pos);
        njPopMatrixEx();
    } else {
        /* @hack spelling: as in collision_get_shape_center */
        Float *part = &work->pos.x;
        Float x = *part++;
        Float y = *part++;
        Float z = *part;
        Float cx;
        Float cy;
        Float cz;

        part = &shape->center.x;
        cx = *part++;
        cy = *part++;
        cz = *part;
        x += cx;
        y += cy;
        z += cz;
        part = &pos->z + 1;
        *--part = z;
        *--part = y;
        *--part = x;
    }
}

/* the center of a box or a wall, as collision_get_shape_center finds a shape's.
 * it offsets an unturned shape's center through the matrix too, not by sums */
static void shape_get_box_center(struct task_work *work,
                                 struct collision_shape *shape, NJS_POINT3 *pos)
{
    if (shape->flags & SHAPE_IN_WORLD) {
        pos->x = shape->center.x;
        pos->y = shape->center.y;
        pos->z = shape->center.z;
    } else {
        njPushMatrix(&_nj_unit_matrix_);
        njTranslateEx(&work->pos);
        if (!(shape->flags & SHAPE_UNTURNED)) {
            if (shape->flags & SHAPE_TURN_YZX) {
                njRotateY(NULL, work->ang.y);
                njRotateZ(NULL, work->ang.z);
                njRotateX(NULL, work->ang.x);
            } else {
                njRotateZ(NULL, work->ang.z);
                njRotateX(NULL, work->ang.x);
                njRotateY(NULL, work->ang.y);
            }
        }
        njCalcPoint(NULL, &shape->center, pos);
        njPopMatrixEx();
    }
}

static void shape_get_axis_box_center(struct task_work *work,
                                      struct collision_shape *shape,
                                      NJS_POINT3 *pos)
{
    if (shape->flags & SHAPE_IN_WORLD) {
        pos->x = shape->center.x;
        pos->y = shape->center.y;
        pos->z = shape->center.z;
        return;
    }
    pos->x = work->pos.x + shape->center.x;
    pos->y = work->pos.y + shape->center.y;
    pos->z = work->pos.z + shape->center.z;
}

/* set up to the direction of a SHAPE_CYLINDER2's or a SHAPE_CAPSULE's axis: y
 * turned by its task's angles, unless the shape is SHAPE_IN_WORLD or
 * SHAPE_UNTURNED, then by its own */
static void shape_get_up(struct task_work *work, struct collision_shape *shape,
                         NJS_VECTOR *up)
{
    NJS_VECTOR y_axis;

    y_axis.x = 0.0F;
    y_axis.y = 1.0F;
    y_axis.z = 0.0F;
    njPushMatrix(&_nj_unit_matrix_);
    njTranslateEx(&work->pos);
    if (shape->flags & SHAPE_TURN_YZX) {
        if (!(shape->flags & SHAPE_IN_WORLD) &&
            !(shape->flags & SHAPE_UNTURNED)) {
            njRotateY(NULL, work->ang.y);
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
        }
        njRotateY(NULL, shape->ang.y);
        njRotateZ(NULL, shape->ang.z);
        njRotateX(NULL, shape->ang.x);
    } else {
        if (!(shape->flags & SHAPE_IN_WORLD) &&
            !(shape->flags & SHAPE_UNTURNED)) {
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, work->ang.y);
        }
        njRotateZ(NULL, shape->ang.z);
        njRotateX(NULL, shape->ang.x);
        njRotateY(NULL, shape->ang.y);
    }
    njCalcVector(NULL, &y_axis, up);
    njPopMatrixEx();
}

/* the tests of shape, of task's record, against other_shape, of other's. each
 * test but collide_standing_wall keeps a touch in the records and hurts what
 * it may. a touch moves a task, or sets a player's push, only where try_push
 * lets the push. collide_standing_wall keeps nothing, and puts its player back
 * unasked. collide_shapes drops what a test returns */

/* two SHAPE_STANDING shapes, tested as spheres. a touch moves no task. it sets
 * each player's push in its player record, SPHERE_PUSH_SHARE of the overlap
 * away from the other. the push lies across x and z */
static Float collide_standing_standing(struct task *task,
                                       struct collision_shape *shape,
                                       struct task *other,
                                       struct collision_shape *other_shape)
{
    struct task_work *work = task->work;
    struct task_work *other_work = other->work;
    struct collision_work *collision = work->collision;
    struct collision_work *other_collision = other_work->collision;
    Float radius = shape->radius;
    Float other_radius = other_shape->radius;
    NJS_POINT3 center;
    NJS_POINT3 other_center;
    NJS_VECTOR away;
    Float distance2;
    Float depth;
    Float x;
    Float y;
    Float z;
    int player;

    shape_get_check_center(work, shape, &center);
    shape_get_check_center(other_work, other_shape, &other_center);
    distance2 = SQUARE(center.x - other_center.x) +
                SQUARE(center.y - other_center.y) +
                SQUARE(center.z - other_center.z);
    if ((radius + other_radius) * (radius + other_radius) > distance2) {
        keep_touch(task, shape, other, other_shape);
        hurt_shapes(task, shape, other, other_shape);
        away.x = center.x - other_center.x;
        away.y = 0.0F;
        away.z = center.z - other_center.z;
        depth = radius + other_radius - njSqrt(distance2);
        if (njScalor(&away) == 0.0F) {
            away.x = away.y = 0.0F;
            away.z = FALLBACK_Z;
        } else {
            njUnitVector(&away);
        }
        depth *= SPHERE_PUSH_SHARE;
        x = away.x * depth;
        y = away.y * depth;
        z = away.z * depth;
        collision->normal.x = away.x;
        collision->normal.z = away.z;
        if (try_push(collision, shape, other_collision, other_shape)) {
            player = player_slot_of(task);
            if (player >= 0) {
                player_records[player]->push.x = x;
                player_records[player]->push.y = y;
                player_records[player]->push.z = z;
            }
        }
        if (try_push(other_collision, other_shape, collision, shape)) {
            player = player_slot_of(other);
            if (player >= 0) {
                player_records[player]->push.x = -x;
                player_records[player]->push.y = -y;
                player_records[player]->push.z = -z;
            }
        }
    }
    return distance2;
}

/* a SHAPE_STANDING shape against an upright cylinder with PUSH_PLAYERS. it
 * lands on the cylinder's top, strikes its bottom from below, or is pushed out
 * of its side */
static Float collide_standing_cylinder(struct task *task,
                                       struct collision_shape *shape,
                                       struct task *other,
                                       struct collision_shape *other_shape)
{
    struct task_work *work = task->work;
    struct task_work *other_work;
    struct collision_work *collision = work->collision;
    struct collision_work *other_collision = other->work->collision;
    Float radius = shape->radius;
    Float other_radius = other_shape->radius;
    Float height = shape->size_y;
    Float other_height = other_shape->size_y;
    Float overhang = shape->size_z;
    NJS_POINT3 center;
    NJS_POINT3 other_center;
    NJS_VECTOR away;
    Float distance2;
    Float top;
    Float bottom;
    Float other_top;
    Float other_bottom;
    Float depth;
    Float x;
    Float z;

    other_work = other->work;
    shape_get_standing_center(&work->pos, shape, &center);
    shape_get_center(other_work, other_shape, &other_center);
    distance2 =
        SQUARE(center.x - other_center.x) + SQUARE(center.z - other_center.z);
    top = center.y + height;
    bottom = center.y - height;
    other_top = other_center.y + other_height;
    other_bottom = other_center.y - other_height;
    if (other_top > bottom && LAND_LEVEL(bottom, top) > other_top &&
        (other_radius + overhang) * (other_radius + overhang) > distance2) {
        /* @hack redundant: a read of bottom that never runs */
        if (0) {
            if (bottom) {
            }
        }
        keep_touch(task, shape, other, other_shape);
        hurt_shapes(task, shape, other, other_shape);
        collision->flags |= COLLISION_ON_TOP;
        other_collision->flags |= COLLISION_UNDER;
        if (try_push(collision, shape, other_collision, other_shape)) {
            if (collision->kind == COLLISION_PLAYER &&
                collision_next_object(task)) {
                work->flags |= WORK_ON_TOP;
            }
            other_work->flags |= WORK_UNDER;
            work->pos.y += other_top - bottom - CLEARANCE;
            work->ang.x = 0;
            work->ang.z = 0;
        }
        mark_standing(task, shape, other, other_shape);
        return distance2;
    }
    if ((radius + other_radius) * (radius + other_radius) > distance2) {
        if (top > other_bottom && other_bottom + BOTTOM_REACH > top) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
            collision->flags |= COLLISION_UNDER;
            other_collision->flags |= COLLISION_ON_TOP;
            if (try_push(collision, shape, other_collision, other_shape)) {
                if (collision->kind == COLLISION_PLAYER &&
                    collision_next_object(task)) {
                    work->flags |= WORK_UNDER;
                }
                other_work->flags |= WORK_ON_TOP;
                work->pos.y += other_bottom - top + CLEARANCE;
            }
        } else if (other_top > bottom && top > other_bottom &&
                   other_top > LAND_LEVEL(bottom, top)) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
            collision->flags |= COLLISION_SIDE;
            other_collision->flags |= COLLISION_SIDE;
            away.x = center.x - other_center.x;
            away.y = 0.0F;
            away.z = center.z - other_center.z;
            depth = njScalor(&away);
            depth -= shape->radius;
            depth -= other_shape->radius;
            if (depth < 0.0F) {
                if (njScalor(&away) == 0.0F) {
                    away.x = away.y = 0.0F;
                    away.z = FALLBACK_Z;
                } else {
                    njUnitVector(&away);
                }
                depth = -PUSH_SHORT - depth;
                x = away.x * depth * STANDING_PUSH_SHARE;
                z = away.z * depth * STANDING_PUSH_SHARE;
                collision->normal.x = away.x;
                collision->normal.z = away.z;
                if (try_push(collision, shape, other_collision, other_shape)) {
                    work->pos.x += x;
                    work->pos.z += z;
                }
            }
        }
    }
    return distance2;
}

static Bool box_contains(NJS_POINT3 *point, Float half_x, Float half_z)
{
    if (-half_x < point->x && point->x < half_x && -half_z < point->z &&
        point->z < half_z) {
        return TRUE;
    }
    return FALSE;
}

/* the sides of a box across x and z */
#define SIDE_NEG_X 0
#define SIDE_X 1
#define SIDE_NEG_Z 2
#define SIDE_Z 3

/* the side of the box from -half_x to half_x and -half_z to half_z that point
 * lies nearest, as the box's diagonals divide it */
static int box_nearest_side(NJS_POINT3 *point, Float half_x, Float half_z)
{
    if (half_x > half_z) {
        if (point->z < -point->x - half_x + half_z &&
            point->z > point->x + half_x - half_z) {
            return SIDE_NEG_X;
        }
        if (point->z > -point->x + half_x - half_z &&
            point->z < point->x - half_x + half_z) {
            return SIDE_X;
        }
        if (point->z < 0.0F) {
            return SIDE_NEG_Z;
        }
        return SIDE_Z;
    }
    if (point->x < -point->z - half_z + half_x &&
        point->x > point->z + half_z - half_x) {
        return SIDE_NEG_Z;
    }
    if (point->x > -point->z + half_z - half_x &&
        point->x < point->z - half_z + half_x) {
        return SIDE_Z;
    }
    if (point->x < 0.0F) {
        return SIDE_NEG_X;
    }
    return SIDE_X;
}

/* set pos to where task stood the step before. that is a player's place one
 * step back in its past, or any other task's position now. the function doesn't
 * read collision */
static void standing_get_last(struct task *task,
                              struct collision_work *collision, NJS_POINT3 *pos)
{
    /* @hack spelling: a switch on the kind, where an if would do */
    switch (collision_get_kind(task)) {
    case COLLISION_PLAYER:
        /* @bug memory: reads the word before player_work for a
         * COLLISION_PLAYER task that is no player's. player_slot_of gives it
         * -1, which the standing tests check before they write a player's
         * push, but nothing checks here (JMP @R3 at 0x8c019cd8).
         * player_get_past then reads player_work[-1] (MOV.L @(R0,R1),R2 at
         * 0x8c04addc), and where that is null it leaves pos unset for the
         * test to read */
        player_get_past(player_slot_of(task), 1, pos, NULL);
        break;
    default:
        *pos = task->work->pos;
        break;
    }
}

/* make pos a player's latest place in its past, where task's record is a
 * player's. the function doesn't read collision */
static void standing_set_last(struct task *task,
                              struct collision_work *collision, NJS_POINT3 *pos)
{
    /* @hack spelling: as in standing_get_last */
    switch (collision_get_kind(task)) {
    case COLLISION_PLAYER:
        player_set_past(player_slot_of(task), 0, pos, NULL);
        break;
    }
}

/* a SHAPE_STANDING shape against a SHAPE_BOX, in the box's turned axes. it
 * lands on the box's top, strikes its bottom from below, or is pushed out
 * through a side. where it stood in the box the step before, it goes out the
 * side nearest there. otherwise it goes out the side its move crossed. a
 * landing or a side push moves a player's latest place in its past too */
static Float collide_standing_box(struct task *task,
                                  struct collision_shape *shape,
                                  struct task *other,
                                  struct collision_shape *other_shape)
{
    NJS_POINT3 center;
    NJS_POINT3 other_center;
    NJS_POINT3 local;
    NJS_POINT3 last_local;
    struct task_work *work = task->work;
    struct task_work *other_work;
    struct collision_work *collision = work->collision;
    struct collision_work *other_collision = other->work->collision;
    Float radius = shape->radius;
    Float other_x = other_shape->radius;
    Float height = shape->size_y;
    Float other_y = other_shape->size_y;
    Float overhang = shape->size_z;
    Float other_z = other_shape->size_z;
    Float reach_x;
    Float reach_z;
    Float distance2;
    Float top;
    Float bottom;
    Float other_top;
    Float other_bottom;
    Float edge;
    Float cross;
    Bool hit;
    int player;
    int side;

    /* @hack spelling: other_work set in a statement after the declarations,
     * where its declaration would do */
    other_work = other->work;
    reach_x = radius + other_x;
    reach_z = radius + other_z;
    shape_get_standing_center(&work->pos, shape, &center);
    shape_get_center(other_work, other_shape, &other_center);
    distance2 =
        SQUARE(center.x - other_center.x) + SQUARE(center.z - other_center.z);
    top = center.y + height;
    bottom = center.y - height;
    other_top = other_center.y + other_y;
    other_bottom = other_center.y - other_y;
    {
        NJS_VECTOR offset;
        NJS_POINT3 last;

        offset.x = center.x - other_center.x;
        offset.y = center.y - other_center.y;
        offset.z = center.z - other_center.z;
        standing_get_last(task, collision, &last);
        last.x -= other_center.x;
        last.y = last.y + height - other_center.y;
        last.z -= other_center.z;
        njPushMatrix(&_nj_unit_matrix_);
        njRotateY(NULL, -other_work->ang.y - other_shape->ang.y);
        njCalcVector(NULL, &offset, &local);
        njCalcVector(NULL, &last, &last_local);
        njPopMatrixEx();
    }
    if (bottom < other_top && (LAND_LEVEL(bottom, top) > other_top ||
                               (last_local.y > height + other_y - LAND_SLACK &&
                                local.y - last_local.y < 0.0F))) {
        if (box_contains(&local, other_x + overhang, overhang + other_z)) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
            collision->flags |= COLLISION_ON_TOP;
            other_collision->flags |= COLLISION_UNDER;
            if (try_push(collision, shape, other_collision, other_shape)) {
                NJS_POINT3 landed;

                if (collision->kind == COLLISION_PLAYER &&
                    collision_next_object(task)) {
                    work->flags |= WORK_ON_TOP;
                }
                other_work->flags |= WORK_UNDER;
                /* @hack redundant: a read of other_top that never runs */
                if (0) {
                    if (other_top) {
                    }
                }
                landed.x = work->pos.x;
                landed.y = work->pos.y + other_top - bottom - CLEARANCE;
                landed.z = work->pos.z;
                work->pos.y = landed.y;
                standing_set_last(task, collision, &landed);
                work->ang.x = 0;
                work->ang.z = 0;
            }
            mark_standing(task, shape, other, other_shape);
        }
    } else if (top > other_bottom && other_bottom + BOTTOM_REACH > top) {
        if (box_contains(&local, reach_x * STRIKE_SHARE,
                         reach_z * STRIKE_SHARE)) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
            collision->flags |= COLLISION_UNDER;
            other_collision->flags |= COLLISION_ON_TOP;
            if (try_push(collision, shape, other_collision, other_shape)) {
                if (collision->kind == COLLISION_PLAYER &&
                    collision_next_object(task)) {
                    work->flags |= WORK_UNDER;
                }
                other_work->flags |= WORK_ON_TOP;
                player = player_slot_of(task);
                if (player >= 0) {
                    player_records[player]->push.y =
                        other_bottom - top + CLEARANCE;
                }
            }
            /* the box stands on the shape that strikes it */
            /* NOLINTNEXTLINE(readability-suspicious-call-argument) */
            mark_standing(other, other_shape, task, shape);
        }
    } else if (top > other_bottom && other_top > bottom) {
        NJS_VECTOR move;
        NJS_VECTOR normal = {0.0F, 0.0F, NO_SIDE_Z};
        NJS_VECTOR push;

        hit = FALSE;
        push = local;
        move.x = local.x - last_local.x;
        move.y = local.y - last_local.y;
        move.z = local.z - last_local.z;
        if (box_contains(&last_local, reach_x, reach_z)) {
            if (box_contains(&local, reach_x, reach_z)) {
                keep_touch(task, shape, other, other_shape);
                hurt_shapes(task, shape, other, other_shape);
                collision->flags |= COLLISION_SIDE;
                other_collision->flags |= COLLISION_SIDE;
                hit = TRUE;
                side = box_nearest_side(&last_local, reach_x, reach_z);
                switch (side) {
                case SIDE_NEG_X:
                    push.x = CLEARANCE_FINE - reach_x;
                    if (other_shape->flags & SHAPE_NO_SLIDE) {
                        push.y = last_local.y;
                        push.z = last_local.z;
                    } else {
                        push.y = local.y;
                        push.z = local.z;
                    }
                    normal.x = -1.0F;
                    normal.y = 0.0F;
                    normal.z = 0.0F;
                    break;
                case SIDE_X:
                    push.x = reach_x - CLEARANCE_FINE;
                    if (other_shape->flags & SHAPE_NO_SLIDE) {
                        push.y = last_local.y;
                        push.z = last_local.z;
                    } else {
                        push.y = local.y;
                        push.z = local.z;
                    }
                    normal.x = 1.0F;
                    normal.y = 0.0F;
                    normal.z = 0.0F;
                    break;
                case SIDE_NEG_Z:
                    push.z = CLEARANCE_FINE - reach_z;
                    if (other_shape->flags & SHAPE_NO_SLIDE) {
                        push.y = last_local.y;
                        push.x = last_local.x;
                    } else {
                        push.y = local.y;
                        push.x = local.x;
                    }
                    normal.x = 0.0F;
                    normal.y = 0.0F;
                    normal.z = -1.0F;
                    break;
                case SIDE_Z:
                    push.z = reach_z - CLEARANCE_FINE;
                    if (other_shape->flags & SHAPE_NO_SLIDE) {
                        push.y = last_local.y;
                        push.x = last_local.x;
                    } else {
                        push.y = local.y;
                        push.x = local.x;
                    }
                    normal.x = 0.0F;
                    normal.y = 0.0F;
                    normal.z = 1.0F;
                    break;
                }
            }
        } else {
            if (move.x >= 0.0F) {
                edge = -radius - other_x;
                if (last_local.x <= edge && edge <= local.x) {
                    cross = ((move.z / move.x) * (edge - last_local.x)) +
                            last_local.z;
                    if (-reach_z <= cross && cross <= reach_z) {
                        /* @hack redundant: a square of cross that never
                         * runs */
                        if (0) {
                            cross = cross * cross;
                        }
                        push.x = edge + CLEARANCE_FINE;
                        if (other_shape->flags & SHAPE_NO_SLIDE) {
                            push.y =
                                ((move.y / move.x) * (edge - last_local.x)) +
                                last_local.y;
                            push.z = cross;
                        } else {
                            push.y = local.y;
                            push.z = local.z;
                        }
                        keep_touch(task, shape, other, other_shape);
                        hurt_shapes(task, shape, other, other_shape);
                        collision->flags |= COLLISION_SIDE;
                        other_collision->flags |= COLLISION_SIDE;
                        hit = TRUE;
                        normal.x = -1.0F;
                        normal.y = 0.0F;
                        normal.z = 0.0F;
                    }
                }
            } else {
                edge = reach_x;
                if (last_local.x >= edge && local.x <= edge) {
                    cross = ((move.z / move.x) * (edge - last_local.x)) +
                            last_local.z;
                    if (cross >= -reach_z && cross <= reach_z) {
                        push.x = edge - CLEARANCE_FINE;
                        if (other_shape->flags & SHAPE_NO_SLIDE) {
                            push.y =
                                ((move.y / move.x) * (edge - last_local.x)) +
                                last_local.y;
                            push.z = cross;
                        } else {
                            push.y = local.y;
                            push.z = local.z;
                        }
                        keep_touch(task, shape, other, other_shape);
                        hurt_shapes(task, shape, other, other_shape);
                        collision->flags |= COLLISION_SIDE;
                        other_collision->flags |= COLLISION_SIDE;
                        hit = TRUE;
                        normal.x = 1.0F;
                        normal.y = 0.0F;
                        normal.z = 0.0F;
                    }
                }
            }
            if (move.z >= 0.0F) {
                edge = -radius - other_z;
                if (last_local.z <= edge && edge <= local.z) {
                    cross = ((move.x / move.z) * (edge - last_local.z)) +
                            last_local.x;
                    if (cross >= -reach_x && cross <= reach_x) {
                        push.z = edge + CLEARANCE_FINE;
                        if (other_shape->flags & SHAPE_NO_SLIDE) {
                            push.y =
                                ((move.y / move.z) * (edge - last_local.z)) +
                                last_local.y;
                            push.x = cross;
                        } else {
                            push.y = local.y;
                            push.x = local.x;
                        }
                        keep_touch(task, shape, other, other_shape);
                        hurt_shapes(task, shape, other, other_shape);
                        collision->flags |= COLLISION_SIDE;
                        other_collision->flags |= COLLISION_SIDE;
                        hit = TRUE;
                        normal.x = 0.0F;
                        normal.y = 0.0F;
                        normal.z = -1.0F;
                    }
                }
            } else {
                edge = reach_z;
                if (last_local.z >= edge && local.z <= edge) {
                    cross = ((move.x / move.z) * (edge - last_local.z)) +
                            last_local.x;
                    if (cross >= -reach_x && cross <= reach_x) {
                        push.z = edge - CLEARANCE_FINE;
                        if (other_shape->flags & SHAPE_NO_SLIDE) {
                            push.y =
                                ((move.y / move.z) * (edge - last_local.z)) +
                                last_local.y;
                            push.x = cross;
                        } else {
                            push.y = local.y;
                            push.x = local.x;
                        }
                        keep_touch(task, shape, other, other_shape);
                        hurt_shapes(task, shape, other, other_shape);
                        collision->flags |= COLLISION_SIDE;
                        other_collision->flags |= COLLISION_SIDE;
                        hit = TRUE;
                        normal.x = 0.0F;
                        normal.y = 0.0F;
                        normal.z = 1.0F;
                    }
                }
            }
        }
        if (hit) {
            if (try_push(collision, shape, other_collision, other_shape)) {
                njPushMatrix(&_nj_unit_matrix_);
                njRotateY(NULL, other_work->ang.y + other_shape->ang.y);
                njCalcVector(NULL, &push, &local);
                njCalcVector(NULL, &normal, &collision->normal);
                njPopMatrixEx();
                work->pos.x = local.x + other_center.x;
                work->pos.y = local.y + other_center.y - height;
                work->pos.z = local.z + other_center.z;
                standing_set_last(task, collision, &work->pos);
            }
        }
    }
    return distance2;
}

/* a SHAPE_STANDING shape against a SHAPE_CYLINDER2. the test lengthens the
 * cylinder's axis at each end by the standing shape's size_y. away from the
 * ends a touch pushes the two apart from the axis. within SURFACE_REACH of an
 * end, it pushes the standing shape out past the end. its center must lie
 * within the cylinder's radius plus the standing shape's size_z of the axis. it
 * lands there where the end faces up */
static Float collide_standing_cylinder2(struct task *task,
                                        struct collision_shape *shape,
                                        struct task *other,
                                        struct collision_shape *other_shape)
{
    struct task_work *work;
    struct task_work *other_work;
    struct collision_work *collision;
    struct collision_work *other_collision;
    /* @hack spelling: the three declared ahead of the shapes' sizes, where
     * any order would do */
    Float depth;
    Float distance;
    Float distance2;
    Float radius;
    Float other_radius;
    Float height;
    Float other_height;
    Float overhang;
    NJS_POINT3 center;
    NJS_POINT3 base;
    NJS_POINT3 ends[2];
    NJS_VECTOR up;
    NJS_LINE axis;
    NJS_POINT3 nearest;
    NJS_VECTOR to_end0;
    NJS_VECTOR to_end1;
    NJS_VECTOR away;

    work = task->work;
    other_work = other->work;
    collision = work->collision;
    other_collision = other_work->collision;
    radius = shape->radius;
    other_radius = other_shape->radius;
    height = shape->size_y;
    other_height = other_shape->size_y;
    overhang = shape->size_z;
    collision_get_shape_center(work, shape, &center);
    shape_get_up(other_work, other_shape, &up);
    shape_get_center(other_work, other_shape, &base);
    ends[0].x = base.x + ((height + other_height) * up.x);
    ends[0].y = base.y + ((height + other_height) * up.y);
    ends[0].z = base.z + ((height + other_height) * up.z);
    ends[1].x = base.x - ((height + other_height) * up.x);
    ends[1].y = base.y - ((height + other_height) * up.y);
    ends[1].z = base.z - ((height + other_height) * up.z);
    axis.px = ends[1].x;
    axis.py = ends[1].y;
    axis.pz = ends[1].z;
    axis.vx = up.x;
    axis.vy = up.y;
    axis.vz = up.z;
    distance = njDistanceP2L(&center, &axis, &nearest);
    to_end0.x = ends[0].x - nearest.x;
    to_end0.y = ends[0].y - nearest.y;
    to_end0.z = ends[0].z - nearest.z;
    to_end1.x = ends[1].x - nearest.x;
    to_end1.y = ends[1].y - nearest.y;
    to_end1.z = ends[1].z - nearest.z;
    if (distance < radius + other_radius) {
        if (njInnerProduct(&to_end0, &to_end1) < 0.0F) {
            distance2 = SQUARE(base.x - nearest.x) +
                        SQUARE(base.y - nearest.y) + SQUARE(base.z - nearest.z);
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
            other_collision->flags |= COLLISION_SIDE;
            if ((height + other_height - SURFACE_REACH) *
                    (height + other_height - SURFACE_REACH) >
                distance2) {
                away.x = center.x - nearest.x;
                away.y = center.y - nearest.y;
                away.z = center.z - nearest.z;
                depth = radius - distance + other_radius - PUSH_SHORT;
                if (njScalor(&away) == 0.0F) {
                    away.x = away.y = 0.0F;
                    away.z = FALLBACK_Z;
                } else {
                    njUnitVector(&away);
                }
                away.x *= depth;
                away.y *= depth;
                away.z *= depth;
                if (try_push(collision, shape, other_collision, other_shape)) {
                    work->pos.x += away.x;
                    work->pos.y += away.y;
                    work->pos.z += away.z;
                }
                if (try_push(other_collision, other_shape, collision, shape)) {
                    other_work->pos.x -= away.x;
                    other_work->pos.y -= away.y;
                    other_work->pos.z -= away.z;
                }
            } else if (other_radius + overhang > distance) {
                NJS_VECTOR y_axis = {0.0F, 1.0F, 0.0F};

                away = nearest;
                njSubVector(&away, &base);
                if (njScalor(&away) == 0.0F) {
                    away.x = away.y = 0.0F;
                    away.z = FALLBACK_Z;
                } else {
                    njUnitVector(&away);
                }
                if (njInnerProduct(&y_axis, &away) > END_UP) {
                    if (collision->kind == COLLISION_PLAYER &&
                        collision_next_object(task)) {
                        work->flags |= WORK_ON_TOP;
                    }
                    other_work->flags |= WORK_UNDER;
                    collision->flags |= COLLISION_ON_TOP;
                    other_collision->flags |= COLLISION_UNDER;
                }
                away.x *= height + other_height - njSqrt(distance2);
                away.y *= height + other_height - njSqrt(distance2);
                away.z *= height + other_height - njSqrt(distance2);
                mark_standing(task, shape, other, other_shape);
                if (try_push(collision, shape, other_collision, other_shape)) {
                    njAddVector(&work->pos, &away);
                    work->flags &= ~PLAYER_WORK_UNK_8;
                }
            }
        }
    }
    return distance;
}

/* set local to pos less center, turned by ang about y */
static void point_to_local(NJS_POINT3 *pos, NJS_POINT3 *center, Angle ang,
                           NJS_POINT3 *local)
{
    NJS_VECTOR offset;
    NJS_VECTOR turned;

    offset.x = pos->x - center->x;
    offset.y = pos->y - center->y;
    offset.z = pos->z - center->z;
#ifdef NONMATCHING
    /* for a finite vector, the kernel gives the bits of the matrix calls below,
     * as engine/dc/turn.h says */
    zam_vec3_turn_y(ang, &offset.x, &turned.x);
#else
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, ang);
    njCalcVector(NULL, &offset, &turned);
    njPopMatrix(1);
#endif
    local->x = turned.x;
    local->y = turned.y;
    local->z = turned.z;
}

/* point_to_local undone in place: turn local back by ang about y, then add
 * center. pos is unread */
static void point_from_local(NJS_POINT3 *pos, NJS_POINT3 *center, Angle ang,
                             NJS_POINT3 *local)
{
    NJS_VECTOR turned;

#ifdef NONMATCHING
    /* for a finite vector, the kernel gives the bits of the matrix calls below,
     * as engine/dc/turn.h says */
    zam_vec3_turn_y(-ang, &local->x, &turned.x);
#else
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, -ang);
    njCalcVector(NULL, local, &turned);
    njPopMatrixEx();
#endif
    local->x = turned.x + center->x;
    local->y = turned.y + center->y;
    local->z = turned.z + center->z;
}

/* a player's SHAPE_STANDING shape against a SHAPE_BOX2. the test sweeps it
 * SWEEP_STEP at a time from where the player stood the step before. the sweep
 * ends at the first step that meets the box. there the shape lands on the box's
 * upper face or strikes its lower one. otherwise it goes out the nearest side.
 * the test turns the box back by the quarter turns nearest its tilt about z.
 * where that is one quarter turn or three, the box's x and y sizes trade
 * places. the shape's record must be a player's, unchecked */
static void collide_standing_box2(struct task *task,
                                  struct collision_shape *shape,
                                  struct task *other,
                                  struct collision_shape *other_shape)
{
    NJS_POINT3 position;
    NJS_POINT3 center;
    struct angles angle;
    NJS_POINT3 previous;
    NJS_POINT3 step;
    NJS_POINT3 local;
    NJS_POINT3 left;
    NJS_POINT3 right;
    NJS_POINT3 point;
    NJS_POINT3 turned;
    NJS_POINT3 correction;
    struct task_work *work = task->work;
    struct task_work *other_work = other->work;
    struct collision_work *collision = work->collision;
    struct collision_work *other_collision = other_work->collision;
    Float radius = shape->radius;
    Float box_x = other_shape->radius;
    Float height = shape->size_y;
    Float box_y = other_shape->size_y;
    Float overhang = shape->size_z;
    Float box_z = other_shape->size_z;
    Float distance;
    Float bottom;
    Float top;
    Float upper_face;
    Float lower_face;
    Float side1;
    Float side2;
    Float along;
    Float across;
    int count;
    Angle tilt;

    player_get_past(player_slot_of(collision->owner), 1, &previous, NULL);
    step.x = work->pos.x - previous.x;
    step.y = work->pos.y - previous.y;
    step.z = work->pos.z - previous.z;
    distance = njSqrt(SQUARE(step.x) + SQUARE(step.y) + SQUARE(step.z));
    if (distance > 0.0F) {
        step.x /= distance;
        step.y /= distance;
        step.z /= distance;
        step.x *= SWEEP_STEP;
        step.y *= SWEEP_STEP;
        step.z *= SWEEP_STEP;
    } else {
        distance = SWEEP_STILL;
        step.x = 0.0F;
        step.y = 0.0F;
        step.z = 0.0F;
    }
    count = (int)ceilf(distance * SWEEP_STEPS_PER_UNIT);
    position.x = previous.x;
    position.y = previous.y + height;
    position.z = previous.z;
    shape_get_box_center(other_work, other_shape, &center);
    if ((other_shape->flags & SHAPE_IN_WORLD) ||
        (other_shape->flags & SHAPE_UNTURNED)) {
        angle.y = other_shape->ang.y;
        angle.z = other_shape->ang.z;
    } else {
        angle.y = other_work->ang.y + other_shape->ang.y;
        angle.z = other_shape->ang.z;
    }
    /* the tilt plus an eighth turn falls in the quarter that starts at the
     * tilt's nearest quarter turn */
    /* @hack spelling: angle.x holds the tilt plus an eighth turn, where a
     * local of its own would do */
    angle.x = (Uint16)(angle.z + EIGHTH_TURN);
    tilt = angle.z;
    if (angle.x >= QUARTER_TURN && angle.x < HALF_TURN) {
        box_x = other_shape->size_y;
        box_y = other_shape->radius;
        angle.z = tilt - QUARTER_TURN;
    } else if (angle.x >= HALF_TURN && angle.x < HALF_TURN + QUARTER_TURN) {
        angle.z = tilt - HALF_TURN;
    } else if (angle.x >= HALF_TURN + QUARTER_TURN &&
               angle.x <= FULL_TURN - 1) {
        box_x = other_shape->size_y;
        box_y = other_shape->radius;
        angle.z = tilt - (HALF_TURN + QUARTER_TURN);
    }
    while (count > 0) {
        count--;
        if (count > 0) {
            position.x += step.x;
            position.y += step.y;
            position.z += step.z;
        } else {
            position.x = work->pos.x;
            position.y = work->pos.y + height;
            position.z = work->pos.z;
        }
        point_to_local(&position, &center, -angle.y, &local);
        if (local.z > -radius - box_z && local.z < radius + box_z) {
            njPushMatrix(&_nj_unit_matrix_);
            njRotateZ(NULL, angle.z);
            point.x = -box_x - overhang;
            point.y = box_y;
            point.z = 0.0F;
            njCalcPoint(NULL, &point, &left);
            point.x = box_x + overhang;
            point.y = box_y;
            point.z = 0.0F;
            njCalcPoint(NULL, &point, &right);
            njPopMatrix(1);
            top = local.y + height;
            bottom = local.y - height;
            upper_face = (njTan(angle.z) * (local.x - left.x)) + left.y;
            lower_face = (njTan(angle.z) * (local.x + left.x)) - left.y;
            if (-overhang - box_z < local.z && local.z < overhang + box_z) {
                if (left.x < local.x && local.x < right.x) {
                    if (upper_face - SURFACE_REACH < bottom &&
                        bottom <= upper_face) {
                        keep_touch(task, shape, other, other_shape);
                        hurt_shapes(task, shape, other, other_shape);
                        other_work->flags |= WORK_UNDER;
                        collision->flags |= COLLISION_ON_TOP;
                        other_collision->flags |= COLLISION_UNDER;
                        if (try_push(collision, shape, other_collision,
                                     other_shape)) {
                            work->pos.y += upper_face - bottom - CLEARANCE_FINE;
                            work->ang.x = 0;
                            work->ang.z = 0;
                            work->flags &= ~PLAYER_WORK_UNK_8;
                            if (collision->kind == COLLISION_PLAYER &&
                                collision_next_object(task)) {
                                work->flags |= WORK_ON_TOP;
                            }
                        }
                        mark_standing(task, shape, other, other_shape);
                        player_set_past(player_slot_of(collision->owner), 0,
                                        &work->pos, NULL);
                        break;
                    }
                }
            }
            if (-right.x < local.x && local.x < -left.x && lower_face <= top &&
                top <= lower_face + SURFACE_REACH) {
                keep_touch(task, shape, other, other_shape);
                hurt_shapes(task, shape, other, other_shape);
                other_work->flags |= WORK_ON_TOP;
                collision->flags |= COLLISION_UNDER;
                other_collision->flags |= COLLISION_ON_TOP;
                if (try_push(collision, shape, other_collision, other_shape)) {
                    if (!(work->flags & PLAYER_WORK_ON_GROUND)) {
                        work->pos.y += lower_face - local.y - height;
                    }
                    work->flags &= ~PLAYER_WORK_UNK_8;
                    if (collision->kind == COLLISION_PLAYER &&
                        collision_next_object(task)) {
                        work->flags |= WORK_UNDER;
                    }
                }
                player_set_past(player_slot_of(collision->owner), 0, &work->pos,
                                NULL);
                break;
            }
#ifdef NONMATCHING
            /* for a finite vector, the kernel gives the bits of the matrix
             * calls below, as engine/dc/turn.h says */
            zam_vec3_turn_z(-angle.z, &local.x, &turned.x);
#else
            njPushMatrix(&_nj_unit_matrix_);
            njRotateZ(NULL, -angle.z);
            njCalcVector(NULL, &local, &turned);
            njPopMatrix(1);
#endif
            if (turned.x > -radius - box_x && turned.x < radius + box_x &&
                turned.y < radius + box_y - BOX2_TRIM_TOP &&
                turned.y > BOX2_TRIM_BOTTOM - radius - box_y) {
                keep_touch(task, shape, other, other_shape);
                hurt_shapes(task, shape, other, other_shape);
                collision->flags |= COLLISION_SIDE;
                other_collision->flags |= COLLISION_SIDE;
                if (try_push(collision, shape, other_collision, other_shape)) {
                    along = (radius + box_x) * turned.z;
                    across = (radius + box_z) * turned.x;
                    side1 = along - across;
                    side2 = along + across;
                    if (side1 > 0.0F) {
                        if (side2 > 0.0F) {
                            correction.x = 0.0F;
                            correction.y = 0.0F;
                            correction.z =
                                ((radius + box_z) * BOX2_SIDE_SHARE) - turned.z;
                        } else {
                            correction.x =
                                ((-radius - box_x) * BOX2_SIDE_SHARE) -
                                turned.x;
                            correction.y = 0.0F;
                            correction.z = 0.0F;
                        }
                    } else if (side2 < 0.0F) {
                        correction.x = 0.0F;
                        correction.y = 0.0F;
                        correction.z =
                            ((-radius - box_z) * BOX2_SIDE_SHARE) - turned.z;
                    } else {
                        correction.x =
                            ((radius + box_x) * BOX2_SIDE_SHARE) - turned.x;
                        correction.y = 0.0F;
                        correction.z = 0.0F;
                    }
                    njPushMatrix(&_nj_unit_matrix_);
                    njRotateZ(NULL, angle.z);
                    njRotateY(NULL, angle.y);
                    njCalcVector(NULL, &correction, &local);
                    njPopMatrix(1);
                    work->pos.x += local.x * BOX2_PUSH_SHARE;
                    if (!(work->flags & PLAYER_WORK_ON_GROUND)) {
                        work->pos.y += local.y * BOX2_PUSH_SHARE;
                    }
                    work->pos.z += local.z * BOX2_PUSH_SHARE;
                    if (njScalor(&local) > 0.0F) {
                        collision->normal = local;
                        njUnitVector(&collision->normal);
                    }
                }
                player_set_past(player_slot_of(collision->owner), 0, &work->pos,
                                NULL);
                break;
            } else {
                /* @hack redundant: an empty else */
            }
        }
    }
}

/* a player's SHAPE_STANDING shape against a SHAPE_WALL, swept as
 * collide_standing_box2 sweeps. the sweep ends at the first step where the
 * shape reaches through the wall's face, within its span. there the test puts
 * the task back in front of the wall, along the wall's z alone. the shape's
 * record must be a player's, unchecked */
static Float collide_standing_wall(struct task *task,
                                   struct collision_shape *shape,
                                   struct task *other,
                                   struct collision_shape *other_shape)
{
    NJS_POINT3 position;
    NJS_POINT3 center;
    struct angles angle;
    NJS_POINT3 previous;
    NJS_POINT3 step;
    NJS_POINT3 local;
    struct task_work *work = task->work;
    struct task_work *other_work = other->work;
    struct collision_work *collision = work->collision;
    Float radius = shape->radius;
    Float wall_x = other_shape->radius;
    Float height = shape->size_y;
    Float wall_y = other_shape->size_y;
    Float wall_z = other_shape->size_z;
    /* @hack redundant: a read of the wall's record's radius, which nothing
     * uses */
    Float other_radius =
        other_work->collision
            ->radius; /* NOLINT(clang-diagnostic-unused-variable) */
    Float distance;
    int count;

    player_get_past(player_slot_of(collision->owner), 1, &previous, NULL);
    step.x = work->pos.x - previous.x;
    step.y = work->pos.y - previous.y;
    step.z = work->pos.z - previous.z;
    if ((other_shape->flags & SHAPE_IN_WORLD) ||
        (other_shape->flags & SHAPE_UNTURNED)) {
        angle.y = other_shape->ang.y;
    } else {
        angle.y = other_work->ang.y + other_shape->ang.y;
    }
    distance = njSqrt(SQUARE(step.x) + SQUARE(step.y) + SQUARE(step.z));
    if (distance > 0.0F) {
        step.x /= distance;
        step.y /= distance;
        step.z /= distance;
        step.x *= SWEEP_STEP;
        step.y *= SWEEP_STEP;
        step.z *= SWEEP_STEP;
    } else {
        distance = SWEEP_STILL;
        step.x = 0.0F;
        step.y = 0.0F;
        step.z = 0.0F;
    }
    count = (int)ceilf(distance * SWEEP_STEPS_PER_UNIT);
    position.x = previous.x;
    position.y = previous.y + height;
    position.z = previous.z;
    shape_get_box_center(other_work, other_shape, &center);
    while (count >= 0) {
        count--;
        if (count > 0) {
            position.x += step.x;
            position.y += step.y;
            position.z += step.z;
        } else {
            position.x = work->pos.x;
            position.y = work->pos.y + height;
            position.z = work->pos.z;
        }
        point_to_local(&position, &center, -angle.y, &local);
        if (local.y + height > -wall_y && local.y - height < wall_y &&
            local.x + radius > -wall_x && local.x - radius < wall_x) {
            if (local.z - radius < wall_z && local.z - radius >= 0.0F) {
                continue;
            }
            if (local.z - radius < 0.0F && local.z > -WALL_REACH) {
                local.z = radius + WALL_CLEARANCE;
                point_from_local(&position, &center, -angle.y, &local);
                local.y -= height;
                work->pos.x = local.x;
                work->pos.z = local.z;
                player_set_past(player_slot_of(collision->owner), 0, &work->pos,
                                NULL);
                /* @hack implicit: no return value where the task is put
                 * back, where return distance would do. collide_shapes
                 * drops the result */
#ifdef NONMATCHING
                /* this side returns the distance, as a Float function's
                 * return should. nothing reads it */
                return distance;
#else
                return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
            }
        }
    }
    return distance;
}

/* a SHAPE_STANDING shape against a SHAPE_AXIS_BOX, tested as
 * collide_standing_box tests a SHAPE_BOX. the test's axes make the up axis y.
 * here a strike on the box's bottom moves the task rather than setting a
 * player's push. each move takes a player's latest place in its past too */
static Float collide_standing_axis_box(struct task *task,
                                       struct collision_shape *shape,
                                       struct task *other,
                                       struct collision_shape *other_shape)
{
    NJS_POINT3 local;
    NJS_POINT3 last_local;
    struct task_work *work;
    struct task_work *other_work;
    /* @hack spelling: each work set inside the load of its record, where a
     * statement of its own would do */
    struct collision_work *collision = (work = task->work)->collision;
    struct collision_work *other_collision =
        (other_work = other->work)->collision;
    Float radius;
    Float other_x = other_shape->radius;
    Float height;
    Float other_y = other_shape->size_y;
    Float overhang;
    Float other_z = other_shape->size_z;
    NJS_VECTOR push = {0.0F, 0.0F, 0.0F};
    NJS_POINT3 center;
    NJS_POINT3 other_center;
    NJS_POINT3 last_center;
    Bool hit = FALSE;
    NJS_VECTOR normal = {0.0F, 0.0F, NO_SIDE_Z};
    /* where the task's foot was the step before. the side tests reuse it for
     * the move from last_local to local */
    NJS_POINT3 last;
    Float reach_x;
    Float reach_z;
    Float low_x;
    Float low_z;
    Float edge;
    Float cross;
    int sign;
    int axis;
    int side;

    standing_get_last(task, collision, &last);
    shape_get_standing_center(&last, shape, &last_center);
    shape_get_standing_center(&work->pos, shape, &center);
    shape_get_axis_box_center(other_work, other_shape, &other_center);
    radius = shape->radius;
    height = shape->size_y;
    overhang = shape->size_z;
    axis = collision_up_axis;
#ifdef NONMATCHING
    /* GCC can't see that every path sets sign. the up axis is one of the six
     * the switch has cases for, as collision.h requires */
    sign = -1;
#endif
    /* @hack redundant: a label nothing jumps to */
first_switch: /* NOLINT(clang-diagnostic-unused-label) */
    switch (axis) {
    case COLLISION_UP_NEG_X:
        local.x = center.z - other_center.z;
        local.y = center.x - other_center.x;
        local.z = center.y - other_center.y;
        local.y *= -1.0F;
        last_local.x = last_center.z - other_center.z;
        last_local.y = last_center.x - other_center.x;
        last_local.z = last_center.y - other_center.y;
        last_local.y *= -1.0F;
        other_x = other_shape->size_z;
        other_y = other_shape->radius;
        other_z = other_shape->size_y;
        sign = 1;
        break;
    case COLLISION_UP_X:
        local.x = center.z - other_center.z;
        local.y = center.x - other_center.x;
        local.z = center.y - other_center.y;
        last_local.x = last_center.z - other_center.z;
        last_local.y = last_center.x - other_center.x;
        last_local.z = last_center.y - other_center.y;
        other_x = other_shape->size_z;
        other_y = other_shape->radius;
        other_z = other_shape->size_y;
        sign = -1;
        break;
    case COLLISION_UP_NEG_Y:
        local.x = center.x - other_center.x;
        local.y = center.y - other_center.y;
        local.z = center.z - other_center.z;
        local.y *= -1.0F;
        last_local.x = last_center.x - other_center.x;
        last_local.y = last_center.y - other_center.y;
        last_local.z = last_center.z - other_center.z;
        last_local.y *= -1.0F;
        other_x = other_shape->radius;
        other_y = other_shape->size_y;
        other_z = other_shape->size_z;
        sign = 1;
        break;
    case COLLISION_UP_Y:
        local.x = center.x - other_center.x;
        local.y = center.y - other_center.y;
        local.z = center.z - other_center.z;
        last_local.x = last_center.x - other_center.x;
        last_local.y = last_center.y - other_center.y;
        last_local.z = last_center.z - other_center.z;
        other_x = other_shape->radius;
        other_y = other_shape->size_y;
        other_z = other_shape->size_z;
        sign = -1;
        break;
    case COLLISION_UP_NEG_Z:
        /* @hack redundant: a read of the y difference that never runs */
        if (0) {
            if (center.y - other_center.y) {
            }
        }
        local.x = center.y - other_center.y;
        local.y = center.z - other_center.z;
        local.z = center.x - other_center.x;
        local.y *= -1.0F;
        last_local.x = last_center.y - other_center.y;
        last_local.y = last_center.z - other_center.z;
        last_local.z = last_center.x - other_center.x;
        last_local.y *= -1.0F;
        other_x = other_shape->size_y;
        other_y = other_shape->size_z;
        other_z = other_shape->radius;
        sign = 1;
        break;
    case COLLISION_UP_Z:
        local.x = center.y - other_center.y;
        local.y = center.z - other_center.z;
        local.z = center.x - other_center.x;
        last_local.x = last_center.y - other_center.y;
        last_local.y = last_center.z - other_center.z;
        last_local.z = last_center.x - other_center.x;
        other_x = other_shape->size_y;
        other_y = other_shape->size_z;
        other_z = other_shape->radius;
        sign = -1;
        break;
    }
    if (other_y > local.y - height &&
        (LAND_LEVEL(local.y - height, local.y + height) > other_y ||
         (last_local.y - height > other_y - LAND_SLACK &&
          local.y < last_local.y))) {
        if (box_contains(&local, other_x + overhang, overhang + other_z)) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
            collision->flags |= COLLISION_ON_TOP;
            other_collision->flags |= COLLISION_UNDER;
            if (try_push(collision, shape, other_collision, other_shape)) {
                if (collision->kind == COLLISION_PLAYER &&
                    collision_next_object(task)) {
                    work->flags |= WORK_ON_TOP;
                }
                other_work->flags |= WORK_UNDER;
                push.y = height + other_y - local.y - CLEARANCE_FINE;
                hit = TRUE;
            }
            /* @hack redundant: a second other_work set and read that never
             * runs, across a call */
            if (0) {
                other_work = (struct task_work *)&last;
                collide_players();
                if (other_work) {
                }
            }
            mark_standing(task, shape, other, other_shape);
        }
    } else {
        /* @hack spelling: reach_z set ahead of reach_x */
        reach_z = radius + other_z;
        reach_x = radius + other_x;
        if (local.y + height > -other_y &&
            BOTTOM_REACH - other_y > local.y + height) {
            if (box_contains(&local, radius + other_x, radius + other_z)) {
                keep_touch(task, shape, other, other_shape);
                hurt_shapes(task, shape, other, other_shape);
                collision->flags |= COLLISION_UNDER;
                other_collision->flags |= COLLISION_ON_TOP;
                if (try_push(collision, shape, other_collision, other_shape)) {
                    if (collision->kind == COLLISION_PLAYER &&
                        collision_next_object(task)) {
                        work->flags |= WORK_UNDER;
                    }
                    other_work->flags |= WORK_ON_TOP;
                    push.y = -height - other_y - local.y;
                    hit = TRUE;
                }
            }
        } else if (local.y + height > -other_y && other_y > local.y - height) {
            /* @hack spelling: other_y holds the normals' 0, where 0.0F would
             * do, and so is returned */
            other_y = 0.0F;
            last.x = local.x - last_local.x;
            last.y = local.y - last_local.y;
            last.z = local.z - last_local.z;
            low_x = -reach_x;
            low_z = -reach_z;
            if (box_contains(&last_local, reach_x, reach_z)) {
                if (box_contains(&local, reach_x, reach_z)) {
                    keep_touch(task, shape, other, other_shape);
                    hurt_shapes(task, shape, other, other_shape);
                    collision->flags |= COLLISION_SIDE;
                    other_collision->flags |= COLLISION_SIDE;
                    hit = TRUE;
                    side = box_nearest_side(&last_local, reach_x, reach_z);
                    switch (side) {
                    case SIDE_NEG_X:
                        push.x = low_x - local.x + CLEARANCE_FINE;
                        if (other_shape->flags & SHAPE_NO_SLIDE) {
                            push.y = last_local.y - local.y;
                            push.z = last_local.z - local.z;
                        }
                        normal.x = -1.0F;
                        normal.y = other_y;
                        normal.z = other_y;
                        break;
                    case SIDE_X:
                        push.x = reach_x - local.x - CLEARANCE_FINE;
                        if (other_shape->flags & SHAPE_NO_SLIDE) {
                            push.y = last_local.y - local.y;
                            push.z = last_local.z - local.z;
                        }
                        normal.x = 1.0F;
                        normal.y = other_y;
                        normal.z = other_y;
                        break;
                    case SIDE_NEG_Z:
                        push.z = low_z - local.z + CLEARANCE_FINE;
                        if (other_shape->flags & SHAPE_NO_SLIDE) {
                            push.y = last_local.y - local.y;
                            push.x = last_local.x - local.x;
                        }
                        normal.x = other_y;
                        normal.y = other_y;
                        normal.z = -1.0F;
                        break;
                    case SIDE_Z:
                        push.z = reach_z - local.z - CLEARANCE_FINE;
                        if (other_shape->flags & SHAPE_NO_SLIDE) {
                            push.y = last_local.y - local.y;
                            push.x = last_local.x - local.x;
                        }
                        normal.x = other_y;
                        normal.y = other_y;
                        normal.z = 1.0F;
                        break;
                    }
                }
            } else {
                if (last.x >= 0.0F) {
                    edge = -radius - other_x;
                    if (last_local.x <= edge && edge <= local.x) {
                        cross = ((last.z / last.x) * (edge - last_local.x)) +
                                last_local.z;
                        if (cross >= low_z && cross <= reach_z) {
                            /* @hack redundant: a read of low_z that never
                             * runs */
                            if (0) {
                                if (low_z) {
                                }
                            }
                            push.x = edge - local.x + CLEARANCE_FINE;
                            if (other_shape->flags & SHAPE_NO_SLIDE) {
                                push.y = last_local.y +
                                         ((last.y / last.x) *
                                          (edge - last_local.x)) -
                                         local.y;
                                push.z = cross - local.z;
                            }
                            keep_touch(task, shape, other, other_shape);
                            hurt_shapes(task, shape, other, other_shape);
                            collision->flags |= COLLISION_SIDE;
                            other_collision->flags |= COLLISION_SIDE;
                            hit = TRUE;
                            normal.x = -1.0F;
                            normal.y = other_y;
                            normal.z = other_y;
                        }
                    }
                } else {
                    edge = reach_x;
                    if (last_local.x >= edge && local.x <= edge) {
                        cross = ((last.z / last.x) * (edge - last_local.x)) +
                                last_local.z;
                        if (cross >= low_z && cross <= reach_z) {
                            push.x = edge - local.x - CLEARANCE_FINE;
                            if (other_shape->flags & SHAPE_NO_SLIDE) {
                                push.y = last_local.y +
                                         ((last.y / last.x) *
                                          (edge - last_local.x)) -
                                         local.y;
                                push.z = cross - local.z;
                            }
                            keep_touch(task, shape, other, other_shape);
                            hurt_shapes(task, shape, other, other_shape);
                            collision->flags |= COLLISION_SIDE;
                            other_collision->flags |= COLLISION_SIDE;
                            hit = TRUE;
                            normal.x = 1.0F;
                            normal.y = other_y;
                            normal.z = other_y;
                        }
                    }
                }
                if (last.z >= 0.0F) {
                    edge = -radius - other_z;
                    if (last_local.z <= edge && edge <= local.z) {
                        cross = ((last.x / last.z) * (edge - last_local.z)) +
                                last_local.x;
                        if (cross >= low_x && cross <= reach_x) {
                            push.z = edge - local.z + CLEARANCE_FINE;
                            if (other_shape->flags & SHAPE_NO_SLIDE) {
                                push.y = last_local.y +
                                         ((last.y / last.z) *
                                          (edge - last_local.z)) -
                                         local.y;
                                push.x = cross - local.x;
                            }
                            keep_touch(task, shape, other, other_shape);
                            hurt_shapes(task, shape, other, other_shape);
                            collision->flags |= COLLISION_SIDE;
                            other_collision->flags |= COLLISION_SIDE;
                            hit = TRUE;
                            normal.x = other_y;
                            normal.y = other_y;
                            normal.z = -1.0F;
                        }
                    }
                } else {
                    edge = reach_z;
                    /* @hack redundant: a read of reach_z that never runs */
                    if (0) {
                        if (reach_z) {
                        }
                    }
                    if (last_local.z >= edge && local.z <= edge) {
                        cross = ((last.x / last.z) * (edge - last_local.z)) +
                                last_local.x;
                        if (cross >= low_x && cross <= reach_x) {
                            push.z = edge - local.z - CLEARANCE_FINE;
                            if (other_shape->flags & SHAPE_NO_SLIDE) {
                                push.y = last_local.y +
                                         ((last.y / last.z) *
                                          (edge - last_local.z)) -
                                         local.y;
                                push.x = cross - local.x;
                            }
                            keep_touch(task, shape, other, other_shape);
                            hurt_shapes(task, shape, other, other_shape);
                            collision->flags |= COLLISION_SIDE;
                            other_collision->flags |= COLLISION_SIDE;
                            hit = TRUE;
                            normal.x = other_y;
                            normal.y = other_y;
                            normal.z = 1.0F;
                        }
                    }
                }
            }
        }
    }
    if (hit) {
        if (try_push(collision, shape, other_collision, other_shape)) {
            if (sign > 0) {
                push.y *= -1.0F;
            }
            axis = collision_up_axis;
            /* @hack spelling: each case's body in braces, with its break
             * inside */
            switch (axis) {
            case COLLISION_UP_NEG_X: {
                work->pos.x += push.y;
                work->pos.y += push.z;
                work->pos.z += push.x;
                collision->normal.x = normal.y;
                collision->normal.y = normal.z;
                collision->normal.z = normal.x;
                break;
            }
            case COLLISION_UP_X: {
                work->pos.x += push.y;
                work->pos.y += push.z;
                work->pos.z += push.x;
                collision->normal.x = normal.y;
                collision->normal.y = normal.z;
                collision->normal.z = normal.x;
                break;
            }
            case COLLISION_UP_NEG_Y: {
                work->pos.x += push.x;
                work->pos.y += push.y;
                work->pos.z += push.z;
                collision->normal.x = normal.x;
                collision->normal.y = normal.y;
                collision->normal.z = normal.z;
                break;
            }
            case COLLISION_UP_Y: {
                work->pos.x += push.x;
                work->pos.y += push.y;
                work->pos.z += push.z;
                collision->normal.x = normal.x;
                collision->normal.y = normal.y;
                collision->normal.z = normal.z;
                break;
            }
            case COLLISION_UP_NEG_Z: {
                /* @hack redundant: a read of push.z that never runs */
                if (0) {
                    if (push.z) {
                    }
                }
                work->pos.x += push.z;
                work->pos.y += push.x;
                work->pos.z += push.y;
                collision->normal.x = normal.z;
                collision->normal.y = normal.x;
                collision->normal.z = normal.y;
                break;
            }
            case COLLISION_UP_Z: {
                work->pos.x += push.z;
                work->pos.y += push.x;
                work->pos.z += push.y;
                collision->normal.x = normal.z;
                collision->normal.y = normal.x;
                collision->normal.z = normal.y;
                break;
            }
            }
            standing_set_last(task, collision, &work->pos);
        }
    }
    /* @hack redundant: a read of radius that never runs */
    if (0) {
        if (radius) {
        }
    }
    return other_y;
}

/* two spheres. a touch pushes each task SPHERE_PUSH_SHARE of the overlap away
 * from the other, across x and z */
static Float collide_sphere_sphere(struct task *task,
                                   struct collision_shape *shape,
                                   struct task *other,
                                   struct collision_shape *other_shape)
{
    NJS_POINT3 center;
    NJS_POINT3 other_center;
    NJS_VECTOR away;
    struct task_work *work = task->work;
    struct task_work *other_work = other->work;
    struct collision_work *collision;
    struct collision_work *other_collision;
    Float radius = shape->radius;
    Float other_radius = other_shape->radius;
    Float distance2;
    Float depth;
    Float x;
    Float y;
    Float z;
    Float *part;

    shape_get_check_center(work, shape, &center);
    shape_get_check_center(other_work, other_shape, &other_center);
    distance2 = mesh_point_distance2(&center, &other_center);
    if ((radius + other_radius) * (radius + other_radius) > distance2) {
        keep_touch(task, shape, other, other_shape);
        hurt_shapes(task, shape, other, other_shape);
        away.x = center.x - other_center.x;
        away.y = 0.0F;
        away.z = center.z - other_center.z;
        depth = radius + other_radius - njSqrt(distance2);
        if (njScalor(&away) == 0.0F) {
            away.x = away.y = 0.0F;
            away.z = FALLBACK_Z;
        } else {
            njUnitVector(&away);
        }
        depth *= SPHERE_PUSH_SHARE;
        /* @hack spelling: away read through a stepped pointer, where its
         * members would do */
        part = &away.x;
        x = *part++ * depth;
        y = *part++ * depth;
        z = *part * depth;
        collision = work->collision;
        other_collision = other_work->collision;
        collision->normal.x = away.x;
        collision->normal.z = away.z;
        if (try_push(collision, shape, other_collision, other_shape)) {
            work->pos.x += x;
            work->pos.y += y;
            work->pos.z += z;
        }
        if (try_push(other_collision, other_shape, collision, shape)) {
            other_work->pos.x -= x;
            other_work->pos.y -= y;
            other_work->pos.z -= z;
        }
    }
    return distance2;
}

/* a sphere against an upright cylinder, taking the sphere as a cylinder as tall
 * as it is wide. either lands on the other where its bottom lies above the
 * other's SPHERE_LAND_LEVEL. otherwise a touch pushes the sphere
 * CYLINDER_PUSH_SHARE of the overlap away across x and z. it pushes the
 * cylinder back by the unit direction */
static void collide_sphere_cylinder(struct task *task,
                                    struct collision_shape *shape,
                                    struct task *other,
                                    struct collision_shape *other_shape)
{
    NJS_POINT3 center;
    NJS_POINT3 base;
    NJS_VECTOR away;
    struct task_work *work = task->work;
    struct task_work *other_work = other->work;
    /* @hack spelling: the records reached through the tasks again, where
     * work's and other_work's would do */
    struct collision_work *collision = task->work->collision;
    struct collision_work *other_collision = other->work->collision;
    Float radius = shape->radius;
    /* @hack spelling: the sphere's half height read from its radius again,
     * where radius would do */
    Float height = shape->radius;
    Float other_radius = other_shape->radius;
    Float other_height = other_shape->size_y;
    Float distance2;
    Float depth;
    Float top;
    Float bottom;
    Float other_top;
    Float other_bottom;
    Float x;
    Float y;
    Float z;

    shape_get_check_center(work, shape, &center);
    shape_get_center(other_work, other_shape, &base);
    distance2 = SQUARE(center.x - base.x) + SQUARE(center.z - base.z);
    top = center.y + height;
    bottom = center.y - height;
    other_top = base.y + other_height;
    other_bottom = base.y - other_height;
    if ((radius + other_radius) * (radius + other_radius) > distance2) {
        if (other_top > bottom &&
            bottom > SPHERE_LAND_LEVEL(other_bottom, other_top)) {
            keep_touch(task, shape, other, other_shape);
            collision->flags |= COLLISION_ON_TOP;
            other_collision->flags |= COLLISION_UNDER;
            hurt_shapes(task, shape, other, other_shape);
            if (try_push(collision, shape, other_collision, other_shape)) {
                if (collision->kind == COLLISION_PLAYER &&
                    collision_next_object(task)) {
                    work->flags |= WORK_ON_TOP;
                }
                other_work->flags |= WORK_UNDER;
                work->pos.y += other_top - bottom;
            }
            mark_standing(task, shape, other, other_shape);
        } else if (top > other_bottom &&
                   other_bottom > SPHERE_LAND_LEVEL(bottom, top)) {
            keep_touch(task, shape, other, other_shape);
            collision->flags |= COLLISION_UNDER;
            other_collision->flags |= COLLISION_ON_TOP;
            hurt_shapes(task, shape, other, other_shape);
            if (try_push(other_collision, other_shape, collision, shape)) {
                if (collision->kind == COLLISION_PLAYER &&
                    collision_next_object(task)) {
                    work->flags |= WORK_UNDER;
                }
                other_work->flags |= WORK_ON_TOP;
                other_work->pos.y += top - other_bottom;
            }
        } else if (other_top > bottom && top > other_bottom) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
            away.x = center.x - base.x;
            away.y = 0.0F;
            away.z = center.z - base.z;
            depth = radius + other_radius - njSqrt(distance2);
            if (njScalor(&away) == 0.0F) {
                away.x = away.y = 0.0F;
                away.z = FALLBACK_Z;
            } else {
                njUnitVector(&away);
            }
            depth *= CYLINDER_PUSH_SHARE;
            x = away.x * depth;
            y = away.y * depth;
            z = away.z * depth;
            collision->flags |= COLLISION_SIDE;
            other_collision->flags |= COLLISION_SIDE;
            collision->normal.x = away.x;
            collision->normal.z = away.z;
            if (try_push(collision, shape, other_collision, other_shape)) {
                work->pos.x += x;
                work->pos.y += y;
                work->pos.z += z;
            }
            /* @bug logic: pushes the cylinder back by the unit direction away,
             * not by its share of the overlap. the subtractions at 0x8c01cb7a
             * to 0x8c01cb92 take away's members from the frame, where the
             * sphere's push at 0x8c01cb4a to 0x8c01cb5a adds the scaled x, y
             * and z */
            if (try_push(other_collision, other_shape, collision, shape)) {
                other_work->pos.x -= away.x;
                other_work->pos.y -= away.y;
                other_work->pos.z -= away.z;
            }
        }
    }
}

/* a sphere against a SHAPE_CYLINDER2. between the axis's ends a touch pushes
 * the two apart from the axis. past an end, it pushes the sphere away from the
 * end. it does so only where the sphere's foot on the axis lies within its
 * radius of the end */
static Float collide_sphere_cylinder2(struct task *task,
                                      struct collision_shape *shape,
                                      struct task *other,
                                      struct collision_shape *other_shape)
{
    NJS_POINT3 center;
    NJS_POINT3 base;
    NJS_POINT3 ends[2];
    NJS_VECTOR up;
    NJS_LINE axis;
    NJS_POINT3 nearest;
    NJS_VECTOR to_end0;
    NJS_VECTOR to_end1;
    NJS_VECTOR away;
    Float end0_distance2;
    Float end1_distance2;
    struct task_work *work = task->work;
    struct task_work *other_work = other->work;
    /* @hack spelling: as in collide_sphere_cylinder */
    struct collision_work *collision = task->work->collision;
    struct collision_work *other_collision = other->work->collision;
    Float radius = shape->radius;
    Float other_radius = other_shape->radius;
    Float length = other_shape->size_y;
    Float distance;
    Float radius2;
    /* @hack spelling: FALLBACK_Z held in a local from the touch on, where
     * the constant at each use would do */
    Float fallback_z;

    shape_get_check_center(work, shape, &center);
    shape_get_up(other_work, other_shape, &up);
    shape_get_center(other_work, other_shape, &base);
    ends[0].x = base.x + (up.x * length);
    ends[0].y = base.y + (up.y * length);
    ends[0].z = base.z + (up.z * length);
    ends[1].x = base.x - (length * up.x);
    ends[1].y = base.y - (length * up.y);
    ends[1].z = base.z - (length * up.z);
    axis.px = ends[1].x;
    axis.py = ends[1].y;
    axis.pz = ends[1].z;
    axis.vx = up.x;
    axis.vy = up.y;
    axis.vz = up.z;
    distance = njDistanceP2L(&center, &axis, &nearest);
    to_end0.x = ends[0].x - nearest.x;
    to_end0.y = ends[0].y - nearest.y;
    to_end0.z = ends[0].z - nearest.z;
    to_end1.x = ends[1].x - nearest.x;
    to_end1.y = ends[1].y - nearest.y;
    to_end1.z = ends[1].z - nearest.z;
    if (distance < radius + other_radius) {
        fallback_z = FALLBACK_Z;
        if (njInnerProduct(&to_end0, &to_end1) < 0.0F) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
            other_collision->flags |= COLLISION_SIDE;
            away.x = center.x - nearest.x;
            away.y = center.y - nearest.y;
            away.z = center.z - nearest.z;
            /* @hack spelling: the depth in two statements, where one would
             * do */
            radius -= distance;
            radius = radius + other_radius - PUSH_SHORT;
            if (njScalor(&away) == 0.0F) {
                away.x = away.y = 0.0F;
                away.z = fallback_z;
            } else {
                njUnitVector(&away);
            }
            away.x *= radius;
            away.y *= radius;
            away.z *= radius;
            if (try_push(collision, shape, other_collision, other_shape)) {
                work->pos.x += away.x;
                work->pos.y += away.y;
                work->pos.z += away.z;
            }
            if (try_push(other_collision, other_shape, collision, shape)) {
                other_work->pos.x -= away.x;
                other_work->pos.y -= away.y;
                other_work->pos.z -= away.z;
            }
        } else {
            radius2 = radius * radius;
            end0_distance2 = njScalor2(&to_end0);
            if (end0_distance2 < radius2) {
                keep_touch(task, shape, other, other_shape);
                hurt_shapes(task, shape, other, other_shape);
                if (try_push(collision, shape, other_collision, other_shape)) {
                    away.x = -to_end0.x;
                    away.y = -to_end0.y;
                    away.z = -to_end0.z;
                    radius = radius - njSqrt(end0_distance2) - PUSH_SHORT;
                    if (njScalor(&away) == 0.0F) {
                        away.x = away.y = 0.0F;
                        away.z = fallback_z;
                    } else {
                        njUnitVector(&away);
                    }
                    other_collision->flags |= COLLISION_UNDER;
                    away.x *= radius;
                    away.y *= radius;
                    away.z *= radius;
                    work->pos.x += away.x;
                    work->pos.y += away.y;
                    work->pos.z += away.z;
                }
            } else {
                end1_distance2 = njScalor2(&to_end1);
                if (end1_distance2 < radius2) {
                    keep_touch(task, shape, other, other_shape);
                    hurt_shapes(task, shape, other, other_shape);
                    if (try_push(collision, shape, other_collision,
                                 other_shape)) {
                        away.x = -to_end1.x;
                        away.y = -to_end1.y;
                        away.z = -to_end1.z;
                        radius = radius - njSqrt(end1_distance2) - PUSH_SHORT;
                        if (njScalor(&away) == 0.0F) {
                            away.x = away.y = 0.0F;
                            away.z = fallback_z;
                        } else {
                            njUnitVector(&away);
                        }
                        other_collision->flags |= COLLISION_ON_TOP;
                        away.x *= radius;
                        away.y *= radius;
                        away.z *= radius;
                        work->pos.x += away.x;
                        work->pos.y += away.y;
                        work->pos.z += away.z;
                    }
                }
            }
        }
    }
    return distance;
}

/* a sphere against a SHAPE_BOX, in the box's turned axes. the sphere lands on
 * the box's top or strikes its bottom from below. otherwise it is pushed out
 * through the side nearest its center */
static Float collide_sphere_box(struct task *task,
                                struct collision_shape *shape,
                                struct task *other,
                                struct collision_shape *other_shape)
{
    struct task_work *work;
    struct task_work *other_work;
    /* @hack spelling: as in collide_standing_axis_box */
    struct collision_work *collision = (work = task->work)->collision;
    struct collision_work *other_collision =
        (other_work = other->work)->collision;
    Float radius = shape->radius;
    Float other_x = other_shape->radius;
    Float other_y = other_shape->size_y;
    Float other_z = other_shape->size_z;
    /* @hack spelling: shape->radius read again, where radius would do */
    Float reach_x = shape->radius + other_x;
    Float reach_z = radius + other_z;
    NJS_POINT3 center;
    NJS_POINT3 other_center;
    NJS_POINT3 local;
    NJS_POINT3 offset;
    Float top;
    Float bottom;
    Float other_top;
    Float other_bottom;
    Bool hit;
    int side;

    shape_get_check_center(work, shape, &center);
    shape_get_box_center(other_work, other_shape, &other_center);
    offset = work->pos;
    njSubVector(&offset, &center);
    top = center.y + radius;
    bottom = center.y - radius;
    other_top = other_center.y + other_y;
    other_bottom = other_center.y - other_y;
    {
        NJS_VECTOR relative;

        relative.x = center.x - other_center.x;
        relative.y = center.y - other_center.y;
        relative.z = center.z - other_center.z;
#ifdef NONMATCHING
        /* for a finite vector, the kernel gives the bits of the matrix calls
         * below, as engine/dc/turn.h says */
        zam_vec3_turn_y(-other_work->ang.y - other_shape->ang.y, &relative.x,
                        &local.x);
#else
        njPushMatrix(&_nj_unit_matrix_);
        njRotateY(NULL, -other_work->ang.y - other_shape->ang.y);
        njCalcVector(NULL, &relative, &local);
        njPopMatrixEx();
#endif
    }
    if (bottom < other_top && LAND_LEVEL(bottom, top) > other_top) {
        if (box_contains(&local, reach_x, reach_z)) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
            collision->flags |= COLLISION_ON_TOP;
            other_collision->flags |= COLLISION_UNDER;
            if (try_push(collision, shape, other_collision, other_shape)) {
                if (collision->kind == COLLISION_PLAYER &&
                    collision_next_object(task)) {
                    work->flags |= WORK_ON_TOP;
                }
                other_work->flags |= WORK_UNDER;
                work->pos.y += other_top - bottom - CLEARANCE;
            }
            mark_standing(task, shape, other, other_shape);
        }
    } else if (top > other_bottom && other_bottom + BOTTOM_REACH > top) {
        if (box_contains(&local, reach_x, reach_z)) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
            collision->flags |= COLLISION_UNDER;
            other_collision->flags |= COLLISION_ON_TOP;
            if (try_push(collision, shape, other_collision, other_shape)) {
                if (collision->kind == COLLISION_PLAYER &&
                    collision_next_object(task)) {
                    work->flags |= WORK_UNDER;
                }
                other_work->flags |= WORK_ON_TOP;
                work->pos.y += other_bottom - top + CLEARANCE;
            }
        }
    } else if (top > other_bottom && other_top > bottom) {
        NJS_VECTOR normal = {0.0F, 0.0F, NO_SIDE_Z};
        NJS_VECTOR push;

        hit = FALSE;
        push = local;
        if (box_contains(&local, reach_x, reach_z)) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
            collision->flags |= COLLISION_SIDE;
            other_collision->flags |= COLLISION_SIDE;
            hit = TRUE;
            side = box_nearest_side(&local, reach_x, reach_z);
            switch (side) {
            case SIDE_NEG_X:
                push.x = CLEARANCE_FINE - reach_x;
                push.y = local.y;
                push.z = local.z;
                normal.x = 1.0F;
                normal.y = 0.0F;
                normal.z = 0.0F;
                break;
            case SIDE_X:
                /* @hack spelling: the sum stored, then adjusted, where
                 * reach_x - CLEARANCE_FINE would do */
                push.x = reach_x;
                push.x += -CLEARANCE_FINE;
                push.y = local.y;
                push.z = local.z;
                normal.x = -1.0F;
                normal.y = 0.0F;
                normal.z = 0.0F;
                break;
            case SIDE_NEG_Z:
                push.z = CLEARANCE_FINE - reach_z;
                push.y = local.y;
                push.x = local.x;
                normal.x = 0.0F;
                normal.y = 0.0F;
                normal.z = 1.0F;
                break;
            case SIDE_Z:
                push.z = reach_z - CLEARANCE_FINE;
                push.y = local.y;
                push.x = local.x;
                normal.x = 0.0F;
                normal.y = 0.0F;
                normal.z = -1.0F;
                break;
            }
        }
        if (hit && try_push(collision, shape, other_collision, other_shape)) {
            njPushMatrix(&_nj_unit_matrix_);
            njRotateY(NULL, other_work->ang.y + other_shape->ang.y);
            njCalcVector(NULL, &push, &local);
            njCalcVector(NULL, &normal, &collision->normal);
            njPopMatrixEx();
            work->pos.x = offset.x + local.x + other_center.x;
            work->pos.z = offset.z + local.z + other_center.z;
        }
    }
    return reach_z;
}

/* a sphere against a SHAPE_CAPSULE. between the axis's ends a touch pushes the
 * two apart from the axis. past an end, a touch of that end's round cap pushes
 * the sphere out of the cap */
static Float collide_sphere_capsule(struct task *task,
                                    struct collision_shape *shape,
                                    struct task *other,
                                    struct collision_shape *other_shape)
{
    struct task_work *work;
    struct task_work *other_work;
    struct collision_work *collision;
    struct collision_work *other_collision;
    Float radius;
    /* @hack spelling: distance declared between the radii, where any place
     * would do */
    Float distance;
    Float other_radius;
    Float length;
    Float depth;
    Float end0_distance2;
    Float end1_distance2;
    NJS_POINT3 center;
    NJS_POINT3 base;
    NJS_POINT3 ends[2];
    NJS_VECTOR up;
    NJS_LINE axis;
    NJS_POINT3 nearest;
    NJS_VECTOR to_end0;
    NJS_VECTOR to_end1;
    NJS_VECTOR away;

    /* @hack redundant: a store to shape that never runs */
    if (0) {
        shape = NULL;
    }
    work = task->work;
    other_work = other->work;
    collision = work->collision;
    other_collision = other_work->collision;
    radius = shape->radius;
    other_radius = other_shape->radius;
    length = other_shape->size_y;
    shape_get_check_center(work, shape, &center);
    shape_get_up(other_work, other_shape, &up);
    shape_get_center(other_work, other_shape, &base);
    ends[0].x = base.x + (length * up.x);
    ends[0].y = base.y + (length * up.y);
    ends[0].z = base.z + (length * up.z);
    ends[1].x = base.x - (length * up.x);
    ends[1].y = base.y - (length * up.y);
    ends[1].z = base.z - (length * up.z);
    axis.px = ends[1].x;
    axis.py = ends[1].y;
    axis.pz = ends[1].z;
    axis.vx = up.x;
    axis.vy = up.y;
    axis.vz = up.z;
    distance = njDistanceP2L(&center, &axis, &nearest);
    to_end0.x = ends[0].x - nearest.x;
    to_end0.y = ends[0].y - nearest.y;
    to_end0.z = ends[0].z - nearest.z;
    to_end1.x = ends[1].x - nearest.x;
    to_end1.y = ends[1].y - nearest.y;
    to_end1.z = ends[1].z - nearest.z;
    if (distance < radius + other_radius) {
        if (njInnerProduct(&to_end0, &to_end1) < 0.0F) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
            other_collision->flags |= COLLISION_SIDE;
            away.x = center.x - nearest.x;
            away.y = center.y - nearest.y;
            away.z = center.z - nearest.z;
            depth = radius - distance + other_radius - PUSH_SHORT;
            if (njScalor(&away) == 0.0F) {
                away.x = away.y = 0.0F;
                away.z = FALLBACK_Z;
            } else {
                njUnitVector(&away);
            }
            away.x *= depth;
            away.y *= depth;
            away.z *= depth;
            if (try_push(collision, shape, other_collision, other_shape)) {
                work->pos.x += away.x;
                work->pos.y += away.y;
                work->pos.z += away.z;
            }
            if (try_push(other_collision, other_shape, collision, shape)) {
                other_work->pos.x -= away.x;
                other_work->pos.y -= away.y;
                other_work->pos.z -= away.z;
            }
        } else {
            if ((end0_distance2 = SQUARE(center.x - ends[0].x) +
                                  SQUARE(center.y - ends[0].y) +
                                  SQUARE(center.z - ends[0].z)) <
                (radius + other_radius) * (radius + other_radius)) {
                keep_touch(task, shape, other, other_shape);
                hurt_shapes(task, shape, other, other_shape);
                if (try_push(collision, shape, other_collision, other_shape)) {
                    away.x = center.x - ends[0].x;
                    away.y = center.y - ends[0].y;
                    away.z = center.z - ends[0].z;
                    depth = radius + other_radius - njSqrt(end0_distance2) -
                            PUSH_SHORT;
                    if (njScalor(&away) == 0.0F) {
                        away.x = away.y = 0.0F;
                        away.z = FALLBACK_Z;
                    } else {
                        njUnitVector(&away);
                    }
                    other_collision->flags |= COLLISION_UNDER;
                    away.x *= depth;
                    away.y *= depth;
                    away.z *= depth;
                    work->pos.x += away.x;
                    work->pos.y += away.y;
                    work->pos.z += away.z;
                }
            } else {
                if ((end1_distance2 = SQUARE(center.x - ends[1].x) +
                                      SQUARE(center.y - ends[1].y) +
                                      SQUARE(center.z - ends[1].z)) <
                    (radius + other_radius) * (radius + other_radius)) {
                    keep_touch(task, shape, other, other_shape);
                    hurt_shapes(task, shape, other, other_shape);
                    if (try_push(collision, shape, other_collision,
                                 other_shape)) {
                        away.x = center.x - ends[1].x;
                        away.y = center.y - ends[1].y;
                        away.z = center.z - ends[1].z;
                        depth = radius + other_radius - njSqrt(end1_distance2) -
                                PUSH_SHORT;
                        if (njScalor(&away) == 0.0F) {
                            away.x = away.y = 0.0F;
                            away.z = FALLBACK_Z;
                        } else {
                            njUnitVector(&away);
                        }
                        other_collision->flags |= COLLISION_ON_TOP;
                        away.x *= depth;
                        away.y *= depth;
                        away.z *= depth;
                        work->pos.x += away.x;
                        work->pos.y += away.y;
                        work->pos.z += away.z;
                    }
                }
            }
        }
    }
    return distance;
}

/* two upright cylinders. either lands on the other where its bottom lies above
 * the other's CYLINDER_LAND_LEVEL. otherwise a touch pushes task
 * CYLINDER_PUSH_SHARE of the overlap away across x and z. it pushes other back
 * by the unit direction */
static void collide_cylinder_cylinder(struct task *task,
                                      struct collision_shape *shape,
                                      struct task *other,
                                      struct collision_shape *other_shape)
{
    NJS_POINT3 center;
    NJS_POINT3 other_center;
    NJS_VECTOR away;
    struct task_work *work = task->work;
    struct task_work *other_work = other->work;
    struct collision_work *collision = work->collision;
    struct collision_work *other_collision = other_work->collision;
    Float radius = shape->radius;
    Float height = shape->size_y;
    Float other_radius = other_shape->radius;
    Float other_height = other_shape->size_y;
    Float distance2;
    Float depth;
    Float top;
    Float bottom;
    Float other_top;
    Float other_bottom;
    Float x;
    Float y;
    Float z;

    shape_get_center(work, shape, &center);
    shape_get_center(other_work, other_shape, &other_center);
    distance2 =
        SQUARE(center.x - other_center.x) + SQUARE(center.z - other_center.z);
    top = center.y + height;
    bottom = center.y - height;
    other_top = other_center.y + other_height;
    other_bottom = other_center.y - other_height;
    if ((radius + other_radius) * (radius + other_radius) > distance2) {
        if (other_top > bottom &&
            bottom > CYLINDER_LAND_LEVEL(other_bottom, other_top)) {
            keep_touch(task, shape, other, other_shape);
            collision->flags |= COLLISION_ON_TOP;
            other_collision->flags |= COLLISION_UNDER;
            hurt_shapes(task, shape, other, other_shape);
            if (try_push(collision, shape, other_collision, other_shape)) {
                if (collision->kind == COLLISION_PLAYER &&
                    collision_next_object(task)) {
                    work->flags |= WORK_ON_TOP;
                }
                other_work->flags |= WORK_UNDER;
                work->pos.y += other_top - bottom - CLEARANCE_FINE;
            }
            mark_standing(task, shape, other, other_shape);
        } else if (top > other_bottom &&
                   other_bottom > CYLINDER_LAND_LEVEL(bottom, top)) {
            keep_touch(task, shape, other, other_shape);
            collision->flags |= COLLISION_UNDER;
            other_collision->flags |= COLLISION_ON_TOP;
            hurt_shapes(task, shape, other, other_shape);
            if (try_push(other_collision, other_shape, collision, shape)) {
                if (collision->kind == COLLISION_PLAYER &&
                    collision_next_object(task)) {
                    work->flags |= WORK_UNDER;
                }
                other_work->flags |= WORK_ON_TOP;
                other_work->pos.y += top - other_bottom + CLEARANCE_FINE;
            }
        } else if (other_top > bottom && top > other_bottom) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
            away.x = center.x - other_center.x;
            away.y = 0.0F;
            away.z = center.z - other_center.z;
            depth = radius + other_radius - njSqrt(distance2);
            if (njScalor(&away) == 0.0F) {
                away.x = away.y = 0.0F;
                away.z = FALLBACK_Z;
            } else {
                njUnitVector(&away);
            }
            depth *= CYLINDER_PUSH_SHARE;
            x = away.x * depth;
            y = away.y * depth;
            z = away.z * depth;
            collision->flags |= COLLISION_SIDE;
            other_collision->flags |= COLLISION_SIDE;
            collision->normal.x = away.x;
            collision->normal.z = away.z;
            if (try_push(collision, shape, other_collision, other_shape)) {
                work->pos.x += x;
                work->pos.y += y;
                work->pos.z += z;
            }
            /* @bug logic: pushes other back by the unit direction away, not by
             * its share of the overlap. the subtractions at 0x8c01de62 to
             * 0x8c01de7a take away's members, where task's push at 0x8c01de32
             * to 0x8c01de42 adds the scaled x, y and z */
            if (try_push(other_collision, other_shape, collision, shape)) {
                other_work->pos.x -= away.x;
                other_work->pos.y -= away.y;
                other_work->pos.z -= away.z;
            }
        }
    }
}

/* an upright cylinder against a SHAPE_BOX, tested as collide_sphere_box tests a
 * sphere. its size_y stands for the sphere's half height */
static Float collide_cylinder_box(struct task *task,
                                  struct collision_shape *shape,
                                  struct task *other,
                                  struct collision_shape *other_shape)
{
    struct task_work *work;
    struct task_work *other_work;
    /* @hack spelling: as in collide_standing_axis_box */
    struct collision_work *collision = (work = task->work)->collision;
    struct collision_work *other_collision =
        (other_work = other->work)->collision;
    /* @hack spelling: box_work, a second name for other_work at the turn
     * and the two flags it sets */
    struct task_work *box_work = other_work;
    Float radius = shape->radius;
    Float other_x = other_shape->radius;
    Float height = shape->size_y;
    Float other_y = other_shape->size_y;
    Float other_z = other_shape->size_z;
    Float reach_x;
    Float reach_z;
    /* @hack spelling: a second name for reach_z at the bottom's test */
    Float bottom_reach_z;
    NJS_POINT3 center;
    NJS_POINT3 other_center;
    NJS_POINT3 local;
    Float top;
    Float bottom;
    Float other_top;
    Float other_bottom;
    Bool hit;
    int side;

    /* @hack redundant: a step of other_z that never runs */
    if (0) {
        other_z++;
    }
    reach_x = radius + other_x;
    reach_z = radius + other_z;
    bottom_reach_z = radius + other_z;
    shape_get_check_center(work, shape, &center);
    shape_get_box_center(other_work, other_shape, &other_center);
    top = center.y + height;
    bottom = center.y - height;
    other_top = other_center.y + other_y;
    other_bottom = other_center.y - other_y;
    {
        NJS_VECTOR relative;

        relative.x = center.x - other_center.x;
        relative.y = center.y - other_center.y;
        relative.z = center.z - other_center.z;
#ifdef NONMATCHING
        /* for a finite vector, the kernel gives the bits of the matrix calls
         * below, as engine/dc/turn.h says */
        zam_vec3_turn_y(-box_work->ang.y - other_shape->ang.y, &relative.x,
                        &local.x);
#else
        njPushMatrix(&_nj_unit_matrix_);
        njRotateY(NULL, -box_work->ang.y - other_shape->ang.y);
        njCalcVector(NULL, &relative, &local);
        njPopMatrixEx();
#endif
    }
    if (bottom < other_top && LAND_LEVEL(bottom, top) > other_top) {
        if (box_contains(&local, reach_x, reach_z)) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
            collision->flags |= COLLISION_ON_TOP;
            other_collision->flags |= COLLISION_UNDER;
            if (try_push(collision, shape, other_collision, other_shape)) {
                if (collision->kind == COLLISION_PLAYER &&
                    collision_next_object(task)) {
                    work->flags |= WORK_ON_TOP;
                }
                box_work->flags |= WORK_UNDER;
                work->pos.y += other_top - bottom - CLEARANCE;
            }
            mark_standing(task, shape, other, other_shape);
        }
    } else if (other_bottom < top && other_bottom + BOTTOM_REACH > top) {
        if (box_contains(&local, reach_x, bottom_reach_z)) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
            collision->flags |= COLLISION_UNDER;
            other_collision->flags |= COLLISION_ON_TOP;
            if (try_push(collision, shape, other_collision, other_shape)) {
                if (collision->kind == COLLISION_PLAYER &&
                    collision_next_object(task)) {
                    work->flags |= WORK_UNDER;
                }
                box_work->flags |= WORK_ON_TOP;
                work->pos.y += other_bottom - top + CLEARANCE;
            }
        }
    } else if (other_bottom < top && other_top > bottom) {
        NJS_VECTOR normal = {0.0F, 0.0F, NO_SIDE_Z};
        NJS_VECTOR push;
        /* @hack spelling: a third name for reach_z at the side's test */
        Float side_reach_z = radius + other_z;

        hit = FALSE;
        push = local;
        if (box_contains(&local, reach_x, reach_z)) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
            collision->flags |= COLLISION_SIDE;
            other_collision->flags |= COLLISION_SIDE;
            hit = TRUE;
            side = box_nearest_side(&local, reach_x, side_reach_z);
            switch (side) {
            case SIDE_NEG_X:
                push.x = CLEARANCE_FINE - reach_x;
                push.y = local.y;
                push.z = local.z;
                normal.x = 1.0F;
                normal.y = 0.0F;
                normal.z = 0.0F;
                break;
            case SIDE_X:
                push.x = reach_x - CLEARANCE_FINE;
                push.y = local.y;
                push.z = local.z;
                normal.x = -1.0F;
                normal.y = 0.0F;
                normal.z = 0.0F;
                break;
            case SIDE_NEG_Z:
                push.z = CLEARANCE_FINE - reach_z;
                push.y = local.y;
                push.x = local.x;
                normal.x = 0.0F;
                normal.y = 0.0F;
                normal.z = 1.0F;
                break;
            case SIDE_Z:
                push.z = reach_z - CLEARANCE_FINE;
                push.y = local.y;
                push.x = local.x;
                normal.x = 0.0F;
                normal.y = 0.0F;
                normal.z = -1.0F;
                break;
            }
        }
        if (hit && try_push(collision, shape, other_collision, other_shape)) {
            njPushMatrix(&_nj_unit_matrix_);
            njRotateY(NULL, other_work->ang.y + other_shape->ang.y);
            njCalcVector(NULL, &push, &local);
            njCalcVector(NULL, &normal, &collision->normal);
            njPopMatrixEx();
            work->pos.x = local.x + other_center.x;
            work->pos.z = local.z + other_center.z;
        }
    }
    return reach_x;
}

/* a SHAPE_SWEPT_SPHERE against a sphere. the sweep runs from its start to the
 * task's position. they touch where it comes within their radii's sum of the
 * sphere's center. a touch pushes nothing. the first test sets
 * SHAPE_SWEEP_FROM_CENTER. later tests start the sweep at the shape's center,
 * which the tests never write */
static void collide_swept_sphere(struct task *task,
                                 struct collision_shape *shape,
                                 struct task *other,
                                 struct collision_shape *other_shape)
{
    NJS_POINT3 pos;
    NJS_POINT3 other_center;
    NJS_POINT3 start;
    NJS_LINE sweep;
    NJS_POINT3 nearest;
    NJS_VECTOR to_pos;
    NJS_VECTOR to_start;
    /* @hack redundant: the two records, which nothing reads */
    struct collision_work *collision =
        task->work->collision; /* NOLINT(clang-diagnostic-unused-variable) */
    struct collision_work *other_collision =
        other->work->collision; /* NOLINT(clang-diagnostic-unused-variable) */
    struct task_work *work = task->work;
    struct task_work *other_work = other->work;
    Float radius = shape->radius;
    Float other_radius = other_shape->radius;
    Float distance;

    if (shape->flags & SHAPE_SWEEP_FROM_CENTER) {
        start.x = shape->center.x;
        start.y = shape->center.y;
        start.z = shape->center.z;
    } else {
        start.x = work->pos.x + SWEEP_NUDGE;
        start.y = work->pos.y;
        start.z = work->pos.z;
        shape->flags |= SHAPE_SWEEP_FROM_CENTER;
    }
    pos.x = work->pos.x;
    pos.y = work->pos.y;
    pos.z = work->pos.z;
    shape_get_check_center(other_work, other_shape, &other_center);
    sweep.px = pos.x;
    sweep.py = pos.y;
    sweep.pz = pos.z;
    sweep.vx = pos.x - start.x;
    sweep.vy = pos.y - start.y;
    sweep.vz = pos.z - start.z;
    distance = njDistanceP2L(&other_center, &sweep, &nearest);
    to_pos.x = pos.x - nearest.x;
    to_pos.y = pos.y - nearest.y;
    to_pos.z = pos.z - nearest.z;
    to_start.x = start.x - nearest.x;
    to_start.y = start.y - nearest.y;
    to_start.z = start.z - nearest.z;
    if (distance < radius + other_radius) {
        if (njInnerProduct(&to_pos, &to_start) < 0.0F) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
        } else if (njDistanceP2P(&pos, &other_center) < radius + other_radius) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
        } else if (njDistanceP2P(&start, &other_center) <
                   radius + other_radius) {
            keep_touch(task, shape, other, other_shape);
            hurt_shapes(task, shape, other, other_shape);
        }
    }
}

/* run the test for the forms of shape and other_shape. a pair collision.h lists
 * no test for does nothing */
static void collide_shapes(struct task *task, struct collision_shape *shape,
                           struct task *other,
                           struct collision_shape *other_shape)
{
    switch (shape->form) {
    case SHAPE_STANDING:
        switch (other_shape->form) {
        case SHAPE_SPHERE:
            collide_sphere_sphere(task, shape, other, other_shape);
            break;
        case SHAPE_STANDING:
            collide_standing_standing(task, shape, other, other_shape);
            break;
        case SHAPE_CYLINDER:
            if (other_shape->push & PUSH_PLAYERS) {
                collide_standing_cylinder(task, shape, other, other_shape);
            } else {
                collide_sphere_cylinder(task, shape, other, other_shape);
            }
            break;
        case SHAPE_BOX:
            /* @hack redundant: a test of the box's PUSH_PLAYERS, which
             * changes nothing */
            if (other_shape->push & PUSH_PLAYERS) {
            }
            collide_standing_box(task, shape, other, other_shape);
            break;
        case SHAPE_CYLINDER2:
            collide_standing_cylinder2(task, shape, other, other_shape);
            break;
        case SHAPE_BOX2:
            collide_standing_box2(task, shape, other, other_shape);
            break;
        case SHAPE_AXIS_BOX:
            collide_standing_axis_box(task, shape, other, other_shape);
            break;
        case SHAPE_CAPSULE:
            collide_sphere_capsule(task, shape, other, other_shape);
            break;
        case SHAPE_WALL:
            collide_standing_wall(task, shape, other, other_shape);
            break;
        }
        break;
    case SHAPE_SPHERE:
        switch (other_shape->form) {
        case SHAPE_SPHERE:
            collide_sphere_sphere(task, shape, other, other_shape);
            break;
        case SHAPE_CYLINDER:
            collide_sphere_cylinder(task, shape, other, other_shape);
            break;
        case SHAPE_BOX:
            collide_sphere_box(task, shape, other, other_shape);
            break;
        case SHAPE_CYLINDER2:
            collide_sphere_cylinder2(task, shape, other, other_shape);
            break;
        case SHAPE_CAPSULE:
            collide_sphere_capsule(task, shape, other, other_shape);
            break;
        }
        break;
    case SHAPE_CYLINDER:
        switch (other_shape->form) {
        case SHAPE_SPHERE:
            /* the sphere's test, with the sphere's task first */
            /* NOLINTNEXTLINE(readability-suspicious-call-argument) */
            collide_sphere_cylinder(other, other_shape, task, shape);
            break;
        case SHAPE_CYLINDER:
            collide_cylinder_cylinder(task, shape, other, other_shape);
            break;
        case SHAPE_BOX:
            collide_cylinder_box(task, shape, other, other_shape);
            break;
        case SHAPE_CAPSULE:
            break;
        }
        break;
    case SHAPE_BOX:
        switch (other_shape->form) {
        case SHAPE_SPHERE:
            /* NOLINTNEXTLINE(readability-suspicious-call-argument) */
            collide_sphere_box(other, other_shape, task, shape);
            break;
        case SHAPE_CYLINDER:
            break;
        case SHAPE_BOX:
            break;
        case SHAPE_CAPSULE:
            break;
        }
        break;
    case SHAPE_CYLINDER2:
        switch (other_shape->form) {
        case SHAPE_SPHERE:
            /* NOLINTNEXTLINE(readability-suspicious-call-argument) */
            collide_sphere_cylinder2(other, other_shape, task, shape);
            break;
        }
        break;
    case SHAPE_CAPSULE:
        switch (other_shape->form) {
        case SHAPE_SPHERE:
            break;
        case SHAPE_CYLINDER:
            break;
        case SHAPE_BOX:
            break;
        case SHAPE_CAPSULE:
            break;
        }
        break;
    case SHAPE_SWEPT_SPHERE:
        switch (other_shape->form) {
        case SHAPE_STANDING:
        case SHAPE_SPHERE:
            collide_swept_sphere(task, shape, other, other_shape);
            break;
        case SHAPE_CYLINDER:
            break;
        case SHAPE_BOX:
            break;
        }
        break;
    }
}

/* where the two records reach each other, test each shape of task's against
 * each of other's. a pair passes over where either shape is SHAPE_OFF or
 * ignores the other's kind */
static void collide_records(struct task *task, struct task *other)
{
    struct collision_work *collision = task->work->collision;
    struct collision_work *other_collision = other->work->collision;
    /* @hack redundant: register on shape, other_shape and off */
    register struct collision_shape *other_shape;
    register struct collision_shape *shape;
    register Uint32 off;
    struct collision_shape *first;
    struct collision_shape *end;
    struct collision_shape *other_end;

    if (list_near_and_test_reach(task, other) == FALSE) {
        return;
    }

    first = collision->shapes;
    end = first + collision->count;
    {
        shape = first;
        off = SHAPE_OFF;
        if (shape >= end) {
            return;
        }

        do {
            first = other_collision->shapes;
            other_end = first + other_collision->count;
            other_shape = first;

            /* @hack redundant: a label nothing jumps to */
        each_other: /* NOLINT(clang-diagnostic-unused-label) */
            while (other_shape < other_end) {
                if (shapes_ignore(task, shape, other, other_shape) != FALSE) {
                    ++other_shape;
                    continue;
                }
                if ((shape->flags & off) != 0) {
                    ++other_shape;
                    continue;
                }
                if ((other_shape->flags & off) != 0) {
                    ++other_shape;
                    continue;
                }

                collide_shapes(task, shape, other, other_shape);
                ++other_shape;
            }

            ++shape;
            /* @hack redundant: a continue at the end of the loop */
            continue; /* NOLINT(readability-redundant-control-flow) */
            /* @hack spelling: the bound read again through its own address,
             * where end would do */
            /* NOLINTNEXTLINE(readability-redundant-casting) */
        } while (shape < *(struct collision_shape **)&end);
    }
}

/* the walks of the lists, one for each kind that comes first in a pair */

static void collide_players(void)
{
    /* @hack redundant: register on the four pointers */
    register struct task **entry;
    register struct task **other;
    register struct task **end;
    register struct task **other_end;

    end = &collision_players[collision_player_count - 1];
    for (entry = collision_players; entry < end; entry++) {
        other_end = &collision_players[collision_player_count];
        for (other = entry + 1; other < other_end; other++) {
            collide_records(*entry, *other);
        }
    }
    end = &collision_players[collision_player_count];
    for (entry = collision_players; entry < end; entry++) {
        other_end = &collision_items[collision_item_count];
        for (other = collision_items; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_enemies[collision_enemy_count];
        for (other = collision_enemies; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_objects[collision_object_count];
        for (other = collision_objects; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_mutual_objects[collision_mutual_object_count];
        for (other = collision_mutual_objects; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_pickups[collision_pickup_count];
        for (other = collision_pickups; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_pickup_targets[collision_pickup_target_count];
        for (other = collision_pickup_targets; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_walls[collision_wall_count];
        for (other = collision_walls; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_unk_9s[collision_unk_9_count];
        for (other = collision_unk_9s; other < other_end; other++) {
            collide_records(*entry, *other);
        }
    }
}

static void collide_attacks(void)
{
    /* @hack redundant: as in collide_players */
    register struct task **entry;
    register struct task **other;
    register struct task **end;
    register struct task **other_end;

    end = &collision_attacks[collision_attack_count];
    for (entry = collision_attacks; entry < end; entry++) {
        other_end = &collision_players[collision_player_count];
        for (other = collision_players; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_items[collision_item_count];
        for (other = collision_items; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_enemies[collision_enemy_count];
        for (other = collision_enemies; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_objects[collision_object_count];
        for (other = collision_objects; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_mutual_objects[collision_mutual_object_count];
        for (other = collision_mutual_objects; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_unk_9s[collision_unk_9_count];
        for (other = collision_unk_9s; other < other_end; other++) {
            collide_records(*entry, *other);
        }
    }
}

static void collide_unk_9s(void)
{
    /* @hack redundant: as in collide_players */
    register struct task **entry;
    register struct task **other;
    register struct task **end;
    register struct task **other_end;

    end = &collision_unk_9s[collision_unk_9_count - 1];
    for (entry = collision_unk_9s; entry < end; entry++) {
        other_end = &collision_unk_9s[collision_unk_9_count];
        for (other = entry + 1; other < other_end; other++) {
            collide_records(*entry, *other);
        }
    }
    end = &collision_unk_9s[collision_unk_9_count];
    for (entry = collision_unk_9s; entry < end; entry++) {
        other_end = &collision_items[collision_item_count];
        for (other = collision_items; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_enemies[collision_enemy_count];
        for (other = collision_enemies; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_objects[collision_object_count];
        for (other = collision_objects; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_mutual_objects[collision_mutual_object_count];
        for (other = collision_mutual_objects; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_pickups[collision_pickup_count];
        for (other = collision_pickups; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_pickup_targets[collision_pickup_target_count];
        for (other = collision_pickup_targets; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_walls[collision_wall_count];
        /* @hack redundant: a walk of the walls with an empty body */
        for (other = collision_walls; other < other_end; other++) {
        }
    }
}

static void collide_enemies(void)
{
    /* @hack redundant: as in collide_players */
    register struct task **entry;
    register struct task **other;
    register struct task **end;
    register struct task **other_end;

    end = &collision_enemies[collision_enemy_count - 1];
    for (entry = collision_enemies; entry < end; entry++) {
        other_end = &collision_enemies[collision_enemy_count];
        for (other = entry + 1; other < other_end; other++) {
            collide_records(*entry, *other);
        }
    }
    end = &collision_enemies[collision_enemy_count];
    for (entry = collision_enemies; entry < end; entry++) {
        other_end = &collision_objects[collision_object_count];
        for (other = collision_objects; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_mutual_objects[collision_mutual_object_count];
        for (other = collision_mutual_objects; other < other_end; other++) {
            collide_records(*entry, *other);
        }
        other_end = &collision_walls[collision_wall_count];
        /* @hack redundant: a walk of the walls with an empty body */
        for (other = collision_walls; other < other_end; other++) {
        }
    }
}

static void collide_objects(void)
{
    /* @hack redundant: as in collide_players */
    register struct task **entry;
    register struct task **other;
    register struct task **end;
    register struct task **other_end;

    end = &collision_objects[collision_object_count];
    for (entry = collision_objects; entry < end; entry++) {
        other_end = &collision_mutual_objects[collision_mutual_object_count];
        for (other = collision_mutual_objects; other < other_end; other++) {
            collide_records(*entry, *other);
        }
    }
}

static void collide_mutual_objects(void)
{
    /* @hack redundant: as in collide_players */
    register struct task **entry;
    register struct task **other;
    register struct task **end;
    register struct task **other_end;

    end = &collision_mutual_objects[collision_mutual_object_count - 1];
    for (entry = collision_mutual_objects; entry < end; entry++) {
        other_end = &collision_mutual_objects[collision_mutual_object_count];
        for (other = entry + 1; other < other_end; other++) {
            collide_records(*entry, *other);
        }
    }
}

static void collision_empty_lists(void)
{
    collision_player_count = 0;
    collision_attack_count = 0;
    collision_last_item_count = collision_item_count;
    collision_last_enemy_count = collision_enemy_count;
    collision_item_count = 0;
    collision_enemy_count = 0;
    collision_object_count = 0;
    collision_mutual_object_count = 0;
    collision_pickup_count = 0;
    collision_pickup_target_count = 0;
    collision_wall_count = 0;
    collision_unk_9_count = 0;
}

static void near_clear(void)
{
    near_target_count = 0;
    near_ring_count = 0;
    near_target_count_player1 = 0;
    near_ring_count_player1 = 0;
    near_targets[0].work = NULL;
    near_rings[0].work = NULL;
    near_targets_player1[0].work = NULL;
    near_rings_player1[0].work = NULL;
}

void collision_restart_scans(void)
{
    collision_scan.task = NULL;
    collision_scan.scan = NULL;
    collision_scan.index = -1;
    collision_scan.id = -1;
}

/* the index of the hit a scan of task's record starts from. it is one past
 * collision_scan.index, or 0 where the last call was of another task or scan.
 * it stops at COLLISION_HITS, past the last hit */
static int scan_start(struct task *task, const void *scan)
{
    if (collision_scan.task != task) {
        collision_scan.task = task;
        collision_scan.scan = NULL;
        collision_scan.id = -1;
        collision_scan.index = -1;
    }
    if (collision_scan.scan != scan) {
        collision_scan.scan = scan;
        collision_scan.id = -1;
        collision_scan.index = -1;
    }
    ++collision_scan.index;
    if (collision_scan.index > COLLISION_HITS) {
        collision_scan.index = COLLISION_HITS;
    }
    return collision_scan.index;
}

/* scan_start for a scan that matches id. where id isn't collision_scan.id,
 * which stays -1, it starts the scan over */
static int scan_start_with_id(struct task *task, const void *scan, int id)
{
    if (collision_scan.task != task) {
        collision_scan.task = task;
        collision_scan.scan = NULL;
        collision_scan.id = -1;
        collision_scan.index = -1;
    }
    if (collision_scan.scan != scan) {
        collision_scan.scan = scan;
        collision_scan.id = -1;
        collision_scan.index = -1;
    }
    if (collision_scan.id != id) {
        /* @bug logic: keeps -1 as the id, where the scan's own test
         * above keeps the new scan. the store at 0x8c01f086 stores R0's -1,
         * where 0x8c01f07a stores the scan. so every id but -1 starts
         * the scan over at each call */
        collision_scan.id = -1;
        collision_scan.index = -1;
    }
    ++collision_scan.index;
    if (collision_scan.index > COLLISION_HITS) {
        collision_scan.index = COLLISION_HITS;
    }
    return collision_scan.index;
}

void collision_clear(void)
{
    collision_restart_scans();
    near_clear();
    collision_empty_lists();
    collision_set_up_axis(COLLISION_UP_Y);
}

void collision_collide_all(void)
{
    collision_restart_scans();
    near_clear();
    njPushMatrixEx();
    collide_players();
    collide_attacks();
    collide_unk_9s();
    collide_enemies();
    collide_objects();
    collide_mutual_objects();
    njPopMatrixEx();
    collision_empty_lists();
}

void collision_fit_radius(struct task *task)
{
    struct collision_work *collision;
    struct collision_shape *shape;
    struct collision_shape *end;
    Float farthest = 0.0F;
    Float reach;
    Float radius;
    Float size_y;
    Float size_z;

    /* @bug uninitialized: collision is never set without a task or a work,
     * which the BT/S at 0x8c01f178 and the BT at 0x8c01f180 let through. the
     * loads from 0x8c01f18a then read through R13 as the caller left it */
#ifdef NONMATCHING
    /* this side repairs the reads the tag above names. they go through R13 as
     * the caller left it, which C can't give. this side fits nothing without a
     * task or a work */
    if (!task || !task->work) {
        return;
    }
#endif
    /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
    if (task && task->work) {
        collision = task->work->collision;
        if (!collision) {
            return;
        }
    }
    end = collision->shapes + collision->count;
    /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
    for (shape = collision->shapes; shape < end; shape++) {
        radius = shape->radius;
        size_y = shape->size_y;
        size_z = shape->size_z;
        reach = njScalor(&shape->center);
        switch (shape->form) {
        case SHAPE_SPHERE:
            reach += radius;
            break;
        case SHAPE_CAPSULE:
            radius += size_y;
            reach += radius;
            break;
        case SHAPE_CYLINDER:
        case SHAPE_CYLINDER2:
            reach += njSqrt(SQUARE(radius) + SQUARE(size_y));
            break;
        default:
            reach += njSqrt(SQUARE(radius) + SQUARE(size_y) + SQUARE(size_z));
            break;
        }
        if (reach > farthest) {
            farthest = reach;
        }
    }
    collision->radius = farthest * FIT_MARGIN;
    if (farthest < FIT_MIN) {
        collision->radius = FIT_DEFAULT;
    }
}

void collision_fit_record_radius(struct collision_work *collision)
{
    collision_fit_radius(collision->owner);
    /* @hack redundant: a return after the tail call */
    return; /* NOLINT(readability-redundant-control-flow) */
}

void collision_set_radius(struct task *task, Float radius)
{
    struct collision_work *collision;

    /* @bug uninitialized: collision is never set without a task or a work,
     * which the BTs at 0x8c01f24a and 0x8c01f250 let through. the test at
     * 0x8c01f256 then reads R5 as the caller left it, and where that isn't 0
     * the store at 0x8c01f25c writes through it */
#ifdef NONMATCHING
    /* this side repairs the read and the store the tag above names. they go
     * through R5 as the caller left it, which C can't give. this side sets
     * nothing without a task or a work */
    if (!task || !task->work) {
        return;
    }
#endif
    /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
    if (task && task->work) {
        collision = task->work->collision;
    }
    if (collision) {
        collision->radius = radius;
    }
    /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
}

Bool collision_get_normal(struct task_work *work, NJS_VECTOR *normal)
{
    struct collision_work *collision = work->collision;
    NJS_VECTOR *side_normal = &collision->normal;

    if (!(collision->flags & COLLISION_SIDE)) {
        return FALSE;
    }
    normal->x = side_normal->x;
    normal->z = side_normal->z;
    return TRUE;
}

Bool fun_8c01f2b0(void)
{
    return TRUE;
}

void collision_free(struct task *task)
{
    struct collision_work *collision;

    if (!task || !task->work) {
        return;
    }
    collision = task->work->collision;
    if (!collision) {
        return;
    }
    if ((collision->flags & COLLISION_OWN_SHAPES) && collision->shapes) {
        syFree(collision->shapes);
        collision->shapes = NULL;
    }
    syFree(collision);
    task->work->collision = NULL;
}

int collision_get_hit_count(struct task *task)
{
    struct task_work *work;

    if (!task) {
        return 0;
    }
    work = task->work;
    if (!work) {
        return 0;
    }
    if (work->collision) {
        return work->collision->hit_count;
    }
    /* @bug uninitialized: without a record, returns R0 as the caller left
     * it, as the BT at 0x8c01f30e skips to the RTS at 0x8c01f314 */
#ifdef NONMATCHING
    /* this side repairs the result the tag names: R0 as the caller left it,
     * which C can't give. this side returns 0, as for a task with no work */
    return 0;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

Bool collision_pushed(struct task *task)
{
    struct task_work *work;

    if (task) {
        work = task->work;
        if (work && work->collision &&
            (work->collision->flags & COLLISION_PUSHED)) {
            return TRUE;
        }
    }
    return FALSE;
}

Bool collision_touched(struct task *task)
{
    struct task_work *work;

    if (task) {
        work = task->work;
        if (work && work->collision &&
            (work->collision->flags & COLLISION_TOUCHED)) {
            return TRUE;
        }
    }
    return FALSE;
}

/* @hack spelling: the scans keep the index through scan_found, where
 * collision_scan.index = index would do */
static void scan_found(int index)
{
    collision_scan.index = index;
}

struct task *collision_next_player(struct task *task)
{
    struct collision_hit *hit = collision_next_player_hit(task);

    if (hit) {
        return hit->task;
    }
    return NULL;
}

struct collision_hit *collision_next_player_hit(struct task *task)
{
    int index;
    struct task_work *work;
    struct collision_work *collision;
    struct collision_hit *hit;
    struct collision_hit *end;

    index = scan_start(task, (const void *)collision_next_player_hit);
    if (task) {
        work = task->work;
        if (work && work->collision) {
            /* @hack spelling: the record set inside the expression that starts
             * the walk, where a statement of its own would do */
            for (hit = (collision = work->collision)->hits + index,
                end = collision->hits + COLLISION_HITS;
                 hit < end; hit++, index++) {
                if (hit->hit_shape == HIT_UNUSED) {
                    break;
                }
                if (collision_get_kind(hit->task) == COLLISION_PLAYER) {
                    scan_found(index);
                    return hit;
                }
            }
        }
    }
    return NULL;
}

struct task *collision_next_attack(struct task *task)
{
    struct collision_hit *hit = collision_next_attack_hit(task);

    if (hit) {
        return hit->task;
    }
    return NULL;
}

struct collision_hit *collision_next_attack_hit(struct task *task)
{
    int index;
    struct task_work *work;
    struct collision_work *collision;
    struct collision_hit *hit;
    struct collision_hit *end;

    index = scan_start(task, (const void *)collision_next_attack_hit);
    if (task) {
        work = task->work;
        if (work && work->collision) {
            /* @hack spelling: as in collision_next_player_hit */
            for (hit = (collision = work->collision)->hits + index,
                end = collision->hits + COLLISION_HITS;
                 hit < end; hit++, index++) {
                if (hit->hit_shape == HIT_UNUSED) {
                    break;
                }
                if (collision_get_kind(hit->task) == COLLISION_ATTACK) {
                    scan_found(index);
                    return hit;
                }
            }
        }
    }
    return NULL;
}

struct task *collision_next_item_or_enemy(struct task *task)
{
    struct collision_hit *hit = collision_next_item_or_enemy_hit(task);

    if (hit) {
        return hit->task;
    }
    return NULL;
}

struct collision_hit *collision_next_item_or_enemy_hit(struct task *task)
{
    int index;
    struct task_work *work;
    struct collision_work *collision;
    struct collision_hit *hit;
    struct collision_hit *end;
    int kind;

    index = scan_start(task, (const void *)collision_next_item_or_enemy_hit);
    if (task) {
        work = task->work;
        if (work && work->collision) {
            /* @hack spelling: as in collision_next_player_hit */
            for (hit = (collision = work->collision)->hits + index,
                end = collision->hits + COLLISION_HITS;
                 hit < end; hit++, index++) {
                if (hit->hit_shape == HIT_UNUSED) {
                    break;
                }
                kind = collision_get_kind(hit->task);
                if (kind == COLLISION_ITEM || kind == COLLISION_ENEMY) {
                    scan_found(index);
                    return hit;
                }
            }
        }
    }
    return NULL;
}

struct task *collision_next_object(struct task *task)
{
    struct collision_hit *hit = collision_next_object_hit(task);

    if (hit) {
        return hit->task;
    }
    return NULL;
}

struct collision_hit *collision_next_object_hit(struct task *task)
{
    int index;
    struct task_work *work;
    struct collision_work *collision;
    struct collision_hit *hit;
    struct collision_hit *end;
    int kind;

    index = scan_start(task, (const void *)collision_next_object_hit);
    if (task) {
        work = task->work;
        if (work && work->collision) {
            /* @hack spelling: as in collision_next_player_hit */
            for (hit = (collision = work->collision)->hits + index,
                end = collision->hits + COLLISION_HITS;
                 hit < end; hit++, index++) {
                if (hit->hit_shape == HIT_UNUSED) {
                    break;
                }
                kind = collision_get_kind(hit->task);
                if (kind == COLLISION_OBJECT ||
                    kind == COLLISION_OBJECT_MUTUAL) {
                    scan_found(index);
                    return hit;
                }
            }
        }
    }
    return NULL;
}

struct task *collision_next_pickup(struct task *task)
{
    struct collision_hit *hit = collision_next_pickup_hit(task);

    if (hit) {
        return hit->task;
    }
    return NULL;
}

struct collision_hit *collision_next_pickup_hit(struct task *task)
{
    int index;
    struct task_work *work;
    struct collision_work *collision;
    struct collision_hit *hit;
    struct collision_hit *end;
    int kind;

    index = scan_start(task, (const void *)collision_next_pickup_hit);
    if (task) {
        work = task->work;
        if (work && work->collision) {
            /* @hack spelling: as in collision_next_player_hit */
            for (hit = (collision = work->collision)->hits + index,
                end = collision->hits + COLLISION_HITS;
                 hit < end; hit++, index++) {
                if (hit->hit_shape == HIT_UNUSED) {
                    break;
                }
                kind = collision_get_kind(hit->task);
                if (kind == COLLISION_PICKUP ||
                    kind == COLLISION_PICKUP_TARGET) {
                    scan_found(index);
                    return hit;
                }
            }
        }
    }
    return NULL;
}

struct task *collision_first_with_id(struct task *task, Uint8 id)
{
    struct collision_hit *hit = collision_first_with_id_hit(task, id);

    if (hit) {
        return hit->task;
    }
    return NULL;
}

struct collision_hit *collision_first_with_id_hit(struct task *task, Uint8 id)
{
    int index;
    struct collision_work *collision;
    struct collision_hit *hit;
    struct collision_hit *end;
    Sint16 hit_shape;
    struct task *hit_task;

    index =
        scan_start_with_id(task, (const void *)collision_first_with_id_hit, id);
    if (task && task->work) {
        collision = task->work->collision;
        if (collision && (collision->flags & COLLISION_TOUCHED)) {
            end = collision->hits + COLLISION_HITS;
            for (hit = collision->hits + index; hit < end; hit++, index++) {
                hit_shape = hit->hit_shape;
                hit_task = hit->task;
                if (hit_shape == HIT_UNUSED) {
                    return NULL;
                }
                if (hit_task && hit_task->work && hit_task->work->collision &&
                    (Uint8)hit_task->work->collision->shapes[hit_shape].id ==
                        id) {
                    scan_found(index);
                    return hit;
                }
            }
        }
    }
    return NULL;
}

struct task *collision_next_player_by_shape(struct task *task, int index)
{
    struct collision_hit *hit = collision_next_player_hit_by_shape(task, index);

    if (hit) {
        return hit->task;
    }
    return NULL;
}

struct collision_hit *collision_next_player_hit_by_shape(struct task *task,
                                                         int index)
{
    int at;
    struct task_work *work;
    struct collision_work *collision;
    struct collision_hit *hit;
    struct collision_hit *end;

    at = scan_start(task, (const void *)collision_next_player_hit_by_shape);
    if (task) {
        work = task->work;
        if (work && work->collision) {
            /* @hack spelling: as in collision_next_player_hit */
            for (hit = (collision = work->collision)->hits + at,
                end = collision->hits + COLLISION_HITS;
                 hit < end; hit++, at++) {
                if (hit->hit_shape == HIT_UNUSED) {
                    break;
                }
                if (hit->own_shape == index &&
                    collision_get_kind(hit->task) == COLLISION_PLAYER) {
                    scan_found(at);
                    return hit;
                }
            }
        }
    }
    return NULL;
}

struct task *collision_next_attack_by_shape(struct task *task, int index)
{
    struct collision_hit *hit = collision_next_attack_hit_by_shape(task, index);

    if (hit) {
        return hit->task;
    }
    return NULL;
}

struct collision_hit *collision_next_attack_hit_by_shape(struct task *task,
                                                         int index)
{
    int at;
    struct task_work *work;
    struct collision_work *collision;
    struct collision_hit *hit;
    struct collision_hit *end;

    at = scan_start(task, (const void *)collision_next_attack_hit_by_shape);
    if (task) {
        work = task->work;
        if (work && work->collision) {
            /* @hack spelling: as in collision_next_player_hit */
            for (hit = (collision = work->collision)->hits + at,
                end = collision->hits + COLLISION_HITS;
                 hit < end; hit++, at++) {
                if (hit->hit_shape == HIT_UNUSED) {
                    break;
                }
                if (hit->own_shape == index &&
                    collision_get_kind(hit->task) == COLLISION_ATTACK) {
                    scan_found(at);
                    return hit;
                }
            }
        }
    }
    return NULL;
}

struct task *collision_next_item_or_enemy_by_shape(struct task *task, int index)
{
    struct collision_hit *hit =
        collision_next_item_or_enemy_hit_by_shape(task, index);

    if (hit) {
        return hit->task;
    }
    return NULL;
}

struct collision_hit *
collision_next_item_or_enemy_hit_by_shape(struct task *task, int index)
{
    int at;
    struct task_work *work;
    struct collision_work *collision;
    struct collision_hit *hit;
    struct collision_hit *end;
    int kind;

    at = scan_start(task,
                    (const void *)collision_next_item_or_enemy_hit_by_shape);
    if (task) {
        work = task->work;
        if (work && work->collision) {
            /* @hack spelling: as in collision_next_player_hit */
            for (hit = (collision = work->collision)->hits + at,
                end = collision->hits + COLLISION_HITS;
                 hit < end; hit++, at++) {
                if (hit->hit_shape == HIT_UNUSED) {
                    break;
                }
                if (hit->own_shape == index) {
                    kind = collision_get_kind(hit->task);
                    if (kind == COLLISION_ITEM || kind == COLLISION_ENEMY) {
                        scan_found(at);
                        return hit;
                    }
                }
            }
        }
    }
    return NULL;
}

struct task *collision_next_object_by_shape(struct task *task, int index)
{
    struct collision_hit *hit = collision_next_object_hit_by_shape(task, index);

    if (hit) {
        return hit->task;
    }
    return NULL;
}

struct collision_hit *collision_next_object_hit_by_shape(struct task *task,
                                                         int index)
{
    int at;
    struct task_work *work;
    struct collision_work *collision;
    struct collision_hit *hit;
    struct collision_hit *end;
    int kind;

    at = scan_start(task, (const void *)collision_next_object_hit_by_shape);
    if (task) {
        work = task->work;
        if (work && work->collision) {
            /* @hack spelling: as in collision_next_player_hit */
            for (hit = (collision = work->collision)->hits + at,
                end = collision->hits + COLLISION_HITS;
                 hit < end; hit++, at++) {
                if (hit->hit_shape == HIT_UNUSED) {
                    break;
                }
                if (hit->own_shape == index) {
                    kind = collision_get_kind(hit->task);
                    if (kind == COLLISION_OBJECT ||
                        kind == COLLISION_OBJECT_MUTUAL) {
                        scan_found(at);
                        return hit;
                    }
                }
            }
        }
    }
    return NULL;
}

struct task *collision_next_pickup_by_shape(struct task *task, int index)
{
    struct collision_hit *hit = collision_next_pickup_hit_by_shape(task, index);

    if (hit) {
        return hit->task;
    }
    return NULL;
}

struct collision_hit *collision_next_pickup_hit_by_shape(struct task *task,
                                                         int index)
{
    int at;
    struct task_work *work;
    struct collision_work *collision;
    struct collision_hit *hit;
    struct collision_hit *end;
    int kind;

    at = scan_start(task, (const void *)collision_next_pickup_hit_by_shape);
    if (task) {
        work = task->work;
        if (work && work->collision) {
            /* @hack spelling: as in collision_next_player_hit */
            for (hit = (collision = work->collision)->hits + at,
                end = collision->hits + COLLISION_HITS;
                 hit < end; hit++, at++) {
                if (hit->hit_shape == HIT_UNUSED) {
                    break;
                }
                if (hit->own_shape == index) {
                    kind = collision_get_kind(hit->task);
                    if (kind == COLLISION_PICKUP ||
                        kind == COLLISION_PICKUP_TARGET) {
                        scan_found(at);
                        return hit;
                    }
                }
            }
        }
    }
    return NULL;
}

struct task *collision_next_with_id_by_shape(struct task *task, int index,
                                             Sint8 id)
{
    struct collision_hit *hit =
        collision_next_hit_with_id_by_shape(task, index, id);

    if (hit) {
        return hit->task;
    }
    return NULL;
}

struct collision_hit *collision_next_hit_with_id_by_shape(struct task *task,
                                                          int index, Sint8 id)
{
    int at;
    struct task_work *work;
    struct collision_work *collision;
    struct collision_hit *hit;
    struct collision_hit *end;
    Sint16 hit_shape;
    struct task *hit_task;

    at = scan_start(task, (const void *)collision_next_hit_with_id_by_shape);
    if (task) {
        work = task->work;
        if (work && work->collision) {
            /* @hack spelling: as in collision_next_player_hit */
            hit = (collision = work->collision)->hits + at;
            /* @hack spelling: at stepped as at = at + 1, where at++ would
             * do */
            for (end = collision->hits + COLLISION_HITS; hit < end;
                 hit++, at = at + 1) {
                hit_shape = hit->hit_shape;
                if (hit_shape == HIT_UNUSED) {
                    break;
                }
                if (hit->own_shape == index) {
                    hit_task = hit->task;
                    if (hit_task && hit_task->work &&
                        hit_task->work->collision &&
                        hit_task->work->collision->shapes[hit_shape].id == id) {
                        scan_found(at);
                        return hit;
                    }
                }
            }
        }
    }
    return NULL;
}

struct task *collision_next_below(struct task *task, int unused)
{
    struct collision_hit *hit = collision_next_below_hit(task, unused);

    if (hit) {
        return hit->task;
    }
    return NULL;
}

struct collision_hit *collision_next_below_hit(struct task *task, int unused)
{
    int index;
    struct collision_work *collision;
    struct collision_hit *hit;
    struct collision_hit *end;
    Sint16 hit_shape;
    struct task *hit_task;

    index = scan_start(task, (const void *)collision_next_below_hit);
    if (task && task->work) {
        collision = task->work->collision;
        if (collision && (collision->flags & COLLISION_ON_TOP)) {
            end = collision->hits + COLLISION_HITS;
            for (hit = collision->hits + index; hit < end; hit++, index++) {
                hit_shape = hit->hit_shape;
                if (hit_shape == HIT_UNUSED) {
                    break;
                }
                if (hit->flags & HIT_STANDS_ON) {
                    hit_task = hit->task;
                    if (hit_task) {
                        /* @hack redundant: a test of unused that changes
                         * nothing */
                        if (unused) {
                        }
                        scan_found(index);
                        return hit;
                    }
                }
            }
        }
    }
    return NULL;
}

struct task *collision_next_above(struct task *task, int unused)
{
    struct collision_hit *hit = collision_next_above_hit(task, unused);

    if (hit) {
        return hit->task;
    }
    return NULL;
}

struct collision_hit *collision_next_above_hit(struct task *task, int unused)
{
    int index;
    struct collision_work *collision;
    struct collision_hit *hit;
    struct collision_hit *end;
    Sint16 hit_shape;
    struct task *hit_task;

    index = scan_start(task, (const void *)collision_next_above_hit);
    if (task && task->work) {
        collision = task->work->collision;
        if (collision) {
            /* @hack spelling: the flag's test in an if of its own, where
             * one test with && would do */
            if (collision->flags & COLLISION_UNDER) {
                end = collision->hits + COLLISION_HITS;
                for (hit = collision->hits + index; hit < end; hit++, index++) {
                    hit_shape = hit->hit_shape;
                    if (hit_shape == HIT_UNUSED) {
                        break;
                    }
                    if (hit->flags & HIT_STOOD_ON) {
                        hit_task = hit->task;
                        if (hit_task) {
                            /* @hack redundant: as in
                             * collision_next_below_hit */
                            if (unused) {
                            }
                            scan_found(index);
                            return hit;
                        }
                    }
                }
            }
        }
    }
    return NULL;
}

Bool collision_hurt(struct task *task)
{
    struct task_work *work;

    if (task) {
        work = task->work;
        if (work && work->collision &&
            (work->collision->flags & COLLISION_HURT)) {
            return TRUE;
        }
    }
    return FALSE;
}

struct collision_hit *collision_get_hit(struct task *task, int index)
{
    struct collision_work *collision;
    struct collision_hit *hit;

    if (index < 0 || index > COLLISION_HITS - 1 || !task || !task->work) {
        return NULL;
    }
    collision = task->work->collision;
    if (!collision) {
        return NULL;
    }
    hit = &collision->hits[index];
    if (hit->task) {
        return hit;
    }
    return NULL;
}

void collision_set_attack(struct task *task, int index, int level)
{
    struct collision_shape *shape = &task->work->collision->shapes[index];

    shape->levels = (shape->levels & ~SHAPE_ATTACK) | (level & SHAPE_ATTACK);
}

int collision_get_attack(struct task *task, int index)
{
    struct collision_shape *shape = &task->work->collision->shapes[index];

    return shape->levels & SHAPE_ATTACK;
}

void collision_set_guard(struct task *task, int index, int level)
{
    struct collision_shape *shape = &task->work->collision->shapes[index];

    shape->levels = (shape->levels & ~SHAPE_GUARD) |
                    ((level << SHAPE_GUARD_SHIFT) & SHAPE_GUARD);
}

int collision_get_guard(struct task *task, int index)
{
    struct collision_shape *shape = &task->work->collision->shapes[index];

    return (shape->levels & SHAPE_GUARD) >> SHAPE_GUARD_SHIFT;
}

struct task *collision_nearest(struct task *task)
{
    struct collision_work *collision = task->work->collision;
    struct task *nearest = NULL;
    struct collision_hit *hit;
    struct collision_hit *end;
    NJS_POINT3 *pos;
    Float least = NEAREST_REACH;
    Float distance;

    end = collision->hits + COLLISION_HITS;
    pos = &task->work->pos;
    for (hit = collision->hits; hit < end; hit++) {
        if (!hit->task) {
            return nearest;
        }
        distance = njDistanceP2P(pos, &hit->task->work->pos);
        if (distance < least) {
            nearest = hit->task;
            least = distance;
        }
    }
    return nearest;
}

struct task *collision_nearest_player(struct task *task)
{
    struct collision_work *collision = task->work->collision;
    struct task *nearest = NULL;
    struct collision_hit *hit;
    struct collision_hit *end;
    NJS_POINT3 *pos;
    NJS_POINT3 *hit_pos;
    struct task_work *hit_work;
    Float least;
    Float distance;

    end = collision->hits + COLLISION_HITS;
    least = NEAREST_REACH;
    pos = &task->work->pos;
    for (hit = collision->hits; hit < end; hit++) {
        if (!hit->task) {
            break;
        }
        hit_work = hit->task->work;
        if (hit_work->collision->kind != COLLISION_PLAYER) {
            continue;
        }
        hit_pos = &hit_work->pos;
        distance = njDistanceP2P(pos, hit_pos);
        if (distance < least) {
            nearest = hit->task;
            least = distance;
        }
    }
    return nearest;
}

struct task *collision_nearest_item_or_enemy(struct task *task)
{
    struct collision_work *collision = task->work->collision;
    struct task *nearest = NULL;
    struct collision_hit *hit;
    struct collision_hit *end;
    NJS_POINT3 *pos;
    NJS_POINT3 *hit_pos;
    struct task_work *hit_work;
    int kind;
    Float least;
    Float distance;

    end = collision->hits + COLLISION_HITS;
    least = NEAREST_REACH;
    pos = &task->work->pos;
    for (hit = collision->hits; hit < end; hit++) {
        if (!hit->task) {
            return nearest;
        }
        hit_work = hit->task->work;
        kind = hit_work->collision->kind;
        if (kind != COLLISION_ITEM && kind != COLLISION_ENEMY) {
            continue;
        }
        hit_pos = &hit_work->pos;
        distance = njDistanceP2P(pos, hit_pos);
        if (distance < least) {
            nearest = hit->task;
            least = distance;
        }
    }
    return nearest;
}

struct task *collision_nearest_object(struct task *task)
{
    struct collision_work *collision = task->work->collision;
    struct task *nearest = NULL;
    struct collision_hit *hit;
    struct collision_hit *end;
    NJS_POINT3 *pos;
    NJS_POINT3 *hit_pos;
    struct task_work *hit_work;
    int kind;
    Float least;
    Float distance;

    end = collision->hits + COLLISION_HITS;
    least = NEAREST_REACH;
    pos = &task->work->pos;
    for (hit = collision->hits; hit < end; hit++) {
        if (!hit->task) {
            return nearest;
        }
        hit_work = hit->task->work;
        kind = hit_work->collision->kind;
        if (kind != COLLISION_OBJECT && kind != COLLISION_OBJECT_MUTUAL) {
            continue;
        }
        hit_pos = &hit_work->pos;
        distance = njDistanceP2P(pos, hit_pos);
        if (distance < least) {
            nearest = hit->task;
            least = distance;
        }
    }
    return nearest;
}

struct task *collision_nearest_pickup(struct task *task)
{
    struct collision_work *collision = task->work->collision;
    struct task *nearest = NULL;
    struct collision_hit *hit;
    struct collision_hit *end;
    NJS_POINT3 *pos;
    NJS_POINT3 *hit_pos;
    struct task_work *hit_work;
    int kind;
    Float least;
    Float distance;

    end = collision->hits + COLLISION_HITS;
    least = NEAREST_REACH;
    pos = &task->work->pos;
    for (hit = collision->hits; hit < end; hit++) {
        if (!hit->task) {
            return nearest;
        }
        hit_work = hit->task->work;
        kind = hit_work->collision->kind;
        if (kind != COLLISION_PICKUP && kind != COLLISION_PICKUP_TARGET) {
            continue;
        }
        hit_pos = &hit_work->pos;
        distance = njDistanceP2P(pos, hit_pos);
        if (distance < least) {
            nearest = hit->task;
            least = distance;
        }
    }
    return nearest;
}

struct task *collision_nearest_unk_9(struct task *task)
{
    struct collision_work *collision = task->work->collision;
    struct task *nearest = NULL;
    struct collision_hit *hit;
    struct collision_hit *end;
    NJS_POINT3 *pos;
    NJS_POINT3 *hit_pos;
    struct task_work *hit_work;
    Float least;
    Float distance;

    end = collision->hits + COLLISION_HITS;
    least = NEAREST_REACH;
    pos = &task->work->pos;
    for (hit = collision->hits; hit < end; hit++) {
        if (!hit->task) {
            break;
        }
        hit_work = hit->task->work;
        if (hit_work->collision->kind != COLLISION_UNK_9) {
            continue;
        }
        hit_pos = &hit_work->pos;
        distance = njDistanceP2P(pos, hit_pos);
        if (distance < least) {
            nearest = hit->task;
            least = distance;
        }
    }
    return nearest;
}

struct task *collision_nearest_with_id(struct task *task, Sint16 id)
{
    struct collision_work *collision = task->work->collision;
    struct task *nearest = NULL;
    struct collision_hit *hit;
    struct collision_hit *end;
    NJS_POINT3 *pos;
    NJS_POINT3 *hit_pos;
    struct task_work *hit_work;
    Float least;
    Float distance;

    end = collision->hits + COLLISION_HITS;
    least = NEAREST_REACH;
    pos = &task->work->pos;
    for (hit = collision->hits; hit < end; hit++) {
        if (!hit->task) {
            break;
        }
        hit_work = hit->task->work;
        if ((Uint8)hit_work->collision->shapes[hit->hit_shape].id != id) {
            continue;
        }
        hit_pos = &hit_work->pos;
        distance = njDistanceP2P(pos, hit_pos);
        if (distance < least) {
            nearest = hit->task;
            least = distance;
        }
    }
    return nearest;
}

/* the direction of (dx, dz) across x and z, as an angle about y */
#define BEARING(dx, dz) ((Angle)(atan2f((dx), (dz)) * ANGLE_PER_RADIAN))

struct task *collision_nearest_within(struct task *task, Angle ang, Angle range)
{
    struct collision_work *collision = task->work->collision;
    struct task *nearest;
    struct collision_hit *hit;
    struct collision_hit *end;
    NJS_POINT3 *pos;
    NJS_POINT3 *hit_pos;
    Float least;
    Float distance;
    Float dx;
    Float dz;
    Angle bearing;

    end = collision->hits + COLLISION_HITS;
    least = NEAREST_REACH;
    nearest = NULL;
    pos = &task->work->pos;
    for (hit = collision->hits; hit < end; hit++) {
        if (!hit->task) {
            break;
        }
        hit_pos = &hit->task->work->pos;
        dx = hit_pos->x - pos->x;
        dz = hit_pos->z - pos->z;
        bearing = BEARING(dx, dz);
        if (range < angle_distance(ang, bearing)) {
            continue;
        }
        distance = njDistanceP2P(pos, hit_pos);
        if (distance < least) {
            nearest = hit->task;
            least = distance;
        }
    }
    return nearest;
}

struct task *collision_nearest_player_within(struct task *task, Angle ang,
                                             Angle range)
{
    struct collision_work *collision = task->work->collision;
    struct task *nearest;
    struct collision_hit *hit;
    struct collision_hit *end;
    NJS_POINT3 *pos;
    NJS_POINT3 *hit_pos;
    struct task_work *hit_work;
    Float least;
    Float distance;
    Float dx;
    Float dz;
    Angle bearing;

    end = collision->hits + COLLISION_HITS;
    least = NEAREST_REACH;
    nearest = NULL;
    pos = &task->work->pos;
    for (hit = collision->hits; hit < end; hit++) {
        if (!hit->task) {
            break;
        }
        hit_work = hit->task->work;
        if (hit_work->collision->kind != COLLISION_PLAYER) {
            continue;
        }
        hit_pos = &hit_work->pos;
        dx = hit_pos->x - pos->x;
        dz = hit_pos->z - pos->z;
        bearing = BEARING(dx, dz);
        if (range < angle_distance(ang, bearing)) {
            continue;
        }
        distance = njDistanceP2P(pos, hit_pos);
        if (distance < least) {
            nearest = hit->task;
            least = distance;
        }
    }
    return nearest;
}

struct task *collision_nearest_item_or_enemy_within(struct task *task,
                                                    Angle ang, Angle range)
{
    struct collision_work *collision = task->work->collision;
    struct task *nearest;
    struct collision_hit *hit;
    struct collision_hit *end;
    NJS_POINT3 *pos;
    NJS_POINT3 *hit_pos;
    struct task_work *hit_work;
    Float least;
    Float distance;
    int kind;
    Float dx;
    Float dz;
    Angle bearing;

    end = collision->hits + COLLISION_HITS;
    least = NEAREST_REACH;
    nearest = NULL;
    pos = &task->work->pos;
    for (hit = collision->hits; hit < end; hit++) {
        if (!hit->task) {
            break;
        }
        hit_work = hit->task->work;
        kind = hit_work->collision->kind;
        if (kind != COLLISION_ITEM && kind != COLLISION_ENEMY) {
            continue;
        }
        hit_pos = &hit_work->pos;
        dx = hit_pos->x - pos->x;
        dz = hit_pos->z - pos->z;
        bearing = BEARING(dx, dz);
        if (range < angle_distance(ang, bearing)) {
            continue;
        }
        distance = njDistanceP2P(pos, hit_pos);
        if (distance < least) {
            nearest = hit->task;
            least = distance;
        }
    }
    return nearest;
}

struct task *collision_nearest_object_within(struct task *task, Angle ang,
                                             Angle range)
{
    struct collision_work *collision = task->work->collision;
    struct task *nearest;
    struct collision_hit *hit;
    struct collision_hit *end;
    NJS_POINT3 *pos;
    NJS_POINT3 *hit_pos;
    struct task_work *hit_work;
    Float least;
    Float distance;
    int kind;
    Float dx;
    Float dz;
    Angle bearing;

    end = collision->hits + COLLISION_HITS;
    least = NEAREST_REACH;
    nearest = NULL;
    pos = &task->work->pos;
    for (hit = collision->hits; hit < end; hit++) {
        if (!hit->task) {
            break;
        }
        hit_work = hit->task->work;
        kind = hit_work->collision->kind;
        if (kind != COLLISION_OBJECT && kind != COLLISION_OBJECT_MUTUAL) {
            continue;
        }
        hit_pos = &hit_work->pos;
        dx = hit_pos->x - pos->x;
        dz = hit_pos->z - pos->z;
        bearing = BEARING(dx, dz);
        if (range < angle_distance(ang, bearing)) {
            continue;
        }
        distance = njDistanceP2P(pos, hit_pos);
        if (distance < least) {
            nearest = hit->task;
            least = distance;
        }
    }
    return nearest;
}

struct task *collision_nearest_pickup_within(struct task *task, Angle ang,
                                             Angle range)
{
    struct collision_work *collision = task->work->collision;
    struct task *nearest;
    struct collision_hit *hit;
    struct collision_hit *end;
    NJS_POINT3 *pos;
    NJS_POINT3 *hit_pos;
    struct task_work *hit_work;
    Float least;
    Float distance;
    int kind;
    Float dx;
    Float dz;
    Angle bearing;

    end = collision->hits + COLLISION_HITS;
    least = NEAREST_REACH;
    nearest = NULL;
    pos = &task->work->pos;
    for (hit = collision->hits; hit < end; hit++) {
        if (!hit->task) {
            break;
        }
        hit_work = hit->task->work;
        kind = hit_work->collision->kind;
        if (kind != COLLISION_PICKUP && kind != COLLISION_PICKUP_TARGET) {
            continue;
        }
        hit_pos = &hit_work->pos;
        dx = hit_pos->x - pos->x;
        dz = hit_pos->z - pos->z;
        bearing = BEARING(dx, dz);
        if (range < angle_distance(ang, bearing)) {
            continue;
        }
        distance = njDistanceP2P(pos, hit_pos);
        if (distance < least) {
            nearest = hit->task;
            least = distance;
        }
    }
    return nearest;
}

struct task *collision_nearest_unk_9_within(struct task *task, Angle ang,
                                            Angle range)
{
    struct collision_work *collision = task->work->collision;
    struct task *nearest;
    struct collision_hit *hit;
    struct collision_hit *end;
    NJS_POINT3 *pos;
    NJS_POINT3 *hit_pos;
    struct task_work *hit_work;
    Float least;
    Float distance;
    Float dx;
    Float dz;
    Angle bearing;

    end = collision->hits + COLLISION_HITS;
    least = NEAREST_REACH;
    nearest = NULL;
    pos = &task->work->pos;
    for (hit = collision->hits; hit < end; hit++) {
        if (!hit->task) {
            break;
        }
        hit_work = hit->task->work;
        if (hit_work->collision->kind != COLLISION_UNK_9) {
            continue;
        }
        hit_pos = &hit_work->pos;
        dx = hit_pos->x - pos->x;
        dz = hit_pos->z - pos->z;
        bearing = BEARING(dx, dz);
        if (range < angle_distance(ang, bearing)) {
            continue;
        }
        distance = njDistanceP2P(pos, hit_pos);
        if (distance < least) {
            nearest = hit->task;
            least = distance;
        }
    }
    return nearest;
}

struct task *collision_nearest_with_id_within(struct task *task, Angle ang,
                                              Angle range, Sint16 id)
{
    struct collision_work *collision = task->work->collision;
    struct task *nearest;
    struct collision_hit *hit;
    struct collision_hit *end;
    NJS_POINT3 *pos;
    NJS_POINT3 *hit_pos;
    struct task_work *hit_work;
    Float least;
    Float distance;
    Float dx;
    Float dz;
    Angle bearing;

    end = collision->hits + COLLISION_HITS;
    least = NEAREST_REACH;
    nearest = NULL;
    pos = &task->work->pos;
    for (hit = collision->hits; hit < end; hit++) {
        if (!hit->task) {
            break;
        }
        hit_work = hit->task->work;
        if ((Uint8)hit_work->collision->shapes[hit->hit_shape].id != id) {
            continue;
        }
        hit_pos = &hit_work->pos;
        dx = hit_pos->x - pos->x;
        dz = hit_pos->z - pos->z;
        bearing = BEARING(dx, dz);
        if (range < angle_distance(ang, bearing)) {
            continue;
        }
        distance = njDistanceP2P(pos, hit_pos);
        if (distance < least) {
            nearest = hit->task;
            least = distance;
        }
    }
    return nearest;
}

int collision_get_kind(struct task *task)
{
    struct task_work *work;
    struct collision_work *collision;

    if (!task) {
        return -1;
    }
    work = task->work;
    if (!work) {
        return -1;
    }
    collision = work->collision;
    if (!collision) {
        return -1;
    }
    return collision->kind;
}

int collision_get_form(struct task *task, int index)
{
    struct task_work *work;
    struct collision_work *collision;

    if (index < 0 || index >= FORM_SHAPES || !task) {
        return -1;
    }
    work = task->work;
    if (!work) {
        return -1;
    }
    collision = work->collision;
    if (!collision) {
        return -1;
    }
    return collision->shapes[index].form;
}

void collision_shape_on(struct task *task, int index)
{
    struct task_work *work;
    struct collision_work *collision;

    if (!task) {
        return;
    }
    work = task->work;
    if (!work) {
        return;
    }
    collision = work->collision;
    if (!collision) {
        return;
    }
    collision->shapes[index].flags &= ~SHAPE_OFF;
}

void collision_shape_off(struct task *task, int index)
{
    struct task_work *work;
    struct collision_work *collision;

    if (!task) {
        return;
    }
    work = task->work;
    if (!work) {
        return;
    }
    collision = work->collision;
    if (!collision) {
        return;
    }
    collision->shapes[index].flags |= SHAPE_OFF;
}

struct collision_shape *collision_get_shape(struct task *task, int index)
{
    struct task_work *work;

    if (task) {
        work = task->work;
        if (work && work->collision) {
            return &work->collision->shapes[index];
        }
    }
    /* @bug uninitialized: without a task, a work or a record, returns R0 as
     * the caller left it, as the BTs at 0x8c020786, 0x8c02078c and 0x8c020792
     * skip to the RTS at 0x8c0207a4 */
#ifdef NONMATCHING
    /* this side repairs the result the tag names: R0 as the caller left it,
     * which C can't give. this side returns NULL */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

void collision_set_shape_center(struct task *task, int index,
                                const NJS_POINT3 *center)
{
    struct task_work *work;
    struct collision_work *collision;

    if (!task) {
        return;
    }
    work = task->work;
    if (!work) {
        return;
    }
    collision = work->collision;
    if (!collision) {
        return;
    }
    collision->shapes[index].center = *center;
}

void collision_set_shape_angles(struct task *task, int index,
                                const struct angles *ang)
{
    struct task_work *work;
    struct collision_work *collision;
    struct collision_shape *shape;

    if (!task) {
        return;
    }
    work = task->work;
    if (!work) {
        return;
    }
    collision = work->collision;
    if (!collision) {
        return;
    }
    shape = &collision->shapes[index];
    shape->ang.x = ang->x;
    shape->ang.y = ang->y;
    shape->ang.z = ang->z;
}

int collision_get_count(int list)
{
    /* @hack redundant: a break after each case's return */
    switch (list) {
    case COLLISION_COUNT_ITEMS:
        return collision_item_count;
        break;
    case COLLISION_COUNT_ENEMIES:
        return collision_enemy_count;
        break;
    default:
        return -1;
    }
}

int collision_get_last_count(int list)
{
    /* @hack redundant: as in collision_get_count */
    switch (list) {
    case COLLISION_COUNT_ITEMS:
        return collision_last_item_count;
        break;
    case COLLISION_COUNT_ENEMIES:
        return collision_last_enemy_count;
        break;
    default:
        return -1;
    }
}

void collision_set_up_axis(int axis)
{
    collision_up_axis = axis;
}

int collision_get_up_axis(void)
{
    return collision_up_axis;
}
