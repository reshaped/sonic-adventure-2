/* collision records, the checks that test them each step, and the calls that
 * read what the checks found */
#ifndef COLLISION_COLLISION_H
#define COLLISION_COLLISION_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct task;
struct task_work;

/* the kinds of collision record. collision_register enters a record in its
 * kind's list, and collision_collide_all tests the lists in pairs. a record
 * of each kind below comes first in a pair with each record of the kinds
 * beside it, less COLLISION_:
 *
 *   PLAYER         every kind but ATTACK
 *   ATTACK         every kind but ATTACK, PICKUP, PICKUP_TARGET and WALL
 *   UNK_9          every kind but PLAYER, ATTACK and WALL
 *   ENEMY          ENEMY, OBJECT and OBJECT_MUTUAL
 *   OBJECT         OBJECT_MUTUAL
 *   OBJECT_MUTUAL  OBJECT_MUTUAL
 *
 * no other kind comes first. within one kind, each two records are tested
 * once */
#define COLLISION_PLAYER 0
/* no shape pushes an attack, hurts it or ignores its kind */
#define COLLISION_ATTACK 1
#define COLLISION_ITEM 2
#define COLLISION_ENEMY 3
#define COLLISION_OBJECT 4
#define COLLISION_OBJECT_MUTUAL 5
#define COLLISION_PICKUP 6
/* a pickup that collision_init makes a target */
#define COLLISION_PICKUP_TARGET 7
/* a wall or a bound */
#define COLLISION_WALL 8
/* shapes push and hurt its records as they do players', but pass them over only
 * under SHAPE_IGNORE_UNK_9 */
#define COLLISION_UNK_9 9
#define COLLISION_KINDS 10

/* in a record's flags. the checks set these six, and collision_reset clears
 * them */
#define COLLISION_TOUCHED 0x1 /* it kept a touch, or a push was tried on it */
#define COLLISION_UNDER 0x2 /* another record it touched lies on top of it */
#define COLLISION_SIDE 0x4 /* it touched another record at a side */
#define COLLISION_ON_TOP 0x8 /* it lies on top of another record it touched */
#define COLLISION_PUSHED 0x10 /* a test let another record's shape push it */
#define COLLISION_HURT 0x20 /* one of its shapes was hurt */
/* the six but COLLISION_HURT: those of a touch */
#define COLLISION_TOUCH_FLAGS                                                  \
    (COLLISION_TOUCHED | COLLISION_UNDER | COLLISION_SIDE | COLLISION_ON_TOP | \
     COLLISION_PUSHED)
#define COLLISION_CHECKS_FLAGS (COLLISION_TOUCH_FLAGS | COLLISION_HURT)
/* in a record's flags: a target, which the checks list near the players.
 * collision_init and collision_init_in_place set it in the record of an item,
 * an enemy or a pickup target */
#define COLLISION_TARGET 0x40
/* the record's shapes are its own copy, which collision_free frees */
#define COLLISION_OWN_SHAPES 0x8000

/* in a task's work flags. a test sets WORK_HURT where it sets COLLISION_HURT in
 * the task's record. where one record lies on another, the upper one's work
 * takes WORK_ON_TOP and the lower one's WORK_UNDER. a test's first record is
 * the pair's first. where a sphere meets an upright cylinder, a SHAPE_CYLINDER2
 * or a SHAPE_BOX, it is the sphere's. that record's work takes its bit only
 * where the record is a player's. its hits must include an object's, as
 * collision_next_object finds them. a test sets the two bits only where its
 * push goes ahead. two tests of a SHAPE_STANDING shape are the exceptions.
 * against a SHAPE_BOX2, the box's bit goes regardless. at a SHAPE_CYLINDER2's
 * end, both do */
#define WORK_UNK_1 0x1
#define WORK_HURT 0x4
#define WORK_UNK_8 0x8
#define WORK_UNDER 0x10
#define WORK_ON_TOP 0x20

