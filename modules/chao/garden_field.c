#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "chao/garden.h"
#include "chao/garden_field.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "collision/collision.h"
#include "draw/lines.h"
#include "object/set.h"
#include "system/task.h"
#include "system/util.h"

enum race_field_kind {
    RACE_FIELD_TREE = 178,
    RACE_FIELD_GLIDE,
    RACE_FIELD_JUMP,
    RACE_FIELD_LAKE,
    RACE_FIELD_FETCH,
    RACE_FIELD_FEAR,
    RACE_FIELD_GOAL,
    RACE_FIELD_ENABLE_TURN,
    RACE_FIELD_ALONG_LANE,
    RACE_FIELD_REMEMBER,
    RACE_FIELD_THINK,
    RACE_FIELD_PARALLEL,
    RACE_FIELD_CELEBRATE,
    RACE_FIELD_PATH,
    RACE_FIELD_POLYGON,
    RACE_FIELD_INDIVIDUAL
};

#define BOX_MARGIN 10.0F
#define FIELD_PUSH PUSH_FIRM(PUSH_ALL_BUT_PICKUPS)
/* a placed box's modes. it fits its box, then registers it each step. a
 * trigger stops at FIELD_DONE once a player has touched it */
enum field_mode { FIELD_START, FIELD_ACTIVE, FIELD_DONE };

/* clang-format off */
static struct collision_shape field_box = {
    0, SHAPE_BOX, FIELD_PUSH, SHAPE_GUARD_LEVEL(3), 0, {0, 0, 0}, 40.0F, 30.0F,
    20.0F, 0, {0, 0, 0}};
/* clang-format on */

/* the SET editor's name for each of race_field_kinds */
static const char *race_field_names[] = {
    "TREE",        "GLIDE",        "JUMP",      "LAKE",
    "KARIMONO",    "KOWAI",        "GOAL",      "ENABLE_TURN",
    "ALONG_LANE",  "REMEMBER_POS", "THINK_POS", "PARALLEL",
    "GOAL_BANZAI", "PATHCOLLI",    "POLYCOLLI", "EACHCOLLI"};
static Uint8 race_field_kinds[] = {
    RACE_FIELD_TREE,      RACE_FIELD_GLIDE,       RACE_FIELD_JUMP,
    RACE_FIELD_LAKE,      RACE_FIELD_FETCH,       RACE_FIELD_FEAR,
    RACE_FIELD_GOAL,      RACE_FIELD_ENABLE_TURN, RACE_FIELD_ALONG_LANE,
    RACE_FIELD_REMEMBER,  RACE_FIELD_THINK,       RACE_FIELD_PARALLEL,
    RACE_FIELD_CELEBRATE, RACE_FIELD_PATH,        RACE_FIELD_POLYGON,
    RACE_FIELD_INDIVIDUAL};
static int race_field_count =
    sizeof(race_field_kinds) / sizeof(race_field_kinds[0]);

/* clang-format off */
static struct collision_shape wall_box = {
    0, SHAPE_BOX2, FIELD_PUSH | PUSH_PLAYERS, SHAPE_GUARD_LEVEL(3),
    SHAPE_TURN_YZX, {0, 0, 0}, 40.0F, 30.0F, 20.0F, 0, {0, 0, 0}};
static struct collision_shape climb_box = {
    (Sint8)GARDEN_CLIMB_FIELD_ID, SHAPE_BOX, FIELD_PUSH, SHAPE_GUARD_LEVEL(3),
    0, {0, 0, 0}, 40.0F, 30.0F, 20.0F, 0, {0, 0, 0}};
static const struct collision_shape field_sphere = {
    0, SHAPE_SPHERE, FIELD_PUSH, SHAPE_GUARD_LEVEL(3),
    SHAPE_IGNORE_PLAYERS | SHAPE_IGNORE_ENEMIES | SHAPE_UNTURNED,
    {0, 0, 0}, 10.0F, 0, 0, 0, {0, 0, 0}};
