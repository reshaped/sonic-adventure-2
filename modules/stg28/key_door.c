/* the key door, KEYDOOR in Pyramid Cave's object list: a panel in a frame. the
 * panel rises once the keyhole of its color sinks. a player who passes the
 * door shuts the panel for good */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "stg28/cave_util.h"
#include "stg28/key.h"
#include "stg28/key_door.h"
#include "stg28/keyhole.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#include "stg28/key_door/text.inc"
#include "stg28/models/key_door.h"

/* throw dust about task's panel. one rectangle of puffs lies ahead of the
 * panel, height above the work's pos. one stands at each side, from the pos
 * up to height above it. nothing calls cave_key_door_dust, and no header
 * declares it. it is external only because SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void cave_key_door_dust(struct task *task, Float height);
/* NOLINTEND(misc-use-internal-linkage) */
/* SHC expands each call of a static function this small. the image calls
 * cave_key_door_set_next_mode, so it is external. no header declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void cave_key_door_set_next_mode(struct task *task, int mode);
/* NOLINTEND(misc-use-internal-linkage) */

/* a door's modes, in its record's mode */
enum key_door_mode {
    MODE_SHUT, /* it waits for its keyhole to sink */
    MODE_RISE,
    MODE_OPEN,
    MODE_FALL,
    MODE_CLOSED, /* the panel stays shut for good */
    MODES
};
#define NO_MODE (-1)

/* a door's record, at task->extra */
struct key_door {
    Sint32 mode;
    Sint32 next_mode; /* the mode the next step takes, or NO_MODE */
    Sint32 unk_08;
    Sint32 steps; /* the steps the panel has risen or fallen */
    /* the copy of the panel's shape in the collision world */
    NJS_OBJECT *object;
    struct task *trigger; /* the child that holds the box past the door */
};

/* where a door stands, in its motion record */
struct key_door_place {
    NJS_POINT3 frame; /* the frame's pos, where the door was placed */
    NJS_POINT3 shut; /* the panel's pos while shut */
};

#define DOOR(task) ((struct key_door *)(task)->extra.p)
#define PLACE(task) ((struct key_door_place *)(task)->motion.p)
/* how far the panel stands above its shut pos. the task's unk_3c holds the
 * Float */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define HEIGHT(task) (*(Float *)&(task)->unk_3c)

#define DOOR_SHAPES 2
#define TRIGGER_SHAPES 1
/* the panel's index in door_shapes. the shape's half height is
 * PANEL_SHUT_HALF in MODE_SHUT and MODE_CLOSED, and PANEL_OPEN_HALF in the
 * other modes */
#define PANEL_SHAPE 1
#define PANEL_SHUT_HALF 30.0F
#define PANEL_OPEN_HALF 22.5F
#define DOOR_PUSH PUSH_SOLID(PUSH_ALL)

/* the panel rises by RISE_SPEED a step. after RISE_STEPS it stands at
 * OPEN_HEIGHT */
#define RISE_SPEED 0.13333333F
#define RISE_STEPS 375
#define OPEN_HEIGHT 50.0F
/* as the panel falls, its height is OPEN_HEIGHT times the cosine of FALL_TURN
 * times the fall's steps. the cosine reaches 0 at FALL_STEPS */
#define FALL_STEPS 30
#define FALL_TURN ((Float)QUARTER_TURN / FALL_STEPS)
/* the work's scl.y once the rise ends */
#define OPEN_SCL_Y 0.2F

/* the panel's shape in the collision world collides only while a player lies
 * within REACH of its pos */
#define REACH 81.0F
/* the door's sounds, ids in pyramid_cave_sound_list. the panel rumbles while
 * it rises, and slams as it shuts. a play of RUMBLE_SOUND lasts
 * RUMBLE_SOUND_STEPS + 1 updates, as sound_play_at_for says */
#define RUMBLE_SOUND 1
#define RUMBLE_SOUND_PRIORITY 1
#define RUMBLE_SOUND_STEPS 15
#define SLAM_SOUND 2
#define SLAM_SOUND_PRIORITY 0
/* the panel shakes camera 0 as it stops at the top and as it lands from a
 * fall. the shake's size is SHAKE_SIZE over player 0's distance, and no more
 * than SHAKE_SIZE. a shake under way gives way only to one at least as big */
#define SHAKE_STEPS 2
#define SHAKE_SIZE 8.0F

