/* Aquatic Mine's water, the SuimenManager task. the water stands at one of
 * three levels, which switches 1 to 3 pick. the manager turns the current
 * level's switch on and the others off. at a change of level, it plays a sound
 * and shakes player 0's pad. it then makes its children anew. the children are
 * a lift, MILIFT of stg07/solid_models.h, and a task for each of the level's
 * pieces. the manager draws the water of levels 0 and 1. each piece's task
 * draws that piece's surface */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/switch.h"
#include "shared/water_bubbles.h"
#include "sound/sound.h"
#include "stg07/solid_models.h"
#include "stg07/water.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "vmu/vibration.h"

#include "stg07/water/text.inc"

static NJS_TEXNAME water_texlist_names[29] = {
#include "stg07/water/water_texlist_names.inc"
};

NJS_TEXLIST water_texlist = {
#include "stg07/water/water_texlist.inc"
};

/* each piece's draw rewrites the vertices of the ring's surface or of the
 * plain one */
#include "stg07/scenery/water.h"

#define LEVELS 3
#define PIECES 16
/* the id of the switch that picks level */
#define LEVEL_SWITCH(level) ((level) + 1)

/* a piece of the water at one of the levels */
struct water_piece {
    Sint32 level;
    NJS_OBJECT *object; /* its collision object */
    Sint8 mode; /* how its task draws it, a water_piece_mode */
};

/* in a piece's work's mode: how it draws its surface. MODE_RING opens a ring
 * in it about player 0, and MODE_PLAIN draws it whole */
enum water_piece_mode { MODE_RING, MODE_PLAIN };

/* the level the manager's children were made for, in its work's tick */
#define CHILDREN_LEVEL(work) ((work)->tick)
/* the steps left until a change of level ends, or 0, in the manager's
 * task->unk_3c */
#define STEPS_TO_STOP(task) ((task)->unk_3c.l)
/* the manager starts at its first step that finds player 0 with its work's
 * timer past WAIT_STEPS. while game_state.steps is below SETTLE_STEPS, it holds
 * the water at level 0 and makes no children */
#define WAIT_STEPS 3
#define SETTLE_STEPS 5
/* a change to a lower level plays LEVEL_DOWN_SOUND, and one to a higher
 * LEVEL_UP_SOUND. either plays for CHANGE_UPDATES sound updates, and starts a
 * fading shake of player 0's pad. CHANGE_STEPS steps later, the manager plays
 * LEVEL_STOPS_SOUND and shakes the pad again */
#define CHANGE_UPDATES 100
#define CHANGE_STEPS 100
#define LEVEL_DOWN_SOUND 0
#define LEVEL_UP_SOUND 1
#define LEVEL_STOPS_SOUND 2
#define CHANGE_PRIORITY 1
#define CHANGE_POWER 2
#define CHANGE_FREQ 38
#define CHANGE_LENGTH 10
#define STOP_POWER 6
#define STOP_FREQ 50
/* the lift stands at LIFT_TOP_Y while the water is at level 0. at the other
 * levels it stands 210 lower, at LIFT_BOTTOM_Y */
#define LIFT_TOP_Y (-24.53F)
#define LIFT_BOTTOM_Y (-234.53F)
/* task_create's flags for each of the manager's children */
#define CHILD_FLAGS (TASK_WORK | TASK_FORCE)
#define MANAGER_TASK_LEVEL 1

/* a water model's polygon list opens with its NJD_CT_TID chunk, whose second
 * halfword, WATER_TID, holds the texture's index. the index takes the
 * halfword's low 13 bits. each draw that sets it keeps the filter mode's bits,
 * NJD_FFM_MASK, and clears NJD_FSS */
#define WATER_TID 1
/* level 0's and level 1's water shows the first LEVEL_FRAMES textures in
 * turn, one a step. a surface shows SURFACE_FRAMES from SURFACE_FIRST_TEXTURE
 * in the same way */
