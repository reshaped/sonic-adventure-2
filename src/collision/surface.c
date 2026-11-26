/* probes of the collision world's surfaces about a point, and the shadow
 * functions */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/mesh.h"
#include "collision/polygon.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "player/action.h"
#include "player/player.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/frame.h"
#include "system/task.h"
#include "system/util.h"

/* how near a point each probe lists the world's objects */
#define FLOOR_REACH 10.0F
#define NEAREST_REACH 5.0F
#define FIRST_FLOOR_REACH 20.0F
#define PLAYER_REACH 200.0F
#define SIDES_REACH 1.0F
/* surface_probe_first_floor returns FIRST_FLOOR_NEAR instead where its floor
 * lies no more than FIRST_FLOOR_MARGIN below a stack word. it never writes
 * that word. it returns FIRST_FLOOR_NONE where it takes no floor */
#define FIRST_FLOOR_NEAR 3000.0F
#define FIRST_FLOOR_NONE 4000.0F
#define FIRST_FLOOR_MARGIN 1.5F
/* how far above the work's pos shadow_floor probes for the floor */
#define SHADOW_FROM 4.0F
/* how far shadow_draw_player draws a shadow above the work's pos, and how
 * far down the surface below may lie for it to draw one */
#define SHADOW_LIFT 0.3F
#define SHADOW_DEPTH 20000.0F
/* how far above pos surface_probe_ground probes, and how far beyond its
 * reach it takes a task's object */
#define GROUND_LIFT 1.0F
#define GROUND_REACH_PAST 30.0F
/* how far past the player's radius and height surface_probe_player sets
 * its point ahead */
#define AHEAD_PAST 2.0F

/* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
Float surface_probe_floor(Float x, Float y, Float z, struct angles *ang)
{
    struct height_probe probe;
    NJS_VECTOR away;
    Angle ang_x;
    Angle ang_z;
    struct world_object *objects;
    NJS_OBJECT *object;
    struct task_force *force;
    Uint16 i;
    Float best;

    probe.pos.x = x;
    probe.pos.y = y;
    probe.pos.z = z;
    objects = world_objects;
    world_list_near(x, y, z, FLOOR_REACH);
    best = NO_FLOOR;
    /* @bug uninitialized: with no floor, nothing sets ang_x and ang_z, and
     * ang takes the stack words at R15+8 and R15+12 (0x8c058490,
     * 0x8c058498) */
#ifdef NONMATCHING
    /* this side repairs the tag above: with no floor, ang takes a flat
     * tilt */
    ang_x = 0;
    ang_z = 0;
#endif
    for (i = 0; i < world_count; i++) {
        object = objects[i].object;
        if (objects[i].owner != NULL) {
            Float scale;
            Float distance;

            away.x = object->pos[0] - x;
            away.y = 0.0F;
            away.z = object->pos[2] - z;
            distance = njScalor(&away);
            scale = object->scl[0];
            if (scale < object->scl[1]) {
                scale = object->scl[1];
            }
            if (scale < object->scl[2]) {
                scale = object->scl[2];
            }
            if (distance > scale * object->model->r) {
                continue;
            }
        }
        if (!(objects[i].flags & WORLD_FLAT)) {
            if (polygon_find_heights(&probe, objects[i].object,
                                     objects[i].flags)) {
                if (objects[i].owner != NULL) {
                    /* @hack spelling: force set in its test, where a
                     * statement of its own would do */
                    if ((force = objects[i].owner->force.p) != NULL) {
                        probe.above.pos += force->speed.y;
                        probe.below.pos += force->speed.y;
                    }
                }
                if (probe.below.found == TRUE && y >= probe.below.pos &&
                    probe.below.pos > best) {
                    best = probe.below.pos;
                    ang_x = probe.below.ang_x;
                    ang_z = probe.below.ang_z;
                }
            }
        }
    }
    ang->x = ang_x;
    ang->z = ang_z;
    return best;
}