/* the panel's color while it hides the current camera's player */
#define SEE_THROUGH_COLOR 0x65ffffff
#define SEE_THROUGH_CONTROL NJD_CONTROL_3D_CNK_CONSTANT_ATTR
/* a line runs from the current camera to its player. the panel hides the
 * player where the line crosses the door's plane. the plane holds the door's
 * pos, and the x and y axes as the door's ang.y turns them. the crossing must
 * lie within HIDE_HALF_WIDTH of the pos across, and between HIDE_BOTTOM and
 * HIDE_TOP above it */
#define HIDE_HALF_WIDTH 30.0F
#define HIDE_BOTTOM (-10.0F)
#define HIDE_TOP 50.0F

/* the dust's rectangles: ahead of the panel, and at its sides */
#define DUST_AHEAD 5.0F
#define DUST_FRONT_HALF_X 27.0F
#define DUST_FRONT_HALF_Z 7.0F
#define DUST_SIDE_X 5.0F
#define DUST_SIDE_Z (-23.0F)
#define DUST_SIDE_HALF_X 7.0F
#define DUST_SCALE 4.0F
#define DUST_SPEED 0.1F
/* a rectangle's density of puffs, at random from DUST_DENSITY_MIN up to twice
 * it */
#define DUST_DENSITY_MIN 0.5F
#define DUST_DENSITY()                                                         \
    (DUST_DENSITY_MIN + (rand() * (DUST_DENSITY_MIN * RAND_SCALE)))

static void key_door_destroy(struct task *task);
static void key_door_step(struct task *task);
static void key_door_draw(struct task *task);
static struct key_door *key_door_alloc(struct task *task);
static void key_door_free(struct task *task);
static void key_door_take_next_mode(struct task *task);
static void key_door_shut_step(struct task *task);
static void key_door_rise_step(struct task *task);
static void key_door_open_step(struct task *task);
static void key_door_fall_step(struct task *task);
static void key_door_closed_step(struct task *task);
static Bool key_door_hides_player(struct task *task);
static void key_door_shake(struct task *task);
static void key_door_trigger_step(struct task *task);
static void key_door_body_step(struct task *task);

KEPT static NJS_CNK_MODEL *panel_models[KEY_COLORS] = {
#include "stg28/key_door/panel_models.inc"
};
NJS_TEXLIST *cave_key_door_panel_texlists[KEY_COLORS + 1] = {
#include "stg28/key_door/cave_key_door_panel_texlists.inc"
};
KEPT static NJS_CNK_MODEL *frame_models[KEY_COLORS] = {
#include "stg28/key_door/frame_models.inc"
};
NJS_TEXLIST *cave_key_door_frame_texlists[KEY_COLORS + 1] = {
#include "stg28/key_door/cave_key_door_frame_texlists.inc"
};
static char *color_names[KEY_COLORS] = {
#include "stg28/key_door/color_names.inc"
};
/* clang-format off */
/* the sill, a bar along x, and the panel */
static struct collision_shape door_shapes[DOOR_SHAPES] = {
    {0, SHAPE_CYLINDER2, DOOR_PUSH, SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000,
     {0.0F, 2.5F, 2.5F}, 2.5F, 30.0F, 0.0F, 0, {0, 0, QUARTER_TURN}},
    {0, SHAPE_BOX, DOOR_PUSH, SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000,
     {0.0F, 25.0F, 2.5F}, 30.0F, PANEL_OPEN_HALF, 2.5F, 0, {0, 0, 0}},
};
/* the box past the door, whose touch by a player shuts the panel. it pushes no
 * shape, and none of a player's, an enemy's or an object's pushes it */
static struct collision_shape trigger_shapes[TRIGGER_SHAPES] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000,
     {0.0F, 50.0F, -50.0F}, 100.0F, 100.0F, 10.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Sint32 rumble_volume = 100;
static Sint32 slam_volume = 100;
typedef void (*key_door_mode_handler)(struct task *task);

KEPT static key_door_mode_handler mode_steps[MODES] = {
#include "stg28/key_door/mode_steps.inc"
};