#define LEVEL_FRAMES 15
#define SURFACE_FRAMES 14
#define SURFACE_FIRST_TEXTURE 15
/* a piece's collision object takes its size class from its model's radius
 * plus CLASS_MARGIN */
#define CLASS_MARGIN 15.0F
/* a surface's model takes as its radius the distance from its center to a
 * corner, plus RADIUS_MARGIN */
#define RADIUS_MARGIN 50.0F
/* how far surface_draw_task's surface reaches past scl.x and scl.z */
#define TASK_SURFACE_MARGIN 100.0F

/* a ring is RING_POINTS vertices on a circle about player 0. its radius is
 * ring_radius less ring_shrink for each unit player 0 lies off the surface.
 * it never falls below 0. the ring's model holds the surface's corners, then
 * the ring's points, then their center */
#define CORNERS 4
#define RING_POINTS 6
#define RING_VERTICES (CORNERS + RING_POINTS + 1)
#define RING_CENTER (CORNERS + RING_POINTS)
/* the words of a struct chunk_vertex_d8 that hold its point's z and its
 * color */
#define VERTEX_Z_WORD 2
#define VERTEX_COLOR_WORD 3
/* the offsets in words, from the last corner's vertex, of the ring's center's
 * x, z and color */
#define CENTER_X ((RING_CENTER - (CORNERS - 1)) * CHUNK_D8_WORDS)
#define CENTER_Z (CENTER_X + VERTEX_Z_WORD)
#define CENTER_COLOR (CENTER_X + VERTEX_COLOR_WORD)
#define UV_OFFSET 270.0F
#define UV_TOP 255.0F
/* the lengths of the ring's first strip, of each other ring strip, and of the
 * plain surface's strip. a strip holds three halfwords a vertex: the
 * vertex's index, then its u and v */
#define RING_FIRST_STRIP_LENGTH 9
#define RING_STRIP_LENGTH 5
#define PLAIN_STRIP_LENGTH 4
/* the halfwords of the polygon list before a strip's first vertex.
 * FIRST_STRIP_SKIP lie before the first strip. they are the NJD_CT_TID chunk's
 * two, then the strip chunk's head, size and count, then the strip's length.
 * STRIP_HEAD_SKIP lie before the ring's third strip, where a second strip chunk
 * starts. they are the first chunk's pad, then the second's head, size and
 * count, then the strip's length. STRIP_SKIP, a strip's length, lies before
 * each other strip */
#define FIRST_STRIP_SKIP 6
#define STRIP_SKIP 1
#define STRIP_HEAD_SKIP 5

static struct water_piece water_pieces[PIECES] = {
#include "stg07/water/water_pieces.inc"
};
/* the level the water stands at, from 0 to LEVELS - 1 */
static Sint32 water_level = 0;
/* the manager's task, from water_start until its destructor runs, or null */
static struct task *water_task = NULL;
static Float ring_radius = 25.0F;
static Float ring_shrink = 0.5F;
/* the textures' units of u and of v to a unit of x and of z */
static Float u_scale = 256.0F / 54.0F;
static Float v_scale = 256.0F / 54.0F;
static Uint32 ring_center_color = 0x60b2b2b2;
/* a sixth of a turn, rounded up */
static Sint32 dat_8c55e854 = 0x2aab;
/* the vertex of each of the ring's points, counted from the last corner's.
 * the points run from a quarter turn down, a sixth of a turn apart */
static Sint32 ring_order[RING_POINTS] = {4, 6, 1, 5, 3, 2};

static void water_piece_destroy(struct task *task);
static void water_piece_step(struct task *task);
static void water_piece_draw(struct task *task);
static void water_wait(struct task *task);
static void water_destroy(struct task *task);
static void water_step(struct task *task);
static void water_draw(struct task *task);
static void piece_bounds(NJS_OBJECT *object, NJS_POINT3 *center, Float *half_x,
                         Float *half_z);
static void surface_draw(NJS_POINT3 *pos, Float half_x, Float half_z);
static void surface_draw_plain(Float half_x, Float half_z);

