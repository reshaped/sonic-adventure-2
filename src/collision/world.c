/* the collision world: the land, which it draws too, the objects tasks put
 * in it, and gravity */
#ifdef NONMATCHING
/* FLT_MAX and INT_MAX, by which this side's world_load_block_grid bounds a
 * cell's size and the bitmap's */
#include <float.h>
#include <limits.h>
#endif
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/polygon.h"
#include "collision/world.h"
#include "draw/light.h"
#include "player/player.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

#ifdef NONMATCHING
/* port_wide, which grows this side's clip under 16:9 */
#include "engine/dc/port.h"
#endif

/* the capacities of the world's arrays. world_collect_near_players keeps up
 * to NEAR_ADDED_MAX tasks' objects and NEAR_LAND_MAX land entries */
#define NEAR_ADDED_MAX 128
#define NEAR_LAND_MAX 128
#define ADDED_MAX 256 /* objects tasks put in the world */
#define DRAWN_MAX 1024 /* land entries world_land_list_drawn lists */
#define POOL_OBJECTS 256 /* objects world_pool_alloc lends */

/* the near radius world_land_start and world_reset_near_radius set */
#define NEAR_RADIUS 400.0F
/* the near radius of world_collect_near_players' second point */
#define SECOND_RADIUS 400.0F
/* how much farther than radius world_list_near reaches for a task's object */
#define ADDED_MARGIN 30.0F

/* world_add sets WORLD_TURNED where the model's center lies OFF_CENTER or more
 * from its origin. an angle must also have a bit of TURNED_ANGLE_BITS */
#define OFF_CENTER 15.0F
#define TURNED_ANGLE_BITS 0xfff0

#define WORLD_LAND_LEVEL 1
/* the modes of the land's work, which starts zeroed, at WORLD_LAND_RUN. nothing
 * sets WORLD_LAND_TO_RUN, which the step turns to WORLD_LAND_RUN */
#define WORLD_LAND_RUN 0
#define WORLD_LAND_TO_RUN 1

/* world_build_land and world_build_land_at_origin, the builds, round buf up to
 * a multiple of BUILD_ALIGN. while game_state.split_screen is 1, they clip
 * later draws to the tiles from BUILD_CLIP_FIRST to BUILD_CLIP_LAST, across and
 * down. a tile is 32 pixels a side, as njUserClipping takes it */
#define BUILD_ALIGN 32
#define BUILD_CLIP_FIRST 3.0F
#define BUILD_CLIP_LAST 10.0F
/* the bits of a land_block_color's rgb that hold red, green and blue */
#define BLOCK_RGB_BITS 0xffffff
/* the bytes world_load_block_grid's name may take, with its NUL, as it is and
 * made .prs */
#define GRID_NAME_BYTES 64

/* a BMP file's headers. the file header takes 14 bytes. each 32-bit field of
 * the two thus lies 2 bytes off a multiple of 4. the SH-4 can't read such a
 * field whole, so each is two halves, low first, which BMP_LONG joins */
struct bmp_long {
    Uint16 lo;
    Uint16 hi;
};
#define BMP_LONG(field) ((Uint32)(field).hi << 16 | (field).lo)
struct bmp_file_header {
    Uint16 type;
    struct bmp_long size;
    struct bmp_long reserved;
    struct bmp_long offbits; /* the pixels' offset from the file's start */
};
struct bmp_info_header {
    struct bmp_long size;
    struct bmp_long width;
    struct bmp_long height;
    Uint16 planes;
    Uint16 bitcount; /* bits to a pixel */
    struct bmp_long compression;
    struct bmp_long sizeimage;
    struct bmp_long xppm;
    struct bmp_long yppm;
    /* the palette's colors, or 0 for all that bitcount allows */
    struct bmp_long clrused;
    struct bmp_long clrimportant;
};

/* The normal stage bitmap read places its file header, info header and
 * four-byte palette entries consecutively in WORK_AREA_FILE. */
#ifdef NONMATCHING
/* the three views as offsets into WORK_AREA_FILE: this side's objects don't
 * define the names the matching side declares, which only symbols.txt
 * places */
#define BLOCK_GRID_FILE ((char *)WORK_AREA_FILE)
#define BLOCK_GRID_INFO                                                        \
    ((struct bmp_info_header *)(WORK_AREA_FILE +                               \
                                sizeof(struct bmp_file_header)))
#define BLOCK_GRID_PALETTE ((Uint8 *)(BLOCK_GRID_INFO + 1))
#else
/* @hack type: SHC takes a separate literal-pool address for each view. */
extern char block_grid_file[];
extern struct bmp_info_header block_grid_file_info;
extern Uint8 block_grid_file_palette[];
#define BLOCK_GRID_FILE block_grid_file
#define BLOCK_GRID_INFO (&block_grid_file_info)
#define BLOCK_GRID_PALETTE block_grid_file_palette
#endif

/* nothing calls these. none is static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* hide the land, or show it again. a hidden land's task neither draws it nor
 * plays its moving parts */
void world_hide_land(void);
void world_show_land(void);
/* leave the land's entries out of world_collect_near_players and
 * world_list_near, or take them in again */
