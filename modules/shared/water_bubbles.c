/* the water's bubbles: BUBBLE and BUBBLES in a stage's object list. a source
 * sends up a large bubble now and then, which a player can burst for air. a
 * field keeps a pool of small bubbles that rise from the floor to the
 * surface */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/mesh.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/lines.h"
#include "effect/texlist.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/water_bubbles.h"
#include "shared/water_ripple.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* a kind's ranges, least to most, for a new bubble's size, sway and rise a
 * step. one random fraction f, from 0 up to 1, picks all three. size and sway
 * lie f of the way from least to most. rise lies 1 - f * f of the way. a larger
 * bubble thus sways wider and rises slower. each step turns the angle of the
 * bubble's sway by up to spin either way, at random */
struct bubble_kind {
    Float size_least;
    Float size_most;
    Angle spin;
    Float sway_least;
    Float sway_most;
    Float rise_least;
    Float rise_most;
};
/* the kinds, as indices of bubble_kinds */
#define KIND_SMALL 0 /* a field's */
#define KIND_LARGE 1 /* a source's */

/* a source's record, in its task's motion: the height of the surface found
 * above the source */
#define SOURCE_SURFACE(task) (*(Float *)(task)->motion.p)

/* a large bubble's record, in its task's motion */
struct large_bubble {
    Float growth; /* what its size gains each step until it leaves */
    Float sway;
    Angle phase; /* the phase of its squash */
    Angle ang; /* the angle of its sway about its source */
    Sint32 life; /* the steps it has left */
    Float size;
    Float squash; /* its sprite's height over its width */
    Float rise;
};
#define LARGE(task) ((struct large_bubble *)(task)->motion.p)

/* a small bubble of a field */
struct small_bubble {
    NJS_POINT3 pos;
    Float size;
    Float surface; /* the height it ends at */
    Sint32 life; /* the steps it has left */
    /* the point of the field it sways about */
    Float cx;
    Float cz;
    Float sway;
    Float rise;
    Angle ang; /* the angle of its sway about that point */
    struct small_bubble *prev;
    struct small_bubble *next;
};

/* a field's record, in its task's motion. bubbles is the pool. free lists
 * the bubbles not in use, and active those that rise, each linked through
 * next and prev */
struct bubble_pool {
    struct small_bubble *bubbles;
    struct small_bubble *free;
    struct small_bubble *active;
};
#define POOL(task) ((struct bubble_pool *)(task)->motion.p)

/* a field's work: a struct task_work with count in timer's place. count is
 * signed, where timer is a Uint16, since it can hold -1. count is the number of
 * bubbles that rise. bubble_field_alloc sets it to -1 where it gives no pool.
 * bubble_field_free sets it to -1 as it frees the pool */
struct bubble_field_work {
    Sint8 mode;
    Sint8 submode;
    Uint8 id;
    Uint8 tick;
    Sint16 flags;
    Sint16 count;
    struct angles ang;
    NJS_POINT3 pos;
    /* x is the field's radius, y the bubbles it starts a step, and z its
     * pool's size */
    NJS_POINT3 scl;
    struct collision_work *collision;
};
#define FIELD(task) ((struct bubble_field_work *)(task)->work)

/* a crush's record, in its task's motion. its pieces are in its extra */
struct bubble_crush {
    Float surface; /* the pieces end as they rise past it */
    Float fade; /* the pieces' color, which falls from 1 to 0 */
};
struct crush_piece {
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    Float size;
    Bool active;
};
#define CRUSH_PIECES 4

/* a ripple's record, in its task's extra */
struct bubble_ripple {
    NJS_POINT3 pos;
    Sint32 steps; /* the steps it has spread */
    Float fade; /* what shade loses each step */
    /* the ripple draws in a gray of shade + 1, which it fades toward black */
    Float shade;
    Float speed; /* what scale gains each step */
    Float first_scale;
    Float scale;
};

/* a source sends up a large bubble every SOURCE_STEPS + 1 steps */
#define SOURCE_STEPS 600
/* a source looks for the surface above the point scl.y over its pos */
#define SOURCE_OFFSET(work) ((work)->scl.y)
/* a bubble ends after BUBBLE_LIFE steps */
#define BUBBLE_LIFE 1200
/* a large bubble grows until it lies LEAVE_HEIGHT above its source. each step
 * adds GROWTH_SHARE of the size picked for it, times its rise */
#define LEAVE_HEIGHT 10.0F
#define GROWTH_SHARE 0.1F
/* a large bubble's squash swings between SQUASH_LEAST and 1, as its phase
 * turns by SQUASH_TURN a step */
#define SQUASH_TURN 910
#define SQUASH_LEAST 0.85F
/* a large bubble bursts once its center comes within SURFACE_SHARE of its
 * size below the surface */
#define SURFACE_SHARE 0.6F
/* a large bubble's sphere takes its size as its radius, and its collision
 * record reaches COLLISION_REACH further */
