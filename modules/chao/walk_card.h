/* the walk menu and the card jobs it starts. a drive's VMU holds a Chao, an
 * egg and the items the Chao collected. the Chao and the egg are each a
 * 1024-byte garden_vms_chao */
#ifndef CHAO_WALK_CARD_H
#define CHAO_WALK_CARD_H
#include <ninja.h>
#include <sg_xpt.h>
#include "chao/transfer.h"
#include "save/card.h"
struct task;

struct walk_install {
    Sint32 state, drive;
    struct garden_vms_chao *chao, *egg;
    Uint8 *items, *image, *header;
    Uint16 chao_offset, egg_offset, item_offset, encounter_offset,
        sprite_offset;
};
struct walk_read {
    Sint32 state, drive;
    struct garden_vms_chao *chao, *egg;
    Uint8 *items;
    Sint32 flags;
    Uint8 *header;
    char filename[14];
    Uint16 chao_offset, egg_offset, item_offset;
};
struct walk_slot {
    Sint32 state;
    struct walk_read *read;
    Uint8 *buffer;
};
struct walk_drives {
    struct walk_slot slots[CARD_DRIVES];
    Sint32 mode, request;
    struct garden_vms_chao *chao, *egg;
    Uint8 *items;
    Sint32 result, flags, cursor, column;
};
enum walk_job_state {
    WALK_INSTALL = 0,
    WALK_INSTALLING = 1,
    WALK_READ = 2,
    WALK_READING = 3,
    WALK_SCAN_SEND = 5,
    WALK_SCAN_RECEIVE = 9,
    WALK_FINISHED = 11
};
enum walk_request { WALK_COLLECT, WALK_SEND_CHAO, WALK_SEND_EGG };
struct walk_job {
    Sint32 state;
    struct garden_transfer *transfer;
    Sint32 result;
    union {
        struct walk_install *install;
        struct walk_read *read;
        struct walk_drives *drives;
    } work;
};
enum walk_cell_flags {
    CELL_FRAME = 1,
    CELL_CURRENT_FORMAT = 2,
    CELL_CHAO = 4,
    CELL_EGG = 8,
    CELL_ITEMS = 16,
    CELL_CHECKED = 32,
    CELL_FOCUS = 64
};

enum walk_menu_state {
    MENU_OPEN,
    MENU_ENTER,
    MENU_PICK_CONTENTS,
    MENU_COLLECT_CONTENTS,
    MENU_PICK_CARD,
    MENU_CHOOSE_CARD,
    MENU_UNUSED_SIX,
    MENU_PREPARE_CARD,
    MENU_WRITE_CARD,
    MENU_WAIT_CARD,
    MENU_START_SAVE,
    MENU_WAIT_FILE,
    MENU_RESULT,
    MENU_CLOSE
};

struct walk_menu {
    Uint8 occupied;
    Sint8 send, drive;
    Sint8 message;
    Uint8 unk_04[4];
    Sint16 timer;
    Uint8 unk_0a[2];
    Sint32 state, previous, column, row, cursor, selected, busy, mode;
    Sint32 unk_2c;
    Angle rotation;
    struct task *title;
    struct task *bar, *frame, *cells[10];
    struct task *cursor_task;
    struct walk_job *job;
};

extern struct walk_menu *garden_walk_menu;
extern NJS_TEXLIST *garden_walk_texture_list;
void garden_walk_menu_step(struct task *task);
void garden_walk_menu_draw(struct task *task);
void garden_walk_menu_destroy(struct task *task);
/* run a step of the walk game's install on install->drive, from
 * install->state. return GARDEN_TRANSFER_BUSY until the install ends. it ends
 * GARDEN_TRANSFER_DONE once the card holds the game and install's parts. the
 * parts are those of install's chao, egg and items that aren't null. a card
 * whose game asks to be deleted loses the game instead. that install writes
 * nothing, and ends DONE once the delete ends. a locked game, or one in
 * another language, is deleted and written anew. the install ends
 * GARDEN_TRANSFER_FAILED where a step fails, with a walk message saying why.
 * only a card that isn't ready for that delete fails without one. its end
 * frees install->image and install->header */
Sint32 garden_install_walk_game(struct walk_install *install);
/* run a step of a read of the walk game's file on read->drive, from
 * read->state. return GARDEN_TRANSFER_BUSY until the read ends. it loads the
 * card's Chao, egg and items into read's chao, egg and items. it skips a null
 * egg or items. Sonic Adventure's walk game gives no items. the read ends
 * GARDEN_TRANSFER_DONE once every load succeeds. it ends
 * GARDEN_TRANSFER_FAILED where the card holds no walk game or a locked one,
 * or a load fails. a null read->chao fails it too. its end frees
 * read->header */
Sint32 garden_read_walk_file(struct walk_read *read);
/* a debug display of the drives, by njPrint. the walk job draws it only in
 * states the job never enters */
void garden_draw_walk_drives(struct walk_drives *drives);
/* move each drive's slot in drives on a step. a card that comes in is read,
 * and a card that goes empties its slot. a card's egg record can hold a Chao
 * instead of an egg. the slot then breeds the two Chao, and writes their egg
 * back to the card. it then reads the card again. a slot that starts that
 * write ends the step. the slots after it wait for the next one.
 * garden_walk_menu must be open */
void garden_update_walk_drives(struct walk_drives *drives);
/* garden_select_walk_drive's result where a Chao is sent to a card whose
 * Sonic Adventure walk game holds one. it stands beside the
 * garden_transfer_status values */
#define WALK_SELECT_LEGACY 6
/* take the card in drive index for drives->request. a collect takes a loaded
 * card. a send takes a card with no walk game. it also takes a loaded card
 * whose record of the kind it sends is empty. a loaded card taken has its Chao,
 * egg and items copied into those of drives that aren't null. from Sonic
 * Adventure's walk game, a collect copies no Chao. its egg is the one
 * garden_update_occupied_drives made of the card's Chao. a send sets
 * drives->result to say whether it copied. a card taken returns
 * GARDEN_TRANSFER_DONE. any other returns WALK_SELECT_LEGACY as above, or
 * GARDEN_TRANSFER_FAILED. the call frees every slot's read and buffer,
 * whatever it returns */
Sint32 garden_select_walk_drive(struct walk_drives *drives, Sint32 index);
/* slots holds a slot for each drive. a drive's bit in
 * garden_walk_menu->occupied is set while its slot is loaded. it is set too
 * while the slot holds a card it got no walk game from. the call clears the
 * other bits. a loaded slot of Sonic Adventure's walk game gets a buffer. it
 * holds the egg chao_import_legacy makes of the card's Chao. an empty slot's
 * buffer is freed. garden_walk_menu must be open */
void garden_update_occupied_drives(struct walk_slot *slots);
/* garden_walk_drive_type's results, from the walk menu's last scan of the
 * drives. WALK_DRIVE_NONE is a drive out of range, or one whose card is
 * missing or still being read. WALK_DRIVE_MULTI_BANK is a drive whose card
 * is_multi_bank says has more banks than one, and WALK_DRIVE_CARD any other */
enum walk_drive_type {
    WALK_DRIVE_NONE,
    WALK_DRIVE_CARD,
    WALK_DRIVE_MULTI_BANK
};
Sint8 garden_walk_drive_type(Sint8 drive);
#endif