/* make parent a child for the piece whose collision object is object, and
 * return it. the child puts object in the collision world, and its destructor
 * takes it out */
static struct task *water_piece_create(struct task *parent, NJS_OBJECT *object)
{
    struct task *task;
    Float radius;
    Uint32 flags;

    task = task_create_child(CHILD_FLAGS, water_piece_step, parent);
    /* @bug memory: writes the child's draw, step and destructor through
     * null where task_create_child gives none (MOV.L at 0x8c501180).
     * water_step tests the child this returns for null (TST at
     * 0x8c501608) */
#ifdef NONMATCHING
    /* this side repairs the tag above: without a child, the call returns the
     * null that water_step tests for */
    if (task == NULL) {
        return NULL;
    }
#endif
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = water_piece_draw;
    task->step = water_piece_step;
    task->destructor = water_piece_destroy;
    radius = object->model->r + CLASS_MARGIN;
    flags = WORLD_FLAT_THIN | WORLD_SOLID;
    if (radius <= WORLD_SMALL_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SMALL_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SMALL_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    task->motion.p = object;
    return task;
}

static void water_piece_destroy(struct task *task)
{
    world_remove(task, task->motion.p);
    task->motion.p = NULL;
}

static void water_piece_step(struct task *task)
{
}

/* draw the piece's surface while player 0 is there. it draws nothing while
 * the current camera is below the piece's center */
static void water_piece_draw(struct task *task)
{
    struct task_work *work;
    Float half_x;
    Float half_z;
    NJS_POINT3 center;
    NJS_POINT3 pos;
    NJS_POINT3 *ring;

    work = task->work;
    if (player_work[0] != NULL) {
        piece_bounds(task->motion.p, &center, &half_x, &half_z);
        if (camera_views[camera_current_index]->pos.y < center.y) {
            return;
        }
        switch (work->mode) {
        case MODE_PLAIN:
            njPushMatrixEx();
            njTranslateV(NULL, &center);
            /* @hack spelling: ring, a local set to 0 for the null, where
             * NULL in the call would do */
            ring = 0;
            surface_draw(ring, half_x, half_z);
            njPopMatrix(1);
            break;
        case MODE_RING:
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslate(NULL, -center.x, -center.y, -center.z);
            njCalcPoint(NULL, &player_work[0]->pos, &pos);
            njPopMatrix(1);
            njPushMatrixEx();
            njTranslateV(NULL, &center);
            ring = &pos;
            surface_draw(ring, half_x, half_z);
            njPopMatrix(1);
            break;
        }
    }
}

struct task *water_start(void)
{
    struct task *task;
    struct task_work *work;

