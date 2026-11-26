/* the item boxes, ITEMBOX and ITEMBOX2_B in a stage's object list. a player
 * breaks a box for the item inside. ITEM_BOMB's explosions are here too */
#ifndef SHARED_ITEM_BOX_H
#define SHARED_ITEM_BOX_H
#include <ninja.h>
#include <sg_xpt.h>

struct explosion;
struct task;

/* the kinds of item a box can hold. a box that holds ITEM_EMPTY gives
 * nothing and shows no icon */
#define ITEM_10_RINGS 0
#define ITEM_20_RINGS 1
#define ITEM_5_RINGS 2
#define ITEM_BARRIER 3
#define ITEM_BOMB 4
#define ITEM_LIFE 5
#define ITEM_MAGNETIC_BARRIER 6
#define ITEM_EMPTY 7
#define ITEM_SPEED_UP 8
#define ITEM_INVINCIBLE 9
#define ITEM_1UP 10
/* the points an item scores as it is given. the 5-ring and 10-ring items
 * score their own. ITEM_EMPTY scores none, and every other item
 * ITEM_POINTS_OTHER */
#define ITEM_POINTS_5_RINGS 50
#define ITEM_POINTS_10_RINGS 100
#define ITEM_POINTS_OTHER 200
/* what each ring of a ring item adds to a mech's gauge */
#define ITEM_GAUGE_PER_RING 0.05F
/* the most a mech's gauge holds, with UPGRADE_EWALKER_EXTRASHIELD. ITEM_LIFE
 * adds it, which fills any gauge */
#define ITEM_GAUGE_FULL 12.0F

/* a kind of item, and the call that gives it to the player in slot player.
 * box is the giving box's task, or null where item_give calls. a box's or a
 * balloon's table of items holds one for each of the ITEM_BOX_ITEMS kinds */
#define ITEM_BOX_ITEMS 11
struct item_box_item {
    Sint32 kind;
    void (*give)(struct task *box, int player);
};

extern const char item_box_texture_filenames[45];

/* the texture lists for texture_load_shared of the boxes, their icons and
 * their effects, ended by null */
extern NJS_TEXLIST *item_box_texlists[];

/* the two boxes' texture lists and models. both lists name the same three
 * textures, and every draw of either box sets item_box_texlist. the SG item
 * box draws ITEMBOX's models too */
extern NJS_TEXLIST item_box_texlist, item_box_b_texlist;
extern NJS_CNK_MODEL item_box_lower_model, item_box_middle_model,
    item_box_upper_model;
extern NJS_CNK_OBJECT item_box_lower_object, item_box_middle_object,
    item_box_upper_object;
extern NJS_CNK_MODEL item_box_b_lower_model, item_box_b_middle_model,
    item_box_b_upper_model;
extern NJS_CNK_OBJECT item_box_b_lower_object, item_box_b_middle_object,
    item_box_b_upper_object;

/* build at buf each of the boxes' six models not built yet whose texture
 * list is loaded. return buf past the builds. the boxes draw the builds only
 * after a call that builds all six. until then they draw the models
 * unbuilt */
Uint8 *item_box_build(Uint8 *buf);

/* the object list's steps for ITEMBOX and ITEMBOX2_B, which differ only in
 * their models. task needs a work, and must be created without TASK_FORCE. the
 * box keeps its item's index in task->force and an enemy_work in task->motion.
 * the heap must have room for the enemy_work, unchecked.
 *
 * the work's scl.x, cut to a whole number and held from 0 to 10, picks the
 * item. from 0 the items are ITEM_SPEED_UP, ITEM_5_RINGS, ITEM_1UP,
 * ITEM_10_RINGS, ITEM_20_RINGS, ITEM_BARRIER, ITEM_BOMB, ITEM_LIFE,
 * ITEM_MAGNETIC_BARRIER, ITEM_EMPTY and ITEM_INVINCIBLE. ang.y and ang.z turn
 * the box.
 *
 * the box can break once it has grown to its full size. a touch by a player's
 * or an attack's collision record breaks it. so does WORK_HURT in the work's
 * flags, while the enemy_work's shot_by isn't -1. the item goes to the player
 * whose record touched the box. after WORK_HURT, or a touch by no player's
 * record, it goes to the player in slot shot_by. where shot_by is then below 0,
 * the box stays whole. a slot of 0 or more must hold a player, unchecked. a
 * player whose record has PLAYER_MISSED breaks the box but gets nothing. the
 * box shows the item by item_icon_row_add.
 *
 * a box that breaks sets PLACEMENT_USED in its placement. a box whose placement
 * has the flag starts broken. a box that gives its 1-up sets PLACEMENT_UNK_20
 * in its placement. a box of ITEM_1UP whose placement has PLACEMENT_UNK_20 but
 * not PLACEMENT_USED holds ITEM_EMPTY instead.
 *
 * a box whose work's submode isn't 0 is a child box, whose task must have a
 * parent, unchecked. a child box never leaves range. it takes either flag as
 * set where its own placement or its parent's has it. it sets PLACEMENT_USED in
 * its own placement, or in its parent's where it has none. it sets
 * PLACEMENT_UNK_20 in its parent's placement, or in its own where the parent
 * has none */
void item_box_step(struct task *task);
void item_box_b_step(struct task *task);

/* give the player in slot player the item of kind, as a box would. the call
 * shows the item by item_icon_row_add. it does nothing for a player whose
 * record has PLAYER_MISSED. nor does it for ITEM_EMPTY, or for a kind outside
 * the ITEM_ kinds. the player's record must exist, unchecked.
 *
 * kind must not be ITEM_1UP or ITEM_BOMB, unchecked. their give calls read the
 * box's task, which item_give passes as null */
void item_give(int player, int kind);

/* start an explosion at pos. its radius grows from 0 by 10 a step. it ends on
 * the step the radius passes its limit. the limit is 500 for explosion_create,
 * and limit for explosion_create_sized. limit must be above 0, unchecked. at a
 * limit of 0, a draw before the first step divides 0 by 0 for its fade.
 *
 * the explosions share one task, which ends with the last of them. while
 * enemy/enemy.h's enemy_hit_hook is null, each call that makes an explosion
 * sets it to this source's test. the test returns TRUE for a pos nearer an
 * explosion's pos than its radius. enemy_take_hit hurts an enemy at such a pos.
 * the task's end sets enemy_hit_hook back to null where it still holds the
 * test.
 *
 * each call returns its explosion, which stays valid until it ends. it returns
 * null where the heap has no room for the task or the explosion. it then makes
 * no explosion and leaves enemy_hit_hook as it was */
struct explosion *explosion_create(NJS_POINT3 *pos);
struct explosion *explosion_create_sized(NJS_POINT3 *pos, Float limit);

#endif
