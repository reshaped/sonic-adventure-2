/* the laser barrier's code. shared/core_laser.c compiles it for Cannon's
 * Core, and shared/colony_laser.c with LASER_DEFAULT_VOLUME for the other
 * stages. the includer defines the art, contact_shape and barrier_shape first.
 * without LASER_DEFAULT_VOLUME, it defines contact_volume too */
#ifndef SHARED_CORE_LASER_IMPL_H
#define SHARED_CORE_LASER_IMPL_H

#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/core_laser.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/task.h"

#define LASER_PLAYERS 2
/* the bit of ang.x that puts one solid box in the beams' place */
#define SOLID_BARRIER 1
#define BARRIER_SHAPES 1
#define CONTACT_SHAPES 1
#define CONTACT_SOUND 15
#define CONTACT_PRIORITY 1
#define CONTACT_VOLUME 0
#define CONTACT_DURATION 120
/* the player a contact follows, which it keeps in its work's tick */
#define CONTACT_PLAYER(work) ((work)->tick)
/* the rows of beams, which each use takes as at least 1 */
#define ROWS(work) ((work)->scl.z)
/* the rows stand scl.y + ROW_GAP apart */
#define ROW_GAP 10.0F
/* the solid box is SOLID_EXTRA_WIDTH wider than scl.x, and reaches
 * SOLID_HALF_DEPTH to either side of the rows */
#define SOLID_EXTRA_WIDTH 50.0F
#define SOLID_HALF_DEPTH 2.5F
/* a contact keeps to the barrier's face, in the barrier's own space. one row's
 * face reaches FACE_HALF_ROW above and below the middle. with more rows, the
 * face takes them FACE_ROW_GAP + scl.y apart. it is FACE_EXTRA_WIDTH wider
 * than twice scl.x */
#define FACE_HALF_ROW 2.5F
#define FACE_ROW_GAP 5.0F
#define FACE_EXTRA_WIDTH 8.0F
/* a row's beam moves BEAM_SPEED at each of game_state.steps, round a period of
 * BEAM_PERIOD times scl.x. each row's count of steps leads that of the row
 * below by ROW_LEAD */
#define BEAM_SPEED 5
#define BEAM_PERIOD 4.0F
#define ROW_LEAD 20
/* a beam's half width is scl.x less ODD_INSET on an odd step. on an even step
 * it is scl.x less EVEN_INSET, and the beam runs half a period behind */
#define ODD_INSET 10.0F
#define EVEN_INSET 12.0F
/* with time stopped, a row's beam centers STOPPED_INSET short of twice scl.x,
 * at an even step's width */
#define STOPPED_INSET 10.0F
/* a beam's model scales by BEAM_SCALE for each unit of the beam's width */
#define BEAM_SCALE 0.1F

static void core_laser_destroy(struct task *task);
static void core_laser_step(struct task *task);
static void core_laser_draw(struct task *task);

static void core_laser_contact(struct task *task)
{
    collision_register(task);
}

/* @hack spelling: the contacts' loop written out at both sites by a macro,
 * where a function both call would do */
#define CREATE_CONTACTS(task, work, player)                                    \
    do {                                                                       \
        struct task *contact;                                                  \
        for ((player) = 0; (player) < LASER_PLAYERS; (player)++) {             \
            if (player_work[player]) {                                         \
                contact =                                                      \
                    task_create_child(TASK_WORK, core_laser_contact, (task));  \
                if (contact) {                                                 \
                    collision_init(contact, contact_shape, CONTACT_SHAPES,     \
                                   COLLISION_ITEM);                            \
                    CONTACT_PLAYER(contact->work) = player;                    \
                    contact->work->pos = (work)->pos;                          \
                    contact->work->collision->flags &= ~COLLISION_TARGET;      \
                }                                                              \
            }                                                                  \
        }                                                                      \
    } while (0)

void core_laser_init(struct task *task)
{
    struct task_work *work = task->work;
    int count;
    int player;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(NJS_OBJECT));
    if (!task->motion.p) {
        return;
    }
    task->draw = core_laser_draw;
    task->step = core_laser_step;
    task->destructor = core_laser_destroy;
    count = ROWS(work);
    if (count < 1) {
        count = 1;
    }
    if (work->ang.x & SOLID_BARRIER) {
        collision_init(task, barrier_shape, BARRIER_SHAPES, COLLISION_OBJECT);
        if (!work->collision) {
            return;
        }
        work->collision->shapes->radius =
            (work->scl.x + SOLID_EXTRA_WIDTH) * 0.5F;
        work->collision->shapes->size_y =
            count * (work->scl.y + ROW_GAP) * 0.5F;
        work->collision->shapes->size_z = SOLID_HALF_DEPTH;
        work->collision->shapes->ang.y = work->ang.y;
        work->collision->shapes->ang.z = work->ang.z;
        collision_fit_radius(task);
    } else {
        CREATE_CONTACTS(task, work, player);
    }
}

