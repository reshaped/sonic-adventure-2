/* the item stocks: the small animals and chaos drives a team's players
 * take, up to ten a team. the stock of player 0's team shows as icons at
 * the bottom right of the screen */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "draw/screen.h"
#include "hud/item_stock.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"

/* the task's level, and its modes in its work: STOCK_START gives the task
 * its draw */
#define STOCK_LEVEL 1
#define STOCK_START 0
#define STOCK_RUN 1

/* the sounds a push plays, by what it adds */
#define SOUND_ANIMAL 0x800f
#define SOUND_CHAOS_DRIVE 0x8011

/* the hold item_stock_hold sets, and the hold each busy step sets */
#define HOLD_STEPS 100
#define STILL_STEPS 60

/* what each busy step adds to slide, and what each step of a slide out adds
 * to slide_speed */
#define SLIDE_IN 0.1F
#define SLIDE_OUT_ACCEL (-0.001F)
/* each busy step scales a shift above 1 by SHIFT_KEEP. it takes SHIFT_LOSS
 * from any other shift, down to 0 */
#define SHIFT_KEEP 0.96F
#define SHIFT_LOSS 0.04F

/* the icons stand on a row of places, in widths of the screen they draw on.
 * the center of place p lies PLACE_0_X - p * PLACE_WIDTH from the screen's
 * left edge. place 0 lies past its right edge. the oldest item takes place
 * OLDEST_PLACE, and each newer one the next place to the left. the dropped
 * items take the places to the right, the latest nearest the oldest item.
 * outside the pause menu, an icon at place p draws at place
 * (p + shift) * slide. a slide of 0 thus draws every icon at place 0 */
#define PLACE_0_X 1.03125F
#define PLACE_WIDTH 0.0625F
#define OLDEST_PLACE 2

/* an icon's side is ICON_SIDE widths of the screen, and ICON_HALF_SIDE is half
 * that. an arriving icon adds ARRIVING_GROWTH to its side for each step left on
 * its timer. it is never more than half again ICON_SIDE */
#define ICON_HALF_SIDE 0.025F
#define ICON_SIDE 0.05F
#define ARRIVING_GROWTH (ICON_SIDE / (2 * ITEM_ARRIVING))
/* each corner of a dropping icon lies DROP_HALF_DIAGONAL from its center.
 * at an angle of EIGHTH_TURN, it is the upright icon. each step its timer
 * counts down drops it DROP_FALL widths of the screen and turns it
 * DROP_SPIN */
#define DROP_HALF_DIAGONAL 0.035355339F
#define DROP_FALL 0.00625F
#define DROP_SPIN 0x300
/* entries[j]'s icon draws at a priority of ICON_Z + j * ICON_Z_STEP.
 * outside the pause menu, a white backing square draws behind it at
 * BACKING_Z + j * ICON_Z_STEP. an upright icon's backing lies BACKING_INSET
 * pixels in from its edges */
#define ICON_Z 0.98F
#define BACKING_Z 0.97F
#define ICON_Z_STEP 0.001F
#define BACKING_INSET 2.0F

/* holding both of these in the pause menu hides the icons */
#define HIDE_BUTTONS (PDD_DGT_TX | PDD_DGT_TY)

/* the line the icons center on, in pixels */
#define ICONS_Y 424.0F
/* the volume of a push's sound */
#define PUSH_VOLUME 120

struct item_stock item_stocks[TEAMS];
static NJS_TEXNAME al_panel_texname[ITEMS];

/* AL_PANEL's textures, which texture_load_set fills by their places in the
 * PVM */
const NJS_TEXLIST al_panel_texlist = {al_panel_texname, ITEMS};
/* the global index of each item's icon, by item */
static const Uint32 item_icons[ITEMS] = {
#include "hud/item_stock/item_icons.inc"
};

/* the task that steps and draws the stocks, or null */
static struct task *item_stock_task = NULL;

static void item_stock_step(struct task *task);
static void item_stock_draw(struct task *task);
static void item_stock_destroy(struct task *task);

/* the team of the player in slot player, a TEAM_ value */
static int item_stock_team(int player)
{
    struct player *record;
    /* @hack spelling: character, a local for record->character, where the
     * read in the switch would do */
    int character;
    int team;

    team = TEAM_NONE;
    record = player_records[player];
    if (record != NULL) {
        character = record->character;
        switch (character) {
        case PLAYER_SONIC:
        case PLAYER_TAILS:
        case PLAYER_KNUCKLES:
        case PLAYER_TAILS_MECH:
        case 8: /* a character no PLAYER_ name has */
            team = TEAM_HERO;
            break;
        default:
            team = TEAM_DARK;
            break;
        }
    }
    return team;
}

static struct item_stock *item_stock_of_team(int team)
{
    return &item_stocks[team];
}