/* in a hit's flags, which collision_reset clears. where a record lands on
 * another's top, HIT_STANDS_ON goes in the lander's latest hit. HIT_STOOD_ON
 * goes in the other's. each goes only where that hit is of the two shapes.
 * neither goes where the lander kept no hit. an upright cylinder that lands on
 * a sphere, or on another that comes first, sets neither. of strikes from
 * below, only a SHAPE_STANDING shape's strike on a SHAPE_BOX's bottom sets
 * them. the box then stands on the shape. a SHAPE_STANDING shape pushed out
 * past either end of a SHAPE_CYLINDER2 sets them as a lander. it does whichever
 * way the end faces */
#define HIT_STANDS_ON 0x1 /* this record stands on the hit's */
#define HIT_STOOD_ON 0x2 /* the hit's record stands on this one */

/* the forms of shape. the checks test a shape of each form below, where its
 * record comes first, against a shape of the forms beside it, less SHAPE_.
 * they pass every other pair over:
 *
 *   SPHERE        SPHERE, CYLINDER, CYLINDER2, BOX and CAPSULE
 *   CYLINDER      SPHERE, CYLINDER and BOX
 *   CYLINDER2     SPHERE
 *   BOX           SPHERE
 *   STANDING      every form but SWEPT_SPHERE and UNK_10
 *   SWEPT_SPHERE  SPHERE and STANDING
 */
#define SHAPE_SPHERE 0
/* an upright cylinder */
#define SHAPE_CYLINDER 1
/* a cylinder whose axis turns by its task's angles, then by its own */
#define SHAPE_CYLINDER2 2
/* a box turned about y by its task's ang.y plus its own */
#define SHAPE_BOX 3
/* a SHAPE_BOX turned about z too, by its own ang.z */
#define SHAPE_BOX2 4
/* a box along the world's axes, never turned */
#define SHAPE_AXIS_BOX 5
/* a SHAPE_CYLINDER2 with round ends */
#define SHAPE_CAPSULE 6
/* an upright cylinder that lands on others, up to size_z past their edges.
 * against any box, a wall or a SHAPE_CYLINDER with PUSH_PLAYERS, its foot is
 * its task's position. against any other SHAPE_CYLINDER, a sphere, a capsule or
 * another SHAPE_STANDING, it is a sphere of its radius */
#define SHAPE_STANDING 7
/* a sphere swept to its task's position, from where SHAPE_SWEEP_FROM_CENTER
 * says. it touches and hurts as the other forms do, but pushes nothing */
#define SHAPE_SWEPT_SPHERE 8
/* a face across x and y, turned about y by its task's ang.y plus its own. it
 * puts a SHAPE_STANDING shape that reaches through it back in front of it */
#define SHAPE_WALL 9
#define SHAPE_UNK_10 10

/* in a shape's flags */
#define SHAPE_NO_OTHER_HITS 0x4 /* a record it touches keeps no hit of it */
#define SHAPE_NO_OWN_HITS 0x8 /* its record keeps no hit of its touches */
#define SHAPE_OFF 0x10 /* the checks pass it over */
/* its center is a world position, and its task's angles turn it only where it
 * is a SHAPE_BOX */
#define SHAPE_IN_WORLD 0x20
/* collision_init_attack's shape takes it. no code of the checks reads it */
#define SHAPE_UNK_40 0x40
/* the angles that turn its center, and a SHAPE_CYLINDER2's or a SHAPE_CAPSULE's
 * axis, apply in the order njRotateY, njRotateZ and njRotateX. without the flag
 * the order is njRotateZ, njRotateX and njRotateY */
#define SHAPE_TURN_YZX 0x200
#define SHAPE_HURTABLE 0x400 /* another shape's attack can hurt it */
/* a hurt shape that has it pushes no other shape that has it */
#define SHAPE_HURT_NO_PUSH 0x4000
/* its center is an offset from its task's position, which its task's angles
 * don't turn. they turn the shape only where it is a SHAPE_BOX */
#define SHAPE_UNTURNED 0x8000
/* on a SHAPE_BOX or a SHAPE_AXIS_BOX. a SHAPE_STANDING shape it pushes out
 * through a side doesn't slide along it. the shape goes back to where it
 * crossed the side, or to where it stood the step before */
