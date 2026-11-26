/* The SDK identifies failed uploads by directory slot. Deliberately store
 * entry 1 before entry 0 so a slot number cannot stand in for load order. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <kamui.h>
#include <nj_texlist.h>
#include <shinobi.h>

#include "engine/dc/pvm.h"
#include "system/loader.h"
#include "system/texture.h"

#define SLOTS 2
#define DIRECTORY_SIZE 24
#define HANDLE_BASE 0x43210000UL
#define GLOBAL_BASE 0x2710UL

Uint8 texture_workspace[8192];
static Uint8 archive[sizeof(texture_workspace)];
static Uint32 archive_size;
static int fail_allocation;
static int fail_ordinal = -1;
static int live_blocks;
static int uploads;
static int reads;
#include "system/texture/text.inc"

static const char *failed_file;
static int references[SLOTS];
static int verify_padding;
/* the heap refuses a block of more bytes than heap_limit, where it isn't 0 */
static Uint32 heap_limit;
/* the archive Ninja was last handed, and the bytes it must hold where
 * expected_size isn't 0 */
static const Uint8 *uploaded;
static Uint8 expected[sizeof(texture_workspace)];
static Uint32 expected_size;

static NJS_TEXNAME console_names[SLOTS], panel_names[SLOTS], drive_names[SLOTS];
NJS_TEXLIST console_texlist = {console_names, SLOTS};
const NJS_TEXLIST al_panel_texlist = {panel_names, SLOTS};
NJS_TEXLIST chaosdrive_tex_texlist = {drive_names, SLOTS};
NJS_TEXERRSTAT _nj_texerr_;

static void put16(Uint8 *p, Uint32 value)
{
    p[0] = (Uint8)value;
    p[1] = (Uint8)(value >> 8);
}

static void put32(Uint8 *p, Uint32 value)
{
    put16(p, value);
    put16(p + 2, value >> 16);
}

static Uint32 get16(const Uint8 *p)
{
    return p[0] | ((Uint32)p[1] << 8);
}

static Uint32 get32(const Uint8 *p)
{
    return get16(p) | (get16(p + 2) << 16);
}

/* two 8x8 RGB565 textures, the first of one kind and the second of another */
static void make_archive_of(Uint32 first, Uint32 second)
{
    Uint32 i;
    memset(archive, 0, sizeof(archive));
    memcpy(archive, "PVMH", 4);
    put32(archive + 4, DIRECTORY_SIZE - 8);
    put16(archive + 8, 8);
    put16(archive + 10, SLOTS);
    archive_size = DIRECTORY_SIZE;
    for (i = 0; i < SLOTS; i++) {
        Uint8 *p = archive + archive_size;
        Uint32 kind = i == 0 ? first : second;
        Uint32 size = njCalcTextureSize(8, 8, kind | NJD_TEXFMT_RGB_565);
        assert(size > 0 && archive_size + 16 + size <= sizeof(archive));
        put16(archive + 12 + 6 * i, SLOTS - 1 - i);
        put32(archive + 14 + 6 * i, GLOBAL_BASE + i);
        memcpy(p, "PVRT", 4);
        put32(p + 4, size + 8);
        put32(p + 8, kind | NJD_TEXFMT_RGB_565);
        put16(p + 12, 8);
        put16(p + 14, 8);
        memset(p + 16, 0xaf, size);
        archive_size += size + 16;
    }
}

/* Two 8x8 RGB565 textures. VQ stores a codebook and encoded indices but
 * omits the DMA dummy bytes, as the last mip does on the KartER download. */
static void make_archive(Uint32 kind)
{
    make_archive_of(kind, kind);
}