Float surface_probe_nearest(Float x, Float y, Float z, struct angles *ang)
{
    struct floor_probe probe;
    NJS_VECTOR away;
    Angle ang_x;
    Angle ang_z;
    struct world_object *objects;
    NJS_OBJECT *object;
    Uint16 i;
    Float best;
    Float was;
    Float now;

    probe.pos.x = x;
    probe.pos.y = y;
    probe.pos.z = z;
    objects = world_objects;
    world_list_near(x, y, z, NEAREST_REACH);
    best = NO_FLOOR;
    /* @bug uninitialized: with no floor, nothing sets ang_x and ang_z. ang
     * takes ang_x's register, R8, as the caller left it, and the stack word
     * at R15+8 (0x8c0585fe, 0x8c058600) */
#ifdef NONMATCHING
    /* this side repairs the tag above: with no floor, ang takes a flat
     * tilt */
    ang_x = 0;
    ang_z = 0;
#endif
    for (i = 0; i < world_count; i++) {
        object = objects[i].object;
        if (objects[i].owner != NULL) {
            Float scale;
            Float distance;
            /* @hack spelling: reach, a local for the product its test
             * could compute */
            Float reach;

            away.x = object->pos[0] - x;
            away.y = 0.0F;
            away.z = object->pos[2] - z;
            distance = njScalor(&away);
            scale = object->scl[0];
            if (scale < object->scl[1]) {
                scale = object->scl[1];
            }
            if (scale < object->scl[2]) {
                scale = object->scl[2];
            }
            reach = scale * object->model->r;
            if (distance > reach) {
                continue;
            }
        }
        if (polygon_find_floor(&probe, objects[i].object, objects[i].flags)) {
            was = njAbs(y - best);
            now = njAbs(y - probe.floor.pos);
            if (was > now) {
                best = probe.floor.pos;
                if (objects[i].owner != NULL &&
                    objects[i].owner->force.p != NULL) {
                    best += TASK_FORCES_OF(objects[i].owner)->speed.y;
                }
                ang_x = probe.floor.ang_x;
                ang_z = probe.floor.ang_z;
            }
        }
    }
    ang->x = ang_x;
    ang->z = ang_z;
    return best;
}

Float surface_probe_nearest_land(Float x, Float y, Float z, struct angles *ang)
{
    Uint16 i;
    struct world_object *objects;
    struct floor_probe probe;
    /* @hack redundant: object, set and never read */
    NJS_OBJECT *object; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    Float best;
    Float was;
    Float now;
    Angle ang_x;
    Angle ang_z;

    probe.pos.x = x;
    probe.pos.y = y;
    probe.pos.z = z;
    objects = world_objects;
    world_list_near(x, y, z, NEAREST_REACH);
    best = NO_FLOOR;
    /* @bug uninitialized: with no floor, nothing sets ang_x and ang_z, and
     * ang takes the stack words at R15+4 and R15+8 (0x8c0586e0,
     * 0x8c0586e8) */
#ifdef NONMATCHING
    /* this side repairs the tag above: with no floor, ang takes a flat
     * tilt */
    ang_x = 0;
    ang_z = 0;
#endif
    for (i = 0; i < world_count; i++) {
        object = objects[i].object;
        if (objects[i].owner != NULL) {
            continue;
        }
        if (!(objects[i].flags & WORLD_FLAT)) {
            if (polygon_find_floor(&probe, objects[i].object,
                                   objects[i].flags)) {
                was = njAbs(y - best);
                now = njAbs(y - probe.floor.pos);
                if (was > now) {
                    best = probe.floor.pos;
                    /* this test never holds: the loop passes over every
                     * task's object */
                    if (objects[i].owner != NULL &&
                        objects[i].owner->force.p != NULL) {
                        best += TASK_FORCES_OF(objects[i].owner)->speed.y;
                    }
                    ang_x = probe.floor.ang_x;
                    ang_z = probe.floor.ang_z;
                }
            }
        }
    }
    ang->x = ang_x;
    ang->z = ang_z;
    return best;
}

