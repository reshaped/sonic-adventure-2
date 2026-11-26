/* the dummy object, DMYOBJ in a stage's object list. it loads chunks,
 * textures and texture animations as dummy_request asks, and draws them */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "draw/lines.h"
#include "draw/texture_anim.h"
#include "object/fall.h"
#include "object/set.h"
#include "shared/dummy_object.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the bytes a texture load allocates for each texture's name.
 * njSetPvmTextureList puts the names only PVM_NAME_SIZE apart. so
 * FILENAME_SIZE less PVM_NAME_SIZE bytes a texture go unused, all at the
 * buffer's end */
#define FILENAME_SIZE 32

/* a dummy object draws a grid while its work's scl.x is 1 or more. the grid
 * lies about the work's pos, turned by its ang. one set of GRID_LINES lines
 * runs along x in the plane z = 0. two more sets lie in the plane y = 0, one
 * along x and one along z. each line runs from -GRID_HALF to GRID_HALF. a
 * set's lines lie GRID_STEP apart from -GRID_HALF, so none lies at
 * GRID_HALF */
#define SHOWS_GRID(work) ((work)->scl.x >= 1.0F)
#define GRID_LINES 20
#define GRID_STEP 10.0F
#define GRID_HALF 100.0F

#define COMMAND(word) ((Uint16)(word))
#define COMMAND_ID(word) ((Uint16)((word) >> 16))

/* a dummy_request's commands. each load reads the buffer dummy_object_buffer
 * points to. no source writes that pointer, which holds WORK_AREA. it lists
 * what it loads as resources of the command's id. a new resource takes the
 * place of the first listed resource of its id, which the load frees. it joins
 * the list's end where none has its id, and an id of 0 matches none. the heap
 * must have room for what a load allocates, unchecked */
enum dummy_command {
    DUMMY_NONE,
    /* read the buffer's chunks with njReadBinary until it returns null.
     * each chunk becomes a DUMMY_CHUNK resource of the command's id. under an
     * id other than 0, each chunk takes the place of the one before it. only
     * the last chunk then stays */
    DUMMY_LOAD_CHUNKS,
    /* load the buffer's PVM into a texture list with room for
     * dummy_request.count names. the PVM must hold no more textures than
     * that, unchecked */
    DUMMY_LOAD_TEXTURES,
    DUMMY_FREE_ALL,
    /* copy a texture animation from the buffer. the buffer holds the
     * animation's size in bytes as a Sint32, then the animation. the animation
     * holds offsets from its start in key_lengths, uvs, textures and cycles.
     * the load makes each offset that isn't 0 a pointer into the copy */
    DUMMY_LOAD_TEXTURE_ANIM
};

enum dummy_kind {
    DUMMY_TEXTURES, /* a PVM's textures, in a texture list of their own */
    DUMMY_CHUNK, /* a chunk that njReadBinary read */
    DUMMY_TEXTURE_ANIM
};

struct dummy_resource {
    struct dummy_resource *next;
    Sint32 id; /* the id of the command that loaded it */
    enum dummy_kind kind;
    union {
        struct {
            Sint32 count;
            NJS_TEXNAME *texname;
            NJS_TEXLIST texlist;
            char *filename; /* FILENAME_SIZE bytes for each name */
        } textures;
        struct {
            /* the name njReadBinary gave the chunk, such as iff_NJCM */
            Uint32 type;
            void *data;
        } chunk;
        struct texture_anim *anim;
    } as;
};

/* a datum ahead of dummy_resources, which no code reads */
static Sint32 dat_dummy_object_0 = 0;
/* the resources the dummy objects have loaded, or null */
static struct dummy_resource *dummy_resources = NULL;

static void dummy_object_destroy(struct task *task);
static void dummy_object_step(struct task *task);
static void dummy_object_draw(struct task *task);

/* free resource and what it holds, and return the resource after it */
static struct dummy_resource *
dummy_resource_free(struct dummy_resource *resource)
{
    struct dummy_resource *next = resource->next;

    switch (resource->kind) {
    /* @hack spelling: the case of DUMMY_CHUNK ahead of DUMMY_TEXTURES's, not
     * in the kinds' order */
    case DUMMY_CHUNK:
        syFree(resource->as.chunk.data);
        break;
    case DUMMY_TEXTURES:
        njReleaseTexture(&resource->as.textures.texlist);
        syFree(resource->as.textures.filename);
        syFree(resource->as.textures.texname);
        break;
    case DUMMY_TEXTURE_ANIM:
        syFree(resource->as.anim);
        break;
    }
    syFree(resource);
    return next;
}

/* append a copy of resource to dummy_resources, and return the copy */
static struct dummy_resource *
dummy_resource_append(struct dummy_resource *resource)
{
    struct dummy_resource *copy = syCalloc(1, sizeof(struct dummy_resource));
    struct dummy_resource *last;

    for (last = dummy_resources; last != NULL && last->next != NULL;
         last = last->next) {
    }
    if (last != NULL) {
        last->next = copy;
    } else {
        dummy_resources = copy;
    }
    *copy = *resource;
    copy->next = NULL;
    return copy;
}