#define SHAPE_NO_SLIDE 0x10000
/* the checks pass the shape over against every record of the kinds beside
 * each */
#define SHAPE_IGNORE_PLAYERS 0x40000 /* players */
#define SHAPE_IGNORE_ENEMIES 0x80000 /* items and enemies */
#define SHAPE_IGNORE_OBJECTS 0x100000 /* objects of both kinds */
#define SHAPE_IGNORE_PICKUPS 0x200000 /* pickups of both kinds */
#define SHAPE_IGNORE_UNK_9 0x400000 /* COLLISION_UNK_9 */
/* every kind but players, attacks and walls, which no flag names */
#define SHAPE_IGNORE_ALL_BUT_PLAYERS                                           \
    (SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS | SHAPE_IGNORE_PICKUPS |      \
     SHAPE_IGNORE_UNK_9)
/* a hunter can dig into a shape with the flag. hunter_offer_dig reads it in the
 * shapes the hunter's record touched */
#define SHAPE_DIGGABLE 0x2000000
/* on a SHAPE_SWEPT_SPHERE: its sweep starts at its center, as a world position.
 * without the flag the sweep starts 0.001 along x from its task's position, and
 * the test then sets the flag */
#define SHAPE_SWEEP_FROM_CENTER 0x40000000
/* in a shape's flags, unread by the checks */
#define SHAPE_UNK_2000 0x2000
#define SHAPE_UNK_800000 0x800000

/* a shape's levels: its attack in the low two bits, then its guard, then flags.
 * a touch hurts a SHAPE_HURTABLE shape where the other shape's attack is above
 * its guard. the other must also have the SHAPE_HURTS_ flag of the hurt shape's
 * kind. no flag names attacks, pickups or walls, so nothing hurts their
 * shapes */
#define SHAPE_ATTACK 0x3
#define SHAPE_GUARD 0xc
#define SHAPE_HURT 0x10 /* it was hurt this step */
#define SHAPE_HURTS_PLAYERS 0x20 /* players and COLLISION_UNK_9 */
#define SHAPE_HURTS_ENEMIES 0x40 /* items and enemies */
#define SHAPE_HURTS_OBJECTS 0x80 /* objects of both kinds */
#define SHAPE_HURTS_ALL                                                        \
    (SHAPE_HURTS_PLAYERS | SHAPE_HURTS_ENEMIES | SHAPE_HURTS_OBJECTS)
/* the bits of a shape's levels for an attack or a guard of level, 0 to 3 */
#define SHAPE_GUARD_SHIFT 2
#define SHAPE_ATTACK_LEVEL(level) (level)
#define SHAPE_GUARD_LEVEL(level) ((level) << SHAPE_GUARD_SHIFT)
/* the highest level. no attack lies above a guard of it */
#define SHAPE_TOP_LEVEL 3

/* a shape's push. its low four bits name the groups of kinds whose shapes it
 * pushes. PUSH_FIRM(groups) puts those bits in the high four, and the groups'
 * shapes then can't push it. attacks are in no group, and they push no shape
 * that has any bit of push. a shape with any bit of push pushes
 * COLLISION_WALL's shapes, and no PUSH_FIRM bit resists theirs */
#define PUSH_PLAYERS 0x1 /* players and COLLISION_UNK_9 */
#define PUSH_ENEMIES 0x2 /* items and enemies */
#define PUSH_OBJECTS 0x4 /* objects of both kinds */
#define PUSH_PICKUPS 0x8 /* pickups of both kinds */
#define PUSH_ALL_BUT_PICKUPS (PUSH_PLAYERS | PUSH_ENEMIES | PUSH_OBJECTS)
#define PUSH_FIRM_SHIFT 4
#define PUSH_FIRM(groups) ((groups) << PUSH_FIRM_SHIFT)
/* the push of a shape that pushes groups' shapes, none of which pushes it */
#define PUSH_SOLID(groups) ((groups) | PUSH_FIRM(groups))
/* every group */
#define PUSH_ALL (PUSH_PLAYERS | PUSH_ENEMIES | PUSH_OBJECTS | PUSH_PICKUPS)

