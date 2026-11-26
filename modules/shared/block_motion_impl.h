/* the shapes, init, destructor and step of BLOCK and BLOCK2. core_block.c and
 * core_lift.c include them, and core_lift.c defines VERTICAL_BLOCK */
#ifndef SHARED_BLOCK_MOTION_IMPL_H
#define SHARED_BLOCK_MOTION_IMPL_H

/* a box of center x, y and z, and of half sizes sx, sy and sz */
#define BLOCK_BOX(levels, flags, x, y, z, sx, sy, sz)                          \
    {0,  SHAPE_BOX, BLOCK_PUSH, (levels), (flags),  {x, y, z},                 \
     sx, sy,        sz,         0,        {0, 0, 0}}
#define SOLID_BOX(x, y, z, sx, sy, sz)                                         \
    BLOCK_BOX(BLOCK_GUARD, SHAPE_UNK_2000, x, y, z, sx, sy, sz)
#define HURTING_BOX(x, y, z, sx, sy, sz)                                       \
    BLOCK_BOX(SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(2),                     \
              SHAPE_HURTABLE | SHAPE_UNK_2000, x, y, z, sx, sy, sz)

/* the block's box, and each portal's six, before the scale. a portal has a
 * solid box for its body, and a box inside it that hurts players. its other
 * four are the sides of its mouth */
static struct collision_shape block_shape[] = {
    SOLID_BOX(0, 0, 0, 14.9F, 14.9F, 14.9F)};
static struct collision_shape portal_shapes[PORTAL_SHAPES] = {
#ifdef VERTICAL_BLOCK
    SOLID_BOX(0, -26.0F, 0, 17.0F, 25.0F, 17.0F),
    HURTING_BOX(0, -25.0F, 0, 15.0F, 25.1F, 15.0F),
    SOLID_BOX(0, -2.5F, 17.5F, 20.0F, 7.5F, 2.5F),
    SOLID_BOX(0, -2.5F, -17.5F, 20.0F, 7.5F, 2.5F),
    SOLID_BOX(17.5F, -2.5F, 0, 2.5F, 7.5F, 20.0F),
    SOLID_BOX(-17.5F, -2.5F, 0, 2.5F, 7.5F, 20.0F)
#else
    SOLID_BOX(0, 0, 26.0F, 17.0F, 17.0F, 25.0F),
    HURTING_BOX(0, 0, 25.0F, 15.0F, 15.0F, 25.1F),
    SOLID_BOX(0, 17.5F, 2.5F, 20.0F, 2.5F, 7.5F),
    SOLID_BOX(0, -17.5F, 2.5F, 20.0F, 2.5F, 7.5F),
    SOLID_BOX(17.5F, 0, 2.5F, 2.5F, 20.0F, 7.5F),
    SOLID_BOX(-17.5F, 0, 2.5F, 2.5F, 20.0F, 7.5F)
#endif
};
static Sint32 sound_volume = 75;
static Float sound_range = 200.0F;

/* half the block's edge, before the scale */
#define BLOCK_HALF_SIZE 15.0F
/* where each of the block's two flashes begins, as a share of its period */
#define EARLY_FLASH_START 0.0F
#define LATE_FLASH_START 0.4F

/* the square of the camera zone's radius, before the scale. the zone's sphere
 * holds the block's box */
#define ZONE_RADIUS_SQUARED 675.0F
/* the block's object is in the world's collision only while a player lies
 * within NEAR_RANGE of it */
#define NEAR_RANGE 57.5F
/* the block sounds while player 0 lies within sound_range of it */
#define BLOCK_SOUND 1
#define BLOCK_SOUND_DURATION 15

/* give child the portal's shapes at the block's scale. their centers' y take
 * y_scale instead, which turns BLOCK2's upper portal's boxes upside down.
 * index is the caller's counter for the loop over the shapes */
#define FIT_PORTAL(child, task, index, y_scale)                                \
    do {                                                                       \
        collision_init((child), &portal_shapes[0], PORTAL_SHAPES,              \
                       COLLISION_OBJECT);                                      \
        for ((index) = 0; (index) < PORTAL_SHAPES; (index)++) {                \
            (child)->work->collision->shapes[index].center.x *=                \
                BLOCK_SCALE(task);                                             \
            (child)->work->collision->shapes[index].center.y *= (y_scale);     \
            (child)->work->collision->shapes[index].center.z *=                \
                BLOCK_SCALE(task);                                             \
            (child)->work->collision->shapes[index].radius *=                  \
                BLOCK_SCALE(task);                                             \
            (child)->work->collision->shapes[index].size_y *=                  \
                BLOCK_SCALE(task);                                             \
            (child)->work->collision->shapes[index].size_z *=                  \
                BLOCK_SCALE(task);                                             \
        }                                                                      \
        collision_fit_radius(child);                                           \
    } while (0)

