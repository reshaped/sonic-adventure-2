/* behaviors that take a Chao to its mover's target */
#ifndef CHAO_GAIT_H
#define CHAO_GAIT_H
struct task;
/* hop forward, and choose a new destination once it lands */
int chao_hop_forward(struct task *chao);
/* walk to a point of water */
int chao_seek_water(struct task *chao);
/* start the gait the Chao's run skill gives, as chao_start_skill_walk does,
 * toward its mover's target */
int chao_choose_gait(struct task *chao);
#endif
