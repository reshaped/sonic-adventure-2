/* a boss's record, its power gauge, and the messengers a boss summons */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "draw/lines.h"
#include "object/mover.h"
#include "player/player.h"
#include "shared/boss.h"
#include "shared/messenger.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/vector.h"
#ifdef NONMATCHING
/* texture_slot_loaded, by which this side's gauge tests each piece's texture */
#include "system/texture.h"
#endif

/* the task level of a gauge made without a parent */
#define GAUGE_LEVEL 3
/* a new gauge shows its frame alone for WAIT_STEPS + 1 steps */
#define WAIT_STEPS 40
/* the bar is LENGTH_PER_POWER pixels long for each unit of power */
#define LENGTH_PER_POWER 32.0F
/* a closing gauge kills its task once its length falls below END_LENGTH */
#define END_LENGTH 10.0F
/* a bar that follows the power turns red at LOW_LENGTH or less */
#define LOW_LENGTH 32.0F

/* the center of the frame's right end, in pixels on a screen 640 wide */
#define FRAME_X 612.0F
#define FRAME_Y 40.0F
/* the priority the frame and the label draw at, which the sprite keeps in
 * p.z. a priority p below -1 gives a sprite the depth of a point -p away */
#define FRAME_PRIORITY (-1.5F)
/* the bar draws at a priority BAR_DEPTH below the frame's. that puts it
 * BAR_DEPTH farther away, behind the frame */
#define BAR_DEPTH 0.01F
/* each piece of the gauge is a cell CELL_SIZE pixels square. a row's pieces
 * lie CELL_SIZE apart */
#define CELL_SIZE 16
/* a texture's side, as a cell's u and v count it */
#define TEXTURE_SIDE 256
/* the label lies a row below the frame, in LABEL_CELLS cells */
#define LABEL_CELLS 5

/* the indices of boss_gauge_texlist's textures. the bar's comes first. the
 * frame's five pieces follow, from its left end to its right end. the
 * label's five cells come next, from left to right. the list ends with
 * hyoji_hpbar_f5, which no draw here selects */
#define TEXTURE_BAR 0
#define TEXTURE_LEFT_END 1
#define TEXTURE_LEFT 2
#define TEXTURE_MIDDLE 3
#define TEXTURE_RIGHT 4
#define TEXTURE_RIGHT_END 5
#define TEXTURE_LABEL_LAST 10
#define GAUGE_TEXTURES 12

/* how far above the player's pos each summon puts its messenger */
#define CAPPED_HEIGHT 20.0F
#define SUMMON_HEIGHT 30.0F

/* a gauge's modes, in its work's mode, in the order it takes them */
enum gauge_mode { MODE_WAIT, MODE_FILL, MODE_FOLLOW, MODE_CLOSE };

/* a gauge's work, laid over the struct task_work that task_create
 * allocates. power lies at ang.x */
struct gauge_work {
    Sint8 mode;
    Uint8 unk_01[5];
    Uint16 timer;
    Float *power;
    Uint8 unk_0c[8];
    NJS_POINT3 pos;
    NJS_POINT3 scl;
};

#define GAUGE(task) ((struct gauge_work *)(task)->work)
/* the bar's length in pixels. a closing gauge draws its frame two cells
 * longer than the length */
#define LENGTH(work) ((work)->pos.x)
/* what MODE_FILL adds to the length at each step. MODE_CLOSE takes twice as
 * much off */
#define FILL_STEP(work) ((work)->pos.z)
/* what MODE_FOLLOW takes off the length at each step */
#define DRAIN_STEP(work) ((work)->scl.y)
/* *power at boss_gauge_create, which a full bar shows */
#define FULL_POWER(work) ((work)->scl.z)

#include "shared/boss/text.inc"

static NJS_TEXNAME boss_gauge_texname[GAUGE_TEXTURES] = {
#include "shared/boss/boss_gauge_texname.inc"
};
NJS_TEXLIST boss_gauge_texlist = {boss_gauge_texname, GAUGE_TEXTURES};
/* boss_gauge_create gives each gauge fill_step and drain_step, in pixels a
 * step */
struct boss_gauge_rates {
    Float unk_00;
    Float fill_step;
    Float drain_step;
    Float unk_0c[3];
};
static struct boss_gauge_rates gauge_rates = {
    30.0F, 0.8F, 0.4F, {20.0F, 0.8F, 0.4F}};