#ifdef VERTICAL_BLOCK
void core_lift_init(struct task *task)
#else
void core_block_init(struct task *task)
#endif
{
    struct task_work *work;
    struct task *child;
    NJS_OBJECT *node;
    int i;
#ifndef VERTICAL_BLOCK
    Float x;
    Float z;
#endif
    struct zone_shape zone;
    Float scale;
#ifndef VERTICAL_BLOCK
    Float near_x;
    Float near_z;
    Float far_x;
    Float far_z;
#endif
    work = task->work;
    ANCHOR(task)->origin.x = work->pos.x;
    ANCHOR(task)->origin.y = work->pos.y;
    ANCHOR(task)->origin.z = work->pos.z;
#ifdef VERTICAL_BLOCK
    work->pos.y += core_block_offset(task);
#else
    polar_xz(HEADING(task), core_block_offset(task), &x, &z);
    work->pos.x += x;
    work->pos.z += z;
#endif
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = core_block_draw;
    task->step = core_block_step;
    task->destructor = core_block_destroy;
#ifdef VERTICAL_BLOCK
    /* @hack redundant: a block no path reaches, of four clears of ang.x */
    if (0) {
        task->work->ang.x = 0;
        task->work->ang.x = 0;
        task->work->ang.x = 0;
        task->work->ang.x = 0;
    }
#else
    /* @hack redundant: a block no path reaches, of two clears of task's
     * motion */
    if (0) {
        task->motion.p = NULL;
        task->motion.p = NULL;
    }
#endif
    node = world_pool_alloc();
    /* @hack redundant: an empty block no path reaches */
    if (0) {
    }
    *node = *core_block_collision;
    task->motion.p = node;
    node->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    node->scl[0] = node->scl[1] = node->scl[2] = BLOCK_SCALE(task);
    node->ang[0] = 0;
    node->ang[1] = HEADING(task);
    node->ang[2] = 0;
    node->pos[0] = work->pos.x;
    node->pos[1] = work->pos.y;
    node->pos[2] = work->pos.z;
    world_add(WORLD_SIZE_CLASS(1) | WORLD_MOVING | WORLD_SOLID, task, node);
    collision_init(task, block_shape, 1, COLLISION_OBJECT);
    work->collision->shapes->radius *= BLOCK_SCALE(task);
    work->collision->shapes->size_y *= BLOCK_SCALE(task);
    work->collision->shapes->size_z *= BLOCK_SCALE(task);
    collision_fit_radius(task);
    if (HAS_CAMERA(task)) {
        zone.shape = ZONE_SPHERE;
        zone.pos = task->work->pos;
        zone.ang.x = 0;
        zone.ang.y = 0;
        zone.ang.z = 0;
        scale = BLOCK_SCALE(task);
        zone.scl.x = njSqrt(scale * scale * ZONE_RADIUS_SQUARED);
#ifdef NONMATCHING
        /* GCC's build sets scl.y, which SHC's leaves unset. a sphere's
         * tests read only scl.x and scl.z */
        zone.scl.y = 0.0F;
#endif
        zone.scl.z = zone.scl.x * zone.scl.x;
        ANCHOR(task)->zone = camset_add_marked(&zone);
    }
    if (!HIDE_NEAR_PORTAL(task)) {
        child = task_create_child(TASK_WORK, core_block_contact, task);
        child->destructor = core_block_contact_destroy;
#ifdef VERTICAL_BLOCK
        child->work->pos.x = ANCHOR(task)->origin.x;
        child->work->pos.y =
            ANCHOR(task)->origin.y - (task->work->scl.z * 0.5F);
        child->work->pos.z = ANCHOR(task)->origin.z;
#else
        polar_xz(HEADING(task), task->work->scl.z * 0.5F, &near_x, &near_z);
        child->work->pos.x = ANCHOR(task)->origin.x + near_x;
        child->work->pos.y = ANCHOR(task)->origin.y;
        child->work->pos.z = ANCHOR(task)->origin.z + near_z;
#endif
        child->work->ang.y = HEADING(task);
        child->work->ang.x = child->work->ang.z = 0;
        FIT_PORTAL(child, task, i, BLOCK_SCALE(task));
#ifndef VERTICAL_BLOCK
        /* @hack redundant: a call of collision_init that no path reaches */
        if (0) {
            collision_init(child, portal_shapes, PORTAL_SHAPES,
                           COLLISION_OBJECT);
        }
#endif
    }
    if (!HIDE_NEAR_PORTAL(task)) {
        child = task_create_child(TASK_WORK, core_block_contact, task);
        child->destructor = core_block_contact_destroy;
#ifdef VERTICAL_BLOCK
        child->work->pos.x = ANCHOR(task)->origin.x;
        child->work->pos.y =
            ANCHOR(task)->origin.y + (task->work->scl.z * 0.5F);
        child->work->pos.z = ANCHOR(task)->origin.z;
#else
        polar_xz(HEADING(task), -(task->work->scl.z * 0.5F), &far_x, &far_z);
        child->work->pos.x = ANCHOR(task)->origin.x + far_x;
        child->work->pos.y = ANCHOR(task)->origin.y;
        child->work->pos.z = ANCHOR(task)->origin.z + far_z;
#endif
        child->work->ang.y = HEADING(task) ^ HALF_TURN;
        child->work->ang.x = child->work->ang.z = 0;
#ifdef VERTICAL_BLOCK
        FIT_PORTAL(child, task, i, -task->work->scl.x - 1.0F);
#else
        FIT_PORTAL(child, task, i, BLOCK_SCALE(task));
#endif
    }
#ifndef VERTICAL_BLOCK
    FLOOR_Y(task) = NO_FLOOR;
#endif
}