#define COLLISION_REACH 8.0F
/* a bubble's ripple starts at a scale of RIPPLE_SCALE times the bubble's size.
 * it spreads by RIPPLE_SPEED a step. its shade starts at RIPPLE_SHADE. each
 * step lowers it by an even fade that would take it to -1 on step
 * RIPPLE_FADE_STEPS. the ripple ends once its shade falls below -RIPPLE_END, a
 * little before then. on its RIPPLE_NEXT step, it starts another ripple at its
 * pos and first scale, from its shade then */
#define RIPPLE_SPEED 0.05F
#define RIPPLE_SHADE 0.5F
#define RIPPLE_SCALE 0.2F
#define RIPPLE_FADE_STEPS 30.0F
#define RIPPLE_NEXT 15
#define RIPPLE_END 0.9F
/* a field gets a pool only where scl.z, the pool's size, is above
 * POOL_LEAST */
#define POOL_LEAST 0.99F
/* a field leaves, as placement_try_leave_by says, once no player lies within
 * its radius plus FIELD_REACH */
#define FIELD_REACH 410.0F
/* a small bubble ends where a player lies nearer than (size + NEAR_REACH)
 * squared. small_bubble_ends describes that bug: it compares a distance with a
 * square. for a field's sizes, 0.2 to 0.6, that is 4.84 to 6.76 */
#define NEAR_REACH 2.0F
/* the players a small bubble avoids: slots 0 to LAST_NEAR_PLAYER */
#define LAST_NEAR_PLAYER 1
/* a size in world units draws a sprite of SPRITE_SCALE times it */
#define SPRITE_SCALE 0.03125F
#define SPRITE_ATTR (NJD_SPRITE_SCALE | NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR)
/* bubble_heights looks at the world's objects within HEIGHTS_REACH.
 * HEIGHT_NONE is the height of no surface, and -HEIGHT_NONE of no floor.
 * water_surface_height returns NO_WATER where no water lies over the point */
#define HEIGHTS_REACH 20.0F
#define HEIGHT_NONE 1000000.0F
#define NO_WATER (-100000.0F)
/* bubble_heights' result: a surface above, a floor below */
#define FOUND_ABOVE 0x2
#define FOUND_BELOW 0x4
/* a crush's pieces fade out over CRUSH_FADE_STEPS steps, and each step they
 * rise CRUSH_RISE beyond their speed. a piece's size runs from CRUSH_SIZE_LEAST
 * up to CRUSH_SIZE_RANGE more. bubble_crush_create scales a piece's spd by
 * CRUSH_SPEED. no piece takes the result, a bug bubble_crush_create
 * describes */
#define CRUSH_FADE_STEPS 30.0F
#define CRUSH_RISE 0.15F
#define CRUSH_SPEED 0.1F
#define CRUSH_SIZE_LEAST 0.2F
#define CRUSH_SIZE_RANGE 1.3F
/* the task level of a crush and of a ripple, and the task_create flags of
 * each */
#define BUBBLES_LEVEL 3
#define CRUSH_FLAGS 0
#define RIPPLE_FLAGS 0

static const struct bubble_kind bubble_kinds[] = {
    {0.2F, 0.6F, 0x5b0, 1.0F, 2.0F, 0.4F, 0.8F},
    {5.0F, 5.0F, 0xb6, 2.0F, 4.0F, 0.08F, 0.1F},
};