Float surface_probe_first_floor(struct task_work *work, struct angles *ang)
{
    Uint16 i;
    struct world_object *objects;
    /* @hack type: unset as an array of three, where one Float would do */
    Float unset[3];
    struct floor_probe probe;
    NJS_VECTOR away;
    NJS_OBJECT *object;

    probe.pos = work->pos;
    objects = world_objects;
    world_list_near(work->pos.x, work->pos.y, work->pos.z, FIRST_FLOOR_REACH);
    /* @bug uninitialized: nothing writes unset, and the loop's test reads
     * unset[1], the stack word at R15+68 (FMOV.S @R9,FR2 at 0x8c058816) */
#ifdef NONMATCHING
    /* this side repairs the tag above: unset[1] reads -NO_FLOOR, above every
     * floor, so this side never returns FIRST_FLOOR_NEAR */
    unset[1] = -NO_FLOOR;
#endif
    for (i = 0; i < world_count; i++) {
        object = objects[i].object;
        if (objects[i].owner == NULL) {
            Float distance;
            Float scale;
            /* @hack spelling: reach, a local for the product its test
             * could compute */
            Float reach;

            away.x = object->pos[0] - work->pos.x;
            away.y = 0.0F;
            away.z = object->pos[2] - work->pos.z;
            distance = njScalor(&away);
            scale = object->scl[0];
            if (scale < object->scl[1]) {
                scale = object->scl[1];
            }
            if (scale < object->scl[2]) {
                scale = object->scl[2];
            }
            reach = scale * object->model->r;
            if (distance > reach) {
                probe.floor.pos = FIRST_FLOOR_NONE;
                continue;
            }
        }
        if (polygon_find_floor(&probe, objects[i].object, objects[i].flags)) {
            if (unset[1] <= probe.floor.pos + FIRST_FLOOR_MARGIN) {
                probe.floor.pos = FIRST_FLOOR_NEAR;
            }
            break;
        } else {
            probe.floor.pos = FIRST_FLOOR_NONE;
        }
    }
    /* @bug uninitialized: with no floor found, nothing writes probe.floor's
     * tilt, and ang takes its stack words at R15+40 and R15+44. with no
     * object near, nothing writes its pos either, and the result is the
     * word at R15+48 (0x8c05883a to 0x8c058848) */
    ang->x = probe.floor.ang_x;
    ang->z = probe.floor.ang_z;
    return probe.floor.pos;
}
/* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */

void shadow_at_height(struct task_work *work, Float height)
{
    /* @hack redundant: the work's distance from height under a test of
     * frame_step, which nothing reads */
    if (frame_step == 0) {
        (void)njAbs(work->pos.y - height);
    }
}

Float shadow_floor(struct task_work *work)
{
    struct angles slope;
    Float floor;

    floor = surface_probe_floor(work->pos.x, work->pos.y + SHADOW_FROM,
                                work->pos.z, &slope);
    /* @hack redundant: the work's distance from the floor under a test of
     * frame_step, which nothing reads */
    if (frame_step == 0) {
        (void)njAbs(work->pos.y - floor);
    }
    return floor;
}

void shadow_at(void)
{
    /* @hack redundant: a test of frame_step that guards nothing */
    if (frame_step == 0) {
    }
}

Float shadow_floor_slope(struct task_work *work, struct angles *slope)
{
    Float floor;

    floor = surface_probe_floor(work->pos.x, work->pos.y, work->pos.z, slope);
    /* @hack redundant: the work's distance from the floor under a test of
     * frame_step, which nothing reads */
    if (frame_step == 0) {
        (void)njAbs(work->pos.y - floor);
    }
    return floor;
}

void shadow_draw_player(struct task_work *work, struct player *player)
{
    struct ground_probe *ground = &player->ground;

    if (ground->below_y > work->pos.y - SHADOW_DEPTH) {
        draw_control_set(DRAW_CONTROL_SHADOW);
        njPushMatrix(NULL);
        njTranslate(NULL, player->unk_218.x, work->pos.y + SHADOW_LIFT,
                    player->unk_218.z);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njCnkModDrawModel(shadow_model);
        njPopMatrix(1);
        draw_control_clear(DRAW_CONTROL_SHADOW);
    }
}