static void gauge_destroy(struct task *task);
static void gauge_step(struct task *task);
static void gauge_draw(struct task *task);

/* nothing calls boss_record_alloc or boss_messenger_summon. neither is static,
 * since SHC leaves out a static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* a new zeroed record that no task holds, or null where the heap has no
 * room */
struct boss_record *boss_record_alloc(void);
/* summon a messenger as boss_messenger_summon_capped does, but SUMMON_HEIGHT
 * above the player, to show stage message message */
void boss_messenger_summon(int player, int message, struct task *parent);
/* NOLINTEND(misc-use-internal-linkage) */

static struct boss_record *record_create(struct task *task, Bool with_motion,
                                         Bool with_block)
{
    struct boss_record *record;
    void *block;

#ifdef NONMATCHING
    /* task->motion as it was, which this side puts back where the record
     * fails */
    void *previous_motion = task->motion.p;
#endif

    record = syCalloc(1, sizeof(struct boss_record));
    if (record == NULL) {
        return NULL;
    }
#ifdef NONMATCHING
    /* this side repairs the bugs tagged in the matching side. it makes each
     * part before it installs the record. where a part fails, it frees what it
     * made. the task's extra and motion then stay as they were, as boss.h
     * says */
    if (with_motion) {
        record->motion = mover_alloc(task);
        if (record->motion == NULL) {
            syFree(record);
            return NULL;
        }
    }
    if (with_block) {
        block = syCalloc(BOSS_BLOCK_SIZE, 1);
        if (block == NULL) {
            if (record->motion != NULL) {
                task->motion.p = previous_motion;
                syFree(record->motion);
            }
            syFree(record);
            return NULL;
        }
        record->block = block;
    }
    task->extra.p = record;
#else
    task->extra.p = record;
    if (with_motion) {
        /* @hack spelling: task reused for the mover, where a local of its
         * own that starts as task would do */
        task = (struct task *)mover_alloc(task);
        if (task == NULL) {
            /* @bug memory: frees the record but leaves it in the caller's
             * task's extra for task_destroy to free again. a failed block
             * below does the same. the frees are at 0x34 and 0x5c of the
             * object's code */
            syFree(record);
            return NULL;
        }
        record->motion = task;
    } else {
        /* @hack redundant: the record's motion cleared, though syCalloc
         * cleared the record */
        record->motion = NULL;
    }
    if (with_block) {
        block = syCalloc(BOSS_BLOCK_SIZE, 1);
        if (block == NULL) {
            /* @bug memory: frees the caller's task where with_motion is
             * FALSE, since only with_motion makes task the mover. under
             * with_motion, the freed mover stays in the caller's task's
             * motion for task_destroy to free again. the free is at 0x58 of
             * the object's code. boss_last1's image calls with with_motion
             * FALSE and with_block TRUE at 0x8c50046e */
            syFree(task);
            syFree(record);
            return NULL;
        }
        record->block = block;
    } else {
        /* @hack redundant: the record's block cleared, though syCalloc
         * cleared the record */
        record->block = NULL;
    }
#endif
    return record;
}

struct boss_record *boss_record_create(struct task *task)
{
    struct boss_record *record;

    record = record_create(task, TRUE, TRUE);
    /* @hack redundant: the result tested for null before it is returned,
     * where returning it would do */
    if (record == NULL) {
        return NULL;
    }
    return record;
}

struct boss_record *boss_record_create_parts(struct task *task,
                                             Bool with_motion, Bool with_block)
{
    struct boss_record *record;

    record = record_create(task, with_motion, with_block);
    /* @hack redundant: the result tested for null before it is returned,
     * where returning it would do */
    if (record == NULL) {
        return NULL;
    }
    return record;
}

struct boss_record *boss_record_alloc(void)
{
    struct boss_record *record;

    record = syCalloc(1, sizeof(struct boss_record));
    /* @hack redundant: the result tested for null before it is returned,
     * where returning it would do */
    if (record == NULL) {
        return NULL;
    }
    return record;
}

void *boss_record_alloc_own(struct boss_record *record, Uint32 size)
{
    void *own;

    if (record == NULL) {
        return NULL;
    }
    own = syCalloc(1, size);
    if (own == NULL) {
        return NULL;
    }
    record->own = own;
    /* @hack spelling: the own block read back from the record, where own
     * would do */
    return record->own;
}