static struct item_stock *item_stock_of_player(int player)
{
    /* @bug memory: a slot without a record has TEAM_NONE, whose stock lies
     * past the end of item_stocks, over al_panel_texname. for a null
     * record, the BT/S at 0x8c0a6fa8 skips the switch, and the MOV #2,R5 in
     * its slot at 0x8c0a6faa leaves the team 2 */
    return item_stock_of_team(item_stock_team(player));
}

void item_stock_start(void)
{
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((item_stock_task = task_create(TASK_WORK, STOCK_LEVEL, item_stock_step,
                                       "Extra_Exec")) != NULL) {
        item_stock_task->destructor = item_stock_destroy;
    }
}

void item_stock_clear_all(void)
{
    struct item_stock *stock;
    int team;

    for (team = 0; team < TEAMS; team++) {
        stock = item_stock_of_team(team);
        stock->count = 0;
        stock->slide = 0.0F;
        stock->shift = 0.0F;
    }
}

void item_stock_stop(void)
{
    if (item_stock_task != NULL) {
        task_destroy(item_stock_task);
        item_stock_task = NULL;
    }
}

void item_stock_hold(int player)
{
    struct item_stock *stock;

    stock = item_stock_of_player(player);
    stock->hold = HOLD_STEPS;
}

void item_stock_push(int player, int item)
{
    struct item_stock *stock;
    struct stock_entry *entry;

    stock = item_stock_of_player(player);
    if (stock->count == STOCK_ITEMS) {
        for (entry = stock->entries;
             entry < &stock->entries[(2 * STOCK_ITEMS) - 1]; entry++) {
            entry[0] = entry[1];
        }
        stock->count--;
        stock->entries[STOCK_ITEMS - 1].timer = ITEM_DROPPING;
        stock->shift += 1.0F;
    } else {
        stock->shift = 0.0F;
    }
    entry = &stock->entries[++stock->count + (STOCK_ITEMS - 1)];
    entry->item = item;
    entry->timer = ITEM_ARRIVING;
    if (entry->item < ITEM_CHAOS_DRIVE) {
        sound_play_at(SOUND_ANIMAL, NULL, 0, PUSH_VOLUME,
                      &player_work[player]->pos);
    } else {
        sound_play_at(SOUND_CHAOS_DRIVE, NULL, 0, PUSH_VOLUME,
                      &player_work[player]->pos);
    }
}

#ifdef NONMATCHING
/* The garden removes an animal only after its task and registry entry exist. */
int item_stock_peek(int player)
{
    const struct item_stock *stock = item_stock_of_player(player);
    if (stock->count > 0) {
        return stock->entries[STOCK_ITEMS + stock->count - 1].item;
    }
    return -1;
}
#endif

int item_stock_pop(int player)
{
    struct item_stock *stock;
    struct stock_entry *entry;

    stock = item_stock_of_player(player);
    if (stock->count > 0) {
        entry = &stock->entries[--stock->count + STOCK_ITEMS];
        return entry->item;
    }
    return -1;
}

/* a stock's timers count down only once its icons are all the way in. its
 * icons slide out ever faster once the stock has been idle for its hold */
static void item_stock_step(struct task *task)
{
    struct item_stock *stock;
    struct task_work *work;
    struct stock_entry *entry;
    /* @hack type: i and index as Sint32, where int would do */
    Sint32 i;
    Sint32 index;

    stock = item_stocks;
    work = task->work;
    if (work->mode == STOCK_START) {
        task->draw = item_stock_draw;
        work->mode = STOCK_RUN;
    }
    for (i = TEAMS; i != 0; stock++, i--) {
        if (stock->entries[stock->count + (STOCK_ITEMS - 1)].timer > 0 ||
            stock->entries[STOCK_ITEMS - 1].timer > 0 || stock->shift > 0.0F) {
            if ((stock->slide += SLIDE_IN) >= 1.0F) {
                stock->slide = 1.0F;
                for (index = stock->count - 1; index >= 0; index--) {
                    entry = &stock->entries[index + STOCK_ITEMS];
                    if (entry->timer > 0) {
                        /* @hack redundant: a switch on the timer whose one
                         * case does nothing */
                        switch (entry->timer) {
                        case ITEM_ARRIVING:
                            break;
                        }
                        entry->timer--;
                    }
                }
                for (index = STOCK_ITEMS - 1; index >= 0; index--) {
                    entry = &stock->entries[index];
                    if (entry->timer > 0) {
                        entry->timer--;
                    } else {
                        break;
                    }
                }
            }
            if (stock->shift > 1.0F) {
                stock->shift *= SHIFT_KEEP;
            } else {
                if ((stock->shift -= SHIFT_LOSS) < 0.0F) {
                    stock->shift = 0.0F;
                }
            }
            stock->hold = STILL_STEPS;
        } else if (stock->hold > 0) {
            stock->hold--;
            stock->slide_speed = 0.0F;
        } else if (stock->slide > 0.0F) {
            stock->slide += stock->slide_speed += SLIDE_OUT_ACCEL;
        } else {
            /* @hack redundant: an empty else */
        }
    }
}