Float (*water_surface_height)(NJS_POINT3 *pos) = NULL;
/* the bubbles' color, and the constant material their draws leave behind */
static NJS_ARGB bubble_color = {
#include "shared/water_bubbles/bubble_color.inc"
};
static NJS_ARGB bubble_clear = {
#include "shared/water_bubbles/bubble_clear.inc"
};
static NJS_TEXLIST bubble_texlist = {
#include "shared/water_bubbles/bubble_texlist.inc"
};
static NJS_TEXANIM bubble_texanim = {
#include "shared/water_bubbles/bubble_texanim.inc"
};
static NJS_SPRITE bubble_sprite = {
#include "shared/water_bubbles/bubble_sprite.inc"
};
/* a large bubble's sphere, whose radius follows its size */
/* clang-format off */
static struct collision_shape bubble_shape = {
    0, SHAPE_SPHERE, 0, SHAPE_HURTS_ALL, SHAPE_UNTURNED,
    {0.0F, 0.0F, 0.0F}, 1.0F, 0.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */

static void bubble_source_step(struct task *task);
static void bubble_record_free(struct task *task);
static void large_bubble_init(struct task *task);
static void large_bubble_emerge(struct task *task);
static void large_bubble_step(struct task *task);
static void large_bubble_draw(struct task *task);
static void large_bubble_set_size(struct task_work *work, Float size);
static void large_bubble_burst(struct task *task, int player);
static void bubble_field_step(struct task *task);
static void bubble_field_draw(struct task *task);
static void bubble_field_destroy(struct task *task);
static Bool small_bubble_ends(struct small_bubble *bubble);
static void bubble_field_alloc(struct task *task, int count);
static void bubble_field_start(struct task *task, Float x, Float z);
static struct small_bubble *bubble_field_drop(struct task *task,
                                              struct small_bubble *bubble);
static void bubble_field_free(struct task *task);
static void bubble_sway(int kind, NJS_POINT3 *pos, Angle *ang, Float sway,
                        Float cx, Float cz, Float rise);
static void bubble_sprite_draw(NJS_POINT3 *pos, NJS_ARGB *color, Float sx,
                               Float sy);
static int bubble_heights(Float *above, Float *below, Float x, Float y,
                          Float z);
static Bool bubble_object_near(struct task *owner, NJS_OBJECT *object, Float x,
                               Float y, Float z);
static void bubble_crush_create(NJS_POINT3 *pos, Float size, Float surface);
static void bubble_random_way(NJS_VECTOR *way);
static void bubble_crush_step(struct task *task);
static void bubble_crush_draw(struct task *task);
static void bubble_ripple_create(NJS_POINT3 *pos, Float speed, Float shade,
                                 Float scale);
static void bubble_ripple_step(struct task *task);
static void bubble_ripple_draw(struct task *task);
static void bubble_ripple_destroy(struct task *task);

/* @hack spelling: two inline functions that set a sine and a cosine and a
 * vector's three members, where the calls and stores written out at each
 * use would do */
#pragma inline(sin_cos, vector_set)

static void sin_cos(Angle ang, Float *sine, Float *cosine)
{
    *sine = njSin(ang);
    *cosine = njCos(ang);
}

static void vector_set(NJS_VECTOR *vector, Float x, Float y, Float z)
{
    vector->x = x;
    vector->y = y;
    vector->z = z;
}

/* draw debug_draw_model's sphere of radius scale about pos. nothing calls it,
 * and no header declares it. it isn't static because SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void bubble_debug_sphere(NJS_POINT3 *pos, Float scale);
/* NOLINTEND(misc-use-internal-linkage) */

void bubble_debug_sphere(NJS_POINT3 *pos, Float scale)
{
    NJS_VECTOR scl;
    struct angles ang;

    ang.x = 0;
    ang.y = 0;
    ang.z = 0;
    scl.x = scale;
    scl.y = scale;
    scl.z = scale;
    debug_draw_model(pos, &ang, &scl, DEBUG_MODEL_SPHERE);
}

/* *out = *from times scale */
static void bubble_vector_scale(NJS_VECTOR *out, Float scale, NJS_VECTOR *from)
{
    out->x = from->x * scale;
    out->y = from->y * scale;
    out->z = from->z * scale;
}

/* *out = *first + *second */
static void bubble_vector_add(NJS_VECTOR *out, NJS_VECTOR *first,
                              NJS_VECTOR *second)
{
    out->x = first->x + second->x;
    out->y = first->y + second->y;
    out->z = first->z + second->z;
}

void bubble_source_init(struct task *task)
{
    struct task_work *work = task->work;
    Float *surface;

    /* @hack redundant: a debug view no path runs, kept for its strings,
     * which SHC still places in this source's constants */
    if (0) {
        (void)"<-COLLI SEARCH OFFSET";
        (void)"COLLISION HEIGHT %f";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(Float));
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((surface = task->motion.p) != NULL &&
        (bubble_heights(surface, NULL, work->pos.x,
                        work->pos.y + SOURCE_OFFSET(work), work->pos.z) &
         FOUND_ABOVE)) {
        work->timer = SOURCE_STEPS;
        task->step = bubble_source_step;
        task->draw = NULL;
        task->destructor = bubble_record_free;
    } else {
        syFree(task->motion.p);
        task->motion.p = NULL;
    }
}

void bubble_field_init(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a debug view no path runs, kept for its strings,
     * which SHC still places in this source's constants */
    if (0) {
        (void)"<-RANGE";
        (void)"<-FREQUENCY";
        (void)"<-MAX (DISP %d)";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct bubble_pool));
    if (work->scl.z > POOL_LEAST) {
        bubble_field_alloc(task, work->scl.z);
        work->scl.z = 0.0F;
    }
    task->step = bubble_field_step;
    task->draw = bubble_field_draw;
    task->destructor = bubble_field_destroy;
}

static void bubble_source_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_VECTOR unused;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the step written timer = timer + 1 inside the test,
     * where ++timer would do */
    if ((work->timer = work->timer + 1) > SOURCE_STEPS) {
        task_create_child(TASK_WORK, large_bubble_init, task);
        work->timer = 0;
    }
}

#ifdef NONMATCHING
/* TRUE once bubble_record_free has run, for large_bubble_emerge to tell that
 * the step it called destroyed its bubble */
static Bool bubble_record_freed;
#endif

static void bubble_record_free(struct task *task)
{
    syFree(task->motion.p);
    task->motion.p = NULL;
#ifdef NONMATCHING
    /* this side tells large_bubble_emerge that a bubble's task is gone */
    bubble_record_freed = TRUE;
#endif
}