static void core_block_destroy(struct task *task)
{
    if (ANCHOR(task)->zone) {
        camset_free_zone(ANCHOR(task)->zone);
    }
    if (task->motion.p) {
        world_remove(task, task->motion.p);
        world_pool_free(task->motion.p);
    }
    task->motion.p = NULL;
}

static void core_block_step(struct task *task)
{
    struct task_work *work;
    NJS_OBJECT *node;
#ifndef VERTICAL_BLOCK
    Float x;
    Float z;
#endif
    NJS_POINT3 pos;
    int phase;
    work = task->work;
    if (placement_try_leave(task)) {
        return;
    }
#ifndef VERTICAL_BLOCK
    /* @hack redundant: a test of the heading that no path reaches */
    if (0) {
        if (HEADING(task)) {
        }
    }
#endif
    node = task->motion.p;
    /* @hack redundant: a test of bit 0 of ang.z that no path reaches */
    if (0) {
        if (task->work->ang.z & 1) {
        }
    }
    /* @hack redundant: the same test again, in a block of its own */
    if (0) {
        if (task->work->ang.z & 1) {
        }
    }
#ifdef VERTICAL_BLOCK
    pos.x = ANCHOR(task)->origin.x;
    pos.y = ANCHOR(task)->origin.y + core_block_offset(task);
    pos.z = ANCHOR(task)->origin.z;
#else
    polar_xz(HEADING(task), core_block_offset(task), &x, &z);
    pos.x = ANCHOR(task)->origin.x + x;
    pos.y = ANCHOR(task)->origin.y;
    pos.z = ANCHOR(task)->origin.z + z;
#endif
    phase = core_block_phase(task);
    if (phase > 0 && phase < MOVING_STEPS(task)) {
        RIDERS(task)[0].speed.x = RIDERS(task)[1].speed.x = pos.x - work->pos.x;
        RIDERS(task)[0].speed.y = RIDERS(task)[1].speed.y = pos.y - work->pos.y;
        RIDERS(task)[0].speed.z = RIDERS(task)[1].speed.z = pos.z - work->pos.z;
    } else {
        RIDERS(task)
        [0].speed.x = RIDERS(task)[1].speed.x = RIDERS(task)[0].speed.y =
            RIDERS(task)[1].speed.y = RIDERS(task)[0].speed.z =
                RIDERS(task)[1].speed.z = 0.0F;
    }
    work->pos = pos;
    if (HAS_CAMERA(task)) {
        if (ANCHOR(task)->zone) {
            ANCHOR(task)->zone->pos = work->pos;
        }
    }
    node->pos[0] = work->pos.x;
    node->pos[1] = work->pos.y;
    node->pos[2] = work->pos.z;
    if (player_near((NJS_POINT3 *)node->pos, NEAR_RANGE)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    collision_register(task);
    if (njDistanceP2P(&player_work[0]->pos, &work->pos) < sound_range) {
        sound_play_task_at_for(BLOCK_SOUND, task, 0, sound_volume,
                               BLOCK_SOUND_DURATION);
    }
}

#endif
