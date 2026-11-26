/* Exercise both ways a Final Rush rock enters the task manager. */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <shinobi.h>

#include "collision/collision.h"
#include "effect/burst.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/flash.h"
#include "sound/sound.h"
#include "stg30/rockfall.h"
#include "stg30/rockfall_art.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"
#include "system/timer.h"

struct game_state game_state;
struct task_work *player_work[PLAYER_SLOTS];
struct player *player_records[PLAYER_SLOTS];
NJS_CNK_OBJECT boulder_object, shard_a_object, shard_b_object, shard_c_object;
NJS_TEXLIST boulder_texlist, shard_a_texlist, shard_b_texlist, shard_c_texlist;

static int live_blocks, fail_after, probes, depth;
static struct task_work runner;
static struct player record;
static struct task *constructing;

void *syMalloc(Uint32 size)
{
    void *block = malloc(size);
    assert(block != NULL);
    live_blocks++;
    return block;
}

void *syCalloc(Uint32 count, Uint32 size)
{
    void *block;
    if (constructing != NULL) {
        assert(constructing->draw == NULL && constructing->destructor == NULL);
        assert(constructing->step == rockfall_init && rockfall_count == 0);
    }
    if (fail_after && --fail_after == 0) {
        return NULL;
    }
    block = calloc(count, size);
    assert(block != NULL);
    live_blocks++;
    return block;
}

void syFree(void *block)
{
    assert(block != NULL && live_blocks > 0);
    live_blocks--;
    free(block);
}

void sound_detach_task(const struct task *task)
{
    assert(task->step == NULL);
}

void njMemCopy(void *to, const void *from, Sint32 length)
{
    memcpy(to, from, (size_t)length);
}

Float njSqrt(Float value)
{
    return (Float)sqrt(value);
}

Float njScalor(NJS_VECTOR *vector)
{
    return njSqrt(vector->x * vector->x + vector->y * vector->y +
                  vector->z * vector->z);
}

int surface_probe_sides(struct surface_probe *probe)
{
    probes++;
    if (constructing->work->scl.x == 0.0F) {
        assert(probe->pos.x == runner.pos.x && probe->pos.z == runner.pos.z);
        assert(probe->pos.y == runner.pos.y + 10.0F);
    } else {
        assert(probe->pos.x == constructing->work->pos.x);
        assert(probe->pos.y == constructing->work->pos.y + 30.0F);
        assert(probe->pos.z == constructing->work->pos.z);
    }
    return 0;
}

int player_near(NJS_POINT3 *pos, Float radius)
{
    return 0;
}

Bool njPushMatrixEx(void)
{
    depth++;
    return TRUE;
}

Bool njPopMatrixEx(void)
{
    assert(depth > 0);
    depth--;
    return TRUE;
}

void njCalcVector(NJS_MATRIX *matrix, NJS_VECTOR *vector, NJS_VECTOR *result)
{
    *result = *vector;
}

void njUnitMatrix(NJS_MATRIX *matrix)
{
}
void njRotateXYZ(NJS_MATRIX *matrix, Angle x, Angle y, Angle z)
{
}
void njRotateX(NJS_MATRIX *matrix, Angle angle)
{
}
void njRotateZ(NJS_MATRIX *matrix, Angle angle)
{
}