void cave_key_door_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *trigger;
    struct task *body;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    /* @bug logic: keeps the children it made where a later allocation
     * fails, and the next step's try makes them again (the branches at
     * 0x8c504c02 and 0x8c504c0e). cave_key_init kills its child on the same
     * failure */
    /* @hack spelling: each child assigned inside its test, where a statement
     * of its own would do */
    if (!(trigger =
              task_create_child(TASK_WORK, key_door_trigger_step, task))) {
        return;
    }
    if (!(body = task_create_child(TASK_WORK, key_door_body_step, task))) {
        return;
    }
    if (!key_door_alloc(task)) {
        return;
    }
    /* @hack spelling: task->work read again, where work would do */
    cave_key_doors[KEY_COLOR(task->work)] = task;
    task->draw = key_door_draw;
    task->step = key_door_step;
    task->destructor = key_door_destroy;
    /* @hack redundant: a block no path reaches, a store to the work's mode */
    if (0) {
        work->mode = 0;
    }
    object = world_pool_alloc();
    DOOR(task)->object = object;
    /* @bug memory: copies the collision object through null where
     * world_pool_alloc has none to lend (MOV.L at 0x8c504c56).
     * key_door_destroy tests the record's object for null (TST at
     * 0x8c504d62) */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the door and its children, and set.c makes it again on a later
     * pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = cave_key_door_collision;
    PLACE(task)->frame = work->pos;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
    object->ang[0] = 0;
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(WORLD_SIZE_CLASS(2) | WORLD_MOVING | WORLD_SOLID, task, object);
    collision_init(body, door_shapes, DOOR_SHAPES, COLLISION_OBJECT);
    collision_init(trigger, trigger_shapes, TRIGGER_SHAPES, COLLISION_OBJECT);
    DOOR(task)->trigger = trigger;
    trigger->work->pos = body->work->pos = task->work->pos;
    trigger->work->ang.x = trigger->work->ang.z = body->work->ang.x =
        body->work->ang.z = 0;
    trigger->work->ang.y = body->work->ang.y = task->work->ang.y;
    HEIGHT(task) = 0.0F;
    DOOR(task)->mode = MODE_SHUT;
    DOOR(task)->next_mode = NO_MODE;
    PLACE(task)->shut = work->pos;
}

static void key_door_destroy(struct task *task)
{
    if (DOOR(task)->object) {
        world_remove(task, DOOR(task)->object);
        world_pool_free(DOOR(task)->object);
    }
    key_door_free(task);
    cave_key_doors[KEY_COLOR(task->work)] = NULL;
}

static void key_door_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    NJS_POINT3 last;

    /* @hack spelling: task->work read again, where work would do */
    cave_key_doors[KEY_COLOR(task->work)] = task;
    key_door_take_next_mode(task);
    mode_steps[DOOR(task)->mode](task);
    object = DOOR(task)->object;
    last = work->pos;
    work->pos.y = PLACE(task)->shut.y + HEIGHT(task);
    TASK_FORCES_OF(task)
    [0].speed.x = TASK_FORCES_OF(task)[1].speed.x = work->pos.x - last.x;
    TASK_FORCES_OF(task)
    [0].speed.y = TASK_FORCES_OF(task)[1].speed.y = work->pos.y - last.y;
    TASK_FORCES_OF(task)
    [0].speed.z = TASK_FORCES_OF(task)[1].speed.z = work->pos.z - last.z;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    if (player_near((NJS_POINT3 *)object->pos, REACH)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- COLOR(      )";
    }
}

static void key_door_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: the five callees called through locals, where calls by
     * their names would do */
    Bool (*pop)(void);
    Sint32 (*set_texture)(NJS_TEXLIST *texlist);
    void (*draw)(NJS_CNK_MODEL *model);
    void (*rotate_y)(NJS_MATRIX *matrix, Angle ang);
    Bool (*push)(void);

    draw_control_set(DRAW_CONTROL_SHADOW);
    rotate_y = njRotateY;
    push = njPushMatrixEx;
    draw = njCnkEasyDrawModel;
    pop = njPopMatrixEx;
    set_texture = njSetTexture;
    /* @hack redundant: a block no path reaches, with tests of work and calls
     * of set_texture and pop */
    if (0) {
        if (work) {
        }
        if (work) {
        }
        set_texture(NULL);
        set_texture(NULL);
        pop();
        pop();
    }
    push();
    njTranslateEx(&PLACE(task)->frame);
    rotate_y(NULL, work->ang.y);
    /* @hack spelling: task->work read again, where work would do */
    set_texture(cave_key_door_frame_texlists[KEY_COLOR(task->work)]);
    draw(frame_models[KEY_COLOR(task->work)]);
    pop();
    push();
    njTranslateEx(&work->pos);
    rotate_y(NULL, work->ang.y);
    set_texture(cave_key_door_panel_texlists[KEY_COLOR(task->work)]);
    if (key_door_hides_player(task)) {
        draw_control_save();
        draw_attr_save();
        draw_control_set(SEE_THROUGH_CONTROL);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        model_draw_tinted(panel_models[KEY_COLOR(task->work)],
                          SEE_THROUGH_COLOR);
        draw_attr_restore();
        draw_control_restore();
    } else {
        draw(panel_models[KEY_COLOR(task->work)]);
    }
    pop();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static struct key_door *key_door_alloc(struct task *task)
{
    task->extra.p = syMalloc(sizeof(struct key_door));
    if (!task->extra.p) {
        return NULL;
    }
    DOOR(task)->steps = 0;
    return task->extra.p;
}