/* swap a copy of resource for the first listed resource of its id, and free
 * that one. it returns the copy, or null where no resource matches */
static struct dummy_resource *
dummy_resource_replace(struct dummy_resource *resource)
{
    struct dummy_resource *copy = NULL;
    struct dummy_resource *prev;
    struct dummy_resource *old;
    /* @hack spelling: next, a local for dummy_resource_free's result, where
     * the result stored in copy->next would do */
    struct dummy_resource *next;

    if (resource->id != 0) {
        prev = NULL;
        for (old = dummy_resources; old != NULL; old = old->next) {
            if (old->id == resource->id) {
                break;
            }
            prev = old;
        }
        if (old != NULL) {
            copy = syCalloc(1, sizeof(struct dummy_resource));
            *copy = *resource;
            next = dummy_resource_free(old);
            copy->next = next;
            if (prev != NULL) {
                prev->next = copy;
            } else {
                dummy_resources = copy;
            }
        }
    }
    return copy;
}

void dummy_object_init(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, which leaves its four
     * strings in the object */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
        (void)"<- RULER";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = dummy_object_draw;
    task->step = dummy_object_step;
    task->destructor = dummy_object_destroy;
    work->submode = 0;
    dummy_request.ready = TRUE;
    dummy_object_count++;
}

static void dummy_object_destroy(struct task *task)
{
    /* @hack spelling: the decrement as a statement of its own, where
     * --dummy_object_count tested in place would do */
    dummy_object_count--;
    if (dummy_object_count == 0) {
        /* @hack spelling: the free inside the loop's test, where a loop
         * whose body frees would do */
        while (dummy_resources != NULL &&
               (dummy_resources = dummy_resource_free(dummy_resources)) !=
                   NULL) {
        }
    }
}

/* where dummy_request.pending is TRUE, the step first frees every resource.
 * it then serves each command written to dummy_request until pending is
 * FALSE. the game stands still meanwhile. a word that names no command stays
 * as it is */
static void dummy_object_step(struct task *task)
{
    struct dummy_resource resource;
    struct dummy_resource *at;
    struct dummy_resource *next;
    struct texture_anim *anim;
    Uint32 word;
    Uint32 id;
    Uint32 type;
    Uint32 offset;
    Sint32 size;
    Uint8 *bytes;
    void *data;
    /* @hack spelling: command, a local for COMMAND(word), where COMMAND(word)
     * switched on in place would do */
    int command;

    /* @hack spelling: an else after the return, where the code after the if
     * would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        if (dummy_request.pending) {
            /* @hack spelling: the free inside the loop's test, where a loop
             * whose body frees would do */
            while (dummy_resources != NULL &&
                   (dummy_resources = dummy_resource_free(dummy_resources)) !=
                       NULL) {
            }
        }
        while (dummy_request.pending) {
            dummy_request.busy = TRUE;
            word = dummy_request.command;
            command = COMMAND(word);
            id = COMMAND_ID(word);
            switch (command) {
            case DUMMY_LOAD_CHUNKS:
                offset = 0;
                dummy_request.ready = FALSE;
                while ((data = njReadBinary((Sint8 *)dummy_object_buffer,
                                            &offset, &type)) != NULL) {
                    resource.id = id;
                    resource.kind = DUMMY_CHUNK;
                    resource.as.chunk.data = data;
                    resource.as.chunk.type = type;
                    if (dummy_resource_replace(&resource) == NULL) {
                        dummy_resource_append(&resource);
                    }
                }
                dummy_request.command = DUMMY_NONE;
                dummy_request.ready = TRUE;
                break;
            case DUMMY_LOAD_TEXTURES:
                dummy_request.ready = FALSE;
                /* @hack redundant: a walk to the list's last resource, which
                 * nothing reads */
                for (at = dummy_resources; at != NULL && at->next != NULL;
                     at = at->next) {
                }
                resource.id = id;
                resource.kind = DUMMY_TEXTURES;
                resource.as.textures.count = dummy_request.count;
                resource.as.textures.texname =
                    syCalloc(1, dummy_request.count * sizeof(NJS_TEXNAME));
                resource.as.textures.filename =
                    syCalloc(1, dummy_request.count * FILENAME_SIZE);
                njSetPvmTextureList(
                    &resource.as.textures.texlist, resource.as.textures.texname,
                    resource.as.textures.filename, resource.as.textures.count);
                njLoadTexturePvmMemory(dummy_object_buffer,
                                       &resource.as.textures.texlist);
                if (dummy_resource_replace(&resource) == NULL) {
                    dummy_resource_append(&resource);
                }
                dummy_request.command = DUMMY_NONE;
                dummy_request.ready = TRUE;
                break;
            case DUMMY_LOAD_TEXTURE_ANIM:
                size = *(Sint32 *)dummy_object_buffer;
                bytes = dummy_object_buffer + sizeof(Sint32);
                dummy_request.ready = FALSE;
                anim = syCalloc(1, size);
                memcpy(anim, bytes, size);
                if (anim->key_lengths != NULL) {
                    anim->key_lengths =
                        (Float *)((Uint8 *)anim + (Uint32)anim->key_lengths);
                }
                if (anim->uvs != NULL) {
                    anim->uvs = (Sint16 *)((Uint8 *)anim + (Uint32)anim->uvs);
                }
                if (anim->textures != NULL) {
                    anim->textures =
                        (Sint16 *)((Uint8 *)anim + (Uint32)anim->textures);
                }
                if (anim->cycles != NULL) {
                    anim->cycles =
                        (Sint16 *)((Uint8 *)anim + (Uint32)anim->cycles);
                }
                resource.id = id;
                resource.kind = DUMMY_TEXTURE_ANIM;
                resource.as.anim = anim;
                if (dummy_resource_replace(&resource) == NULL) {
                    dummy_resource_append(&resource);
                }
                dummy_request.command = DUMMY_NONE;
                dummy_request.ready = TRUE;
                break;
            /* @hack spelling: the case of DUMMY_FREE_ALL after
             * DUMMY_LOAD_TEXTURE_ANIM's, not in the commands' order */
            case DUMMY_FREE_ALL:
                dummy_request.ready = FALSE;
                /* @hack spelling: each resource freed here, where
                 * dummy_resource_free would do */
                for (at = dummy_resources; at != NULL; at = next) {
                    next = at->next;
                    switch (at->kind) {
                    /* @hack spelling: the case of DUMMY_CHUNK ahead of
                     * DUMMY_TEXTURES's, not in the kinds' order */
                    case DUMMY_CHUNK:
                        syFree(at->as.chunk.data);
                        break;
                    case DUMMY_TEXTURES:
                        njReleaseTexture(&at->as.textures.texlist);
                        syFree(at->as.textures.filename);
                        syFree(at->as.textures.texname);
                        break;
                    case DUMMY_TEXTURE_ANIM:
                        syFree(at->as.anim);
                        /* @hack spelling: no break at the case's end, where
                         * a break would do */
                    }
                    syFree(at);
                }
                dummy_resources = NULL;
                dummy_request.command = DUMMY_NONE;
                dummy_request.ready = TRUE;
                /* @hack spelling: no break at the case's end, where a break
                 * would do */
            }
        }
        dummy_request.busy = FALSE;
    }
}