/* the directions the checks can take as up, which collision_set_up_axis sets.
 * against a SHAPE_BOX, a SHAPE_AXIS_BOX or a SHAPE_CYLINDER with PUSH_PLAYERS,
 * the checks put a SHAPE_STANDING shape's center size_y up from its task's
 * position. a SHAPE_STANDING shape lands on the side of a SHAPE_AXIS_BOX that
 * faces up */
#define COLLISION_UP_NEG_X 0
#define COLLISION_UP_X 1
#define COLLISION_UP_NEG_Y 2
#define COLLISION_UP_Y 3
#define COLLISION_UP_NEG_Z 4
#define COLLISION_UP_Z 5

/* the most hits a record keeps */
#define COLLISION_HITS 16

/* one shape, as a task describes it to collision_init */
struct collision_shape {
    Sint8 id; /* its task's number for it, which the _with_id calls match */
    Uint8 form; /* SHAPE_SPHERE to SHAPE_UNK_10 */
    Uint8 push; /* the groups it pushes, and PUSH_FIRM of those it resists */
    Uint8 levels; /* its attack and guard, then SHAPE_HURT and the rest */
    Uint32 flags; /* SHAPE_NO_OTHER_HITS and the rest */
    /* an offset from its task's position, which its task's angles turn. it
     * isn't turned under SHAPE_UNTURNED or in a SHAPE_AXIS_BOX. under
     * SHAPE_IN_WORLD it is a world position */
    NJS_POINT3 center;
    Float radius; /* its radius, or half its size along x */
    Float size_y; /* half its height or length, or half its size along y */
    /* half a box's or a wall's size along z. in a SHAPE_STANDING shape, how far
     * past the edges of what it lands on it may land */
    Float size_z;
    Uint32 unk_20;
    struct angles ang; /* its own angles, which turn it as its form says */
};

/* a touch a record keeps */
struct collision_hit {
    Sint8 own_shape; /* the index of the record's shape that touched */
    /* the index of the touched shape in the touched task's record, or
     * HIT_UNUSED in an unused hit, which ends the kept ones */
    Sint8 hit_shape;
    Uint16 flags; /* HIT_STANDS_ON and HIT_STOOD_ON */
    struct task *task; /* the touched task */
};
/* the hit_shape of an unused hit */
#define HIT_UNUSED (-1)

/* a task's collision record, at its work's collision */
struct collision_work {
    Uint16 kind; /* COLLISION_PLAYER to COLLISION_UNK_9 */
    Sint16 hit_count; /* how many hits it kept this step */
    Uint16 flags; /* COLLISION_TOUCHED and the rest */
    Uint16 count; /* how many shapes it has */
    /* how far its shapes reach from its task's position. the checks pass over
     * two records whose tasks lie their radii's sum apart or more */
    Float radius;
    struct collision_shape *shapes; /* owned only with COLLISION_OWN_SHAPES */
    /* the touches it kept this step, in the order the checks found them */
    struct collision_hit hits[COLLISION_HITS];
    /* a direction out of the touched shape toward its own. some tests write it
     * at a touch, and collision_reset leaves it. the tests of spheres and
     * upright cylinders write only its x and z. those make a unit vector, or 0
     * and 0.1 where the centers share x and z. the box tests write a side's
     * normal. it points into the box where a sphere or an upright cylinder
     * meets a SHAPE_BOX. a SHAPE_AXIS_BOX's landing or strike makes it 100
     * along one axis: (0, 0, 100) under COLLISION_UP_Y */
    NJS_VECTOR normal;
    struct task *owner; /* the task whose work holds it */
    /* the latest touch this step that the shapes' flags let it keep: the index
     * of its shape, the index of the touched shape, and the touched record. a
     * touch sets them even where every hit is in use. collision_reset makes
     * them -1, -1 and null */
    Sint16 own_shape;
    Sint16 hit_shape;
    struct collision_work *hit;
};