void surface_probe_ground(NJS_POINT3 *pos, struct ground_probe *ground)
{
    /* @hack spelling: at, a copy of pos for every use after the copy into
     * probe, where pos itself would do */
    NJS_POINT3 *at;
    int i;
    struct world_object *objects;
    struct height_probe probe;
    NJS_POINT3 center;
    NJS_VECTOR away;
    NJS_OBJECT *object;
    Float distance;
    Float scale;
    struct task_force *force;

    ground->last_above_flags = ground->above_flags;
    ground->last_below_flags = ground->below_flags;
    ground->above_flags = 0;
    ground->below_flags = 0;
    at = pos;
    probe.pos = *at;
    probe.pos.y += GROUND_LIFT;
    objects = world_objects;
    ground->above_y = -NO_FLOOR;
    ground->below_y = NO_FLOOR;
    for (i = 0; i < world_count; i++) {
        object = objects[i].object;
        if (objects[i].flags & WORLD_FLAT) {
            if (polygon_find_model_heights(&probe, objects[i].object) &&
                probe.above.found == TRUE && probe.pos.y < probe.above.pos &&
                (stage_current_id() != STAGE_RELAY_SONIC ||
                 probe.above.pos < ground->above_y)) {
                ground->above_y = probe.above.pos;
                ground->above_flags = objects[i].flags;
            }
        } else if (objects[i].owner != NULL) {
            if (objects[i].flags & WORLD_TURNED) {
                njPushMatrixEx();
                njUnitMatrix(NULL);
                if (object->evalflags & NJD_EVAL_ZXY_ANG) {
                    njRotateY(NULL, object->ang[1]);
                    njRotateX(NULL, object->ang[0]);
                    njRotateZ(NULL, object->ang[2]);
                } else {
                    njRotateZ(NULL, object->ang[2]);
                    njRotateY(NULL, object->ang[1]);
                    njRotateX(NULL, object->ang[0]);
                }
                njCalcPoint(NULL, &object->model->center, &center);
                njPopMatrixEx();
                away.x = object->pos[0] + center.x - at->x;
                away.y = 0.0F;
                away.z = object->pos[2] + center.z - at->z;
            } else {
                away.x = object->pos[0] - at->x;
                away.y = 0.0F;
                away.z = object->pos[2] - at->z;
            }
            distance = njScalor(&away);
            scale = object->scl[0];
            if (object->scl[1] > scale) {
                scale = object->scl[1];
            }
            if (object->scl[2] > scale) {
                scale = object->scl[2];
            }
            scale = (scale * object->model->r) + GROUND_REACH_PAST;
            if (distance > scale) {
                continue;
            }
        }
        if (polygon_find_heights(&probe, objects[i].object, objects[i].flags)) {
            if (objects[i].owner != NULL) {
                force = objects[i].owner->force.p;
                if (force != NULL) {
                    probe.above.pos += force->speed.y;
                    probe.below.pos += force->speed.y;
                }
            }
            if (probe.above.found == TRUE &&
                (!(ground->above_flags & WORLD_FLAT) ||
                 stage_current_id() == STAGE_RELAY_SONIC) &&
                probe.pos.y < probe.above.pos &&
                probe.above.pos < ground->above_y) {
                ground->above_y = probe.above.pos;
                ground->above_flags = objects[i].flags;
            }
            if (probe.below.found == TRUE && probe.pos.y >= probe.below.pos &&
                probe.below.pos > ground->below_y) {
                ground->below_y = probe.below.pos;
                ground->below_flags = objects[i].flags;
                ground->below_ang_x = probe.below.ang_x;
                ground->below_ang_z = probe.below.ang_z;
            }
        }
    }
}

void surface_probe_player(struct task_work *work, struct player *player)
{
    struct ground_probe *ground;
    NJS_POINT3 pos;
    NJS_POINT3 ahead;

    ground = &player->ground;
    if (ground->above_flags & WORLD_FLAT) {
        /* @hack spelling: the angles in a block of their own, where one
         * struct angles for the function would do */
        struct angles ang;

        /* @hack spelling: ahead, the ripple's place here, where a point of
         * its own would do */
        ahead.x = work->pos.x;
        ahead.y = ground->above_y;
        ahead.z = work->pos.z;
        ang.x = ground->below_ang_x;
        ang.y = work->ang.y;
        ang.z = ground->below_ang_z;
        /* @hack spelling: the angles passed by the address of their first
         * member, where &ang would do */
        player_ripple(player->slot, &ahead, (struct angles *)&ang.x);
        if (!(ground->last_above_flags & WORLD_FLAT)) {
            player_splash(player->slot, &ahead, NULL);
        }
    }
    world_list_near(work->pos.x, work->pos.y, work->pos.z, PLAYER_REACH);
    pos = work->pos;
    surface_probe_ground(&pos, ground);
    ground = &player->ahead;
    ahead.x = player->consts.radius + AHEAD_PAST;
    ahead.y = player->consts.height + AHEAD_PAST;
    ahead.z = 0.0F;
    {
        struct angles ang;

        ang = work->ang;
        player_read_stick(work, player, &ang.y, NULL);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateZ(NULL, ang.z);
        njRotateX(NULL, ang.x);
        njRotateY(NULL, -ang.y);
    }
    njCalcVector(NULL, &ahead, &ahead);
    njPopMatrixEx();
    njAddVector(&pos, &ahead);
    surface_probe_ground(&pos, ground);
}