/* the resources draw in the list's order. a DUMMY_TEXTURES resource thus
 * gives its textures to the iff_NJCM chunks after it. a DUMMY_TEXTURE_ANIM
 * resource animates the last iff_NJCM chunk before it. it writes its
 * animation's frame game_state.steps into the model of that chunk's object,
 * not its children's. the chunk has drawn already, so the change shows from
 * the next draw on */
static void dummy_object_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct dummy_resource *resource;
    NJS_CNK_OBJECT *object;
    NJS_POINT3 start;
    NJS_POINT3 end;
    int i;

    if (SHOWS_GRID(work)) {
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, work->ang.x);
        for (i = 0; i < GRID_LINES; i++) {
            start.y = (i * GRID_STEP) - GRID_HALF;
            start.x = -GRID_HALF;
            start.z = 0.0F;
            end.y = (i * GRID_STEP) - GRID_HALF;
            end.x = GRID_HALF;
            end.z = 0.0F;
            line_draw_points(&start, &end);
        }
        for (i = 0; i < GRID_LINES; i++) {
            start.x = (i * GRID_STEP) - GRID_HALF;
            start.z = -GRID_HALF;
            start.y = 0.0F;
            end.x = (i * GRID_STEP) - GRID_HALF;
            end.z = GRID_HALF;
            end.y = 0.0F;
            line_draw_points(&start, &end);
            start.z = (i * GRID_STEP) - GRID_HALF;
            start.x = -GRID_HALF;
            start.y = 0.0F;
            end.z = (i * GRID_STEP) - GRID_HALF;
            end.x = GRID_HALF;
            end.y = 0.0F;
            line_draw_points(&start, &end);
        }
        njPopMatrixEx();
    }
    if (dummy_request.show) {
        object = NULL;
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, work->ang.x);
        draw_control_set(DRAW_CONTROL_SHADOW);
        for (resource = dummy_resources; resource != NULL;
             resource = resource->next) {
            switch (resource->kind) {
            /* @hack spelling: the case of DUMMY_CHUNK ahead of
             * DUMMY_TEXTURES's, not in the kinds' order */
            case DUMMY_CHUNK:
                switch (resource->as.chunk.type) {
                case iff_NJCM:
                    njCnkEasyDrawObject(resource->as.chunk.data);
                    object = resource->as.chunk.data;
                    break;
                /* @hack redundant: the case of a basic object, which draws
                 * nothing */
                case iff_NJBM:
                    break;
                }
                break;
            case DUMMY_TEXTURES:
                njSetTexture(&resource->as.textures.texlist);
                break;
            case DUMMY_TEXTURE_ANIM:
                if (object != NULL) {
                    texture_anim_apply(object->model, resource->as.anim,
                                       game_state.steps);
                }
                break;
            }
        }
        draw_control_clear(DRAW_CONTROL_SHADOW);
        njPopMatrixEx();
    }
}