/* the first step of a large bubble, a child of its source */
static void large_bubble_init(struct task *task)
{
    const struct bubble_kind *kind = &bubble_kinds[KIND_LARGE];
    Float fraction;
    Float size;
    Float rise;

    task->motion.p = syCalloc(1, sizeof(struct large_bubble));
    fraction = (Float)rand() * RAND_SCALE;
    size = kind->size_least + (fraction * (kind->size_most - kind->size_least));
    LARGE(task)->sway =
        kind->sway_least + (fraction * (kind->sway_most - kind->sway_least));
    rise = kind->rise_least + ((1.0F - (fraction * fraction)) *
                               (kind->rise_most - kind->rise_least));
    LARGE(task)->size = 0.0F;
    LARGE(task)->growth = size * rise * GROWTH_SHARE;
    LARGE(task)->rise = rise;
    LARGE(task)->life = BUBBLE_LIFE;
    collision_init(task, &bubble_shape, 1, COLLISION_OBJECT);
    task->step = large_bubble_emerge;
    task->draw = large_bubble_draw;
    task->destructor = bubble_record_free;
}

/* a large bubble's steps until it lies LEAVE_HEIGHT above its source. it
 * grows, and moves as large_bubble_step moves it */
static void large_bubble_emerge(struct task *task)
{
    struct task_work *source = task->parent->work;
    struct large_bubble *bubble = task->motion.p;
    struct task_work *work = task->work;

    bubble->size += bubble->growth;
    /* @hack spelling: the size read through task->motion in this test and in
     * the call below, where bubble->size would do */
    if (water_surface_height != NULL &&
        !(water_surface_height(&work->pos) > work->pos.y + LARGE(task)->size)) {
        task_destroy(task);
    } else {
        large_bubble_set_size(work, LARGE(task)->size);
        /* @bug memory: reads the work, and may write the step, of the task
         * that large_bubble_step destroys as the bubble bursts or ends: the
         * call at 0x2b4 of the object's code, the read at 0x2c2 and the
         * write at 0x2cc */
#ifdef NONMATCHING
        /* this side repairs the tag above: it reads nothing of the bubble
         * once large_bubble_step has destroyed it */
        bubble_record_freed = FALSE;
        large_bubble_step(task);
        if (bubble_record_freed) {
            return;
        }
#else
        large_bubble_step(task);
#endif
        if (work->pos.y > source->pos.y + LEAVE_HEIGHT) {
            task->step = large_bubble_step;
        }
    }
}

static void large_bubble_step(struct task *task)
{
    struct task_work *source;
    struct task_work *work;
    struct large_bubble *bubble;
    Float surface;
    Float sine;
    Float cosine;

    source = task->parent->work;
    work = task->work;
    surface = SOURCE_SURFACE(task->parent);
    if (water_surface_height != NULL &&
        surface > water_surface_height(&work->pos)) {
        surface = water_surface_height(&work->pos);
    }
    LARGE(task)->phase += SQUASH_TURN;
    sin_cos(LARGE(task)->phase, &sine, &cosine);
    LARGE(task)->squash =
        SQUASH_LEAST + ((1.0F - SQUASH_LEAST) * (0.5F + (0.5F * sine)));
    bubble = task->motion.p;
    bubble_sway(KIND_LARGE, &work->pos, &bubble->ang, bubble->sway,
                source->pos.x, source->pos.z, bubble->rise);
    /* @hack spelling: the tests of the touch nested, where one test of both
     * would do */
    if (work->collision->flags & COLLISION_TOUCHED) {
        if (work->collision->hit->kind == COLLISION_PLAYER) {
            large_bubble_burst(task, 0);
            task_destroy(task);
            return;
        }
    }
    if (!(surface > work->pos.y + (LARGE(task)->size * SURFACE_SHARE))) {
        work->pos.y = surface;
        bubble_ripple_create(&work->pos, RIPPLE_SPEED, RIPPLE_SHADE,
                             LARGE(task)->size * RIPPLE_SCALE);
        task_destroy(task);
        return;
    }
    if (--LARGE(task)->life < 0) {
        task_destroy(task);
        return;
    }
    collision_register(task);
}