static void key_door_free(struct task *task)
{
    if (task->extra.p) {
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

static void key_door_take_next_mode(struct task *task)
{
    if (DOOR(task)->next_mode != NO_MODE) {
        DOOR(task)->mode = DOOR(task)->next_mode;
        DOOR(task)->next_mode = NO_MODE;
    }
}

void cave_key_door_set_next_mode(struct task *task, int mode)
{
    DOOR(task)->next_mode = mode;
}

static void key_door_shut_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *keyhole = cave_keyholes[KEY_COLOR(work)];

    if (keyhole && KEYHOLE(keyhole)->mode == KEYHOLE_MODE_SINK) {
        DOOR(task)->steps = 0;
        cave_key_door_set_next_mode(task, MODE_RISE);
    }
}

static void key_door_rise_step(struct task *task)
{
    struct task_work *work = task->work;
    struct key_door *door;
    struct collision_work *trigger;
    struct task *keyhole;
    int color;

    /* @hack spelling: color, a local read once, where KEY_COLOR(work) would
     * do */
    color = KEY_COLOR(work);
    keyhole = cave_keyholes[color];
    if (!keyhole) {
        return;
    }
    /* @hack redundant: a block no path reaches, a test of keyhole */
    if (0) {
        if (keyhole) {
        }
    }
    DOOR(task)->steps++;
    HEIGHT(task) = DOOR(task)->steps * RISE_SPEED;
    if (DOOR(task)->steps >= RISE_STEPS) {
        key_door_shake(task);
        HEIGHT(task) = OPEN_HEIGHT;
        task->work->scl.y = OPEN_SCL_Y;
        cave_key_door_set_next_mode(task, MODE_OPEN);
    }
    /* @hack spelling: the assignment of door inside the expression, where a
     * statement of its own would do */
    trigger = (door = DOOR(task))->trigger->work->collision;
    if ((trigger->flags & COLLISION_TOUCHED) &&
        collision_next_player(door->trigger)) {
        trigger->flags &= ~COLLISION_TOUCHED;
        HEIGHT(task) = 0.0F;
        sound_play_at(SLAM_SOUND, NULL, SLAM_SOUND_PRIORITY, slam_volume,
                      &work->pos);
        cave_key_door_set_next_mode(task, MODE_CLOSED);
    }
    sound_play_at_for(RUMBLE_SOUND, task, RUMBLE_SOUND_PRIORITY, rumble_volume,
                      RUMBLE_SOUND_STEPS, &work->pos);
}

static void key_door_open_step(struct task *task)
{
    struct task_work *work = task->work;
    struct key_door *door;
    struct collision_work *trigger;
    struct task *keyhole;

    keyhole = cave_keyholes[KEY_COLOR(work)];
    if (!keyhole) {
        return;
    }
    /* @hack spelling: the assignment of door inside the expression, where a
     * statement of its own would do */
    trigger = (door = DOOR(task))->trigger->work->collision;
    if ((trigger->flags & COLLISION_TOUCHED) &&
        collision_next_player(door->trigger)) {
        trigger->flags &= ~COLLISION_TOUCHED;
        HEIGHT(task) = OPEN_HEIGHT;
        cave_key_door_set_next_mode(task, MODE_FALL);
        DOOR(task)->steps = 0;
    }
}

static void key_door_fall_step(struct task *task)
{
    struct task_work *work = task->work;

    DOOR(task)->steps++;
    HEIGHT(task) = njCos(DOOR(task)->steps * FALL_TURN) * OPEN_HEIGHT;
    if (DOOR(task)->steps >= FALL_STEPS) {
        key_door_shake(task);
        HEIGHT(task) = 0.0F;
        sound_play_at(SLAM_SOUND, NULL, SLAM_SOUND_PRIORITY, slam_volume,
                      &work->pos);
        cave_key_door_set_next_mode(task, MODE_CLOSED);
    }
}

static void key_door_closed_step(struct task *task)
{
}

static Bool key_door_hides_player(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 player;
    NJS_POINT3 camera;
    Float crossing;
    Float x;
    Float y;
    Float top;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, -work->ang.y);
    njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
    njCalcPoint(NULL, &player_work[camera_current_index]->pos, &player);
    njCalcPoint(NULL, &camera_views[camera_current_index]->pos, &camera);
    njPopMatrixEx();
    /* @hack spelling: crossing set first to the player's z, where a local of
     * its own would do */
    crossing = player.z;
    if (crossing == camera.z || crossing * camera.z > 0.0F) {
        return FALSE;
    }
    crossing = -camera.z / (crossing - camera.z);
    x = camera.x + (crossing * (player.x - camera.x));
    y = (crossing * (player.y - camera.y)) + camera.y;
    /* @hack redundant: a block no path reaches, a store to x */
    if (0) {
        x = HIDE_TOP;
    }
    /* @hack spelling: top, a local that holds HIDE_TOP, where the constant
     * would do */
    top = HIDE_TOP;
    return x > -HIDE_HALF_WIDTH && x < HIDE_HALF_WIDTH && y > HIDE_BOTTOM &&
           y < top;
}