/* clang-format on */

static void field_step(struct task *field)
{
    struct task_work *work = field->work;

    if (work->submode) {
        if (work->timer == 0) {
            task_destroy(field);
            return;
        }
        work->timer--;
    }
    collision_register(field);
}

static void field_draw(struct task *field)
{
    struct task_work *work = field->work;
    Float radius = work->scl.x;

    if (chao_tuning.show_fields) {
        draw_control_save();
        draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
        draw_control_clear(NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL);
        constant_material_set(0.25F, 0.0F, 0.0F, 1.0F);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njScale(NULL, radius, radius, radius);
        njCnkSimpleDrawObject(&debug_sphere_object);
        njPopMatrixEx();
        constant_material_set(1.0F, 1.0F, 1.0F, 1.0F);
        draw_control_restore();
    }
}

static void field_destroy(struct task *field)
{
    (void)field;
}

static void child_field_destroy(struct task *field)
{
    struct task *parent;
    if (field->parent) {
        parent = field->parent;
#ifdef NONMATCHING
        /* a field made since may have replaced this one as the Chao's */
        if (((struct chao_work *)parent->work)->field != field) {
            return;
        }
#endif
        ((struct chao_work *)parent->work)->field = NULL;
    }
}

#ifdef NONMATCHING
/* this side makes a field's sphere and sets its draw and destructor. it returns
 * FALSE where the task or the sphere's record is missing */
static Bool init_field_sphere(struct task *field, Uint8 list,
                              void (*destroy)(struct task *))
{
    if (!field) {
        return FALSE;
    }
    if (!collision_init(field, &field_sphere, 1, list)) {
        task_destroy(field);
        return FALSE;
    }
    field->draw = field_draw;
    field->destructor = destroy;
    return TRUE;
}
#endif

void garden_field_set_draw_radius(struct task *field, Float radius)
{
    struct task_work *work = field->work;
    work->scl.x = radius;
}

void garden_field_set_lifetime(struct task *field, int frames)
{
    struct task_work *work = field->work;

    if (frames > 0) {
        work->submode = 1;
        work->timer = frames;
    } else {
        work->submode = 0;
    }
}

void garden_field_set_kind(struct task *field, int kind)
{
    struct collision_shape *shape = collision_get_shape(field, 0);
    shape->id = kind;
}

struct task *garden_object_field(int kind, NJS_POINT3 *position, Float radius)
{
    return garden_timed_object_field(kind, position, radius, 0);
}

struct task *garden_timed_object_field(int kind, NJS_POINT3 *position,
                                       Float radius, int frames)
{
    return garden_oriented_object_field(kind, position, radius, 0, frames);
}

struct task *garden_oriented_object_field(int kind, NJS_POINT3 *position,
                                          Float radius, Angle pitch, int frames)
{
    struct task *field =
        task_create(TASK_WORK, 2, field_step, "AL_FieldExecutor");
    struct collision_shape *shape;
    struct task_work *work;

#ifdef NONMATCHING
    /* this side returns null where the field or its sphere can't be made. the
     * matching side writes through the null */
    if (!init_field_sphere(field, COLLISION_OBJECT, field_destroy)) {
        return NULL;
    }
#else
    field->draw = field_draw;
    field->destructor = field_destroy;
    collision_init(field, &field_sphere, 1, COLLISION_OBJECT);
#endif
    shape = collision_get_shape(field, 0);
    shape->id = kind;
    shape->radius = radius;
    work = field->work;
    work->pos = *position;
    work->scl.x = radius;
    garden_field_set_lifetime(field, frames);
    work->ang.x = pitch;
    return field;
}

struct task *garden_enemy_field(int kind, NJS_POINT3 *position, Float radius)
{
    return garden_timed_enemy_field(kind, position, radius, 0);
}

struct task *garden_timed_enemy_field(int kind, NJS_POINT3 *position,
                                      Float radius, int frames)
{
    return garden_oriented_enemy_field(kind, position, radius, 0, frames);
}

