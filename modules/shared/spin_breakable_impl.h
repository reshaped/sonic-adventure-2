/* the iron and wooden spinning crates, which spin_iron.c and spin_wood.c
 * include. the includer defines CRATE(name) to name the crate's art and
 * PIECE(name) its pieces' art. BREAK_SOUND is the crate's break sound, and
 * RAW_PIECE_FLAGS the debris flags of a piece thrown without its build.
 * IRON_CRATE is 1 for the iron crate and 0 for the wooden one */
#ifndef SHARED_SPIN_BREAKABLE_IMPL_H
#define SHARED_SPIN_BREAKABLE_IMPL_H
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/spin_crate_shape.h"
#include "shared/spin_crates.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

#define HALF_SIZE 10.0F

/* a crate's modes, in its work's submode. a CRATE_BREAKING crate's pieces
 * have yet to fly. a CRATE_GONE crate's pieces have flown */
enum crate_mode { CRATE_WHOLE, CRATE_BREAKING, CRATE_GONE };

/* a breaking crate's timer counts its steps from 0 at the break. its pieces
 * fly once the timer passes PIECES_STEP */
#define PIECES_STEP 2

#define BREAK_PRIORITY 1
#define BREAK_SOUND_DURATION 80
#define BREAK_POINTS 20
#define BREAK_PUFFS 14
#define BREAK_PUFF_RADIUS 7.0F
#define BREAK_PUFF_RADIUS_RANGE 3.0F
#define BREAK_PUFF_SPEED 0.5F
#define BREAK_PUFF_SCALE 2.5F
#define BREAK_PUFF_SCALE_RANGE 4.0F
/* a piece flies at a speed from PIECE_SPEED up to PIECE_SPEED +
 * PIECE_SPEED_RANGE */
#define PIECE_SPEED 0.7F
#define PIECE_SPEED_RANGE 1.2F

extern NJS_TEXLIST PIECE(piece1_texlist);
extern NJS_TEXLIST PIECE(piece2_texlist);
extern NJS_TEXLIST PIECE(piece3_texlist);
extern NJS_CNK_MODEL PIECE(piece1_model);
extern NJS_CNK_MODEL PIECE(piece2_model);
extern NJS_CNK_MODEL PIECE(piece3_model);
extern NJS_CNK_OBJECT PIECE(piece1_object);
extern NJS_CNK_OBJECT PIECE(piece2_object);
extern NJS_CNK_OBJECT PIECE(piece3_object);

/* a kind of piece a crate breaks into, and how many of it */
struct crate_piece {
    NJS_TEXLIST *texlist;
    NJS_CNK_MODEL *model;
    Sint32 count;
};

#define CRATE_PIECES 3
/* builds holds the crate's model at CRATE_BUILD, then from PIECE_BUILD each
 * of pieces' models, in that array's order */
#define CRATE_BUILD 0
#define PIECE_BUILD 1
#define CRATE_BUILDS (PIECE_BUILD + CRATE_PIECES)

NJS_TEXLIST *CRATE(texlists)[] = {
    &CRATE(texlist),
    &PIECE(piece1_texlist),
    &PIECE(piece2_texlist),
    &PIECE(piece3_texlist),
    NULL,
};
static struct crate_piece pieces[] = {
    {&PIECE(piece1_texlist), &PIECE(piece1_model), 16},
    {&PIECE(piece2_texlist), &PIECE(piece2_model), 4},
    {&PIECE(piece3_texlist), &PIECE(piece3_model), 4},
};
static Bool built = FALSE;
static struct model_build builds[] = {
    {MODEL_BUILD_ALONE_AS_LAND, &CRATE(texlist), &CRATE(object), NULL},
    {MODEL_BUILD_ALONE, &PIECE(piece1_texlist), &PIECE(piece1_object), NULL},
    {MODEL_BUILD_ALONE, &PIECE(piece2_texlist), &PIECE(piece2_object), NULL},
    {MODEL_BUILD_ALONE, &PIECE(piece3_texlist), &PIECE(piece3_object), NULL},
};
static struct collision_shape shape = CRATE_SHAPE;

static void box_destroy(struct task *task);
static void box_step(struct task *task);
static void box_draw(struct task *task);

Uint8 *CRATE(build)(Uint8 *buffer)
{
    if (model_builds_compile(&buffer, builds, CRATE_BUILDS) == CRATE_BUILDS) {
        built = TRUE;
    }
    return buffer;
}

void CRATE(init)(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
#if IRON_CRATE
    if (player_work[1] != NULL) {
        task->step = spin_wood_init;
        return;
    }
#endif
    task->draw = box_draw;
    task->step = box_step;
    task->destructor = box_destroy;
    work->submode = CRATE_WHOLE;
    collision_init_in_place(task, &shape, 1, COLLISION_OBJECT);
}

static void box_destroy(struct task *task)
{
}

#include "shared/breakable_hit.h"

