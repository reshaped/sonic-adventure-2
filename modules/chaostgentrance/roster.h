/* the Chao the race entrance's picker offers. row 0 holds the Chao on each
 * drive's memory card, at the drive's index. a slot there is null where the
 * Chao can't race. row n holds garden n's Chao that can race, from index 0
 * on */
#ifndef CHAOSTGENTRANCE_ROSTER_H
#define CHAOSTGENTRANCE_ROSTER_H
#include <sg_xpt.h>
#include "chao/garden_save.h"
#include "chao/race_record.h"
#include "chao/transfer.h"
struct chao_save;
extern struct garden_transfer entrance_card_read;
/* what entrance_drive_kind holds for each drive since its last scan */
enum { ENTRANCE_DRIVE_EMPTY, ENTRANCE_DRIVE_CARD, ENTRANCE_DRIVE_MULTI_BANK };
extern Sint16 entrance_drive_kind[RACE_ENTRANTS];
/* entrance_roster's row of the memory cards' Chao */
#define ENTRANCE_CARD_ROW 0
extern struct chao_save *entrance_roster[CHAO_GARDENS + 1][RACE_ENTRANTS];
extern Sint32 entrance_roster_ready;
void entrance_sort_roster(void);
#endif