static void large_bubble_draw(struct task *task)
{
    struct task_work *work = task->work;

    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    bubble_sprite_draw(&work->pos, NULL, LARGE(task)->size,
                       LARGE(task)->squash * LARGE(task)->size);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void large_bubble_set_size(struct task_work *work, Float size)
{
    struct collision_shape *shape = work->collision->shapes;

    shape->radius = size;
    work->collision->radius = size + COLLISION_REACH;
}

/* give player air, and crush the bubble into pieces that rise to its
 * source's surface */
static void large_bubble_burst(struct task *task, int player)
{
    struct task_work *work = task->work;

    player_request(player, PLAYER_REQUEST_BREATHE, 0);
    bubble_crush_create(&work->pos, LARGE(task)->size,
                        SOURCE_SURFACE(task->parent));
}

static void bubble_field_step(struct task *task)
{
    struct bubble_field_work *work = FIELD(task);
    struct small_bubble *bubble;
    Float reach;
    /* the bubbles this step has yet to start. a fraction is the chance of
     * one more */
    Float amount;
    int count;

    reach = work->scl.x + FIELD_REACH;
    /* @hack redundant: a step of reach that no path reaches */
    if (0) {
        reach++;
    }
    if (placement_try_leave_by(task, reach * reach)) {
        return;
    }
    /* NOLINTNEXTLINE(bugprone-float-loop-counter) */
    for (amount = work->scl.y; amount > 0.0F; amount -= 1.0F) {
        if (amount > (Float)rand() * RAND_SCALE) {
            bubble_field_start(task, work->pos.x, work->pos.z);
        }
    }
    bubble = POOL(task)->active;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((count = work->count) > 0) {
        do {
            if (small_bubble_ends(bubble)) {
                bubble = bubble_field_drop(task, bubble);
            } else {
                bubble_sway(KIND_SMALL, &bubble->pos, &bubble->ang,
                            bubble->sway, bubble->cx, bubble->cz, bubble->rise);
                bubble = bubble->next;
            }
        } while (--count && bubble != NULL);
    }
}

static void bubble_field_draw(struct task *task)
{
    struct bubble_field_work *work = FIELD(task);
    struct small_bubble *bubble;
    Float reach;
    int count;
    Float scale;

    reach = work->scl.x + FIELD_REACH;
    /* @hack spelling: the else after a branch that returns, where the rest
     * of the body alone would do */
    if (placement_try_leave_by(task, reach * reach)) {
        return;
    } else {
        bubble = POOL(task)->active;
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njSetConstantMaterial(&bubble_color);
        count = work->count;
        if (count > 0) {
            do {
                scale = bubble->size * SPRITE_SCALE;
                bubble_sprite.p = bubble->pos;
                bubble_sprite.sx = scale;
                bubble_sprite.sy = scale;
                njDrawSprite3D(&bubble_sprite, 0, SPRITE_ATTR);
                bubble = bubble->next;
            } while (--count);
        }
        njSetConstantMaterial(&bubble_clear);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
}

static void bubble_field_destroy(struct task *task)
{
    bubble_field_free(task);
}

/* whether the bubble ends this step. it ends at its surface, when its life runs
 * out, or near player 0 or 1. NEAR_REACH says how near. a bubble below its
 * surface spends a step of its life here. player_get_past gives the players'
 * places */
static Bool small_bubble_ends(struct small_bubble *bubble)
{
    NJS_POINT3 *pos = &bubble->pos;
    NJS_POINT3 player;
    NJS_POINT3 *at;
    Float distance;
    int i;

    if (!(pos->y < bubble->surface)) {
        return TRUE;
    }
    if (--bubble->life < 0) {
        return TRUE;
    }
    i = 0;
    /* @hack spelling: a jump into the loop's test, where a for would do */
    goto test;
    do {
        /* @hack redundant: a read of pos that nothing uses */
        distance = pos->y;
        player_get_past(i, 0, &player, NULL);
        /* @hack spelling: at, a pointer to player set here, where &player
         * in the call would do */
        at = &player;
        distance = njDistanceP2P(at, pos);
        /* @bug logic: the bubble ends within (size + NEAR_REACH) squared of
         * the player, not within size + NEAR_REACH. the call at 0x71a of the
         * object's code gives a distance, and the test at 0x72c squares the
         * sum it compares that with */
        if (distance <
            (bubble->size + NEAR_REACH) * (bubble->size + NEAR_REACH)) {
            return TRUE;
        }
        i++;
    test:;
    } while (i <= LAST_NEAR_PLAYER);
    return FALSE;
}

/* give the field a pool of count bubbles, all free. the field gets none where
 * count is 0 or less, or the heap has no room for it */
static void bubble_field_alloc(struct task *task, int count)
{
    struct bubble_field_work *work = FIELD(task);
    struct small_bubble *bubbles;
    struct small_bubble *bubble;
    int i;

    POOL(task)->active = NULL;
    if (count > 0 &&
        (bubbles = syCalloc(count, sizeof(struct small_bubble))) != NULL) {
        bubble = bubbles;
        bubble->prev = NULL;
        bubble->next = bubble + 1;
        for (i = 1, bubble++; i < count; i++, bubble++) {
            bubble->prev = bubble - 1;
            bubble->next = bubble + 1;
        }
        bubbles[count - 1].next = NULL;
        work->count = 0;
        POOL(task)->free = bubbles;
        POOL(task)->bubbles = bubbles;
        return;
    }
    work->count = -1;
    POOL(task)->free = NULL;
    POOL(task)->bubbles = NULL;
}

/* start a free bubble at the floor below (x, z), the field's center. it rises
 * to the surface above that point. the call does nothing where no bubble is
 * free, or bubble_heights finds no floor or no surface. it sways about a random
 * point within the field's radius of (x, z) in x and z */
static void bubble_field_start(struct task *task, Float x, Float z)
{
    struct bubble_field_work *work = FIELD(task);
    struct small_bubble *bubble;
    const struct bubble_kind *kind;
    Float fraction;

    bubble = POOL(task)->free;
    if (bubble == NULL) {
        return;
    }
    if ((bubble_heights(&bubble->surface, &bubble->pos.y, x, work->pos.y, z) &
         (FOUND_ABOVE | FOUND_BELOW)) != (FOUND_ABOVE | FOUND_BELOW)) {
        return;
    }
    work->count++;
    POOL(task)->free = bubble->next;
    bubble->next = POOL(task)->active;
    bubble->prev = NULL;
    if (POOL(task)->active != NULL) {
        POOL(task)->active->prev = bubble;
    }
    POOL(task)->active = bubble;
    bubble->cx =
        ((Float)rand() * work->scl.x * (2.0F * RAND_SCALE)) - work->scl.x + x;
    bubble->cz =
        ((Float)rand() * work->scl.x * (2.0F * RAND_SCALE)) - work->scl.x + z;
    kind = &bubble_kinds[KIND_SMALL];
    fraction = (Float)rand() * RAND_SCALE;
    bubble->size =
        kind->size_least + (fraction * (kind->size_most - kind->size_least));
    bubble->sway =
        kind->sway_least + (fraction * (kind->sway_most - kind->sway_least));
    bubble->rise = ((1.0F - (fraction * fraction)) *
                    (kind->rise_most - kind->rise_least)) +
                   kind->rise_least;
    bubble->life = BUBBLE_LIFE;
}

/* move bubble from the rising list to the free one. return the bubble that
 * followed it in the rising list */
static struct small_bubble *bubble_field_drop(struct task *task,
                                              struct small_bubble *bubble)
{
    struct bubble_field_work *work = FIELD(task);
    struct bubble_pool *pool;
    /* @hack spelling: pool set inside free's initializer, where a statement
     * of its own would do */
    struct small_bubble *free = (pool = task->motion.p)->free;
    struct small_bubble *prev = bubble->prev;
    struct small_bubble *next = bubble->next;

    if (pool->active == bubble) {
        POOL(task)->active = next;
    }
    if (prev != NULL) {
        prev->next = next;
    }
    if (next != NULL) {
        next->prev = prev;
    }
    bubble->prev = NULL;
    bubble->next = free;
    if (free != NULL) {
        free->prev = bubble;
    }
    POOL(task)->free = bubble;
    work->count--;
    return next;
}

static void bubble_field_free(struct task *task)
{
    struct bubble_field_work *work = FIELD(task);

    syFree(POOL(task)->bubbles);
    work->count = -1;
    POOL(task)->free = NULL;
    POOL(task)->active = NULL;
    POOL(task)->bubbles = NULL;
}

/* turn *ang by a random angle of up to kind's spin either way. place pos
 * sway from (cx, cz) along *ang, and raise pos by rise */
static void bubble_sway(int kind, NJS_POINT3 *pos, Angle *ang, Float sway,
                        Float cx, Float cz, Float rise)
{
    Float sine;
    Float cosine;
    Angle step;

    step = ((Float)rand() * bubble_kinds[kind].spin * (2.0F * RAND_SCALE)) -
           bubble_kinds[kind].spin;
    *ang += step;
    sin_cos(*ang, &sine, &cosine);
    pos->x = cx + (sway * cosine);
    pos->z = cz + (sway * sine);
    pos->y += rise;
}

/* draw the bubble's sprite at pos, sx by sy, in color, or in bubble_color
 * where color is null */
static void bubble_sprite_draw(NJS_POINT3 *pos, NJS_ARGB *color, Float sx,
                               Float sy)
{
    if (color == NULL) {
        color = &bubble_color;
    }
    njFogDisable();
    njSetConstantMaterial(color);
    bubble_sprite.p = *pos;
    bubble_sprite.sx = sx * SPRITE_SCALE;
    bubble_sprite.sy = sy * SPRITE_SCALE;
    njDrawSprite3D(&bubble_sprite, 0, SPRITE_ATTR);
    njSetConstantMaterial(&bubble_clear);
    njFogEnable();
}

/* find the lowest surface above (x, y, z) and the highest floor below it. it
 * looks among the world's objects within HEIGHTS_REACH. the result has
 * FOUND_ABOVE for a surface and FOUND_BELOW for a floor. *above gets the
 * surface's height, or HEIGHT_NONE, and *below the floor's, or -HEIGHT_NONE.
 * either may be null. a task's object in the world must have a force.
 *
 * the water stands in for a surface above where no object has one. it does only
 * where above isn't null and water_surface_height is set. the result then has
 * FOUND_ABOVE, whatever the water's height, and *above takes that height unless
 * it is NO_WATER */
static int bubble_heights(Float *above, Float *below, Float x, Float y, Float z)
{
    struct height_probe probe;
    NJS_POINT3 pos;
    struct world_object *object;
    struct task *owner;
    NJS_OBJECT *model;
    Float top;
    Float bottom;
    Float high;
    Float low;
    Float lift;
    int found = 0;
    int left;

    vector_set(&probe.pos, x, y, z);
    world_list_near(x, y, z, HEIGHTS_REACH);
    top = HEIGHT_NONE;
    bottom = -HEIGHT_NONE;
    left = world_count;
    object = world_objects;
    if (left) {
        /* @bug logic: the loop takes world_count + 1 objects, the last a
         * stale entry past the list: the test at 0xb5a of the object's code
         * reads the count before its decrement */
        do {
            model = object->object;
            owner = object->owner;
            if (bubble_object_near(owner, model, x, y, z) &&
                polygon_find_heights(&probe, model, object->flags)) {
                high = probe.above.pos;
                low = probe.below.pos;
                if (owner != NULL) {
                    lift = TASK_FORCES_OF(owner)->speed.y;
                    high += lift;
                    low += lift;
                }
                if (probe.above.found && high > y && top > high) {
                    top = high;
                    found |= FOUND_ABOVE;
                }
                if (probe.below.found && y > low && low > bottom) {
                    bottom = low;
                    found |= FOUND_BELOW;
                }
            }
            object++;
        } while (left--);
    }
    if (above != NULL) {
        *above = top;
    }
    if (below != NULL) {
        *below = bottom;
    }
    if (water_surface_height != NULL && !(found & FOUND_ABOVE) &&
        above != NULL) {
        pos.x = x;
        pos.y = y;
        pos.z = z;
        top = water_surface_height(&pos);
        found |= FOUND_ABOVE;
        if (top > NO_WATER) {
            *above = top;
        }
    }
    return found;
}

/* whether object may lie under (x, z). the land's objects, whose owner is
 * null, always may. a task's object may where its model's radius, times its
 * largest scale, reaches (x, z). y isn't read */
static Bool bubble_object_near(struct task *owner, NJS_OBJECT *object, Float x,
                               Float y, Float z)
{
    NJS_MODEL *model;
    Float *scl;
    Float dz;
    Float dx;
    Float distance;
    /* @hack redundant: largest declared register, where a plain local would
     * do */
    register Float largest;
    Float scl_y;
    Float radius;

    if (owner == NULL) {
        return TRUE;
    }
    dz = object->pos[2] - z;
    dx = object->pos[0] - x;
    /* @hack spelling: scl_y read ahead of the distance, where the test that
     * reads it would do */
    scl = object->scl;
    scl_y = scl[1];
    distance = (dx * dx) + (dz * dz);
    largest = scl[0];
    if (scl_y > largest) {
        largest = scl_y;
    }
    if (scl[2] > largest) {
        /* @hack spelling: the scale read again through object, where scl[2]
         * would do */
        largest = object->scl[2];
    }
    /* @hack redundant: a read of the model's radius that nothing uses */
    radius = ((void)(model = object->model)->r, model->r * model->r);
    largest = largest * largest * radius;
    if (largest > distance) {
        return TRUE;
    }
    return FALSE;
}

/* crush a bubble into CRUSH_PIECES pieces, which rise until they pass
 * surface. size isn't read, and pos reaches no piece. nothing is made where
 * task_create fails or the heap has no room for the crush's records */
static void bubble_crush_create(NJS_POINT3 *pos, Float size, Float surface)
{
    struct task *task;
    struct crush_piece *piece;
    struct bubble_crush *crush;
    NJS_VECTOR way;
    int i;

    task = task_create(CRUSH_FLAGS, BUBBLES_LEVEL, bubble_crush_step,
                       "bubble_crush_exec");
    if (task == NULL) {
        return;
    }
    piece = syMalloc(sizeof(struct crush_piece) * CRUSH_PIECES);
    crush = syMalloc(sizeof(struct bubble_crush));
    if (piece != NULL && crush != NULL) {
        i = CRUSH_PIECES;
        task->draw = bubble_crush_draw;
        task->extra.p = piece;
        task->motion.p = crush;
        crush->fade = 1.0F;
        crush->surface = surface;
        do {
            piece->active = TRUE;
            piece->size = CRUSH_SIZE_LEAST +
                          ((Float)rand() * (CRUSH_SIZE_RANGE * RAND_SCALE));
            bubble_random_way(&way);
            /* @bug uninitialized: each piece's spd and pos stay as syMalloc
             * left them, and the crush's step and draw read them. the calls
             * at 0xd32, 0xd40 and 0xd4a of the object's code write way, not
             * the piece. the first reads the piece's spd, and the third its
             * pos */
#ifdef NONMATCHING
            /* this side repairs the tag above: each piece starts piece->size
             * along way from pos, and moves along way at CRUSH_SPEED */
            bubble_vector_scale(&piece->spd, CRUSH_SPEED, &way);
            bubble_vector_scale(&way, piece->size, &way);
            bubble_vector_add(&piece->pos, pos, &way);
#else
            bubble_vector_scale(&way, CRUSH_SPEED, &piece->spd);
            bubble_vector_scale(&way, piece->size, &way);
            bubble_vector_add(&way, pos, &piece->pos);
#endif
            piece++;
        } while (--i);
    } else {
        syFree(crush);
        syFree(piece);
        task_destroy(task);
    }
}

/* set *way to a random unit vector that never points down */
static void bubble_random_way(NJS_VECTOR *way)
{
    Float sine;
    Float cosine;
    Float lift;
    Float level;
    Angle ang;

    ang = RANDOM_TURN();
    sin_cos(ang, &sine, &cosine);
    ang = (Float)rand() * (HALF_TURN * RAND_SCALE);
    sin_cos(ang, &lift, &level);
    sine *= level;
    cosine *= level;
    vector_set(way, sine, lift, cosine);
}

static void bubble_crush_step(struct task *task)
{
    struct bubble_crush *crush = task->motion.p;
    struct crush_piece *piece = task->extra.p;
    Bool done = TRUE;
    int i;
    Float surface;

    crush->fade -= 1.0F / CRUSH_FADE_STEPS;
    if (crush->fade > 0.0F) {
        surface = crush->surface;
        i = CRUSH_PIECES;
        do {
            if (piece->active) {
                bubble_vector_add(&piece->pos, &piece->spd, &piece->pos);
                piece->pos.y += CRUSH_RISE;
                if (piece->pos.y > surface) {
                    piece->active = FALSE;
                } else {
                    done = FALSE;
                }
            }
            piece++;
        } while (--i);
    }
    if (done) {
        task_destroy(task);
    }
}

static void bubble_crush_draw(struct task *task)
{
    struct crush_piece *piece = task->extra.p;
    struct bubble_crush *crush = task->motion.p;
    NJS_ARGB color;
    Float fade;
    int i;

    i = CRUSH_PIECES;
    fade = crush->fade;
    color.a = 1.0F;
    color.r = fade;
    color.g = fade;
    color.b = fade;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    do {
        if (piece->active) {
            bubble_sprite_draw(&piece->pos, &color, piece->size, piece->size);
        }
        piece++;
        /* @hack spelling: the count's step as a statement of its own, where
         * --i in the test would do */
        i--;
    } while (i);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

/* start a ripple at pos, which spreads at speed from scale, and starts at
 * shade. where task_create fails, the call writes through null. where the
 * heap has no room for the ripple's record, the call destroys the task */
static void bubble_ripple_create(NJS_POINT3 *pos, Float speed, Float shade,
                                 Float scale)
{
    struct task *task;
    struct bubble_ripple *ripple;

    task = task_create(RIPPLE_FLAGS, BUBBLES_LEVEL, bubble_ripple_step,
                       "bbl_ripple_exec");
    task->draw = bubble_ripple_draw;
    task->destructor = bubble_ripple_destroy;
    task->extra.p = syMalloc(sizeof(struct bubble_ripple));
    if (task->extra.p != NULL) {
        ripple = task->extra.p;
        ripple->pos = *pos;
        ripple->speed = speed;
        ripple->first_scale = scale;
        ripple->scale = scale;
        ripple->shade = shade;
        ripple->steps = 0;
        ripple->fade = (shade + 1.0F) * (1.0F / RIPPLE_FADE_STEPS);
    } else {
        task_destroy(task);
    }
}

static void bubble_ripple_step(struct task *task)
{
    struct bubble_ripple *ripple = task->extra.p;
    Float scale;
    Float shade;

    scale = ripple->scale + ripple->speed;
    ripple->scale = scale;
    /* @hack spelling: shade read and then lowered in two statements, where
     * one would do */
    shade = ripple->shade;
    shade -= ripple->fade;
    if (shade < -RIPPLE_END) {
        task_destroy(task);
        return;
    }
    if (++ripple->steps == RIPPLE_NEXT) {
        bubble_ripple_create(&ripple->pos, ripple->speed, shade,
                             ripple->first_scale);
    }
    ripple->shade = shade;
}

static void bubble_ripple_draw(struct task *task)
{
    struct bubble_ripple *ripple = task->extra.p;
    Float gray;

    njSetTexture(&ripple_texlist);
    njFogDisable();
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT);
    njSetConstantAttr(0, NJD_FST_UA | NJD_FST_DB);
    njPushMatrix(NULL);
    njTranslateV(NULL, &ripple->pos);
    gray = ripple->shade + 1.0F;
    constant_material_set(1.0F, gray, gray, gray);
    njScale(NULL, ripple->scale, 1.0F, ripple->scale);
    njCnkSimpleDrawObject(&ripple_object);
    njPopMatrix(1);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    njFogEnable();
}

static void bubble_ripple_destroy(struct task *task)
{
}