struct task *garden_oriented_enemy_field(int kind, NJS_POINT3 *position,
                                         Float radius, Angle pitch, int frames)
{
    struct task *field =
        task_create(TASK_WORK, 2, field_step, "AL_FieldExecutor");
    struct collision_shape *shape;
    struct task_work *work;

#ifdef NONMATCHING
    /* this side returns null where the field or its sphere can't be made. the
     * matching side writes through the null */
    if (!init_field_sphere(field, COLLISION_ENEMY, field_destroy)) {
        return NULL;
    }
#else
    field->draw = field_draw;
    field->destructor = field_destroy;
    collision_init(field, &field_sphere, 1, COLLISION_ENEMY);
#endif
    shape = collision_get_shape(field, 0);
    shape->id = kind;
    shape->radius = radius;
    work = field->work;
    work->pos = *position;
    work->scl.x = radius;
    garden_field_set_lifetime(field, frames);
    work->ang.x = pitch;
    return field;
}

struct task *garden_create_child_field(struct task *chao, int kind,
                                       NJS_POINT3 *position, Float radius)
{
    return garden_create_timed_field(chao, kind, position, radius, 0);
}

struct task *garden_create_timed_field(struct task *chao, int kind,
                                       NJS_POINT3 *position, Float radius,
                                       int frames)
{
    struct task *field = task_create_child(TASK_WORK, field_step, chao);
    struct collision_shape *shape;
    struct task_work *work;

#ifdef NONMATCHING
    /* this side returns null where the field or its sphere can't be made. the
     * matching side writes through the null */
    if (!init_field_sphere(field, COLLISION_ENEMY, child_field_destroy)) {
        return NULL;
    }
#else
    field->draw = field_draw;
    field->destructor = child_field_destroy;
    collision_init(field, &field_sphere, 1, COLLISION_ENEMY);
#endif
    shape = collision_get_shape(field, 0);
    shape->id = kind;
    shape->radius = radius;
    work = field->work;
    work->pos = *position;
    work->scl.x = radius;
    garden_field_set_lifetime(field, frames);
    ((struct chao_work *)chao->work)->field = field;
    return field;
}

static void fit_field_box(struct task_work *work)
{
    struct collision_shape *shape = work->collision->shapes;

    shape->radius = work->scl.x + BOX_MARGIN;
    shape->size_y = work->scl.y + BOX_MARGIN;
    shape->size_z = work->scl.z + BOX_MARGIN;
    collision_fit_record_radius(work->collision);
}

void garden_race_field(struct task *task)
{
    struct task_work *work = task->work;
    struct collision_shape *shape;

    /* @hack redundant: a block that never runs, kept for the SET editor's
     * captions, which SHC places among this source's strings */
    if (0) {
        (void)"< CUBE >";
        (void)"<      >";
        (void)"-> COLLI KIND  ";
        (void)"-> ROT Y       ";
        (void)"->             ";
        (void)"-> YOKO        ";
        (void)"-> TATE        ";
    }
    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case FIELD_START:
#ifdef NONMATCHING
        /* this side tries again the next step where collision_init fails, whose
         * shape the lines below write */
        if (!collision_init(task, &field_box, 1, COLLISION_ENEMY)) {
            return;
        }
#else
        collision_init(task, &field_box, 1, COLLISION_ENEMY);
#endif
        fit_field_box(work);
        shape = collision_get_shape(task, 0);
        shape->id = race_field_kinds[(Uint16)work->ang.x % race_field_count];
        work->mode = FIELD_ACTIVE;
        break;
    case FIELD_ACTIVE:
        collision_register(task);
        break;
    default:
        work->mode = FIELD_START;
        break;
    }
}

