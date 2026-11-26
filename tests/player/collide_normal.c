/* Run player_collide against a box that pushed the player at a side, after
 * each call that leaves its normal a y: player_apply_accel an address, whose
 * sign alone tells, and player_apply_push the speed ahead. */
#include <assert.h>
#include <math.h>

#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "player/player.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* player.c's */
static Float collide_y;
Float player_slide_y;

static struct task pusher;
static int asked;
static int slides;

static struct task *player_last_side_pusher(struct task_work *work,
                                            struct player *record)
{
    return &pusher;
}
/* collision.c's: the record's normal's x and z alone */
Bool collision_get_normal(struct task_work *work, NJS_VECTOR *normal)
{
    normal->x = work->collision->normal.x;
    normal->z = work->collision->normal.z;
    return TRUE;
}
int player_action_flags(struct task_work *work, struct player *record)
{
    asked++;
    return ACTION_FLAG_400;
}
static void player_slide_along_wall(struct task_work *work,
                                    struct player_motion *motion,
                                    struct player *record)
{
    slides++;
}
static int player_collide_ground(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record)
{
    return 0;
}
Bool player_read_stick(struct task_work *work, struct player *record,
                       Angle *angle, Float *strength)
{
    return FALSE;
}
static void ground_turn_to_world(NJS_VECTOR *normal, NJS_VECTOR *vector,
                                 Angle heading)
{
}
/* a player that faces along x */
void turn_to_world(struct task_work *work, NJS_VECTOR *vector)
{
}
void turn_from_world(struct task_work *work, NJS_VECTOR *vector)
{
}
static void player_limit_speed(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record)
{
}
static void player_stop_creep(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record)
{
}
void player_stop(struct player_motion *motion, struct player *record)
{
}
int stage_current_id(void)
{
    return STAGE_CITY_ESCAPE;
}
Float njScalor(NJS_VECTOR *v)
{
    return sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}
Float njUnitVector(NJS_VECTOR *v)
{
    Float length = njScalor(v);

    v->x /= length;
    v->y /= length;
    v->z /= length;
    return length;
}
Float njInnerProduct(NJS_VECTOR *v1, NJS_VECTOR *v2)
{
    return (v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z);
}
/* the box takes neither the turn round a sphere nor the drop off a shape */
Float njSqrt(Float n)
{
    assert(0);
    return 0.0F;
}
Angle angle_difference(Angle from, Angle to)
{
    assert(0);
    return 0;
}
Angle angle_distance(Angle from, Angle to)
{
    assert(0);
    return 0;
}
void matrix_push_unit(void)
{
    assert(0);
}
Bool njPopMatrixEx(void)
{
    assert(0);
    return FALSE;
}
void njRotateX(NJS_MATRIX *m, Angle ang)
{
    assert(0);
}
void njRotateY(NJS_MATRIX *m, Angle ang)
{
    assert(0);
}
void njRotateZ(NJS_MATRIX *m, Angle ang)
{
    assert(0);
}
void njCalcVector(NJS_MATRIX *m, NJS_VECTOR *vs, NJS_VECTOR *vd)
{
    assert(0);
}

#include "extracted.inc"

int main(void)
{
    static struct player record;
    static struct collision_shape own;
    static struct collision_shape box;
    static struct collision_work collision;
    static struct collision_work touched;
    static struct task_work pusher_work;
    static struct task_work work;
    static struct player_motion motion;

    box.form = SHAPE_BOX;
    touched.shapes = &box;
    touched.owner = &pusher;
    pusher_work.collision = &touched;
    pusher.work = &pusher_work;
    collision.shapes = &own;
    collision.hit = &touched;
    collision.normal.x = -1.0F;
    work.collision = &collision;
    record.consts.push_speed = 1.0F;

    /* a player that falls straight down the box's side has no motion along
     * the normal's x and z. after player_apply_accel the normal's y is an
     * address's: next to 0, and below it, so the facing is above 0 */
    player_apply_accel(&work, &motion, &record);
    motion.spd = (NJS_VECTOR){0.0F, -1.0F, 0.0F};
    assert(player_collide(&work, &motion, &record) == PLAYER_BUMPED);
    assert(asked == 1 && slides == 0);
    assert(record.wall_normal.x == -1.0F && record.wall_normal.z == 0.0F);
    assert(record.wall_normal.y < 0.0F && record.wall_normal.y > -0x1p-100F);

    /* what the call left reaches that player_collide alone: with no call
     * before it, the next one's normal lies level, and faces neither way */
    assert(player_collide(&work, &motion, &record) == PLAYER_BUMPED);
    assert(asked == 1 && slides == 0);
    assert(record.wall_normal.y == 0.0F && !signbit(record.wall_normal.y));

    /* after a push the y is the speed ahead: the normal leans, the player
     * runs into it and slides along the box */
    record.spd.x = 0.5F;
    player_apply_push(&work, &motion, &record);
    assert(asked == 2);
    assert(player_collide(&work, &motion, &record) == PLAYER_BUMPED);
    assert(asked == 2 && slides == 1);
    assert(record.wall_normal.x == -1.0F / sqrtf(1.25F) &&
           record.wall_normal.y == 0.5F / sqrtf(1.25F));

    /* a push that has stopped leaves a y of 0, not an address's */
    record.spd.x = 0.0F;
    player_apply_push(&work, &motion, &record);
    motion.spd = (NJS_VECTOR){0.0F, -1.0F, 0.0F};
    assert(player_collide(&work, &motion, &record) == PLAYER_BUMPED);
    assert(asked == 3 && slides == 1);
    assert(record.wall_normal.y == 0.0F && !signbit(record.wall_normal.y));
    return 0;
}
