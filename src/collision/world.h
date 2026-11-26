/* the collision world: the land, which it draws too, the objects tasks put
 * in it, and gravity */
#ifndef COLLISION_WORLD_H
#define COLLISION_WORLD_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* in a world object's flags: a land entry that collides. a land entry with
 * neither it nor a bit of WORLD_FLAT stays out of world_collect_near_players'
 * and world_list_near's lists */
#define WORLD_SOLID 0x1
/* water, and water that holds a player back more. WORLD_FLAT is either */
#define WORLD_FLAT_THIN 0x2
#define WORLD_FLAT_THICK 0x2000
#define WORLD_FLAT (WORLD_FLAT_THIN | WORLD_FLAT_THICK)
/* WORLD_FLAT_THIN water is deep where a ground_probe's above_y lies more than
 * this from its below_y */
#define WATER_DEEP 7.0F
/* the object is turned, and its model's center lies off its origin. world_add
 * sets it where the center lies 15 or more from the origin. one of the
 * object's angles, as a Ninja angle mod 0x10000, must also be 16 or more.
 * world_collect_near_players turns the center by the object's angles before it
 * measures to the object. world_list_near does too, but for water */
#define WORLD_TURNED 0x10000000
/* a player who runs or lands on the object raises puffs of smoke, sand or mud
 * from it */
#define WORLD_DUSTY 0x20
/* an object no player climbs or holds by its ledge */
#define WORLD_NO_CLIMB 0x80
/* ground whose slope doesn't pull a player who stands on it */
#define WORLD_UPRIGHT 0x100
/* no player hangs from a ledge of the object's ground */
#define GROUND_NO_LEDGE 0x200
/* ground a player clings to: the player falls into it, not along
 * world_gravity */
#define GROUND_CLING 0x100000
/* ground on which player_accel_climb reverses the stick's sideways push. it
 * does so while the camera's direction lies within a quarter turn of the
 * player's up */
#define GROUND_CLIMB_REVERSED 0x4000
/* ground that guides a player along it. player_accel_ground steers a pushed
 * player downhill on it while player->level is below 0.94. a wall of it never
 * stops a head-on motion. where a wall deflects a player whose last ground had
 * it, the player's motion turns onto the wall's line */
#define GROUND_GUIDES 0x80000
/* how well a player grips the ground, least first. GROUND_SLIPPERY is the three
 * that grip less than plain ground */
#define GROUND_GRIP_LEAST 0x4
#define GROUND_GRIP_LOW 0x8
#define GROUND_GRIP_HALF 0x10
#define GROUND_GRIP_MORE 0x200000
#define GROUND_SLIPPERY (GROUND_GRIP_LEAST | GROUND_GRIP_LOW | GROUND_GRIP_HALF)
/* a wall that turns a player who runs into it aside, and never crashes it */
#define GROUND_DEFLECTS 0x1000
/* a wall that crashes a player who runs into it fast enough */
#define GROUND_CRASHES 0x400000
/* no object with either bit stands a player */
#define STAND_REFUSES (GROUND_DEFLECTS | GROUND_CRASHES)
/* an object a player in the air passes through. the player's collision takes it
 * only where the step began on the ground or on a shape */
#define GROUND_NO_LANDING 0x1000000
/* an object that moves. its collision mesh is made anew each step. an object
 * without the bit keeps its mesh while it stays near. no player climbs it or
 * holds it by its ledge */
#define WORLD_MOVING 0x08000000
/* ground that hurts a player who stands on it */
#define WORLD_HURTS 0x400
/* ground on which footsteps_create's task plays a player's footsteps */
#define GROUND_FOOTSTEPS 0x800
/* the object's size class, from 0 to WORLD_SIZE_CLASSES - 1. the callers of
 * world_add each measure the size their own way. polygon_find_model_sides and
 * polygon_find_near_heights test a triangle within a reach the class sets. no
 * image calls either, and no other code acts on the class */