/* The cases below stop before movement or drawing. */
void njCalcPoint(NJS_MATRIX *matrix, NJS_POINT3 *point, NJS_POINT3 *result)
{
    abort();
}
void njRotateY(NJS_MATRIX *matrix, Angle angle)
{
    abort();
}
void njRotateEx(Angle *angles, Sint32 order)
{
    abort();
}
void njMultiMatrix(NJS_MATRIX *matrix, NJS_MATRIX *source)
{
    abort();
}
void njGetMatrix(NJS_MATRIX *matrix)
{
    abort();
}
void njSetMatrix(NJS_MATRIX *matrix, NJS_MATRIX *source)
{
    abort();
}
void njTranslate(NJS_MATRIX *matrix, Float x, Float y, Float z)
{
    abort();
}
void njTranslateEx(NJS_VECTOR *vector)
{
    abort();
}
void njFogDisable(void)
{
    abort();
}
void njFogEnable(void)
{
    abort();
}
void njCnkEasyDrawModel(NJS_CNK_MODEL *model)
{
    abort();
}
void njDirectDrawObject(NJS_DIRECT_HEAD *model)
{
    abort();
}
Sint32 njSetTexture(NJS_TEXLIST *textures)
{
    abort();
}
void current_matrix_angles_xyz(Angle *x, Angle *y, Angle *z)
{
    abort();
}
Bool sphere_in_front_within(NJS_POINT3 *center, Float radius, Float reach)
{
    abort();
}
void flash_manager_create_sparks_depth(Float x, Float y, Float z, Float scale,
                                       Uint8 sparks, Float depth)
{
    abort();
}
struct particle *burst_create_wobbling(NJS_POINT3 *pos, NJS_VECTOR *velocity,
                                       Float scale)
{
    abort();
}

static struct task *new_rock(void)
{
    struct task *task = task_create(TASK_WORK, 2, rockfall_init, "ObjectBurst");
    assert(task != NULL && task_levels[2] == task);
    assert(live_blocks == 2 && rockfall_count == 0);
    return task;
}

static void expect_empty(void)
{
    assert(task_levels[2] == NULL && rockfall_count == 0 && live_blocks == 0);
    assert(depth == 0);
}

static void rejected(int failure, Bool as_placement)
{
    struct task *rock = new_rock();
    int old_probes = probes;
    constructing = rock;
    fail_after = failure;
    if (as_placement) {
        rock->step(rock);
    } else {
        assert(!rockfall_attach(rock));
    }
    constructing = NULL;
    assert(rock->step == task_destroy && !rock->draw && !rock->destructor);
    assert(!rock->work->collision && !rock->motion.p && live_blocks == 2);
    assert(rockfall_count == 0 && probes == old_probes && fail_after == 0);
    rock->step(rock);
    expect_empty();
}

static void accepted(Bool shard)
{
    struct task *rock = new_rock();
    int old_probes = probes;
    constructing = rock;
    if (shard) {
        rock->work->scl.x = 1.0F;
        assert(rockfall_attach(rock));
    } else {
        rock->step(rock);
    }
    constructing = NULL;
    assert(rock->draw && rock->destructor && rockfall_count == 1);
    assert(rock->step != rockfall_init && rock->step != task_destroy);
    assert(rock->motion.p && live_blocks == 5 && probes == old_probes + 1);
    assert(ROCKFALL(rock)->floor.y == record.ground.below_y);
    assert(rock->work->collision->owner == rock);
    assert(rock->work->collision->count == 1);
    assert(rock->work->collision->shapes->form ==
           (shard ? SHAPE_SPHERE : SHAPE_BOX2));
    assert(rock->work->collision->shapes->radius ==
           njScalor(&ROCKFALL(rock)->size));
    rock->step(rock); /* player is present, but outside the wake radius */
    assert(rock->work->mode == 0 && rock->step != task_destroy);
    if (shard) {
        player_work[0] = NULL;
    } else {
        player_records[0] = NULL;
    }
    rock->step(rock);
    assert(rock->step == task_destroy && rockfall_count == 1);
    rock->step(rock);
    expect_empty();
    player_work[0] = &runner;
    player_records[0] = &record;
}

int main(int argc, char **argv)
{
    int allocation;
    (void)argv;
    runner.pos.x = 50.0F;
    runner.pos.y = 1000.0F;
    runner.pos.z = 8000.0F;
    record.ground.below_y = 900.0F;
    player_records[0] = &record;
    rejected(0, FALSE);
    player_work[0] = &runner;
    player_records[0] = NULL;
    rejected(0, TRUE);
    player_records[0] = &record;
    if (argc == 2) {
        rejected(0, FALSE);
        rejected(0, TRUE);
        return 0;
    }
    /* Actor motion, collision record, and its shape copy. */
    for (allocation = 1; allocation <= 3; allocation++) {
        rejected(allocation, FALSE);
        rejected(allocation, TRUE);
    }
    accepted(TRUE);
    accepted(FALSE);
    return 0;
}
