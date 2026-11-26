#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "boss_bigbogy/sway.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* the high half of a chunk's word, sign-extended. in a chunk's first word
 * it counts the words after that one. in a vertex chunk's second word it
 * counts the chunk's vertices */
/* @hack spelling: the mask before the shift, where the shift alone would do */
#define HIGH_HALF(word) ((Sint16)(((word) & 0xffff0000) >> 16))
#define UNREACHED_TEST(value)                                                  \
    /* @hack redundant: a test of value, in a block no path reaches */         \
    if (0) {                                                                   \
        if (value) {                                                           \
        }                                                                      \
    }
#define POINT_WORDS 3
#define VN_VERTEX_WORDS 6
#define VN_NF_VERTEX_WORDS 7
/* a vertex sways on or right of the edge
 * x = SWAY_ANCHOR_X + y * SWAY_EDGE_SLOPE. it sways only at x >= 0 and
 * y <= SWAY_MAX_Y */
#define SWAY_ANCHOR_X 40.0F
#define SWAY_MAX_Y 50.0F
#define SWAY_EDGE_SLOPE 3.0F

struct sway_vertex {
    Sint8 moving; /* 1 for a vertex that sways */
    Angle start;
    Angle phase;
    Float reach;
    NJS_POINT3 rest;
    Uint8 unused[12];
};
/* the task's copy of its object, with a model and vertex list of its own.
 * vertices holds a sway_vertex for each vertex of the list, in the list's
 * order */
struct sway_mesh {
    Sint16 words; /* the vertex list's length, NJD_CE's word included */
    Sint16 count; /* how many vertices the list holds */
    struct sway_vertex *vertices;
    Uint32 unused;
    NJS_CNK_OBJECT *copy;
    NJS_CNK_OBJECT *original; /* the object as bigbogy_sway_create found it */
};
/* a sway's record, in its task's motion */
struct sway_motion {
    struct sway_mesh *mesh;
    /* the caller's object, whose model the task replaces */
    NJS_CNK_OBJECT *object;
};
enum sway_parameter {
    REACH_PER_UNIT, /* a vertex's reach per unit of x past SWAY_ANCHOR_X */
    UNUSED,
    WAVELENGTH, /* the units of x over which start turns once */
    MAX_REACH
};

static Angle phase_step = 0x380;
/* @hack type: a Float array indexed by enum sway_parameter, where a struct
 * of four named Floats would do */
static Float tuning[] = {0.04F, 0.8F, 4.0F, 2.3F};

static void write_position(const NJS_POINT3 *pos, Float *out)
{
    *out++ = pos->x;
    *out++ = pos->y;
    *out = pos->z;
}

/* @hack spelling: an accessor for sway->mesh, where reading the field would
 * do */
static struct sway_mesh *motion_mesh(struct sway_motion *sway)
{
    return sway->mesh;
}

static struct sway_mesh *copy_mesh(NJS_CNK_OBJECT *object)
{
    struct sway_mesh *mesh;
    Sint32 *chunk;
    Sint32 *vlist;
    struct sway_vertex *vertices;
    struct sway_vertex *vertex;
    NJS_CNK_MODEL *model;
    NJS_CNK_OBJECT *copy;
    NJS_CNK_OBJECT *original;
    NJS_CNK_MODEL *copied_model;
    NJS_CNK_OBJECT *copied_object;
    Sint32 *first_chunk;
    Float *point;
    Sint32 header;
    Sint16 size;
    Sint16 words;
    Sint16 total;
    Sint16 count;
    Uint16 i;
    int chunk_vertices;
    Sint8 stride;
    /* @hack type: skip_bytes, a byte count added through a char pointer,
     * where point += stride - POINT_WORDS would do */
    Uint32 skip_bytes;
    Angle phase_per_unit;
    Float distance;
    NJS_POINT3 pos;

