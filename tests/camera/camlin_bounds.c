/* camlin.c's reads at the ends of what it was given: a table's walks, and
 * camlin_path_point over a path longer than CAMLIN_PATH_POINTS */
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "camera/camlin.c"

/* a camlin area's bytes, and the whole entries they hold */
#define TABLE_BYTES ((size_t)(CAMLIN_AREA_1 - CAMLIN_AREA_0))
#define TABLE_ENTRIES (TABLE_BYTES / sizeof(struct camlin_node))

/* the nodes check_path's path runs through. with the first and last twice
 * and CAMLIN_NONE, the path fills MARK_PATH_ROOM entries */
#define PATH_NODES (MARK_PATH_ROOM - 3)

static struct camlin_node path_nodes[PATH_NODES];

Float njDistanceP2P(NJS_POINT3 *p1, NJS_POINT3 *p2)
{
    Float x = p1->x - p2->x;
    Float y = p1->y - p2->y;
    Float z = p1->z - p2->z;

    return sqrtf((x * x) + (y * y) + (z * z));
}

/* tables 1 and 3 lie end to end in one heap block, as CAMLIN_AREA_0 and
 * CAMLIN_AREA_1 do. past is the entry after table 1's whole entries, and
 * table 3's first bytes complete it. it lies at pos and is marked used, so a
 * walk that takes it finds it. table 3's past entry runs out of the block,
 * so a walk that reads it fails under the sanitizers */
static void check_tables(void)
{
    Uint8 *area = calloc(2, TABLE_BYTES);
    struct camlin_node *past;
    NJS_POINT3 pos = {1.0F, 2.0F, 3.0F};

    assert(area != NULL);
    camlin_tables[1] = (struct camlin_node *)area;
    camlin_tables[2] = (struct camlin_node *)(area + TABLE_BYTES);
    camlin_tables[3] = camlin_tables[2];
    camlin_tables[4] = (struct camlin_node *)(area + (2 * TABLE_BYTES));
    camlin_tables[5] = camlin_tables[4];
    past = &camlin_tables[1][TABLE_ENTRIES];
    past->player_pos = pos;
    past->camera_pos = pos;
    past->used = TRUE;
    assert(camlin_nearest(1, &pos) == NULL);
    assert(camlin_nearest_camera(1, &pos) == NULL);
    assert(camlin_nearest(3, &pos) == NULL);
    assert(camlin_nearest_camera(3, &pos) == NULL);
    free(area);
}

/* path_nodes[i] lies at x = i. the B-spline at a key weighs it 4 to 1
 * against each neighbor, so the point at node i's place lies at x = i */
static void check_path(void)
{
    Sint16 path[MARK_PATH_ROOM];
    NJS_POINT3 out;
    int count;
    int i;

    curve_weights_init();
    camlin_tables[1] = path_nodes;
    count = 0;
    path[count++] = 0;
    for (i = 0; i < PATH_NODES; i++) {
        path_nodes[i].camera_pos.x = (Float)i;
        path[count++] = i;
    }
    path[count++] = PATH_NODES - 1;
    path[count] = CAMLIN_NONE;
    assert(count == MARK_PATH_ROOM - 1);
    camlin_path_point(path, 41.0F, &out, 1);
    assert(out.x > 39.999F && out.x < 40.001F);
    assert(out.y == 0.0F && out.z == 0.0F);
}

int main(void)
{
    check_tables();
    check_path();
    return 0;
}
