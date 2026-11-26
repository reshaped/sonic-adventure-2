#include <ninja.h>
#include <sg_xpt.h>

#include "chao/course.h"
#include "chao/garden.h"
#include "chao/ground.h"
#include "chao/work.h"
#include "collision/mesh.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/lines.h"
#include "object/mover.h"
#include "system/task.h"

/* garden_floor_below looks among the world's objects within BELOW_REACH, and
 * garden_floor_angles within ANGLES_REACH */
#define BELOW_REACH 10.0F
#define ANGLES_REACH 300.0F
/* a cell of the height map holds HEIGHT_PRESENT and a height under
 * HEIGHT_MASK. the height is the floor's plus HEIGHT_OFFSET, in steps of
 * HEIGHT_STEP. a cell without a floor holds 0, which CELL_HEIGHT reads as
 * -HEIGHT_OFFSET */
#define HEIGHT_MASK 0x3fff
#define HEIGHT_PRESENT 0x4000
#define HEIGHT_OFFSET 100.0F
#define HEIGHT_STEP 0.04F
#define STEPS_PER_UNIT 25.0F
/* the map's samples lie SAMPLE_SPACING apart, from MAP_MIN to MAP_MAX on x
 * and z. the build takes them from BUILD_START, just short of MAP_MIN. it
 * probes each from PROBE_HEIGHT. a height below FLOOR_FOUND_MIN is no floor
 * found */
#define MAP_HALF 200.0F
#define MAP_MIN (-MAP_HALF)
#define MAP_MAX MAP_HALF
#define SAMPLE_SPACING 1.0F
#define SAMPLES_PER_UNIT (1.0F / SAMPLE_SPACING)
#define BUILD_START (-200.1F)
#define PROBE_HEIGHT 20.0F
#define FLOOR_FOUND_MIN (-5000.0F)

typedef Uint16 height_row[GARDEN_HEIGHT_SIDE];
static Uint32 drawn_cell = 0;

Float garden_floor_below(NJS_POINT3 *position)
{
    struct height_probe probe;
    Float highest = GARDEN_NO_NEARBY_FLOOR;
    Float floor;
    int i;

    world_list_near(position->x, position->y, position->z, BELOW_REACH);
    probe.pos.x = position->x;
    probe.pos.y = position->y;
    probe.pos.z = position->z;
    for (i = 0; i < world_count; i++) {
        if (polygon_find_heights(&probe, world_objects[i].object, 0) &&
            probe.below.found) {
            floor = probe.below.pos;
            if (floor > highest) {
                highest = floor;
            }
        }
    }
    return highest;
}

Float garden_floor_angles(Float x, Float y, Float z, struct angles *angles)
{
    struct height_probe probe;
    NJS_VECTOR offset;
    Angle tilt_x;
    Angle tilt_z;
    struct world_object *objects;
    NJS_OBJECT *object;
    struct task_force *force;
    Uint16 i;
    Float highest;

#ifdef NONMATCHING
    /* this side starts the tilts at 0, so that no path reads them unset */
    tilt_x = 0;
    tilt_z = 0;
#endif
    probe.pos.x = x;
    probe.pos.y = y;
    probe.pos.z = z;
    objects = world_objects;
    world_list_near(x, y, z, ANGLES_REACH);
    highest = NO_FLOOR;
    for (i = 0; i < world_count; i++) {
        object = objects[i].object;
        if (objects[i].owner) {
            Float scale;
            Float distance;

            offset.x = object->pos[0] - x;
            offset.y = 0.0F;
            offset.z = object->pos[2] - z;
            distance = njScalor(&offset);
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
                if (objects[i].owner) {
                    if ((force = objects[i].owner->force.p) != NULL) {
                        probe.above.pos += force->speed.y;
                        probe.below.pos += force->speed.y;
                    }
                }
                if (probe.above.found == TRUE && y >= probe.above.pos &&
                    probe.above.pos > highest) {
                    highest = probe.above.pos;
                    tilt_x = probe.above.ang_x;
                    tilt_z = probe.above.ang_z;
                }
                if (probe.below.found == TRUE && y >= probe.below.pos &&
                    probe.below.pos > highest) {
                    highest = probe.below.pos;
                    tilt_x = probe.below.ang_x;
                    tilt_z = probe.below.ang_z;
                }
            }
        }
    }
    /* @bug uninitialized: where no surface was found, the stores from
     * 0x0c502f2c write tilt_x and tilt_z, which nothing set */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: where no surface was found, it
     * leaves angles as they were */
    if (angles && highest != NO_FLOOR) {
#else
    if (angles) {
#endif
        angles->x = tilt_x;
        angles->z = tilt_z;
    }
    return highest;
}