/* a target near a player, and how near */
struct near_target {
    struct task_work *work;
    Float distance; /* squared, between the two tasks' positions */
};

/* the most entries a near_targets list and a near_rings list hold. an entry
 * whose work is null ends each, and isn't counted */
#define NEAR_TARGETS 656
#define NEAR_RINGS 256

/* the most records each kind's list holds */
#define COLLISION_PLAYERS 16
#define COLLISION_ATTACKS 128
#define COLLISION_ITEMS 128
#define COLLISION_ENEMIES 128
#define COLLISION_OBJECTS 256
#define COLLISION_MUTUAL_OBJECTS 128
#define COLLISION_PICKUPS 128
#define COLLISION_PICKUP_TARGETS 128
#define COLLISION_WALLS 256
#define COLLISION_UNK_9S 16

/* collision_get_count's and collision_get_last_count's lists */
#define COLLISION_COUNT_ITEMS 0 /* collision_items */
#define COLLISION_COUNT_ENEMIES 1 /* collision_enemies */

/* SHC places the source's bss in the order it first meets the names. so the
 * lists and counts below are declared here in the order of their addresses.
 * they come ahead of the lists collision.c alone declares */

/* the targets within 100 of player 0's task at the last checks, in the order
 * met. the _player1 lists hold player 1's. a near_rings list takes a target
 * whose work id is WORK_ID_RING, and a near_targets list any other. a
 * COLLISION_PLAYER record of neither player has no lists. a target near it
 * overwrites the first three words of its work, mode to ang.x */
extern struct near_target near_targets[NEAR_TARGETS + 1];
extern struct near_target near_rings[NEAR_RINGS + 1];
extern struct near_target near_targets_player1[NEAR_TARGETS + 1];
extern struct near_target near_rings_player1[NEAR_RINGS + 1];
/* the records collision_register enters, a list for each kind, and the count
 * each list holds. collision_collide_all and collision_clear empty the lists,
 * first keeping the items' and the enemies' counts in the _last counts.
 * emptying a list leaves its entries until collision_register fills it again */
extern Uint16 collision_player_count;
extern Uint16 collision_attack_count;
extern Uint16 collision_item_count;
extern Uint16 collision_enemy_count;
extern Uint16 collision_last_item_count;
extern Uint16 collision_last_enemy_count;
extern Uint16 collision_object_count;
extern Uint16 collision_mutual_object_count;
extern Uint16 collision_pickup_count;
extern Uint16 collision_pickup_target_count;
extern Uint16 collision_wall_count;
extern Uint16 collision_unk_9_count;
extern struct task *collision_players[COLLISION_PLAYERS];
extern struct task *collision_attacks[COLLISION_ATTACKS];
extern struct task *collision_items[COLLISION_ITEMS];
extern struct task *collision_enemies[COLLISION_ENEMIES];

/* give task's work a record of kind with its own copy of count shapes, and
 * return TRUE. the record takes the radius collision_fit_radius finds. a record
 * the work already had is replaced, and leaks. task must not be null,
 * unchecked.
 *
 * it returns FALSE without a work, or where the record can't be allocated, and
 * changes nothing. in the portable build, a failed shape allocation frees the
 * new record and preserves the work's previous record. the original frees
 * the previous record and loses the new allocation. */
Bool collision_init(struct task *task, const struct collision_shape *shapes,
                    int count, Uint8 kind);
/* collision_init, but the record uses shapes in place, without a copy. the
 * checks and the calls write to the caller's shapes, which must outlive the
 * record. collision_free leaves them. it returns FALSE only without a work, or
 * where the record can't be allocated */
Bool collision_init_in_place(struct task *task, struct collision_shape *shapes,
                             int count, Uint8 kind);
/* give task's work a COLLISION_ATTACK record of one shape of its own, and
 * return TRUE. the shape takes form, radius, and flags with SHAPE_UNK_40. its
 * center, other sizes, levels and push are 0, and so is the record's radius. it
 * fails as collision_init does. the _sphere and _swept forms pass the form
 * their names say. nothing in the game calls those two, and only they call
 * collision_init_attack */
