/* Slide a grounded player along a wall it was pushed against, whose side
 * normal gives x and z alone. The slide's normal takes its y from
 * player_slide_y: a mech's strike test and its lock-on's measures leave one
 * there, and without them the normal lies level. */
#include <assert.h>
#include <math.h>
#include <string.h>

#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* player.c's */
Float player_slide_y;
struct player *player_records[PLAYER_SLOTS];
struct task *collision_items[COLLISION_ITEMS];
struct task *collision_enemies[COLLISION_ENEMIES];

static int rival = -1;
static int items;
static NJS_POINT3 center;

/* collision.c's: the record's normal's x and z alone */
Bool collision_get_normal(struct task_work *work, NJS_VECTOR *normal)
{
    normal->x = work->collision->normal.x;
    normal->z = work->collision->normal.z;
    return TRUE;
}
int collision_get_last_count(int list)
{
    return list == COLLISION_COUNT_ITEMS ? items : 0;
}
void collision_get_shape_center(struct task_work *work,
                                const struct collision_shape *shape,
                                NJS_POINT3 *pos)
{
    *pos = center;
}
int player_find_rival(int slot)
{
    return rival;
}
/* a walker that faces along x */
void turn_to_world(struct task_work *work, NJS_VECTOR *vector)
{
}
Float njScalor(NJS_VECTOR *v)
{
    return sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}
Float njScalor2(NJS_VECTOR *v)
{
    return (v->x * v->x) + (v->y * v->y) + (v->z * v->z);
}
Float njUnitVector(NJS_VECTOR *v)
{
    Float length = njScalor(v);

    v->x /= length;
    v->y /= length;
    v->z /= length;
    return length;
}
void njSubVector(NJS_VECTOR *vd, NJS_VECTOR *vs)
{
    vd->x -= vs->x;
    vd->y -= vs->y;
    vd->z -= vs->z;
}
Float njInnerProduct(NJS_VECTOR *v1, NJS_VECTOR *v2)
{
    return (v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z);
}
/* the line where a wall facing -x meets level ground runs along z */
Float njDistancePL2PL(NJS_PLANE *pl1, NJS_PLANE *pl2, NJS_LINE *l)
{
    l->px = l->py = l->pz = 0.0F;
    l->vx = l->vy = 0.0F;
    l->vz = 1.0F;
    return 0.0F;
}
/* every target lies on the aim's line */
Float njDistanceP2L(NJS_POINT3 *p, NJS_LINE *l, NJS_POINT3 *cp)
{
    return 0.0F;
}
Int njCollisionCheckBC(NJS_BOX *box, NJS_CAPSULE *capsule)
{
    return 1;
}
Angle vector_angle(NJS_VECTOR *first, NJS_VECTOR *second, NJS_VECTOR *axis)
{
    return 0;
}
Angle angle_distance(Angle from, Angle to)
{
    return 0;
}

#include "extracted.inc"
#include "walker.inc"

/* slide a player that a wall facing -x pushed, and return the normal the
 * slide leaves its record */
static NJS_VECTOR slide(void)
{
    struct collision_work collision = {0};
    struct task_work work = {0};
    struct player_motion motion = {0};
    struct player record = {0};

    collision.flags = COLLISION_PUSHED | COLLISION_SIDE;
    collision.normal.x = -1.0F;
    collision.normal.y = 1.0F;
    work.collision = &collision;
    work.flags = PLAYER_WORK_ON_GROUND;
    motion.spd.x = 1.0F;
    motion.spd.z = 1.0F;
    record.ground_normal.y = 1.0F;
    player_slide_along_wall(&work, &motion, &record);
    assert(motion.spd.x == 0.0F && motion.spd.z == sqrtf(2.0F));
    return record.wall_normal;
}

/* the unit normal of a wall facing -x whose y starts at y */
static NJS_VECTOR tilted(Float y)
{
    NJS_VECTOR normal = {-1.0F, y, 0.0F};

    njUnitVector(&normal);
    return normal;
}

static void expect(NJS_VECTOR got, NJS_VECTOR want)
{
    assert(memcmp(&got, &want, sizeof got) == 0);
}

int main(void)
{
    static struct player walker;
    static struct player other;
    struct collision_shape shape = {0};
    struct collision_work collision = {0};
    struct task_work target = {0};
    struct task task = {0};
    struct task_work work = {0};

    /* no step left the slide a y: the normal lies level */
    expect(slide(), (NJS_VECTOR){-1.0F, 0.0F, 0.0F});

    /* a running mech tests its strike against an item that lies 7.5 above
     * the strike's reach, and too far for the strike */
    task.work = &target;
    collision.owner = &task;
    collision.shapes = &shape;
    target.collision = &collision;
    collision_items[0] = &task;
    items = 1;
    center = (NJS_POINT3){100.0F, 7.5F, 0.0F};
    work.mode = PLAYER_MODE_RUN;
    assert(walker_target_near(&work, &walker) == FALSE);
    assert(player_slide_y == 7.5F);
    expect(slide(), tilted(7.5F));

    /* the other player comes first, and with no item its y stays */
    player_slide_y = 0.0F;
    items = 0;
    rival = 1;
    player_records[1] = &other;
    other.unk_218 = (NJS_POINT3){100.0F, -4.0F, 0.0F};
    assert(walker_target_near(&work, &walker) == FALSE);
    assert(player_slide_y == -4.0F);
    expect(slide(), tilted(-4.0F));

    /* a mech that isn't standing, running or sliding tests no strike */
    player_slide_y = 0.0F;
    work.mode = PLAYER_MODE_JUMP;
    assert(walker_target_near(&work, &walker) == FALSE);
    expect(slide(), (NJS_VECTOR){-1.0F, 0.0F, 0.0F});

    /* the lock-on measures a target in reach: the aim's z */
    walker.unk_254 = (NJS_POINT3){0.0F, 20.0F, 0.0F};
    walker.aim = (NJS_VECTOR){1.0F, 0.0F, 3.0F};
    center = (NJS_POINT3){100.0F, 20.0F, 0.0F};
    assert(lockon_target_distance(&collision, &walker) == 10000.0F);
    assert(player_slide_y == 3.0F);
    expect(slide(), tilted(3.0F));

    /* a target out of reach is measured against no line, and leaves none */
    player_slide_y = 0.0F;
    center.x = 1000.0F;
    assert(lockon_target_distance(&collision, &walker) == 0.0F);
    assert(player_slide_y == 0.0F);

    /* a box that stops the lock-on: its center's x */
    center = (NJS_POINT3){50.0F, 20.0F, 0.0F};
    assert(lockon_box_distance(&collision, &walker) == 2500.0F);
    assert(player_slide_y == 50.0F);
    expect(slide(), tilted(50.0F));

    /* the other player in reach, beyond the near range: the y the aim
     * starts at */
    center = (NJS_POINT3){100.0F, 20.0F, 0.0F};
    assert(lockon_player_distance(&collision, &walker) == 10000.0F);
    assert(player_slide_y == 20.0F);
    expect(slide(), tilted(20.0F));
    return 0;
}