#define WORLD_SIZE_CLASSES 4
#define WORLD_SIZE_CLASS_SHIFT 29
#define WORLD_SIZE_CLASS(class) ((class) << WORLD_SIZE_CLASS_SHIFT)
/* the bits that hold the class */
#define WORLD_SIZE_CLASS_BITS WORLD_SIZE_CLASS(WORLD_SIZE_CLASSES - 1)
/* the size class flags holds */
#define WORLD_SIZE_CLASS_OF(flags)                                             \
    (((flags) & WORLD_SIZE_CLASS_BITS) >> WORLD_SIZE_CLASS_SHIFT)
/* the largest size of each class but 0, which takes any larger size */
#define WORLD_SIZE_CLASS_1_MAX 200.0F
#define WORLD_SIZE_CLASS_2_MAX 100.0F
#define WORLD_SIZE_CLASS_3_MAX 30.0F
/* the same limits a tenth as large, by which some callers class their models */
#define WORLD_SMALL_SIZE_CLASS_1_MAX 20.0F
#define WORLD_SMALL_SIZE_CLASS_2_MAX 10.0F
#define WORLD_SMALL_SIZE_CLASS_3_MAX 3.0F

/* an object of the collision world: a land entry's, or one a task put in it */
struct world_object {
    Sint32 flags;
    NJS_OBJECT *object;
    struct task *owner; /* the task that put it in, or null for the land */
};

/* the bits by which a land entry collides, as solid or as water */
#define LAND_COLLIDES (WORLD_SOLID | WORLD_FLAT)
/* in a land entry's flags, beside its world flags. LAND_NO_FOG shares its bit
 * with GROUND_CRASHES, and LAND_SIMPLE with GROUND_NO_LANDING */
#define LAND_NO_FOG 0x00400000 /* compiled and drawn with fog off */
/* drawn out to _nj_far_clip_ in place of half the land's clip */
#define LAND_FAR 0x00800000
/* drawn by njCnkSimpleDrawModel, not njCnkEasyDrawModel */
#define LAND_SIMPLE 0x01000000
#define LAND_BUILT 0x02000000 /* its model is compiled, into built */
#define LAND_NO_BUILD 0x04000000 /* its model is never compiled */
/* the land's draw takes it, and world_build_land compiles its model */
#define LAND_DRAWN 0x80000000

/* an object of the land, and the sphere round it */
struct land_entry {
    NJS_POINT3 center;
    Float radius;
    NJS_OBJECT *object; /* a drawn entry's object is a chunk object */
    NJS_DIRECT_HEAD *built; /* its compiled model, under LAND_BUILT */
    /* the land blocks it is drawn in, a bit each, or 0 for every block. the
     * land's draw takes it only while the camera's land_blocks hold one */
    Sint32 blocks;
    Uint32 flags;
};

/* a part of the land that moves */
struct land_motion {
    Float frame;
    Float speed; /* the frames it advances each step */
    /* a step sets frame to 0 where it would reach frames - speed - 1 or
     * more */
    Float frames;
    NJS_CNK_OBJECT *object;
    NJS_MOTION *motion;
};

/* in a land's flags: its entries' models were compiled at the origin. the
 * land's draw places each at its object's position and angles */
#define LAND_AT_ORIGIN 0x2

/* a land, which an overlay holds and world_land_start takes */
struct land_table {
    Sint16 count; /* its entries */
    /* the land's draw takes the first drawn entries, and
     * world_collect_near_players the rest. where drawn is negative, both take
     * every entry */
    Sint16 drawn;
    Sint16 unk_04;
    Sint16 motion_count;
    Sint16 flags;
    /* how many of the land's starts hold its textures. world_land_start counts
     * one, and its task's end takes it back. the textures load with the first
     * and are released with the last */
    Sint16 texture_users;
    /* the land's draw measures from a point half the clip ahead of the camera.
     * it takes an entry whose sphere comes within half the clip of that
     * point */
    Float clip;
    struct land_entry *entries;
    struct land_motion *motions; /* motion_count of them, or null */
    char *pvm; /* the PVM its textures load from, or null for none */
    NJS_TEXLIST *texlist;
};