#if IRON_CRATE
static int box_breaks(struct task *task, struct task *other)
{
    struct player *record;
    int slot;
    int character;

    if (breakable_player_hit(task, other) != TRUE) {
        return FALSE;
    }
    slot = player_slot_of(other);
    record = player_records[slot];
    character = record->character;
    if (character == PLAYER_SONIC &&
        (record->upgrades & UPGRADE_BIT(UPGRADE_SONIC_FLAMERING))) {
        return TRUE;
    }
    if (character == PLAYER_TAILS_MECH &&
        (record->upgrades & UPGRADE_BIT(UPGRADE_TWALKER_HYPERCANNON))) {
        return TRUE;
    }
    if (character == PLAYER_KNUCKLES &&
        (record->upgrades & UPGRADE_BIT(UPGRADE_KNUCKLES_GROVES))) {
        return TRUE;
    }
    if (character == PLAYER_SHADOW &&
        (record->upgrades & UPGRADE_BIT(UPGRADE_SHADOW_FLAMERING))) {
        return TRUE;
    }
    if (character == PLAYER_EGGMAN_MECH &&
        (record->upgrades & UPGRADE_BIT(UPGRADE_EWALKER_BAZOOKA))) {
        return TRUE;
    }
    if (character == PLAYER_ROUGE &&
        (record->upgrades & UPGRADE_BIT(UPGRADE_ROUGE_BOOTS))) {
        return TRUE;
    }
    return FALSE;
}
#define BOX_BREAK_TEST box_breaks
#else
#define BOX_BREAK_TEST breakable_player_hit
#endif

static void box_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *hit;
    int i;
    int j;
    int slot;
    NJS_POINT3 center;
    NJS_VECTOR spread;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->submode) {
    case CRATE_WHOLE:
        if ((hit = collision_next_player(task)) != NULL &&
            BOX_BREAK_TEST(task, hit)) {
            center = work->pos;
            center.y += HALF_SIZE;
            for (i = 0; i < BREAK_PUFFS; i++) {
                dust_short_sphere(
                    &center,
                    ((Float)rand() * RAND_SCALE * BREAK_PUFF_RADIUS_RANGE) +
                        BREAK_PUFF_RADIUS,
                    ((Float)rand() * RAND_SCALE) + BREAK_PUFF_SPEED,
                    ((Float)rand() * RAND_SCALE * BREAK_PUFF_SCALE_RANGE) +
                        BREAK_PUFF_SCALE);
            }
            work->timer = 0;
            work->submode = CRATE_BREAKING;
            sound_play_at_for(BREAK_SOUND, work, BREAK_PRIORITY, SOUND_LOUDEST,
                              BREAK_SOUND_DURATION, &work->pos);
            if (hit->work->id == WORK_ID_PLAYER) {
                if ((slot = player_slot_of(hit)) != -1 &&
                    (player_records[slot]->character != PLAYER_SONIC ||
                     player_work[slot]->mode != PLAYER_MODE_SONIC_BOUNCE)) {
                    player_set_speed(slot, 0.0F, 0.0F, 0.0F);
                }
            }
            score_add(BREAK_POINTS);
        } else {
            collision_register(task);
        }
        break;
    case CRATE_BREAKING:
        if (work->timer++ > PIECES_STEP) {
            center = work->pos;
#if IRON_CRATE
            center.y += HALF_SIZE;
#endif
            spread.x = HALF_SIZE;
            spread.y = HALF_SIZE;
            spread.z = HALF_SIZE;
            if (built) {
                for (i = 0; i < CRATE_PIECES; i++) {
                    for (j = 0; j < pieces[i].count; j++) {
                        debris_throw_built_flags(
                            &center, DEBRIS_THROW_SQUARE, &spread, work->ang.y,
                            ((Float)rand() * (PIECE_SPEED_RANGE * RAND_SCALE)) +
                                PIECE_SPEED,
                            NO_FLOOR, pieces[i].model,
                            builds[i + PIECE_BUILD].built, pieces[i].texlist,
                            0);
                    }
                }
            } else {
                for (i = 0; i < CRATE_PIECES; i++) {
                    for (j = 0; j < pieces[i].count; j++) {
                        debris_throw_flags(
                            &center, DEBRIS_THROW_SQUARE, &spread, work->ang.y,
                            ((Float)rand() * (PIECE_SPEED_RANGE * RAND_SCALE)) +
                                PIECE_SPEED,
                            NO_FLOOR, pieces[i].model, pieces[i].texlist,
                            RAW_PIECE_FLAGS);
                    }
                }
            }
            work->submode = CRATE_GONE;
        }
        collision_register(task);
        break;
    case CRATE_GONE:
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        return;
    }
    work->ang.x += (Angle)(work->scl.x * CRATE_TURN_X);
    work->ang.y += (Angle)(work->scl.x * CRATE_TURN_Y);
    work->ang.z += (Angle)(work->scl.x * CRATE_TURN_Z);
}

static void box_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: the set editor's label, in a block no path reaches */
    if (0) {
        (void)"ROTSPD";
    }
    njSetTexture(&CRATE(texlist));
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    if (built) {
        njDirectDrawObject(builds[CRATE_BUILD].built);
    } else {
        njCnkEasyDrawModel(&CRATE(model));
    }
    njPopMatrixEx();
}

#endif