Bool collision_init_attack(struct task *task, Float radius, Uint32 flags,
                           int form);
Bool collision_init_attack_sphere(struct task *task, Float radius,
                                  Uint32 flags);
Bool collision_init_attack_swept(struct task *task, Float radius, Uint32 flags);
/* clear what the checks left in task's record. that is its hits, its latest
 * touch, its COLLISION_CHECKS_FLAGS and each shape's SHAPE_HURT. task, its work
 * and its record must not be null, unchecked */
void collision_reset(struct task *task);
/* collision_reset task's record, enter it in its kind's list for the next
 * checks, and return TRUE. the reset clears what the last checks found, so a
 * task must read its hits before it registers.
 *
 * it returns FALSE and changes nothing without a record. it does the same while
 * the record's owner has task_destroy for its step, as task_kill leaves it.
 * after the reset, it returns FALSE where the kind's list is full, or the kind
 * is unknown. task's work must not be null, unchecked. an entered record must
 * live until the lists are emptied */
Bool collision_register(struct task *task);
/* set the radius of task's record to 1.01 times its shapes' farthest reach from
 * the task's position. where that reach is under 0.1, the radius is 25. a shape
 * reaches the length of its center plus its extent. the extent is radius in a
 * sphere and radius + size_y in a capsule. in a SHAPE_CYLINDER or a
 * SHAPE_CYLINDER2 it is the length of (radius, size_y). in any other form it is
 * the length of (radius, size_y, size_z). a SHAPE_IN_WORLD shape's center
 * counts as an offset too. without a record it does nothing. task and its work
 * must not be null, unchecked */
void collision_fit_radius(struct task *task);
/* collision_fit_radius(collision->owner) */
void collision_fit_record_radius(struct collision_work *collision);
/* set the radius of task's record. without a record it does nothing. task and
 * its work must not be null, unchecked */
void collision_set_radius(struct task *task, Float radius);
/* free task's record, and its shapes where it owns them, and set the work's
 * collision to null. without a task, a work or a record it does nothing. a
 * record collision_register entered must live until the lists are emptied */
void collision_free(struct task *task);

/* empty the lists collision_register fills and the near lists, start every scan
 * over, and make COLLISION_UP_Y the up axis */
void collision_clear(void);
/* run the checks on the records in the lists, in the pairs of kinds above. then
 * empty the lists. the checks write their results into the records and remake
 * the near lists. they also move tasks and change bits of their works' flags.
 * they can zero a landing task's ang.x and ang.z, and write a player's push in
 * player_records. they push a matrix on Ninja's stack for their run, which must
 * have room for it, unchecked.
 *
 * it starts the scans over before the checks, and the checks then scan players'
 * records with collision_next_object. so a collision_next_object scan of the
 * last task they scanned goes on from where they left it */
void collision_collide_all(void);
/* start the next scan from the first hit, whatever task it scans */
void collision_restart_scans(void);
/* set or get the checks' up axis. axis must be one of COLLISION_UP_NEG_X to
 * COLLISION_UP_Z, unchecked. world_reset_gravity makes it COLLISION_UP_Y, as
 * collision_clear does */
void collision_set_up_axis(int axis);
int collision_get_up_axis(void);

/* where work's record has COLLISION_SIDE, copy its normal's x and z into normal
 * and return TRUE. the normal need not be the side touch's: some tests set
 * COLLISION_SIDE without writing it. without COLLISION_SIDE it returns FALSE
 * and copies nothing. normal->y stays. work's record must not be null,
 * unchecked */
Bool collision_get_normal(struct task_work *work, NJS_VECTOR *normal);
/* it returns TRUE. nothing in the game calls it */
Bool fun_8c01f2b0(void);
/* how many hits task's record kept this step, or 0 without a task or a work.
 * without a record the result is undefined */
int collision_get_hit_count(struct task *task);
/* TRUE where task's record has the flag each call names: COLLISION_TOUCHED,
 * COLLISION_PUSHED or COLLISION_HURT. FALSE where it hasn't, or without a task,
 * a work or a record */
