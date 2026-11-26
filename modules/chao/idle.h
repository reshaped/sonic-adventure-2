/* a garden Chao's idle behaviors */
#ifndef CHAO_IDLE_H
#define CHAO_IDLE_H
struct task;

/* stand a sitting or lying Chao up, and finish once it stands */
int chao_stand_up_behavior(struct task *chao);
/* a sitting or lying Chao sits idle, and any other stands idle. after 61 to
 * 181 steps, slowing on the floor, the Chao chooses what to do */
int chao_idle_behavior(struct task *chao);
/* chao_idle_look_behavior plays CHAO_ANIM_KYOROKYORO_UP or _DOWN, and
 * chao_idle_stand_behavior CHAO_ANIM_STAND, until the motion has ended 5 to 14
 * times. chao_idle_wait_behavior plays CHAO_ANIM_OSYABURI until it has ended
 * 10 to 19 times. each then finishes */
int chao_idle_look_behavior(struct task *chao);
int chao_idle_stand_behavior(struct task *chao);
int chao_idle_wait_behavior(struct task *chao);

#endif