void boss_record_free_parts(struct task *task)
{
    struct boss_record *record = task->extra.p;

#ifdef NONMATCHING
    /* this side repairs the bug tagged below: a null record frees nothing */
    if (record == NULL) {
        return;
    }
#endif
    /* @bug memory: reads and writes a boss's record through null.
     * boss_record_alloc_own tests its record for null at 0x10e of the
     * object's code. boss_record_free_parts reads through task->extra at
     * 0x140, and boss_record_set_focus writes through its record at 0x70c */
    if (record->own != NULL) {
        syFree(record->own);
        record->own = NULL;
    }
    if (record->block != NULL) {
        syFree(record->block);
        record->block = NULL;
    }
    if (record->motion != NULL) {
        /* @hack spelling: task->motion freed, where the record's motion
         * would do while task->motion holds the same record */
        syFree(task->motion.p);
        record->motion = NULL;
        task->motion.p = NULL;
    }
}

struct task *boss_gauge_create(struct task *parent, Float *power)
{
    struct task *task;
    struct gauge_work *work;

    if (parent == NULL) {
        task = task_create(TASK_WORK, GAUGE_LEVEL, gauge_step,
                           "BossPowerGuageExec");
    } else {
        task = task_create_child(TASK_WORK, gauge_step, parent);
    }
#ifdef NONMATCHING
    /* this side returns null where the task can't be made. the matching side
     * writes through the null */
    if (task == NULL) {
        return NULL;
    }
#endif
    work = GAUGE(task);
    work->power = power;
    /* @hack spelling: the power read through the work's pointer, where
     * power would do */
    FULL_POWER(work) = *work->power;
    LENGTH(work) = 0.0F;
    /* @hack spelling: the task's draw and destructor set between the work's
     * fields, where the work's fields set together would do */
    task->draw = gauge_draw;
    task->destructor = gauge_destroy;
    FILL_STEP(work) = gauge_rates.fill_step;
    DRAIN_STEP(work) = gauge_rates.drain_step;
    return task;
}

static void gauge_destroy(struct task *task)
{
}

static void gauge_step(struct task *task)
{
    struct gauge_work *work = GAUGE(task);
    Float limit;

    if (game_state.time_stopped || game_state.mini_event_running) {
        return;
    }
    switch (work->mode) {
    case MODE_WAIT:
        if (work->timer++ == WAIT_STEPS) {
            work->mode++;
        }
        break;
    case MODE_FILL:
        limit = FULL_POWER(work) * LENGTH_PER_POWER;
        LENGTH(work) += FILL_STEP(work);
        if (LENGTH(work) >= limit) {
            LENGTH(work) = limit;
            work->mode++;
        }
        break;
    case MODE_FOLLOW:
        limit = *work->power * LENGTH_PER_POWER;
        if (LENGTH(work) > limit) {
            LENGTH(work) -= DRAIN_STEP(work);
            if (LENGTH(work) < limit) {
                LENGTH(work) = limit;
            }
        }
        if (LENGTH(work) <= 0.0F) {
            LENGTH(work) = FULL_POWER(work) * LENGTH_PER_POWER;
            work->mode++;
        }
        break;
    case MODE_CLOSE:
        LENGTH(work) -= FILL_STEP(work) * 2.0F;
        if (LENGTH(work) < END_LENGTH) {
            task_kill(task);
        }
        break;
    }
}

#ifdef NONMATCHING
/* this side draws a gauge piece only where its texture is loaded. before the
 * first piece it draws, it turns the fog off and sets the blending */
static Bool gauge_piece_ready(NJS_TEXLIST *list, int texture, Bool *drawing)
{
    if (!texture_slot_loaded(list, texture)) {
        return FALSE;
    }
    if (!*drawing) {
        njFogDisable();
        njSetTexture(list);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        *drawing = TRUE;
    }
    return TRUE;
}

static void gauge_frame_piece(NJS_SPRITE *piece, Bool *drawing)
{
    if (gauge_piece_ready(piece->tlist, piece->tanim->texid, drawing)) {
        njDrawSprite2D(piece, 0, piece->p.z, NJD_SPRITE_ALPHA);
    }
}
#define DRAW_GAUGE_CELL(piece, drawing) gauge_frame_piece(piece, drawing)
#else
#define DRAW_GAUGE_CELL(piece, drawing)                                        \
    njDrawSprite2D(piece, 0, (piece)->p.z, NJD_SPRITE_ALPHA)
#endif

/* draw the frame, the label under it and the bar inside it. each is a row of
 * pieces, drawn from right to left. piece, a copy of sprite, moves along each
 * row. every piece takes its size, center and texture from cell through tanim.
 * so a change to cell holds for each piece drawn after it */
