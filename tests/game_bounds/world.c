#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "collision/world.h"
#include "system/areas.h"

static union {
    unsigned long alignment;
    unsigned char bytes[4096];
} bitmap;
static struct land_block_color palette[256];
static const struct land_block_color short_palette[] = {
    {0, 91}, {1, 92}, {0xff000001U, 93}};

static int load_grid(const char *name, const NJS_POINT3 *origin,
                     const NJS_POINT3 *cell)
{
    return world_load_block_grid(name, palette, origin, cell);
}

static int load_short_grid(const NJS_POINT3 *origin, const NJS_POINT3 *cell)
{
    return world_load_block_grid("STAGE.BMP", short_palette, origin, cell);
}

#undef WORK_AREA_FILE
#define WORK_AREA_FILE bitmap.bytes
#undef WORK_AREA_END
#define WORK_AREA_END (bitmap.bytes + sizeof(bitmap.bytes))
#include "src/collision/world.c"

struct game_state game_state;
static Sint32 prs_size;
static Sint32 bmp_size;
static int prs_reads;
static int bmp_reads;
static int allocation_calls;
static int allocated;
static int fail_at;

Sint32 loader_read_prs_buffer(const char *name, void *dest, Uint32 capacity)
{
    assert(strcmp(name, "STAGE.prs") == 0);
    assert(dest == bitmap.bytes && capacity == sizeof(bitmap.bytes));
    ++prs_reads;
    return prs_size;
}

Sint32 loader_read_file_checked(const char *name, void *dest, Uint32 capacity)
{
    assert(strcmp(name, "STAGE.BMP") == 0);
    assert(dest == bitmap.bytes && capacity == sizeof(bitmap.bytes));
    ++bmp_reads;
    return bmp_size;
}

void *syMalloc(Uint32 size)
{
    ++allocation_calls;
    if (allocation_calls == fail_at) {
        return NULL;
    }
    ++allocated;
    return malloc(size);
}

void syFree(void *ptr)
{
    assert(ptr != NULL && allocated > 0);
    --allocated;
    free(ptr);
}

static Uint8 *make_grid(int bits)
{
    struct bmp_file_header *file = (struct bmp_file_header *)bitmap.bytes;
    struct bmp_info_header *info = BLOCK_GRID_INFO;
    int colors = 1 << bits;
    Uint8 *pixels;
    int i;

    memset(bitmap.bytes, 0, sizeof(bitmap.bytes));
    file->type = 0x4d42;
    file->offbits.lo = sizeof(*file) + sizeof(*info) + colors * 4;
    file->size.lo = file->offbits.lo + 8;
    info->size.lo = sizeof(*info);
    info->bitcount = bits;
    info->planes = 1;
    info->width.lo = 32 / bits;
    info->height.lo = 2;
    for (i = 0; i < colors; ++i) {
        BLOCK_GRID_PALETTE[4 * i] = (Uint8)i;
        palette[i].rgb = i;
        palette[i].blocks = 1000 + i;
    }
    pixels = bitmap.bytes + file->offbits.lo;
    for (i = 0; i < 8; ++i) {
        pixels[i] = (Uint8)(0x13 + 19 * i);
    }
    prs_size = file->size.lo;
    bmp_size = -1;
    return pixels;
}

static void check_grid(int bits)
{
    struct bmp_info_header *info = BLOCK_GRID_INFO;
    int columns = 32 / bits;
    int colors = 1 << bits;
    int x;
    int z;
    int expected;
    Uint8 *pixels = make_grid(bits);
    NJS_POINT3 origin = {4.0F, 0.0F, 2.0F};
    NJS_POINT3 cell = {2.0F, 0.0F, 3.0F};
    Float right = origin.x + columns * cell.x;
    Float bottom = origin.z - 2 * cell.z;

    assert(world_blocks_at(77, origin.x, origin.z) == -1);
    assert(load_grid("STAGE.BMP", &origin, &cell));
    for (z = 0; z < 2; ++z) {
        for (x = 0; x < columns; ++x) {
            expected = pixels[4 * z + (x * bits) / 8];
            expected >>= 8 - bits - (x * bits) % 8;
            expected &= colors - 1;
            assert(world_blocks_at(77, origin.x + x * cell.x,
                                   origin.z - z * cell.z) == 1000 + expected);
            assert(world_blocks_at(77, origin.x + (x + 0.99F) * cell.x,
                                   origin.z - (z + 0.99F) * cell.z) ==
                   1000 + expected);
        }
    }
    assert(world_blocks_at(77, right, origin.z) == 77);
    assert(world_blocks_at(77, right, bottom) == 77);
    assert(world_blocks_at(77, origin.x, bottom) == 77);
    assert(world_blocks_at(77, right + 0.01F, bottom + 0.01F) == 77);
    assert(world_blocks_at(77, origin.x - 0.01F, origin.z) == 77);
    assert(world_blocks_at(77, origin.x, origin.z + 0.01F) == 77);
    assert(world_blocks_at(77, origin.x, bottom - 0.01F) == 77);
    world_free_block_grid();
    --info->width.lo;
    assert(!load_grid("STAGE.BMP", &origin, &cell));
    ++info->width.lo;
    cell.z = 0.0F;
    assert(!load_grid("STAGE.BMP", &origin, &cell));
}