/* a color of the block grid's palette, and its land blocks. rgb holds the color
 * as 0xRRGGBB in its low 24 bits. where the camera stands on that color, the
 * land's draw leaves out every block not in blocks */
struct land_block_color {
    Uint32 rgb;
    Sint32 blocks;
};

/* the most objects world_objects holds */
#define WORLD_OBJECTS 1024
/* the objects the last world_collect_near_players or world_list_near listed,
 * world_count of them */
extern Sint16 world_count;
extern struct world_object *world_objects;

/* in the flags of a task's work. world_add sets it, and
 * world_collect_near_players passes over the objects of a task without it */
#define WORLD_ADDED 0x100

/* the way things fall, a unit vector, and its tilt from straight down about
 * x and about z */
extern NJS_VECTOR world_gravity;
extern Angle world_gravity_ang_x;
extern Angle world_gravity_ang_z;

/* world_free frees the world's arrays. world_alloc allocates each null array
 * from the Shinobi heap, and leaves null any the heap can't give. it then
 * empties the world of land and objects, and forgets the land's task without
 * destroying it. it clears game_state.block_grid_loaded but leaves the grid
 * allocated. each camera's land_block_mask then takes every block, and its
 * land_blocks none */
void world_free(void);
void world_alloc(void);
/* set the near radius: world_collect_near_players keeps an object whose
 * sphere comes within it of the first point. world_reset_near_radius sets
 * it back to 400, as world_land_start does */
void world_set_near_radius(Float radius);
void world_reset_near_radius(void);
/* start the land's task, which draws land and plays its moving parts. any
 * land's task before it is destroyed first. it returns 0 where task_create
 * fails, which leaves no task. otherwise it empties the world as world_alloc
 * does and resets the near radius. it takes back every object world_pool_alloc
 * lent. it then loads land's textures and returns 1. for a null land, the task
 * does nothing and the result is undefined. the task's end releases the
 * textures, empties the world again and frees the block grid */
int world_land_start(struct land_table *land);
#ifdef NONMATCHING
/* load the block grid as the other side does, and return 1. this side
 * searches only color_count entries of colors. it checks the file before it
 * takes it. it returns 0 where it refuses its arguments or the file, where
 * neither file reads and where the heap runs out. a 0 keeps the grid loaded
 * before. where colors holds no match for a palette color, the land's draw
 * keeps every block while the camera stands on that color */
int world_load_block_grid(const char *name,
                          const struct land_block_color *colors,
                          const NJS_POINT3 *origin, const NJS_POINT3 *cell,
                          Uint32 color_count);
/* the stages' calls, which take color_count from colors' array extent */
/* NOLINTNEXTLINE(readability-identifier-naming) */
#define world_load_block_grid(name, colors, origin, cell)                      \
    world_load_block_grid((name), (colors), (origin), (cell),                  \
                          sizeof(colors) / sizeof((colors)[0]))
#else
/* load the block grid from a BMP file, and return 1. it reads name as a PRS
 * file, with everything from its first dot made .prs. failing that, it reads
 * name itself. name must hold a dot, and both names must fit 64 bytes. a grid
 * already loaded must be freed first, or the load drops it unfreed.
 *
 * the file takes 1, 4 or 8 bits to a pixel. its palette follows a 40-byte
 * info header. a row's bits must be a multiple of 32. the load takes the rows
 * end to end, without BMP's padding. each pixel is a cell of cell->x by
 * cell->z. the grid's left edge lies at origin->x, and its rows run from
 * origin->z toward -z.
 *
 * the grid keeps a copy of each palette color's first match in colors, by
 * rgb. each search reads at most as many entries as the palette has colors. a
 * color with no match leaves its copy as syMalloc left it.
 *
 * it returns 0 where neither file reads, which leaves the grid as it was. it
 * returns 0 for another pixel size, with the grid's columns and rows set from
 * the file. it returns 0 where the heap runs out, which leaves the grid null */
int world_load_block_grid(char *name, struct land_block_color *colors,
                          NJS_POINT3 *origin, NJS_POINT3 *cell);