static void gauge_draw(struct task *task)
{
    struct gauge_work *work = GAUGE(task);
    NJS_TEXANIM cell = {CELL_SIZE, CELL_SIZE, CELL_SIZE / 2, CELL_SIZE / 2,
                        0,         0,         TEXTURE_SIDE,  TEXTURE_SIDE,
                        0,         0};
    /* @hack type: sprite and piece as arrays of one, where plain records
     * would do */
    NJS_SPRITE sprite[1] = {{{0.0F, 0.0F, 0.0F}, 1.0F, 1.0F, 0, NULL, NULL}};
    NJS_SPRITE piece[1];
    NJS_POINT3 frame = {FRAME_X, FRAME_Y, FRAME_PRIORITY};
    /* @hack type: pieces as a Sint8, where an int would do while the frame
     * has under 128 pieces */
    Sint8 pieces;
    /* @hack type: i as a Sint8, where an int would do while the frame has
     * under 128 pieces */
    Sint8 i;
    /* @hack type: texture as a Sint8, where an int would do */
    Sint8 texture;
    Float remaining;
    Float cut;
    Float width;
    Float red;
    Float blue;
    Float half;
    /* @hack spelling: texlist, a local for &boss_gauge_texlist, where the
     * address written at each use would do */
    NJS_TEXLIST *texlist;
#ifdef NONMATCHING
    /* whether this side has drawn a piece. it turns the fog back on after
     * one */
    Bool drawing = FALSE;
#endif

    if (game_state.mini_event_running) {
        return;
    }
#ifdef NONMATCHING
    /* gauge_piece_ready disables fog before the first piece it draws */
#else
    njFogDisable();
#endif
    sprite->tanim = &cell;
    /* @hack redundant: sprite's p set from the work's pos, which the next
     * store to p replaces */
    sprite->p = work->pos;
    texlist = &boss_gauge_texlist;
    sprite->tlist = texlist;
    sprite->p = frame;
#ifdef NONMATCHING
#else
    njSetTexture(texlist);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
#endif
    piece[0] = sprite[0];
    if (work->mode != MODE_CLOSE) {
        pieces = (FULL_POWER(work) * 2.0F) + 2.0F;
        for (i = 0; i < pieces; i++) {
            switch (i) {
            case 0:
                texture = TEXTURE_RIGHT_END;
                break;
            case 1:
                texture = TEXTURE_RIGHT;
                break;
            default:
                if (i == pieces - 2) {
                    texture = TEXTURE_LEFT;
                } else if (i == pieces - 1) {
                    texture = TEXTURE_LEFT_END;
                } else {
                    texture = TEXTURE_MIDDLE;
                }
                break;
            }
            piece->tanim->texid = texture;
            DRAW_GAUGE_CELL(piece, &drawing);
            piece->p.x -= CELL_SIZE;
        }
    } else {
        /* a closing frame spans the length and two cells more. its pieces
         * draw from the right end, a cell apart. the middle piece before the
         * left end moves right. the left end then lands where the span ends.
         * where that piece is the second from the right, it narrows
         * instead */
        remaining = LENGTH(work) + (CELL_SIZE * 2.0F);
        /* @hack spelling: i = i + 1, where i++ would do */
        for (i = 0; remaining >= 0.0F; i = i + 1) {
            cut = 0.0F;
            if (i == 0) {
                texture = TEXTURE_RIGHT_END;
            } else if (remaining <= CELL_SIZE) {
                texture = TEXTURE_LEFT_END;
                remaining = -1.0F;
            } else {
                texture = TEXTURE_MIDDLE;
            }
            piece->tanim->texid = texture;
            if (remaining <= CELL_SIZE * 2.0F && remaining > CELL_SIZE) {
                if (i == 1) {
                    /* @hack spelling: the assignment to width inside the
                     * expression for half, where a statement of its own
                     * would do */
                    half = (width = remaining - CELL_SIZE) * 0.5F;
                    piece->p.x = frame.x - width;
                    piece->tanim->sx = width;
                    /* @bug logic: draws the narrowed piece over the frame's
                     * right end in a closing gauge's last steps. cell keeps
                     * this cx, half in place of CELL_SIZE / 2, and the label
                     * sets back only cell's sx. so the left end and the
                     * label's cells draw CELL_SIZE / 2 - half too far right.
                     * the stores are at 0x538 and 0x5e6 of the object's code */
                    piece->tanim->cx = half;
                    cut = width;
                } else {
                    /* @hack spelling: width, a local for how far the piece
                     * moves right, where the distance added in place would
                     * do */
                    width = (CELL_SIZE * 2.0F) - remaining;
                    piece->p.x += width;
                }
                remaining = CELL_SIZE;
            }
            DRAW_GAUGE_CELL(piece, &drawing);
            remaining -= CELL_SIZE;
            if (cut != 0.0F) {
                piece->p.x -= cut;
            } else {
                piece->p.x -= CELL_SIZE;
            }
        }
    }
    piece[0] = sprite[0];
    piece->p.y += CELL_SIZE;
    piece->tanim->texid = TEXTURE_LABEL_LAST;
    piece->tanim->sx = CELL_SIZE;
    /* @hack spelling: i = i + 1, where i++ would do */
    for (i = 0; i < LABEL_CELLS; i = i + 1) {
        DRAW_GAUGE_CELL(piece, &drawing);
        piece->p.x -= CELL_SIZE;
        piece->tanim->texid--;
    }
    piece[0] = sprite[0];
    if (work->mode != MODE_WAIT && work->mode != MODE_CLOSE) {
#ifdef NONMATCHING
        /* this side skips the bar where its texture isn't loaded */
        if (!gauge_piece_ready(texlist, TEXTURE_BAR, &drawing)) {
            goto finish;
        }
#endif
        piece->p.x -= CELL_SIZE;
        piece->tanim->texid = TEXTURE_BAR;
        if (LENGTH(work) <= LOW_LENGTH && work->mode >= MODE_FOLLOW) {
            red = 1.0F;
            blue = 0.0F;
        } else {
            red = 0.0F;
            blue = 1.0F;
        }
        constant_material_set(1.0F, red, 0.0F, blue);
        /* a bar of a cell or less is one piece, narrowed to the length. a
         * longer bar is whole pieces. its last piece overlaps the one before,
         * so that the bar spans the length */
        remaining = LENGTH(work);
        while (remaining >= 0.0F) {
            if (LENGTH(work) <= CELL_SIZE) {
                half = LENGTH(work) * 0.5F;
                /* @bug logic: puts the right edge of a bar of a cell or less
                 * at frame.x. a longer bar's right edge lies half a cell left
                 * of frame.x, from the store at 0x640 of the object's code.
                 * the store here is at 0x68e */
                piece->p.x = frame.x - half;
                piece->tanim->sx = LENGTH(work);
                piece->tanim->cx = half;
                remaining = -1.0F;
            } else if (remaining <= CELL_SIZE) {
                piece->p.x += CELL_SIZE - remaining;
                remaining = -1.0F;
            }
            njDrawSprite2D(piece, 0, piece->p.z - BAR_DEPTH,
                           NJD_SPRITE_COLOR | NJD_SPRITE_ALPHA);
            remaining -= CELL_SIZE;
            piece->p.x -= CELL_SIZE;
        }
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    }
#ifdef NONMATCHING
/* where this side's draw ends. it turns the fog back on only where a piece
 * turned it off */
finish:
    if (!drawing) {
        return;
    }
#endif
    njFogEnable();
}

void boss_record_set_focus(struct boss_record *record, NJS_POINT3 *pos,
                           NJS_VECTOR *offset)
{
    record->focus.x = pos->x + offset->x;
    record->focus.y = pos->y + offset->y;
    record->focus.z = pos->z + offset->z;
}

void boss_vector_sum(NJS_VECTOR *out, const NJS_VECTOR *first,
                     const NJS_VECTOR *second)
{
    vector_sum(first, second, out);
}

void boss_messenger_summon_capped(int player, int message_count,
                                  struct task *parent)
{
    NJS_POINT3 pos;
    int message = stage_get_retries() - 1;

    if (message < 0) {
        return;
    }
    if (message > message_count - 1) {
        message = message_count - 1;
    }
    pos = player_work[player]->pos;
    pos.y += CAPPED_HEIGHT;
    messenger_create(&pos, 0, player, message, MESSENGER_SUMMON, parent);
}

void boss_messenger_summon(int player, int message, struct task *parent)
{
    NJS_POINT3 pos;

    pos = player_work[player]->pos;
    pos.y += SUMMON_HEIGHT;
    messenger_create(&pos, 0, player, message, MESSENGER_SUMMON, parent);
}