int surface_probe_sides(struct surface_probe *probe)
{
    Uint16 i;
    struct world_object *objects;
    int any;
    NJS_VECTOR away;
    struct surface_probe found;
    NJS_OBJECT *object;
    Float distance;
    Float scale;
    Float was;
    Float now;
    int side;
    NJS_POINT3 *pos;

    any = 0;
    pos = &probe->pos;
    found.pos = probe->pos;
    for (side = 0; side < SURFACE_SIDES; side++) {
        probe->sides[side].found = FALSE;
    }
    probe->sides[SURFACE_NEG_X].pos = NO_FLOOR;
    probe->sides[SURFACE_POS_X].pos = -NO_FLOOR;
    probe->sides[SURFACE_NEG_Y].pos = NO_FLOOR;
    probe->sides[SURFACE_POS_Y].pos = -NO_FLOOR;
    probe->sides[SURFACE_NEG_Z].pos = NO_FLOOR;
    probe->sides[SURFACE_POS_Z].pos = -NO_FLOOR;
    objects = world_objects;
    world_list_near(pos->x, pos->y, pos->z, SIDES_REACH);
    for (i = 0; i < world_count; i++) {
        object = objects[i].object;
        if (objects[i].owner != NULL) {
            away.x = object->pos[0] - pos->x;
            away.y = object->pos[1] - pos->y;
            away.z = object->pos[2] - pos->z;
            distance = njScalor(&away);
            scale = object->scl[0];
            if (object->scl[1] > scale) {
                scale = object->scl[1];
            }
            if (object->scl[2] > scale) {
                scale = object->scl[2];
            }
            if (distance > scale * object->model->r) {
                continue;
            }
        }
        if (objects[i].flags & WORLD_FLAT) {
            continue;
        }
        if (polygon_find_sides(&found, objects[i].object, objects[i].flags)) {
            any = 1;
            if (found.sides[SURFACE_NEG_X].found) {
                was = njAbs(pos->x - probe->sides[SURFACE_NEG_X].pos);
                now = njAbs(pos->x - found.sides[SURFACE_NEG_X].pos);
                if (now < was) {
                    probe->sides[SURFACE_NEG_X] = found.sides[SURFACE_NEG_X];
                }
            }
            if (found.sides[SURFACE_POS_X].found) {
                was = njAbs(probe->sides[SURFACE_POS_X].pos - pos->x);
                now = njAbs(found.sides[SURFACE_POS_X].pos - pos->x);
                if (was > now) {
                    probe->sides[SURFACE_POS_X] = found.sides[SURFACE_POS_X];
                }
            }
            if (found.sides[SURFACE_NEG_Y].found) {
                was = njAbs(pos->y - probe->sides[SURFACE_NEG_Y].pos);
                now = njAbs(pos->y - found.sides[SURFACE_NEG_Y].pos);
                if (was > now) {
                    probe->sides[SURFACE_NEG_Y] = found.sides[SURFACE_NEG_Y];
                }
            }
            if (found.sides[SURFACE_POS_Y].found) {
                was = njAbs(probe->sides[SURFACE_POS_Y].pos - pos->y);
                now = njAbs(found.sides[SURFACE_POS_Y].pos - pos->y);
                if (now < was) {
                    probe->sides[SURFACE_POS_Y] = found.sides[SURFACE_POS_Y];
                }
            }
            if (found.sides[SURFACE_NEG_Z].found) {
                was = njAbs(pos->z - probe->sides[SURFACE_NEG_Z].pos);
                now = njAbs(pos->z - found.sides[SURFACE_NEG_Z].pos);
                if (was > now) {
                    probe->sides[SURFACE_NEG_Z] = found.sides[SURFACE_NEG_Z];
                }
            }
            if (found.sides[SURFACE_POS_Z].found) {
                was = njAbs(probe->sides[SURFACE_POS_Z].pos - pos->z);
                now = njAbs(found.sides[SURFACE_POS_Z].pos - pos->z);
                if (was > now) {
                    probe->sides[SURFACE_POS_Z] = found.sides[SURFACE_POS_Z];
                }
            }
        }
    }
    return any;
}

