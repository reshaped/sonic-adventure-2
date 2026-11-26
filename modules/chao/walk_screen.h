/* the pieces of the walk menu's screen, each a task. a creator makes its
 * piece a child of parent. one that returns the task returns null where
 * parent is null or the task can't be made */
#ifndef CHAO_WALK_SCREEN_H
#define CHAO_WALK_SCREEN_H
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/motion_ids.h"
#include "save/card.h"

struct task;
struct garden_vms_chao;

/* garden_walk_menu.mode. a card job sets WALK_MENU_FREEZE as it starts,
 * which stops the cells' tasks and closes the page arrows. WALK_MENU_CLOSING
 * closes every piece but the previews and their names */
enum walk_menu_mode { WALK_MENU_FREEZE = 3, WALK_MENU_CLOSING = 4 };

/* store the message showing in garden_walk_menu.message. the bar has to be
 * up and showing one */
void walk_remember_message(void);
/* the text id, for garden_message, of link index of walk_message_links */
Uint8 walk_message_text_id(int index);

/* load the menu's textures for the text language, and point
 * garden_walk_texture_list at them. release them, leaving the list null */
void walk_load_textures(void);
void walk_release_textures(void);

/* the most messages the bar's queue holds */
#define WALK_BAR_MESSAGES 4

/* the bar shows its queue of messages in turn, and starts the queue over at
 * its end. once garden_walk_menu.state passes MENU_RESULT, the bar fades out
 * at the queue's end instead. it does so at MENU_RESULT too, once the menu's
 * timer runs out. a message is the chain of walk_message_links from its id.
 * these calls do nothing without a bar */

/* make id the queue's only message, shown from its start. a negative id
 * leaves the bar empty until the next garden_walk_message */
void garden_walk_message(Sint16 id);
/* add id to the queue, unless it holds WALK_BAR_MESSAGES */
void garden_walk_queue_message(Sint16 id);
/* garden_walk_message(id), unless id is the queue's first message already */
void garden_walk_change_message(Sint16 id);
/* the ids of the messages that report a failure */
enum walk_file_notice {
    WALK_NOTICE_NO_SPACE = 10,
    WALK_NOTICE_BAD_FILE = 28,
    WALK_NOTICE_CARD = 30,
    WALK_NOTICE_OTHER_GAME = 31
};

/* the title, the send screen's frame, the receive screen's frame and the
 * bar. delay sets how many steps each waits before it comes in. there is
 * one title, one frame and one bar at a time. a creator makes none while
 * one is up */
void walk_create_title(struct task *parent, Sint16 delay);
struct task *walk_create_send_frame(struct task *parent, Sint16 delay);
struct task *walk_create_receive_frame(struct task *parent, Sint16 delay);
void walk_create_message_bar(struct task *parent, Sint16 delay);

/* a grid's cells run down each column's WALK_GRID_ROWS rows. cell id is in
 * column id / WALK_GRID_ROWS, row id % WALK_GRID_ROWS */
#define WALK_GRID_ROWS 2
/* the receive screen's cells: the Chao's, the egg's, then WALK_MAX_ITEMS
 * items' */
enum walk_content_cell_id { WALK_CHAO_CELL, WALK_EGG_CELL, WALK_ITEM_CELL };
#define WALK_MAX_ITEMS 8
#define WALK_CONTENT_CELLS (WALK_ITEM_CELL + WALK_MAX_ITEMS)
#define WALK_CONTENT_COLUMNS (WALK_CONTENT_CELLS / WALK_GRID_ROWS)
/* the send screen's cells, one a drive */
#define WALK_DRIVE_CELLS CARD_DRIVES
#define WALK_DRIVE_COLUMNS (WALK_DRIVE_CELLS / WALK_GRID_ROWS)
/* the highlight of cell id of a grid. cells is WALK_CONTENT_CELLS for the
 * receive screen's grid, and any other count is the send screen's. the
 * highlight is gold while the flags of cell id's task in
 * garden_walk_menu.cells hold CELL_CHECKED, else grey */
struct task *walk_create_cell_highlight(struct task *parent, Sint8 id,
                                        Sint8 cells);
