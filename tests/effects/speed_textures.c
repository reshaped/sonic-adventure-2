/* The stage owns the effect lists through level-zero tasks. Supply a decoded
 * local archive and count individual device handles at upload and release. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <ninja.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "effect/sonic_effect.h"
#include "effect/sonic_effect_assets.h"
#include "sound/sound.h"
#include "system/areas.h"
#include "system/loader.h"
#include "system/task.h"

#define SLOTS 39
#define DIRECTORY_BYTES 96
#define TEXEL_BYTES (8 * 8 * 2)
#define ARCHIVE_BYTES (DIRECTORY_BYTES + SLOTS * (16 + TEXEL_BYTES))

enum bank { SONIC, AMY, SHADOW, BANK_COUNT };

static const struct {
    const char *file;
    void (*load)(void);
    NJS_TEXLIST *list;
} banks[BANK_COUNT] = {{"S_EFFTEX.PRS", s_efftex_load, &s_efftex_texlist},
                       {"AMY_EFFTEX.PRS", amy_efftex_load, &s_efftex_texlist},
                       {"SH_EFFTEX.PRS", sh_efftex_load, &sh_efftex_texlist}};

static NJS_TEXNAME sonic_names[SLOTS], shadow_names[SLOTS];
NJS_TEXLIST s_efftex_texlist = {sonic_names, SLOTS};
NJS_TEXLIST sh_efftex_texlist = {shadow_names, SLOTS};
Uint8 texture_workspace[8192];
NJS_TEXERRSTAT _nj_texerr_;

static int reads[BANK_COUNT];
static int held[BANK_COUNT][SLOTS];
static int live_blocks, fail_next_alloc;
static enum bank missing = BANK_COUNT;
static enum bank reading;

void *syCalloc(Uint32 count, Uint32 size)
{
    void *block;
    if (fail_next_alloc) {
        fail_next_alloc = 0;
        return NULL;
    }
    block = calloc(count, size);
    assert(block != NULL);
    live_blocks++;
    return block;
}

void *syMalloc(Uint32 size)
{
    return syCalloc(1, size);
}

void syFree(void *block)
{
    if (block != NULL) {
        assert(live_blocks > 0);
        live_blocks--;
        free(block);
    }
}

void sound_detach_task(const struct task *task)
{
    (void)task;
}

void collision_free(struct task *task)
{
    /* The texture holders have no collision work. */
    (void)task;
    assert(0);
}

/* A PVMH with numbered slots and 39 ordinary 8x8 RGB565 PVRT chunks. */
Sint32 loader_read_prs_buffer(const char *name, void *destination,
                              Uint32 capacity)
{
    Uint8 *archive = destination;
    int i;
    for (reading = SONIC; reading < BANK_COUNT; reading++) {
        if (strcmp(name, banks[reading].file) == 0) {
            break;
        }
    }
    assert(reading < BANK_COUNT);
    reads[reading]++;
    if (reading == missing) {
        return -1;
    }
    assert(capacity >= ARCHIVE_BYTES);
    memset(archive, 0, ARCHIVE_BYTES);
    memcpy(archive, "PVMH", 4);
    archive[4] = DIRECTORY_BYTES - 8;
    archive[10] = SLOTS;
    for (i = 0; i < SLOTS; i++) {
        Uint8 *chunk = archive + DIRECTORY_BYTES + i * (16 + TEXEL_BYTES);
        archive[12 + 2 * i] = (Uint8)i;
        memcpy(chunk, "PVRT", 4);
        chunk[4] = 8 + TEXEL_BYTES;
        chunk[8] = NJD_TEXFMT_RGB_565;
        chunk[9] = NJD_TEXFMT_TWIDDLED >> 8;
        chunk[12] = chunk[14] = 8;
    }
    return ARCHIVE_BYTES;
}

void njSetPvmTextureList(NJS_TEXLIST *list, NJS_TEXNAME *entries, char *names,
                         Sint32 count)
{
    Sint32 i;
    for (i = 0; i < count; i++) {
        entries[i].filename = names + i * PVM_NAME_SIZE;
    }
    list->textures = entries;
}

NJS_TEXERRSTAT *njGetTextureErrStat(void)
{
    return &_nj_texerr_;
}

static Uint32 handle(enum bank bank, int slot)
{
    return 1U + bank * SLOTS + slot;
}

Sint32 njLoadTexturePvmMemory(Uint8 *archive, NJS_TEXLIST *list)
{
    int i;
    assert(archive[10] == SLOTS && reading < BANK_COUNT);
    list->nbTexture = SLOTS;
    for (i = 0; i < SLOTS; i++) {
        assert(!held[reading][i]);
        held[reading][i] = 1;
        list->textures[i].attr = (Uint32)i;
        list->textures[i].texaddr = handle(reading, i);
    }
    return 1;
}

Sint32 njReleaseTextureLN(Uint32 texture)
{
    Uint32 index = texture - 1U;
    assert(index < BANK_COUNT * SLOTS);
    assert(held[index / SLOTS][index % SLOTS]);
    held[index / SLOTS][index % SLOTS] = 0;
    return 1;
}

static void expect_bank(enum bank bank, int loaded)
{
    int i;
    for (i = 0; i < SLOTS; i++) {
        assert(held[bank][i] == loaded);
        if (loaded) {
            assert(banks[bank].list->textures[i].texaddr == handle(bank, i));
        }
    }
}

static void expect_clean_stage(void)
{
    enum bank bank;
    int i;
    assert(live_blocks == 0 && task_levels[0] == NULL);
    for (bank = SONIC; bank < BANK_COUNT; bank++) {
        expect_bank(bank, 0);
        for (i = 0; i < SLOTS; i++) {
            assert(banks[bank].list->textures[i].texaddr == 0);
        }
    }
}

static void shared_slot(enum bank first, enum bank next)
{
    struct task *owner;
    int first_reads = reads[first], next_reads = reads[next];
    int shadow_reads = reads[SHADOW];

    banks[first].load();
    owner = task_levels[0];
    assert(owner != NULL);
    banks[next].load();
    banks[first].load();
    /* Shadow and Metal Sonic request the same independent archive. */
    sh_efftex_load();
    sh_efftex_load();
    task_destroy_levels_2_to_5();
    assert(reads[first] == first_reads + 1 && reads[next] == next_reads);
    assert(reads[SHADOW] == shadow_reads + 1);
    expect_bank(first, 1);
    expect_bank(SHADOW, 1);

    task_destroy(owner);
    expect_bank(first, 0);
    expect_bank(SHADOW, 1);
    banks[next].load();
    assert(reads[next] == next_reads + 1);
    expect_bank(next, 1);
    task_destroy_all();
    task_destroy_all();
    expect_clean_stage();
}

static void retry_load(enum bank bank)
{
    int before = reads[bank];
    fail_next_alloc = 1;
    banks[bank].load();
    assert(!fail_next_alloc && reads[bank] == before);
    expect_clean_stage();

    missing = bank;
    banks[bank].load();
    assert(reads[bank] == before + 1);
    expect_clean_stage();

    missing = BANK_COUNT;
    banks[bank].load();
    assert(reads[bank] == before + 2);
    expect_bank(bank, 1);
    task_destroy_all();
    expect_clean_stage();
}

int main(void)
{
    enum bank bank;
    task_init();
    shared_slot(SONIC, AMY);
    shared_slot(AMY, SONIC);
    for (bank = SONIC; bank < BANK_COUNT; bank++) {
        retry_load(bank);
    }
    return 0;
}