    /* @hack redundant: the addresses of copied_object and copied_model,
     * which nothing reads */
    (void)&copied_object;
    (void)&copied_model;
    total = 0;
    words = 0;
    chunk = object->model->vlist;
    while (*chunk != NJD_CE) {
#ifdef NONMATCHING
        /* the walks that follow have no stride for any other type */
        if ((Uint8)*chunk != NJD_CV_VN && (Uint8)*chunk != NJD_CV_VN_NF) {
            return NULL;
        }
#endif
        words++;
        size = HIGH_HALF(*chunk++);
        /* @hack spelling: the assignment of count inside the sum, where a
         * statement of its own would do */
        total += (count = HIGH_HALF(*chunk));
        words += size;
        chunk += size;
    }
    /* @hack redundant: the unreached test of count */
    UNREACHED_TEST(count);
    words++; /* NJD_CE's word */
    vlist = syCalloc(1, words * sizeof(*vlist));
    if (!vlist) {
        return NULL;
    }
    vertices = syCalloc(1, total * sizeof(*vertices));
    if (!vertices) {
        syFree(vlist);
        return NULL;
    }
    /* @hack spelling: each allocation below assigned inside its test, where
     * a statement of its own would do */
    if (!(model = syCalloc(1, sizeof(*model)))) {
        syFree(vertices);
        syFree(vlist);
        return NULL;
    }
    if (!(copy = syCalloc(1, sizeof(*copy)))) {
        syFree(model);
        syFree(vertices);
        syFree(vlist);
        return NULL;
    }
    if (!(original = syCalloc(1, sizeof(*original)))) {
        /* @bug memory: copy leaks where the allocation of original fails, at
         * 0x8c508a3a. copy is allocated at 0x8c508a1c */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it frees copy */
        syFree(copy);
#endif
        syFree(model);
        syFree(vertices);
        syFree(vlist);
        return NULL;
    }
    if (!(mesh = syCalloc(1, sizeof(*mesh)))) {
        /* @bug memory: original leaks where the allocation of mesh fails,
         * at 0x8c508a58. original is allocated at 0x8c508a3a */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it frees original */
        syFree(original);
#endif
        syFree(copy);
        syFree(model);
        syFree(vertices);
        syFree(vlist);
        return NULL;
    }
    mesh->original = original;
    *mesh->original = *object;
    mesh->copy = copy;
    mesh->words = words;
    mesh->count = total;
    *mesh->copy = *mesh->original;
    mesh->copy->model = model;
    *mesh->copy->model = *mesh->original->model;
    njMemCopy4(vlist, mesh->original->model->vlist, mesh->words);
    mesh->copy->model->vlist = vlist;

    /* @hack redundant: the unreached tests of vlist and copied_model */
    UNREACHED_TEST(vlist);
    UNREACHED_TEST(copied_model);
    vertex = vertices;
    /* @hack spelling: the list read through copied_object and copied_model
     * into first_chunk, where chunk = mesh->copy->model->vlist would do */
    first_chunk = (copied_model = (copied_object = mesh->copy)->model)->vlist;
    chunk = first_chunk;
    phase_per_unit = (Float)FULL_TURN / tuning[WAVELENGTH];
    while (*chunk != NJD_CE) {
        /* @hack spelling: the reads of the chunk's first two words apart
         * from their steps, where *chunk++ would do */
        header = *chunk;
        chunk++;
        count = HIGH_HALF(*chunk);
        chunk++;
        /* @hack redundant: the unreached test of chunk_vertices */
        UNREACHED_TEST(chunk_vertices);
        switch ((Uint8)header) {
        case NJD_CV_VN:
            stride = VN_VERTEX_WORDS;
            break;
        case NJD_CV_VN_NF:
            stride = VN_NF_VERTEX_WORDS;
            break;
#ifdef NONMATCHING
        default:
            /* no chunk reaches here: the first walk returns null for one of
             * another type */
            stride = VN_VERTEX_WORDS;
            break;
#endif
        }
        /* @hack redundant: two tests of chunk in a block no path reaches */
        if (0) {
            if (chunk) {
                if (chunk) {
                }
            }
        }
        point = (Float *)chunk;
        skip_bytes = (stride - POINT_WORDS) * sizeof(*point);
        chunk_vertices = (Uint16)count;
        for (i = 0; i < chunk_vertices;
             ++i, point = (Float *)((char *)point + skip_bytes)) {
            pos.x = *point++;
            pos.y = *point++;
            pos.z = *point++;
            /* @hack spelling: the test against the edge nested in the other
             * two, where one && of all three would do */
            if (pos.y <= SWAY_MAX_Y && pos.x >= 0.0F) {
                if (pos.x >= (pos.y * SWAY_EDGE_SLOPE) + SWAY_ANCHOR_X) {
                    vertex->moving = 1;
                }
            }
            if (vertex->moving == 1) {
                distance = pos.x - SWAY_ANCHOR_X;
                if (distance < 0.0F) {
                    distance = 0.0F;
                }
                vertex->start = (Uint32)distance % (Uint32)tuning[WAVELENGTH] *
                                phase_per_unit;
                vertex->phase = 0;
                /* @hack spelling: distance scaled in place into the reach,
                 * where a local of its own would do */
                distance *= tuning[REACH_PER_UNIT];
                if (distance > tuning[MAX_REACH]) {
                    distance = tuning[MAX_REACH];
                }
                vertex->reach = distance;
            }
            vertex->rest = pos;
            ++vertex;
        }
        chunk = (Sint32 *)point;
    }
    mesh->vertices = vertices;
    return mesh;
}

