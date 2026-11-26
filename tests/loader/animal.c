/* Stage animal acquisition, drawing and release with a controlled GD and PVR.
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "system/areas.h"

static union {
    unsigned char bytes[512];
    double alignment;
} workspace;
#undef WORK_AREA_FILE
#undef WORK_AREA_END
#define WORK_AREA_FILE (workspace.bytes)
#define WORK_AREA_END (workspace.bytes + sizeof(workspace.bytes))

#include "object/animal.c"

static int transfer;
static int allocations;
static int fail_allocation;
static int allocation_number;
static int texture_refs;
static int draws;
NJS_CNK_MODEL shadow_model[1];

static void put_word(Uint8 *p, uint32_t word)
{
    memcpy(p, &word, 4);
}

/* One native chunk and the relocation of its first word. The model has no
 * children. The one-channel motion has an empty node track. */
static void write_chunk(Uint8 *p, const char *tag)
{
    memcpy(p, tag, 4);
    put_word(p + 4, 96);
    if (strcmp(tag, "NMDM") == 0) {
        put_word(p + 8, 12);
        put_word(p + 12, 30);
    }
    memcpy(p + 104, "POF0", 4);
    put_word(p + 108, 4);
    p[112] = strcmp(tag, "NMDM") == 0 ? 0x40 : 0x41;
}

Sint32 loader_read_prs_buffer(const char *name, void *dest, Uint32 capacity)
{
    struct animal_file_head *head = dest;
    Uint8 *chunk = (Uint8 *)(head + 1);

    assert(strcmp(name, "MINIMAL_PEN.PRS") == 0);
    assert(dest == WORK_AREA_FILE && capacity == sizeof(workspace.bytes));
    memset(dest, 0, capacity);
    head->words = 5;
    head->sizes[0] = 116;
    head->sizes[1] = 16;
    head->sizes[2] = 116;
    head->sizes[3] = 116;
    write_chunk(chunk, "NJCM");
    memcpy(chunk + 116, "PVMH", 4);
    write_chunk(chunk + 132, "NMDM");
    write_chunk(chunk + 248, "NMDM");
    return transfer;
}

static void *allocate(Uint32 bytes)
{
    void *p;
    if (++allocation_number == fail_allocation) {
        return NULL;
    }
    p = calloc(1, bytes);
    assert(p != NULL);
    allocations++;
    return p;
}

void *syCalloc(Uint32 count, Uint32 bytes)
{
    return allocate(count * bytes);
}

void syFree(void *p)
{
    assert(p != NULL && allocations > 0);
    free(p);
    allocations--;
}

void *njReadBinary(Sint8 *buffer, Uint32 *position, Uint32 *type)
{
    assert(*position == 0);
    assert(!memcmp(buffer, "NJCM", 4) || !memcmp(buffer, "NMDM", 4));
    return allocate(96);
}

Uint32 texture_pvm_count(const void *p, Uint32 size)
{
    assert(size == 16 && !memcmp(p, "PVMH", 4));
    return 1;
}

Bool texture_load_pvm_memory(const void *p, Uint32 size, NJS_TEXLIST *target)
{
    assert(size == 16 && target->nbTexture == 1 && target->textures);
    if (++allocation_number == fail_allocation) {
        return FALSE;
    }
    target->textures[0].texaddr = 7;
    texture_refs++;
    return TRUE;
}

void texture_discard_pvm(NJS_TEXLIST *target)
{
    if (target->textures[0].texaddr) {
        assert(target->textures[0].texaddr == 7 && texture_refs == 1);
        texture_refs--;
        target->textures[0].texaddr = 0;
    }
}

Sint32 njSetTexture(NJS_TEXLIST *target)
{
    assert(target != NULL && texture_refs == 1);
    draws++;
    return 0;
}
void njScale(NJS_MATRIX *m, Float x, Float y, Float z)
{
}
void njTranslate(NJS_MATRIX *m, Float x, Float y, Float z)
{
}
void njRotateZ(NJS_MATRIX *m, Angle angle)
{
}
Bool njPushMatrixEx(void)
{
    return TRUE;
}
Bool njPopMatrixEx(void)
{
    return TRUE;
}
int collision_get_up_axis(void)
{
    return COLLISION_UP_Y;
}
void njCnkEasyDrawObject(NJS_CNK_OBJECT *object)
{
    assert(0);
}
void njCnkEasyDrawMotion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                         Float frame)
{
    assert(object == animal_models[0].object &&
           motion == animal_models[0].motion);
}
void njCnkModDrawModel(NJS_CNK_MODEL *model)
{
    assert(model == shadow_model);
}

int main(void)
{
    struct animal_file file = {"PEN", 0.5F};
    struct animal *animal;
    struct task task = {0};
    struct animal record = {0};
    int fail;

    task.extra.p = &record;
    animal = animal_of(&task);
    assert(animal->slot == 0);
    assert(!animal_load_model(&animal_models[0], &animal_files[0]));
    transfer = -1;
    assert(!animal_load_model(&animal_models[0], &file));
    animal_draw_model(&task);
    assert(!draws && !allocations && !texture_refs);
    transfer = 32 + 116 + 16 + 116;
    assert(!animal_load_model(&animal_models[0], &file));
    assert(!allocation_number);
    transfer += 116;
    for (fail = 1; fail <= 4; fail++) {
        allocation_number = 0;
        fail_allocation = fail;
        assert(!animal_load_model(&animal_models[0], &file));
        assert(!animal_models[0].object && !animal_models[0].motion &&
               !animal_models[0].texlist && !allocations && !texture_refs);
    }
    fail_allocation = 0;
    assert(animal_load_model(&animal_models[0], &file));
    assert(animal_models[0].speed == file.speed && allocations == 3);
    animal_draw_model(&task);
    assert(draws == 1 && texture_refs == 1);
    animals_free();
    animals_free();
    animal_draw_model(&task);
    assert(draws == 1 && !allocations && !texture_refs);
    return 0;
}