void *syCalloc(Uint32 count, Uint32 size)
{
    void *block;
    if (fail_allocation > 0 && --fail_allocation == 0) {
        return NULL;
    }
    if (heap_limit != 0 && count * size > heap_limit) {
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
    if (block == NULL) {
        return;
    }
    assert(live_blocks > 0);
    free(block);
    live_blocks--;
}

Sint32 loader_read_prs_buffer(const char *name, void *destination,
                              Uint32 capacity)
{
    reads++;
    if ((failed_file != NULL && strcmp(name, failed_file) == 0) ||
        archive_size > capacity) {
        /* Like a failed read/decode, this leaves old bytes in the work area. */
        return -1;
    }
    memcpy(destination, archive, archive_size);
    return (Sint32)archive_size;
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

Sint32 njLoadTexturePvmMemory(Uint8 *input, NJS_TEXLIST *list)
{
    int i, ordinal;
    Uint32 offset = DIRECTORY_SIZE;
    NJS_TEXSURFACE surface;
    uploads++;
    uploaded = input;
    assert(expected_size == 0 || memcmp(input, expected, expected_size) == 0);
    assert(list->nbTexture == 0 && get16(input + 10) == SLOTS);
    list->nbTexture = get16(input + 10);
    for (i = 0; i < SLOTS; i++) {
        list->textures[i].attr = get16(input + 12 + 6 * i);
        list->textures[i].texaddr = get32(input + 14 + 6 * i);
    }
    for (ordinal = 0; ordinal < SLOTS; ordinal++) {
        for (i = 0; i < SLOTS && list->textures[i].attr != (Uint32)ordinal;
             i++) {
        }
        assert(i < SLOTS && list->textures[i].texaddr == GLOBAL_BASE + i);
        if (ordinal == fail_ordinal) {
            _nj_texerr_.n = i;
            return -1;
        }
        if (verify_padding) {
            Uint32 j;
            Uint32 format = get32(input + offset + 8);
            Uint32 encoded = njCalcTextureSize(8, 8, format);
            assert(memcmp(input + offset, "PVRT", 4) == 0);
            assert(njCheckTextureFormatLow(&surface, 8, 8, format) ==
                   KMSTATUS_SUCCESS);
            assert(surface.TextureSize > encoded);
            assert(get32(input + offset + 4) == surface.TextureSize + 8);
            assert(input[offset + 16 + encoded - 1] == 0xaf);
            for (j = encoded; j < surface.TextureSize; j++) {
                assert(input[offset + 16 + j] == 0);
            }
            offset += 16 + surface.TextureSize;
        }
        list->textures[i].texaddr = HANDLE_BASE + i;
        references[i]++;
    }
    return 1;
}

Sint32 njReleaseTextureLN(Uint32 handle)
{
    Uint32 i;
    assert(handle >= HANDLE_BASE && handle < HANDLE_BASE + SLOTS);
    i = handle - HANDLE_BASE;
    assert(references[i] > 0);
    references[i]--;
    return 1;
}

static void balanced(int blocks, int refs)
{
    assert(live_blocks == blocks);
    assert(references[0] == refs && references[1] == refs);
}

static void cleared(const NJS_TEXLIST *list)
{
    Uint32 i;
    for (i = 0; i < list->nbTexture; i++) {
        assert(list->textures[i].texaddr == 0 && list->textures[i].attr == 0);
    }
}

static void bounds(void)
{
    Uint32 i;
    for (i = 0; i < archive_size; i++) {
        assert(texture_pvm_count(archive, i) == 0);
    }
    assert(texture_pvm_count(archive, archive_size) == SLOTS);
    /* A physical texture after the declared last texture is a count mismatch.
     */
    i = 8 + get32(archive + DIRECTORY_SIZE + 4);
    memcpy(archive + archive_size, archive + DIRECTORY_SIZE, i);
    assert(texture_pvm_count(archive, archive_size + i) == 0);
    put16(archive + 18, 1);
    assert(texture_pvm_count(archive, archive_size) == 0);
    put16(archive + 18, 2);
    assert(texture_pvm_count(archive, archive_size) == 0);
    put16(archive + 18, 0);
    put16(archive + 10, 513);
    assert(texture_pvm_count(archive, archive_size) == 0);
    put16(archive + 10, SLOTS);
    put16(archive + DIRECTORY_SIZE + 12, 7);
    assert(texture_pvm_count(archive, archive_size) == 0);
    put16(archive + DIRECTORY_SIZE + 12, 8);
    put32(archive + DIRECTORY_SIZE + 4, 0xffffffffUL);
    assert(texture_pvm_count(archive, archive_size) == 0);
}

static void upload(void)
{
    NJS_TEXNAME names[SLOTS] = {{"first", 63, 400}, {"second", 15, 401}};
    NJS_TEXLIST target = {names, SLOTS};
    int failure, repetition;
    assert(texture_load_pvm_memory(archive, archive_size, &target));
    assert(target.nbTexture == SLOTS && target.textures == names);
    assert(strcmp(names[0].filename, "first") == 0 &&
           strcmp(names[1].filename, "second") == 0);
    assert(names[0].texaddr == HANDLE_BASE && names[0].attr == 1);
    assert(names[1].texaddr == HANDLE_BASE + 1 && names[1].attr == 0);
    for (failure = 0; failure < SLOTS; failure++) {
        assert(texture_load_pvm_memory(archive, archive_size, &target));
        balanced(1, 1);
        fail_ordinal = failure;
        for (repetition = 0; repetition < 3; repetition++) {
            assert(!texture_load_pvm_memory(archive, archive_size, &target));
            cleared(&target);
            balanced(0, 0);
        }
        fail_ordinal = -1;
    }
    target.nbTexture = 1;
    names[0].texaddr = 122;
    names[1].texaddr = 123;
    assert(!texture_load_pvm_memory(archive, archive_size, &target));
    cleared(&target);
    assert(names[1].texaddr == 123);
    target.nbTexture = SLOTS;
    assert(texture_load_pvm_memory(archive, archive_size, &target));
    assert(njReleaseTexture(&target) == 1);
    cleared(&target);
    texture_discard_pvm(&target);
}

static void ownership(void)
{
    NJS_TEXNAME original[SLOTS] = {{0}};
    NJS_TEXNAME replaced[] = {{NULL, 7, 0x1111}};
    NJS_TEXLIST list = {original, SLOTS};

    assert(texture_load_pvm_memory(archive, archive_size, &list));
    list.textures = replaced;
    list.nbTexture = 1;
    assert(njReleaseTexture(&list) == 1);
    cleared(&list);
    balanced(0, 0);

    list.textures = original;
    list.nbTexture = SLOTS;
    assert(texture_load_pvm_memory(archive, archive_size, &list));
    /* A bank reset retires the records without dereferencing a retired
     * overlay. Its old handles may now designate newly loaded textures. */
    texture_forget_all_pvms();
    references[0] = references[1] = 0;
    balanced(0, 0);
    assert(texture_load_pvm_memory(archive, archive_size, &console_texlist));
    texture_discard_pvm(&list);
    cleared(&list);
    balanced(1, 1);
    texture_discard_pvm(&console_texlist);
}

static void file(void)
{
    NJS_TEXNAME names[SLOTS] = {{0}};
    NJS_TEXLIST target = {names, SLOTS};
    char long_name[300];
    assert(texture_load_pvm("FIRST", &target));
    failed_file = "MISSING.PRS";
    assert(!texture_load_pvm("MISSING", &target));
    assert(uploads == 1);
    cleared(&target);
    assert(texture_load_pvm_alloc("MISSING") == NULL);
    assert(!texture_load_pvm_in("MISSING.PRS", &target, texture_workspace,
                                sizeof(texture_workspace)));
    failed_file = NULL;
    assert(!texture_load_pvm_in("FIRST.PRS", &target, texture_workspace,
                                archive_size - 1));
    assert(texture_load_pvm_in("FIRST.PRS", &target, texture_workspace,
                               archive_size));
    archive[0] = 0;
    assert(!texture_load_pvm("FIRST", &target));
    cleared(&target);
    assert(uploads == 2 && texture_load_pvm_alloc("FIRST") == NULL);
    memset(long_name, 'A', sizeof(long_name) - 1);
    long_name[sizeof(long_name) - 1] = 0;
    reads = 0;
    assert(!texture_load_pvm(long_name, &target));
    assert(reads == 0);
}

static void allocation(void)
{
    NJS_TEXLIST *allocated;
    int i;
    make_archive(NJD_TEXFMT_VQ);
    verify_padding = 1;
    assert(texture_pvm_count(archive, archive_size) == SLOTS);
    /* the list, the owner and the entries. the archive grows in the work
     * area, so the load takes no fourth block */
    for (i = 1; i <= 3; i++) {
        fail_allocation = i;
        assert(texture_load_pvm_alloc("PVM") == NULL);
        balanced(0, 0);
        assert(uploads == 0);
    }
    fail_allocation = 0;
    fail_ordinal = 1;
    assert(texture_load_pvm_alloc("PVM") == NULL);
    balanced(0, 0);
    fail_ordinal = -1;
    allocated = texture_load_pvm_alloc("PVM");
    assert(allocated != NULL && allocated->nbTexture == SLOTS);
    assert(allocated->textures[0].filename == NULL &&
           allocated->textures[1].filename == NULL);
    balanced(2, 1);
    assert(njReleaseTexture(allocated) == 1);
    syFree(allocated);
}

/* fill the archive with a chunk of each kind, each with texels of its own,
 * and expected with the archive as it must lie once each chunk holds what
 * Ninja copies of it. it returns the grown archive's bytes */
static Uint32 make_short_archive(Uint32 first, Uint32 second)
{
    NJS_TEXSURFACE surface;
    Uint32 i, j, format, encoded, stored;
    Uint32 at = DIRECTORY_SIZE;
    Uint32 grown = DIRECTORY_SIZE;

    make_archive_of(first, second);
    memcpy(expected, archive, DIRECTORY_SIZE);
    for (i = 0; i < SLOTS; i++) {
        format = get32(archive + at + 8);
        encoded = njCalcTextureSize(8, 8, format);
        assert(njCheckTextureFormatLow(&surface, 8, 8, format) ==
               KMSTATUS_SUCCESS);
        stored = surface.TextureSize > encoded ? surface.TextureSize : encoded;
        for (j = 0; j < encoded; j++) {
            archive[at + 16 + j] = (Uint8)(1 + (j + 101 * i) % 255);
        }
        assert(grown + 16 + stored <= sizeof(expected));
        memcpy(expected + grown, archive + at, 16 + encoded);
        put32(expected + grown + 4, stored + 8);
        memset(expected + grown + 16 + encoded, 0, stored - encoded);
        at += 16 + encoded;
        grown += 16 + stored;
    }
    assert(at == archive_size && grown > archive_size);
    return grown;
}

/* an archive with a short chunk grows in the room its load read it into,
 * and takes the heap's copy only where the room can't hold it grown. the
 * heap here gives the loads' small blocks and no copy of an archive, as the
 * game's can't for the largest of the events' archives */
static void grows_in_room(Uint32 first, Uint32 second)
{
    NJS_TEXNAME names[SLOTS] = {{0}};
    NJS_TEXLIST target = {names, SLOTS};
    NJS_TEXLIST *allocated;
    Uint32 grown = make_short_archive(first, second);

    expected_size = grown;
    heap_limit = 512;
    assert(texture_load_pvm_in("FIRST.PRS", &target, texture_workspace,
                               sizeof(texture_workspace)));
    assert(uploaded == texture_workspace);
    balanced(1, 1);
    /* the room may hold the grown archive and not a byte more */
    assert(texture_load_pvm_in("FIRST.PRS", &target, texture_workspace,
                               (Sint32)grown));
    assert(uploaded == texture_workspace);
    assert(texture_load_pvm("FIRST", &target));
    assert(uploaded == texture_workspace);
    balanced(1, 1);
    allocated = texture_load_pvm_alloc("FIRST");
    assert(allocated != NULL && uploaded == texture_workspace);
    balanced(3, 2);
    assert(njReleaseTexture(allocated) == 1);
    syFree(allocated);
    balanced(1, 1);
    /* a room one byte short leaves the copy to the heap */
    assert(!texture_load_pvm_in("FIRST.PRS", &target, texture_workspace,
                                (Sint32)grown - 1));
    cleared(&target);
    balanced(0, 0);
    heap_limit = 0;
    assert(texture_load_pvm_in("FIRST.PRS", &target, texture_workspace,
                               (Sint32)grown - 1));
    assert(uploaded != texture_workspace);
    balanced(1, 1);
    texture_discard_pvm(&target);
    expected_size = 0;
}

static void room(void)
{
    /* the second chunk alone is short, as in the events' archives, and
     * nothing moves. then the first alone, and both: the second moves up */
    grows_in_room(NJD_TEXFMT_TWIDDLED, NJD_TEXFMT_VQ);
    grows_in_room(NJD_TEXFMT_VQ, NJD_TEXFMT_TWIDDLED);
    grows_in_room(NJD_TEXFMT_VQ, NJD_TEXFMT_VQ);
}

static void set(void)
{
    int i;
    assert(!texture_load_set(-1));
    assert(!texture_load_set(1));
    assert(uploads == 0);
    assert(texture_load_set(0));
    balanced(3, 3);
    assert(texture_load_set(0));
    balanced(3, 3);
    texture_release_set();
    balanced(0, 0);
    failed_file = TEXTURE_SET_CHAOSDRIVE_TEX ".PRS";
    for (i = 0; i < 3; i++) {
        assert(!texture_load_set(0));
        balanced(0, 0);
        cleared(&console_texlist);
        cleared(&al_panel_texlist);
        cleared(&chaosdrive_tex_texlist);
    }
    texture_release_set();
}

static void shared(void)
{
    NJS_TEXNAME originals[SLOTS] = {{"Water", 0, 0}, {"earth", 0, 0}};
    NJS_TEXNAME borrowed[SLOTS] = {{"EARTH", 0, 0}, {"water", 0, 0}};
    NJS_TEXLIST owner = {originals, SLOTS};
    NJS_TEXLIST alias = {borrowed, SLOTS};
    NJS_TEXLIST *lists[] = {&alias, &owner, NULL};
    NJS_TEXLIST **groups[] = {lists, NULL};
    NJS_TEXNAME resident_name = {"earth", 14, 998};
    NJS_TEXLIST resident = {&resident_name, 1};
    struct texture_pvm pvms[] = {{"OWNER", &owner}, {NULL, NULL}, {NULL, NULL}};
    assert(!texture_await_shared(pvms, groups));
    assert(texture_load_shared(pvms, groups));
    assert(texture_await_shared(pvms, groups));
    assert(borrowed[0].texaddr == originals[1].texaddr &&
           borrowed[0].attr == originals[1].attr);
    assert(borrowed[1].texaddr == originals[0].texaddr &&
           borrowed[1].attr == originals[0].attr);
    balanced(1, 1);
    /* A handle left by a previous use cannot satisfy a missing name. */
    borrowed[0].filename = "missing";
    assert(!texture_load_shared(pvms, groups));
    assert(!texture_await_shared(pvms, groups));
    cleared(&alias);
    cleared(&owner);
    balanced(0, 0);
    borrowed[0].filename = "EARTH";
    /* The last matching PVM has precedence. An already resident provider
     * may replace an owner's visible entry without taking its allocation. */
    pvms[1].texlist = &resident;
    assert(texture_load_shared(pvms, groups));
    assert(borrowed[0].texaddr == 998 && borrowed[0].attr == 14);
    assert(originals[1].texaddr == 998);
    texture_release_shared(pvms, groups);
    assert(resident_name.texaddr == 998 && resident_name.attr == 14);
    texture_release_shared(pvms, groups);
    cleared(&alias);
    cleared(&owner);
}

static void event_shared_alias(void)
{
    NJS_TEXNAME common_names[SLOTS] = {{"lens01", 0, 0}, {"dust01", 0, 0}};
    NJS_TEXNAME other_names[SLOTS] = {{0}};
    NJS_TEXNAME lens_name = {"lens01", 0, 0};
    NJS_TEXNAME warp_name = {"sikake_37_32", 7, 99};
    NJS_TEXLIST common = {common_names, SLOTS};
    NJS_TEXLIST other = {other_names, SLOTS};
    NJS_TEXLIST lens = {&lens_name, 1};
    NJS_TEXLIST warp = {&warp_name, 1};
    const NJS_TEXNAME *optional[] = {&warp_name};
    NJS_TEXLIST *effects[] = {&lens, &warp, NULL};
    NJS_TEXLIST **groups[] = {effects, NULL};
    struct texture_pvm pvms[] = {
        {"efftex_common", &common}, {NULL, NULL}, {NULL, NULL}};

    /* E0000's common PVM supplies the flare. The separate warp material is
     * absent, even when an earlier use left a handle in its slot. */
    assert(texture_load_shared_with_optional(pvms, groups, optional, 1));
    assert(lens_name.texaddr == HANDLE_BASE &&
           lens_name.attr == common_names[0].attr);
    cleared(&warp);
    balanced(1, 1);

    assert(!texture_load_shared(pvms, groups));
    cleared(&common);
    cleared(&lens);
    cleared(&warp);
    balanced(0, 0);

    /* The exemption follows the slot. The same missing filename in a
     * required list still rolls back a previously successful load. */
    assert(texture_load_shared_with_optional(pvms, groups, optional, 1));
    lens_name.filename = warp_name.filename;
    assert(!texture_load_shared_with_optional(pvms, groups, optional, 1));
    cleared(&common);
    cleared(&lens);
    cleared(&warp);
    balanced(0, 0);

    lens_name.filename = "lens01";
    pvms[1].name = "OTHER";
    pvms[1].texlist = &other;
    failed_file = "OTHER.PRS";
    assert(!texture_load_shared_with_optional(pvms, groups, optional, 1));
    cleared(&common);
    cleared(&other);
    cleared(&lens);
    cleared(&warp);
    balanced(0, 0);
    failed_file = NULL;

    /* A later provider can supply the optional slot on retry. */
    other_names[1].filename = warp_name.filename;
    assert(texture_load_shared_with_optional(pvms, groups, optional, 1));
    assert(warp_name.texaddr == HANDLE_BASE + 1 &&
           warp_name.attr == other_names[1].attr);
    balanced(2, 2);
    texture_release_shared(pvms, groups);
    cleared(&common);
    cleared(&other);
    cleared(&lens);
    cleared(&warp);
    balanced(0, 0);
}

static void duel_shared_aliases(void)
{
    NJS_TEXNAME names[SLOTS] = {{"jungle128_1", 0, 0}, {"water", 0, 0}};
    NJS_TEXNAME preview_name = {"tutates", 11, 12};
    NJS_TEXNAME prop_names[SLOTS] = {{"jungle128_3", 13, 14},
                                     {"jungle128_1", 0, 0}};
    NJS_TEXLIST owner = {names, SLOTS};
    NJS_TEXLIST preview = {&preview_name, 1};
    NJS_TEXLIST props = {prop_names, SLOTS};
    NJS_TEXLIST *lists[] = {&preview, &props, NULL};
    NJS_TEXLIST **groups[] = {lists, NULL};
    struct texture_pvm pvms[] = {{"OWNER", &owner}, {NULL, NULL}};
    const NJS_TEXNAME *optional[] = {NULL, &preview_name, &prop_names[0]};

    /* The two absent materials used to release all five duel archives.
     * A neighboring slot still needs its own binding. */
    assert(texture_load_shared_with_optional(pvms, groups, optional, 3));
    assert(!texture_await_shared(pvms, groups));
    cleared(&preview);
    assert(prop_names[0].texaddr == 0 && prop_names[0].attr == 0);
    assert(prop_names[1].texaddr == names[0].texaddr &&
           prop_names[1].attr == names[0].attr);
    balanced(1, 1);

    prop_names[1].filename = prop_names[0].filename;
    assert(!texture_load_shared_with_optional(pvms, groups, optional, 3));
    cleared(&owner);
    cleared(&preview);
    cleared(&props);
    balanced(0, 0);
}

static void check_slot_loaded(void)
{
    NJS_TEXNAME slots[2] = {{0}};
    NJS_TEXLIST list = {NULL, 2};

    assert(!texture_slot_loaded(NULL, 0));
    assert(!texture_slot_loaded(&list, 0));
    list.textures = slots;
    list.nbTexture = 0;
    assert(!texture_slot_loaded(&list, 0));
    list.nbTexture = 2;
    slots[1].texaddr = 1;
    assert(!texture_slot_loaded(&list, 0));
    assert(texture_slot_loaded(&list, 1));
    assert(!texture_slot_loaded(&list, 2));
    slots[1].texaddr = 0;
    assert(!texture_slot_loaded(&list, 1));
}

int main(int argc, char **argv)
{
    check_slot_loaded();
    assert(argc == 2);
    make_archive(NJD_TEXFMT_TWIDDLED);
    if (strcmp(argv[1], "bounds") == 0) {
        bounds();
    } else if (strcmp(argv[1], "upload") == 0) {
        upload();
    } else if (strcmp(argv[1], "ownership") == 0) {
        ownership();
    } else if (strcmp(argv[1], "file") == 0) {
        file();
    } else if (strcmp(argv[1], "allocation") == 0) {
        allocation();
    } else if (strcmp(argv[1], "room") == 0) {
        room();
    } else if (strcmp(argv[1], "set") == 0) {
        set();
    } else {
        assert(strcmp(argv[1], "shared") == 0);
        shared();
        event_shared_alias();
        duel_shared_aliases();
    }
    balanced(0, 0);
    return 0;
}