static void sway_destroy(struct task *task)
{
    struct sway_motion *sway = task->motion.p;
    struct sway_mesh *mesh = sway->mesh;
    NJS_CNK_MODEL *model;

    if (mesh->original) {
        *sway->object = *mesh->original;
    }
    if (mesh) {
        if (mesh->vertices) {
            syFree(mesh->vertices);
            mesh->vertices = NULL;
        }
        if (mesh->copy) {
            if (mesh->copy->model) {
                model = mesh->copy->model;
                if (model->vlist) {
                    syFree(model->vlist);
                    model->vlist = NULL;
                }
                /* @hack redundant: the test of plist before it is set to
                 * null */
                if (model->plist) {
                    model->plist = NULL;
                }
                syFree(model);
                mesh->copy->model = NULL;
            }
            syFree(mesh->copy);
            mesh->copy = NULL;
        }
        if (mesh->original) {
            syFree(mesh->original);
            mesh->original = NULL;
        }
        syFree(mesh);
        sway->mesh = NULL;
    }
}

static void sway_vertices(struct task *task)
{
    struct sway_motion *sway;
    struct sway_mesh *mesh;
    struct sway_vertex *vertex;
    NJS_CNK_MODEL *model;
    NJS_CNK_OBJECT *object;
    Sint32 *chunk;
    Sint32 *first_chunk;
    Float *point;
    Sint32 header;
    Uint16 count;
    Sint8 stride;
    Uint16 i;
    NJS_POINT3 pos;

    /* @hack redundant: the addresses of object and model, which nothing
     * reads */
    (void)&object;
    (void)&model;
    sway = task->motion.p;
    mesh = motion_mesh(sway);
    vertex = mesh->vertices;
    /* @hack spelling: the list read through object and model into
     * first_chunk, where chunk = mesh->copy->model->vlist would do */
    first_chunk = (model = (object = mesh->copy)->model)->vlist;
    /* @hack redundant: the unreached test of model */
    UNREACHED_TEST(model);
    chunk = first_chunk;
    while (*chunk != NJD_CE) {
        header = *chunk++;
        count = HIGH_HALF(*chunk++);
        switch ((Uint8)header) {
        case NJD_CV_VN:
            stride = VN_VERTEX_WORDS;
            break;
        case NJD_CV_VN_NF:
            stride = VN_NF_VERTEX_WORDS;
            break;
#ifdef NONMATCHING
        /* this side stops at a chunk of any other type. the matching side steps
         * past it by the last chunk's stride, or by an unset one */
        default:
            return;
#endif
        }
        point = (Float *)chunk;
        for (i = 0; i < count; i++) {
            /* @hack spelling: the steps past an unmoving vertex in a branch
             * of their own, where steps after the if would do */
            if (vertex->moving != 1) {
                vertex++;
                point += stride;
                continue;
            }
            pos = vertex->rest;
            pos.z += njSin(vertex->start + vertex->phase) * vertex->reach;
            vertex->phase += phase_step;
            write_position(&pos, point);
            vertex++;
            point += stride;
        }
        chunk = (Sint32 *)point;
    }
    sway->object->model = mesh->copy->model;
}

static void sway_step(struct task *task)
{
    if (!game_state.time_stopped) {
        sway_vertices(task);
    }
}

struct task *bigbogy_sway_create(struct task *parent, NJS_CNK_OBJECT *object)
{
    struct task *task;
    struct sway_motion *sway;
    if (parent) {
        task = task_create_child(TASK_WORK, sway_step, parent);
        if (!task) {
            return NULL;
        }
        sway = syCalloc(1, sizeof(*sway));
        if (!sway) {
            task_kill(task);
            return NULL;
        }
        task->motion.p = sway;
        sway->object = object;
        sway->mesh = copy_mesh(object);
        /* @bug memory: the task reads through a null mesh where copy_mesh
         * fails, at 0x8c508ef0. sway_destroy reads through it at 0x8c508cf0,
         * before its test at 0x8c508d2e. sway_vertices reads through it at
         * 0x8c508dba, and bigbogy_sway_object at 0x8c508f0c */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: where copy_mesh fails, it
         * kills the task and returns null */
        if (!sway->mesh) {
            task_kill(task);
            return NULL;
        }
#endif
        task->destructor = sway_destroy;
        return task;
    }
    /* @bug uninitialized: the call returns task unset where parent is null.
     * the branch at 0x8c508ec0 skips every assignment of it */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it returns null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

NJS_CNK_OBJECT *bigbogy_sway_object(struct task *task)
{
    struct sway_motion *sway = task->motion.p;
    struct sway_mesh *mesh = sway->mesh;
    return mesh->copy;
}