static void key_door_shake(struct task *task)
{
    struct task_work *work = task->work;
    Float distance;
    Float size;

    distance = njDistanceP2P(&work->pos, &player_work[0]->pos);
    if (distance > 1.0F) {
        size = SHAKE_SIZE / distance;
    } else {
        size = SHAKE_SIZE;
    }
    if (camera_stacks[0].shake_steps > 0) {
        if (size >= camera_stacks[0].shake_size) {
            camera_shake(0, SHAKE_STEPS, size);
        }
    } else {
        camera_shake(0, SHAKE_STEPS, size);
    }
}

void cave_key_door_dust(struct task *task, Float height)
{
    struct task_work *work = task->work;
    Float x;
    Float z;
    struct angles ang;
    NJS_POINT3 pos;
    NJS_VECTOR offset;

    ang.x = 0;
    ang.y = work->ang.y;
    ang.z = 0;
    pos = work->pos;
    cave_polar_xz(work->ang.y, DUST_AHEAD, &x, &z);
    pos.x += x;
    pos.z += z;
    pos.y += height;
    dust_smoke_rect(&pos, DUST_FRONT_HALF_X, DUST_FRONT_HALF_Z, &ang,
                    DUST_SCALE, DUST_DENSITY(), DUST_SPEED);
    ang.x = QUARTER_TURN;
    ang.y = work->ang.y + QUARTER_TURN;
    ang.z = 0;
    pos = work->pos;
    cave_point_turn_y(-DUST_SIDE_X, DUST_SIDE_Z, ang.y, &offset);
    pos.x += offset.x;
    pos.z += offset.z;
    pos.y += height * 0.5F;
    dust_smoke_rect(&pos, DUST_SIDE_HALF_X, height * 0.5F, &ang, DUST_SCALE,
                    DUST_DENSITY(), DUST_SPEED);
    ang.y = work->ang.y - QUARTER_TURN;
    pos = work->pos;
    cave_point_turn_y(DUST_SIDE_X, DUST_SIDE_Z, ang.y, &offset);
    pos.x += offset.x;
    pos.z += offset.z;
    pos.y += height * 0.5F;
    dust_smoke_rect(&pos, DUST_SIDE_HALF_X, height * 0.5F, &ang, DUST_SCALE,
                    DUST_DENSITY(), DUST_SPEED);
}

static void key_door_trigger_step(struct task *task)
{
    struct task *door = task->parent;

    task->work->pos = door->work->pos;
    task->work->ang.y = door->work->ang.y;
    collision_register(task);
}

static void key_door_body_step(struct task *task)
{
    struct task *door = task->parent;
    struct key_door *record;

    task->work->pos = door->work->pos;
    task->work->ang.y = door->work->ang.y;
    /* @hack spelling: the assignment of record inside the test, where a
     * statement of its own would do */
    if ((record = door->extra.p)->mode == MODE_SHUT ||
        record->mode == MODE_CLOSED) {
        task->work->collision->shapes[PANEL_SHAPE].size_y = PANEL_SHUT_HALF;
    } else {
        task->work->collision->shapes[PANEL_SHAPE].size_y = PANEL_OPEN_HALF;
    }
    collision_fit_radius(task);
    collision_register(task);
}
