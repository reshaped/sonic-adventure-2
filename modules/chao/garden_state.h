/* the Chao world's state across its areas. garden_previous, garden_current
 * and garden_next hold CHAO_AREA_ values */
#ifndef CHAO_GARDEN_STATE_H
#define CHAO_GARDEN_STATE_H
#include <sg_xpt.h>

struct task;

extern Bool garden_bad_file, garden_file_loaded, garden_needs_first_save;
extern Bool garden_save_on_exit;
extern int garden_previous, garden_current, garden_next;
extern struct task *garden_player_control;

#endif