static void core_laser_destroy(struct task *task)
{
    if (((NJS_OBJECT *)task->motion.p)->model) {
        world_remove(task, task->motion.p);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void core_laser_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *child;
    NJS_POINT3 point;
    Float top;
    Float bottom;
    Float right;
    Float left;
    int count;
    int player;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a loop no path enters, which tests count */
    while (0) {
        if (count < 1) {
        }
    }
    if (game_state.time_stopped) {
        if (task->child) {
            task_destroy_children(task);
        }
        /* @hack redundant: a test of SOLID_BARRIER no path reaches */
        if (0) {
            if (work->ang.x & SOLID_BARRIER) {
            }
        }
    } else {
        if (task->child) {
            count = ROWS(work);
            if (count < 1) {
                count = 1;
            }
            if (count == 1) {
                top = FACE_HALF_ROW;
                bottom = -FACE_HALF_ROW;
            } else {
                top = count * (work->scl.y + FACE_ROW_GAP) * 0.5F;
                bottom = -top;
            }
            right = ((work->scl.x * 2.0F) + FACE_EXTRA_WIDTH) * 0.5F;
            left = -right;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            njRotateZ(NULL, work->ang.z);
            njPushMatrixEx();
            njInvertMatrix(NULL);
            /* each contact takes the point of the barrier's face nearest its
             * player, found in the barrier's own space */
            child = task->child;
            do {
                if (player_work[CONTACT_PLAYER(child->work)]) {
                    njCalcPoint(NULL,
                                &player_work[CONTACT_PLAYER(child->work)]->pos,
                                &point);
                    point.z = 0.0F;
                    if (point.y > top) {
                        point.y = top;
                    }
                    if (point.y < bottom) {
                        point.y = bottom;
                    }
                    if (point.x > right) {
                        point.x = right;
                    }
                    if (point.x < left) {
                        point.x = left;
                    }
                    child->work->pos = point;
                }
                child = child->next;
            } while (child && child != task->child);
            njPopMatrixEx();
            child = task->child;
#ifdef LASER_DEFAULT_VOLUME
            /* @hack redundant: two tests of player against CONTACT_DURATION
             * that no path reaches */
            if (0) {
                if (player > CONTACT_DURATION || player < CONTACT_DURATION) {
                }
            }
#endif
            do {
                if (player_work[CONTACT_PLAYER(child->work)]) {
                    struct task_work *contact = child->work;

                    njCalcPoint(NULL, &contact->pos, &contact->pos);
                    /* each pass scans task->child, the first contact, and not
                     * the contact it is at. collision_next_player goes on
                     * from the hit it found last for the same task. so the
                     * passes take the first contact's player hits in turn,
                     * and no pass reads another contact's. the sound plays on
                     * the first contact */
                    if (task->child->work->collision &&
                        collision_next_player(task->child)) {
                        sound_play_for(CONTACT_SOUND, task->child,
                                       CONTACT_PRIORITY, CONTACT_VOLUME,
                                       CONTACT_DURATION);
#ifndef LASER_DEFAULT_VOLUME
                        sound_set_volume(task->child, SOUND_ANY_ID,
                                         contact_volume);
#endif
                    }
                }
                child = child->next;
            } while (child && child != task->child);
            njPopMatrixEx();
        } else if (!(work->ang.x & SOLID_BARRIER)) {
            CREATE_CONTACTS(task, work, player);
        }
        if (work->collision) {
            collision_register(task);
        }
    }
}

static void core_laser_draw_beam(Float center, Float half_width, Float limit)
{
    Float right = center + half_width;
    Float left = center - half_width;
    Float midpoint;
    Float scale;

    if (right > limit) {
        right = limit;
    }
    if (right < -limit) {
        right = -limit;
    }
    if (left > limit) {
        left = limit;
    }
    if (left < -limit) {
        left = -limit;
    }
    midpoint = (right + left) * 0.5F;
    scale = (right - left) * BEAM_SCALE;
    njPushMatrixEx();
    njTranslate(NULL, midpoint, 0.0F, 0.0F);
    njScale(NULL, scale, 1.0F, 1.0F);
    njCnkEasyDrawModel(core_laser_beam[0].model);
    njPopMatrixEx();
}

static void core_laser_draw_row(struct task_work *work, int steps)
{
    int period;
    int offset;
    int phase;

    if (game_state.time_stopped) {
        core_laser_draw_beam((work->scl.x * 2.0F) - STOPPED_INSET,
                             work->scl.x - EVEN_INSET, work->scl.x);
    } else {
        offset = steps * BEAM_SPEED;
        period = work->scl.x * BEAM_PERIOD;
        if (period < 1) {
            period = 1;
        }
        if (steps & 1) {
            phase = offset % period;
            core_laser_draw_beam(phase - (work->scl.x * 2.0F),
                                 work->scl.x - ODD_INSET, work->scl.x);
        } else {
            phase = (offset + (period / 2)) % period;
            core_laser_draw_beam(phase - (work->scl.x * 2.0F),
                                 work->scl.x - EVEN_INSET, work->scl.x);
        }
    }
    njTranslate(NULL, work->scl.x, 0.0F, 0.0F);
    njCnkEasyDrawModel(core_laser_right[0].model);
    njTranslate(NULL, -(work->scl.x * 2.0F), 0.0F, 0.0F);
    njCnkEasyDrawModel(core_laser_left[0].model);
}

static void core_laser_draw(struct task *task)
{
    struct task_work *work = task->work;
    int steps;
    Float offset;
    int count;

    count = ROWS(work);
    if (count < 1) {
        count = 1;
    }
    offset = (work->scl.y + ROW_GAP) * (count - 1) * 0.5F;
    njSetTexture(&core_laser_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njTranslate(NULL, 0.0F, -offset, 0.0F);
    njFogDisable();
    steps = game_state.steps;
    while (count--) {
        njPushMatrixEx();
        core_laser_draw_row(work, steps);
        njPopMatrixEx();
        njTranslate(NULL, 0.0F, work->scl.y + ROW_GAP, 0.0F);
        steps += ROW_LEAD;
    }
    njFogEnable();
    njPopMatrixEx();
}

#endif