static void check_admission(void)
{
    static const char too_long[] =
        "0123456789012345678901234567890123456789012345678901234567890.BMP";
    struct bmp_file_header *file = (struct bmp_file_header *)bitmap.bytes;
    struct bmp_info_header *info = BLOCK_GRID_INFO;
    NJS_POINT3 origin = {0, 0, 0};
    NJS_POINT3 cell = {1, 1, 1};
    Uint8 *pixels = make_grid(8);
    Sint32 full_size = prs_size;
    int reads = prs_reads;
    int raw_reads = bmp_reads;
    int i;

    assert(!load_grid(NULL, &origin, &cell));
    assert(!load_grid("NO_EXTENSION", &origin, &cell));
    assert(!load_grid(too_long, &origin, &cell));
    assert(prs_reads == reads && bmp_reads == raw_reads);
    prs_size = -1;
    bmp_size = -1;
    assert(!load_short_grid(&origin, &cell));
    assert(!game_state.block_grid_loaded && allocated == 0);
    assert(prs_reads == reads + 1 && bmp_reads == raw_reads + 1);

    memset(pixels, 1, 8);
    /* Eternal Engine supplies neither mask. The original search read past
     * the array and found 0 for #490000 and 0xc4b90000 for #100000. Both
     * colors use the all-block mask in the portable renderer. */
    BLOCK_GRID_PALETTE[4 * 5 + 2] = 0x49;
    BLOCK_GRID_PALETTE[4 * 6 + 2] = 0x10;
    BLOCK_GRID_PALETTE[4 * 5] = 0;
    BLOCK_GRID_PALETTE[4 * 6] = 0;
    pixels[1] = 5;
    pixels[2] = 6;
    bmp_size = full_size;
    assert(load_short_grid(&origin, &cell));
    assert(world_blocks_at(0, 0, 0) == 92);
    assert(world_blocks_at(0, 1, 0) == -1);
    assert(world_blocks_at(0, 2, 0) == -1);
    assert(game_state.block_grid_loaded && allocated == 2);

    /* A short read can leave a previously valid tail in scratch. */
    prs_size = -1;
    bmp_size = full_size - 1;
    assert(!load_short_grid(&origin, &cell));
    bmp_size = sizeof(*file) + sizeof(*info) - 1;
    assert(!load_short_grid(&origin, &cell));
    prs_size = full_size;
    bmp_size = -1;
    --file->size.lo;
    assert(!load_short_grid(&origin, &cell));
    file->size.lo += 2;
    assert(!load_short_grid(&origin, &cell));
    --file->size.lo;
    ++file->offbits.hi;
    assert(!load_short_grid(&origin, &cell));
    file->offbits.hi = 0;
    ++info->compression.lo;
    assert(!load_short_grid(&origin, &cell));
    info->compression.lo = 0;
    info->clrused.lo = 257;
    assert(!load_short_grid(&origin, &cell));
    info->clrused.lo = 2;
    assert(!load_short_grid(&origin, &cell));
    assert(world_blocks_at(0, 0, 0) == 92 && allocated == 2);

    memset(pixels, 0, 8);
    for (i = 1; i <= 2; ++i) {
        fail_at = allocation_calls + i;
        assert(!load_short_grid(&origin, &cell));
        assert(world_blocks_at(0, 0, 0) == 92 && allocated == 2);
    }
    fail_at = 0;
    assert(load_short_grid(&origin, &cell));
    assert(allocated == 2 && world_blocks_at(0, 0, 0) == 91);
    world_free_block_grid();
    assert(allocated == 0 && !game_state.block_grid_loaded);
}

int main(void)
{
    check_grid(1);
    check_grid(4);
    check_grid(8);
    check_admission();
    return 0;
}