/* draw the icons of player's stock on a screen width pixels wide, whose
 * left edge is x. they center on the line y. the pause menu draws each stocked
 * item in its place, and nothing more. otherwise the icons draw slid and
 * shifted. an arriving icon hides until its timer starts to count, and
 * then shrinks to its side. a dropping icon falls and turns */
static void item_stock_draw_stock(int player, Float x, Float y, Float width)
{
    struct item_stock *stock;
    struct stock_entry *entry;
    NJS_TEXTURE_VTX vtx[4];
    /* @hack type: i as a Sint32, where an int would do */
    Sint32 i;
    int timer;

    stock = item_stock_of_player(player);
    if (stage_get_mode() == STAGE_PAUSE) {
        for (i = stock->count - 1; i >= 0; i--) {
            Float cx;
            Float cy;
            Float size;

            entry = &stock->entries[i + STOCK_ITEMS];
            cx = x + (width * (PLACE_0_X - ((i + OLDEST_PLACE) * PLACE_WIDTH)));
            size = width * ICON_HALF_SIDE;
            /* @hack spelling: cy, a copy of y, where y would do */
            cy = y;
            vtx[0].x = vtx[1].x = cx - size;
            vtx[2].x = vtx[3].x = cx + size;
            vtx[0].y = vtx[2].y = cy - size;
            vtx[1].y = vtx[3].y = cy + size;
            vtx[0].z = vtx[1].z = vtx[2].z = vtx[3].z =
                ICON_Z + ((i + STOCK_ITEMS) * ICON_Z_STEP);
            vtx[0].col = vtx[1].col = vtx[2].col = vtx[3].col = 0xffffffff;
            vtx[0].u = 0.0F;
            vtx[0].v = 0.0F;
            vtx[1].u = 0.0F;
            vtx[1].v = 1.0F;
            vtx[2].u = 1.0F;
            vtx[2].v = 0.0F;
            vtx[3].u = 1.0F;
            vtx[3].v = 1.0F;
            njDrawTexture(vtx, 4, item_icons[entry->item], 1);
        }
    } else {
        for (i = stock->count - 1; i >= 0; i--) {
            NJS_POLYGON_VTX poly[4];
            Float cx;
            Float cy;
            Float size;

            entry = &stock->entries[i + STOCK_ITEMS];
            timer = entry->timer;
            if (timer < ITEM_ARRIVING) {
                cx = x + (width *
                          (PLACE_0_X -
                           (stock->slide * ((i + OLDEST_PLACE) + stock->shift) *
                            PLACE_WIDTH)));
                /* @hack spelling: cy, a copy of y, where y would do */
                cy = y;
                size = ICON_SIDE;
                if (timer > 0) {
                    size += timer * ARRIVING_GROWTH;
                }
                size *= width * 0.5F;
                vtx[0].x = vtx[1].x = cx - size;
                vtx[2].x = vtx[3].x = cx + size;
                vtx[0].y = vtx[2].y = cy - size;
                vtx[1].y = vtx[3].y = cy + size;
                vtx[0].z = vtx[1].z = vtx[2].z = vtx[3].z =
                    ((i + STOCK_ITEMS) * ICON_Z_STEP) + ICON_Z;
                vtx[0].col = vtx[1].col = vtx[2].col = vtx[3].col = 0xffffffff;
                vtx[0].u = 0.0F;
                vtx[0].v = 0.0F;
                vtx[1].u = 0.0F;
                vtx[1].v = 1.0F;
                vtx[2].u = 1.0F;
                vtx[2].v = 0.0F;
                vtx[3].u = 1.0F;
                vtx[3].v = 1.0F;
                njDrawTexture(vtx, 4, item_icons[entry->item], 1);
                size -= BACKING_INSET;
                poly[0].x = poly[1].x = cx - size;
                poly[2].x = poly[3].x = cx + size;
                poly[0].y = poly[2].y = cy - size;
                poly[1].y = poly[3].y = cy + size;
                poly[0].z = poly[1].z = poly[2].z = poly[3].z =
                    ((i + STOCK_ITEMS) * ICON_Z_STEP) + BACKING_Z;
                poly[0].col = poly[1].col = poly[2].col = poly[3].col =
                    0xffffffff;
                njDrawPolygon(poly, 4, 0);
            }
        }
        for (i = STOCK_ITEMS - 1; i >= 0; i--) {
            NJS_POLYGON_VTX poly[4];
            Float cx;
            Float cy;
            Float size;
            Float cos_part;
            Float sin_part;
            Angle ang;

            entry = &stock->entries[i];
            timer = entry->timer;
            /* an older drop's timer ran out first, so no older drop shows */
            if (timer <= 0) {
                return;
            }
            cx = x + (width *
                      (PLACE_0_X -
                       (stock->slide *
                        ((i - (STOCK_ITEMS - OLDEST_PLACE)) + stock->shift) *
                        PLACE_WIDTH)));
            cy = y + ((ITEM_DROPPING - timer) * width * DROP_FALL);
            ang = ((ITEM_DROPPING - timer) * DROP_SPIN) + EIGHTH_TURN;
            size = width * DROP_HALF_DIAGONAL;
            cos_part = size * njCos(ang);
            sin_part = size * njSin(ang);
            vtx[0].x = cx + sin_part;
            vtx[0].y = cy - cos_part;
            vtx[1].x = cx - cos_part;
            vtx[1].y = cy - sin_part;
            vtx[2].x = cx + cos_part;
            vtx[2].y = cy + sin_part;
            vtx[3].x = cx - sin_part;
            vtx[3].y = cy + cos_part;
            vtx[0].z = vtx[1].z = vtx[2].z = vtx[3].z =
                (i * ICON_Z_STEP) + ICON_Z;
            vtx[0].col = vtx[1].col = vtx[2].col = vtx[3].col = 0xffffffff;
            vtx[0].u = 0.0F;
            vtx[0].v = 0.0F;
            vtx[1].u = 0.0F;
            vtx[1].v = 1.0F;
            vtx[2].u = 1.0F;
            vtx[2].v = 0.0F;
            vtx[3].u = 1.0F;
            vtx[3].v = 1.0F;
            njDrawTexture(vtx, 4, item_icons[entry->item], 1);
            sin_part -= BACKING_INSET;
            cos_part -= BACKING_INSET;
            poly[0].x = cx + sin_part;
            poly[0].y = cy - cos_part;
            poly[1].x = cx - cos_part;
            poly[1].y = cy - sin_part;
            poly[2].x = cx + cos_part;
            poly[2].y = cy + sin_part;
            poly[3].x = cx - sin_part;
            poly[3].y = cy + cos_part;
            poly[0].z = poly[1].z = poly[2].z = poly[3].z =
                (i * ICON_Z_STEP) + BACKING_Z;
            poly[0].col = poly[1].col = poly[2].col = poly[3].col = 0xffffffff;
            njDrawPolygon(poly, 4, 0);
        }
    }
}