void garden_build_height_map(void)
{
    struct angles tilt;
    Float x = BUILD_START;
    Float z;
    Float height;
    int encoded;
    int ix;
    int iz;

    njPrintC(NJM_LOCATION(20, 15), "PLEASE WAIT");
    for (ix = 0; ix < GARDEN_HEIGHT_SIDE; ix++) {
        z = BUILD_START;
        for (iz = 0; iz < GARDEN_HEIGHT_SIDE; iz++) {
            height = garden_floor_angles(x, PROBE_HEIGHT, z, &tilt);
            if (height < FLOOR_FOUND_MIN) {
                encoded = 0;
            } else {
                encoded = ((int)((height + HEIGHT_OFFSET) * STEPS_PER_UNIT) &
                           HEIGHT_MASK) |
                          HEIGHT_PRESENT;
            }
            ((height_row *)garden_height_map)[ix][iz] = encoded;
            z += SAMPLE_SPACING;
        }
        x += SAMPLE_SPACING;
    }
}

/* @bug memory: the map is one cell, GARDEN_HEIGHT_SIDE squared. the reads
 * from 0x0c5030aa take cell ix * GARDEN_HEIGHT_SIDE + iz, for ix and iz up to
 * 401, far past it */
#ifdef NONMATCHING
/* this side repairs the bug tagged above. it reads every cell but the first as
 * 0, a cell without a floor */
#define MAP_CELL(x, z) ((x) == 0 && (z) == 0 ? garden_height_map[0] : 0)
#else
#define MAP_CELL(x, z) (((height_row *)&garden_height_map[0])[(x)][(z)])
#endif
#define CELL_HEIGHT(cell)                                                      \
    ((((cell) & HEIGHT_MASK) * HEIGHT_STEP) - HEIGHT_OFFSET)

void garden_sample_height_map(Float x, Float z, NJS_POINT2 *height)
{
    Float corner[4];
    Float dx;
    Float dz;
    Float near_edge;
    Float far_edge;
    Uint32 ix;
    Uint32 iz;

    if (x < MAP_MIN) {
        x = MAP_MIN;
    } else if (x > MAP_MAX) {
        x = MAP_MAX;
    }
    if (z < MAP_MIN) {
        z = MAP_MIN;
    } else if (z > MAP_MAX) {
        z = MAP_MAX;
    }
    dx = (x + MAP_HALF) * SAMPLES_PER_UNIT;
    dz = (z + MAP_HALF) * SAMPLES_PER_UNIT;
    ix = (int)dx;
    iz = (int)dz;
    corner[0] = CELL_HEIGHT(MAP_CELL(ix, iz));
    corner[1] = CELL_HEIGHT(MAP_CELL(ix + 1, iz));
    corner[2] = CELL_HEIGHT(MAP_CELL(ix, iz + 1));
    corner[3] = CELL_HEIGHT(MAP_CELL(ix + 1, iz + 1));
    dx = njFraction(dx);
    dz = njFraction(dz);
    near_edge = (corner[1] * dx) + ((1.0F - dx) * corner[0]);
    far_edge = (corner[3] * dx) + ((1.0F - dx) * corner[2]);
    height->y = (dz * far_edge) + (near_edge * (1.0F - dz));
}