/* the model of drive id's card in its cell of the send screen's grid. delay
 * sets how many steps it waits to place itself, and to end once the menu
 * closes. it shows nothing for a drive without a card */
struct task *walk_create_drive_frame(struct task *parent, Sint8 id, int delay);

/* the id of the receive screen's one Chao or egg preview. an id below it is
 * the send screen's drive whose card holds the Chao or egg */
#define WALK_CONTENT_PREVIEW 10
/* the Chao or egg saved holds, frozen as a preview, or null where it holds
 * neither. saved can't be null. a drive's preview stands in column and row
 * of the send screen's grid. the receive screen's stands in its one place.
 * column also turns the preview */
struct task *walk_create_chao_preview(struct task *parent, int column, int row,
                                      struct garden_vms_chao *saved, Sint8 id);
/* the Chao or egg of preview task, or null */
struct task *walk_preview_chao(struct task *task);
/* link motion into the Chao of preview task over frames. a preview that
 * walk_create_chao_preview made of an egg takes none */
void walk_set_preview_motion(struct task *task, int motion, int frames);
/* the motions of a cell's Chao: idle, under the cursor, refusing and
 * accepted */
enum walk_preview_motion {
    WALK_PREVIEW_IDLE = CHAO_ANIM_GENKI_SUWARI,
    WALK_PREVIEW_FOCUS = CHAO_ANIM_HAND_DANCE_UP,
    WALK_PREVIEW_REFUSE = CHAO_ANIM_NADE_IYA_LOOP,
    WALK_PREVIEW_ACCEPT = CHAO_ANIM_BANZAI_JUMP
};
/* the egg saved holds, frozen as a preview, or null where saved is null or
 * holds nothing. id is as for walk_create_chao_preview, but unsigned. saved
 * can't hold a Chao */
struct task *walk_create_egg_preview(struct task *parent, int column, int row,
                                     struct garden_vms_chao *saved, Uint8 id);

/* the grids the cursor can mark */
enum walk_cursor_grid { WALK_CURSOR_CONTENTS, WALK_CURSOR_DRIVES };
/* the cursor, on garden_walk_menu's column and row of grid, or null while
 * one is up. it grows at once: delay goes unread. on the contents, it hides
 * on the row below the grid */
struct task *walk_create_cursor(struct task *parent, int delay, Sint8 grid);
/* the model of item in column and row of the receive screen's contents.
 * item is as in garden_transfer.items: a seed below GARDEN_SEED_TYPES, and
 * from there a fruit. the fruit's kind is item + GARDEN_FRUIT_ITEM_OFFSET,
 * which must be below GARDEN_FRUIT_TYPES */
struct task *walk_create_item_preview(struct task *parent, Sint8 column,
                                      Sint8 row, Uint8 item);
/* the receive screen's button, on the row below the contents. it pulses
 * while the cursor is on that row. parent can't be null */
void walk_create_receive_button(struct task *parent);
/* the receive screen's page of drives: the card in drive *drive between its
 * neighbors'. the page turns as *drive changes, so drive has to outlive it */
struct task *walk_create_drive_page(struct task *parent, Sint32 *drive);
/* the arrows beside the page, which press with the L and R triggers.
 * parent can't be null */
void walk_create_page_arrows(struct task *parent);
/* the name of the Chao of preview parent, at position in screen pixels and
 * at depth position->z. it ends once the preview holds no Chao */
struct task *walk_show_preview_name(struct task *parent, NJS_POINT3 *position);
/* kill preview's first child, the name walk_show_preview_name gave it.
 * preview may be null */
void walk_hide_preview_name(struct task *preview);

/* garden_walk_menu's mode and busy flag, or -1 without a menu. a piece sets
 * busy each step it moves, and the menu waits until a step leaves it clear.
 * setting it returns TRUE */
int walk_menu_mode(void);
int walk_menu_busy(void);
int walk_set_menu_busy(int busy);
/* the egg preview of the send screen's drive cell id, or null */
struct task *walk_cell_egg_preview(int id);
#endif