static void item_stock_draw(struct task *task)
{
    if (game_state.unk_0b == 0) {
        return;
    }
    if (game_state.cleared == 1) {
        return;
    }
    if (game_state.solo_player != 0) {
        return;
    }
    if (game_state.mini_event_running == 1) {
        return;
    }
    if (stage_get_mode() == STAGE_PAUSE &&
        (player_pads[0]->on & HIDE_BUTTONS) == HIDE_BUTTONS) {
        return;
    }
    /* @hack type: game_state's two_players read as game_state_two_players,
     * where game_state.two_players would do */
    if (game_state_two_players != 0) {
        return;
    }
    switch (stage_current_id()) {
    case STAGE_VERSUS_SONIC_1:
    case STAGE_VERSUS_MECH_1:
    case STAGE_VERSUS_MECH_2:
    case STAGE_VERSUS_KNUCKLES:
    case STAGE_VERSUS_SONIC_2:
    case STAGE_BOSS_BIGFOOT:
    case STAGE_BOSS_HOTSHOT:
    case STAGE_BOSS_FDOG:
    case STAGE_BOSS_BIGBOGY:
    case STAGE_BOSS_GOLEM:
    case STAGE_BOSS_LAST1:
    case STAGE_BOSS_LAST2:
    case STAGE_BOSS_GOLEM_E:
    case STAGE_CART_1:
    case STAGE_CART_2:
        return;
    }
    /* two_players is 0 here, by the return on game_state_two_players above, so
     * the arm for two screens never runs */
    if (game_state.two_players) {
        switch (camera_current_index) {
        case 0:
            item_stock_draw_stock(0, 0.0F, ICONS_Y, SCREEN_WIDTH / 2.0F);
            break;
        case 1:
            item_stock_draw_stock(1, SCREEN_WIDTH / 2.0F, ICONS_Y,
                                  SCREEN_WIDTH / 2.0F);
            break;
        }
    } else {
        item_stock_draw_stock(0, 0.0F, ICONS_Y, SCREEN_WIDTH);
    }
}

static void item_stock_destroy(struct task *task)
{
    item_stock_task = NULL;
}