Bool collision_touched(struct task *task);
Bool collision_pushed(struct task *task);
Bool collision_hurt(struct task *task);
/* the hit at index in task's record. it returns null where index lies outside 0
 * to COLLISION_HITS - 1, or the hit's task is null. it also returns null
 * without a task, a work or a record */
struct collision_hit *collision_get_hit(struct task *task, int index);
/* the kind of task's record, or -1 without a task, a work or a record */
int collision_get_kind(struct task *task);
/* how many tasks a list holds: list is COLLISION_COUNT_ITEMS or
 * COLLISION_COUNT_ENEMIES. collision_get_count gives the count the list has
 * now, and collision_get_last_count the one it had when the lists were last
 * emptied. any other list gives -1. nothing in the game calls
 * collision_get_count */
int collision_get_count(int list);
int collision_get_last_count(int list);

/* the calls on shape index of task's record. index must name a shape,
 * unchecked */

/* set or get the shape's attack or guard level, 0 to 3. a set takes level's low
 * two bits. task, its work and its record must not be null, unchecked */
void collision_set_attack(struct task *task, int index, int level);
int collision_get_attack(struct task *task, int index);
void collision_set_guard(struct task *task, int index, int level);
int collision_get_guard(struct task *task, int index);
/* the shape's form, or -1 where index lies outside 0 to 15, or without a task,
 * a work or a record. it reads the shape at index whether the record has that
 * many or not. nothing in the game calls it */
int collision_get_form(struct task *task, int index);
/* clear or set the shape's SHAPE_OFF. without a task, a work or a record they
 * do nothing */
void collision_shape_on(struct task *task, int index);
void collision_shape_off(struct task *task, int index);
/* the shape. without a task, a work or a record the result is undefined */
struct collision_shape *collision_get_shape(struct task *task, int index);
/* set the shape's center, or its own angles. without a task, a work or a record
 * they do nothing. nothing in the game calls collision_set_shape_angles */
void collision_set_shape_center(struct task *task, int index,
                                const NJS_POINT3 *center);
void collision_set_shape_angles(struct task *task, int index,
                                const struct angles *ang);

/* set pos to where shape's center lies in the world while work stands as it
 * does now. shape need not be one of work's record's. where work's angles turn
 * the center, it pushes a matrix on Ninja's stack and pops one after. the stack
 * must have room for it, unchecked */
void collision_get_shape_center(struct task_work *work,
                                const struct collision_shape *shape,
                                NJS_POINT3 *pos);

/* the scans. each walks the hits of task's record for the next hit its comment
 * names. it returns that hit's task, or null where none is left. it stops at
 * the first unused hit. a scan's _hit form is the same scan, and returns the
 * hit itself. without a task, a work or a record a scan finds nothing. a hit
 * task without a work or a record is of no kind.
 *
 * the scans share one place in the hits. a scan called right after itself for
 * the same task goes on after the hit it found last. after a call of another
 * scan or for another task, it starts over from the first hit. so it does after
 * collision_restart_scans. once a scan finds none, it finds none until it
 * starts over, while the hits stay */
struct task *collision_next_player(struct task *task); /* a player's */
struct collision_hit *collision_next_player_hit(struct task *task);
struct task *collision_next_attack(struct task *task); /* an attack's */
struct collision_hit *collision_next_attack_hit(struct task *task);
/* an item's or an enemy's */
struct task *collision_next_item_or_enemy(struct task *task);
struct collision_hit *collision_next_item_or_enemy_hit(struct task *task);
/* an object's of either kind */
struct task *collision_next_object(struct task *task);
struct collision_hit *collision_next_object_hit(struct task *task);
/* a pickup's of either kind. nothing in the game calls the two */
struct task *collision_next_pickup(struct task *task);
struct collision_hit *collision_next_pickup_hit(struct task *task);
/* the same five scans, of shape index's touches alone. the index doesn't start
 * a scan over. a scan for another shape of the same task goes on after the hit
 * the last one found. nothing in the game calls any of the five but the
 * players' */
struct task *collision_next_player_by_shape(struct task *task, int index);
struct collision_hit *collision_next_player_hit_by_shape(struct task *task,
                                                         int index);