    if (water_task == NULL) {
        task = task_create(TASK_WORK, MANAGER_TASK_LEVEL, water_wait,
                           "SuimenManager");
        if (task != NULL) {
            work = task->work;
            CHILDREN_LEVEL(work) = 0;
            water_task = task;
            water_level = 0;
            return task;
        }
    }
    return NULL;
}

void water_stop(void)
{
    if (water_task != NULL) {
        task_destroy(water_task);
    }
}

/* the manager's step until it starts, when it gives the manager its own
 * step, draw and destructor. where shared/water_bubbles.h's
 * water_surface_height is null, it then sets it to water_height */
static void water_wait(struct task *task)
{
    struct task_work *work;

    work = task->work;
    work->timer++;
    if (player_work[0] != NULL && work->timer > WAIT_STEPS) {
        /* @hack spelling: the draw set before the step, where struct
         * task's order would set the step first */
        task->draw = water_draw;
        task->step = water_step;
        task->destructor = water_destroy;
        if (water_surface_height == NULL) {
            water_surface_height = water_height;
        }
    }
}

static void water_destroy(struct task *task)
{
    if (water_task == task) {
        water_task = NULL;
    }
    if (water_surface_height == water_height) {
        water_surface_height = NULL;
    }
}

/* a switch that is on picks its level, where that isn't the current one. of
 * two such switches, the higher level's wins */
static void water_step(struct task *task)
{
    struct task_work *work;
    struct task *child;
    int i;
    int level;

    work = task->work;
    if ((water_level != 0 || CHILDREN_LEVEL(work) != 0) &&
        game_state.steps < SETTLE_STEPS) {
        water_level = 0;
        CHILDREN_LEVEL(work) = 0;
        if (task->child != NULL) {
            task_destroy_children(task);
        }
    }
    level = -1;
    for (i = 0; i < LEVELS; i++) {
        if (water_level != i && switch_is_on(LEVEL_SWITCH(i))) {
            level = i;
        }
    }
    if (level >= 0 && water_level != level) {
        water_level = level;
    }
    for (i = 0; i < LEVELS; i++) {
        /* @hack spelling: the comparison's 1 and 0 written out, where the
         * comparison would do */
        switch_set(LEVEL_SWITCH(i), water_level == i ? 1 : 0);
    }
    if (STEPS_TO_STOP(task) != 0 && --STEPS_TO_STOP(task) == 0) {
        sound_play(LEVEL_STOPS_SOUND, NULL, 0, 0);
        vibration_start(0, STOP_POWER, STOP_FREQ, VIBRATION_UNTIMED);
    }
    if (CHILDREN_LEVEL(work) != water_level && water_level >= 0 &&
        water_level < LEVELS) {
        if (CHILDREN_LEVEL(work) > water_level) {
            sound_play_for(LEVEL_DOWN_SOUND, work, CHANGE_PRIORITY, 0,
                           CHANGE_UPDATES);
        } else {
            sound_play_for(LEVEL_UP_SOUND, work, CHANGE_PRIORITY, 0,
                           CHANGE_UPDATES);
        }
        STEPS_TO_STOP(task) = CHANGE_STEPS;
        /* @hack redundant: the test of the tick at or above 0, which always
         * holds for the unsigned tick */
        if (CHILDREN_LEVEL(work) >= 0) {
            vibration_start_fading(0, CHANGE_POWER, CHANGE_FREQ, CHANGE_LENGTH);
        }
        task_destroy_children(task);
        CHILDREN_LEVEL(work) = water_level;
    }
    if (task->child == NULL && game_state.steps >= SETTLE_STEPS) {
        level = CHILDREN_LEVEL(work);
        if (level >= 0 && level < LEVELS) {
            child = task_create_child(CHILD_FLAGS, milift_init, task);
            if (child != NULL) {
                switch (water_level) {
                case 0:
                    child->work->pos.x = 0.0F;
                    child->work->pos.y = LIFT_TOP_Y;
                    child->work->pos.z = 0.0F;
                    break;
                default:
                    child->work->pos.x = 0.0F;
                    child->work->pos.y = LIFT_BOTTOM_Y;
                    child->work->pos.z = 0.0F;
                    break;
                }
                child->work->ang.x = 0;
                child->work->ang.y = QUARTER_TURN;
                child->work->ang.z = 0;
                child->work->submode = SOLID_MODEL_STAYS;
            }
        }
        /* @hack type: i compared as unsigned, where i < PIECES would do */
        for (i = 0; (Uint32)i < PIECES; i++) {
            if (water_pieces[i].level == water_level) {
                child = water_piece_create(task, water_pieces[i].object);
                if (child != NULL) {
                    child->work->mode = water_pieces[i].mode;
                }
            }
        }
    }
}

/* draw the water of level 0 or 1 while it is the current level. then set the
 * texture its next draw shows, by game_state.steps */
static void water_draw(struct task *task)
{
    /* @hack spelling: texture, a local for the texture word, where the
     * word itself would do */
    Sint16 texture;

    njSetTexture(&water_texlist);
    switch (water_level) {
    case 0:
        njCnkEasyDrawObject(water_level_0_object);
        texture = water_level_0_object->model->plist[WATER_TID];
        texture = (texture & NJD_FFM_MASK) | (game_state.steps % LEVEL_FRAMES);
        water_level_0_object->model->plist[WATER_TID] = texture;
        break;
    case 1:
        njCnkEasyDrawObject(water_level_1_object);
        texture = water_level_1_object->model->plist[WATER_TID];
        texture = (texture & NJD_FFM_MASK) | (game_state.steps % LEVEL_FRAMES);
        water_level_1_object->model->plist[WATER_TID] = texture;
        break;
    }
}

/* draw a surface with a ring about player 0. the surface lies at the pos of
 * task's work, turned by its ang.y. it reaches scl.x + TASK_SURFACE_MARGIN
 * either way along x, and scl.z + TASK_SURFACE_MARGIN along z. nothing calls
 * the function, and no header declares it. it stays external, since SHC leaves
 * out a static function that nothing calls */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void surface_draw_task(struct task *task);

void surface_draw_task(struct task *task)
{
    struct task_work *work;
    NJS_POINT3 pos;
    NJS_POINT3 *ring;

    work = task->work;
    if (placement_try_leave(task)) {
        return;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njInvertMatrix(NULL);
    njCalcPoint(NULL, &player_work[0]->pos, &pos);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    ring = &pos;
    surface_draw(ring, work->scl.x + TASK_SURFACE_MARGIN,
                 work->scl.z + TASK_SURFACE_MARGIN);
    njPopMatrixEx();
}

/* set *center to the center of the box that bounds object's model's points,
 * moved by object's pos. set *half_x and *half_z to the box's half
 * sizes along x and z */
static void piece_bounds(NJS_OBJECT *object, NJS_POINT3 *center, Float *half_x,
                         Float *half_z)
{
    NJS_POINT3 *point;
    Float max_x;
    Float max_y;
    Float max_z;
    Float min_x;
    Float min_y;
    Float min_z;
    int count;
    int i;

    count = object->model->nbPoint;
    point = object->model->points;
    /* @hack spelling: the six bounds set in this order, where x, y and z's
     * would do */
    max_z = point->z;
    max_x = point->x;
    min_z = point->z;
    min_x = point->x;
    max_y = point->y;
    min_y = point->y;
    for (i = 0; i < count; i++, point++) {
        if (point->x > max_x) {
            max_x = point->x;
        }
        if (point->z > max_z) {
            max_z = point->z;
        }
        if (point->y > max_y) {
            max_y = point->y;
        }
        if (min_x > point->x) {
            min_x = point->x;
        }
        if (min_z > point->z) {
            min_z = point->z;
        }
        if (min_y > point->y) {
            min_y = point->y;
        }
    }
    center->x = object->pos[0] + ((max_x + min_x) * 0.5F);
    center->y = object->pos[1] + ((max_y + min_y) * 0.5F);
    center->z = object->pos[2] + ((max_z + min_z) * 0.5F);
    *half_x = (max_x - min_x) * 0.5F;
    *half_z = (max_z - min_z) * 0.5F;
}

Float water_height(NJS_POINT3 *pos)
{
    Float height;
    Float half_x;
    Float half_z;
    NJS_POINT3 center;
    int i;

    height = WATER_NONE;
    /* @hack type: i compared as unsigned, where i < PIECES would do */
    for (i = 0; (Uint32)i < PIECES; i++) {
        if (water_pieces[i].level == water_level) {
            piece_bounds(water_pieces[i].object, &center, &half_x, &half_z);
            if (center.x + half_x > pos->x && center.x - half_x < pos->x &&
                center.z + half_z > pos->z && center.z - half_z < pos->z) {
                height = center.y;
                break;
            }
        }
    }
    return height;
}

/* draw a surface about the current matrix's origin. it reaches half_x either
 * way along x, and half_z along z. a null pos draws it plain. otherwise a ring
 * opens in it about pos, which is in the surface's frame */
static void surface_draw(NJS_POINT3 *pos, Float half_x, Float half_z)
{
    NJS_CNK_OBJECT *object;
    struct chunk_vertex_d8 *vertex;
    Sint16 *strip;
    Sint16 *texture_word;
    Sint32 *order;
    Float max_x;
    Float max_z;
    Float min_x;
    Float min_z;
    Float radius;
    Float x;
    Float z;
    Angle ang;
    int i;
    int index;
    Sint16 texture;

    if (pos == NULL) {
        surface_draw_plain(half_x, half_z);
        return;
    }
    object = water_ring_object;
    texture_word = &object->model->plist[WATER_TID];
    /* @hack redundant: a step of texture_word in a block no path reaches,
     * kept for the registers SHC gives the draw */
    if (0) {
        texture_word++;
    }
    texture = *texture_word;
    texture = (texture & NJD_FFM_MASK) |
              ((game_state.steps % SURFACE_FRAMES) + SURFACE_FIRST_TEXTURE);
    *texture_word = texture;
    vertex =
        (struct chunk_vertex_d8 *)(object->model->vlist + CHUNK_HEAD_WORDS);
    max_x = half_x;
    max_z = half_z;
    min_x = -half_x;
    min_z = -half_z;
    /* @hack spelling: the ring's work in a block of its own, and the strips'
     * in another, where one block would do */
    {
        NJS_POINT3 at;

        at = *pos;
        radius = ring_radius - (ring_shrink * njAbs(at.y));
        if (radius <= 0.0F) {
            radius = 0.0F;
        }
        /* @hack redundant: a store of radius to itself in a block no path
         * reaches, kept for the frame and the registers SHC gives the draw */
        if (0) {
            radius = radius; /* NOLINT(clang-diagnostic-self-assign) */
        }
        /* @hack redundant: the same, for a store of min_x to min_z */
        if (0) {
            min_z = min_x;
        }
        /* @hack redundant: the same, for stores of half_z to max_z and of
         * half_x to max_x */
        if (0) {
            max_z = half_z;
            max_x = half_x;
        }
        vertex->point.x = min_x;
        vertex->point.z = half_z;
        vertex++;
        vertex->point.x = min_x;
        vertex->point.z = min_z;
        vertex++;
        vertex->point.x = half_x;
        vertex->point.z = half_z;
        vertex++;
        vertex->point.x = half_x;
        vertex->point.z = min_z;
        ang = QUARTER_TURN;
        /* @hack redundant: a store of max_x to radius in a block no path
         * reaches, kept for the registers SHC gives the draw */
        if (0) {
            radius = max_x;
        }
        for (order = ring_order; order < ring_order + RING_POINTS; order++) {
            x = at.x + (radius * njSin(ang));
            z = at.z + (radius * njCos(ang));
            if (x > max_x) {
                x = max_x;
            }
            if (min_x > x) {
                x = min_x;
            }
            if (z > max_z) {
                z = max_z;
            }
            if (min_z > z) {
                z = min_z;
            }
            index = *order;
            vertex[index].point.x = x;
            vertex[index].point.z = z;
            /* @hack redundant: a store of z to ang in a block no path
             * reaches, kept for the registers SHC gives the draw */
            if (0) {
                ang = z;
            }
            ang -= FULL_TURN / RING_POINTS;
        }
        x = at.x;
        z = at.z;
        if (x > max_x) {
            x = max_x;
        }
        if (min_x > x) {
            x = min_x;
        }
        if (z > max_z) {
            z = max_z;
        }
        if (min_z > z) {
            z = min_z;
        }
        /* @hack redundant: a store of radius to z in a block no path
         * reaches, kept for the registers SHC gives the draw */
        if (0) {
            z = radius;
        }
        /* @hack type: the ring's center stored through Float and Uint32
         * views of vertex, which points at the last corner, where
         * vertex[RING_CENTER - (CORNERS - 1)] would do */
        ((Float *)vertex)[CENTER_X] = x;
        ((Float *)vertex)[CENTER_Z] = z;
        ((Uint32 *)vertex)[CENTER_COLOR] = ring_center_color;
    }
    {
        struct chunk_vertex_d8 *cursor;
        NJS_POINT3 points[RING_VERTICES];
        NJS_TEX uv[RING_VERTICES];
        NJS_TEX *tex;
        NJS_POINT3 *point;
        int strip_index;

        /* @hack spelling: object set inside the expression, and as the
         * address of the array's first element, where object, already set,
         * would do */
        vertex = (struct chunk_vertex_d8 *)((object = &water_ring_object[0])
                                                ->model->vlist +
                                            CHUNK_HEAD_WORDS);
        strip = object->model->plist;
        /* @hack redundant: steps of vertex in a block no path reaches, kept
         * for the registers SHC gives the draw */
        if (0) {
            vertex++;
            vertex++;
            vertex++;
            vertex++;
        }
        cursor = vertex;
        for (i = 0; i < RING_VERTICES; i++, cursor++) {
            points[i].x = cursor->point.x;
            points[i].z = cursor->point.z;
        }
        point = points;
        for (i = 0; i < RING_VERTICES; i++) {
            tex = &uv[i];
            tex->u = (point->x + UV_OFFSET) * u_scale;
            tex->v = UV_TOP - ((point->z + UV_OFFSET) * v_scale);
            point++;
            /* @hack redundant: a store of uv's end to point in a block no
             * path reaches, kept for the registers SHC gives the draw */
            if (0) {
                point = (NJS_POINT3 *)&uv[RING_VERTICES];
            }
            /* @hack redundant: the same store again, in a block of its own */
            if (0) {
                point = (NJS_POINT3 *)&uv[RING_VERTICES];
            }
            /* @hack redundant: the same store again, in a block of its own */
            if (0) {
                point = (NJS_POINT3 *)&uv[RING_VERTICES];
            }
            /* @hack redundant: the same store again, in a block of its own */
            if (0) {
                point = (NJS_POINT3 *)&uv[RING_VERTICES];
            }
            /* @hack redundant: the same store again, in a block of its own */
            if (0) {
                point = (NJS_POINT3 *)&uv[RING_VERTICES];
            }
        }
        /* @hack redundant: steps of strip in a block no path reaches, kept
         * for the registers SHC gives the draw */
        if (0) {
            strip++;
            strip++;
            strip++;
        }
        strip += FIRST_STRIP_SKIP;
        for (i = 0; i < RING_FIRST_STRIP_LENGTH; i++) {
            index = *strip++;
            *strip++ = uv[index].u;
            *strip++ = uv[index].v;
        }
        strip += STRIP_SKIP;
        for (i = 0; i < RING_STRIP_LENGTH; i++) {
            strip_index = *strip++;
            *strip++ = uv[strip_index].u;
            *strip++ = uv[strip_index].v;
        }
        /* @hack redundant: a store of strip_index through strip in a block no
         * path reaches, kept for the registers SHC gives the draw */
        if (0) {
            *strip = strip_index;
        }
        strip += STRIP_HEAD_SKIP;
        for (i = 0; i < RING_STRIP_LENGTH; i++) {
            index = *strip++;
            *strip++ = uv[index].u;
            *strip++ = uv[index].v;
        }
        strip += STRIP_SKIP;
        for (i = 0; i < RING_STRIP_LENGTH; i++) {
            index = *strip++;
            *strip++ = uv[index].u;
            *strip++ = uv[index].v;
        }
    }
    water_ring_object->model->r =
        njSqrt((half_x * half_x) + (half_z * half_z)) + RADIUS_MARGIN;
    water_ring_object->model->center.x = 0.0F;
    water_ring_object->model->center.y = 0.0F;
    water_ring_object->model->center.z = 0.0F;
    njSetTexture(&water_texlist);
    njCnkEasyDrawModel(water_ring_object->model);
}

/* draw the plain surface, as surface_draw does for a null pos */
static void surface_draw_plain(Float half_x, Float half_z)
{
    NJS_CNK_OBJECT *object;
    struct chunk_vertex_d8 *vertex;
    Sint16 *strip;
    NJS_POINT3 points[CORNERS];
    NJS_TEX uv[CORNERS];
    NJS_TEX *tex;
    struct chunk_vertex_d8 *cursor;
    int i;
    int index;
    Sint16 texture;
    NJS_POINT3 *point;
    NJS_TEX *second;
    NJS_TEX *end;

    object = water_plain_object;
    texture = object->model->plist[WATER_TID];
    texture = (texture & NJD_FFM_MASK) |
              ((game_state.steps % SURFACE_FRAMES) + SURFACE_FIRST_TEXTURE);
    object->model->plist[WATER_TID] = texture;
    vertex =
        (struct chunk_vertex_d8 *)(object->model->vlist + CHUNK_HEAD_WORDS);
    vertex->point.x = -half_x;
    vertex->point.z = half_z;
    vertex++;
    vertex->point.x = -half_x;
    vertex->point.z = -half_z;
    vertex++;
    vertex->point.x = half_x;
    vertex->point.z = half_z;
    vertex++;
    vertex->point.x = half_x;
    vertex->point.z = -half_z;
    vertex =
        (struct chunk_vertex_d8 *)(object->model->vlist + CHUNK_HEAD_WORDS);
    strip = object->model->plist;
    cursor = vertex;
    for (i = 0; i < CORNERS; i++, cursor++) {
        points[i].x = cursor->point.x;
        points[i].z = cursor->point.z;
    }
    point = points;
    /* @hack redundant: steps of point in a block no path reaches, kept for
     * the registers SHC gives the draw */
    if (0) {
        point++;
        point++;
        point++;
    }
    /* @hack spelling: the loop over the texture coordinates written as two
     * corners a pass, the second through a copy of tex, and tex set before
     * end, where a loop of a corner a pass would do */
    for (tex = uv, end = uv + CORNERS; tex < end;) {
        tex->u = (point->x + UV_OFFSET) * u_scale;
        tex->v = UV_TOP - ((point->z + UV_OFFSET) * v_scale);
        /* @hack spelling: tex = tex + 1, where tex++ would do */
        tex = tex + 1;
        point++;
        second = tex;
        tex++;
        second->u = (point->x + UV_OFFSET) * u_scale;
        second->v = UV_TOP - ((point->z + UV_OFFSET) * v_scale);
        point++;
        /* @hack redundant: a store of uv to point in a block no path reaches,
         * kept for the registers SHC gives the draw and the order of its
         * pool */
        if (0) {
            point = (NJS_POINT3 *)uv;
        }
        /* @hack redundant: the same store again, in a block of its own */
        if (0) {
            point = (NJS_POINT3 *)uv;
        }
        /* @hack redundant: the same, for three stores of v_scale through
         * point */
        if (0) {
            point->x = v_scale;
            point->x = v_scale;
            point->x = v_scale;
        }
        /* @hack redundant: the same, for a store of u_scale through point */
        if (0) {
            point->x = u_scale;
        }
        /* @hack redundant: the same, for two stores of UV_TOP through point */
        if (0) {
            point->x = UV_TOP;
            point->x = UV_TOP;
        }
        /* @hack redundant: the same, for a step of end */
        if (0) {
            end++;
        }
    }
    /* @hack redundant: steps of strip in a block no path reaches, kept for
     * the registers SHC gives the draw */
    if (0) {
        strip++;
        strip++;
        strip++;
    }
    /* @hack spelling: strip = strip + FIRST_STRIP_SKIP, where += would do */
    strip = strip + FIRST_STRIP_SKIP;
    for (i = 0; i < PLAIN_STRIP_LENGTH; i++) {
        index = *strip++;
        *strip++ = uv[index].u;
        *strip++ = uv[index].v;
    }
    /* @bug logic: sets the radius of the ring's model, where the model this
     * draw draws is the plain one's (FMOV.S at 0x8c501e22) */
    water_ring_object->model->r =
        njSqrt((half_x * half_x) + (half_z * half_z)) + RADIUS_MARGIN;
    njSetTexture(&water_texlist);
    njCnkEasyDrawModel(water_plain_object->model);
}
