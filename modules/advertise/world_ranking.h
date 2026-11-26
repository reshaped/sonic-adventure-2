/* the world ranking's work, and its task, which does nothing. the work has
 * no textures */
#ifndef ADVERTISE_WORLD_RANKING_H
#define ADVERTISE_WORLD_RANKING_H
#include "advertise/menus.h"

/* the work, menu_works.work_18 */
extern struct world_ranking_work world_ranking_work;

/* create the task, worldRankingExec, with the work cleared. the task's
 * destructor clears loaded */
void world_ranking_create(void);
/* set loaded, or clear it. the work has no textures */
void world_ranking_load(void);
void world_ranking_release(void);

#endif
