/* the item stocks: the small animals and chaos drives a team's players
 * take, up to ten a team. the stock of player 0's team shows as icons at
 * the bottom right of the screen */
#ifndef HUD_ITEM_STOCK_H
#define HUD_ITEM_STOCK_H
#include <sg_xpt.h>

/* an item is a small animal's kind, below ITEM_CHAOS_DRIVE, or
 * ITEM_CHAOS_DRIVE plus a chaos drive's kind. ITEMS counts them */
#define ITEM_CHAOS_DRIVE 21
#define ITEMS 25

/* the teams, by a player's character. TEAM_HERO holds Sonic, Tails,
 * Knuckles, Tails's mech and character 8. TEAM_DARK holds the others.
 * TEAM_NONE, the team of a slot without a record, has no stock */
#define TEAM_HERO 0
#define TEAM_DARK 1
#define TEAM_NONE 2
#define TEAMS 2

/* the most items a stock holds */
#define STOCK_ITEMS 10

/* an item in a stock, and its timer in steps. a taken item's timer starts
 * at ITEM_ARRIVING. it starts again at ITEM_DROPPING when the item drops
 * out of a full stock. timers count down only while the stock's icons are
 * all the way in */
struct stock_entry {
    Sint32 item;
    Sint32 timer;
};
#define ITEM_ARRIVING 30
#define ITEM_DROPPING 60

/* a team's stock. its count items lie from entries[STOCK_ITEMS] up, the
 * oldest first. the entries below hold the items that dropped out, the latest
 * at entries[STOCK_ITEMS - 1]. a dropped item shows while its timer runs. the
 * stock is busy while its newest item's timer or the latest drop's runs. it
 * is busy too while shift is above 0. a busy stock's icons slide in */
struct item_stock {
    struct stock_entry entries[2 * STOCK_ITEMS];
    Sint32 count;
    /* how far the icons have slid in, from 0 to 1, all the way. a slide out
     * stops at the first step that finds slide at 0 or below. slide can
     * therefore rest below 0. slide_speed, a slide out's speed, is negative */
    Float slide;
    Float slide_speed;
    /* how many places left of its own each icon draws. a push into a full
     * stock moves each item a place right. it adds 1 to shift, so that the
     * icons draw where they were. the steps take shift back to 0, so the icons
     * glide into place. any other push sets it to 0 */
    Float shift;
    /* the steps the icons stay in, once the stock isn't busy, before they
     * slide out. each busy step sets it to 60 */
    Sint32 hold;
};

/* the stocks of TEAM_HERO and TEAM_DARK */
extern struct item_stock item_stocks[TEAMS];

/* start Extra_Exec, the task that steps and draws the stocks, unless the
 * heap can't give it */
void item_stock_start(void);
/* empty both stocks, slid out and unshifted. the items that dropped out
 * keep their timers. a stock whose latest drop still runs slides back in */
void item_stock_clear_all(void);
/* destroy the task, if it runs */
void item_stock_stop(void);

/* the calls below take the stock of the team of the player in slot player.
 * the slot must hold a player's record. an empty slot's team is TEAM_NONE,
 * whose stock lies past the end of item_stocks */

/* set the stock's hold to 100 steps. once the stock is idle, its icons stay
 * where they stand that long before they slide out. the call slides no icon
 * in */
void item_stock_hold(int player);
/* add item to the stock, and play an animal's or a drive's sound at the
 * player's place. in a full stock, the oldest item drops out. the slot's
 * work must exist, unchecked */
void item_stock_push(int player, int item);
#ifdef NONMATCHING
/* Return the newest item, or -1 for an empty stock. */
int item_stock_peek(int player);
#endif
/* take the stock's newest item and return it, or return -1 where the stock
 * is empty */
int item_stock_pop(int player);

#endif