static void fit_turned_field_box(struct task_work *work)
{
    struct collision_shape *shape = work->collision->shapes;

    shape->radius = work->scl.x + BOX_MARGIN;
    shape->size_y = work->scl.y + BOX_MARGIN;
    shape->size_z = work->scl.z + BOX_MARGIN;
    shape->ang.x = work->ang.x;
    shape->ang.y = work->ang.y;
    shape->ang.z = work->ang.z;
    collision_fit_record_radius(work->collision);
}

void garden_cube_field(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block that never runs, kept for its caption */
    if (0) {
        (void)"< WALL >";
    }
    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case FIELD_START:
#ifdef NONMATCHING
        /* this side tries again the next step where collision_init fails, whose
         * shape the lines below fit */
        if (!collision_init(task, &wall_box, 1, COLLISION_OBJECT)) {
            return;
        }
#else
        collision_init(task, &wall_box, 1, COLLISION_OBJECT);
#endif
        fit_turned_field_box(work);
        work->mode = FIELD_ACTIVE;
        break;
    case FIELD_ACTIVE:
        collision_register(task);
        break;
    default:
        work->mode = FIELD_START;
        break;
    }
}

void garden_climb_field(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block that never runs, kept for its captions */
    if (0) {
        (void)"< CLIMB >";
        (void)"->  ";
    }
    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case FIELD_START:
#ifdef NONMATCHING
        /* this side tries again the next step where collision_init fails, whose
         * shape the lines below fit */
        if (!collision_init(task, &climb_box, 1, COLLISION_OBJECT)) {
            return;
        }
#else
        collision_init(task, &climb_box, 1, COLLISION_OBJECT);
#endif
        fit_field_box(work);
        work->mode = FIELD_ACTIVE;
        break;
    case FIELD_ACTIVE:
        collision_register(task);
        break;
    default:
        work->mode = FIELD_START;
        break;
    }
}

void garden_lobby_trigger(struct task *task)
{
    struct task_work *work = task->work;
    struct collision_hit *hit;
    Bool untouched;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case FIELD_START:
#ifdef NONMATCHING
        /* this side tries again the next step where collision_init fails, whose
         * shape the lines below fit */
        if (!collision_init(task, &field_box, 1, COLLISION_OBJECT)) {
            return;
        }
#else
        collision_init(task, &field_box, 1, COLLISION_OBJECT);
#endif
        fit_field_box(work);
        work->mode = FIELD_ACTIVE;
        break;
    case FIELD_ACTIVE:
        untouched = FALSE;
        for (;;) {
            hit = collision_next_player_hit_by_shape(task, 0);
            if (hit) {
                if (hit->hit_shape == 0) {
                    chao_change_area(CHAO_AREA_LOBBY);
                    work->mode = FIELD_DONE;
                    break;
                }
            } else {
                untouched = TRUE;
                break;
            }
        }
        if (untouched) {
            collision_register(task);
        }
        break;
    case FIELD_DONE:
        break;
    default:
        work->mode = FIELD_START;
        break;
    }
}

void garden_entrance_trigger(struct task *task)
{
    struct task_work *work = task->work;
    struct collision_hit *hit;
    Bool untouched;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case FIELD_START:
#ifdef NONMATCHING
        /* this side tries again the next step where collision_init fails, whose
         * shape the lines below fit */
        if (!collision_init(task, &field_box, 1, COLLISION_OBJECT)) {
            return;
        }
#else
        collision_init(task, &field_box, 1, COLLISION_OBJECT);
#endif
        fit_field_box(work);
        work->mode = FIELD_ACTIVE;
        break;
    case FIELD_ACTIVE:
        untouched = FALSE;
        for (;;) {
            hit = collision_next_player_hit_by_shape(task, 0);
            if (hit) {
                if (hit->hit_shape == 0) {
                    chao_change_area(CHAO_AREA_ENTRANCE);
                    work->mode = FIELD_DONE;
                    break;
                }
            } else {
                untouched = TRUE;
                break;
            }
        }
        if (untouched) {
            collision_register(task);
        }
        break;
    case FIELD_DONE:
        break;
    default:
        work->mode = FIELD_START;
        break;
    }
}
