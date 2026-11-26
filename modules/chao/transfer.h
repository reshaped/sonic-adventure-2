/* the exchanges of Chao between the garden and a VMU. each call below starts
 * a task that works through *transfer, which must outlive it. a call returns
 * 0, or -1 where its task can't start */
#ifndef CHAO_TRANSFER_H
#define CHAO_TRANSFER_H
#include <sg_xpt.h>
#include "chao/race_record.h"
#include "chao/residents.h"

/* garden_transfer.status, and the card jobs' results. a walk menu ends
 * GARDEN_TRANSFER_CANCELLED where the player backs out or confirms nothing.
 * picking a card that already holds a Chao ends it so too. any other end
 * without a transfer is GARDEN_TRANSFER_FAILED */
enum garden_transfer_status {
    GARDEN_TRANSFER_IDLE, /* the walk menu waits on the player */
    GARDEN_TRANSFER_BUSY,
    GARDEN_TRANSFER_DONE,
    GARDEN_TRANSFER_CANCELLED,
    GARDEN_TRANSFER_FAILED
};
/* garden_transfer.phase: the walk menu is open, its install runs, then the
 * card holds what the install wrote */
enum garden_transfer_phase {
    GARDEN_TRANSFER_PHASE_MENU,
    GARDEN_TRANSFER_PHASE_INSTALLING,
    GARDEN_TRANSFER_PHASE_INSTALLED
};

struct garden_vms_chao {
    struct chao_save saved;
    Uint8 unk_3e8[24];
};

/* chao and egg hold the Chao's and the egg's records, whichever way the
 * transfer runs. chao_returned and egg_received say whether a receive
 * delivers each */
struct garden_transfer {
    Sint32 drive, status, phase, unk_0c;
    struct garden_vms_chao *chao, *egg;
    Sint32 chao_returned, egg_received;
    Uint8 items[8];
    Sint32 item_count;
};
/* an item of garden_transfer.items from GARDEN_SEED_TYPES on is the fruit of
 * kind item + GARDEN_FRUIT_ITEM_OFFSET */
#define GARDEN_FRUIT_ITEM_OFFSET 3

extern struct chao_save garden_vms_racers[RACE_ENTRANTS];
/* start a card job that writes the transfer's Chao to its drive, or one
 * that reads them */
int garden_write_vms_chao(struct garden_transfer *transfer);
int garden_read_vms_chao(struct garden_transfer *transfer);
/* open the walk menu to send a resident to a VMU. the receive call opens it
 * to take in the Chao a VMU brings back */
int garden_open_vmu_send(struct garden_transfer *transfer);
int garden_open_vmu_receive(struct garden_transfer *transfer);
#endif