int surface_probe_sides_flat(struct surface_probe *probe)
{
    Uint16 i;
    struct world_object *objects;
    int took_flat;
    NJS_VECTOR away;
    struct surface_probe found;
    NJS_OBJECT *object;
    Float distance;
    Float scale;
    Float was;
    Float now;
    int side;
    NJS_POINT3 *pos = &probe->pos;

    took_flat = 0;
    found.pos = probe->pos;
    for (side = 0; side < SURFACE_SIDES; side++) {
        probe->sides[side].found = FALSE;
    }
    probe->sides[SURFACE_NEG_X].pos = NO_FLOOR;
    probe->sides[SURFACE_POS_X].pos = -NO_FLOOR;
    probe->sides[SURFACE_NEG_Y].pos = NO_FLOOR;
    probe->sides[SURFACE_POS_Y].pos = -NO_FLOOR;
    probe->sides[SURFACE_NEG_Z].pos = NO_FLOOR;
    probe->sides[SURFACE_POS_Z].pos = -NO_FLOOR;
    objects = world_objects;
    world_list_near(pos->x, pos->y, pos->z, SIDES_REACH);
    for (i = 0; i < world_count; i++) {
        object = objects[i].object;
        if (objects[i].owner != NULL) {
            away.x = object->pos[0] - pos->x;
            away.y = object->pos[1] - pos->y;
            away.z = object->pos[2] - pos->z;
            distance = njScalor(&away);
            scale = object->scl[0];
            if (object->scl[1] > scale) {
                scale = object->scl[1];
            }
            if (object->scl[2] > scale) {
                scale = object->scl[2];
            }
            if (distance > scale * object->model->r) {
                continue;
            }
        }
        if (polygon_find_sides(&found, objects[i].object, objects[i].flags)) {
            if (found.sides[SURFACE_NEG_X].found) {
                was = njAbs(pos->x - probe->sides[SURFACE_NEG_X].pos);
                now = njAbs(pos->x - found.sides[SURFACE_NEG_X].pos);
                if (now < was) {
                    probe->sides[SURFACE_NEG_X] = found.sides[SURFACE_NEG_X];
                }
            }
            if (found.sides[SURFACE_POS_X].found) {
                was = njAbs(probe->sides[SURFACE_POS_X].pos - pos->x);
                now = njAbs(found.sides[SURFACE_POS_X].pos - pos->x);
                if (was > now) {
                    probe->sides[SURFACE_POS_X] = found.sides[SURFACE_POS_X];
                }
            }
            if (found.sides[SURFACE_NEG_Y].found) {
                was = njAbs(pos->y - probe->sides[SURFACE_NEG_Y].pos);
                now = njAbs(pos->y - found.sides[SURFACE_NEG_Y].pos);
                if (was > now) {
                    probe->sides[SURFACE_NEG_Y] = found.sides[SURFACE_NEG_Y];
                    if (objects[i].flags & WORLD_FLAT) {
                        took_flat = 1;
                    }
                }
            }
            if (found.sides[SURFACE_POS_Y].found) {
                was = njAbs(probe->sides[SURFACE_POS_Y].pos - pos->y);
                now = njAbs(found.sides[SURFACE_POS_Y].pos - pos->y);
                if (now < was) {
                    probe->sides[SURFACE_POS_Y] = found.sides[SURFACE_POS_Y];
                }
            }
            if (found.sides[SURFACE_NEG_Z].found) {
                was = njAbs(pos->z - probe->sides[SURFACE_NEG_Z].pos);
                now = njAbs(pos->z - found.sides[SURFACE_NEG_Z].pos);
                if (was > now) {
                    probe->sides[SURFACE_NEG_Z] = found.sides[SURFACE_NEG_Z];
                }
            }
            if (found.sides[SURFACE_POS_Z].found) {
                was = njAbs(probe->sides[SURFACE_POS_Z].pos - pos->z);
                now = njAbs(found.sides[SURFACE_POS_Z].pos - pos->z);
                if (was > now) {
                    probe->sides[SURFACE_POS_Z] = found.sides[SURFACE_POS_Z];
                }
            }
        }
    }
    return took_flat;
}