#endif
/* compile the models of land's LAND_DRAWN entries without LAND_NO_BUILD. it
 * takes the textures of the land world_land_start took, which must not be null.
 * the models lie end to end from buf rounded up to a multiple of 32. it clears
 * NJD_EVAL_HIDE in the object of each entry it comes to. each entry it compiles
 * takes LAND_BUILT, and its compiled model in built. it returns the end of the
 * last model, or buf rounded up where it compiles none.
 *
 * it stops before a model whose end would reach DEMO_AREA. the test compares
 * addresses as numbers, so a buf in the RAM's mapping at 0x0c000000 never meets
 * it. while game_state.split_screen is 1, it first clips later draws to tiles
 * 3 to 10, across and down. a tile is 32 pixels a side. it leaves the clip
 * set.
 *
 * world_build_land clears land's LAND_AT_ORIGIN. world_build_land_at_origin
 * sets it, and compiles a copy of each object at the origin. the copy is
 * unturned and unscaled, without its children. an entry whose model an earlier
 * entry compiled shares that compiled model, and its object takes
 * NJD_EVAL_HIDE */
Uint32 world_build_land(struct land_table *land, Uint32 buf);
Uint32 world_build_land_at_origin(struct land_table *land, Uint32 buf);
/* destroy the land's task and return 1. without a task it does nothing, and
 * the result is undefined */
int world_land_stop(void);
/* free the block grid and clear game_state.block_grid_loaded, so that the
 * draw takes each camera's land_block_mask whole */
void world_free_block_grid(void);
/* list in world_objects the objects near a point, of those the last
 * world_collect_near_players kept, up to WORLD_OBJECTS. a land entry is near
 * where its sphere comes within radius of the point. a task's object is near
 * where its sphere comes within radius + 30. that sphere's radius is the
 * model's, scaled by the object's smallest scale unless NJD_EVAL_UNIT_SCL. its
 * center is the object's position plus the model's center, unscaled. it
 * measures to water in x and z alone, from the center unturned */
void world_list_near(Float x, Float y, Float z, Float radius);
/* keep for world_list_near the objects near the players, and list them in
 * world_objects. it keeps up to 128 objects of tasks with WORLD_ADDED, then up
 * to 128 of the land's entries. it passes over a task whose step, or whose
 * parent's step, is task_destroy, as task_kill leaves it.
 *
 * an object is near where its sphere comes within the near radius of the first
 * point. it is near too within 400 of the second. here a task's object's sphere
 * takes the model's radius, unscaled. it measures to a land entry of water in x
 * and z alone. the first point is player 0's position, else player 1's, else
 * camera 0's. the second is player 1's, while player 0 is there too */
void world_collect_near_players(void);
/* put object in the world for task, under flags, and set WORLD_ADDED in the
 * task's work. it adds WORLD_TURNED where that bit's test holds. while tasks
 * have 256 objects in the world, it does nothing */
void world_add(Sint32 flags, struct task *task, NJS_OBJECT *object);
/* take out of the world the last object task put in it, then have
 * collision_mesh_forget forget object's mesh. object need not be the one taken
 * out. a task with no object in the world changes nothing */
void world_remove(struct task *task, NJS_OBJECT *object);
/* lend an object from a pool of 256, or return null where all are lent. a lent
 * object's model, child and sibling are null. the rest is as its last borrower
 * or the heap left it. a pool world_alloc has just allocated marks its objects
 * lent or not as the heap left it. world_land_start clears the marks */
NJS_OBJECT *world_pool_alloc(void);
/* take back an object world_pool_alloc lent. any other object changes
 * nothing */
void world_pool_free(NJS_OBJECT *object);
/* world_reset_gravity points world_gravity straight down, zeroes its two angles
 * and makes COLLISION_UP_Y the collision checks' up axis. world_tilt_gravity
 * points it straight down, then tilts it by ang_x about x and ang_z about z. it
 * then works the two angles back out of the new vector. it leaves the up axis
 * as it was */
void world_reset_gravity(void);
void world_tilt_gravity(Angle ang_x, Angle ang_z);

#endif