void world_disable_land(void);
void world_enable_land(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* the objects the last world_collect_near_players kept for world_list_near:
 * world_near_added_count tasks' objects and world_near_land_count land
 * entries */
static Sint32 world_near_added_count = 0;
static Sint32 world_near_land_count = 0;
static struct world_object *world_near_added = NULL;
static struct land_entry *world_near_land = NULL;
static Float world_near_radius = NEAR_RADIUS;
struct world_object *world_objects = NULL;
/* whether the task draws the land and plays its moving parts, and whether
 * world_collect_near_players and world_list_near take its entries */
static Bool world_land_shown = TRUE;
static Bool world_land_enabled = TRUE;
/* the objects tasks put in the world, world_added_count of ADDED_MAX */
static struct world_object *world_added = NULL;
/* the land's entries to draw this frame, world_drawn_count of them */
static struct land_entry **world_drawn = NULL;
/* the block grid's cells, block_grid_bits each and block_grid_columns to a row,
 * and the blocks of each palette color */
static char *block_grid = NULL;
static struct land_block_color *block_grid_colors = NULL;
static Sint32 block_grid_bits = 4;
static Float block_grid_x = 0.0F;
static Float block_grid_z = 0.0F;
static Float block_grid_cell_x = 50.0F;
static Float block_grid_cell_z = 50.0F;
static Sint32 block_grid_columns = 0;
static Sint32 block_grid_rows = 0;
/* the POOL_OBJECTS objects world_pool_alloc lends */
static NJS_OBJECT *world_pool = NULL;

Sint16 world_count;
NJS_VECTOR world_gravity;
Angle world_gravity_ang_x;
Angle world_gravity_ang_z;
/* the land the task draws, or null */
static struct land_table *world_land;
/* the land's task and its work, while the task runs */
static struct task *world_land_task;
static struct task_work *world_land_work;
static Sint16 world_added_count;
static Sint16 world_drawn_count;
/* for each of world_pool's objects, TRUE where it is lent */
static Uint8 *world_pool_lent;

static void world_reset(void);
static struct land_entry *world_find_built(struct land_table *land,
                                           struct land_entry *self,
                                           NJS_OBJECT *object);
static void world_land_step(struct task *task);
static void world_land_load_textures(void);
static void world_land_release_textures(void);
static void world_land_destroy(struct task *task);
static void world_land_run(struct task *task);
static void world_land_draw(struct task *task);
static void world_land_draw_entries(void);
static void world_land_draw_motions(void);
static void world_land_play_motions(void);
static Sint32 world_blocks_at(Sint32 blocks, Float x, Float z);
static void world_land_list_drawn(void);
static void world_pool_free_all(void);

void world_free(void)
{
    if (world_near_added != NULL) {
        syFree(world_near_added);
        world_near_added = NULL;
    }
    if (world_near_land != NULL) {
        syFree(world_near_land);
        world_near_land = NULL;
    }
    if (world_objects != NULL) {
        syFree(world_objects);
        world_objects = NULL;
    }
    if (world_added != NULL) {
        syFree(world_added);
        world_added = NULL;
    }
    if (world_drawn != NULL) {
        syFree(world_drawn);
        world_drawn = NULL;
    }
    if (world_pool != NULL) {
        syFree(world_pool);
        world_pool = NULL;
    }
    if (world_pool_lent != NULL) {
        syFree(world_pool_lent);
        world_pool_lent = NULL;
    }
}

void world_alloc(void)
{
    if (world_near_added == NULL) {
        world_near_added =
            syMalloc(NEAR_ADDED_MAX * sizeof(struct world_object));
    }
    if (world_near_land == NULL) {
        world_near_land = syMalloc(NEAR_LAND_MAX * sizeof(struct land_entry));
    }
    if (world_objects == NULL) {
        world_objects = syMalloc(WORLD_OBJECTS * sizeof(struct world_object));
    }
    if (world_added == NULL) {
        world_added = syMalloc(ADDED_MAX * sizeof(struct world_object));
    }
    if (world_drawn == NULL) {
        world_drawn = syMalloc(DRAWN_MAX * sizeof(struct land_entry *));
    }
    if (world_pool == NULL) {
        world_pool = syMalloc(POOL_OBJECTS * sizeof(NJS_OBJECT));
    }
    if (world_pool_lent == NULL) {
        world_pool_lent = syMalloc(POOL_OBJECTS * sizeof(Uint8));
    }
    world_reset();
}

void world_set_near_radius(Float radius)
{
    world_near_radius = radius;
}

void world_reset_near_radius(void)
{
    world_near_radius = NEAR_RADIUS;
}

/* @hack spelling: world_added_count cleared through a function of its own,
 * where the store written at its one call would do */
static void world_clear_added(void)
{
    world_added_count = 0;
    /* @hack redundant: a return where the function ends */
    return; /* NOLINT(readability-redundant-control-flow) */
}

/* empty the world and forget the land and its task, as world_alloc says */
static void world_reset(void)
{
    world_count = 0;
    world_land = NULL;
    /* @hack spelling: the two nulls in one chained assignment, where
     * world_land_task's statement, then world_land_work's, would do */
    world_land_work = (struct task_work *)(world_land_task = NULL);
    world_land_shown = TRUE;
    world_land_enabled = TRUE;
    world_clear_added();
    world_drawn_count = 0;
    world_near_added_count = 0;
    world_near_land_count = 0;
    game_state.block_grid_loaded = FALSE;
    game_state.land_block_mask[0] = -1;
    game_state.land_block_mask[1] = -1;
    game_state.land_blocks[0] = 0;
    game_state.land_blocks[1] = 0;
}

int world_land_start(struct land_table *land)
{
    struct task *task;
    /* @hack spelling: work, a local for task->work, where task->work stored
     * in place would do */
    struct task_work *work;

    world_land_stop();
    task = task_create(TASK_WORK, WORLD_LAND_LEVEL, world_land_step,
                       "LandManager");
    if (task == NULL) {
        return 0;
    }
    work = task->work;
    world_reset();
    world_land_task = task;
    world_land_work = work;
    world_pool_free_all();
    task->draw = world_land_draw;
    task->destructor = world_land_destroy;
    world_near_radius = NEAR_RADIUS;
    world_land = land;
    /* @hack spelling: world_land tested where land holds the same, where
     * land != NULL would do */
    /* @hack spelling: the null land's path between two gotos, where an if
     * that returns would do */
    if (world_land != NULL) {
        goto load;
    }
    world_land_shown = FALSE;
    world_land_enabled = FALSE;
    /* @hack implicit: no return value for a null land, where return 0 would do:
     * no caller reads the result */
#ifdef NONMATCHING
    /* the hack's plain form: GCC refuses a return with no value in a function
     * that returns int */
    return 0;
#else
    return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
    /* @hack redundant: a goto no path reaches */
    goto load;
load:
    fun_8c059fb2();
    world_land_load_textures();
    return 1;
}

#ifdef NONMATCHING
/* Use the name's first extension, as the stage's original reader does. */
static Sint32 read_block_bitmap(const char *name)
{
    char path[GRID_NAME_BYTES];
    const char *dot;
    Uint32 capacity = (Uint32)(WORK_AREA_END - WORK_AREA_FILE);
    Sint32 size;

    if (name == NULL || strlen(name) >= sizeof(path)) {
        return -1;
    }
    dot = strchr(name, '.');
    if (dot == NULL || (size_t)(dot - name) > sizeof(path) - sizeof(".prs")) {
        return -1;
    }
    memcpy(path, name, (size_t)(dot - name));
    strcpy(path + (dot - name), ".prs");
    size = loader_read_prs_buffer(path, BLOCK_GRID_FILE, capacity);
    if (size <= 0) {
        size = loader_read_file_checked(name, BLOCK_GRID_FILE, capacity);
    }
    return size;
}

/* Short palettes are legal. All stored cell indices must name an entry. */
static Bool grid_indices_fit(const Uint8 *pixels, Uint32 size, Uint32 bits,
                             Uint32 colors)
{
    Uint32 mask = (1U << bits) - 1U;
    Uint32 i;
    Uint32 shift;

    if (colors <= mask) {
        for (i = 0; i < size; ++i) {
            for (shift = 0; shift < 8; shift += bits) {
                if (((pixels[i] >> shift) & mask) >= colors) {
                    return FALSE;
                }
            }
        }
    }
    return TRUE;
}

/* Pass the palette extent separately at the compiled entry point. Stage
 * callers use the array-sized declaration in world.h. */
#undef world_load_block_grid
int world_load_block_grid(const char *name,
                          const struct land_block_color *colors,
                          const NJS_POINT3 *origin, const NJS_POINT3 *cell,
                          Uint32 color_count)
{
    const struct bmp_file_header *file =
        (const struct bmp_file_header *)BLOCK_GRID_FILE;
    const struct bmp_info_header *info = BLOCK_GRID_INFO;
    const Uint8 *palette = BLOCK_GRID_PALETTE;
    struct land_block_color *mapping;
    const Uint8 *pixels;
    char *grid;
    Uint32 columns;
    Uint32 rows;
    Uint32 bits;
    Uint32 entries;
    Uint32 pixel_offset;
    Uint32 file_size;
    Uint32 row_bytes;
    Uint32 size;
    Uint32 rgb;
    Uint32 i;
    Uint32 j;
    Sint32 length;

    if (colors == NULL || color_count == 0 || origin == NULL || cell == NULL ||
        !(cell->x > 0.0F && cell->x <= FLT_MAX && cell->z > 0.0F &&
          cell->z <= FLT_MAX)) {
        return 0;
    }
    length = read_block_bitmap(name);
    if (length < 0 || (Uint32)length < sizeof(*file) + sizeof(*info) ||
        file->type != 0x4d42 || BMP_LONG(info->size) != sizeof(*info) ||
        info->planes != 1 || BMP_LONG(info->compression) != 0) {
        return 0;
    }
    columns = BMP_LONG(info->width);
    rows = BMP_LONG(info->height);
    bits = info->bitcount;
    if ((bits != 1 && bits != 4 && bits != 8) || columns == 0 || rows == 0 ||
        columns > INT_MAX / rows || columns % (32U / bits) != 0) {
        return 0;
    }
    entries = BMP_LONG(info->clrused);
    if (entries == 0) {
        entries = 1U << bits;
    }
    file_size = BMP_LONG(file->size);
    pixel_offset = BMP_LONG(file->offbits);
    if (entries > (1U << bits) || file_size > (Uint32)length ||
        pixel_offset < sizeof(*file) + sizeof(*info) + (4U * entries) ||
        pixel_offset > file_size) {
        return 0;
    }
    row_bytes = columns / (8U / bits);
    size = row_bytes * rows;
    if (size > file_size - pixel_offset) {
        return 0;
    }
    pixels = (const Uint8 *)BLOCK_GRID_FILE + pixel_offset;
    if (!grid_indices_fit(pixels, size, bits, entries)) {
        return 0;
    }

    grid = syMalloc(size);
    if (grid == NULL) {
        return 0;
    }
    mapping = syMalloc(entries * sizeof(*mapping));
    if (mapping == NULL) {
        syFree(grid);
        return 0;
    }
    memcpy(grid, pixels, size);
    for (i = 0; i < entries; ++i, palette += 4) {
        rgb =
            palette[0] | ((Uint32)palette[1] << 8) | ((Uint32)palette[2] << 16);
        mapping[i].rgb = rgb;
        /* Four retail maps use colors absent from their stage's mapping.
         * Two Eternal Engine colors picked up masks from adjacent storage.
         * Keep all terrain visible where the stage supplies no mask. */
        mapping[i].blocks = -1;
        for (j = 0; j < color_count; ++j) {
            if ((colors[j].rgb & BLOCK_RGB_BITS) == rgb) {
                mapping[i] = colors[j];
                break;
            }
        }
    }

    world_free_block_grid();
    block_grid = grid;
    block_grid_colors = mapping;
    block_grid_columns = (int)columns;
    block_grid_rows = (int)rows;
    block_grid_bits = (int)bits;
    block_grid_x = origin->x;
    block_grid_z = origin->z;
    block_grid_cell_x = cell->x;
    block_grid_cell_z = cell->z;
    game_state.block_grid_loaded = TRUE;
    return 1;
}

#else
int world_load_block_grid(char *name, struct land_block_color *colors,
                          NJS_POINT3 *origin, NJS_POINT3 *cell)
{
    char path[GRID_NAME_BYTES];
    /* @hack spelling: dot, a local for strchr's result, not passed to strcpy
     * in place */
    char *dot;
    struct bmp_file_header *file;
    /* @hack spelling: a local pointer to the info header, where its fields
     * could be read in place */
    struct bmp_info_header *info;
    struct land_block_color *out;
    struct land_block_color *color;
    Uint8 *palette;
    /* @hack spelling: pixels, a local for their address, not passed to
     * njMemCopy2 in place */
    char *pixels;
    Uint32 rgb;
    Uint32 used;
    int size;
    int count;
    int i;
    int j;

    strcpy(path, name);
    dot = strchr(path, '.');
    strcpy(dot, ".prs");
    if (loader_read_prs_get_size(path, BLOCK_GRID_FILE) <= 0 &&
        loader_read_file_get_size(name, BLOCK_GRID_FILE) <= 0) {
        return 0;
    }
    file = (struct bmp_file_header *)BLOCK_GRID_FILE;
    info = BLOCK_GRID_INFO;
    block_grid_columns = BMP_LONG(info->width);
    block_grid_rows = BMP_LONG(info->height);
    size = block_grid_columns * block_grid_rows;
    switch (info->bitcount) {
    case 1:
        size >>= 3;
        count = 1 << 1;
        block_grid_bits = 1;
        break;
    case 4:
        size >>= 1;
        count = 1 << 4;
        block_grid_bits = 4;
        break;
    case 8:
        count = 1 << 8;
        block_grid_bits = 8;
        break;
    /* @hack redundant: a case that does what the default does */
    case 24:
        return 0;
    default:
        return 0;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((block_grid = syMalloc(size)) == NULL) {
        return 0;
    }
    pixels = BLOCK_GRID_FILE + BMP_LONG(file->offbits);
    njMemCopy2(block_grid, pixels, size >> 1);
    used = BMP_LONG(info->clrused);
    /* @hack redundant: a sum of three header fields that nothing uses */
    (void)(file->type + info->planes + info->compression.lo);
    if (used != 0) {
        count = used;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((block_grid_colors =
             syMalloc(count * sizeof(struct land_block_color))) == NULL) {
        syFree(block_grid);
        block_grid = NULL;
        return 0;
    }
    out = block_grid_colors;
    palette = BLOCK_GRID_PALETTE;
    /* @hack spelling: i = i - 1, where i-- would do */
    for (i = count; i > 0; i = i - 1) {
        /* a palette color's bytes are its blue, green and red, then one
         * unused */
        rgb = *palette++;
        rgb += *palette++ << 8;
        rgb += *palette++ << 16;
        palette++;
        for (j = count, color = colors; j > 0; j--, color++) {
            if ((color->rgb & BLOCK_RGB_BITS) == rgb) {
                *out = *color;
                break;
            }
        }
        out++;
    }
    block_grid_x = origin->x;
    block_grid_z = origin->z;
    block_grid_cell_x = cell->x;
    block_grid_cell_z = cell->z;
    game_state.block_grid_loaded = TRUE;
    return 1;
}
#endif

Uint32 world_build_land(struct land_table *land, Uint32 buf)
{
    struct land_entry *entry;
    Uint32 next;
    int left;

    if (game_state.split_screen == 1) {
        NJS_POINT2 tiles[2];

        tiles[0].x = BUILD_CLIP_FIRST;
        tiles[0].y = BUILD_CLIP_FIRST;
        tiles[1].x = BUILD_CLIP_LAST;
        tiles[1].y = BUILD_CLIP_LAST;
        njUserClipping(NJD_CLIP_INSIDE, tiles);
    }
    land->flags &= ~LAND_AT_ORIGIN;
    buf = (buf + (BUILD_ALIGN - 1)) & ~(BUILD_ALIGN - 1);
    /* @hack spelling: entry and left set ahead of the four calls, not at the
     * loop's start */
    entry = land->entries;
    left = land->count;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(world_land->texlist);
    light_apply(LIGHT_LAND);
    njSetDirectClip();
    for (; left > 0; left--, entry++) {
        if ((entry->flags & LAND_DRAWN) && !(entry->flags & LAND_NO_BUILD)) {
            entry->object->evalflags &= ~NJD_EVAL_HIDE;
            if (entry->flags & LAND_NO_FOG) {
                njFogDisable();
            }
            next = buf + njCnkDirectObjectCompileSize(
                             (NJS_CNK_OBJECT *)entry->object);
            if (entry->flags & LAND_NO_FOG) {
                njFogEnable();
            }
            /* @bug logic: compares next with DEMO_AREA as numbers, so the
             * bound never holds for a buf in the RAM's alias at H'0C000000,
             * which four Chao binaries pass (CMP/HS R1,R8 at 0x8c02aa76) */
            if (next >= (Uint32)DEMO_AREA) {
                break;
            }
            if (entry->flags & LAND_NO_FOG) {
                njFogDisable();
            }
            njCnkDirectObjectCompile((NJS_CNK_OBJECT *)entry->object,
                                     (void *)buf);
            if (entry->flags & LAND_NO_FOG) {
                njFogEnable();
            }
            entry->built = (NJS_DIRECT_HEAD *)buf;
            buf = next;
            entry->flags |= LAND_BUILT;
        }
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    return buf;
}

/* the first of land's entries before self whose object shares object's
 * model and whose model a build compiled, or null */
static struct land_entry *world_find_built(struct land_table *land,
                                           struct land_entry *self,
                                           NJS_OBJECT *object)
{
    int left;
    struct land_entry *entry;

    for (left = land->count, entry = land->entries; left > 0 && entry != self;
         left--, entry++) {
        if ((entry->flags & LAND_DRAWN) && !(entry->flags & LAND_NO_BUILD) &&
            (entry->flags & LAND_BUILT) &&
            entry->object->model == object->model) {
            return entry;
        }
    }
    return NULL;
}

Uint32 world_build_land_at_origin(struct land_table *land, Uint32 buf)
{
    struct land_entry *entry;
    struct land_entry *built;
    Uint32 next;
    int left;

    if (game_state.split_screen == 1) {
        NJS_POINT2 tiles[2];

        tiles[0].x = BUILD_CLIP_FIRST;
        tiles[0].y = BUILD_CLIP_FIRST;
        tiles[1].x = BUILD_CLIP_LAST;
        tiles[1].y = BUILD_CLIP_LAST;
        njUserClipping(NJD_CLIP_INSIDE, tiles);
    }
    land->flags |= LAND_AT_ORIGIN;
    buf = (buf + (BUILD_ALIGN - 1)) & ~(BUILD_ALIGN - 1);
    /* @hack spelling: entry and left set ahead of the four calls, not at the
     * loop's start */
    entry = land->entries;
    left = land->count;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(world_land->texlist);
    light_apply(LIGHT_LAND);
    njSetDirectClip();
    /* @hack spelling: entry stepped ahead of left, not in world_build_land's
     * order */
    for (; left > 0; entry++, left--) {
        if ((entry->flags & LAND_DRAWN) && !(entry->flags & LAND_NO_BUILD)) {
            built = world_find_built(land, entry, entry->object);
            if (built != NULL) {
                entry->object->evalflags |= NJD_EVAL_HIDE;
                entry->built = built->built;
                entry->flags |= LAND_BUILT;
            } else {
                NJS_OBJECT copy;

                entry->object->evalflags &= ~NJD_EVAL_HIDE;
                copy = *entry->object;
                copy.pos[0] = 0.0F;
                copy.pos[1] = 0.0F;
                copy.pos[2] = 0.0F;
                copy.ang[0] = 0;
                copy.ang[1] = 0;
                copy.ang[2] = 0;
                copy.evalflags = NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG |
                                 NJD_EVAL_UNIT_SCL | NJD_EVAL_BREAK;
                if (entry->flags & LAND_NO_FOG) {
                    njFogDisable();
                }
                next =
                    buf + njCnkDirectObjectCompileSize((NJS_CNK_OBJECT *)&copy);
                if (entry->flags & LAND_NO_FOG) {
                    njFogEnable();
                }
                /* @bug logic: the bound never holds for a buf in the RAM's
                 * alias at H'0C000000, as in world_build_land (CMP/HS R1,R9 at
                 * 0x8c02acb6) */
                if (next >= (Uint32)DEMO_AREA) {
                    break;
                }
                if (entry->flags & LAND_NO_FOG) {
                    njFogDisable();
                }
                njCnkDirectObjectCompile((NJS_CNK_OBJECT *)&copy, (void *)buf);
                if (entry->flags & LAND_NO_FOG) {
                    njFogEnable();
                }
                entry->built = (NJS_DIRECT_HEAD *)buf;
                buf = next;
                entry->flags |= LAND_BUILT;
            }
        }
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    return buf;
}

int world_land_stop(void)
{
    if (world_land_task == NULL) {
        /* @hack implicit: no return value without a task, where return 0 would
         * do: no caller reads the result */
#ifdef NONMATCHING
        /* the hack's plain form: GCC refuses a return with no value in a
         * function that returns int */
        return 0;
#else
        return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
    }
    task_destroy(world_land_task);
    return 1;
}

void world_free_block_grid(void)
{
    if (block_grid != NULL) {
        syFree(block_grid);
        block_grid = NULL;
    }
    if (block_grid_colors != NULL) {
        syFree(block_grid_colors);
        block_grid_colors = NULL;
    }
    game_state.block_grid_loaded = FALSE;
}

static void world_land_step(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work at both
     * uses would do */
    struct task_work *work = task->work;

    switch (work->mode) {
    case WORLD_LAND_TO_RUN:
        work->mode = WORLD_LAND_RUN;
        /* fall through */
    case WORLD_LAND_RUN:
        world_land_run(task);
        break;
    /* @hack redundant: a default that returns for any other mode */
    default:
        return;
    }
}

/* count a hold on the land's textures, and load them with the first.
 * world_land_release_textures takes a hold back, and releases them with the
 * last. no land, or one without a PVM, counts nothing */
static void world_land_load_textures(void)
{
    /* @hack spelling: land, a local for world_land, where world_land read in
     * place would do */
    struct land_table *land = world_land;

    if (land != NULL && land->pvm != NULL) {
        if (land->texture_users == 0) {
            texture_load_pvm(land->pvm, land->texlist);
        }
        land->texture_users++;
    }
}

static void world_land_release_textures(void)
{
    /* @hack spelling: land, a local for world_land, where world_land read in
     * place would do */
    struct land_table *land = world_land;

    if (land != NULL && land->pvm != NULL && land->texture_users != 0) {
        land->texture_users--;
        if (land->texture_users == 0) {
            njReleaseTexture(land->texlist);
        }
    }
}

static void world_land_destroy(struct task *task)
{
    world_land_release_textures();
    world_reset();
    world_free_block_grid();
}

/* play the land's moving parts while the land is shown */
static void world_land_run(struct task *task)
{
    draw_attr_save();
    /* @hack spelling: world_land_shown compared with TRUE, where
     * world_land_shown alone would do */
    if (world_land_shown == TRUE) {
        world_land_play_motions();
    }
    draw_attr_restore();
}

/* draw the land while it is shown, but not for the cameras that
 * game_state.hidden_cameras holds. task_draw_all leaves out levels 2 to 5
 * alike */
static void world_land_draw(struct task *task)
{
    if (game_state.hidden_cameras == HIDDEN_CAMERA_0 &&
        camera_current_index == 0) {
        return;
    }
    if (game_state.hidden_cameras == HIDDEN_CAMERA_1 &&
        camera_current_index == 1) {
        return;
    }
    if (game_state.hidden_cameras == (HIDDEN_CAMERA_0 | HIDDEN_CAMERA_1)) {
        return;
    }
    draw_attr_save();
    /* @hack spelling: world_land_shown compared with TRUE, where
     * world_land_shown alone would do */
    if (world_land_shown == TRUE) {
        world_land_list_drawn();
        world_land_draw_entries();
        world_land_draw_motions();
    }
    draw_attr_restore();
}

/* draw the entries world_land_list_drawn listed, each at its object's position
 * and angles. while game_state.builds_drawn is 1, an entry under LAND_BUILT
 * draws its compiled model instead. a model compiled at the origin is placed
 * as its object is. any other draws where it was compiled */
static void world_land_draw_entries(void)
{
    struct land_entry **drawn;
    struct land_entry *entry;
    NJS_OBJECT *object;
    int left;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njSetTexture(world_land->texlist);
    njSetDirectClip();
    light_apply(LIGHT_LAND);
    drawn = world_drawn;
    if (world_land->flags & LAND_AT_ORIGIN) {
        for (left = world_drawn_count; left > 0; drawn++, left--) {
            entry = *drawn;
            object = entry->object;
            /* @hack spelling: each arm places the object itself, where
             * njTranslateEx and njRotateEx ahead of the test would do */
            if (game_state.builds_drawn == 1 && (entry->flags & LAND_BUILT)) {
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                njDirectDrawObject(entry->built);
                matrix_load_parent();
            } else {
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                if (entry->flags & LAND_NO_FOG) {
                    njFogDisable();
                }
                if (entry->flags & LAND_SIMPLE) {
                    njCnkSimpleDrawModel((NJS_CNK_MODEL *)object->model);
                } else {
                    njCnkEasyDrawModel((NJS_CNK_MODEL *)object->model);
                }
                if (entry->flags & LAND_NO_FOG) {
                    njFogEnable();
                }
                matrix_load_parent();
            }
        }
    } else {
        for (left = world_drawn_count; left > 0; drawn++, left--) {
            entry = *drawn;
            object = entry->object;
            if (game_state.builds_drawn == 1 && (entry->flags & LAND_BUILT)) {
                njDirectDrawObject(entry->built);
            } else {
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                if (entry->flags & LAND_NO_FOG) {
                    njFogDisable();
                }
                if (entry->flags & LAND_SIMPLE) {
                    njCnkSimpleDrawModel((NJS_CNK_MODEL *)object->model);
                } else {
                    njCnkEasyDrawModel((NJS_CNK_MODEL *)object->model);
                }
                if (entry->flags & LAND_NO_FOG) {
                    njFogEnable();
                }
                matrix_load_parent();
            }
        }
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static void world_land_draw_motions(void)
{
    struct land_motion *motion = world_land->motions;
    int left;

    if (motion == NULL) {
        return;
    }
    /* @hack spelling: left set ahead of the two calls, not at the loop's
     * start */
    left = world_land->motion_count;
    njSetTexture(world_land->texlist);
    draw_control_set(DRAW_CONTROL_SHADOW);
    for (; left > 0; left--, motion++) {
        njCnkEasyDrawMotion(motion->object, motion->motion, motion->frame);
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static void world_land_play_motions(void)
{
    struct land_motion *motion = world_land->motions;
    Float frame;
    /* @hack spelling: speed, a local for motion->speed, where motion->speed
     * read in place would do */
    Float speed;
    int left;

    if (motion == NULL) {
        return;
    }
    for (left = world_land->motion_count; left > 0; left--, motion++) {
        speed = motion->speed;
        frame = motion->frame + speed;
        if (frame >= motion->frames - speed - 1.0F) {
            frame = 0.0F;
        }
        motion->frame = frame;
    }
}

void world_hide_land(void)
{
    world_land_shown = FALSE;
}

void world_show_land(void)
{
    world_land_shown = TRUE;
}

/* the blocks the block grid gives the cell under x and z. it returns -1, every
 * block, while game_state.block_grid_loaded is FALSE. it returns blocks where
 * no grid is allocated or the point lies off it */
static Sint32 world_blocks_at(Sint32 blocks, Float x, Float z)
{
    /* @hack type: the cells as char, though each use takes a cell as
     * unsigned, where Uint8 here and in block_grid, without the casts, would
     * do */
    /* @hack spelling: grid, a local for block_grid stepped to the cell,
     * where the cell read as block_grid[...] would do */
    char *grid;
    Float column;
    Float row;
    unsigned int offset;
    unsigned int bit;
    int stride;
#ifdef NONMATCHING
    /* the cell's row as an integer, which the test below bounds */
    unsigned int row_index;
#endif

    if (!game_state.block_grid_loaded) {
        return -1;
    }
    if (block_grid == NULL || block_grid_colors == NULL) {
        return blocks;
    }
    column = x - block_grid_x;
    row = z - block_grid_z;
    column /= block_grid_cell_x;
    row /= -block_grid_cell_z;
#ifdef NONMATCHING
    /* this side repairs the two tags below: it refuses a column or a row at
     * the grid's width or height. it tests the Floats first, as a large width
     * or height rounds either way as a Float, and then the integers */
    if (!(column >= 0.0F && column <= (Float)block_grid_columns &&
          row >= 0.0F && row <= (Float)block_grid_rows)) {
        return blocks;
    }
    offset = (unsigned int)column;
    row_index = (unsigned int)row;
    if (offset >= (unsigned int)block_grid_columns ||
        row_index >= (unsigned int)block_grid_rows) {
        return blocks;
    }
    grid = block_grid;
#else
    /* @bug memory: a point whose column is block_grid_columns exactly
     * passes the test (FCMP/GT at 0x8c02b262). it reads the next row's first
     * cell, which lies past the grid when the row is the last */
    /* @bug memory: a point whose row is block_grid_rows exactly passes the
     * test (FCMP/GT at 0x8c02b27c), and reads a cell past the grid's last
     * row */
    if (column > (Float)block_grid_columns || column < 0.0F ||
        row > (Float)block_grid_rows || row < 0.0F) {
        return blocks;
    }
    grid = block_grid;
    offset = (unsigned int)njRoundOff(column);
#endif
    /* @hack spelling: offset = offset >> n, where offset >>= n in either
     * case would do */
    switch (block_grid_bits) {
    case 1:
        bit = offset & 7;
        offset = offset >> 3;
        break;
    case 4:
        bit = offset & 1;
        offset = offset >> 1;
        break;
    case 8:
        bit = 0;
        break;
    default:
        return 0;
    }
    /* @hack spelling: stride set by a second switch on block_grid_bits,
     * where the first switch's cases would do */
    switch (block_grid_bits) {
    case 1:
        stride = block_grid_columns >> 3;
        break;
    case 4:
        stride = block_grid_columns >> 1;
        break;
    case 8:
        stride = block_grid_columns;
        break;
    /* @hack redundant: a default no path reaches, as the first switch
     * returns for any other width */
    default:
        return 0;
    }
    /* @hack spelling: grid = grid + ..., where grid += ... would do */
#ifdef NONMATCHING
    /* this side steps by the integer row the test above bounded, where the
     * matching side multiplies a rounded Float */
    grid += row_index * (unsigned int)stride + offset;
#else
    grid = grid + (offset + (unsigned int)(njRoundOff(row) * (Float)stride));
#endif
    /* @hack spelling: the cell held in blocks, where a char of its own would
     * do */
    blocks = *grid;
    switch (block_grid_bits) {
    case 1:
        while (bit < 7) {
            blocks = (unsigned char)blocks >> 1;
            bit++;
        }
        blocks &= 1;
        break;
    case 4:
        if (bit == 0) {
            blocks = (unsigned char)blocks >> 4;
        }
        blocks &= 15;
        break;
    case 8:
        break;
    /* @hack redundant: a default no path reaches, as the first switch
     * returns for any other width */
    default:
        return 0;
    }
    /* @hack spelling: the result stored in blocks and returned from there,
     * not returned in place */
    blocks = block_grid_colors[(unsigned char)blocks].blocks;
    return blocks;
}

/* list in world_drawn the entries the land draws this frame, up to DRAWN_MAX.
 * it takes them from the land's first drawn entries, or all of them where drawn
 * is negative. an entry must have LAND_DRAWN. its sphere must come within half
 * the clip of a point half the clip ahead of the camera. under LAND_FAR, it
 * must come within _nj_far_clip_ instead. an entry with blocks must share one
 * with the camera's blocks. those are the camera's land_block_mask less the
 * blocks the grid leaves out where it stands. the list stores them in the
 * camera's land_blocks */
static void world_land_list_drawn(void)
{
    /* @hack spelling: entry set where it is declared, not at the loop's
     * start */
    struct land_entry *entry = world_land->entries;
    Float clip = world_land->clip;
    Sint32 blocks;
    NJS_VECTOR offset;
    struct camera_place *camera;
    /* @hack spelling: x and z, locals for the camera's x and z, not read in
     * place */
    Float x;
    Float z;
    Float px;
    Float py;
    Float pz;
    Float reach;
    Float delta;
    Float distance;
    int left;
    int count;
    /* @hack spelling: flags, a local for entry->flags, where entry->flags
     * read in place would do */
    Uint32 flags;

    /* @hack spelling: the mask picked by a test of camera_current_index,
     * where land_block_mask[camera_current_index] would do for a
     * camera_current_index of 0 or 1 */
    if (camera_current_index == 0) {
        blocks = game_state.land_block_mask[0];
    } else {
        blocks = game_state.land_block_mask[1];
    }
    /* @hack spelling: offset's x and y zeroed in one chained assignment, where
     * x's statement, then y's, would do */
    offset.x = offset.y = 0.0F;
#ifdef NONMATCHING
    /* the port's 16:9 grows the clip, so that the sphere below reaches as
     * far along the view's side edges as it does at 4:3. an edge leaves the
     * sphere at the clip times the cosine of its angle from the view's
     * middle */
    if (port_wide) {
        Float half = _nj_screen_.w * 0.5F;
        Float dist = _nj_screen_.dist;
        Float scale_x = dist * WIDE_ASPECT;

        clip *= dist * njSqrt((scale_x * scale_x) + (half * half)) /
                (scale_x * njSqrt((dist * dist) + (half * half)));
    }
#endif
    /* @hack spelling: clip halved after offset.z takes its negated half, not
     * halved first, with offset.z = -clip */
    offset.z = -(clip * 0.5F);
    clip *= 0.5F;
    njPushMatrix(&_nj_unit_matrix_);
    /* @hack spelling: the camera read again for each turn and for camera,
     * not read once into camera ahead of the turns */
    njRotateY(NULL, camera_views[camera_current_index]->ang.y);
    njRotateX(NULL, camera_views[camera_current_index]->ang.x);
    njCalcPoint(NULL, &offset, &offset);
    njPopMatrixEx();
    camera = camera_views[camera_current_index];
    x = camera->pos.x;
    px = x + offset.x;
    py = camera->pos.y + offset.y;
    z = camera->pos.z;
    pz = z + offset.z;
    blocks &= world_blocks_at(blocks, x, z);
    /* @hack spelling: the blocks stored by a test of camera_current_index,
     * where land_blocks[camera_current_index] would do for a
     * camera_current_index of 0 or 1 */
    if (camera_current_index == 0) {
        game_state.land_blocks[0] = blocks;
    } else {
        game_state.land_blocks[1] = blocks;
    }
    count = 0;
    left = world_land->drawn >= 0 ? world_land->drawn : world_land->count;
    /* @hack spelling: left-- as the loop's test, where left > 0 with left--
     * in the step would do */
    for (; left--; entry++) {
        if (entry->blocks != 0 && (blocks & entry->blocks) == 0) {
            continue;
        }
        flags = entry->flags;
        if (!(flags & LAND_DRAWN)) {
            continue;
        }
        reach = entry->radius;
        /* @hack spelling: the test negated, with clip's arm first, where
         * flags & LAND_FAR with the arms swapped would do */
        if (!(flags & LAND_FAR)) {
            reach += clip;
        } else {
            reach += _nj_far_clip_;
        }
        delta = px - entry->center.x;
        if (delta > reach) {
            continue;
        }
        distance = delta * delta;
        delta = py - entry->center.y;
        if (delta > reach) {
            continue;
        }
#ifdef NONMATCHING
        /* SHC adds each square below by FMAC, which rounds the sum once */
        distance = __builtin_fmaf(delta, delta, distance);
#else
        distance += delta * delta;
#endif
        delta = pz - entry->center.z;
        if (delta > reach) {
            continue;
        }
#ifdef NONMATCHING
        /* SHC adds this square by FMAC too */
        distance = __builtin_fmaf(delta, delta, distance);
#else
        distance += delta * delta;
#endif
        /* @hack spelling: reach squared in place, where distance > reach *
         * reach would do */
        reach *= reach;
        if (distance > reach) {
            continue;
        }
        world_drawn[count++] = entry;
        /* @hack spelling: the count stored and the function left at
         * DRAWN_MAX, where a break to the store after the loop would do */
        if (count >= DRAWN_MAX) {
            world_drawn_count = count;
            return;
        }
    }
    world_drawn_count = count;
}

void world_list_near(Float x, Float y, Float z, Float radius)
{
    struct world_object *out = world_objects;
    struct world_object *added;
    NJS_OBJECT *object;
    struct land_entry *entry;
    NJS_POINT3 center;
    Float *coord;
    Float dx;
    Float dy;
    Float dz;
    Float distance;
    Float reach;
    Float scale;
    /* @hack type: count as Sint16, where an int would do */
    Sint16 count;

    world_count = 0;
    njPushMatrixEx();
    for (added = &world_near_added[world_near_added_count - 1];
         added >= world_near_added; added--) {
        object = added->object;
        if (added->flags & WORLD_FLAT) {
            dx = x - object->pos[0] - object->model->center.x;
            dy = 0.0F;
            dz = z - object->pos[2] - object->model->center.z;
        } else if (!(added->flags & WORLD_TURNED)) {
            dx = x - object->pos[0] - object->model->center.x;
            dy = y - object->pos[1] - object->model->center.y;
            dz = z - object->pos[2] - object->model->center.z;
        } else {
            njUnitMatrix(NULL);
            if (object->evalflags & NJD_EVAL_ZXY_ANG) {
                njRotateY(NULL, object->ang[1]);
                njRotateX(NULL, object->ang[0]);
                njRotateZ(NULL, object->ang[2]);
            } else {
                njRotateZ(NULL, object->ang[2]);
                njRotateY(NULL, object->ang[1]);
                njRotateX(NULL, object->ang[0]);
            }
            njCalcPoint(NULL, &object->model->center, &center);
            dx = x - object->pos[0] - center.x;
            dy = y - object->pos[1] - center.y;
            dz = z - object->pos[2] - center.z;
        }
        /* @hack spelling: distance, a local for the sum the test reads once,
         * where the sum in the test would do */
#ifdef NONMATCHING
        /* SHC's sum: dy's square by FMUL, then dx's and dz's added by FMAC */
        distance = __builtin_fmaf(dz, dz, __builtin_fmaf(dx, dx, dy * dy));
#else
        distance = (dx * dx) + (dy * dy) + (dz * dz);
#endif
        /* @hack spelling: ADDED_MARGIN added in each arm, not once after the
         * test */
        if (object->evalflags & NJD_EVAL_UNIT_SCL) {
            reach = object->model->r + radius + ADDED_MARGIN;
        } else {
            scale = object->scl[0];
            if (scale > object->scl[1]) {
                scale = object->scl[1];
            }
            if (scale > object->scl[2]) {
                scale = object->scl[2];
            }
#ifdef NONMATCHING
            /* SHC adds the scaled radius to radius by FMAC */
            reach =
                __builtin_fmaf(object->model->r, scale, radius) + ADDED_MARGIN;
#else
            reach = (object->model->r * scale) + radius + ADDED_MARGIN;
#endif
        }
        /* @hack spelling: reach squared in place, where distance < reach *
         * reach would do */
        reach *= reach;
        if (distance < reach) {
            /* @hack spelling: the record copied member by member, flags
             * last, where *out = *added would do */
            out->object = object;
            out->owner = added->owner;
            out->flags = added->flags;
            world_count++;
            out++;
            if (world_count >= WORLD_OBJECTS) {
                njPopMatrixEx();
                return;
            }
        }
    }
    njPopMatrixEx();
    /* @hack spelling: world_land_enabled compared with TRUE, where
     * world_land_enabled alone would do */
    if (world_land_enabled == TRUE && world_land != NULL) {
        for (entry = world_near_land, count = world_near_land_count; count > 0;
             entry++, count--) {
            if (!(entry->flags & LAND_COLLIDES)) {
                continue;
            }
            /* @hack spelling: the entry's center walked as Floats, where
             * member reads would do */
            coord = &entry->center.x;
            dx = *coord++;
            dy = *coord++;
            dz = *coord;
            /* @hack spelling: dx = dx - x, where dx -= x would do */
            dx = dx - x;
            dz -= z;
            /* @hack spelling: each difference squared in place, where the
             * squares taken in njSqrt's argument would do */
            dx *= dx;
            if (entry->flags & WORLD_FLAT) {
                dy = 0.0F;
            } else {
                dy -= y;
                dy *= dy;
            }
            dz *= dz;
            /* @hack spelling: distance and reach, locals for the two sides
             * of the test, where both in the test would do */
            distance = njSqrt(dx + dy + dz) - radius;
            reach = entry->radius;
            if (distance < reach) {
                out->flags = entry->flags;
                out->owner = NULL;
                out->object = entry->object;
                world_count++;
                out++;
                if (world_count >= WORLD_OBJECTS) {
                    return;
                }
            }
        }
    }
}

void world_collect_near_players(void)
{
    struct world_object *out = world_objects;
    struct world_object *near_added;
    struct world_object *added;
    struct task *owner;
    NJS_OBJECT *object;
    struct land_entry *entry;
    struct land_entry *near_land;
    struct task_work **players;
    NJS_POINT3 positions[2];
    NJS_POINT3 center;
    /* @hack spelling: pos, the loops' cursor, also takes the points' stores,
     * where positions[0] and positions[1] stored in place would do */
    NJS_POINT3 *pos;
    /* @hack spelling: object_pos and model_center stepped as Float cursors,
     * where object_pos[0] + model_center[0] and so on would do */
    Float *object_pos;
    Float *model_center;
    Float *coord;
    Float cx;
    Float cy;
    Float cz;
    Float dx;
    Float dy;
    Float dz;
    Float distance;
    Float reach;
    Float radius;
    int player_count;
    int i;
    int count;

    players = player_work;
    pos = positions;
    *pos = camera_views[0]->pos;
    player_count = 1;
    /* @hack spelling: players stepped to player 1, where players[0] and
     * players[1] would do */
    if (*players != NULL) {
        *pos = (*players)->pos;
        players++;
        if (*players != NULL) {
            pos++;
            *pos = (*players)->pos;
            player_count++;
        }
    } else {
        players++;
        if (*players != NULL) {
            *pos = (*players)->pos;
        }
    }
    world_count = 0;
    world_near_added_count = 0;
    near_added = world_near_added;
    njPushMatrixEx();
    for (added = &world_added[world_added_count - 1]; added >= world_added;
         added--) {
        owner = added->owner;
        object = added->object;
        if (owner->step != task_destroy &&
            (owner->parent == NULL || owner->parent->step != task_destroy) &&
            (owner->work->flags & WORLD_ADDED)) {
            if (!(added->flags & WORLD_TURNED)) {
                object_pos = object->pos;
                model_center = &object->model->center.x;
            } else {
                njUnitMatrix(NULL);
                if (object->evalflags & NJD_EVAL_ZXY_ANG) {
                    njRotateY(NULL, object->ang[1]);
                    njRotateX(NULL, object->ang[0]);
                    njRotateZ(NULL, object->ang[2]);
                } else {
                    njRotateZ(NULL, object->ang[2]);
                    njRotateY(NULL, object->ang[1]);
                    njRotateX(NULL, object->ang[0]);
                }
                njCalcPoint(NULL, &object->model->center, &center);
                object_pos = object->pos;
                model_center = &center.x;
            }
            /* @hack redundant: a test of object_pos that never runs */
            if (0) {
                if (object_pos) {
                }
            }
            cx = *object_pos++ + *model_center++;
            cy = *object_pos++ + *model_center++;
            cz = *object_pos + *model_center;
            radius = world_near_radius;
            for (i = player_count, pos = positions; i > 0;
                 radius = SECOND_RADIUS, i--, pos++) {
                /* @hack spelling: the point walked as Floats, where pos->x,
                 * ->y and ->z would do */
                coord = &pos->x;
                dx = *coord++ - cx;
                dy = *coord++ - cy;
                dz = *coord - cz;
                /* @hack spelling: distance, a local for the sum the test
                 * reads once, where the sum in the test would do */
#ifdef NONMATCHING
                /* SHC's sum: dy's square by FMUL, then dx's and dz's added by
                 * FMAC */
                distance =
                    __builtin_fmaf(dz, dz, __builtin_fmaf(dx, dx, dy * dy));
#else
                distance = (dx * dx) + (dy * dy) + (dz * dz);
#endif
                reach = object->model->r + radius;
                /* @hack spelling: reach squared in place, where distance <
                 * reach * reach would do */
                reach *= reach;
                if (distance < reach) {
                    *near_added = *added;
                    near_added++;
                    world_near_added_count++;
                    /* @hack spelling: the record copied member by member,
                     * flags last, where *out = *added would do */
                    /* @hack spelling: owner read again from added, though
                     * the local owner holds the same, where out->owner =
                     * owner would do */
                    out->object = object;
                    out->owner = added->owner;
                    out->flags = added->flags;
                    world_count++;
                    out++;
                    if (world_near_added_count >= NEAR_ADDED_MAX) {
                        goto added_done;
                    }
                    if (world_count >= WORLD_OBJECTS) {
                        njPopMatrixEx();
                        return;
                    }
                    break;
                }
            }
        }
    }
added_done:
    njPopMatrixEx();
    world_near_land_count = 0;
    near_land = world_near_land;
    /* @hack spelling: world_land_enabled compared with TRUE, where
     * world_land_enabled alone would do */
    if (world_land_enabled == TRUE && world_land != NULL) {
        entry = world_land->entries;
        count = world_land->drawn;
        if (count >= 0) {
            entry += count;
            count = world_land->count - count;
        } else {
            count = world_land->count;
        }
        for (; count > 0; entry++, count--) {
            if (!(entry->flags & LAND_COLLIDES)) {
                continue;
            }
            /* @hack redundant: a store of 2 to world_land_enabled that never
             * runs */
            if (0) {
                world_land_enabled = 2;
            }
            radius = world_near_radius;
            for (i = player_count, pos = positions; i > 0;
                 radius = SECOND_RADIUS, i--, pos++) {
                /* @hack spelling: the center and the point walked as Floats,
                 * where member reads would do */
                coord = &entry->center.x;
                /* @hack spelling: coord stepped in a statement of its own,
                 * where dx = *coord++ would do */
                dx = *coord;
                coord++;
                dy = *coord++;
                dz = *coord;
                coord = &pos->x;
                dx -= *coord++;
                dy -= *coord++;
                dz -= *coord;
                /* @hack spelling: each difference squared in place, where
                 * the squares taken in njSqrt's argument would do */
                dx *= dx;
                dy *= dy;
                dz *= dz;
                if (entry->flags & WORLD_FLAT) {
                    dy = 0.0F;
                }
                /* @hack spelling: distance and reach, locals for the two
                 * sides of the test, where both in the test would do */
                distance = njSqrt(dx + dy + dz) - radius;
                reach = entry->radius;
                if (distance < reach) {
                    *near_land = *entry;
                    near_land++;
                    world_near_land_count++;
                    out->flags = entry->flags;
                    out->owner = NULL;
                    out->object = entry->object;
                    world_count++;
                    out++;
                    if (world_near_land_count >= NEAR_LAND_MAX) {
                        return;
                    }
                    if (world_count >= WORLD_OBJECTS) {
                        return;
                    }
                    break;
                }
            }
        }
    }
}

void world_disable_land(void)
{
    world_land_enabled = FALSE;
}

void world_enable_land(void)
{
    world_land_enabled = TRUE;
}

void world_add(Sint32 flags, struct task *task, NJS_OBJECT *object)
{
    /* @hack spelling: count, a pointer to world_added_count, where
     * world_added_count read directly would do */
    /* @hack redundant: register on count */
    register Sint16 *count = &world_added_count;
    /* @hack type: index as Sint16, where an int would do */
    Sint16 index = *count;
    NJS_MODEL *model;
    /* @hack spelling: distance, a local for the sum the test reads once,
     * where the sum in the test would do */
    Float distance;
    /* @hack spelling: x, y and z, locals for the center's members, where
     * model->center read in place would do */
    Float x;
    Float y;
    Float z;

    if (index < ADDED_MAX) {
        model = object->model;
        x = model->center.x;
        y = model->center.y;
        z = model->center.z;
#ifdef NONMATCHING
        /* SHC's sum: y's square by FMUL, then x's and z's added by FMAC */
        distance = __builtin_fmaf(z, z, __builtin_fmaf(x, x, y * y));
#else
        distance = (x * x) + (y * y) + (z * z);
#endif
        if (distance >= OFF_CENTER * OFF_CENTER) {
            if ((object->ang[0] | object->ang[1] | object->ang[2]) &
                TURNED_ANGLE_BITS) {
                flags |= WORLD_TURNED;
            }
        }
        /* @hack spelling: the flags stored through index and the other two
         * members through *count, which holds the same value until the last
         * store, where all three through *count would do */
        world_added[index].flags = flags;
        world_added[*count].object = object;
        world_added[(*count)++].owner = task;
        task->work->flags |= WORLD_ADDED;
    }
}

void world_remove(struct task *task, NJS_OBJECT *object)
{
    /* @hack type: i as Sint16, where an int would do */
    Sint16 i;
    /* @hack spelling: j, a second counter for the shift, which starts where
     * the search left i, where i would do */
    /* @hack type: j as Sint16, where an int would do */
    Sint16 j;

    for (i = world_added_count - 1; i >= 0; i--) {
        if (task == world_added[i].owner) {
            break;
        }
    }
    if (i < 0) {
        return;
    }
    world_added_count--;
    for (j = i; j < world_added_count; j++) {
        /* @hack spelling: the record copied member by member, flags last,
         * where world_added[j] = world_added[j + 1] would do */
        world_added[j].object = world_added[j + 1].object;
        world_added[j].owner = world_added[j + 1].owner;
        world_added[j].flags = world_added[j + 1].flags;
    }
    collision_mesh_forget(object);
}

/* take back every object world_pool_alloc lent */
static void world_pool_free_all(void)
{
    /* @hack type: i as Sint16, where an int would do */
    Sint16 i;

    for (i = POOL_OBJECTS - 1; i >= 0; i--) {
        world_pool_lent[i] = FALSE;
    }
}

NJS_OBJECT *world_pool_alloc(void)
{
    /* @hack type: i as Sint16, where an int would do */
    Sint16 i;

    for (i = POOL_OBJECTS - 1; i >= 0; i--) {
        /* @hack spelling: == FALSE, where !world_pool_lent[i] would do */
        if (world_pool_lent[i] == FALSE) {
            break;
        }
    }
    /* @hack spelling: the null return last, after the lending, where an
     * early return for i < 0 would do */
    if (i >= 0) {
        world_pool_lent[i] = TRUE;
        world_pool[i].model = NULL;
        world_pool[i].child = NULL;
        world_pool[i].sibling = NULL;
        return &world_pool[i];
    }
    return NULL;
}

void world_pool_free(NJS_OBJECT *object)
{
    /* @hack type: i as Sint16, where an int would do */
    Sint16 i;

    for (i = POOL_OBJECTS - 1; i >= 0; i--) {
        if (object == &world_pool[i]) {
            break;
        }
    }
    if (i >= 0) {
        world_pool_lent[i] = FALSE;
    }
}

void world_reset_gravity(void)
{
    world_gravity.x = 0.0F;
    world_gravity.y = -1.0F;
    world_gravity.z = 0.0F;
    world_gravity_ang_x = 0;
    world_gravity_ang_z = 0;
    collision_set_up_axis(COLLISION_UP_Y);
}

void world_tilt_gravity(Angle ang_x, Angle ang_z)
{
    /* @hack spelling: the vector stored backward through a Float cursor,
     * where stores to world_gravity.z, .y and .x would do */
    Float *gravity = &world_gravity.x;
    Float x;
    Float y;
    Float z;

    x = -(njCos(-ang_x) * njSin(-ang_z));
    y = -(njCos(-ang_x) * njCos(-ang_z));
    z = njSin(-ang_x);
    gravity += 3;
    *--gravity = z;
    *--gravity = y;
    *--gravity = x;
    world_gravity_ang_x = (Angle)(asinf(-z) * ANGLE_PER_RADIAN);
    world_gravity_ang_z = -(Angle)(atan2f(-x, -y) * ANGLE_PER_RADIAN);
}