/* the length of the line that shows a sample */
#define SAMPLE_LINE 2.0F
void garden_draw_height_map(void)
{
    NJS_POINT3 base;
    NJS_POINT3 top;

    njPrintC(NJM_LOCATION(20, 10), "PLEASE WAIT");
    njWaitVSync();
    if (drawn_cell++ > 0) {
        drawn_cell = 0;
    }
    /* @bug memory: drawn_cell alternates between 1 and 0, and on 1 the read
     * at 0x0c5031ce takes a cell past the map's one */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it reads the cell by MAP_CELL */
    base.y = CELL_HEIGHT(MAP_CELL(drawn_cell, 0));
#else
    base.y = CELL_HEIGHT(garden_height_map[drawn_cell]);
#endif
    base.x = (drawn_cell * SAMPLE_SPACING) - MAP_HALF;
    base.z = MAP_MIN;
    top = base;
    top.y += SAMPLE_LINE;
    line_draw_points(&base, &top);
}

Float garden_map_floor(NJS_POINT3 *position)
{
    NJS_POINT2 height;
    garden_sample_height_map(position->x, position->z, &height);
    return height.y;
}

#pragma inline(clear_mover_contacts, set_water_height)
static void clear_mover_contacts(struct mover *mover)
{
    mover->flags &= ~0x100;
    mover->flags &= ~0x200;
    mover->flags &= ~MOVER_ON_FLOOR;
    mover->flags &= ~MOVER_AT_WALL;
    mover->flags &= ~0x1000;
    mover->flags &= ~MOVER_FACING_WALL;
}

/* the height of each garden's water */
#define NEUTRAL_WATER (-2.0F)
#define HERO_WATER (-1.5F)
#define DARK_WATER (-1.0F)
static void set_water_height(struct mover *mover)
{
    switch (chao_current_area()) {
    case CHAO_AREA_NEUTRAL:
        mover->flat_y = NEUTRAL_WATER;
        break;
    case CHAO_AREA_HERO:
        mover->flat_y = HERO_WATER;
        break;
    case CHAO_AREA_DARK:
        mover->flat_y = DARK_WATER;
        break;
    }
}

int chao_meet_floor(struct task *chao)
{
    struct mover *mover = chao->motion.p;
    struct task_work *base = chao->work;
    struct chao_work *work = (struct chao_work *)base;
    NJS_POINT3 *position = &base->pos;
    NJS_POINT2 height;
    Float floor;

    if (mover->flags & MOVER_RESTING) {
        /* @hack implicit: a bare return, whose result is the flags SHC's
         * test of MOVER_RESTING leaves, where return mover->flags would do */
#ifdef NONMATCHING
        /* this side returns the mover's flags, which SHC's code leaves in R0 */
        return mover->flags;
#else
        /* NOLINTNEXTLINE(clang-diagnostic-return-mismatch) */
        return;
#endif
    }
    clear_mover_contacts(mover);
    switch (work->ground_mode) {
    case CHAO_GROUND_COLLISION:
        mover_meet_stage(chao);
        break;
    case CHAO_GROUND_HEIGHT_MAP:
        garden_sample_height_map(position->x, position->z, &height);
        floor = height.y;
        if (position->y < floor) {
            /* @hack spelling: the flag set through chao->work, where base
             * would do */
            chao->work->flags |= MOVER_WORK_ON_FLOOR;
            mover->flags |= MOVER_ON_FLOOR;
            mover->spd.x *= mover->friction;
            mover->spd.z *= mover->friction;
            position->y = floor;
            return TRUE;
        }
        break;
    case CHAO_GROUND_RACE:
        if (work->flags & CHAO_RACE_STAGE_COLLISION) {
            mover_meet_stage(chao);
        }
        chao_follow_course_floor(chao);
        break;
    }
    set_water_height(mover);
    return FALSE;
}

int garden_meet_floor(struct task *object)
{
    struct mover *mover = object->motion.p;
    if (mover->flags & MOVER_RESTING) {
        /* @hack implicit: a bare return, as chao_meet_floor's, where return
         * mover->flags would do */
#ifdef NONMATCHING
        /* this side returns the mover's flags, which SHC's code leaves in R0 */
        return mover->flags;
#else
        /* NOLINTNEXTLINE(clang-diagnostic-return-mismatch) */
        return;
#endif
    }
    clear_mover_contacts(mover);
    mover_meet_stage(object);
    set_water_height(mover);
    return FALSE;
}

void chao_set_ground_mode(struct task *chao, int mode)
{
    struct chao_work *work = CHAO_WORK(chao);
    work->ground_mode = mode;
}
