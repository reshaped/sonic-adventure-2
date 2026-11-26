/* A moving object to mesh where the pool has no mesh left:
 * collision_mesh_update makes no mesh, and writes through no null one. */
#include <assert.h>

#include "collision/polygon.c"

struct game_state game_state;
struct collision_mesh *collision_mesh_list;
struct world_object *world_objects;
Sint16 world_count;
NJS_MATRIX _nj_unit_matrix_;
static int allocs;
static int clears;

/* the pool is out of meshes */
struct collision_mesh *collision_mesh_alloc(void)
{
    allocs++;
    return NULL;
}

void collision_mesh_free(struct collision_mesh *mesh)
{
    assert(0);
}

void collision_mesh_clear_buckets(void)
{
    clears++;
}

void collision_mesh_take_buckets(struct collision_mesh *mesh)
{
    assert(mesh != NULL);
}

struct polygon *polygon_alloc(Float z)
{
    return NULL;
}

Float vector_length2(Float x, Float y, Float z)
{
    return (x * x) + (y * y) + (z * z);
}

void world_collect_near_players(void)
{
}

Bool njPushMatrix(NJS_MATRIX *m)
{
    return TRUE;
}

Bool njPushMatrixEx(void)
{
    return TRUE;
}

Bool njPopMatrixEx(void)
{
    return TRUE;
}

void njTranslateEx(NJS_VECTOR *v)
{
}

void njRotateX(NJS_MATRIX *m, Angle ang)
{
}

void njRotateY(NJS_MATRIX *m, Angle ang)
{
}

void njRotateZ(NJS_MATRIX *m, Angle ang)
{
}

void njScaleEx(NJS_VECTOR *v)
{
}

void njCalcPoint(NJS_MATRIX *m, NJS_POINT3 *ps, NJS_POINT3 *pd)
{
    *pd = *ps;
}

void njPrintC(Int loc, const char *s)
{
}

void njPrintColor(Uint32 c)
{
}

void njPrintD(Int loc, Int val, Int digit)
{
}

int main(void)
{
    static NJS_MODEL model;
    static NJS_OBJECT object;
    static struct world_object entry;

    object.model = &model;
    object.scl[0] = object.scl[1] = object.scl[2] = 1.0F;
    entry.flags = WORLD_MOVING;
    entry.object = &object;
    world_objects = &entry;
    world_count = 1;
    collision_mesh_update();
    assert(allocs == 1 && clears == 0);
    return 0;
}
