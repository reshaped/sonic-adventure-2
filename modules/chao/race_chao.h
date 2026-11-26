/* the copies between a Chao's parameters and a track record's racer. the
 * upkeep tick that the Chao's behaviors take is here too */
#ifndef CHAO_RACE_CHAO_H
#define CHAO_RACE_CHAO_H
#include <sg_xpt.h>

struct task;
struct garden_race_chao;
struct chao_parameters;

/* where the Chao's upkeep timer is 0, start it and return TRUE. otherwise
 * return FALSE */
Bool chao_take_upkeep_tick(struct task *chao);
void chao_pack_racer(struct garden_race_chao *racer,
                     const struct chao_parameters *parameters);
void chao_unpack_racer(struct chao_parameters *parameters,
                       const struct garden_race_chao *racer);

#endif