struct task *collision_next_attack_by_shape(struct task *task, int index);
struct collision_hit *collision_next_attack_hit_by_shape(struct task *task,
                                                         int index);
struct task *collision_next_item_or_enemy_by_shape(struct task *task,
                                                   int index);
struct collision_hit *
collision_next_item_or_enemy_hit_by_shape(struct task *task, int index);
struct task *collision_next_object_by_shape(struct task *task, int index);
struct collision_hit *collision_next_object_hit_by_shape(struct task *task,
                                                         int index);
struct task *collision_next_pickup_by_shape(struct task *task, int index);
struct collision_hit *collision_next_pickup_hit_by_shape(struct task *task,
                                                         int index);
/* shape index's touch of a shape whose id is id. a hit task without a work or a
 * record has no shape to match */
struct task *collision_next_with_id_by_shape(struct task *task, int index,
                                             Sint8 id);
struct collision_hit *collision_next_hit_with_id_by_shape(struct task *task,
                                                          int index, Sint8 id);
/* a task the record stands on: a hit with HIT_STANDS_ON, found only while the
 * record has COLLISION_ON_TOP. unused changes nothing */
struct task *collision_next_below(struct task *task, int unused);
struct collision_hit *collision_next_below_hit(struct task *task, int unused);
/* a task that stands on the record: a hit with HIT_STOOD_ON, found only while
 * the record has COLLISION_UNDER. unused changes nothing */
struct task *collision_next_above(struct task *task, int unused);
struct collision_hit *collision_next_above_hit(struct task *task, int unused);

/* the first hit of task's record whose touched shape's id is id. it finds one
 * only while the record has COLLISION_TOUCHED. it returns the hit's task, or
 * null, and the _hit form returns the hit. a hit task without a work or a
 * record has no shape to match. every call starts from the first hit */
struct task *collision_first_with_id(struct task *task, Uint8 id);
struct collision_hit *collision_first_with_id_hit(struct task *task, Uint8 id);
/* the id of a shape whose touch breaks what looks for the id with
 * collision_first_with_id: a cage, Meteor Herd's container and gate, and Mad
 * Space's shutter */
#define SHAPE_ID_BREAKER 17

/* the nearest task the call accepts among the hits of task's record. it returns
 * null where it accepts none. each call accepts only a task nearer than 1000000
 * by njDistanceP2P. it stops at the first hit without a task. collision_nearest
 * accepts any task. the next five accept a task whose record is of the kinds
 * their names give. the _with_id calls accept a task whose touched shape's id,
 * as a Uint8, is id. the _within calls accept only a task whose bearing from
 * task about y lies within range of ang.
 *
 * task, its work and its record must not be null, unchecked. nor may a hit
 * task's work, nor its record where the call reads its kind or its shapes.
 * nothing in the game calls any of them but collision_nearest_unk_9_within and
 * collision_nearest_with_id_within */
struct task *collision_nearest(struct task *task);
struct task *collision_nearest_player(struct task *task);
struct task *collision_nearest_item_or_enemy(struct task *task);
struct task *collision_nearest_object(struct task *task);
struct task *collision_nearest_pickup(struct task *task);
struct task *collision_nearest_unk_9(struct task *task);
struct task *collision_nearest_with_id(struct task *task, Sint16 id);
struct task *collision_nearest_within(struct task *task, Angle ang,
                                      Angle range);
struct task *collision_nearest_player_within(struct task *task, Angle ang,
                                             Angle range);
struct task *collision_nearest_item_or_enemy_within(struct task *task,
                                                    Angle ang, Angle range);
struct task *collision_nearest_object_within(struct task *task, Angle ang,
                                             Angle range);
struct task *collision_nearest_pickup_within(struct task *task, Angle ang,
                                             Angle range);
struct task *collision_nearest_unk_9_within(struct task *task, Angle ang,
                                            Angle range);
struct task *collision_nearest_with_id_within(struct task *task, Angle ang,
                                              Angle range, Sint16 id);

#endif
